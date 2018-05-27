/*
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * squidGuard is copyrighted (C) 1998 by
 * ElTele Øst AS, Oslo, Norway, with all rights reserved.
 *
 * $Id: sgLog.c,v 1.120 2018/03/18 18:55:34 root Exp root $
 */


/* This module is well tested and stable for a long time AND
 * needs maximum performance, so we remove _FORTIFY_SOURCE.
 */
#undef _FORTIFY_SOURCE

#include "ufdb.h"
#include "ufdblib.h"
#include "sg.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>

extern int    globalFatalError; 
extern int    globalPid;      		/* from main.c */
extern char * UFDBglobalLogDir;   
extern FILE * globalErrorLog;
extern int    globalDebugTimeDelta;

UFDB_GCC_ALIGN64
static pthread_mutex_t log_mutex = UFDB_STATIC_MUTEX_INIT;
static int    globalLogging = 1;

static int    rotationError = 0;
static int    UFDBforceLogRotation = 0;

static unsigned long    logfilesize = 0;
static ufdb_crashfun_t  crashfun = (ufdb_crashfun_t) NULL;

static char   UFDBlogFilename[1024];



/* Rotate log files.
 * NOTE: There is a race condition when multiple instances try to rotate.
 *       This may happen with ufdbgclient (N processes vs. N threads).
 */
static void RotateLogfile( 
   char *      filename )
{
   int         i;
   char        oldfile[1024];
   char        newfile[1024];

   /* rotate the log file:
    * file.log.7  ->  file.log.8
    * file.log.6  ->  file.log.7
    * file.log.5  ->  file.log.6
    * file.log.4  ->  file.log.5
    * file.log.3  ->  file.log.4
    * file.log.2  ->  file.log.3
    * file.log.1  ->  file.log.2
    * file.log    ->  file.log.1
    */

   for (i = 8; i > 1; i--)
   {
      sprintf( newfile, "%s.%d", filename, i );
      sprintf( oldfile, "%s.%d", filename, i-1 );
      rename( oldfile, newfile );
   }
   sync();	/* sync() may resolve some borderline disk space issues */

   sprintf( newfile, "%s.%d", filename, 1 );
   if (rename( filename, newfile ) != 0)
   {
      /* OOPS, rename did not work. maybe no space or a permission problem
       * we need to do damage control: remove the original file.
       */
      if (unlink( filename ) != 0)
      {
         /* it is getting worse... */
	 if (truncate( filename, 0 ))
	    ;
      }
      if (++rotationError < 5)
      {
	 syslog( LOG_ALERT, "%s cannot rotate its log files !!  Check permissions and file system space.  *****", UFDBprogname );
      }
   }
}


UFDB_GCC_HOT static void ufdbLog( 
   char *    msg  )
{
   char *    nl;
   int       multiline;
   char      date[22];
   unsigned long  logmsglen;
   char      logmsg[UFDB_MAX_URL_LENGTH+2048+2048];
#ifdef UFDB_DEBUG_LOGGING
   int       newlog = 0;
#endif

#if 0
   if (!globalLogging)
      return;
#endif

   niso( 0, date );

   multiline = 0;
   logmsg[0] = '\0';
   logmsglen = 0;
   while ((nl = strchr( msg, '\n' )) != NULL)
   {
      *nl = '\0';
      logmsglen += (unsigned long) snprintf( logmsg+logmsglen, sizeof(logmsg)-logmsglen-30, 
                                             "%s [%d] %s%s\n", date, globalPid, multiline?"   ":"", msg );
      msg = nl + 1;
      multiline = 1;
      if (logmsglen > sizeof(logmsg)-2048)
      {
	 logmsglen += (unsigned long) snprintf( logmsg+logmsglen, sizeof(logmsg)-logmsglen-30, 
                                                "%s [%d] ... ... ...\n", date, globalPid );
         goto msg_too_long;
      }
   }

   if (*msg != '\0')
      logmsglen += (unsigned long) snprintf( logmsg+logmsglen, sizeof(logmsg)-logmsglen-30, 
                                             "%s [%d] %s%s\n", date, globalPid, multiline?"   ":"", msg );

msg_too_long:
   pthread_mutex_lock( &log_mutex );			/* LOG_MUTEX *******************/

   if (UFDBforceLogRotation)
   {
      UFDBforceLogRotation = 0;
      if (globalErrorLog != NULL)  
      {
	 RotateLogfile( UFDBlogFilename );
	 ufdbSetGlobalErrorLogFile( 1 );
#ifdef UFDB_DEBUG_LOGGING
         newlog = 1;
#endif
      }
   }

   if (globalErrorLog == NULL) 
   {
      fputs( logmsg, stderr );
      fflush( stderr );
   }
   else
   {
      if (logfilesize == 0)
	 logfilesize = ftell( globalErrorLog );

      /* write is *much* faster than fputs */
      if (write( fileno(globalErrorLog), logmsg, (size_t) logmsglen ) > 0)
	 logfilesize += logmsglen;

      if (logfilesize > UFDBglobalMaxLogfileSize)
      {
	 RotateLogfile( UFDBlogFilename );
	 ufdbSetGlobalErrorLogFile( 1 );
#ifdef UFDB_DEBUG_LOGGING
         newlog = 1;
#endif
	 logfilesize = 1;
      }
   }

   pthread_mutex_unlock( &log_mutex );			/* LOG_MUTEX *******************/
}


void ufdbGlobalSetLogging( int logging )
{
   globalLogging = logging;
}


void UFDBrotateLogfile( void )
{
   UFDBforceLogRotation = 1;
}


void ufdbSetGlobalErrorLogFile( int mutex_is_used )
{
   struct stat s;
   int         retval;

   if (!globalLogging)
      return;

   if (globalErrorLog == stderr)
      return;

   if (UFDBglobalLogDir == NULL)
      strcpy( UFDBlogFilename, DEFAULT_LOGDIR );
   else
      strcpy( UFDBlogFilename, UFDBglobalLogDir );
   strcat( UFDBlogFilename, "/" );

   if (UFDBprogname != NULL  &&  UFDBprogname[0] != '\0')
   {
      strcat( UFDBlogFilename, UFDBprogname );
      strcat( UFDBlogFilename, ".log" );
   }
   else
      strcat( UFDBlogFilename, DEFAULT_LOGFILE );

   retval = stat( UFDBlogFilename, &s );
   if (retval == 0)
   {
      if (s.st_size > (off_t) UFDBglobalMaxLogfileSize)
	 RotateLogfile( UFDBlogFilename );
   }

   if (!mutex_is_used)
      pthread_mutex_lock( &log_mutex );            /* critical section */

   if (globalErrorLog != NULL)
      fclose( globalErrorLog );

   globalErrorLog = fopen( UFDBlogFilename, "a" );
   if (globalErrorLog == NULL)
   {
      fprintf( stderr, "%s: cannot write to logfile %s: %s (uid=%d,euid=%d)\n",
      	 	       UFDBprogname, UFDBlogFilename, strerror(errno), getuid(), geteuid() );
      if (retval == 0)
      {
         fprintf( stderr, "%s: the logfile exists, the owner has uid %d and the file mode is %04o\n",
	                  UFDBprogname, s.st_uid, s.st_mode );
      }
      else
      {
         fprintf( stderr, "%s: the logfile does not exist and I cannot create it\n",
	                  UFDBprogname );
      }

      /*
       * We insist in having a logfile so try an other directory...
       */
      strcpy( UFDBlogFilename, "/tmp/" );
      strcat( UFDBlogFilename, UFDBprogname );
      strcat( UFDBlogFilename, ".log" );
      globalErrorLog = fopen( UFDBlogFilename, "a" );
      if (globalErrorLog == NULL)
	 fprintf( stderr, "%s: cannot write to logfile %s: %s\n", UFDBprogname, UFDBlogFilename, strerror(errno) );
   }

   if (globalErrorLog == NULL)
      logfilesize = 1;
   else
   {
      fprintf( globalErrorLog, "\n"
                               "The ufdbGuard (" UFDB_VERSION ") software suite is free and Open Source Software.\n"
                               "Copyright (C) 2005-2018 by URLfilterDB B.V. and others.\n"
			       "\n" );
      fflush( globalErrorLog );
      logfilesize = ftell( globalErrorLog );
#if 0
      if (fcntl( fileno(globalErrorLog), F_SETFD, FD_CLOEXEC ))
         ;
#endif
   }

   if (!mutex_is_used)
      pthread_mutex_unlock( &log_mutex );          /* end critical section */
}


UFDB_GCC_HOT void niso( 
   time_t    t, 
   char *    buf )
{
   struct tm lc;

   if (t == 0)
      t = time(NULL) + globalDebugTimeDelta;
   localtime_r( &t, &lc );
   sprintf( buf, "%04d-%02d-%02d %02d:%02d:%02d", 
            lc.tm_year + 1900, lc.tm_mon + 1, lc.tm_mday, lc.tm_hour, lc.tm_min, lc.tm_sec );
}


UFDB_GCC_HOT 
void ufdbLogError( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];

   if (!globalLogging)
      return;

   strcpy( msg, "ERROR: " );

   va_start( ap, format );
   if (vsnprintf( msg+7, UFDB_MAX_URL_LENGTH-32-7, format, ap ) > UFDB_MAX_URL_LENGTH-7-34)
   {
      msg[UFDB_MAX_URL_LENGTH-34] = ' ';
      msg[UFDB_MAX_URL_LENGTH-33] = '.';
      msg[UFDB_MAX_URL_LENGTH-32] = '.';
      msg[UFDB_MAX_URL_LENGTH-31] = '.';
      msg[UFDB_MAX_URL_LENGTH-30] = '\0';
   }
   va_end( ap );

   ufdbLog( msg );
}


UFDB_GCC_HOT 
void ufdbLogMessage( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];

   if (!globalLogging)
      return;

   va_start( ap, format );
   if (vsnprintf( msg, UFDB_MAX_URL_LENGTH-32, format, ap ) > UFDB_MAX_URL_LENGTH-34)
   {
      msg[UFDB_MAX_URL_LENGTH-34] = ' ';
      msg[UFDB_MAX_URL_LENGTH-33] = '.';
      msg[UFDB_MAX_URL_LENGTH-32] = '.';
      msg[UFDB_MAX_URL_LENGTH-31] = '.';
      msg[UFDB_MAX_URL_LENGTH-30] = '\0';
   }
   va_end( ap );

   ufdbLog( msg );
}


UFDB_GCC_HOT 
void ufdbLogFatalError( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];
   char    logmsg[UFDB_MAX_URL_LENGTH];

   va_start( ap, format );
   if (vsnprintf( msg, UFDB_MAX_URL_LENGTH-64, format, ap ) > UFDB_MAX_URL_LENGTH-64-2)
   {
      msg[UFDB_MAX_URL_LENGTH-64] = ' ';
      msg[UFDB_MAX_URL_LENGTH-63] = '.';
      msg[UFDB_MAX_URL_LENGTH-62] = '.';
      msg[UFDB_MAX_URL_LENGTH-61] = '.';
      msg[UFDB_MAX_URL_LENGTH-60] = '\0';
   }
   va_end( ap );

   sprintf( logmsg, "\nFATAL ERROR: %s  *****\n ", msg );
   ufdbLog( logmsg );

   if (UFDBglobalDebug > 1)
   {
      /* only print to tty */
      if (fileno(stderr) >= 0  &&  isatty(fileno(stderr)))
         fprintf( stderr, "%s\n", logmsg );
   }

   UFDBglobalFatalError = 1;

   if (UFDBglobalCrashOnFatal  &&  crashfun != (ufdb_crashfun_t) NULL)
      (crashfun)();
}


void ufdbRegisterFatalErrorCrashfun( ufdb_crashfun_t crfun )
{
   crashfun = crfun;
}


void UFDBcloseFilesNoLog( void )
{
   int i;
   int logfile;

   if (UFDBglobalDebug)
      ufdbLogMessage( "UFDBcloseFilesNoLog: closing all files" );

   if (globalErrorLog == NULL)
      logfile = fileno( stderr );
   else
      logfile = fileno( globalErrorLog );

   for (i = 0;  i < 2*UFDB_MAX_THREADS + 32;  i++)
   {
      if (i != logfile)
	 (void) close( i );
   }
}

