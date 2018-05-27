/*
 * httpserver.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * serve HTTP GET requests for /cgi-bin/URLblocked.cgi
 *
 * RCS $Id: httpserver.c,v 1.97 2017/09/17 21:25:46 root Exp root $
 */


#if defined(__OPTIMIZE__) && defined(__GNUC__)  &&  defined(GCC_INLINE_STRING_FUNCTIONS_ARE_FASTER)
#undef _FORTIFY_SOURCE
#define __USE_STRING_INLINES  1
#endif

#if 0
#define strmatch2(a,b) (strcmp(a,b) == 0)
#define strmatch3(a,b) (strcmp(a,b) == 0)
#define strmatch4(a,b) (strcmp(a,b) == 0)
#define strmatch5(a,b) (strcmp(a,b) == 0)
#else
#define strmatch2(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == '\0')
#define strmatch3(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == '\0')
#define strmatch4(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == '\0')
#define strmatch5(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  && (a)[5] == '\0')
#endif

#include "ufdb.h"
#include "ufdblib.h"
#include "httpserver.h"
#include "version.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>


static int    defaultFLVsize = 0;
static char * defaultFLVcontent = NULL;
static int    defaultMP3size = 0;
static char * defaultMP3content = NULL;
static int    defaultMPGsize = 0;
static char * defaultMPGcontent = NULL;
static int    defaultWMVsize = 0;
static char * defaultWMVcontent = NULL;


#define HTML_COMMENT \
   "<!-- ufdbguardd http-server " UFDB_VERSION " -->\r\n"  \
   "<!-- long comment to disable MSIE and Chrome so-called friendly error page -->\r\n"  \
   "<!-- long comment to disable MSIE and Chrome so-called friendly error page -->\r\n"  \
   "<!-- long comment to disable MSIE and Chrome so-called friendly error page -->\r\n"  \
   "<!-- long comment to disable MSIE and Chrome so-called friendly error page -->\r\n"  \
   "<!-- long comment to disable MSIE and Chrome so-called friendly error page -->\r\n"

#define LANG_IND_EN  0
#define LANG_IND_NL  1
#define LANG_IND_DE  2
#define LANG_IND_PL  3
#define LANG_IND_IT  4
#define LANG_IND_ES  5
#define LANG_IND_PT  6
#define LANG_IND_FR  7
#define LANG_IND_TR  8
#define LANG_IND_SV  9


#define LANG_IND_DEFAULT  LANG_IND_EN

static char _fatal_error_text [] =
   "<center>\n"
   "<font color=red><b>"
   "Access to the internet is blocked because<br>\n"
   "the URL filter has a fatal error. <br>\n"
   "Ask you helpdesk or web proxy administrator for help.\n"
   "</b></font>\n"
   "</center>\n";
static char _loading_database_text [] =
   "<center>\n"
   "<font color=red><b>"
   "Access to the internet is temporarily blocked because<br>\n"
   "a new URL database is being loaded by the URL filter. <br>\n"
   "Wait one minute and try again.\n"
   "</b></font>\n"
   "</center>\n";

static const char * _title [] =
{
   /* EN */  "403 Forbidden",			
   /* NL */  "403 Geen Toegang",
   /* DE */  "403 Verboten",
   /* PL */  "403 Cenzura, zakaz pobrania",
   /* IT */  "403 Accesso negato",
   /* ES */  "403 Ning&uacute;n acces",
   /* PT */  "403 Proibido",
   /* FR */  "403 Interdit",
   /* TR */  "403 Eri&#351;im engellendi",
   /* SV */  "403 F&ouml;rbjuden"
};
static const char * _forbidden [] =
{
   "Forbidden",
   "Geen Toegang",
   "Verboten",
   "Cenzura, zakaz pobrania",
   "Accesso negato",
   "Ning&uacute;n acces",
   "Proibido",
   "Interdit",
   "Eri&#351;im engellendi",
   "Sidan stoppad enligt landstingets riktlinjer"
};
static const char * _explain_1 [] =
{
   "Access is blocked since the URL is in the filter category",
   "De toegang is geblokkeerd omdat de URL in de categorie",
   "Zugriff verweigert weil die URL die Klassifizierung",
   "Nie otworzysz tej strony bo jest ona sklasyfikowana jako",
   "L'accesso &egrave; negato poich&eacute; l'URL appartiene a none",
   "Se bloquea el acceso puesto que el URL se considera ser",
   "O acesso a este site foi bloqueado porque o conte&uacute;do est&aacute;",
   "L'access est inderdit parce que le site est",
   "Ula&#351;mak istedi&#287;iniz sayfaya eri&#351;im kapal&#305;d&#305;r. S&#305;n&#305;f&#305;:",
   "Access till denna sida &auml;r stoppad:"
};
static const char * _explain_2 [] =
{
   ".",
   " valt.",
   " hat.",
   " przez program kontroli ufdbGuard.",
   ".",
   ".",
   ".",
   ".",
   ".",
   "."
};
static const char * _moreInfo1 [] =
{
   "More information about ufdbGuard is <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Meer informatie over ufdbGuard is <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Mehr Informationen &uuml;ber ufdbGuard ist <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Informacja o tym programie kontroli jest na <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Maggiori informazioni su ufdbGuard sono disponibili <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "M&aacute;s informaci&oacute;n sobre ufdbGuard est&aacute; <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Mais informa&ccedil;&atilde;o sobre ufdbGuard est&aacute; <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Plus d'information de ufdbGuard est <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "ufdbGuard hakk&#305;nda bilgi i&ccedil;in <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL,
   "Mer information om ufdbGuard &auml;r <a href=\"" UFDB_EXPLAIN_DENY_REASON_URL
};
static const char * _moreInfo2 [] =
{
   "\" target=\"_blank\">here</a>.",
   "\" target=\"_blank\">hier</a>.",
   "\" target=\"_blank\">hier</a>.",
   "\" target=\"_blank\">stronie</a>.",
   "\" target=\"_blank\">qui</a>.",
   "\" target=\"_blank\">aqu&iacute;</a>.",
   "\" target=\"_blank\">aqui</a>.",
   "\" target=\"_blank\">ici</a>.",
   "\" target=\"_blank\">t&#305;klay&#305;n&#305;z</a>.",
   "\" target=\"_blank\"h&auml;r</a>."
};
static const char * _goBack [] = 
{
   "Click here to go back",
   "Klik hier om terug te gaan",
   "Klicken Sie hier um zur&uuml;ck zu gehen",
   "Wycofaj do poprzedniej strony",
   "tornare indietro",
   "ir detr&aacute;s",
   "volte",
   "rentrer",
   "&Ouml;nceki sayfa",
   "Klicka h&auml;r f&ouml;r att komma tillbaks"
};


static void initData( 
   const char * imagesDirectory )
{
   int         in;
   struct stat statbuf;
   char        filename[1024];

   sprintf( filename, "%s/default.flv", imagesDirectory );
   in = open( filename, O_RDONLY );
   if (in >= 0)
   {
      if (fstat( in, &statbuf ) >= 0)
      {
	 defaultFLVcontent = (char *) ufdbMalloc( statbuf.st_size );
	 if (read( in, defaultFLVcontent, statbuf.st_size ) == statbuf.st_size)
	    defaultFLVsize = statbuf.st_size;
      }
      close( in );
   }

   sprintf( filename, "%s/default.mp3", imagesDirectory );
   in = open( filename, O_RDONLY );
   if (in >= 0)
   {
      if (fstat( in, &statbuf ) >= 0)
      {
	 defaultMP3content = (char *) ufdbMalloc( statbuf.st_size );
	 if (read( in, defaultMP3content, statbuf.st_size ) == statbuf.st_size)
	    defaultMP3size = statbuf.st_size;
      }
      close( in );
   }

   sprintf( filename, "%s/default.mpeg", imagesDirectory );
   in = open( filename, O_RDONLY );
   if (in >= 0)
   {
      if (fstat( in, &statbuf ) >= 0)
      {
	 defaultMPGcontent = (char *) ufdbMalloc( statbuf.st_size );
	 if (read( in, defaultMPGcontent, statbuf.st_size ) == statbuf.st_size)
	    defaultMPGsize = statbuf.st_size;
      }
      close( in );
   }

   sprintf( filename, "%s/default.wmv", imagesDirectory );
   in = open( filename, O_RDONLY );
   if (in >= 0)
   {
      if (fstat( in, &statbuf ) >= 0)
      {
	 defaultWMVcontent = (char *) ufdbMalloc( statbuf.st_size );
	 if (read( in, defaultWMVcontent, statbuf.st_size ) == statbuf.st_size)
	    defaultWMVsize = statbuf.st_size;
      }
      close( in );
   }

   /* TODO: read all forbidden images */
}


void writeHttpdPidFile( void )
{
   FILE * fp;

   (void) unlink( globalHttpdPidFile );
   fp = fopen( globalHttpdPidFile, "w" );
   if (fp == NULL)
      ufdbLogError( "cannot write to PID file %s - check file and directory permission and ownership", globalHttpdPidFile );
   else
   {
      fprintf( fp, "%d\n", globalPid );
      fclose( fp );
      if (UFDBglobalDebugHttpd  || UFDBglobalDebug)
         ufdbLogMessage( "wrote my pid (%d) to %s", globalPid, globalHttpdPidFile );
   }
}


void removeHttpdPidFile( void )
{
   if (UFDBglobalDebugHttpd  || UFDBglobalDebug)
      ufdbLogMessage( "removing pid file %s", globalHttpdPidFile );
   (void) unlink( globalHttpdPidFile );
}


static void writeBuffer( int fd, char * buffer, int size )
{
   int retval;

try_again:
   retval = write( fd, buffer, size );
   if (retval < 0)
   {
      if (errno == EINTR)
         goto try_again;
      ufdbLogError( "httpServer: write failed: %s", strerror(errno) );
      return;
   }

   if (retval < size)
   {
      buffer += retval;
      size -= retval;
      goto try_again;
   }
}


static void AnswerHttpTimeout( int fd )
{
   int       hdrlen;
   int       bodylen;
   time_t    now_t;
   struct tm t;
   char      header[2048];
   char      content[8192+1200];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpTimeout" );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   hdrlen = sprintf( header, 
            "HTTP/1.0 200 OK\r\n"						/* or 50X ?? */
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=180\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );

   bodylen = sprintf( content,
            "<html>\r\n"
	    "<head>\r\n"
	    "<title>Timeout</title>\r\n"
	    "</head>\r\n"
	    "<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"orange\" text=\"black\">\r\n"
	    "<font size=\"+2\">A timeout error occurred. <br> </font>\r\n"
	    "<font size=\"+1\">\r\n"
	    "The HTTP message for ufdbGuard was not received within the timeout period<br>\r\n"
	    "The URL that you are trying to access is blocked.<br>\r\n"
	    "</font>\r\n"
	    HTML_COMMENT
	    "</body>\r\n"
	    "</html>\r\n"
	    );

   writeBuffer( fd, header, hdrlen );
   writeBuffer( fd, content, bodylen );
}


static void AnswerHttpCrossdomain( int fd )
{
   int       hdrlen;
   int       bodylen;
   time_t    now_t;
   struct tm t;
   char      header[2048];
   char      content[8192+1200];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpCrossdomain:" );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );
   hdrlen = sprintf( header, 
            "HTTP/1.0 200 OK\r\n"
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=60\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/xml\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );
   bodylen = sprintf( content,
	    "<?xml version=\"1.0\"?>\n"
	    "<cross-domain-policy>\n"
  	    "   <allow-access-from domain=\"*\" />\n"
  	    "</cross-domain-policy>\n"
	    );

   writeBuffer( fd, header, hdrlen );
   writeBuffer( fd, content, bodylen );
}


static void AnswerHttpEmpty( int fd, char * url )
{
   int       hdrlen;
   int       bodylen;
   time_t    now_t;
   struct tm t;
   char      header[2048];
   char      content[8192+1200];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpEmpty: %s", url );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   hdrlen = sprintf( header, 
            "HTTP/1.0 200 OK\r\n"
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=180\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );

   bodylen = sprintf( content,
            "<html>\r\n"
	    "<head>\r\n"
	    "<title>Error</title>\r\n"
	    "</head>\r\n"
	    "<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"orange\" text=\"black\">\r\n"
	    "<font size=\"+2\">An error occurred. <br> </font>\r\n"
	    "<font size=\"+1\">\r\n"
	    "This http server can only serve URL requests for ufdbGuard <br>\r\n"
	    "redirection messages and does not understand the URL. <br>\r\n"
	    "URL: <tt>%s</tt> <br>\r\n"
	    "Most likely the configuration of \"redirect\" statements is incorrect.  It should include \"/cgi-bin/URLblocked.cgi\". <br>\r\n"
	    "</font>\r\n"
	    HTML_COMMENT
	    "</body>\r\n"
	    "</html>\r\n"
	    ,
	    url
	    );

   writeBuffer( fd, header, hdrlen );
   writeBuffer( fd, content, bodylen );
}


static void AnswerHttpNotFound( int fd, char * url )
{
   int       hdrlen;
   time_t    now_t;
   struct tm t;
   char      header[2048];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpNotFound: %s", url );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   hdrlen = sprintf( header, 
            "HTTP/1.0 404 Not Found\r\n"
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=5\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n"
	    "Content-Length: 0\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );

   writeBuffer( fd, header, hdrlen );
}


static void AnswerHttpHead( int fd, char * url )
{
   int       hdrlen;
   time_t    now_t;
   struct tm t;
   char      header[2048];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpHead: %s", url );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   hdrlen = sprintf( header, 
            "HTTP/1.0 204 No Content\r\n"					/* TODO: or maybe 304 unmodified ?? */
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=5\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/plain\r\n"
	    "Content-Length: 0\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );

   writeBuffer( fd, header, hdrlen );
}


static void AnswerHttpPost( int fd, char * url )
{
   int       hdrlen;
   time_t    now_t;
   struct tm t;
   char      header[2048];

   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpPost: %s", url );

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   hdrlen = sprintf( header, 
            "HTTP/1.0 204 No Content\r\n"
	    "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	    "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	    "Cache-Control: max-age=5\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/plain\r\n"
	    "Content-Length: 0\r\n"
	    "\r\n",
            &"SunMonTueWedThuFriSat"[t.tm_wday*3],
            t.tm_mday,
            &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
            t.tm_year + 1900,
            t.tm_hour, t.tm_min, t.tm_sec
	    );

   writeBuffer( fd, header, hdrlen );
}


static void decodeURIvalue( 
   char * new_text,
   char * text )
{
   char * b;

   b = new_text;
   while (*text != '\0')
   {
      if (*text == '%')
      {
	 int u1, u2, ascii;
	 text++;
#ifdef UFDB_REPARSE_PERCENT_IN_URI
reparse:
#endif
	 if (*text == '\0' || *text+1 == '\0')
	 {
	    *b++ = '%';
	    *b = '\0';
	    return;
	 }
	 u1 = *text++;
	 if (u1 == '%')
	 {
	    *b++ = '%';
	    continue;
	 }
	 if (u1 >= 'A'  &&  u1 <= 'F')
	    u1 = u1 - 'A' + 10;
	 else if (u1 >= 'a'  &&  u1 <= 'f')
	    u1 = u1 - 'a' + 10;
	 else
	    u1 = u1 - '0';
	 u2 = *text++;
	 if (u2 >= 'A'  &&  u2 <= 'F')
	    u2 = u2 - 'A' + 10;
	 else if (u2 >= 'a'  &&  u2 <= 'f')
	    u2 = u2 - 'a' + 10;
	 else
	    u2 = u2 - '0';
         ascii = u1 * 16 + u2;
	 if (ascii < 9 || ascii > 127)
	    ascii = 32;
#ifdef UFDB_REPARSE_PERCENT_IN_URI
	 if (ascii == '%')
	    goto reparse;
#endif
	 if (ascii <= 'Z' && ascii >= 'A')
	    *b++ = ascii + ('a' - 'A');
	 else
	    *b++ = ascii;
      }
      else
      {
	 if (*text <= 'Z' && *text >= 'A')
	    *b++ = *text + ('a' - 'A');
	 else
	    *b++ = *text;
	 text++;
      }
   }
   *b = '\0';
}


#define CONTENTTYPE_IMAGE  0
#define CONTENTTYPE_JAVA   1
#define CONTENTTYPE_HTML   2
#define CONTENTTYPE_XML    3
#define CONTENTTYPE_CSS    4
#define CONTENTTYPE_TEXT   5
#define CONTENTTYPE_JSON   6
#define CONTENTTYPE_STREAM 7
#define CONTENTTYPE_204    8

static const char * contentType2String( 
   int contentType )
{
   switch (contentType)
   {
      case CONTENTTYPE_IMAGE:    return "image";
      case CONTENTTYPE_JAVA:     return "java";
      case CONTENTTYPE_HTML:     return "html";
      case CONTENTTYPE_XML:      return "xml";
      case CONTENTTYPE_CSS:      return "css";
      case CONTENTTYPE_TEXT:     return "text";
      case CONTENTTYPE_JSON:     return "json";
      case CONTENTTYPE_STREAM:   return "stream";
      case CONTENTTYPE_204:   	 return "E204";
   }
   return "error";
}


/* If the URL has no file suffix, we cannot know the content type.
 * Historically it was guessed that the file type was HTML.
 * Now we try to guess the file type looking at the URL because sometimes there are clues.
 * If there are no clues, we return the default_type.
 */
static int guessContentType( 
   char * url,
   int    default_type )
{
   char * path;
   char * r;

   /* strip http://  https://   ftp://  */
   r = strstr( url, "://" );
   if (r != NULL  &&  r-url < 7)
      url = r + 3;

   path = strchr( url, '/' );
   if (path == NULL  ||  *(path+1) == '\0')		/* not always HTML :-( but our best guess */
      return CONTENTTYPE_HTML;

   if (strncmp( url, "www.googleadservices.com/pagead/conversion/", 43 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "googleadservices.com/pagead/conversion/", 39 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "www.google-analytics.com/collect" ) == 0)
      return CONTENTTYPE_IMAGE;

   /* object is xml, html, java or video ...  :-)  so we choose text because it
    * sends an empty object.
    */
   if (strncmp( url, "pubads.g.doubleclick.net/gampad/ads", 35 ) == 0)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "googleads.g.doubleclick.net/pagead/ads", 38 ) == 0)
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "a.analytics.yahoo.com/fpc.pl" ) == 0  ||
       strcmp( url, "o.analytics.yahoo.com/fpc.pl" ) == 0  ||
       strcmp( url, "s.analytics.yahoo.com/fpc.pl" ) == 0)
      return CONTENTTYPE_JAVA;
   if (strcmp( url, "a.analytics.yahoo.com/p.pl" ) == 0  ||
       strcmp( url, "o.analytics.yahoo.com/p.pl" ) == 0  ||
       strcmp( url, "s.analytics.yahoo.com/p.pl" ) == 0  ||
       strcmp( url, "a.analytics.yahoo.com/itr.pl" ) == 0  ||
       strcmp( url, "o.analytics.yahoo.com/itr.pl" ) == 0  ||
       strcmp( url, "s.analytics.yahoo.com/itr.pl" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "row.bc.yahoo.com/b" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "geo.yahoo.com/t" ) == 0  ||
       strcmp( url, "geo.yahoo.com/p" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "pclick.internal.yahoo.com/p/s=", 30 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "br.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "de.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "nl.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "fr.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "it.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "ch.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "at.yahoo.com/jserror" ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".bc.yahoo.com/b" )) != NULL  &&
        strcmp( r, ".bc.yahoo.com/b" ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".chartbeat.net/ping" )) != NULL  &&
       r < path)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "www.nytimes.com/adx/bin/clientside", 34 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "goshka.mail.ru/gstat" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "swa.mail.ru/cgi-bin/counters" ) == 0)
      return CONTENTTYPE_JAVA;
   if (strcmp( url, "rs.mail.ru/un" ) == 0)
      return CONTENTTYPE_IMAGE;
   if ((r = strstr( url, ".top.mail.ru/counter" )) != NULL  &&
       r < path)
      return CONTENTTYPE_IMAGE;
   if ((r = strstr( url, ".top.list.ru/counter" )) != NULL  &&
       r < path)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "s.youtube.com/api/stats/", 24 ) == 0)
      return CONTENTTYPE_TEXT;						/* 204 */

   if (strncmp( url, "www.youtube.com/cp/", 15 ) == 0  ||
       strncmp( url, "www.youtube.com/p/", 14 ) == 0   ||
       strncmp( url, "www.youtube.com/v/", 14 ) == 0   ||
       strncmp( url, "www.youtube.com/ptracking", 25 ) == 0  ||
       strncmp( url, "www.youtube.com/videoplayback", 29 ) == 0)
      return CONTENTTYPE_STREAM;

   if ((r = strstr( url, ".scorecard.com/" )) != NULL  &&
       r < path)
   {
      if (strncmp( url, "b.scorecardresearch.com/", 24 ) == 0)
      {
	 if (strcmp( path, "/b" ) == 0  ||
	     strcmp( path, "/p" ) == 0  ||
	     strcmp( path, "/p2" ) == 0)
	    return CONTENTTYPE_IMAGE;
      }
      if (strncmp( url, "a.scorecardresearch.com/rpc.flow", 32 ) == 0)
	 return CONTENTTYPE_JAVA;

      return CONTENTTYPE_TEXT;
   }

   if (strncmp( url, "link.theplatform.com/s/", 23 ) == 0  &&
       strstr( path, "/tracker.log" ) != NULL)
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "t-l3.hulu.com/beacon/v3/playback" ) == 0  ||
       strcmp( url, "t.hulu.com/beacon/v3/error" ) == 0)
      return CONTENTTYPE_TEXT;						/* void */
   if (strncmp( url, "t2.hulu.com/v3/playertracking/", 30 ) == 0  ||
       strncmp( url, "t2.hulu.com/v3/recommendationtracking/tracking", 46 ) == 0  ||
       strcmp(  url, "t2.hulu.com/v3/contentinteraction/cploaded" ) == 0  ||
       strcmp(  url, "t.hulu.com/beacon/v3/error" ) == 0  ||
       strncmp( url, "t2.hulu.com/v3/revenue/", 23 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "r.casalemedia.com/rum" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "as.casalemedia.com/", 19 ) == 0)
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "t.mookie1.com/t/v1/imp" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "ulog.tealiumiq.com/ulog" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "rtd.tubemogul.com/upi/" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "ad.yieldmanager.com/unpixel" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "ad.yieldmanager.com/imp" ) == 0)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "www.tns-counter.ru/v1", 21 ) == 0  ||
       strncmp( url, "rw.tns-counter.ru/v1", 20 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "adserver.adtech.de/addyn", 24 ) == 0)
      return CONTENTTYPE_JAVA;

   if (strcmp( url, "pagead2.googlesyndication.com/pagead/imgad" ) == 0)	/* gif or swf */
      return CONTENTTYPE_STREAM;

   if (strstr( url, ".overture.com/js" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "adfarm.mediaplex.com/ad/tr/", 27 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "b.aol.com/click" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "b.aol.com/ping" ) == 0)
      return CONTENTTYPE_TEXT;

   if ((r = strstr( url, ".doubleclick.net/" )) != NULL  &&  r < path)
   {
      if (strcmp( path, "/push" ) == 0)
         return CONTENTTYPE_TEXT;			/* HTML or 204 no content */
      if (strncmp( path, "/xbbe/view", 10 ) == 0)
         return CONTENTTYPE_JAVA;
      if (strncmp( path, "/xbbe/pixel", 11 ) == 0)
         return CONTENTTYPE_HTML;
      if (strncmp( path, "/xbbe/match", 11 ) == 0)
         return CONTENTTYPE_TEXT;

      if (strncmp( path, "/mads/", 6 ) == 0)
         return CONTENTTYPE_HTML;

      if (strncmp( path, "/adi/", 5 ) == 0)
         return CONTENTTYPE_HTML;

      if (strncmp( path, "/adj/", 5 ) == 0  ||
          strncmp( path, "/pfadj/", 7 ) == 0  ||
          strncmp( path, "/noidadj/", 9 ) == 0)
         return CONTENTTYPE_JAVA;

      if (strncmp( path, "/adx/", 5 ) == 0  ||
          strncmp( path, "/pfadx/", 7 ) == 0  ||
          strncmp( path, "/noidadx/", 9 ) == 0)
         return CONTENTTYPE_XML;

      if (strcmp( path, "/simgad" ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strncmp( path, "/pixel", 6 ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strncmp( path, "/imp", 4 ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strcmp( path, "/activity" ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strcmp( path, "/activityi" ) == 0)
	 return CONTENTTYPE_HTML;
      if (strncmp( path, "/ad/", 4 ) == 0)		/* redirects to IMAGE (most), HTML (hulu.com) and XML (where?) content ... */
	 return CONTENTTYPE_TEXT;
      if (strcmp( path, "/json" ) == 0)
         return CONTENTTYPE_JSON;
      if (strncmp( path, "/pagead/conversion/", 19 ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strncmp( path, "/pagead/viewthroughconversion/", 30 ) == 0)
	 return CONTENTTYPE_IMAGE;

      r = strchr( path+1, '/' );
      if (r != NULL)			/* ad-emea.doubleclick.net/N6514/adj/newsticker/homepage */
      {
	 if (strncmp( r, "/adi/", 5 ) == 0)
	    return CONTENTTYPE_HTML;
	 if (strncmp( r, "/adj/", 5 ) == 0  ||
	     strncmp( r, "/pfadj/", 7 ) == 0  ||
	     strncmp( r, "/noidadj/", 9 ) == 0)
	    return CONTENTTYPE_JAVA;
	 if (strncmp( r, "/adx/", 5 ) == 0  ||
	     strncmp( r, "/pfadx/", 7 ) == 0  ||
	     strncmp( r, "/noidadx/", 9 ) == 0)
	    return CONTENTTYPE_XML;
	 if (strncmp( r, "/imp", 4 ) == 0)
	    return CONTENTTYPE_IMAGE;
	 if (strncmp( r, "/pixel", 6 ) == 0)
	    return CONTENTTYPE_IMAGE;
	 if (strncmp( r, "/ad/", 5 ) == 0)
	    return CONTENTTYPE_TEXT;
	 if (strcmp( r, "/json" ) == 0)
	    return CONTENTTYPE_JSON;
      }
   }

   if (strncmp( url, "zdbb.net/l/", 11 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "sync.mathtag.com/sync/img" ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".ivwbox.de/" )) != NULL  &&  r < path)
   {
      if (strstr( path, "ivw/cp/" ) != NULL)
         return CONTENTTYPE_IMAGE;
   }

   if ((r = strstr( url, ".tlm100.net/" )) != NULL  &&  r < path)
   {
      if (strstr( path, "/pse" ) != NULL)
         return CONTENTTYPE_IMAGE;
   }

   if ((r = strstr( url, ".intellitxt.com/" )) != NULL  &&  r < path)
   {
      if (strncmp( path, "/intellitxt/front.asp", 21 ) == 0)
         return CONTENTTYPE_JAVA;
      if (strncmp( path, "/v4/init", 8 ) == 0)
         return CONTENTTYPE_JAVA;
      if (strncmp( path, "/v5/init", 8 ) == 0)
         return CONTENTTYPE_JAVA;
      if (strncmp( path, "/v6/init", 8 ) == 0)
         return CONTENTTYPE_JAVA;
      if (strncmp( path, "/v7/init", 8 ) == 0)
         return CONTENTTYPE_JAVA;
   }

   if (strcmp( url, "ad.zanox.com/ppv/" ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strcmp( url, "ad.zanox.com/ppc/" ) == 0)
      return CONTENTTYPE_HTML;
   if (strcmp( url, "zbox.zanox.com/ppb/" ) == 0)
      return CONTENTTYPE_HTML;

   if (strcmp( url, "ps.googleusercontent.com/beacon" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( url, ".sitestat.com/" ) != NULL)
   {
      if (ufdbStrStrEnd( path, "/s" ))
	 return CONTENTTYPE_IMAGE;
   }

   if (strcmp( url, "hits.blogsoft.org/" ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".adjuggler.net/servlet" )) != NULL  &&
       r < path)
   {
      if (strstr( path, "/vj" ) != NULL)
	 return CONTENTTYPE_JAVA;
      if (strstr( path, "/vh" ) != NULL)
	 return CONTENTTYPE_HTML;
      if (strstr( path, "/vx" ) != NULL)		/* used in video player for event tracking */
	 return CONTENTTYPE_TEXT;
      if (strstr( path, "/cc" ) != NULL)		/* used in video player for event tracking */
	 return CONTENTTYPE_TEXT;	
   }

   if (strncmp( url, "view.atdmt.com/", 15 ) == 0  && path != NULL)
   {
      /* /jaction is either html of java ! */
      if (strncmp( path, "/action/", 8 ) == 0)
         return CONTENTTYPE_IMAGE;
      if (strstr( path, "/jaction/" ) != NULL)
	 return CONTENTTYPE_JAVA;
      if (strstr( path, "/jview/" ) != NULL)
	 return CONTENTTYPE_JAVA;
      if (strstr( path, "/view/" ) != NULL)	/* redirected image */
	 return CONTENTTYPE_IMAGE;
      if (strstr( path, "/iview/" ) != NULL)	/* iframe or bodyimage  */
	 return CONTENTTYPE_HTML;
      if (strstr( path, "/iaction/" ) != NULL)	/* iframe or bodyimage  */
	 return CONTENTTYPE_HTML;
   }

   if (strcmp( url, "www.facebook.com/dialog/oauth" ) == 0  ||
       strcmp( url, "static.ak.fbcdn.net/connect/xd_proxy.php" ) == 0)
      return CONTENTTYPE_HTML;
   if (strncmp( url, "static.ak.fbcdn.net/connect.php/js/fb.share", 43 ) == 0)
      return CONTENTTYPE_JAVA;
   if (strncmp( url, "static.ak.fbcdn.net/connect.php/css/", 36 ) == 0)
      return CONTENTTYPE_CSS;
   if (strncmp( url, "static.ak.connect.facebook.com/connect.php/en_us/css/", 53 ) == 0)
      return CONTENTTYPE_CSS;
   if (strcmp( url, "static.ak.connect.facebook.com/connect.php/en_us/js/api/canvasutil/connect/xfbml" ) == 0)
      return CONTENTTYPE_JSON;
   if (strncmp( url, "static.ak.connect.facebook.com/connect.php", 42 ) == 0)
      return CONTENTTYPE_JAVA;
   if (strncmp( url, "www.facebook.com/ajax/presence/reconnect.php", 44 ) == 0)
      return CONTENTTYPE_JAVA;
   if (strcmp( url, "www.facebook.com/fr/u.php" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "log.adap.tv/log", 15 ) == 0)
      return CONTENTTYPE_TEXT;
#if 0
   if (strncmp( url, "www.facebook.com/plugins/like.php", 33 ) == 0)
      return CONTENTTYPE_HTML;
#endif

   if (strcmp( url, "trk.kissmetrics.com/e" ) == 0  ||
       strcmp( url, "trk.kissmetrics.com/s" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "whos.amung.us/pjswidget", 23 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "secure-us.imrworldwide.com/cgi-bin/m" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "a.collective-media.net/cmadj/", 29 ) == 0)
      return CONTENTTYPE_JAVA;

   if (strcmp( url, "traffic.shareaholic.com/e" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "d.agkn.com/iframe" ) == 0)
      return CONTENTTYPE_HTML;

   if (strncmp( path, "/adsadclient", 12 ) == 0   &&
       (r = strstr( url, ".msn.com/" )) != NULL  &&
       r < path)
      return CONTENTTYPE_TEXT;			/* produces java, xml and html */

   if (strcmp( url, "catalog.video.msn.com/frauddetect.aspx" ) == 0)
      return CONTENTTYPE_TEXT;

   if (strncmp( url, "www.linkedin.com/analytics/", 27 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "dp.33across.com/ps", 18 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "d.agkn.com/pixel!", 17 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "data.aggregateknowledge.com/pixel!", 34 ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".sitemeter.com" )) != NULL  &&
       r < path)
   {
      if (strcmp( path, "/meter.asp" ) == 0)
	 return CONTENTTYPE_IMAGE;
   }

   if ((r = strstr( url, ".terra." )) != NULL  &&
       r < path)
   {
      if (strcmp( path, "/td.asp" ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strcmp( path, "/uv" ) == 0)
	 return CONTENTTYPE_IMAGE;
   }

   if (strncmp( url, "adlog.com.com/adlog/i/", 22 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "pbid.pro-market.net/engine" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "a.tribalfusion.com/j.ad" ) == 0)
      return CONTENTTYPE_JAVA;

   if (strcmp( url, "p.ic.tynt.com/b/p" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( path, "valog.loginside.co.kr/", 22 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "ftjcfx.com/image-", 17 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "lduhtrp.net/image-", 18 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strstr( url, "img.pheedo.com/img.phdo" ) != NULL)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "an.yandex.ru/count/", 19 ) == 0)
      return CONTENTTYPE_TEXT;
   if (strncmp( url, "bs.yandex.ru/informer/", 21 ) == 0)
      return CONTENTTYPE_IMAGE;
   if (strncmp( url, "mc.yandex.ru/watch/", 19 ) == 0)			/* can be json and image */
      return CONTENTTYPE_TEXT;
   if (strncmp( url, "mc.yandex.ru/clmap/", 19 ) == 0)			/* tracker */
      return CONTENTTYPE_TEXT;

   if (strncmp( url, "counter.yadro.ru/hit", 20 ) == 0)			/* it was HTML but it now is GIF/IMAGE */
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "b.kavanga.ru/exp", 16 ) == 0)
      return CONTENTTYPE_JAVA;
   if (strstr( url, ".api.playtomic.com/tracker/q.aspx" ) != NULL)         /* 204 no content */
      return CONTENTTYPE_HTML;
   if (strncmp( url, "an.yandex.ru/code/", 16 ) == 0)
      return CONTENTTYPE_JAVA;
   if (strncmp( url, "zero.kz/c.php", 13 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( path, "/realmedia/ads/" ) != NULL)
   {
      if (strstr( path, "/adstream_jx" ) != NULL  ||
          strstr( path, "/adstream_mjx" ) != NULL)
	 return CONTENTTYPE_JAVA;

      if (strstr( path, "/adstream_lx" ) != NULL  ||
          strstr( path, "/adstream_nx" ) != NULL)
         return CONTENTTYPE_IMAGE;

      if (strstr( path, "/ads/cap.cgi" ) != NULL)		/* /adstream.cap */
         return CONTENTTYPE_IMAGE;
   }

   if (strstr( url, "v.fwmrm.net" ) != NULL)
   {
      if (strcmp( path, "/ad/p/1" ) == 0)
	 return CONTENTTYPE_XML;
      if (strcmp( path, "/ad/l/1" ) == 0)
	 return CONTENTTYPE_HTML;
      if (strcmp( path, "/ad/g/1" ) == 0)
	 return CONTENTTYPE_JAVA;
   }

   if (strstr( url, "overture.com/ls_js_" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strstr( path, "/scripts/beacon.dll" ) != NULL  ||
       strstr( path, "/scripts/beacon2.dll" ) != NULL)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "trk.vindicosuite.com/tracking/v3/instream/impression", 52 ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, ".smartadserver.com/a/diff" )) != NULL  &&
       r < path)
      return CONTENTTYPE_JAVA;
   if ((r = strstr( url, ".smartadserver.com/def/" )) != NULL  &&
       r < path)
      return CONTENTTYPE_JAVA;

   if (strcmp( path, "/hit.xiti" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( path, "/javascript/" ) != NULL  ||  
       strstr( path, "/ajaxpro/" ) != NULL)
      return CONTENTTYPE_JAVA;

   r = strstr( path, ".js.php" );
   if (r != NULL  &&  strlen(r) == 7)
      return CONTENTTYPE_JAVA;
   r = strstr( path, "/js.php" );
   if (r != NULL  &&  strlen(r) == 7)
      return CONTENTTYPE_JAVA;
   r = strstr( path, "/javascript.php" );
   if (r != NULL  &&  strlen(r) == 15)
      return CONTENTTYPE_JAVA;

   r = strstr( path, "/css.php" );
   if (r != NULL  &&  strlen(r) == 8)
      return CONTENTTYPE_CSS;

   r = strstr( path, "/image.php" );
   if (r != NULL  &&  strlen(r) == 10)
      return CONTENTTYPE_IMAGE;
   r = strstr( path, "/pic.php" );
   if (r != NULL  &&  strlen(r) == 8)
      return CONTENTTYPE_IMAGE;
   if (strstr( path, "/image.php/" ) != NULL)
      return CONTENTTYPE_IMAGE;
   if (strstr( path, "/image.svc/" ) != NULL)
      return CONTENTTYPE_IMAGE;

   r = strstr( path, "/java.php" );
   if (r != NULL  &&  strlen(r) == 9)
      return CONTENTTYPE_JAVA;

   if (strstr( path, "/js.ng/" ) != NULL  ||
       strstr( path, "/js/" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strstr( path, "/scripts/" ) != NULL  ||
       strstr( path, "/script/" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "xml.", 4 ) == 0)
      return CONTENTTYPE_XML;

   /* SiteCatalyst beacon */
   if (strncmp( path, "/b/ss/", 6 ) == 0  &&
       (strstr( path, "/fas" ) != NULL  ||
        strstr( path, "/h." ) != NULL   ||
	strstr( path, "/g." ) != NULL ))
      return CONTENTTYPE_IMAGE;

   if (strcmp( path, "/csi" ) == 0  &&  
       (strstr( url, "google." ) != NULL  ||  strstr( url, "gstatic.com" ) != NULL))
      return CONTENTTYPE_IMAGE;

   if (strcmp( path, "/www/delivery/ajs.php" ) == 0  ||
       strcmp( path, "/www/delivery/spcjs.php" ) == 0  ||
       strcmp( path, "/openx/www/delivery/ajs.php" ) == 0)
      return CONTENTTYPE_JAVA;
   if (strcmp( path, "/www/delivery/lg.php" ) == 0  ||
       strcmp( path, "/openx/www/delivery/lg.php" ) == 0  ||
       strcmp( path, "/jump/www/delivery/lg.php" ) == 0)
      return CONTENTTYPE_IMAGE;
   if ((r = strstr( url, "openx." )) != NULL  &&  r < path)
   {
      if (strcmp( path, "/jstag" ) == 0)
	 return CONTENTTYPE_JAVA;
      if (strcmp( path, "/ajs.php" ) == 0)
	 return CONTENTTYPE_JAVA;
      if (strcmp( path, "/ag.php" ) == 0)
	 return CONTENTTYPE_JAVA;
      if (strcmp( path, "/lg.php" ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strcmp( path, "/img" ) == 0)
	 return CONTENTTYPE_IMAGE;
      if (strcmp( path, "/json" ) == 0)
	 return CONTENTTYPE_JSON;
      if (strcmp( path, "/set" ) == 0)
	 return CONTENTTYPE_IMAGE;
      /* afr.php is HTML */
      /* http://us-ads.openx.net/w/1.0/afr is HTML */
   }

   if (strcmp( url, "x.bidswitch.net/ul_cb/sync" ) == 0  ||
      strcmp( url, "x.bidswitch.net/sync" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "metrics.brightcove.com/tracker" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "tl.r7ls.net/fl/" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "eniro.tns-cs.net/j0", 19 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "www.stumbleupon.com/hostedbadge.php" ) == 0)
      return CONTENTTYPE_JAVA;
   if (strcmp( url, "www.stumbleupon.com/services/1.1/badge.getinfo" ) == 0)
      return CONTENTTYPE_JSON;

   if (strcmp( url, "www.facebook.com/brandlift.php" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( url, ".channel.facebook.com/x/" ) != NULL)
      return CONTENTTYPE_JSON;
   if (strstr( url, ".channel.facebook.com/p" ) != NULL)
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "maps.google.com/maps/ms" ) == 0)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "stats.jtvnw.net/", 16 ) == 0  && path[1] >= '0' && path[1] <= '9')
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "dev.visualwebsiteoptimizer.com/j.php", 36 ) == 0)
      return CONTENTTYPE_JAVA;

   if (strcmp( url, "sendgrid.me/wf/open" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "hitserver.ibope.com.br/b" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "data.coremetrics.com/eluminate" ) == 0)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, "tt.omtrdc.net" )) != NULL  &&
       r < path  &&
       strstr( path, "/mbox" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strstr( url, "emediate.eu/eas" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strstr( path, "/xml-rpc" ) != NULL)
      return CONTENTTYPE_XML;

   if (strstr( path, "/xml/" ) != NULL)
      return CONTENTTYPE_XML;

   if (strcmp( path, "/open/1" ) == 0)
      return CONTENTTYPE_STREAM;

#if 0
   if (strstr( path, "/video/" ) != NULL)
      return CONTENTTYPE_STREAM;
#endif

   /* NOTE: http://pixel.mathtag.com/event/js?mt_id=...  is  JAVA !!! */

   if (strstr( path, "/jserver/" ) != NULL  ||
       strstr( path, "/js/" ) != NULL ||
       ((r = strstr( path, "/js")) != NULL  &&  strlen(r) == 3) )
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "pixel.", 6 ) == 0  ||
       strncmp( url, "pix.", 4 ) == 0)
      return CONTENTTYPE_IMAGE;

   r = strstr( path, "/pixel" );
   if (r != NULL  &&  strlen(r) == 6)
      return CONTENTTYPE_IMAGE;

   if (strcmp( path, "/stats/beacon" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "vk.com/videostats.php" ) == 0)
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "www.adhood.com/adserver/ad.php" ) == 0)
      return CONTENTTYPE_JAVA;

   if (strcmp( url, "t.beanstalkdata.com/webvisit" ) == 0  ||
       strcmp( url, "proc.beanstalkdata.com/mongo/track.php" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( url, "hit.deckstats.com/es/s/ctrl" ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( path, "/j0=", 4 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( url, "images.", 7 ) == 0  ||
       strncmp( url, "image.", 6 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( url, "webtrekk" ) != NULL  &&
       (strstr( path, "/wt" ) != NULL ||
        strstr( path, "/hm" ) != NULL ||
	strstr( path, "/ce" ) != NULL))
      return CONTENTTYPE_IMAGE;

   if (strcmp( path, "/json-proxy" ) == 0  ||  strcmp( path, "/json/" ) == 0)
      return CONTENTTYPE_JSON;

   if (strstr( path, "/jsonp" ) != NULL)
      return CONTENTTYPE_JSON;

   if (strstr( path, "/piwik.php" ) != NULL)
      return CONTENTTYPE_IMAGE;

   if ((r = strstr( url, "cedexis" )) != NULL  &&  r < path  &&  strlen(path) > 6)
      return CONTENTTYPE_JAVA;

   if (strncmp( path, "/cgi-bin/ivw/cp/", 16 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( path, "/js_" ) != NULL)
      return CONTENTTYPE_JAVA;

   if (strncmp( url, "img.", 4 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strncmp( path, "/imp", 4 ) == 0)
      return CONTENTTYPE_IMAGE;

   if (strstr( path, "/track" ) != NULL)		/* trackers can be image/java etc. type=text is safe because it is empty */
      return CONTENTTYPE_TEXT;

   if (strstr( url, ".cnzz.com/stat.htm" ) != NULL)	/* usually a gif but an emtpy doc is safer */
      return CONTENTTYPE_TEXT;

   if (strncmp( url, "log.", 4 ) == 0  ||
       strncmp( url, "track", 5 ) == 0)			/* trackers can be image/java etc. type=text is safe because it is empty */
      return CONTENTTYPE_TEXT;

   if (strcmp( path, "/adj" ) == 0)
      return CONTENTTYPE_JAVA;

   if (strlen( path ) > 200  &&  
       (strstr( url, "log" ) != NULL  ||		/* might be a tracker */
        strncmp( url, "stat", 4 ) == 0))
      return CONTENTTYPE_TEXT;

   if (strcmp( url, "tm.scribit.com/vat/visitt") == 0)
      return CONTENTTYPE_IMAGE;

   if (strcmp( path, "/log" ) == 0  ||  
       strcmp( path, "/stat.php" ) == 0  ||
       strcmp( path, "/hit.php" ) == 0  ||
       strstr( path, "/tracker" ) != NULL  ||
       strstr( url, "counter" ) != NULL  ||
       strstr( path, "/ping" ) != NULL)			/* may be an image or other type, but TEXT is safe for all */
      return CONTENTTYPE_TEXT;

   if (strstr( url, "/owa/log" ) != NULL)
      return CONTENTTYPE_TEXT;				/* may be an image or other type, but TEXT is safe for all */

   if (strstr( path, "/g.pixel" ) != NULL)
      return CONTENTTYPE_IMAGE;

   if (strstr( path, "/adlog" ) != NULL)		/* may be pixel or something else, TEXT is safe for all */
      return CONTENTTYPE_TEXT;

   if (strstr( path, "/event" ) != NULL)		/* may be pixel or something else, TEXT is safe for all */
      return CONTENTTYPE_TEXT;

   if (strcmp( path, "/gen_204" ) == 0)			/* most likely has 204 page */
      return CONTENTTYPE_204;

   if (strlen( path ) <= 3)				/* might be a beacon image, java or other type, but TEXT is safe for all */
      return CONTENTTYPE_TEXT;

   if (strncmp( path, "/b/", 3 ) == 0)			/* most likely a beacon image; but use text to be safer */
      return CONTENTTYPE_TEXT;

   if (strncmp( url, "analytic", 8 ) == 0)		/* image or other type, TEXT is safe */
      return CONTENTTYPE_TEXT;

   if (strstr( path, "api/event" ) != NULL)		/* image or other type, TEXT is safe */
      return CONTENTTYPE_TEXT;

   if (strstr( url, "heatmap" ) != NULL)		/* json, image or other type, TEXT is safe */
      return CONTENTTYPE_TEXT;

   return default_type;
}


static void AnswerHttpUrlBlocked( 
   int       fd, 
   int       lang, 
   char *    parameters, 
   const char *  imagesDirectory )
{
   char *    p;
   char *    suffix;
   char *    pptr;
   time_t    now_t;
   struct tm t;
   char *    mode = "default";
   char *    color = "orange";
   char *    size = "normal";
   char *    httpcode = NULL;
   char      clientaddr[512] = "";
   char      clientname[256] = "";
   char      clientuser[256] = "";
   char      source[256] = "";
   char      category[256] = "unknown";
   char *    textcolor = "white";
   char *    bgcolor = "black";
   char *    titlesize = "+0";
   char *    textsize = "+0";
   int       contentLength;
   int       contentType;
   int       headerLength;
   char      header[2048];
   char      admin[2048];
   char      url[8192];
   char      whyblocked[8192];
   char      text[8192];
   char      content[8192+8192+1200];
   char      moreInfoParams[8192+1200];

#if 0
   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "AnswerHttpUrlBlocked: %d %%s %s", lang, parameters, imagesDirectory );
#endif

   if (*parameters == '&')  /* Apache mod_rewrite adds an '&' */
      parameters++;
   suffix = NULL;
   content[0] = '\0';
   contentLength = 0;
   strcpy( admin, "The network administrator" );
   strcpy( url, "unknown" );

   /*
    * valid parameters are:
    * admin
    * mode
    * color
    * size
    * httpcode
    * clientaddr 
    * clientname
    * clientuser/clientident
    * clientgroup/source
    * targetgroup/category
    * url
    */
   pptr = NULL;
   p = strtok_r( parameters, "&", &pptr );
   while (p != NULL)
   {
      char * value;
      
      value = strchr( p, '=' );
      if (value == NULL)
      {
         ufdbLogError( "AnswerHttpUrlBlocked: parameter %s has no value", p );
	 p = strtok_r( NULL, "&", &pptr );
	 continue;
      }
      *value = '\0';
      value++;
      if (strcmp( p, "admin" ) == 0)
         decodeURIvalue( admin, value );
      else if (strcmp( p, "mode" ) == 0)
         mode = value;
      else if (strcmp( p, "color" ) == 0  ||  strcmp( p, "colour" ) == 0)
         color = value;
      else if (strcmp( p, "size" ) == 0)
         size = value;
      else if (strcmp( p, "httpcode" ) == 0)
         httpcode = value;
      else if (strcmp( p, "clientaddr" ) == 0)
         decodeURIvalue( clientaddr, value );
      else if (strcmp( p, "clientname" ) == 0)
         decodeURIvalue( clientname, value );
      else if (strcmp( p, "clientuser" ) == 0  ||  strcmp( p, "clientident" ) == 0)
         decodeURIvalue( clientuser, value );
      else if (strcmp( p, "clientgroup" ) == 0  ||  strcmp( p, "source" ) == 0  ||  strcmp( p, "srcclass" ) == 0)
         decodeURIvalue( source, value );
      else if (strcmp( p, "category" ) == 0  ||  strcmp( p, "targetgroup" ) == 0  ||  strcmp( p, "targetclass" ) == 0)
         decodeURIvalue( category, value );
      else if (strcmp( p, "url" ) == 0)
      {
         decodeURIvalue( url, value );

	 /* strip the URL */
	 p = strchr( url, '?' );
	 if (p != NULL)
	    *p = '\0';
	 else
	 {
	    p = strchr( url, ';' );
	    if (p != NULL)
	       *p = '\0';
	    else
	    {
	       p = strchr( url, '&' );
	       if (p != NULL)
		  *p = '\0';
	    }
	 }

#if 0
	 if (UFDBglobalDebug || UFDBglobalDebugHttpd)
	    ufdbLogMessage( "   blocked url: %s", url );
#endif

	 /* url is the last parameter so stop parsing */
	 break;
      }
      else 
         ufdbLogError( "AnswerHttpUrlBlocked: unknown parameter '%s'", p );

      p = strtok_r( NULL, "&", &pptr );
   }

   content[0] = '\0';

   p = strstr( url, "://" );
   if (p == NULL)
      pptr = strchr( url, '/' );
   else
      pptr = strchr( p+3, '/' );

   if (pptr == NULL)		/* no URL path... */
   {
      contentType = guessContentType( url, CONTENTTYPE_HTML );
      p = NULL;
   }
   else
   {
      p = strrchr( pptr, '.' );
      if (p == NULL  || strlen(p) > 6)		/* URL path has no suffix */
      {
	 p = NULL;
	 contentType = guessContentType( url, CONTENTTYPE_HTML );
      }
      else
      {
	 char * ch;

	 p++;
	 ch = p;
	 while (*ch != '\0')
	 {
	    if (isupper(*ch))
	       *ch = tolower(*ch);
	    ch++;
	 }
	 suffix = p;

	 if (strmatch3( p, "bmp" )  ||
	     strmatch3( p, "gif" )  ||
	     strmatch3( p, "ico" )  ||
	     strmatch3( p, "img" )  ||
	     strmatch3( p, "jpg" )  ||
	     strmatch3( p, "jpe" )  ||
	     strmatch4( p, "jpeg" )  ||
	     strmatch3( p, "png" )  ||
	     strmatch4( p, "webp" )  ||
	     strmatch4( p, "tiff" ))
	 {
	    contentType = CONTENTTYPE_IMAGE;
	 }
	 else if (strmatch3( p, "css" ))
	 {
	    contentType = CONTENTTYPE_CSS;
	 }
	 else if (strmatch2( p, "js" )  ||
		  strmatch3( p, "jar" ))
	 {
	    contentType = CONTENTTYPE_JAVA;
	 }
	 else if (strmatch3( p, "xml" )  ||
		  strmatch4( p, "sxml" )  ||
		  strmatch3( p, "rss" ))
	 {
	    contentType = CONTENTTYPE_XML;
	 }
	 else if (strmatch3( p, "asx" )   ||
	          strmatch3( p, "cab" )   ||
		  strmatch5( p, "class" )  ||
		  strmatch4( p, "divx" )  ||
		  strmatch4( p, "h264" )  ||
		  strmatch3( p, "mpg" )  ||
		  strmatch4( p, "mpeg" )  ||
		  strmatch3( p, "ogv" )  ||
		  strmatch2( p, "qt" )  ||
		  strmatch2( p, "ra" )  ||
		  strmatch3( p, "ram" )  ||
		  strmatch2( p, "rv" )  ||
		  strmatch3( p, "wmv" )  ||
		  strmatch3( p, "avi" )  ||
		  strmatch3( p, "mov" )  ||
		  strmatch3( p, "swf" )  ||
		  strmatch3( p, "mp4" )  ||
		  strmatch3( p, "m4v" )  ||
		  strmatch3( p, "flv" )  ||
		  strmatch3( p, "f4v" )  ||
		  strmatch3( p, "bz2" )   ||
		  strmatch3( p, "dat" )  ||
		  strmatch3( p, "doc" )  ||
		  strmatch2( p, "xz" )   ||
		  strmatch2( p, "gz" )   ||
		  strmatch3( p, "mp3" )  ||
		  strmatch3( p, "msi" )  ||
		  strmatch3( p, "mst" )  ||
		  strmatch3( p, "ppt" )  ||
		  strmatch3( p, "pdf" )  ||
		  strmatch3( p, "rar" )  ||
		  strmatch3( p, "tar" )  ||
		  strmatch3( p, "ttf" )  ||
		  strmatch3( p, "xls" )  ||
		  strmatch3( p, "zip" )   ||
		  strmatch4( p, "woff" )   ||
		  strmatch5( p, "woff2" )   ||
		  strmatch3( p, "bin" ))
	 {
	    contentType = CONTENTTYPE_STREAM;
	 }
	 else if (strmatch3( p, "txt" )  ||
		  strmatch3( p, "csv" ))
	 {
	    contentType = CONTENTTYPE_TEXT;
	 }
	 else if (strmatch4( p, "json" ))
	 {
	    contentType = CONTENTTYPE_JSON;
	 }
	 else if (strmatch3( p, "htm" )  ||
		  strmatch5( p, "shtml" )  ||
		  strmatch5( p, "dhtml" )  ||
		  strmatch4( p, "html" ))
	 {
	    contentType = CONTENTTYPE_HTML;
	 }
	 else  /* there is an unknown suffix */
	 {
	    contentType = guessContentType( url, CONTENTTYPE_HTML );
	 }
      }
   }

   /*  transparent  grey  ads  xml  The network administrator  http://adnet.bluebillywig.com/crossdomain.xml  */
   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
      ufdbLogMessage( "   AnswerHttpUrlBlocked:  mode %s  color %s  category %s  content-type %s  p \"%s\"  admin \"%s\"  httpcode \"%s\"  url %s", 
                      mode, color, category, contentType2String(contentType), 
		      p == NULL ? "" : p,  admin, 
		      httpcode == NULL ? "-" : httpcode,  url );

   if (strcmp( color, "orange" ) == 0)
   {
      textcolor = "white";
      bgcolor = "#ee8811";
   }
   else if (strcmp( color, "white" ) == 0)
   {
      textcolor = "#3f003f";
      bgcolor = "white";
   }
   else if (strcmp( color, "black" ) == 0)
   {
      textcolor = "#f0f0f0";
      bgcolor = "black";
   }
   else if (strcmp( color, "red" ) == 0)
   {
      textcolor = "#f0f0f0";
      bgcolor = "red";
   }
   else if (strcmp( color, "grey" ) == 0  ||  strcmp( color, "gray" ) == 0)
   {
      textcolor = "#111111";
      bgcolor = "#c2c2c2";
   }
   else
   {
      textcolor = "white";
      bgcolor = "#ee8811";
   }

   if (strcmp( size, "normal" ) == 0)
   {
      titlesize = "+2";
      textsize = "+0";
   }
   else if (strcmp( size, "small" ) == 0)
   {
      titlesize = "+1";
      textsize = "-1";
   }
   else if (strcmp( size, "large" ) == 0)
   {
      titlesize = "+3";
      textsize = "+1";
   }
   else
   {
      titlesize = "+2";
      textsize = "+0";
   }

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   sprintf( moreInfoParams, "ufdbcat=%s&ufdbsrc=%s&ufdburl=%s", category, source, url );

   if (httpcode != NULL  &&  strcmp( httpcode, "204" ) == 0)
      contentType = CONTENTTYPE_204;

   /* Check for graphics and send a bitmap.  */
   if (contentType == CONTENTTYPE_IMAGE)
   {
      int  ifd;
      char file[1024];

      /* Send an image for URLs that have one of the following suffixes:
       * bmp, gif, jpg, jpeg, png, ico.
       * with special png file for the category ads.
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: image/png\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );
      if (strcmp( category, "ads" ) == 0  ||
          strcmp( category, "always-block" ) == 0  ||
	  strcmp( category, "alwaysblock" ) == 0 )
      {
         if (strcmp( mode, "noads" ) == 0)
	    p = "no-ads.png";
	 else if (strcmp( mode, "cross" ) == 0)
	    p = "smallcross.png";
	 else if (strcmp( mode, "square" ) == 0)
	    p = "square.png";
         else if (strcmp( mode, "simple-red" ) == 0)
	    p = "transparent.png";
	 else  /* transparent */
	    p = "transparent.png";
	 sprintf( file, "%s/%s", imagesDirectory, p );
	 ifd = open( file, O_RDONLY );
	 if (ifd < 0)
	 {
	    ufdbLogError( "cannot open image file %s: %s *****", file, strerror(errno) );
	    contentLength = 0;
	 }
	 else
	 {
	    contentLength = read( ifd, content, sizeof(content)-1 );
	    if (contentLength < 0)
	       ufdbLogError( "cannot read image file %s: %s *****", file, strerror(errno) );
	    close( ifd );
	 }
      }
      else   /* send bitmap; category is NOT "ads" */
      {
	 char * langStr;

	 switch (lang)
	 {
	    case LANG_IND_NL:	langStr = "nl";  break;
	    case LANG_IND_DE:	langStr = "de";  break;
	    case LANG_IND_PL:	langStr = "pl";  break;
	    case LANG_IND_IT:	langStr = "it";  break;
	    case LANG_IND_ES:	langStr = "es";  break;
	    case LANG_IND_PT:	langStr = "pt";  break;
	    case LANG_IND_FR:	langStr = "fr";  break;
	    case LANG_IND_TR:	langStr = "tr";  break;
	    case LANG_IND_SV:	langStr = "sv";  break;
	    case LANG_IND_EN:
	    default: 		langStr = "en";
	 }
	 if (strcmp( mode, "cross" ) == 0)
	    sprintf( file, "%s/smallcross.png", imagesDirectory );
	 else if (strcmp( mode, "square" ) == 0)
	    sprintf( file, "%s/square.png", imagesDirectory );
         else if (strcmp( mode, "simple-red" ) == 0)
	    sprintf( file, "%s/transparent.png", imagesDirectory );
	 else if (strcmp( mode, "transparent" ) == 0  ||  strcmp( mode, "transparant" ) == 0)
	    sprintf( file, "%s/transparent.png", imagesDirectory );
	 else
	    sprintf( file, "%s/forbidden-normal-%s.png", imagesDirectory, langStr );
	 ifd = open( file, O_RDONLY );
	 if (ifd < 0)
	 {
	    ufdbLogError( "cannot open image file %s: %s *****", file, strerror(errno) );
	    contentLength = 0;
	 }
	 else
	 {
	    contentLength = read( ifd, content, sizeof(content)-1 );
	    if (contentLength < 0)
	       ufdbLogError( "cannot read image file %s: %s *****", file, strerror(errno) );
	    close( ifd );
	 }
      }
   }
   else if (contentType == CONTENTTYPE_JAVA)
   {
      /*
       * send empty reply for URLs that have one of the following suffixes:
       * js, jar
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: text/javascript\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );
      content[0] = '\0';
      contentLength = 0;
   }
   else if (contentType == CONTENTTYPE_STREAM)
   {
      char * mimeType;

      mimeType = "application/octet-stream";
      content[0] = '\0';
      contentLength = 0;

      if (UFDBglobalDebug)
         ufdbLogMessage( "blocking stream for %s: suffix: \"%s\" ", url, suffix==NULL?"null":suffix );

      if (suffix != NULL)
      {
	 if (defaultFLVsize > 0  &&  
	     (strmatch3( suffix, "flv" ) ||
	      strmatch3( suffix, "f4v" ) ||
	      strmatch3( suffix, "f4a" ) ||
	      strmatch3( suffix, "f4b" ) ||
	      strmatch3( suffix, "f4p" ) ||
	      strmatch3( suffix, "swf" )))
	 {
	    mimeType = "video/x-flv";
	    memcpy( content, defaultFLVcontent, defaultFLVsize );
	    contentLength = defaultFLVsize;
	 }
	 else
	 if (defaultMP3size > 0  &&  
	     (strmatch3( suffix, "mp3" )  ||
	      strmatch3( suffix, "mp2" )))
	 {
	    mimeType = "audio/mp3";
	    memcpy( content, defaultMP3content, defaultMP3size );
	    contentLength = defaultMP3size;
	 }
	 else
	 if (defaultMP3size > 0  &&  
	     (strmatch3( suffix, "mp4" )  ||
	      strmatch4( suffix, "mpeg" )  ||
	      strmatch3( suffix, "mpe" )  ||
	      strmatch3( suffix, "mpg" )  ||
	      strmatch3( suffix, "mpa" )  ||
	      strmatch4( suffix, "mpg4" )))
	 {
	    mimeType = "video/mpeg";
	    memcpy( content, defaultMPGcontent, defaultMPGsize );
	    contentLength = defaultMPGsize;
	 }
	 else
	 if (defaultWMVsize > 0  &&  
	     (strmatch3( suffix, "wmv" )  ||
	      strmatch3( suffix, "asf" )  ||
	      strmatch3( suffix, "wvx" )))
	 {
	    mimeType = "video/x-ms-wmv";
	    memcpy( content, defaultWMVcontent, defaultWMVsize );
	    contentLength = defaultWMVsize;
	 }
      }

      /*
       * send empty reply for URLs that have one of the following suffixes:
       * rar, tar, zip, ...
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: %s\r\n"
	       "Content-Length: %d\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec,
	       mimeType,
	       contentLength
	       );
   }
   else if (contentType == CONTENTTYPE_JSON)
   {
      /*
       * send empty reply for URLs that have one of the following suffixes:
       * json
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: application/json\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );
      content[0] = '\0';
      contentLength = 0;
   }
   else if (contentType == CONTENTTYPE_CSS)
   {
      /*
       * send empty reply for URLs that have one of the following suffixes:
       * css
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: text/css\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );
      strcpy( content, "\r\n" );
      contentLength = 2;
   }
   else if (contentType == CONTENTTYPE_TEXT)
   {
      /*
       * send empty reply for URLs that have one of the following suffixes:
       * txt
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: text/plain\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );
      content[0] = '\0';
      contentLength = 0;
   }
   else if (contentType == CONTENTTYPE_XML)
   {
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Content-Type: text/xml\r\n"
	       "Connection: close\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );

      if (strstr( url, "/crossdomain.xml" ) != NULL  ||
          strstr( url, "/clientaccesspolicy.xml" ) != NULL)
      {
	 strcpy( content, "<?xml version=\"1.0\"?>\r\n"
			  "<cross-domain-policy>\r\n"
			  "   <allow-access-from domain=\"*\" secure=\"false\" />\r\n"
			  "   <allow-http-request-headers-from domain=\"*\" headers=\"*\" secure=\"false\"/>\r\n"
			  "   <site-control permitted-cross-domain-policies=\"all\"/>\r\n"
			  "</cross-domain-policy>\r\n"  );
      }
      else
      {
	 strcpy( content, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
			  "<ufdbguardd>\r\n"
			  "   <dummy value=\"0\" />\r\n"
			  "</ufdbguardd>\r\n"  );
      }
      contentLength = strlen( content );
   }
   else if (contentType == CONTENTTYPE_204)
   {
      /*
       * send a HTML 204 message
       */
      headerLength = sprintf( header, 
	       "HTTP/1.0 204 No Content\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: text/plain\r\n"
	       "X-blocked-category: %s\r\n"
	       "X-blocked-URL: %s\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec,
	       category,
	       url
	       );
      content[0] = '\0';
      contentLength = 0;
   }
   else		/**********************************************  contentType == CONTENTTYPE_HTML  */
   {
      headerLength = sprintf( header, 
	       "HTTP/1.0 200 OK\r\n"
	       "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
	       "Server: ufdbhttpd/" UFDB_VERSION "\r\n"
	       "Cache-Control: max-age=180\r\n"
	       "Connection: close\r\n"
	       "Content-Type: text/html\r\n"
	       "\r\n",
	       &"SunMonTueWedThuFriSat"[t.tm_wday*3],
	       t.tm_mday,
	       &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
	       t.tm_year + 1900,
	       t.tm_hour, t.tm_min, t.tm_sec
	       );

      if (strcmp( category, "fatal-error" ) == 0)		/* HTML, fatal-error */
      {
	 strcpy( text, _fatal_error_text );
	 contentLength = sprintf( content,
		  "<html>\r\n"
		  "<head>\r\n"
		  "<title>%s</title>\r\n"
		  "</head>\r\n"
		  "<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"%s\" text=\"%s\">\r\n"
		  "%s\r\n"
		  "</body>\r\n"
		  HTML_COMMENT
		  "</html>\r\n"
		  "<!-- lang:%d %s %s %s %s %s -->\r\n" 
		  ,
		  _title[lang],
		  bgcolor, textcolor,
		  text,
		  lang, color, mode, category, contentType2String(contentType), url );
      }
      else if (strcmp( category, "loading-database" ) == 0)	/* HTML, loading-database */
      {
	 strcpy( text, _loading_database_text );
	 contentLength = sprintf( content,
		  "<html>\r\n"
		  "<head>\r\n"
		  "<title>%s</title>\r\n"
		  "</head>\r\n"
		  "<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"%s\" text=\"%s\">\r\n"
		  "%s\r\n"
		  "</body>\r\n"
		  HTML_COMMENT
		  "</html>\r\n"
		  "<!-- lang:%d %s %s %s %s %s -->\r\n"
		  ,
		  _title[lang],
		  bgcolor, textcolor,
		  text,
		  lang, color, mode, category, contentType2String(contentType), url );
      }
      else							/* HTML */
      {
	 sprintf( whyblocked, "blocked by ufdbGuard: url=%s", url );
	 if (category[0] != '\0')
	 {
	    strcat( whyblocked, " category=" );
	    strcat( whyblocked, category );
	 }
	 if (source[0] != '\0')
	 {
	    strcat( whyblocked, " source=" );
	    strcat( whyblocked, source );
	 }

	 /*
	  * send ads-specific reply for the ads category.
	  */
	 if (strcmp( category, "ads" ) == 0  ||			/* HTML, ADS */
	     strcmp( category, "always-block" ) == 0  ||
	     strcmp( category, "alwaysblock" ) == 0 )
	 {
	    if (strcmp( mode, "noads" ) == 0)
	       sprintf( text, " <a title=\"%s\" target=\"_blank\">no ads</a> ", whyblocked );
	    else if (strcmp( mode, "square" ) == 0)
	       sprintf( text, " <a title=\"%s\" target=\"_blank\">[]</a> ", whyblocked );
	    else if (strcmp( mode, "cross" ) == 0)
	       sprintf( text, " <a title=\"%s\" target=\"_blank\">x</a> ", whyblocked );
	    else if (strcmp( mode, "simple-red" ) == 0)
	       sprintf( text, " <font color=red><i><a title=\"%s\" target=\"_blank\">%s</a></i></font> ", whyblocked, category );
	    else  if (strcmp( mode, "transparent" ) == 0  ||  strcmp( mode, "transparant" ) == 0)
	       strcpy( text, " " );
	    else  /* default */
	    {
	       sprintf( text, " <font color=\"%s\"><i><a title=\"%s\" target=\"_blank\">%s %s%s</a></i></font> ", 
	                textcolor, whyblocked, _explain_1[lang], category, _explain_2[lang] );
	    }

	    if (strcmp( mode, "transparent" ) == 0  ||  strcmp( mode, "transparant" ) == 0)
	    {
	       /* no bgcolor and font color */
	       contentLength = sprintf( content,
			"<html>\r\n"
			"<head>\r\n"
			"<title>%s</title>\r\n"
			"</head>\r\n"
			"<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1>\r\n"
			"<center>%s</center>\r\n"
			HTML_COMMENT
			"</body>\r\n"
			"</html>\r\n"
			"<!-- lang:%d %s %s %s %s %s -->\r\n"
			,
			_title[lang],
			text,
			lang, color, mode, category, contentType2String(contentType), url );
	    }
	    else					/* HTML, ADS, NOT transparent */
	    {
	       char infotext[1024];

	       if (strcmp( mode, "simple-red" ) == 0)
	       {
	          infotext[0] = '\0';
		  bgcolor = "ffcccc";
		  textcolor = "red";
	       }
	       else
	       {
		  int  n;
		  n = 0;
		  infotext[n] = '\0';
		  if (source[0] != '\0'  &&  strcmp( source, "unknown" ) != 0)
		     n += sprintf( &infotext[n], "source=%s &nbsp; ", source );
		  if (clientuser[0] != '\0'  &&  strcmp( clientuser, "unknown" ) != 0)
		     n += sprintf( &infotext[n], "user=%s &nbsp; ", clientuser );
		  if (clientaddr[0] != '\0'  &&  strcmp( clientaddr, "unknown" ) != 0)
		     n += sprintf( &infotext[n], "client=%s &nbsp; ", clientaddr );
		  if (clientname[0] != '\0')
		     n += sprintf( &infotext[n], "clientname=%s &nbsp; ", clientname );
	       }

	       contentLength = sprintf( content,
			"<html>\r\n"
			"<head>\r\n"
			"<title>%s</title>\r\n"
			"</head>\r\n"
			"<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"%s\" text=\"%s\">\r\n"
			"<center>\r\n"
			"<font size=\"%s\">%s</font>\r\n"
			"<br>\r\n&nbsp;<p />\r\n"
			"<font size=\"-3\">%s</font>\r\n"
			"</center>\r\n"
			"</body>\r\n"
			HTML_COMMENT
			"</html>\r\n"
			"<!-- lang:%d %s %s %s %s %s -->\r\n"
			,
			_title[lang],
			bgcolor, textcolor,
			textsize, text, 
			infotext,
			lang, color, mode, category, contentType2String(contentType), url );
	    }
	 }
	 else if (strcmp( category, "social-badges" ) == 0  || 		/* HTML, SOCIAL BADGE */
	          strcmp( category, "social_badges" ) == 0)
	 {
	    contentLength = sprintf( content,
		     "<html>\r\n"
		     "<head>\r\n"
		     "<title>block social networking badge</title>\r\n"
		     "</head>\r\n"
		     "<body bgcolor=\"#fafafa\">\r\n"
		     "<center>\r\n"
		     "<font size=\"-1\" color=\"#1f1f1f\">" 
		     "<a title=\"The social networking badge is blocked.\"> B </a>"
		     "</font>\r\n"
		     "</center>\r\n"
		     HTML_COMMENT
		     "</html>\r\n"
		     "<!-- lang:%d %s %s %s %s %s -->\r\n"
		     "</body>\r\n"
		     ,
		     lang, color, mode, category, contentType2String(contentType), url );

	 }
	 else								/* HTML, all other categories */
	 {
	    if (strcmp( mode, "transparent" ) == 0  ||  strcmp( mode, "transparant" ) == 0)
	    {
	       contentLength = sprintf( content,
			"<html>\r\n"
			"<head>\r\n"
			"<title>%s</title>\r\n"
			"</head>\r\n"
			"<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1>\r\n"
			"<center>\r\n"
			"<i>%s</i><p />\r\n"
			"<font size=\"%s\">\r\n"
			"%s <i>%s</i>%s <br>\r\n"
			"URL: <tt>%s</tt> <br>\r\n"
			"<br>\r\n"
			"<a href=\"javascript:history.go(-1);\">%s</a>. <br>\r\n"
			"<br>\r\n"
			"%s?%s%s\r\n"
			"</center>\r\n"
			"</font>\r\n"
			"</body>\r\n"
			HTML_COMMENT
			"</html>\r\n"
			"<!-- lang:%d %s %s %s %s %s -->\r\n"
			,
			_title[lang],
			_forbidden[lang],
			textsize,
			_explain_1[lang], category, _explain_2[lang],
			url,
			_goBack[lang],
			_moreInfo1[lang], moreInfoParams, _moreInfo2[lang],
			lang, color, mode, category, contentType2String(contentType), url );
	    }
	    else if (strcmp( mode, "simple-red" ) == 0)		/* HTML, NO ADS, simple-red */
	    {
	       contentLength = sprintf( content,
			"<html>\r\n"
			"<head>\r\n"
			"<title>%s</title>\r\n"
			"</head>\r\n"
			"<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"ffcccc\" link=\"red\" alink=\"red\" vlink=\"red\" text=\"red\">\r\n"
			"<center>\r\n"
			"<a href=\"%s?%s\" title=\"%s\" target=\"_blank\"> "
			   "%s<br>\r\n"
			   "<i>%s</i></a>\r\n"
			"</center>\r\n"
			"</body>\r\n"
			HTML_COMMENT
			"</html>\r\n"
			"<!-- lang:%d %s %s %s %s %s -->\r\n"
			,
			_title[lang],
			UFDB_EXPLAIN_DENY_REASON_URL, moreInfoParams, whyblocked,
			_forbidden[lang], 
			category,
			lang, color, mode, category, contentType2String(contentType), url );
	    }
	    else					/* HTML, NO ADS, MODE NOT transparent/simple-red */
	    {
	       contentLength = sprintf( content,
			"<html>\r\n"
			"<head>\r\n"
			"<title>%s</title>\r\n"
			"</head>\r\n"
			"<body topmargin=1 leftmargin=1 marginheight=1 marginwidth=1 bgcolor=\"%s\" link=\"%s\" alink=\"%s\" vlink=\"%s\" text=\"%s\">\r\n"
			"<center>\r\n"
			"<a href=\"%s?%s\" title=\"%s\" target=\"_blank\"> "
			   "<font size=\"%s\">%s</font></a><p />\r\n"
			"<font size=\"%s\">\r\n"
			"%s <i>%s</i>%s</a><br>\r\n"
			"URL: <tt>%s</tt> <br>\r\n"
			"<p>\r\n"
			"<a title=\"%s\" href=\"javascript:history.go(-1);\">%s</a>. <br>\r\n"
			"%s\r\n"
			"<p>\r\n"
			"%s?%s%s\r\n"
			"</font>\r\n"
			"</center>\r\n"
			"</body>\r\n"
			HTML_COMMENT
			"</html>\r\n"
			"<!-- lang:%d %s %s %s %s %s -->\r\n"
			,
			_title[lang],
			bgcolor, textcolor, textcolor, textcolor, textcolor,			/* body */
			UFDB_EXPLAIN_DENY_REASON_URL, moreInfoParams, whyblocked,
			titlesize, _forbidden[lang],		/* font */
			textsize,				/* font */
			_explain_1[lang], category, _explain_2[lang],
			url,
			whyblocked, _goBack[lang],
			admin,
			_moreInfo1[lang], moreInfoParams, _moreInfo2[lang],
			lang, color, mode, category, contentType2String(contentType), url );
	    }
	 }
      }
   }

#if 0
   if (UFDBglobalDebug)
      ufdbLogMessage( "AnswerHttpUrlBlocked: strlen(header): %d  contentLength: %d", headerLength, contentLength );
#endif

   writeBuffer( fd, header, headerLength );
   if (contentLength > 0)
      writeBuffer( fd, content, contentLength );
}


static int FindLanguageIndex( char * language )
{
   if (strmatch2( language, "en" ))
      return LANG_IND_EN;
   else if (strmatch2( language, "nl" ))
      return LANG_IND_NL;
   else if (strmatch2( language, "de" ))
      return LANG_IND_DE;
   else if (strmatch2( language, "pl" ))
      return LANG_IND_PL;
   else if (strmatch2( language, "it" ))
      return LANG_IND_IT;
   else if (strmatch2( language, "pt" ))
      return LANG_IND_PT;
   else if (strmatch2( language, "fr" ))
      return LANG_IND_FR;
   else if (strmatch2( language, "tr" ))
      return LANG_IND_TR;
   else if (strmatch2( language, "sv" ))
      return LANG_IND_SV;
   else
      return -1;
}


static int FindLanguage( char * headers )
{
   char * l;
   char * end;
   int    ind;
   int    length;
   char   language[64];

   l = strstr( headers, "Accept-Language:" );
   if (l == NULL)
      return LANG_IND_EN;
   l += sizeof("Accept-Language:") - 1;

   while (*l != '\0')
   {
      while (isspace(*l))
	 l++;
      end = l;
      while (isalpha(*end))
         end++;
      length = end - l;
      if (length > 63)
         length = 63;
      strncpy( language, l, length );
      language[length] = '\0';
      ind = FindLanguageIndex( language );
      if (ind >= 0)
         return ind;
      /* try the next language */
      l = end + 1;
      while (!isalpha(*l)  &&  *l != '\0')
      {
         if (*l == '\r'  ||  *l == '\n')
	    return LANG_IND_EN;
         l++;
      }
   }
   return LANG_IND_EN;
}


/* A typical request looks like this:
 *
 * GET /cgi-bin/URLblocked?mode=normal&... HTTP/1.1
 * Host: www.myserver.com
 * Accept: text/html
 * Accept-Encoding: compress
 * Connection: Keep-Alive
 *
 * OR
 *
 * GET /cgi-bin/URLblocked?mode=normal&... HTTP/1.0
 * User-Agent: Wget/1.8.2
 * Accept: text/html
 * Accept-Encoding: compress
 * Connection: Keep-Alive
 *
 */
static void ServeHttpClient( 
   int            fd,
   const char *   imagesDirectory )
{
   int            start, maxbytes;
   int            nbytes;
   int            ntrials;
   time_t	  start_time, now;
   char *         p;
   char *         command;
   char *         reqptr;
   char           request[16384];

   start_time = time( NULL );
   if (UFDBglobalDebug)
      ufdbLogMessage( "ServeHttpClient t %3ld  fd %d", (long) (start_time%1000), fd );

   ntrials = 20;
   start = 0;

try_again:
   errno = 0;
   maxbytes = 16380 - start;
   nbytes = read( fd, &request[start], maxbytes );
   now = time( NULL );
   if (UFDBglobalDebug)
      ufdbLogMessage( "ServeHttpClient t %3ld  fd %d  read %d bytes", (long) (now%1000), fd, nbytes );
   if (nbytes < 0)
   {
      if (now - start_time >= 4)
      {
	 ufdbLogError( "ServeHttpClient: timeout with %d bytes received", start );
	 AnswerHttpTimeout( fd );
	 return;
      }
      if (errno == EINTR  ||  errno == EAGAIN)
         goto try_again;
      else
      {
         ufdbLogError( "ServeHttpClient: unrecoverable error: %s", strerror(errno) );
	 AnswerHttpEmpty( fd, "unknown" );
	 return;
      }
   }
   start += nbytes;
   request[start] = '\0';
   if (strstr( request, "\r\n\r\n" ) == NULL)
   {
      if (--ntrials > 0)
      {
	 if (now - start_time >= 4)
	 {
	    ufdbLogError( "ServeHttpClient: timeout with %d bytes received", start );
	    AnswerHttpTimeout( fd );
	    return;
	 }
	 usleep( 5001 );
	 goto try_again;
      }
      else
      {
         ufdbLogError( "ServeHttpClient: did not get a whole HTTP request within the time limit\n"
         	       "I got: <<%s>>", 
		       request );
	 AnswerHttpEmpty( fd, "unknown" );
	 return;
      }
   }

   if (UFDBglobalDebugHttpd  || UFDBglobalDebug)
      ufdbLogMessage( "ServeHttpClient: new request:\n%s", request );

   /* We got the full header. Now we can parse it and send an answer. */
   p = strtok_r( request, " \t", &reqptr );
   if (p == NULL  ||  
       (strcmp( p, "GET" ) != 0  &&  strcmp( p, "POST" ) != 0  &&  strcmp( p, "HEAD" ) != 0))
   {
      ufdbLogError( "ServeHttpClient: we got an unsupported message that is not a http GET/HEAD/POST but '%s'", 
                    p==NULL ? "NULL" : p );
      AnswerHttpNotFound( fd, p==NULL ? "NULL" : p );
      return;
   }
   command = p;

   p = strtok_r( NULL, "? \t", &reqptr );
   if (p == NULL)
   {
      ufdbLogError( "ServeHttpClient: received GET/HEAD/POST command without URL" );
      AnswerHttpNotFound( fd, "NULL" );
      return;
   }

   /* p points to a URL, 
    * HTTP/1.0 has http://hostname/path
    * HTTP/1.1 has /path
    * and we only want "/path".
    */
   if (strncasecmp( p, "http://", 7 ) == 0)
   {
      char * p7;

      p7 = strchr( p+7, '/' );
      if (p7 == NULL)
      {
	 ufdbLogError( "ServeHttpClient: unsupported URL for GET/POST: %s", p );
	 AnswerHttpEmpty( fd, p );
	 return;
      }
      p = p7;
   }
   else if (strncasecmp( p, "https://", 8 ) == 0)
   {
      char * p8;

      p8 = strchr( p+8, '/' );
      if (p8 == NULL)
      {
	 ufdbLogError( "ServeHttpClient: unsupported URL for GET/POST: %s", p );
	 AnswerHttpEmpty( fd, p );
	 return;
      }
      p = p8;
   }
   else if (strncasecmp( p, "ftp://", 6 ) == 0)
   {
      char * p6;

      p6 = strchr( p+6, '/' );
      if (p6 == NULL)
      {
	 ufdbLogError( "ServeHttpClient: unsupported URL for GET/POST: %s", p );
	 AnswerHttpEmpty( fd, p );
	 return;
      }
      p = p6;
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "ServeHttpClient: command %s  p %s", command, p );

   if (strcmp( command, "HEAD" ) == 0)
   {
      AnswerHttpHead( fd, p );
      return;
   }
   if (strcmp( command, "POST" ) == 0)
   {
      AnswerHttpPost( fd, p );
      return;
   }

   if (strcmp( p, "/cgi-bin/URLblocked.cgi" ) == 0)
   {
      int lang; 

      p = strtok_r( NULL, " \t", &reqptr );
      lang = FindLanguage( reqptr );
      AnswerHttpUrlBlocked( fd, lang, p, imagesDirectory );
   }
   else 
   if (strcmp( p, "/crossdomain.xml" ) == 0  ||  
       strcmp( p, "/clientaccesspolicy.xml" ) == 0)
   {
      AnswerHttpCrossdomain( fd );
   }
   else 
   if (strcmp( p, "/favicon.ico" ) == 0  ||  
       strcmp( p, "/robots.txt" ) == 0)
   {
      AnswerHttpNotFound( fd, p );
   }
   else
   {
      ufdbLogError( "ServeHttpClient: unsupported URL for GET/POST: \"%s\"", p );
      AnswerHttpEmpty( fd, p );
   }
}


static void ServeHttpConnections( 
   int             s, 
   const char *    imagesDirectory )
{
   int             newfd;
   int             n;
   fd_set          fds;
   struct timeval  tv;

   while (1)
   {
      FD_ZERO( &fds );
      FD_SET( s, &fds );
      tv.tv_sec = 0;
      tv.tv_usec = 750000;
      errno = 0;
      /* select() is used to enable signals to be received by this (non-threaded) process */
      n = select( s+1, &fds, (fd_set *) NULL, (fd_set *) NULL, &tv );
#if 0
      ufdbLogMessage( " select returns %d   errno is %d", n, errno );
#endif
      if (n < 0  && errno == EINTR)
      {
         ufdbLogError( "signal received. exiting..." );
	 removeHttpdPidFile();
	 exit( 0 );
      }
      if (n == 0)			/* timeout */
         continue;
      newfd = accept( s, NULL, NULL );
      if (newfd < 0)
      {
         if (errno == EINTR)
	    continue;
	 if (errno == EAGAIN)
	    continue;
	 ufdbLogError( "SimulateHttpServer: \"accept\" returns error: %s", strerror(errno) );
	 continue;
      }

      tv.tv_sec = 3;
      tv.tv_usec = 0;
      setsockopt( newfd, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
      tv.tv_sec = 3;
      tv.tv_usec = 0;
      setsockopt( newfd, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );
#if 0
      int sock_parm;
      sock_parm = 16384;
      setsockopt( newfd, SOL_SOCKET, SO_SNDBUF, (void *) &sock_parm, sizeof(sock_parm) );
      sock_parm = 16384;
      setsockopt( newfd, SOL_SOCKET, SO_RCVBUF, (void *) &sock_parm, sizeof(sock_parm) );
#endif

      ServeHttpClient( newfd, imagesDirectory );
      close( newfd );
   }
}


#ifndef HAVE_INET_ATON

int inet_aton( 
   const char *     cp, 
   struct in_addr * inp )
{
  unsigned int      byte_result = 0;
  unsigned long     result = 0;
  char              c = '.'; 		/* mark c to indicate invalid IP in case length is 0 */
  int               dot_count = 0;

  if (cp == NULL)
    return 0;

  while (*cp != '\0')
  {
    int digit;

    c = *cp++;
    digit = (int) (c - '0');	
    if (digit >= 0  &&  digit <= 9)
    {
      byte_result = byte_result * 10 + digit;
      if (byte_result > 255)
	return 0;
    }
    else if (c == '.')
    {
      dot_count++;
      result = (result << 8) + (unsigned long) byte_result;
      byte_result = 0;
    }
    else
      return 0;
  }

  if (c != '.')					/* IP number can't end on '.' */
  {
    /*
      Handle short-forms addresses according to standard. Examples:
      127		-> 0.0.0.127
      127.1		-> 127.0.0.1
      127.2.1		-> 127.2.0.1
    */
    switch (dot_count) {
    case 1: result <<= 8;   /* Fall through */
    case 2: result <<= 8;   /* Fall through */
    }
    inp->s_addr = (result << 8) + (unsigned long) byte_result;
    return 1;
  }

  return 0;
}

#endif


void ufdbSimulateHttpServer( 
   const char *        interface, 
   int                 port, 
   const char *        username,
   const char *        imagesDirectory,
   int                 flags )
{
   int                 s;
   int                 sock_parm;
   struct sockaddr_in  addr;
   struct timeval      tv;

   errno = 0;
   s = socket( AF_INET, SOCK_STREAM, 0 );
   if (s < 0)
   {
      ufdbLogError( "SimulateHttpServer: cannot create socket: %s", strerror(errno) );
      return;
   }

   addr.sin_family = AF_INET;
   addr.sin_port = htons( port );
   if (interface == NULL  ||  strcmp(interface,"all")==0)
      addr.sin_addr.s_addr = htonl( INADDR_ANY );
   else
   {
      struct in_addr iaddr;
      if (inet_pton( AF_INET, interface, &iaddr ) == 0)
      {
	 addr.sin_addr.s_addr = htonl( INADDR_ANY );
	 ufdbLogError( "interface parameter '%s' is invalid. I will listen on port %d on ALL interfaces.", 
	               interface, port );
      }
      else
	 addr.sin_addr.s_addr = iaddr.s_addr;
   }

   /*
    * Allow server-side addresses to be reused (don't have to wait for timeout).
    */
   sock_parm = 1;
   setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_parm, sizeof(sock_parm) );

   /*
    * This http server has very little data to receive and send...
    */
   sock_parm = 12 * 1024;
   setsockopt( s, SOL_SOCKET, SO_SNDBUF, (void *) &sock_parm, sizeof(sock_parm) );
   sock_parm = 12 * 1024;
   setsockopt( s, SOL_SOCKET, SO_RCVBUF, (void *) &sock_parm, sizeof(sock_parm) );

   tv.tv_sec = 6;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 6;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   /* with anti-aliasing warnings ON, connect/bind cause compiler warning which we may ignore */
   if (bind( s, (struct sockaddr *) &addr, sizeof(addr) ) < 0)
   {
      ufdbLogError( "SimulateHttpServer: cannot bind socket: %s\n"
      		    "Check for other processes using port %d  uid=%d euid=%d", 
		    strerror(errno), port, getuid(), geteuid() );
      close( s );
      return;
   }
   
   /* Now that the socket is bound, we can drop root privileges */
   if (username != NULL && username[0] != '\0')
   {
      UFDBdropPrivileges( username );
   }

   writeHttpdPidFile();
   atexit( removeHttpdPidFile );

   /*
    * According to comment in the Apache httpd source code, these socket
    * options should only be set after a successful bind....
    */
   sock_parm = 1;
   setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, (void *) &sock_parm, sizeof(sock_parm) );

#ifdef TCP_FASTOPEN
   /* change the socket options to TCO_FASTOPEN */
   sock_parm = 256;
   setsockopt( s, SOL_TCP, TCP_FASTOPEN, (void *) &sock_parm, sizeof(sock_parm) );
#endif

#if 0
   sock_parm = 1;
   setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (void *) &sock_parm, sizeof(sock_parm) );
#endif

   if (listen( s, 256 ) < 0)
   {
      ufdbLogError( "SimulateHttpServer: cannot listen on socket: %s", strerror(errno) );
      close( s );
      return;
   }

   ufdbLogMessage( "SimulateHttpServer: listening on port %d", port );
    
   initData( imagesDirectory );

   ServeHttpConnections( s, imagesDirectory );
}

