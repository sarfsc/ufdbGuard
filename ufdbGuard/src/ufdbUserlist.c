/*
 * ufdbUserlist.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdbUserlist.c,v 1.4 2017/08/24 10:39:12 root Exp root $
 */


#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

extern pthread_rwlock_t  TheDynamicSourcesLock;



/* UFDBretrieveExecUserlist() uses a cache to support slow LDAP queries.
 * During a reload the execuserlist results are not deleted any more and an
 * unchanged command to retrieve a list of users is returned from the cache.
 * For the 15-minute refresh of a userlist, there is a new function: UFDBrefreshExecUserlist();
 */

typedef struct ulCacheElem {
   char *       command;
   struct sgDb  udb;
} ulCacheElem;

UFDB_GCC_ALIGN64
static unsigned     NulCache = 0;
static ulCacheElem  ulCache[UFDB_MAX_USERLISTS];

static pthread_mutex_t ul_mutex = UFDB_STATIC_MUTEX_INIT;


static void encodeCommand( char * command, char * encodedCommand, int maxlen )
{
   int flen;

   flen = 0;
   while (*command != '\0')
   {
      if (*command == ' ' || *command == '\t')
      {
         *encodedCommand++ = '_';
      }
      else if (*command == '/' || *command == '\\')
      {
         *encodedCommand++ = '+';
      }
      else if (*command == '#' || *command == '|')
      {
         *encodedCommand++ = '+';
         *encodedCommand++ = '-';
         flen++;
      }
      else if (*command == '\'' || *command == '"' || *command == '`')
      {
         *encodedCommand++ = '+';
         *encodedCommand++ = '+';
         flen++;
      }
      else if (*command == '[' || *command == ']' || *command == '{' || *command == '}')
      {
         *encodedCommand++ = '-';
         *encodedCommand++ = '+';
         flen++;
      }
      else if (*command == '$' || *command == '%' || *command == '!' || *command == '~' || 
               *command == '(' || *command == ')')
      {
         *encodedCommand++ = '_';
         *encodedCommand++ = '_';
         flen++;
      }
      else if (*command == '<' || *command == '>' || *command == '?')
      {
         *encodedCommand++ = '_';
         *encodedCommand++ = '+';
         flen++;
      }
      else
	 *encodedCommand++ = *command;
      command++;

      flen++;
      if (flen == maxlen-8  &&  strlen(command) > 3)
      {
         /* The maximum filename length is maxlen characters and we are about to go over it.
	  * So calculate a hash for the rest of the command and put the hash value in the encoded command.
	  */
	 unsigned long h;
	 h = (69313 * *command) ^ *(command+1);
	 while (*command != '\0')
	 {
	    h = (h << 5) + ((*command * 7) ^ (h >> 3));
	    command++;
	 }
	 sprintf( encodedCommand, ".%6lu", h % 1000000 );
	 return;
      }
   }
   *encodedCommand = '\0';
}


static FILE * openCacheForWrite( char * command )
{
   int    ret;
   FILE * fc;
   char * dbhome;
   char   fileName[2048];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;
   strcpy( fileName, dbhome );
   strcat( fileName, "/cache.execlists" );
   errno = 0;
   ret = mkdir( fileName, 0770 );
   if (ret != 0  &&  errno != EEXIST)
   {
      ufdbLogError( "cannot create cache directory \"%s\": %s", fileName, strerror(errno) );
      encodeCommand( command, fileName+strlen(fileName), 255 - sizeof("/cache.execlists") );
   }
   else
   {
      (void) chmod( fileName, 0770 );
      strcat( fileName, "/" );
      encodeCommand( command, fileName+strlen(fileName), 255 );
   }

   fc = fopen( fileName, "w" );
   if (fc == NULL)
      ufdbLogError( "cannot write to userlist cache file \"%s\": %s", fileName, strerror(errno) );
   else if (UFDBglobalDebug)
      ufdbLogMessage( "   writing to userlist cache file \"%s\"", fileName );

   return fc;
}


static FILE * openCacheForRead( char * command )
{
   FILE * fc;
   char * dbhome;
   struct stat stbuf;
   char   fileName[2048];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;
   strcpy( fileName, dbhome );
   strcat( fileName, "/cache.execlists" );
   if (stat( fileName, &stbuf ) == 0)
   {
      strcat( fileName, "/" );
      encodeCommand( command, fileName+strlen(fileName), 255 );
   }
   else
      encodeCommand( command, fileName+strlen(fileName), 255 - sizeof("/cache.execlists") );

   fc = fopen( fileName, "r" );
   if (fc != NULL)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "   reading from userlist cache file \"%s\"", fileName );
   }

   return fc;
}


static void readUserlistFromCache( FILE * fin, struct sgDb * db )
{
   char          line[10000];

   db->dbhome = NULL;
   db->dbcp = (void *) UFDBmemDBinit();
   db->type = SGDBTYPE_EXECUSERLIST;

   while (UFDBfgetsNoNL(line,sizeof(line),fin) != NULL)
   {
      UFDBmemDBinsert( (struct UFDBmemDB *) db->dbcp, line, NULL );
   }
}


static int execUserlistCommand( char * command, struct sgDb * db )
{
   int           ullineno;
   FILE *        fin;
   FILE *        fcache;
   char *        lc;
   time_t        tb, te;
   char          line[10000];

   if (UFDBglobalTerminating)
      return 0;

   /* make a new cache file for this command */
   fcache = openCacheForWrite( command );
   
   db->dbhome = NULL;
   db->dbcp = (void *) UFDBmemDBinit();
   db->type = SGDBTYPE_EXECUSERLIST;

   tb = time( NULL );
   errno = 0;
   if ((fin = popen(command,"r")) == NULL)
   {
      ufdbLogError( "can't execute command of execuserlist \"%s\": %s  *****", command, strerror(errno) );
      return 0;
   }
   ullineno = 0;

   while (UFDBfgetsNoNL(line,sizeof(line),fin) != NULL)
   {
      ullineno++;

      if (UFDBglobalDebug > 1  ||  UFDBglobalDebugExternalScripts)
         ufdbLogMessage( "   execuserlist: received \"%s\"", line );

      if (line[0] == '#')			/* skip comments */
         continue;

      if (line[0] == '\0')
      {
         ufdbLogError( "execuserlist \"%s\": line %d: line is empty", command, ullineno );
         continue;
      }

      for (lc = line;  *lc != '\0';  lc++)   /* convert username to lowercase chars */
      {
	 if (*lc <= 'Z'  &&  *lc >= 'A')
	    *lc += 'a' - 'A';
      }
      UFDBmemDBinsert( (struct UFDBmemDB *) db->dbcp, line, NULL );
      if (fcache != NULL)
	 fprintf( fcache, "%s\n", line );
   }
   (void) pclose( fin );
   if (fcache != NULL)
      fclose( fcache );
   te = time( NULL );

   db->entries = ullineno;
   ufdbLogMessage( "finished retrieving userlist (%d lines in %ld seconds) generated by \"%s\"", ullineno, te - tb, command );

   return 1;
}


struct sgDb * UFDBretrieveExecUserlist( char * command )
{
   unsigned      i;
   FILE *        fcache;

   for (i = 0;  i < NulCache;  i++)
   {
      if (strcmp( ulCache[i].command, command ) == 0)
         return &(ulCache[i].udb);
   }

   /* TO-DO: remove old userlist commands from the cache */
   /* TO-DO: but do this only if in the last 2 reloads the cache is not used */

   pthread_mutex_lock( &ul_mutex );
   i = NulCache;
   ulCache[i].command = ufdbStrdup( command );
   ulCache[i].udb.dbhome = NULL;
   NulCache++;
   pthread_mutex_unlock( &ul_mutex );

   if (NulCache == UFDB_MAX_USERLISTS)
      ufdbLogFatalError( "UFDBrefreshExecUserlist: maximum number of %d dynamic userlists is reached", UFDB_MAX_USERLISTS );

   /* See if we have the command cached in a file */
   fcache = openCacheForRead( command );
   if (fcache == NULL)
      execUserlistCommand( command, &(ulCache[i].udb) );
   else
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "   reading cached userlist from file; command is \"%s\"", command );
      readUserlistFromCache( fcache, &(ulCache[i].udb) );
      fclose( fcache );
   }

   return &(ulCache[i].udb);
}


void UFDBrefreshExecUserlist( char * command )
{
   unsigned     i;
   int          found;
   int          ret;
   struct UFDBmemDB * oldUserlist;
   struct sgDb  tmpDb;

   found = 0;
   pthread_mutex_lock( &ul_mutex );
   for (i = 0;  i < NulCache;  i++)
   {
      if (strcmp( ulCache[i].command, command ) == 0)
      {
         found = 1;
         break;
      }
   }
   pthread_mutex_unlock( &ul_mutex );

   if (found)
   {
      execUserlistCommand( command, &tmpDb );

      if (UFDBglobalDebugExternalScripts  ||  UFDBglobalDebug > 1)
         ufdbLogMessage( "UFDBrefreshExecUserlist: replacing userlist generated with command \"%s\"", command );

      ret = pthread_rwlock_wrlock( &TheDynamicSourcesLock );
      if (ret != 0)
	 ufdbLogError( "UFDBrefreshExecUserlist: pthread_rwlock_wrlock TheDynamicSourcesLock failed with code %d", ret );

      (void) pthread_mutex_lock( &ul_mutex );
      oldUserlist = ulCache[i].udb.dbcp;
      ulCache[i].udb.dbcp = tmpDb.dbcp;
      (void) pthread_mutex_unlock( &ul_mutex );

      ret = pthread_rwlock_unlock( &TheDynamicSourcesLock );
      if (ret != 0)
	 ufdbLogError( "UFDBrefreshExecUserlist: pthread_rwlock_unlock TheDynamicSourcesLock failed with code %d", ret );

      UFDBmemDBdeleteDB( oldUserlist );
   }
   else
   {
      ufdbLogError( "UFDBrefreshExecUserlist: could not find command in the cache: \"%s\"  *****", command );

      pthread_mutex_lock( &ul_mutex );
      i = NulCache;
      ulCache[i].command = ufdbStrdup( command );
      ulCache[i].udb.dbhome = NULL;
      NulCache++;
      pthread_mutex_unlock( &ul_mutex );

      if (NulCache == UFDB_MAX_USERLISTS)
         ufdbLogFatalError( "UFDBrefreshExecUserlist: maximum number of %d dynamic userlists is reached", UFDB_MAX_USERLISTS );

      execUserlistCommand( command, &tmpDb );

      (void) pthread_mutex_lock( &ul_mutex );
      ulCache[i].udb.dbcp = tmpDb.dbcp;
      (void) pthread_mutex_unlock( &ul_mutex );
   }
}

