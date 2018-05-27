/* 
 * ufdbsignal.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 * 
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * send a signal to ufdbguardd/ufdbhttpd/icapd
 *
 * $Id: ufdbsignal.c,v 1.13 2016/08/22 13:56:05 root Exp root $
 */

#define GNU_SOURCE

#include "ufdb.h"
#include "ufdblib.h"
#include "httpserver.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>


static int bequiet = 0;


static void usage( char option )
{
   fprintf( stderr, "ufdbsignal: unknown option(s)\n\n" );
   fprintf( stderr, "usage: ufdbsignal [-v] [-h] [-q] -C 'signal procname'\n" );
   fprintf( stderr, "where signal is sigterm|sighup|sigkill and procname is ufdbguardd|ufdbhtppd|icapd\n" );
   fprintf( stderr, "  -h : this help text\n" );
   fprintf( stderr, "  -v : print version and exit\n" );
   fprintf( stderr, "  -q : be quiet\n" );
   exit( 1 );
}



static void catchChildSignal( int signal )
{
   int        status;
   pid_t      pid;

   while ((pid = waitpid( -1, &status, WNOHANG )) > 0)
      ;
}


pid_t readpidfromfile( char * filename )
{
   FILE * fp;
   pid_t  pid;

   pid = -1;
   fp = fopen( filename, "r" );
   if (fp != NULL)
   {
      if (1 != fscanf( fp, "%d", &pid ))
      {
	 ufdbLogError( "pid file %s does not have a process id", filename );
	 fprintf( stderr, "pid file %s does not have a process id\n", filename );
      }
      else if (UFDBglobalDebug)
         ufdbLogMessage( "read pid %d from file %s", pid, filename );

      fclose( fp );
   }
   else
   {
      ufdbLogError( "cannot open pid file %s: %s", filename, strerror(errno) );
      fprintf( stderr, "cannot open pid file %s: %s\n", filename, strerror(errno) );
   }
   return pid;
}


int sendsignal( pid_t pid, char * signame )
{
   int   signo;
   int   retval;
   uid_t uid;
   struct passwd * pw;

   if (strcmp( signame, "sighup" ) == 0)
      signo = SIGHUP;
   else if (strcmp( signame, "sigterm" ) == 0)
      signo = SIGTERM;
   else if (strcmp( signame, "sigusr1" ) == 0)
      signo = SIGUSR1;
   else if (strcmp( signame, "sigusr2" ) == 0)
      signo = SIGUSR2;
   else if (strcmp( signame, "sigkill" ) == 0)
      signo = SIGKILL;
   else
   {
      ufdbLogFatalError( "unknown or unsupported signal \"%s\"", signame );
      return 0;
   }

#if defined(__linux__) && 0
   {
      int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid); 
      uid_t ruid, euid, suid;

      getresuid( &ruid, &euid, &suid );
      printf( "ruid=%d euid=%d suid=%d\n", ruid, euid, suid );
   }
#endif

   uid = getuid();
   if (uid != 0)	/* user "root" (uid 0) does not need further checks */
   {
      int ok = 0;

      /* check that we are "squid" or UFDB_USER (defaults to "ufdb") */
      pw = getpwnam( UFDB_USER );
      if (pw != NULL  &&  pw->pw_uid == uid)
         ok = 1;
      else
      {
	 pw = getpwnam( "squid" );
	 if (pw != NULL  &&  pw->pw_uid == uid)
	    ok = 1;
      }

      if (!ok)
      {
	 ufdbLogError( "I have user id %d and I am NOT authorised to use ufdbsignal", uid );
	 return 0;
      }
   }
   ufdbLogMessage( "I have user id %d and I am authorised to use ufdbsignal. Sending signal %d to pid %d.",
   		   uid, signo, pid );

   errno = 0;
   retval = kill( pid, signo );
   if (retval < 0)
   {
      ufdbLogError( "could not send signal %s to process %d: %s", signame, pid, strerror(errno) );
      return 0;
   }
   else
   {
      /* I will sleep a little while since 
       * 1) the receiving process needs to get an opportunity to act upon the signal
       * 2) the callers of ufdbsignal need that the receiving process acts as quickly as possible. 
       *    This is partly accomplished by releasing the CPU.
       */
       struct timespec t;
       t.tv_sec = 0;
       t.tv_nsec = 100 * 1000000L;            /* 100 ms  or  0.1 seconds */
       nanosleep( &t, NULL );
   }

   return 1;
}


int doCommand( char * commandline )
{
   int    nkw;
   pid_t  pid;
   char * pidfile;
   char   command[80];
   char   arg1[256];
   char   arg2[256];

   ufdbSetSignalHandler( SIGCHLD, catchChildSignal );		/* TODO: remove */

   if (UFDBglobalDebug)
      ufdbLogMessage( "doCommand \"%s\"", commandline );

   nkw = sscanf( commandline, "%s %s %s", command, arg1, arg2 );
   if (nkw < 2 )
   {
      ufdbLogFatalError( "bad command line: %s", commandline );
      return 0;
   }

   if (strncmp( command, "sig", 3 ) == 0)
   {
      if (strcmp( arg1, "ufdbhttpd" ) == 0)
         pidfile = UFDBHTTPD_PID_FILE;			/* TODO: make the piddir a command line option */
      else if (strcmp( arg1, "ufdbguardd" ) == 0)
         pidfile = UFDBGUARDD_PID_FILE;
      else if (strcmp( arg1, "icapd" ) == 0)
         pidfile = UFDBICAP_PID_FILE;
      else
      {
         ufdbLogFatalError( "bad argument for %s command: %s", arg1, command );
	 return 0;
      }

      pid = readpidfromfile( pidfile );
      if (pid < 0)
      {
	 if (!bequiet)
	    fprintf( stderr, "ufdbsignal: failed sending signal to %s\n", arg1 );
	 ufdbLogError( "failed sending signal to %s since the pid could not be read from %s", arg1, pidfile );
	 (void) unlink( pidfile );
         return 0;
      }

      if (!sendsignal( pid, command ))
      {
	 if (!bequiet)
	    fprintf( stderr, "ufdbsignal: failed sending signal %s to %s (pid %ld)\n", command, arg1, (long) pid );
	 ufdbLogError( "failed sending signal to %s", arg1 );
	 if (errno == ESRCH)  /* kill failed with ESRCH means that the process does not exit */
	 {
	    if (!bequiet)
	       fprintf( stderr, "ufdbsignal: process %d does not exist; removing pid file %s\n", pid, pidfile );
	    ufdbLogMessage( "ufdbsignal: process %d does not exist; removing pid file %s\n", pid, pidfile );
	    (void) unlink( pidfile );
	 }
         return 0;
      }
      else
      {
	 ufdbLogMessage( "sent signal %s to %s (pid=%d)", command, arg1, pid );
	 if (!bequiet)
	    fprintf( stderr, "sent signal %s to %s (pid=%d)\n", command, arg1, pid );
	 return 1;
      }
   }

   ufdbLogFatalError( "bad command: %s", command );
   fprintf( stderr, "bad command: %s\n", command );
   return 0;
}


int main( 
   int            argc,
   char **        argv )
{
   int            i;

   globalPid = getpid();
   globalErrorLog = NULL;
   strcpy( UFDBprogname, "ufdbsignal" );
   UFDBglobalLogDir = DEFAULT_LOGDIR;
   ufdbSetGlobalErrorLogFile( 0 );

   while ((i = getopt(argc, argv, "C:dhqv")) != EOF)
   {
      switch (i) {
      case 'C':
	 ufdbLogMessage( "-C \"%s\"", optarg );
	 if (doCommand( optarg ))
	    exit( 0 );
	 exit( 1 );
         break;
      case 'v':
         printf( "ufdbsignal " UFDB_VERSION "\n" );
	 exit( 0 );
	 break;
      case 'd':
	 UFDBglobalDebug = 1;
         break;
      case '?':
      case 'h':
         usage( '\0' );
	 break;
      case 'q':
      	 bequiet = 1;
	 break;
      default:
         usage( i );
      }
   }

   usage( '\0' );
   return 1;
}

