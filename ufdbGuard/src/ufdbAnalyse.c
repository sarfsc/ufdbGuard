/*
 * ufdbanalyse.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * read a Squid log file and produce a report with a table showing
 * percentages for categories.
 *
 * This program is meant to be used as a tool to find out what types
 * of websites are visited.
 *
 * RCS: $Id: ufdbAnalyse.c,v 1.32 2017/05/23 15:54:01 root Exp root $
 */

/* we need maximum speed: */
#undef _FORTIFY_SOURCE

#define UFDB_API_NO_THREADS 1

#include "ufdb.h"
#include "ufdblib.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#if HAVE_PUTC_UNLOCKED

#define myfast_putc(c,fp)   putc_unlocked(c,fp)
#define myfast_getc(fp)     getc_unlocked(fp)

__inline__ static int fast_puts( const char * s, FILE * fp )
{
   int retval;

   retval = 1;
   while (*s != '\0'  &&  ((retval = myfast_putc(*s,fp)) != EOF))
      s++;

   return retval;
}


__inline__ static char * fast_gets( char * s, int size, FILE * fp )
{
   char * retval;
   int    n;
   int    ch;

   retval = s;
   n = 0;
   while (n < size  && 
          (ch = myfast_getc(fp)) != EOF)
   {
      n++;
      *s++ = ch;
      if (ch == '\n')
      {
         *s = '\0';
	 return retval;
      }
   }
   
   if (n == 0 && ch == EOF)
      return NULL;

   *s = '\0';
   return retval;
}

#else

#define myfast_putc(c,fp)  	fputc(c,fp)
#define fast_puts(s,fp)  	fputs(s,fp)

#define myfast_getc(fp)    	fgetc(fp)
#define fast_gets(s,size,fp)    fgets(s,size,fp)

#endif

#if 0
void ufdbFree( void * ptr )
{
   if (ptr != NULL)
      free( ptr );
}

void * ufdbMalloc( size_t n )
{
   char * mem;
   
   mem = malloc( n );
   return mem;
}

void * ufdbRealloc( void * ptr, size_t n )
{
   char * mem;
   
   mem = realloc( ptr, n );
   return mem;
}

void * ufdbCalloc( size_t n, size_t num )
{
   char * mem;

   mem = calloc( n, num );
   return mem;
}

void ufdbGetMallocMutex( char * fn )
{
}

void ufdbReleaseMallocMutex( char * fn )
{
}

char * ufdbStrdup( const char * s )
{
   int size = strlen( s ) + 1;
   return strcpy( ufdbMalloc(size), s );
}
#endif



void ufdbLogMessage( const char * line, ... )
{
   fprintf( stderr, "%s\n:", line );
}


void ufdbLogError( const char * line, ... )
{
   fprintf( stderr, "%s\n:", line );
}


void ufdbLogFatalError( const char * line, ... )
{
   fprintf( stderr, "%s\n:", line );
}


void ufdbSetGlobalErrorLogFile( int mutex_is_used )
{
}


int pthread_mutex_lock( void * mutex )
{
   return 0;
}

int pthread_mutex_unlock( void * mutex )
{
   return 0;
}


static void usage( void )
{
   fprintf( stderr, "usage: ufdbAnalyse [-a] -l <squid-log-file> -d <domainname> -e <email-address> -n <full-name>\n" );
   fprintf( stderr, "flags: -a  logfile has format of Apache instead of Squid\n" );
   fprintf( stderr, "       -l  Squid log file (may be used multiple times)\n" );
   fprintf( stderr, "       -d  your domainname, e.g. example.com\n" );
   fprintf( stderr, "       -e  your email address, e.g. joe@example.com\n" );
   fprintf( stderr, "       -n  your full name\n" );
   fprintf( stderr, "       -v  verbose output\n" );
   exit( 1 );
}


static int parseApacheLog( char * line, char ** code, char ** nbytes, char ** url )
{
   char * p;
   char * slash;
   char * u;

   /* a line in a Apache log file looks like this:
    * 203.199.204.12 - - [30/Nov/2007:11:27:21 +0100]  "GET /smallcross.gif HTTP/1.0"  200  61
    *                    "http://74.86.20.90/livescore/data/crbz-31739122ndtv06/2007/2007_IND_PAK/IND_PAK_NOV30_DEC04/gen_scag.html" 
    *                    "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; FDM; InfoPath.1; .NET CLR 2.0.50727)"
    */

   p = strchr( line, '"' );
   if (p == NULL)
      return 0;

   p = strtok( p, " \t" );		/* HTTP command */
   if (p == NULL)
      return 0;

   u = strtok( NULL, " \t" );		/* URL */
   if (u == NULL)
      return 0;

   /* strip the URL for privacy; remove username/password and query terms */

   p = strstr( u, "://" );
   if (p != NULL)
      u = p + 3;
   slash = strchr( u, '/' );
   p = strchr( u, '@' );
   if (p != NULL)
   {
      if (slash == NULL  ||  p < slash)
	 u = p + 1;
   }

   if (slash != NULL)
   {
      /* truncate very long URLs */
      if (strlen(slash) > 120)
	 *(slash+120) = '\0';

      p = strchr( slash, '?' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, '&' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, ';' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, '%' );
      if (p != NULL)
      {
         char * p2;
	 p2 = strstr( p, "%3F" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3f" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%26" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3B" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3b" );
	 if (p2 != NULL)
	    *p2 = '\0';
      }
   }
   *url = u;

   p = strtok( NULL, " \t" );			/* protocol */
   if (p == NULL)
      return 0;

   *code = strtok( NULL, " \t" );		/* HTTP return code */
   if (*code == NULL)
      return 0;

   *nbytes = strtok( NULL, " \t" );		/* #bytes */
   if (*nbytes == NULL)
      return 0;

   /* skip the rest of the line */

   return 1;
}


static int parseSquidLog( char * line, char ** code, char ** nbytes, char ** url )
{
   char * p;
   char * slash;
   char * u;

   /* a line in a Squid log file looks like this:
    * 1195621581.452     74 10.1.1.2 TCP_MISS/200 1169 GET http://example.com - DIRECT/194.46.8.130 text/html
    */
   if (strtok( line, " \t" ) == NULL) 	/* time */
      return 0;

   if (strtok( NULL, " \t" ) == NULL)	/* dummy */
      return 0;

   if (strtok( NULL, " \t" ) == NULL)	/* IP */
      return 0;

   *code = strtok( NULL, " \t" );	/* Squid code / HTTP code */
   if (*code == NULL)
      return 0;
   p = strchr( *code, '/' );
   if (p != NULL)
      *code = p + 1;

   *nbytes = strtok( NULL, " \t" );	/* #bytes */
   if (*nbytes == NULL)
      return 0;

   if (strtok( NULL, " \t" ) == NULL)	/* HTTP command */
      return 0;

   u = strtok( NULL, " \t" );		/* URL */
   if (u == NULL)
      return 0;

   /* ignore the rest of the input line */

   p = strstr( u, "://" );
   if (p != NULL)
      u = p + 3;
   slash = strchr( u, '/' );
   p = strchr( u, '@' );
   if (p != NULL)
   {
      if (slash == NULL  ||  p < slash)
	 u = p + 1;
   }

   /* strip the URL for privacy; remove username/password and query terms */
   if (slash != NULL)
   {
      /* truncate very long URLs */
      if (strlen(slash) > 120)
	 *(slash+120) = '\0';

      p = strchr( slash, '?' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, '&' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, ';' );
      if (p != NULL)
	 *p = '\0';
      p = strchr( slash, '%' );
      if (p != NULL)
      {
         char * p2;
	 p2 = strstr( p, "%3F" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3f" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%26" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3B" );
	 if (p2 != NULL)
	    *p2 = '\0';
	 p2 = strstr( p, "%3b" );
	 if (p2 != NULL)
	    *p2 = '\0';
      }
   }

   *url = u;

   return 1;
}


static int parseLogfile( 
   FILE *          fp, 
   char *          logfilename, 
   char *          email, 
   int             apachelog, 
   int             verbose, 
   unsigned long * total )
{
   int    errors;
   int    n;
   unsigned long   nbytes_read;
   unsigned long   nbytes_sent;
   unsigned long   nlines_sent;
   char * code;
   char * nbytes;
   char * url;
   FILE * logfile;
   char   linebuf[16*1024];
   char   output[4192];

   logfile = fopen( logfilename, "r" );
   if (logfile == NULL)
   {
      fprintf( stderr, "cannot open \"%s\"\n", logfilename );
      return 0;
   }

   if (verbose)
      printf( "processing log \"%s\"\n", logfilename );

   errors = 0;
   nbytes = 0;
   nbytes_read = 0;
   nbytes_sent = 0;
   nlines_sent = 0;

   code = NULL;
   url = NULL;
   while (fast_gets( linebuf, 16*1024-2, logfile ) != NULL)
   {
      linebuf[16*1024-2] = '\0';
      nbytes_read += strlen( linebuf );
      nlines_sent++;

      if (apachelog)
         n = parseApacheLog( linebuf, &code, &nbytes, &url );
      else
         n = parseSquidLog( linebuf, &code, &nbytes, &url );
      if (n == 0)
      {
	 fprintf( stderr, "%s: line %ld can not be parsed\n", logfilename, nlines_sent );
         errors++;
	 if (errors > 10)
	    return 0;
	 continue;
      }

      n = sprintf( output, "%s %s %s\n", code, nbytes, url );
      nbytes_sent += (long) n;

      if (verbose && (nlines_sent % 25000 == 0))
      {
         putchar( '.' );
	 fflush( stdout );
      }

      if (fast_puts( output, fp ) == EOF)
      {
	 fprintf( stderr, "parseLogfile: while writing a temporary file fast_puts returned error: %s\n", 
                  strerror(errno) );
	 return 0;
      }
      else
	 *total += n;
   }

   if (verbose)
      printf( "\n" );

   if (errors > 0)
   {
      fprintf( stderr, "There were %d parse errors.\n", errors );
      fprintf( stderr, "Are you sure that a Squid log file (e.g. access.log) is specified ?\n" );
      return 0;
   }

   if (nlines_sent < 5)
   {
      fprintf( stderr, "WARNING: the logfile \"%s\" has only %ld URLs which is not sufficient for an analysis.\n", logfilename, nlines_sent );
   }

   printf( "logfile: %s  read %3.1f MB, stored %3.1f MB (%ld URLs)\n", 
	   logfilename, nbytes_read/(1024.0*1024.0), nbytes_sent/(1024.0*1024.0), nlines_sent );

   return errors==0;
}


static char * TLDs[] = 
{
   "ac", "ad", "ae", "aero", "af", "ag", "ai", "al",
   "am", "an", "ao", "aq", "ar", "arpa", "as", "asia",
   "at", "au", "aw", "ax", "az", "ba", "bb", "bd",
   "be", "bf", "bg", "bh", "bi", "biz", "bj", "bm",
   "bn", "bo", "br", "bs", "bt", "bv", "bw", "by",
   "bz", "ca", "cat", "cc", "cd", "cf", "cg", "ch",
   "ci", "ck", "cl", "cm", "cn", "co", "com", "coop",
   "cr", "cu", "cv", "cx", "cy", "cz", "de", "dj",
   "dk", "dm", "do", "dz", "ec", "edu", "ee", "eg",
   "er", "es", "et", "eu", "fi", "fj", "fk", "fm",
   "fo", "fr", "ga", "gb", "gd", "ge", "gf", "gg",
   "gh", "gi", "gl", "gm", "gn", "gov", "gp", "gq",
   "gr", "gs", "gt", "gu", "gw", "gy", "hk", "hm",
   "hn", "hr", "ht", "hu", "id", "ie", "il", "im",
   "in", "info", "int", "io", "iq", "ir", "is", "it",
   "je", "jm", "jo", "jobs", "jp", "ke", "kg", "kh",
   "ki", "km", "kn", "kp", "kr", "kw", "ky", "kz",
   "la", "lb", "lc", "li", "lk", "lr", "ls", "lt",
   "lu", "lv", "ly", "ma", "mc", "md", "me", "mg",
   "mh", "mil", "mk", "ml", "mm", "mn", "mo", "mobi",
   "mp", "mq", "mr", "ms", "mt", "mu", "museum", "mv",
   "mw", "mx", "my", "mz", "na", "name", "nc", "ne",
   "net", "nf", "ng", "ni", "nl", "no", "np", "nr",
   "nu", "nz", "om", "org", "pa", "pe", "pf", "pg",
   "ph", "pk", "pl", "pm", "pn", "pr", "pro", "ps",
   "pt", "pw", "py", "qa", "re", "ro", "rs", "ru",
   "rw", "sa", "sb", "sc", "sd", "se", "sg", "sh",
   "si", "sj", "sk", "sl", "sm", "sn", "so", "sr",
   "st", "su", "sv", "sy", "sz", "tc", "td", "tel",
   "tf", "tg", "th", "tj", "tk", "tl", "tm", "tn",
   "to", "tp", "tr", "travel", "tt", "tv", "tw", "tz",
   "ua", "ug", "uk", "us", "uy", "uz", "va", "vc",
   "ve", "vg", "vi", "vn", "vu", "wf", "ws", "xxx", "ye",
   "yt", "za", "zm", "zw", NULL
};

static int validTLD( 
   char * line )
{
   char * tld;
   char ** t;

   tld = strrchr( line, '.' );
   if (tld == NULL)
      return 0;

   tld++;
   for (t = TLDs; *t != NULL; t++)
   {
      if (strcasecmp( *t, tld ) == 0)
         return 1;
   }
   return 0;
}


int main( int argc, char * argv[] )
{
   char   opt;
   int    counter;
   int    i;
   int    n;
   int    tmpf;
   FILE * tmpfp;
   int    s;
   FILE * fp;
   int    numlogs;
   int    apachelog;
   int    verbose;
   char * tmpdir;
   unsigned long  total_written;
   char   domain[128];
   char   email[128];
   char   fullname[128];
   char   logfile[64][1024];
   char   tempfilename[256];
   char   header[2048];
   char   buffer[32*1024];
   struct timeval      tv;

   logfile[0][0] = email[0] = domain[0] = fullname[0] = '\0';
   numlogs = 0;
   apachelog = 0;
   verbose = 0;

   setenv( "POSIXLY_CORRECT", "POSIX.2", 1 );
   while ((opt = getopt( argc, argv, "?val:d:e:n:" )) > 0)
   {
      switch (opt)
      {
      case 'a':
      	 apachelog = 1;
	 break;
      case 'd':
         strcpy( domain, optarg );
	 break;
      case 'e':
         strcpy( email, optarg );
	 break;
      case 'n':
         strcpy( fullname, optarg );
	 break;
      case 'l':
	 if (numlogs == 64)
	 {
	    fprintf( stderr, "only 64 logfiles can be analysed.\n" );
	    exit( 1 );
	 }
         strcpy( logfile[numlogs], optarg );
	 numlogs++;
	 break;
      case 'v':
         verbose++;
	 break;
      case '?':
      default:
	 usage();
	 break;
      }
   }

   if (fullname[0] == '\0' || numlogs == 0 || email[0] == '\0' || domain[0] == '\0')
      usage();

   if (strchr( email, '@' ) == NULL  ||
       strchr( email, '.' ) == NULL  ||
       strstr( email, "example.com" ) != NULL  ||
       !validTLD(email))
   {
      fprintf( stderr, "The report is emailed to you so you must supply a correct and valid email address\n" );
      fprintf( stderr, "No data is uploaded.  No report will be emailed.\n" );
      exit( 1 );
   }

   fp = fopen( logfile[0], "r" );
   if (fp == NULL)
   {
      fprintf( stderr, "cannot read Squid logfile \"%s\"\n", logfile[0] );
      exit( 1 );
   }
   fclose( fp );

   printf( "The results will be sent by the support desk via email to %s\n", email );


#if 0
   printf( "header sent to %s\n", UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE );
   printf( "%s", header );
#endif

   /* TO-DO: also send local always-allow and always-block categories */

   tmpdir = getenv( "TMPDIR" );
   if (tmpdir == NULL)
      tmpdir = "/tmp";
   sprintf( tempfilename, "%s/ufdb-analyse-%d-%ld", tmpdir, getpid(), random() % 9999 );
   tmpfp = fopen( tempfilename, "w" );
   if (tmpfp == NULL)
   {
      fprintf( stderr, "%s: cannot open temporary file.\n", tempfilename );
      exit( 1 );
   }
   if (verbose)
      printf( "processed URLs are written to temporary file %s\n", tempfilename );

   total_written = 0;
   for (i = 0; i < numlogs; i++)
   {
      if (!parseLogfile( tmpfp, logfile[i], email, apachelog, verbose, &total_written ))
      {
	 fprintf( stderr, "Too many errors occurred.  Upload was interrupted.\n" );
         fclose( tmpfp );
	 unlink( tempfilename );
	 exit( 1 );
      }
   }
   fclose( tmpfp );

   if (verbose)
      printf( "the URLs are in temporary file %s\n", tempfilename );

   tmpf = open( tempfilename, O_RDONLY|O_EXCL, 0600 );
   if (tmpf < 0)
   {
      fprintf( stderr, "%s: cannot open temporary file for reading: %s\n", tempfilename, strerror(errno) );
      exit( 1 );
   }

   /* TO-DO: generate a token, upload it and use the unique token in the feedback email */

   if (verbose)
      printf( "going to upload the URLs (%3.2lf MB)\n", total_written/(1024.0*1024.0) );

   s = UFDBopenSocket( UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, 80 );
   if (s < 0)
   {
      fprintf( stderr, "cannot open communication socket with http://%s  %s\n", 
               UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE, strerror(errno) );
      unlink( tempfilename );
      exit( 1 );
   }
   tv.tv_sec = 600;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 600;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   sprintf( header, "POST " UFDB_UPLOAD_ANALYSE_SQUID_LOG_CGI " HTTP/1.0\r\n"
		    "Host: " UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE "\r\n"
		    "Accept: */*\r\n"
   	 	    "User-Agent: ufdbAnalyse-" UFDB_VERSION "\r\n"
		    "Content-Type: text/plain\r\n"
		    "Content-Length: %ld\r\n"
		    "Connection: close\r\n"
		    "X-mydomain: %s\r\n"
		    "X-myemail: %s\r\n"
		    "X-fullname: %s\r\n"
		    "\r\n",
		    total_written,
		    domain, 
		    email, 
		    fullname );
   n = strlen( header );
   if (write( s, header, n ) != n)
   {
      fprintf( stderr, "cannot write header to http://%s\n", UFDB_UPLOAD_UNCATEGORISED_URLS_WEBSITE );
      unlink( tempfilename );
      exit( 1 );
   }

   counter = 0;
   while ((n = read( tmpf, buffer, 32*1024)) > 0)
   {
      i = write( s, buffer, n );
      if (i < 0 || i != n)
      {
         printf( "error uploading URLs: counter=%d i=%d n=%d %s\n", counter, i, n, 
	         i > 0 ? "incomplete write" : strerror(errno) );
	 break;
      }
      counter++;
      if (verbose && counter % 32 == 0)
      {
         putchar( '.' );
	 fflush( stdout );
      }
   }

   if (verbose)
      putchar( '\n' );

   /* for every 10 MB sleep a second */
   n = total_written / (10 * 1024 * 1024);
   if (n < 1)
      n = 1;
   if (n > 5)
      n = 5;
   if (verbose)
      printf( "Waiting for a response from the server ...\n" );
   sleep( n );

   n = read( s, buffer, 4096 );
   if (n < 0)
   {
      printf( "error reading the server response: %s\n", strerror(errno) );
      unlink( tempfilename );
      exit( 1 );
   }
   buffer[n] = '\0';
   if (sscanf( buffer, "HTTP/%d.%d %d", &i, &i, &n ) != 3)
   {
      printf( "error parsing server response:\n%s\n", buffer );
      unlink( tempfilename );
      exit( 1 );
   }
   if (verbose)
      printf( "server response status code is %d (%s)", n, n==200 ? "OK" : "something is wrong" );

   if (n != 200)
   {
      printf( "The server responded with an unexpected status code %d.  Contact the support desk.\n", n );
      unlink( tempfilename );
      exit( 1 );
   }

   /* TO-DO: the server must send a unique ID and it must be parsed and printed. */
   /* TO-DO: the unique ID can be used in future references with the helpdesk */

   close( s );
   close( tmpf );
   unlink( tempfilename );

   exit( 0 );
   /*NOTREACHED*/
   return 0;
}

