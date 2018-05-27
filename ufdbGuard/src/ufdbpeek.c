/*
 * ufdbpeek.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 *
 * RCS: $Id: ufdbpeek.c,v 1.30 2018/05/25 13:11:45 root Exp root $
 */

#include "ufdb.h"
#include "ufdbchkport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include "sg.h"


static int loadDatabaseTables = 0;

static void usage( void )
{
   fprintf( stderr, "ufdbpeek is Copyright (C) 2011-2018 by URLfilterDB B.V. with all rights reserved.\n" );
   fprintf( stderr, "ufdbpeek version " UFDB_VERSION "\n" );
   fprintf( stderr, "usage: ufdbpeek [-p <port>] [-C CAcertsFile] [-D CAcertsDir] [-d] [-A] [-F] [-G] [-S] [-Y] <domain>\n" );
   exit( 1 );
}


static void doLoadDatabaseTables( char * dbhome )
{
   int    i;
   char   fname[1024];
   char * category;

   char * categories[] = {
      "safe",
      "adult",
      "arms",
      "ms-data-collection",
      "p2p",
      "proxies",
      "malware",
      "warez",
      "phishtank",
      "gambling",
      "violence",
      "drugs",
      "illegal",
      "extappl",
      "socialnet",
      "youtube",
      "vimeo",
      "dailymotion",
      "webtv",
      "webradio",
      "chat",
      "dating",
      "games",
      "entertain",
      "finance",
      "jobs",
      "shops",
      "sports",
      "travel",
      "news",
      "webmail",
      "forum",
      "private",
      "toolbars",
      "audio-video",
      "ads",
      "mozilla",
      "religion",
      "searchengine",
      "checked",
      NULL
   };

   ufdbSetting( "dbhome", dbhome );

   for (i = 0;  (category = categories[i]) != NULL;  i++ )
   {
      struct stat stbuf;
      ufdbCategory( ufdbStrdup(category) );
      sprintf( fname, "%s/domains", category );
      ufdbCategoryDomainList( ufdbStrdup(fname) );
      sprintf( fname, "%s/expressions", category );
      if (stat( fname, &stbuf ) == 0  &&  stbuf.st_size > 0)
         ufdbCategoryExpressionList( ufdbStrdup(fname), "n" );
      ufdbCategoryEnd();
   }
}


int main( int argc, char * argv[] )
{
   int    opt;
   int    port;
   int    certErrors;
   char * content;
   char * t;
   char * url;
   char   cn[1024];

   strcpy( UFDBprogname, "ufdbpeek" );
   globalPid = getpid();
   ufdbSetting( "logdir", "." );
   ufdbSetGlobalErrorLogFile( 0 );
   globalErrorLog = NULL;

   if (argc <= 1)
      usage();

   UFDBglobalDebug = 1;
   UFDBglobalPeek = 1;
   UFDBglobalHttpsOfficialCertificate = 0;
   UFDBglobalHttpsEnforceContentPeek = 1;
   UFDBglobalHttpsWithHostname = 1;
   UFDBglobalYoutubeEdufilter = 1;
   UFDBglobalAllowGoogleHTTPSusingIP = 1;
   UFDBsetTunnelCheckMethod( UFDB_API_HTTPS_CHECK_AGRESSIVE );
   ufdbSetSignalHandler( SIGPIPE, SIG_IGN );
   UFDBglobalReconfig = 0;

   port = 443;
   url = NULL;

   while ((opt = getopt( argc, argv, "?C:D:dhp:AFGSY" )) > 0)
   {
      switch (opt) {
      case 'C':
      	 strcpy( UFDBglobalCAcertsFile, optarg );
	 break;
      case 'D':
      	 strcpy( UFDBglobalCAcertsDir, optarg );
	 break;
      case 'd':
	 loadDatabaseTables = 1;
	 UFDBglobalDebug++;
         break;
      case 'p':
         port = atoi( optarg );
	 break;
      case 'A':
	 UFDBglobalDebugAim = 1;
	 UFDBglobalAimOverHttps = 1;
         break;
      case 'F':
	 UFDBglobalDebugFBchat = 1;
	 UFDBglobalFBchatOverHttps = 1;
         break;
      case 'G':
	 UFDBglobalDebugGtalk = 1;
	 UFDBglobalGtalkOverHttps = 1;
         break;
      case 'S':
	 UFDBglobalDebugSkype = 1;
	 UFDBglobalSkypeOverHttps = 1;
         break;
      case 'Y':
	 UFDBglobalDebugYahooMsg = 1;
	 UFDBglobalYahooMsgOverHttps = 1;
         break;
      case '?':
      case 'h':
          usage();
	  exit( 0 );
	  break;
      }
   }

   if (optind < argc)
      url = argv[optind];
   else
      usage();

   ufdbLogMessage( "ufdbpeek v" UFDB_VERSION " is Copyright (C) 2011-2018 by URLfilterDB B.V. with all rights reserved." );

   if (loadDatabaseTables)
      doLoadDatabaseTables( DEFAULT_DBHOME );

   UFDBinitHTTPSchecker();
   if (strncmp( url, "http://", 7 ) == 0)
      url += 7;
   if (strncmp( url, "https://", 8 ) == 0)
      url += 8;
   if ((t = strchr( url, '@' )) != NULL)
      *t = '\0';
   if ((t = strstr( url, ":443" )) != NULL)
      *t = '\0';
#if 0
   if ((t = strchr( url, ':' )) != NULL)
      *t = '\0';
#endif
   if ((t = strchr( url, '/' )) != NULL)
      *t = '\0';

   certErrors = 0;
   content = NULL;
   (void) UFDBsslPeekServer( url, port, cn, &certErrors, &content, 0 );

   if (content != NULL)
   {
      if (strlen(content) > 30000)
	 ufdbLogMessage( "%30000.00000s", content );
      else
         ufdbLogMessage( "%s", content );
   }

   if (globalErrorLog != NULL)
   {
      fflush( globalErrorLog );
      fclose( globalErrorLog );
   }

   return 0;
}

