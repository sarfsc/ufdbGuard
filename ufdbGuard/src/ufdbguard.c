/*
 * ufdbguard,c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * squidGuard is copyrighted (C) 1998 by
 * ElTele Øst AS, Oslo, Norway, with all rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as
 * published by the Free Software Foundation.  It is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License (GPL) for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * (GPL) along with this program.
 */

#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <sys/time.h>

/* use linebuffer output: flush() is not needed any more */
#if 0
#define FLUSH(f)   fflush(f)
#else
#define FLUSH(f)   /* */
#endif


static int parseOnly = 0;
static int noRedirects = 0;


static void usage( void )
{
  fprintf( stderr, 
	   "Usage: ufdbGuard [-C block] [-t time] [-c file] [-v] [-d]\n" );
  fprintf( stderr, "Options:\n" );
  fprintf( stderr, "  -v          : show version number\n" );
  fprintf( stderr, "  -d          : all errors to stderr\n" );
  fprintf( stderr, "  -c file     : load alternate configfile\n" );
  fprintf( stderr, "  -t time     : specify startuptime on format: yyyy-mm-ddTHH:MM:SS\n" );
#if 0
  fprintf( stderr, "  -u          : update .ufdb files from .diff files\n" );
  fprintf( stderr, "  -C file|all : create new .db files from urls/domain files\n" );
  fprintf( stderr, "                specified in \"file\".\n" );
#endif
  fprintf( stderr, "  -T          : test mode: log unwanted URLs but do not block them\n" );
  fprintf( stderr, "  -r          : log the redirections\n" );

  exit( 1 );
}

void pass( struct SquidInfo *si )
{
  if (UFDBglobalLogAllRequests)
  {
    const char * category;
    if (squidInfo.aclpass == NULL)
	category = "unknown";
    else if (squidInfo.aclpass->name == NULL)
	category = "unknown";
    else
	category = squidInfo.aclpass->name;

    if (squidInfo.ident[0] == '\0')
    {
	squidInfo.ident[0] = '-';
	squidInfo.ident[1] = '\0';
    }
    ufdbLogMessage( "PASS  %-8s %-15s %-10s %-9s %s",
		    squidInfo.ident, squidInfo.srcIP, acl->name, category,
		    squidInfo.url2log );
  }

  putchar( '\n' );  	/* OK */
}

void block( struct SquidInfo *si )
{
  /* src found, so block the URL and send the redirect string */
  if (si->srcDomain[0] == '\0') {
    si->srcDomain[0] = '-';
    si->srcDomain[1] = '\0';
  }

  if (si->ident[0] == '\0') {
    si->ident[0] = '-';
    si->ident[1] = '\0';
  }

  if (UFDBglobalDebugRedirect)
  {
    ufdbLogMessage( "REDIRECT %s %s/%s %s %s %s\n", redirect, si->srcIP,
		    si->srcDomain, si->ident, "GET", si->urlgroup );
  }

  if (noRedirects)
    putchar( '\n' );  	/* fake OK */
  else
  {
    char urlgrp[64];
    const char * category;

    if (si->aclpass == NULL)
      category = "unknown";
    else if (si->aclpass->name == NULL)
      category = "unknown";
    else
      category = si->aclpass->name;

    if (si->urlgroup[0] == '#')
      urlgrp[0] = '\0';
    else
    {
      urlgrp[0] = ' ';
      trncpy( &urlgrp[1], si->urlgroup, 62 );
      urlgrp[63] = '\0';
    }
    printf( "%s %s/%s %s %s%s\n", redirect, si->srcIP,
	    si->srcDomain, si->ident, "GET", urlgrp );

    if (UFDBglobalLogAllRequests)
    {
      ufdbLogMessage( "BLOCK %-8s %-15s %-10s %-9s %s",
		      si->ident, si->srcIP, acl->name, category,
		      si->url2log );
    }
  }
}

int ufdbguard_main( int argc, char ** argv, char ** envp )
{
  int    ch;
  long   numreq = 0;
  struct SquidInfo squidInfo;
  struct Source * src;
  struct Acl * acl;
  struct timeval start_time, ready_time, stop_time;
  char * redirect;
  char * configFile = NULL;
  time_t t;
  char * vbuf;
  struct tms timer;
  UFDBthreadAdmin * admin;
  char   niso_str[22];
  char   buf[UFDB_MAX_URL_LENGTH];
  char   tmp[UFDB_MAX_URL_LENGTH];

  strcpy( UFDBprogname, "ufdbGuard" );

  UFDBtimerInit( &timer );
  admin = UFDBallocThreadAdmin();

  /* instead of using fflush(), set linebuffer mode for stdout */
  vbuf = (char *) ufdbMalloc( 4096 );
  (void) setvbuf( stdout, vbuf, _IOLBF, 4096 );

  gettimeofday( &start_time, NULL );
  globalPid = getpid();
  while ((ch = getopt(argc, argv, "Phdrt:c:vCu")) > 0)
  {
    switch (ch) {
    case 'd':
      UFDBglobalDebug = 1;
      break;
    case 'r':
      UFDBglobalDebugRedirect = 1;
      break;
    case 'c':
      configFile = optarg;
      break;
    case 'C':
    case 'u':		
      fprintf( stderr, "ufdbGuard: option '-%c' is not implemented. Use ufdbGenTable instead.\n", ch );
      exit( 1 );
      break;
    case 'v':
      fprintf( stderr, "ufdbGuard: %s\n", UFDB_VERSION );
      exit( 0 );
      break;
    case 't':
      if ((t = iso2sec(optarg)) == -1) {
	fprintf( stderr, "-t dateformat error, should be yyyy-mm-ddTHH:MM:SS\n" );
	exit(0);
      }
      if (t < 0) {
	fprintf( stderr, "-t date have to after 1970-01-01T01:00:00\n" );
	exit(0);
      }
      niso( t, niso_str );
      ufdbLogMessage( "ufdbGuard emulating date %s", niso_str );
      globalDebugTimeDelta = t - start_time.tv_sec;
      start_time.tv_sec = start_time.tv_sec + globalDebugTimeDelta;
      break;
    case 'T':
      ufdbLogMessage( "-T option found.  Going into test mode." );
      noRedirects = 1;		/* Test mode means noRedirects */
      break;
    case 'P':			/* undocumented -P option for development purposes */
      parseOnly = 1;
      break;
    case '?':
    case 'h':
    default:
      usage();
    }
  }

  globalArgv = argv;
  globalEnvp = envp;
  ufdbSetGlobalErrorLogFile( 0 );
  if (sgReadConfig( configFile ) == 0)
  {
     ufdbLogFatalError( "missing configuration file" );
     exit( 3 );
  }
  /* ufdbSetGlobalErrorLogFile( 0 ); */
  ufdbLogMessage( "ufdbGuard " UFDB_VERSION " started at %ld.%03ld", 
                  (long) start_time.tv_sec, (long) start_time.tv_usec/1000 );
  if (UFDBglobalDebug)
     ufdbLogMessage( "debug output is enabled." );

  ufdbSetThreadCPUaffinity( 0 );
  UFDBinitHTTPSchecker();

  if (globalUpdate || globalCreateDb != NULL)
  {
    ufdbLogMessage( "db update done" );
    gettimeofday( &stop_time, NULL );
    stop_time.tv_sec = stop_time.tv_sec + globalDebugTimeDelta;
    ufdbLogMessage( "ufdbGuard stopped at %ld.%03ld",
	            (long) stop_time.tv_sec, (long) stop_time.tv_usec/1000 );
    exit( 0 );
  }

  ufdbHandleAlarmForTimeEvents( UFDB_PARAM_INIT );

  ufdbSetSignalHandler( SIGHUP,  sgHandlerSigHUP );
  ufdbSetSignalHandler( SIGPIPE, SIG_IGN );

  ufdbSetSignalHandler( SIGINT,  sgSignalHandler );
  ufdbSetSignalHandler( SIGTERM, sgSignalHandler );
#if 0
  ufdbSetSignalHandler( SIGQUIT, sgSignalHandler );
  ufdbSetSignalHandler( SIGILL,  sgSignalHandler );
  ufdbSetSignalHandler( SIGSEGV, sgSignalHandler );
#endif

  UFDBlogConfig();

  if (UFDBglobalDenyMode)
    ufdbLogMessage( "ufdbGuard started in DENY mode.");
  else
    ufdbLogMessage( "ufdbGuard started in standard ALLOW mode.");

  gettimeofday( &ready_time, NULL );
  ready_time.tv_sec = ready_time.tv_sec + globalDebugTimeDelta;
  ufdbLogMessage( "ufdbGuard ready for requests at %ld.%03ld",
	        (long) ready_time.tv_sec, (long) ready_time.tv_usec/1000 );

#if 1
  UFDBtimerStop( &timer );
  UFDBtimerPrint( &timer, "time to initialize" );
  UFDBtimerInit( &timer );
#endif

  UFDBglobalReconfig = 0;

  while (1)
  {
    while (fgets(buf, UFDB_MAX_URL_LENGTH, stdin) != NULL)	   /********** MAIN LOOP ************/
    {
      buf[UFDB_MAX_URL_LENGTH-1] = '\0';   		/* force null termination */
      if (sig_hup)
      {
	sgReloadConfig();
	sig_hup = 0;
      }
      if (sig_other)
        break;
      numreq++;

      if (UFDBglobalFatalError || failsafe_mode) {
	putchar( '\n' );		/* OK */
	FLUSH( stdout );
	continue;
      }

#if 0
      fprintf( stderr, ">>> %s\n", buf );
#endif

      if (parseLine(admin, buf, &squidInfo) != 1) 	/* put input line in struct squidInfo */
      {
	ufdbLogError( "error parsing input line: %s", buf );
	putchar( '\n' ); 		/* OK */
      }
      else
      {
	for (src = (struct Source *) SourceList; ; )
	{
	  int access;

	  src = UFDBfindSource( src, &squidInfo );
	  acl = UFDBfindACLbySource( src, &squidInfo );
	  if (parseOnly)
	  {
	    putchar( '\n' );		/* OK */
	    break;
	  }
	  redirect = tmp;
	  access = UFDBdecideAccessBasedOnURL( 0, acl, &squidInfo, redirect );
	  if (access == UFDB_ACL_ACCESS_DUNNO  ||  access == UFDB_ACL_ACCESS_ALLOW)
	  {
	    /* src not found */
	    if (src == NULL || src->cont_search == 0) 
	    {
		pass (&squidInfo);
		break;
	    }
	    else
	      if (src->next != NULL) {
		src = src->next;
		continue;
	      }
              else
              {
		pass (&squidInfo);
		break;
	      }
	  }
	  else
	  {
	    block (&squidInfo);
	    break;
	  }
	} /*for (;;)*/
        UFDBfreeRevURL( admin, squidInfo.revUrl );
      }
      FLUSH( stdout );

      if (sig_hup)
      {
        sig_hup = 0;
        sgReloadConfig();
      }
      if (sig_other)
        break;
    } /* while fgets() */

    FLUSH( stdout );

    if (sig_other)
    {
      ufdbLogMessage( "signal %d received. exiting...", sig_other );
      exit( 2 );
    }

#if 1
    UFDBtimerStop( &timer );
    ufdbLogMessage( "processed %ld requests", numreq );
    UFDBtimerPrint( &timer, "time used to handle requests" );
#endif

    if (feof(stdin))
      ufdbLogMessage( "EOF on input stream" );

    fclose( stdin );
    fclose( stdout );
    ufdbFree( vbuf );
    ufdbFree( admin );
#if defined(UFDB_FREE_MEMORY)
    ufdbFreeAllMemory();
#endif

#if !HAVE_SIGACTION
#if HAVE_SIGNAL
    if (errno != EINTR) 
    {
      gettimeofday( &stop_time, NULL );
      stop_time.tv_sec = stop_time.tv_sec + globalDebugTimeDelta;
      ufdbLogMessage( "ufdbGuard stopped at %d.%03d",
	            stop_time.tv_sec, stop_time.tv_usec/1000 );
      exit(2);
    }
#endif
#else 
    gettimeofday( &stop_time, NULL );
    stop_time.tv_sec = stop_time.tv_sec + globalDebugTimeDelta;
    ufdbLogMessage( "ufdbGuard stopped at %ld.%03ld",
	          (long) stop_time.tv_sec, (long) stop_time.tv_usec/1000 );
    exit(0);
#endif
  } /* while (1) */

  /*NOTREACHED*/
  exit( 0 );
}


void sgReloadConfig( void )
{
  sig_hup = 0;
  ufdbLogMessage( "got sigHUP reload config" );

  /* closing global logfile */
  fclose( globalErrorLog );
  globalErrorLog = NULL;

  execve( *globalArgv, globalArgv, globalEnvp );
  fprintf( stderr, "error execve: %d\n", errno );
}


static char * my_fast_fgets_nonl( char * s, int size, FILE * stream )
{
   char * buf;
   int    ch;

   buf = s;
   while ((ch = getc_unlocked(stream)) != EOF  &&  --size > 0)
   {
      if (ch == '\n'  ||  ch == '\r')
         break;
      *buf++ = ch;
   }
   *buf = '\0';
   if (ch == EOF  &&  buf == s)
      return NULL;
   return s;
}


void execute_pstack( char * reason )
{
   if (strcmp( UFDB_GDB_PATH, "no" ) == 0)
   {
      ufdbLogMessage( "ufdb-pstack is not called since the gdb package was not installed." );
      ufdbLogMessage( "It is highly recommended to install gdb to produce a crash report.  *****" );
      ufdbLogMessage( "Crash reports are essential to be able to resolve bugs quickly." );
   }
   else
   {
      FILE * fp;
      char   cmd[512];
      char   buffer[2048];

#if 0
      ufdbSetSignalHandler( SIGALRM, interruptPstack );
      alarm( 6 );
#endif

      sprintf( cmd, DEFAULT_BINDIR "/ufdb-pstack %d %s 2>&1", getpid(), reason );

      /* gdb sometimes needs root permissions, so the popen() is done as root */
      UFDBraisePrivileges( UFDBglobalUserName, "open pipe for ufdb-pstack" );
      errno = 0;
      fp = popen( cmd, "r" );
      if (fp == NULL)
         ufdbLogError( "could not open pipe and execute \"%s\": %s", cmd, strerror(errno) );

      while (my_fast_fgets_nonl( buffer, 2040, fp ) != NULL)
      {
         ufdbLogMessage( "pstack  %s", buffer );
      }
      pclose( fp );
      ufdbLogMessage( "pstack  END" );
      UFDBdropPrivileges( UFDBglobalUserName );
   }
}


void BadSignalCaught( int signum )	
{
   char      me[48];
   
   /* find out which thread has a signal */
   sprintf( me, "pid %d", (int) getpid() );
   ufdbLogMessage( "%s caught bad signal %d.", me, signum );

   execute_pstack( me );
}


void __fortify_fail( const char * msg )
{
   UFDBglobalReconfig = 1;
   UFDBglobalTerminating = 1;
   (void) alarm( 0 );

   ufdbLogMessage( "fortified code failed: %s.  simulating bad signal to call ufdb-pstack...", msg );

   BadSignalCaught( SIGQUIT );
   _exit( 4 );
}

