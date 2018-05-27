/* 
 * ufdbgclient.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * Light-weight process that connects to the ufdbguardd daemon to perform a URL verification.
 *
 * $Id: ufdbgclient.c,v 1.65 2017/05/27 21:44:43 root Exp root $
 */

#include "sg.h"

#include <stdio.h>
#include <time.h>
#include <libgen.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if HAVE_UNIX_SOCKETS
#include <sys/un.h>
#endif

#include <pthread.h>

extern int    globalPid;

static int    debugRedirect;
static int    multiThreading = 0;

char   serverName[256];
int    noRedirects = 0;
int    portNum;
int    squidConcurrentInput = 0;
int    timeout = 21;
int    testSimulation = 0;
int    be_quiet = 0;
int    slowdown = 0;
int    httpsChecks = UFDB_API_HTTPS_CHECK_OFF;
int    connectionErrorBehaviour = UFDB_ALLOW;
char   connectErrorRedirect[1024] = 
		"http://cgibin.urlfilterdb.com/cgi-bin/URLblocked.cgi?"
		"category=fatal-error";

static pthread_t  worker[UFDB_MAX_SQUID_CONCURRENCY];


int connect_to_server( void )
{
   int                 s;
   int                 protocol;
   int                 retval;
   struct hostent *    server;
   struct sockaddr_in  addr_in;

   errno = 0;

   /*
    * create the socket to connect to the daemon.
    */
#if HAVE_UNIX_SOCKETS
   protocol = AF_UNIX;
   s = socket( protocol, SOCK_STREAM, 0 );
   if (s < 0)
   {
      protocol = AF_INET;
      s = socket( protocol, SOCK_STREAM, 0 );
   }
#else
   protocol = AF_INET;
   s = socket( protocol, SOCK_STREAM, 0 );
#endif

   if (s < 0)
   {
      ufdbLogError( "cannot create socket: %s", strerror(errno) );
      exit( 1 );
   }

#if HAVE_UNIX_SOCKETS
   if (protocol == AF_UNIX)
   {
      struct sockaddr_un  addr_un;

      addr_un.sun_family = AF_UNIX;
      sprintf( addr_un.sun_path, "/tmp/ufdbguardd-%05d", portNum );
      /* with anti-aliasing warnings ON, connect/bind cause compiler warnings whch we may ignore */
      retval = connect( s, (struct sockaddr *) &addr_un, sizeof(addr_un) );
      if (retval < 0)
         ufdbLogError( "cannot connect to UNIX socket %s", addr_un.sun_path );
   }
   else
#endif
   {
      /* 
       * check the server name.
       */
      server = gethostbyname( serverName );
      if (server == NULL)
      {
	 char *  c; 			/* see if we have a dotted IP address */
	 struct in_addr server_addr;

	 c = serverName;
	 while (*c != '\0')
	 {
	    if (*c != '.' && (*c < '0' || *c > '9'))
	    {
	       fprintf( stderr, "%s: hostname lookup failed for server '%s'\n", UFDBprogname, serverName );
	       exit( 1 );
	    }
	    c++;
	 }

	 /* the name looks like a dotted IP address */
         /* TODO: support IPv6 for serverName in ufdbgclient */
	 server_addr.s_addr = inet_addr( serverName );
	 server = gethostbyaddr( (const char *) &server_addr, sizeof(struct in_addr), AF_INET );
	 if (server == NULL)
	 {
	    fprintf( stderr, "%s: IP lookup failed for server '%s'\n", UFDBprogname, serverName );
	    exit( 1 );
	 }
      }

      addr_in.sin_family = AF_INET;
      memcpy( (void *) &addr_in.sin_addr, (void *) server->h_addr_list[0], sizeof(addr_in.sin_addr) );  
      addr_in.sin_port = htons( portNum );
      /* with anti-aliasing warnings ON, connect/bind cause compiler warnings which we may ignore */
      retval = connect( s, (struct sockaddr *) &addr_in, sizeof(addr_in) );
      if (retval < 0)
         ufdbLogError( "cannot connect to IP socket on server %s, port %d", serverName, portNum );
   }

   if (retval < 0)
   {
      /* be silent about the connection error */
      close( s );
      s = -1;
   }
   else
   {
      int            sock_parm;
      struct timeval tv;

      sock_parm = 1;
      setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, (void *) &sock_parm, sizeof(sock_parm) );

      /*
       * Allow server-side addresses to be reused (don't have to wait for timeout).
       * Turn off NAGLE.
       */
      if (protocol == AF_INET)
      {
	 sock_parm = 1;
	 setsockopt( s, IPPROTO_TCP, TCP_NODELAY, (void *) &sock_parm, sizeof(sock_parm) );
      }

      /*
       * TCP buffers can be quite large.
       * Optimize the window size by setting it to a relatively small fixed size on the 
       * client and the server.
       */
      if (protocol == AF_UNIX)
      {
	 sock_parm = 16384;
	 setsockopt( s, SOL_SOCKET, SO_SNDBUF, (void *) &sock_parm, sizeof(sock_parm) );
	 sock_parm = 16384;
	 setsockopt( s, SOL_SOCKET, SO_RCVBUF, (void *) &sock_parm, sizeof(sock_parm) );
      }

      /*
       *  Prevent long blocking on communication with the daemon.
       */
      tv.tv_sec = timeout;	/* 'timeout' seconds timeout for reads */
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );

      tv.tv_sec = 3;		/* 3 second timeout for writes */
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );
   }

   return s;
}


static void process_cmd( 
   int    fd, 
   char * url, 
   char * ip, 
   char * name )
{
   int    len;
   int    nbytes;
   int    hassep;
   char * method;
   char   line[UFDB_HTTP_1_1_MAX_URI_LENGTH+512];

   if (strlen(url) > sizeof(line)-2)
      url[sizeof(line)-2] = '\0';

   hassep = strchr( ip, '/') != NULL;
   method = "GET";
   if (strstr( url, ":443" ) != NULL)
      method = "CONNECT";
   if (strstr( url, "://" ) == NULL  &&  strstr( url, ":443" ) == NULL)
      sprintf( line, "http://%s %s%s %s %s\n", url, ip, hassep ? "" : "/-", name, method );
   else
      sprintf( line, "%s %s%s %s %s\n", url, ip, hassep ? "" : "/-", name, method );

   len = strlen( line );
   nbytes = write( fd, line, len );
   if (nbytes != len)
   {
      ufdbLogError( "line length is %d but wrote %d bytes.", len, nbytes );
      exit( 1 );
   }
   if (UFDBglobalDebug)
      ufdbLogMessage( "process_cmd: sent to server: %s", line );

   nbytes = read( fd, line, sizeof(line) );
   if (nbytes > sizeof(line)-1)
      nbytes = sizeof(line) - 1;
   if (nbytes < 0)
   {
      ufdbLogFatalError( "read from daemon failed: %s", strerror(errno) );
      if (fwrite( "\n", 1, 1, stdout ))
         ;
   }
   else
   {
      line[nbytes] = '\0';            /* force null termination */
      if (fwrite( line, nbytes, 1, stdout ))
         ;
   }
}


static void do_test_sim( 
   int     s, 
   char *  url,
   char *  ip,
   char *  user )
{
   time_t  t0;
   time_t  now;

   now = t0 = time( NULL );
   while (now - t0 < 20)
   {
      process_cmd( s, url, ip, user );
      usleep( 501 );

      now = time( NULL );
   }
}


static void process_input( 
   int    server_socket  )
{
   int    nbytes;
   int    len;
   int    fd_stdout = fileno( stdout );
   int    num_waivers;
   time_t t0;
   time_t t1;
   time_t last_failed_connect;
   char * result;
   char   channelID[16];
   char   requestBuffer[UFDB_HTTP_1_1_MAX_URI_LENGTH+512];      /* channel-ID username ip fqdn GET SNI */
   char   resultBuffer[UFDB_HTTP_1_1_MAX_URI_LENGTH+512];

   /*
    * loop to process queries on stdin.
    * If we cannot connect to the server, we continue looping:
    * - try to connect again to the server socket
    * - if failure, send an "OK" back to the requestor.
    */
   num_waivers = 6;
   last_failed_connect = time( NULL );
   if (server_socket < 0)
   {
      ufdbLogFatalError( "cannot connect to ufdbguardd daemon socket: %s\n"
                         "Check if ufdbguardd is running and if the -p option is required.", 
                         strerror(errno) );
      num_waivers = 1;
   }
   
   while (UFDBfgets(requestBuffer, sizeof(requestBuffer)-1, stdin) != NULL)	   /* read request */
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "URL request: %s", requestBuffer );

      if (server_socket < 0)
      {
	 num_waivers--;
	 if (num_waivers < 0  || (time(NULL) - last_failed_connect > 5))
	 {
	    num_waivers = 6;
            /* retry to connect after 6 lookups or after 5 seconds */
	    server_socket = connect_to_server();		
	    if (server_socket >= 0)
	    {
	       ufdbLogMessage( "communication with daemon re-established" );
	    }
            else
	       last_failed_connect = time( NULL );
	 }
      }

      channelID[0] = '\0';
      if (requestBuffer[0] <= '9'  &&  requestBuffer[0] >= '0')
      {
         int    i;
         char * request;

         i = 0;
         request = requestBuffer;
         while (i < 15  &&  *request != '\0'  &&  *request >= '0'  &&  *request <= '9')
            channelID[i++] = *request++;
         if (*request == ' ')
            channelID[i] = '\0';
         else
            /* oops: no valid channel-ID, must be URL starting with digit */
            channelID[0] = '\0';

         request = requestBuffer;
      }

      if (server_socket < 0)		/* daemon is dead, connectionErrorBehaviour is leading */
      {
	 last_failed_connect = time( NULL );
	 if (connectionErrorBehaviour == UFDB_ALLOW)
	    len = sprintf( resultBuffer, "%s\n", channelID );
	 else if (channelID[0] != '\0')
	    len = sprintf( resultBuffer, "%s %s\n", channelID, connectErrorRedirect );
	 else
	    len = sprintf( resultBuffer, "%s\n", connectErrorRedirect );
	 if (write( fd_stdout, resultBuffer, len ) != len)
	 {
	    ufdbLogError( "lost communication with parent (write): %s\n"
                          "Check if squid is running.\n"
                          "Last request: %s",
	    		  strerror(errno), requestBuffer );
	    break;
	 }
	 continue;
      }

      len = strlen( requestBuffer );
      if (len >= sizeof(requestBuffer)-1)		/* truncate the request */
      {
         len = sizeof(requestBuffer)-2;
	 requestBuffer[len] = '\n';
	 requestBuffer[len+1] = '\0';
      }

      /* forward the request to ufdbguardd */
      errno = 0;
      nbytes = write( server_socket, requestBuffer, len );

      if (nbytes <= 0)  
      {								/* server socket was closed */
	 if (nbytes == 0)
	    errno = EINVAL;
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "communication with ufdbguardd daemon lost (write): %s  ******\n"
	 	       "Check if ufdbguardd is running.\n"
		       "last request: %s",
	 	       strerror(errno), 
		       requestBuffer );
	 if (connectionErrorBehaviour == UFDB_ALLOW)
	    len = sprintf( resultBuffer, "%s\n", channelID );		/* TODO: Squid 3.4+ wants "OK" */
	 else if (channelID[0] != '\0')
	    len = sprintf( resultBuffer, "%s %s\n", channelID, connectErrorRedirect );	/* TODO: Squid 3.4+ wants "OK" */
	 else
	    len = sprintf( resultBuffer, "%s\n", channelID );		/* TODO: Squid 3.4+ wants "OK" */
	 if (write( fd_stdout, resultBuffer, len ) != len)
	 {
	    ufdbLogError( "lost communication with parent (write): %s\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
	    		  strerror(errno), 
			  requestBuffer );
	    break;
	 }
	 continue;
      }

      if (nbytes != len)
      {
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "line length is %d but wrote %d bytes to ufdbguardd (%s).\n"
		       "all verifications will be rated \"%s\"",
	               len, nbytes, strerror(errno),
		       connectionErrorBehaviour == UFDB_ALLOW ? "OK" : "BLOCK" );
	 if (connectionErrorBehaviour == UFDB_ALLOW)
	    len = sprintf( resultBuffer, "%s\n", channelID );
	 else if (channelID[0] != '\0')
	    len = sprintf( resultBuffer, "%s %s\n", channelID, connectErrorRedirect );
	 else
	    len = sprintf( resultBuffer, "%s\n", channelID );
	 if (write( fd_stdout, resultBuffer, len ) != len)
	 {
	    ufdbLogError( "lost communication with parent (write): %s\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
	    		  strerror(errno),
			  requestBuffer );
	    break;
	 }
	 continue;
      }

      /* read the result that is sent by ufdbguardd */

      result = resultBuffer;

      errno = 0;
      t0 = time( NULL );
      nbytes = read( server_socket, result, sizeof(resultBuffer)-1 );
      t1 = time( NULL );
      if ((t1 - t0) > 15)
      {
         ufdbLogMessage( "WARNING: server response is slow: nbytes=%d numsec=%ld error=%s request=%s", 
	                 nbytes, (long) (t1 - t0), strerror(errno), requestBuffer );
      }

      if (nbytes <= 0)
      {								/* server socket was closed */
	 if (nbytes == 0)
	    errno = EINVAL;
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "communication with ufdbguardd daemon lost (read): %s\n"
	 	       "Check if ufdbguardd is running.\n"
		       "Last request: %s",
	 	       strerror(errno),
		       requestBuffer );
	 if (connectionErrorBehaviour == UFDB_ALLOW)
	    len = sprintf( resultBuffer, "%s\n", channelID );
	 else if (channelID[0] != '\0')
	    len = sprintf( resultBuffer, "%s %s\n", channelID, connectErrorRedirect );
	 else
	    len = sprintf( resultBuffer, "%s\n", channelID );
	 if (write( fd_stdout, resultBuffer, len ) != len)
	 {
	    ufdbLogError( "lost communication with parent (write): %s  *****\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
	    		  strerror(errno),
			  requestBuffer );
	    return;
	 }
	 continue;
      }

      if (nbytes > sizeof(resultBuffer) - 17)
	 nbytes = sizeof(resultBuffer) - 17;
      result[nbytes] = '\0';            	

      if (debugRedirect)
      {
	 if (nbytes > 1)
	    ufdbLogMessage( "   REDIRECT  -->%s   -->%s", requestBuffer, resultBuffer );
      }

      if (noRedirects)					/* test mode */
      {
	 len = sprintf( resultBuffer, "%s\n", channelID );
	 if (write( fd_stdout, resultBuffer, len ) != len)
	 {
	    ufdbLogError( "lost communication with parent (write): %s  *****\n"
	                  "check if squid is running.\n"
			  "last request: %s",
	    		  strerror(errno),
			  requestBuffer );
	    ufdbLogFatalError( "lost communication with Squid. exiting..." );
	    return;
	 }
      }
      else
      {
	 if (write( fd_stdout, resultBuffer, nbytes ) != nbytes)	/* forward result */
	 {
	    ufdbLogError( "lost communication with parent (write): %s  *****\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
	    		  strerror(errno),
			  requestBuffer );
	    ufdbLogFatalError( "lost communication with Squid. exiting..." );
	    return;
	 }
      }

      if ((t1 - t0) >= 11)		/* WHY is the server so slow ? */
      {
	 if (server_socket >= 0)
	 {
	    ufdbLogError( "ufdbguardd took %ld seconds to respond.  closing connection.  *****", t1-t0 );
	    close( server_socket );
	 }
	 server_socket = -1;
	 num_waivers = 0;
	 last_failed_connect = time( NULL );
      }
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "end of input" );
}


static int mt_get_request( char * buffer, int * len )
{
   static pthread_mutex_t readlock = PTHREAD_MUTEX_INITIALIZER;
   char * res;

   pthread_mutex_lock( &readlock );

   res = UFDBfgets( buffer, UFDB_HTTP_1_1_MAX_URI_LENGTH+512, stdin );

   pthread_mutex_unlock( &readlock );

   if (res == NULL)
      return 0;

   *len = strlen( buffer );
   return 1;
}


static ssize_t mt_write( int fd, const void * buf, size_t count )
{
   static pthread_mutex_t writelock = PTHREAD_MUTEX_INITIALIZER;
   ssize_t  retval;

   if (slowdown)
   {
      sched_yield();
      usleep( 1001 );
   }

   pthread_mutex_lock( &writelock );

   retval = write( fd, buf, count );

   pthread_mutex_unlock( &writelock );

   return retval;
}


static void * mt_process_input( void * ptr )
{
   int    server_socket;
   int    nbytes;
   int    len;
   int    fd_stdout = fileno( stdout );
   int    num_waivers;
   int    tnum;
   time_t t0;
   time_t t1;
   time_t last_failed_connect;
   time_t now;
   char * request;
   char * result;
   char   reqbuf[UFDB_HTTP_1_1_MAX_URI_LENGTH+512];
   char   replybuf[UFDB_HTTP_1_1_MAX_URI_LENGTH+512];

   /*
    * loop to process queries on stdin.
    * If we cannot connect to the server, we continue looping:
    * - try to connect again to the server socket
    * - if failure, send an "OK" back to the requestor.
    */
   tnum = (int) ((long) ptr);
   last_failed_connect = time( NULL );
   server_socket = -1;
   num_waivers = 1;
   server_socket = connect_to_server();
   
   while (mt_get_request( reqbuf, &len ))
   {
      if (len >= sizeof(reqbuf))		/* truncate URI */
      {
         len = sizeof(reqbuf) - 1;
         reqbuf[len-1] = '\n';
         reqbuf[len] = '\0';
      }

      if (UFDBglobalDebug)
	 ufdbLogMessage( "W%02d: request: %s", tnum, reqbuf );

      if (server_socket < 0)
      {
	 num_waivers--;
         /* retry to connect after 6 lookups or after 3 seconds */
         now = time( NULL );
	 if (num_waivers < 0  || (now - last_failed_connect >= 3))
	 {
	    num_waivers = 6;
	    server_socket = connect_to_server();
	    if (server_socket >= 0)
	    {
	       ufdbLogMessage( "W%02d: communication with daemon re-established", tnum );
	    }
	    else
	       last_failed_connect = now;
	 }
      }

      if (server_socket < 0)	                /* daemon is dead, connectionErrorBehaviour is leading */
      {
         /* copy the optional channel-ID */
         request = reqbuf;
         result = replybuf;
         if (*request <= '9'  &&  *request >= '0')
         {
            while (*request != '\0'  &&  *request >= '0'  &&  *request <= '9')
               *result++ = *request++;
            if (*request == ' ')
               *result++ = ' ';
            else
               /* oops: no valid channel-ID, must be URL starting with digit */
               result = replybuf;
         }
	 if (connectionErrorBehaviour == UFDB_ALLOW)
         {
            *result++ = 'O';
            *result++ = 'K';
            *result++ = '\n';
            *result = '\0';
         }
	 else
	    sprintf( result, "%s\n", connectErrorRedirect );

         if (UFDBglobalDebug)
            ufdbLogMessage( "W%02d: reply: %s", tnum, replybuf );

         nbytes = strlen( replybuf );
	 if (mt_write( fd_stdout, replybuf, nbytes ) != nbytes)
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s\n"
                          "Check if squid is running.\n"
                          "Last request: %s",
	    		  tnum, strerror(errno), reqbuf );
	    break;
	 }
         continue;
      }

      /* forward the request to ufdbguardd */
      errno = 0;
      nbytes = write( server_socket, reqbuf, len );

      if (nbytes <= 0)  
      {								/* server socket was closed */
	 if (nbytes == 0)
	    errno = EINVAL;
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "W%02d: communication with ufdbguardd daemon lost (write): %s  ******\n"
	 	       "Check if ufdbguardd is running.\n"
		       "last request: %s",
                       tnum,
	 	       strerror(errno), 
		       reqbuf );

         /* copy the optional channel-ID */
         request = reqbuf;
         result = replybuf;
         if (*request <= '9'  &&  *request >= '0')
         {
            while (*request != '\0'  &&  *request >= '0'  &&  *request <= '9')
               *result++ = *request++;
            if (*request == ' ')
               *result++ = ' ';
            else
               /* oops: no valid channel-ID, must be URL starting with digit */
               result = replybuf;
         }
	 if (connectionErrorBehaviour == UFDB_ALLOW)
         {
            *result++ = 'O';
            *result++ = 'K';
            *result++ = '\n';
            *result = '\0';
         }
	 else
         {
	    sprintf( result, "OK status=302 url=\"%s\"\n", connectErrorRedirect );
                                                                /* TODO: OK rewrite-url="URL"     iff CONNECT */
                                                                /* OK status=302 url="URL"  otherwise */
         }
         nbytes = strlen( replybuf );
	 if (mt_write( fd_stdout, replybuf, nbytes ) != nbytes)
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
                          tnum,
	    		  strerror(errno), 
			  reqbuf );
	    break;
	 }
	 continue;
      }

      if (nbytes != len)
      {
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "W%02d: line length is %d but wrote %d bytes to ufdbguardd (%s).\n"
		       "all verifications will be rated \"%s\"",
	               tnum, len, nbytes, strerror(errno),
		       connectionErrorBehaviour == UFDB_ALLOW ? "OK" : "BLOCK" );

         /* copy the optional channel-ID */
         request = reqbuf;
         result = replybuf;
         if (*request <= '9'  &&  *request >= '0')
         {
            while (*request != '\0'  &&  *request >= '0'  &&  *request <= '9')
               *result++ = *request++;
            if (*request == ' ')
               *result++ = ' ';
            else
               /* oops: no valid channel-ID, must be URL starting with digit */
               result = replybuf;
         }
	 if (connectionErrorBehaviour == UFDB_ALLOW)
         {
            *result++ = 'O';
            *result++ = 'K';
            *result++ = '\n';
            *result = '\0';
         }
	 else
	    sprintf( result, "%s\n", connectErrorRedirect );    /* TODO: Squid 3.4+ wants "OK" */
         nbytes = strlen( replybuf );
	 if (mt_write( fd_stdout, replybuf, nbytes ) != len)
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
	    		  tnum, strerror(errno),
			  reqbuf);
	    break;
	 }
	 continue;
      }

      /* read the result that is sent by ufdbguardd */

      result = replybuf;

      errno = 0;
      t0 = time( NULL );
      nbytes = read( server_socket, replybuf, sizeof(replybuf) );
      t1 = time( NULL );
      if ((t1 - t0) > 4)
      {
         ufdbLogMessage( "W%02d: WARNING: server response is slow: nbytes=%d numsec=%ld error=%s request=%s", 
	                 tnum, nbytes, (long) (t1 - t0), strerror(errno), reqbuf );
      }

      if (nbytes <= 0)
      {								/* server socket was closed */
	 if (nbytes == 0)
	    errno = EINVAL;
         close( server_socket );
	 server_socket = -1;
	 num_waivers = 6;
	 last_failed_connect = time( NULL );
	 ufdbLogError( "W%02d: communication with ufdbguardd daemon lost (read): %s\n"
	 	       "Check if ufdbguardd is running.\n"
		       "Last request: %s",
                       tnum,
	 	       strerror(errno),
		       reqbuf );

         /* copy the optional channel-ID */
         request = reqbuf;
         result = replybuf;
         if (*request <= '9'  &&  *request >= '0')
         {
            while (*request != '\0'  &&  *request >= '0'  &&  *request <= '9')
               *result++ = *request++;
            if (*request == ' ')
               *result++ = ' ';
            else
               /* oops: no valid channel-ID, must be URL starting with digit */
               result = replybuf;
         }
	 if (connectionErrorBehaviour == UFDB_ALLOW)
         {
            *result++ = 'O';
            *result++ = 'K';
            *result++ = '\n';
            *result = '\0';
         }
	 else
	    sprintf( result, "%s\n", connectErrorRedirect );    /* TODO: Squid 3.4+ wants "OK" */
         nbytes = strlen( replybuf );
	 if (mt_write( fd_stdout, replybuf, nbytes ) != nbytes)
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s  *****\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
                          tnum,
	    		  strerror(errno),
			  reqbuf );
	    break;
	 }
	 continue;
      }

      if (nbytes > sizeof(replybuf) - 17)
	 nbytes = sizeof(replybuf) - 17;             /* truncate very large replies */
      replybuf[nbytes] = '\0';            	

      if (debugRedirect)
      {
	 if (nbytes >= 5)
	    ufdbLogMessage( "W%02d: REDIRECT  -->%s   -->%s", tnum, reqbuf, replybuf );
      }

      if (noRedirects)					/* test mode */
      {
         /* copy the optional channel-ID */
         request = reqbuf;
         result = replybuf;
         if (*request <= '9'  &&  *request >= '0')
         {
            while (*request != '\0'  &&  *request >= '0'  &&  *request <= '9')
               *result++ = *request++;
            if (*request == ' ')
               *result++ = ' ';
            else
               /* oops: no valid channel-ID, must be URL starting with digit */
               result = replybuf;
         }
	 if (connectionErrorBehaviour == UFDB_ALLOW)
         {
            *result++ = 'O';
            *result++ = 'K';
            *result++ = '\n';
            *result = '\0';
         }
	 else
	    sprintf( result, "%s\n", connectErrorRedirect );
         nbytes = strlen( replybuf );
	 if (mt_write( fd_stdout, replybuf, nbytes ) != nbytes)
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s  *****\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
                          tnum,
	    		  strerror(errno),
			  reqbuf );
	    ufdbLogFatalError( "W%02d: lost communication with Squid. exiting...", tnum );
	    break;
	 }
      }
      else
      {
	 if (mt_write( fd_stdout, replybuf, nbytes ) != nbytes)	        /* forward result */
	 {
	    ufdbLogError( "W%02d: lost communication with parent (write): %s  *****\n"
	                  "Check if squid is running.\n"
			  "Last request: %s",
                          tnum,
	    		  strerror(errno),
			  reqbuf );
	    ufdbLogFatalError( "W%02d: lost communication with Squid. exiting...", tnum );
	    break;
	 }
      }

      if (UFDBglobalDebug)
         ufdbLogMessage( "W%02d: reply: %s", tnum, replybuf );

      if ((t1 - t0) >= 15)		/* WHY is the server so slow ? */
      {
	 if (server_socket >= 0)
	    ufdbLogMessage( "W%02d: ufdbguardd took %ld seconds to respond.", tnum, t1-t0 );
      }
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "W%02d: end of input", tnum );

   return NULL;
}


static void client_usage( void )
{
   fprintf( stderr, "usage: ufdbgclient [-d] [-v] [-r] [-m N | -C] [-T] [-t timeout] [-S server] [-p port] [-l logdir] [<URL> <IP> <user>]\n" );
   fprintf( stderr, "Options:\n" );
   fprintf( stderr, "  -d          : debug\n" );
   fprintf( stderr, "  -v          : show version number\n" );
   fprintf( stderr, "  -r          : log the redirections (performance impact)\n" );
#if 0
   fprintf( stderr, "  -N          : no log file\n" );
#endif
   fprintf( stderr, "  -m NT       : use Squid concurrency with internal multithreading (NT threads)\n" );
   fprintf( stderr, "  -C          : use Squid concurrency with internal queueing\n" );
   fprintf( stderr, "  -e policy   : policy on connection error with ufdbguardd: URL lookup result is 'deny' or 'allow'\n" );
   fprintf( stderr, "  -E URL      : redirection URL for denied access on connention error\n" );
   fprintf( stderr, "  -T          : test mode: log unwanted URLs but do not block them\n" );
   fprintf( stderr, "  -t numsec   : timeout waiting for daemon (default: %d seconds)\n", timeout );
   fprintf( stderr, "  -S server   : specify server name (default is %s)\n", "localhost" );
   fprintf( stderr, "  -p portnum  : override communication port (default is %d)\n", UFDB_DAEMON_PORT );
   fprintf( stderr, "  -l logdir   : log directory (default is %s)\n", DEFAULT_LOGDIR );
   fprintf( stderr, "  -q          : quiet mode\n" );
   fprintf( stderr, "  <URL>       : e.g. www.urlfilterdb.com\n" );
   fprintf( stderr, "  <IP>        : e.g. 10.1.1.1\n" );
   fprintf( stderr, "  <user>      : e.g. johndoe or '-'\n" );
}


/* 
 * we do NOT want to link with the bz2 library so we have some stubs.
 */
void BZ2_bzBuffToBuffDecompress()
{ }


static int denyORallow( char * option )
{
#if 0
   fprintf( stderr, "denyORallow \"%s\"\n", option );
#endif

   if (strcmp( option, "allow" ) == 0)
      return UFDB_ALLOW;
   else if (strcmp( option, "deny" ) == 0  ||  strcmp( option, "block" ) == 0)
      return UFDB_DENY;

   fprintf( stderr, "-e option must be followed by 'allow' or 'deny'\n" );
   client_usage();
   exit( 2 );
   return UFDB_ALLOW;  /* make compiler happy */
}


int main( 
   int                 argc, 
   char **             argv )
{
   int                 i;
   int                 s;
   struct stat         stbuf;
   struct tms          timer;
   struct timeval      start_time;
   pthread_attr_t      attr;

   globalPid = getpid();
   strcpy( UFDBprogname, "ufdbgclient" );

   strcpy( serverName, "localhost" );
   portNum = UFDB_DAEMON_PORT;

   while ((s = getopt(argc, argv, "e:E:NChdrTvql:m:p:S:t:Z")) != EOF)
   {
      switch (s) {
      case 'd':
	 UFDBglobalDebug++;
	 break;
      case 'e':
	 connectionErrorBehaviour = denyORallow( optarg );	
         break;
      case 'E':
         strcpy( connectErrorRedirect, optarg );
	 break;
      case 'r':
	 debugRedirect = 1;
	 break;
      case 'l':
	 UFDBglobalLogDir = ufdbStrdup( optarg );
	 if (stat( UFDBglobalLogDir, &stbuf ) != 0)
	 {
	    fprintf( stderr, "ufdbgclient: error in -l %s: %s\n", UFDBglobalLogDir, strerror(errno) );
	    exit( 1 );
	 }
	 if (!S_ISDIR(stbuf.st_mode))
	 {
	    fprintf( stderr, "ufdbgclient: %s is not a directory\n", UFDBglobalLogDir );
	    exit( 1 );
	 }
	 break;
      case 'm':
         multiThreading = atoi( optarg );
         if (multiThreading < 2 || multiThreading > UFDB_MAX_SQUID_CONCURRENCY)
         {
            ufdbLogError( "-m multi-threading option value must be between 2 and %d", 
                          UFDB_MAX_SQUID_CONCURRENCY );
            if (multiThreading < 2)
               multiThreading = 2;
            else
               multiThreading = UFDB_MAX_SQUID_CONCURRENCY;
         }
         break;
      case 'N':
         ufdbGlobalSetLogging( 0 );
	 break;
      case 'C':
	 squidConcurrentInput = 1;
         break;
      case 'p':
         portNum = atoi( optarg );
	 if (portNum < 1)
	 {
	    fprintf( stderr, "ufdbgclient: port number must be > 0\n" );
	    exit( 2 );
	 }
	 break;
      case 'q':
	 be_quiet = 1;
         break;
      case 'v':
	 fprintf( stderr, "ufdbgclient: %s\n", UFDB_VERSION );
	 exit( 0 );
	 break;
      case 's':
         slowdown = 1;
         break;
      case 'S':
         strcpy( serverName, optarg );
	 break;
      case 't':
	 timeout = atoi( optarg );
	 if (timeout < 2)
	    timeout = 2;
	 if (timeout > 120)
	    timeout = 120;
         break;
      case 'T':
	 fprintf( stderr, "-T option found.  Going into test mode.\n" );
	 noRedirects = 1;		/* Test mode means noRedirects */
	 break;
      case 'Z':
	 testSimulation = 1;
         break;
      case '?':
      case 'h':
      default:
         client_usage();
	 exit( 0 );
      }
   }

   ufdbSetGlobalErrorLogFile( 0 );
   signal( SIGPIPE, SIG_IGN );
   
   if (testSimulation)
   {
      s = connect_to_server();
      if (s < 0)
      {
         ufdbLogError( "test simulation: could not connect to server. aborting" );
	 exit( 6 );
      }

      do_test_sim( s, argv[optind], argv[optind+1], argv[optind+2] );

      close( s );
      exit( 0 );
   }

   s = connect_to_server();

   /* 
    * process one query from the command line ?
    *
    * Note that Squid send comments at the end of the command in squid.conf to us as well
    * so look for a '#' and ignore it.
    */
   if (optind < argc - 2  &&  argv[optind][0] != '#')
   {
      if (s < 0)
      {
	 int  nbytes;
	 char line[1024];

	 if (connectionErrorBehaviour == UFDB_ALLOW)
	    strcpy( line, "\n" );
	 else
	    sprintf( line, "%s\n", connectErrorRedirect );
	 nbytes = strlen( line );
	 if (fwrite( line, nbytes, 1, stdout ))
	    ;
	 ufdbLogError( "cannot connect to port %d on server %s: %s", 
	               portNum, serverName, strerror(errno) );
	 exit( 8 );
      }
      process_cmd( s, argv[optind], argv[optind+1], argv[optind+2] );
      close( s );
      exit( 0 );
   }

   if (!be_quiet)
   {
      gettimeofday( &start_time, NULL );
      ufdbLogMessage( "ufdbgclient " UFDB_VERSION " started" );
      UFDBtimerInit( &timer );
   }

   if (s < 0)
   {
      /* The ufdbguardd daemon is not yet running, or has terminated.
       * On system boot, we only need to wait a few seconds.
       */
      ufdbLogMessage( "cannot connect to ufdbguardd daemon - is it running?  If yes, check the -p option." );
      sleep( 2 );
      s = connect_to_server();
      if (s < 0)
	 ufdbLogMessage( "after sleeping 2 seconds I still could not connect to the ufdbguardd daemon" );
      else
	 ufdbLogMessage( "after sleeping 2 seconds I could connect to the ufdbguardd daemon" );
   }

   if (multiThreading == 0)
      process_input( s );
   else
   {
      close( s );
      s = -1;
      pthread_attr_init( &attr );
      pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
      pthread_attr_setstacksize( &attr, 256 * 1024 );
      for (i = 0;  i < multiThreading;  i++)
      {
         pthread_create( &worker[i], &attr, mt_process_input, (void *) ((long) i) );
      }
      ufdbLogMessage( "%d worker threads created.", multiThreading );
#ifdef _POSIX_PRIORITY_SCHEDULING
      sched_yield();
#else
      usleep( 100000 );
#endif
      for (i = 0;  i < multiThreading; i++)
         pthread_join( worker[i], NULL );
   }

   if (!be_quiet)
   {
      char cpuusagetxt[200];

      UFDBtimerStop( &timer );
      UFDBtimerPrintString( cpuusagetxt, &timer, "CPU times" );
      gettimeofday( &start_time, NULL );
      ufdbLogMessage( "ufdbgclient " UFDB_VERSION " finished\n%s", cpuusagetxt );
   }

   if (s >= 0)
      close( s );

   exit( 0 );
}

