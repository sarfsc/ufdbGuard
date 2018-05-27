/*
 * ufdbchkport.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdbchkport.c,v 1.109 2018/05/25 13:09:00 root Exp root $
 */

#include "ufdb.h"
#include "ufdblib.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"
#include "httpsQueue.h"

#include "ufdbHashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#ifdef _POSIX_PRIORITY_SCHEDULING
#include <sched.h>
#endif

#include <sys/select.h>

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>


#undef UFDB_HTTPS_CACHE_DEBUG
#if defined(UFDB_DEBUG)
#define UFDB_HTTPS_CACHE_DEBUG 1
#endif 

#if UFDB_DO_DEBUG || 0
#define DEBUG(x) fprintf x 
#else
#define DEBUG(x) 
#endif


static volatile int tls_inited = 0;
static pthread_mutex_t init_mutex = UFDB_STATIC_MUTEX_INIT;
static SSL_CTX * ssl_ctx = NULL;

static int num_static_ssl_locks = 0;
static pthread_mutex_t * crypto_mutexes = NULL;

static struct UFDBhashtable * myht = NULL;
static int ufdbCacertsLoaded = 0;
static time_t lastHTTPScachePurgeTime = 0;
static time_t OldestInHTTPScache = 0;

static int  UFDBverifyPortHasHTTPS( const char * hostname, int portnumber, int flags );
static int  lookupHTTPScache( const char * hostname, int portnumber, int keepLockForInsert, struct httpsInfo ** hinfo );
static void updateHTTPScache( const char * hostname, int portnumber, int status, int lockSetBySearch, struct httpsInfo ** hinfo );
static void insertHTTPScache( const char * hostname, int portnumber, int status, int lockSetBySearch, struct httpsInfo ** hinfo );

#ifdef UFDB_LOAD_CERT_CHAIN
static int UFDBloadIntermediateCertificates( X509 * cert );
#endif

#if  0  ||  defined(UFDB_DEBUG)

static pthread_mutex_t https_mutex = UFDB_STATIC_MUTEX_INIT;

#define SSL_MUTEX_FN_INIT 	int _mutex_retval;
#define SSL_MUTEX_LOCK(fn) \
{                                                                \
   _mutex_retval = pthread_mutex_lock( &https_mutex );           \
   if (_mutex_retval != 0)                                       \
      ufdbLogError( fn ": mutex_lock failed with code %d", _mutex_retval );  \
}
#define SSL_MUTEX_UNLOCK(fn) \
{                                                                \
   _mutex_retval = pthread_mutex_unlock( &https_mutex );         \
   if (_mutex_retval != 0)                                       \
      ufdbLogError( fn ": mutex_unlock failed with code %d", _mutex_retval );  \
}

#else
#define SSL_MUTEX_FN_INIT
#define SSL_MUTEX_LOCK(fn)
#define SSL_MUTEX_UNLOCK(fn)
#endif


#define UFDB_DEBUG_PROBES 1


/*
 * Parsed OpenSSL version number.
 */
typedef struct {
    int     major;
    int     minor;
    int     micro;
    int     patch;
    int     status;
} TLS_VINFO;

static int ssl_check_certificate( SSL * ssl, const char * hostname, int portnumber, char * cn );


static int hostnameIsIP( const char * hostname )
{
   const char * h;

   if (hostname == NULL)
      return 0;

   /* check IPv4 */
   h = hostname;
   while (*h != '\0')
   {
      if (*h != '.'  &&  !isdigit(*h))
      {
	 /* check for IPv6 */
	 h = hostname;
	 while (*h != '\0')
	 {
	    if (*h != ':'  &&  !isxdigit(*h))
	       return 0;
	    h++;
	 }
	 return 1;
      }
      h++;
   }

   return 1;
}


static int detectSkypeSN( 
   const char *    hostname, 
   int             portnumber )
{
   int             n;
   int  	   s;
   struct timeval  tv;
   unsigned char   snbuffer[100] = { 
      0x80, 0x46, 0x01, 0x03, 0x01, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x05, 0x00, 0x00,
      0x04, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x09, 0x00, 0x00, 0x64, 0x00, 0x00, 0x62, 0x00, 0x00, 0x08,
      0x00, 0x00, 0x03, 0x00, 0x00, 0x06, 0x01, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x03, 0x00, 0x80, 0x06,
      0x00, 0x40, 0x02, 0x00, 0x80, 0x04, 0x00, 0x80,
      0x16, 0x03, 0x01, 0x00, 0x00, 0x06, 0xa3, 0x66, 0x09, 0x33, 0x5d, 0x3f, 0xf2, 0xb4, 0xca, 0x44
   };
   unsigned char   expected[11] = {
      0x16, 0x03, 0x01, 0x00, 0x4a, 0x02, 0x00, 0x00, 0x46, 0x03, 0x01  /* , 0x4d, 0xbe, 0xde */
   };

   if (UFDBglobalDebug  ||  UFDBglobalDebugSkype)
      ufdbLogMessage( "   probing for Skype node on %s:%d ...", hostname, portnumber );

   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      if (UFDBglobalDebugSkype)
	 ufdbLogMessage( "     socket open failed for %s:%d", hostname, portnumber );
      return UFDB_API_ERR_SOCKET;
   }

   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   /* Skype node detection */

   if (72 != write( s, snbuffer, 72 ))
   {
      if (UFDBglobalDebug || UFDBglobalDebugSkype)
	 ufdbLogMessage( "      write failed for %s:%d: %s", hostname, portnumber, strerror(errno) );
      close( s );
      return UFDB_API_ERR_READ;
   }

   n = read( s, snbuffer, 50 );
   close( s );
   
#if UFDB_DEBUG_PROBES
   if (UFDBglobalDebugSkype)
   {
      int i;
      if (UFDBglobalDebug == 0)
	 if (n > 20)
	    n = 20;
      for (i = 0; i < n; i++)
	 ufdbLogMessage( "      skype node probe reply byte %2d, 0x%02x  %c", i, snbuffer[i], (snbuffer[i] & 0x7f)|0x20 );
   }
#endif
   if (strncmp( (char *) snbuffer, "SSH", 3 ) == 0)
   {
      ufdbLogMessage( "%s:%d responded with SSH so it has a TUNNEL.", hostname, portnumber );
      return UFDB_API_ERR_TUNNEL;
   }

   if (n >= (int) sizeof(expected)  &&  memcmp( snbuffer, expected, sizeof(expected) ) == 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugSkype)
	 ufdbLogMessage( "      received Skype Node Reply Message from %s:%d", hostname, portnumber );
      return UFDB_API_ERR_IS_SKYPE;
   }

   if (n < 0)
   {
      if (UFDBglobalDebugSkype)
	 ufdbLogMessage( "      read failed from %s:%d: n=%d, %s", hostname, portnumber, n, strerror(errno) );
      return UFDB_API_ERR_READ;
   }
   else
   {
      if (UFDBglobalDebugSkype)
	 ufdbLogMessage( "      got reply of %d bytes to Skype Node probe from %s:%d", n, hostname, portnumber );
#if UFDB_DEBUG_PROBES
      if (UFDBglobalDebugSkype)
      {
	 int i;
	 if (UFDBglobalDebug == 0)
	    if (n > 20)
	       n = 20;
	 for (i = 0; i < n; i++)
	    ufdbLogMessage( "      reply byte %2d, 0x%02x  %c", i, snbuffer[i], (snbuffer[i] & 0x7f)|0x20 );
      }
#endif
   }

   return UFDB_API_OK;
}


static const char * TLSalert2string( int alertCode )
{
   switch (alertCode)
   {
       case   0:   return "close notify";
       case  10:   return "unexpected message";
       case  20:   return "bad record mac";
       case  21:   return "decryption failed";
       case  22:   return "record overflow";
       case  30:   return "decompression failure";
       case  40:   return "handshake failure";
       case  41:   return "no certificate";
       case  42:   return "bad certificate";
       case  43:   return "unsupported certificate";
       case  44:   return "certificate revoked";
       case  45:   return "certificate expired";
       case  46:   return "certificate unknown";
       case  47:   return "illegal parameter";
       case  48:   return "unknown ca";
       case  49:   return "access denied";
       case  50:   return "decode error";
       case  51:   return "decrypt error";
       case  60:   return "export restriction";
       case  70:   return "protocol version";
       case  71:   return "insufficient security";
       case  80:   return "internal error";
       case  90:   return "user canceled";
       case 100:   return "no renegotiation";
       case 110:   return "unsupported extension";
       case 111:   return "certificate unobtainable";
       case 112:   return "unrecognized name";
       case 113:   return "bad certificate status response";
       case 114:   return "bad certificate hash value";
   }
   return "unknown";
}


/* 
 * Detect Gtalk protocol for hostname:port.
 * example of gtalk server on port 443: 209.85.157.126
 * returns  UFDB_API_OK, UFBD_API_ERR_IS_GTALK, UFDB_API_ERR_TUNNEL or UFDB_API_ERR_SOCKET.
 */
int UFDBdetectGtalk( 
   const char *    hostname, 
   int             portnumber )
{
   int             s;
   int             n;
   struct timeval  tv;
   unsigned char   loginbuffer[] = { 
      0x80, 0x46, 0x01, 0x03, 0x01, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x80, 0x03, 0x00, 
      0x80, 0x07, 0x00, 0xc0, 0x06, 0x00, 0x40, 0x02, 0x00, 0x80, 0x04, 0x00, 0x80, 0x00, 0x00, 0x04, 
      0x00, 0xfe, 0xff, 0x00, 0x00, 0x0a, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x09, 0x00, 0x00, 0x64, 0x00, 
      0x00, 0x62, 0x00, 0x00, 0x03, 0x00, 0x00, 0x06, 
      0x1f, 0x17, 0x0c, 0xa6, 0x2f, 0x00, 0x78, 0xfc, 0x46, 0x55, 0x2e, 0xb1, 0x83, 0x39, 0xf1, 0xea
      };
   unsigned char   expected[30] = {
      0x16, 0x03, 0x01, 0x00, 0x4a, 0x02, 0x00, 0x00, 0x46, 0x03, 0x01, 0x42, 0x85, 0x45, 0xa7, 0x27,
      0xa9, 0x5d, 0xa0, 0xb3, 0xc5, 0xe7, 0x53, 0xda, 0x48, 0x2b, 0x3f, 0xc6, 0x5a, 0xca
      };
   unsigned char   TLSalert[6] = {
      0x15, 0x03, 0x01, 0x00, 0x02, 0x02
      };
   unsigned char   reply[300];

   if (UFDBglobalDebug  ||  UFDBglobalDebugGtalk)
      ufdbLogMessage( "   probing for Gtalk on %s:%d ...", hostname, portnumber );

   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugGtalk)
	 ufdbLogMessage( "     socket open failed for %s:%d", hostname, portnumber );
      return UFDB_API_ERR_SOCKET;
   }

   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   if (72 != write( s, loginbuffer, 72 ))
   {
      if (UFDBglobalDebug || UFDBglobalDebugGtalk)
	 ufdbLogMessage( "      write to %s:%d failed: %s", hostname, portnumber, strerror(errno) );
      close( s );
      return UFDB_API_ERR_SOCKET;
   }

   n = read( s, reply, sizeof(reply) );
   close( s );

   if (n >= (int) sizeof(expected)  &&  memcmp( reply, expected, sizeof(expected) ) == 0)
   {
      if (UFDBglobalDebug  ||  UFDBglobalDebugGtalk)
	 ufdbLogMessage( "      received Gtalk Reply Message from %s:%d", hostname, portnumber );
      return UFDB_API_ERR_IS_GTALK;
   }

   if (n >= (int) sizeof(TLSalert)+1  &&  memcmp( reply, TLSalert, sizeof(TLSalert) ) == 0)
   {
      ufdbLogMessage( "received a TLS protocol alert %d (%s) from %s:%d ", 
                      (int) reply[6], TLSalert2string((int) reply[6]), hostname, portnumber );
      return UFDB_API_ERR_TLS;
   }

   if (n < 0)
   {
      if (UFDBglobalDebugGtalk)
	 ufdbLogMessage( "      read from %s:%d failed: n=%d, %s", hostname, portnumber, n, strerror(errno) );
      return UFDB_API_ERR_READ;
   }
   else
   {
      if (UFDBglobalDebugGtalk)
	 ufdbLogMessage( "      Got reply of %d bytes to Gtalk probe message from %s:%d.", n, hostname, portnumber );

      if (strncmp( (char *) reply, "HTTP/", 5 ) == 0  &&  (reply[5] >= '0' && reply[5] <= '2'))
      {
	 ufdbLogMessage( "received a plain HTTP message from %s:%d", hostname, portnumber );
	 return UFDB_API_ERR_TLS;
      }
      else if (strncmp( (char *) reply, "SSH", 3 ) == 0)
      {
	 ufdbLogMessage( "%s:%d responded with SSH so it has a TUNNEL.", hostname, portnumber );
	 return UFDB_API_ERR_TUNNEL;
      }
      else if (n > 80)
      {
	 reply[ sizeof(reply)-1 ] = '\0';
	 if (strncasecmp( (char*) reply, "<?xml", 5 ) == 0  &&
	     strstr( (char*) reply, "<stream:stream" ) != NULL  &&
	     strstr( (char*) reply, "from=\"chat.facebook.com" ) != NULL)
	 {
	    if (UFDBglobalDebug  ||  UFDBglobalDebugFBchat)
	       ufdbLogMessage( "received Facebook Chat Reply Message from %s:%d", hostname, portnumber );
	    return UFDB_API_ERR_IS_FBCHAT;
	 }
      }
#if UFDB_DEBUG_PROBES
      else 
      {
	 /* it is not a Gtalk server, print the reply */
         if (UFDBglobalDebugGtalk)
	 {
	    int i;
	    if (UFDBglobalDebug == 0)
	       if (n > 200)
		  n = 200;
	    for (i = 0; i < n; i++)
	       ufdbLogMessage( "      gtalk probe reply byte %2d, 0x%02x  %c", i, reply[i], (reply[i] & 0x7f)|0x20 );
	 }
      }
#endif
   }

   return UFDB_API_OK;
}


/* 
 * Detect Skype protocol for hostname:port.
 * returns  UFDB_API_OK, UFBD_API_ERR_IS_SKYPE, UFDB_API_ERR_TUNNEL or UFDB_API_ERR_SOCKET.
 */
int UFDBdetectSkype( 
   const char *    hostname, 
   int             portnumber )
{
#if UFDB_PROBE_SKYPE_LOGIN_NODE
   int             n;
   int  	   s;
   struct timeval  tv;
   unsigned char   loginbuffer[100] = { 
      0x16, 0x03, 0x01, 0x00, 0x00,
      0x80, 0x46, 0x01, 0x03, 0x01, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x05, 0x00, 0x00,
      0x04, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x09, 0x00, 0x00, 0x64, 0x00, 0x00, 0x62, 0x00, 0x00, 0x08,
      0x00, 0x00, 0x03, 0x00, 0x00, 0x06, 0x01, 0x00, 0x80, 0x07, 0x00, 0xc0, 0x03, 0x00, 0x80, 0x06,
      0x00, 0x40, 0x02, 0x00, 0x80, 0x04, 0x00, 0x80,
      0x16, 0x03, 0x01, 0x00, 0x00, 0x06, 0xa3, 0x66, 0x09, 0x33, 0x5d, 0x3f, 0xf2, 0xb4, 0xca, 0x44
   };
   unsigned char   expected[5] = {
      0x17, 0x03, 0x01, 0x00, 0x00
   };

   if (UFDBglobalDebug  ||  UFDBglobalDebugSkype)
      ufdbLogMessage( "   probing for Skype login on %s:%d ...", hostname, portnumber );

   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugSkype)
	 ufdbLogMessage( "     socket open failed for %s:%d", hostname, portnumber );
      return UFDB_API_ERR_SOCKET;
   }

   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   /* Skype login server detection */

   if (72 != write( s, loginbuffer, 72 ))
   {
      if (UFDBglobalDebug || UFDBglobalDebugSkype)
	 ufdbLogMessage( "      write to %s:%d failed: %s", hostname, portnumber, strerror(errno) );
      close( s );
      return UFDB_API_ERR_READ;
   }

   n = read( s, loginbuffer, 30 );
   close( s );
   
#if UFDB_DEBUG_PROBES
   if (UFDBglobalDebugSkype)
   {
      int i;
      if (UFDBglobalDebug == 0)
	 if (n > 20)
	    n = 20;
      for (i = 0; i < n; i++)
	 ufdbLogMessage( "      skype server probe reply byte %2d, 0x%02x  %c", i, loginbuffer[i], (loginbuffer[i] & 0x7f)|0x20 );
   }
#endif

   if (n >= (int) sizeof(expected)  &&  memcmp( loginbuffer, expected, sizeof(expected) ) == 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugSkype)
	 ufdbLogMessage( "      received Skype Login Reply Message from %s:%d", hostname, portnumber );
      return UFDB_API_ERR_IS_SKYPE;
   }

   if (n < 0)
   {
      if (UFDBglobalDebugSkype)
	 ufdbLogMessage( "      read from %s:%d failed: n=%d, %s", hostname, portnumber, n, strerror(errno) );
      return UFDB_API_ERR_READ;
   }
   else
   {
      if (UFDBglobalDebugSkype)
	 ufdbLogMessage( "      Got reply of %d bytes to Skype login probe from %s:%d.", n, hostname, portnumber );

      if (strncmp( (char *) loginbuffer, "SSH", 3 ) == 0)
      {
	 ufdbLogMessage( "%s:%d responded with SSH so it has a TUNNEL.", hostname, portnumber );
	 return UFDB_API_ERR_TUNNEL;
      }
      else 
      {
	 /* it is not a Skype login server, try to detect a Skype node */
	 return detectSkypeSN( hostname, portnumber );
      }
   }

#else
   /* !UFDB_PROBE_SKYPE_LOGIN_NODE */
   return detectSkypeSN( hostname, portnumber );
#endif

   return UFDB_API_OK;
}


int UFDBdetectSSH( 
   const char *    hostname, 
   int             portnumber )
{
   int             n;
   int  	   s;
   struct timeval  tv;
   char            loginbuffer[8];

   if (UFDBglobalDebug)
      ufdbLogMessage( "   probing for SSH server on %s:%d ...", hostname, portnumber );

   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "     opening of socket failed for %s:%d", hostname, portnumber );
      return UFDB_API_ERR_SOCKET;
   }

   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   /* SSH server detection */

   n = read( s, loginbuffer, 4 );
   close( s );
   
   if (n >= 3  &&  strncmp( loginbuffer, "SSH", 3 ) == 0)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "      received SSH Server Message from %s:%d", hostname, portnumber );
      return UFDB_API_ERR_TUNNEL;
   }

   return UFDB_API_OK;
}


UFDB_GCC_INLINE
static const char * sslerr2str( int errnum )
{
   switch (errnum)
   {
      case SSL_ERROR_NONE:             return "SSL_ERROR_NONE";
      case SSL_ERROR_SSL:              return "SSL_ERROR_SSL";
      case SSL_ERROR_WANT_READ:        return "SSL_ERROR_WANT_READ";
      case SSL_ERROR_WANT_WRITE:       return "SSL_ERROR_WANT_WRITE";
      case SSL_ERROR_WANT_X509_LOOKUP: return "SSL_ERROR_WANT_X509_LOOKUP";
      case SSL_ERROR_SYSCALL:          return "SSL_ERROR_SYSCALL";
      case SSL_ERROR_ZERO_RETURN:      return "SSL_ERROR_ZERO_RETURN";
      case SSL_ERROR_WANT_CONNECT:     return "SSL_ERROR_WANT_CONNECT";
      case SSL_ERROR_WANT_ACCEPT:      return "SSL_ERROR_WANT_ACCEPT";
   }
   return "unknown";
}


static int probePort( 
   const char * hostname, 
   int          portnumber )
{
   int    connect_status;
   int    otherStat;

   if (UFDBglobalDebug)
      ufdbLogMessage( "   probePort %s:%d", hostname, portnumber );

   otherStat = UFDB_API_OK;

   if (hostnameIsIP(hostname))
   {
      otherStat = UFDBdetectGtalk( hostname, portnumber );
      if (otherStat == UFDB_API_ERR_IS_GTALK)
      {
	 connect_status = otherStat;
	 ufdbLogMessage( "Gtalk detected on %s:%d", hostname, portnumber );
	 goto done;
      }
      else if (otherStat == UFDB_API_ERR_IS_FBCHAT)
      {
	 connect_status = otherStat;
	 ufdbLogMessage( "Facebook Chat detected on %s:%d", hostname, portnumber );
	 goto done;
      }
      else if (otherStat == UFDB_API_ERR_TUNNEL)
      {
	 connect_status = otherStat;
	 ufdbLogMessage( "Tunnel detected on %s:%d", hostname, portnumber );
	 goto done;
      }
      else if (otherStat == UFDB_API_ERR_TLS)
      {
	 connect_status = otherStat;
	 goto done;
      }

      if (otherStat == UFDB_API_ERR_SOCKET)
      {
	 connect_status = otherStat;
	 goto done;
      }
      else
      {
	 if (UFDBglobalReconfig  ||  UFDBglobalTerminating)
	 {
	    ufdbLogMessage( "aborting HTTPS probe because ufdbguardd is reloading its configuration" );
	    connect_status = UFDB_API_ERR_SOCKET;
	    goto done;
	 }

	 otherStat = UFDBdetectSkype( hostname, portnumber );
	 if (otherStat == UFDB_API_ERR_IS_SKYPE)
	 {
	    connect_status = otherStat;
	    ufdbLogMessage( "Skype detected on %s:%d", hostname, portnumber );
	    goto done;
	 }
	 else if (otherStat == UFDB_API_ERR_TUNNEL)
	 {
	    connect_status = otherStat;
	    ufdbLogMessage( "Tunnel detected on %s:%d", hostname, portnumber );
	    goto done;
	 }
      }
   }

   /* NOTE: UFDBdetectSkype() and UFDBdetectGtalk() also detect SSH tunnels,
    * so we only call UFDBdetectSSH() when hostname is not an IP address.
    */
   if (otherStat != UFDB_API_ERR_SOCKET  &&
       !hostnameIsIP(hostname)  &&
       !UFDBglobalReconfig  &&  !UFDBglobalTerminating  &&
       UFDBdetectSSH( hostname, portnumber ) == UFDB_API_ERR_TUNNEL)
   {
      ufdbLogMessage( "SSH detected on %s:%d", hostname, portnumber );
      connect_status = UFDB_API_ERR_TUNNEL;
      goto done;
   }

   /* we have an unknown protocol.  It is dealt with below */

   if (UFDBglobalUnknownProtocolOverHttps)
   {
      connect_status = UFDB_API_ERR_UNKNOWN_PROTOCOL;
      ufdbLogMessage( "Unknown Protocol is used on %s:%d", hostname, portnumber );
      goto done;
   }
   else
   {
      /* There is an unknown protocol and the option allow-unknown-protocol-over-https is off */
      connect_status = UFDB_API_ERR_TUNNEL;
      ufdbLogMessage( "forbidden Unknown Protocol used on %s:%d so it is flagged as a TUNNEL  *****", 
		      hostname, portnumber  );
      goto done;
   }

   return UFDB_API_OK;

done:
   return connect_status;
}


int UFDBopenssl_read( char * buf, int bufsize, SSL * ssl )
{
   SSL_MUTEX_FN_INIT
   int  ret;

   SSL_MUTEX_LOCK( "UFDBopenssl_read" )

   errno = 0;
   ret = -2;
   do
   {
      ret = SSL_read( ssl, buf, bufsize );
   }
   while (ret == -1  &&
	  SSL_get_error(ssl, ret) == SSL_ERROR_SYSCALL  &&
	  errno == EINTR);

   SSL_MUTEX_UNLOCK( "UFDBopenssl_read" )

   return ret;
}


int UFDBopenssl_write( char * buf,  int bufsize,  SSL * ssl )
{
   SSL_MUTEX_FN_INIT
   int  ret;

   SSL_MUTEX_LOCK( "UFDBopenssl_write" )

   errno = 0;
   do
   {
      ret = SSL_write( ssl, buf, bufsize );
   }
   while (ret == -1  &&
	  SSL_get_error(ssl,ret) == SSL_ERROR_SYSCALL  &&
	  errno == EINTR);

   SSL_MUTEX_UNLOCK( "UFDBopenssl_write" )

   return ret;
}


void UFDBopenssl_close( SSL * ssl )
{
   SSL_MUTEX_FN_INIT

   if (ssl == NULL)
      return;

   SSL_MUTEX_LOCK( "openssl_close" )

#if 0
   ERR_clear_error();
#endif
   if (SSL_shutdown( ssl ) < 0)
      ERR_clear_error();
   SSL_free( ssl );

   SSL_MUTEX_UNLOCK( "openssl_close" )
}


static void print_errors( void )
{
   unsigned long sslerr;
   const char *  file;
   int           line;
   const char *  data;
   int           flags;
   int           header = 0;
   char          errstring[256];

   header = 0;
   while ((sslerr = ERR_get_error_line_data( &file, &line, &data, &flags )) != 0)
   {
      if (!header)
      {
	 ufdbLogError( "TLS/SSL connection failed with OpenSSL error:" );
	 header = 1;
      }

      ERR_error_string_n( sslerr, errstring, sizeof(errstring) );
      ufdbLogMessage( "   SSL error: %s", errstring );
   }
}


int UFDBloadAllowedHTTPSsites( 
   char * filename )
{
   return UFDB_API_OK;
}


static int purgeHTTPSinfo( const void * key, const void * value )
{
   struct httpsInfo * info;

   info = (struct httpsInfo *) value;
   if (info->t < OldestInHTTPScache)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "      key %s purged", (char*) key );
      ufdbFree( info->content );
      /* caller frees key and value */
      return 1;
   }

   return 0;
}


int UFDBread( 
   int	  fd, 
   char * buffer, 
   int    max )
{
   int    n;
   int    total;

   total = 0;
   while ((n = read( fd, buffer, max )) > 0)
   {
      total += n;
      buffer += n;
      max -= n;
   }
   if (total == 0)
      return n;
   return total;
}


static unsigned int hostname2hash( 
   const void * key )
{
   unsigned int hash;
   const char * hostname = (const char *) key;

   hash = 0x55;
   while (*hostname != '\0')
   {
      hash = ((hash << 4) ^ ((unsigned int) (*hostname))) - 17;
      hostname++;
   }

   return hash & 0x7FFFFFFF;
}


static int mystrcmp( const void * a, const void * b )
{
   if (a == NULL  ||  b == NULL)
   {
      ufdbLogError( "mystrcmp: a and/or b is NULL" );
      return 0;
   }
   return strcmp( (const char *) a, (const char *) b ) == 0;
}


void UFDBsetTunnelCheckMethod( int method )
{
   UFDBglobalTunnelCheckMethod = method;
}


static void ufdb_pthread_locking_callback( 
   int          mode, 
   int          type, 
   const char * file, 
   int          line )
{
   int          ret;	/* must ignore warning about variable set but not used */

#if 0
   if (UFDBglobalDebug)
      ufdbLogMessage( "      CRYPTO lock %6s %s%s %2d", 
                      (mode & CRYPTO_LOCK) ? "lock" : "unlock", 
		      (mode & CRYPTO_READ) ? "read" : "",
		      (mode & CRYPTO_WRITE) ? "write" : "",
		      type );
#endif

   if (type < 0  || type > num_static_ssl_locks)
   {
      ufdbLogMessage( "CRYPTO lock type %d out of range (max=%d)  *****", type, num_static_ssl_locks );
      return;
   }

   if (mode & CRYPTO_LOCK)
   {
      ret = pthread_mutex_lock( &crypto_mutexes[type] );
      if (ret != 0)
#ifdef UFDB_DEBUG
	 ufdbLogError( "ufdb_pthread_locking_callback: mutex_lock[%d] failed with code %d", type, ret );
#else
         ;
#endif
   }
   else if (mode & CRYPTO_UNLOCK)
   {
      ret = pthread_mutex_unlock( &crypto_mutexes[type] );
      if (ret != 0)
#ifdef UFDB_DEBUG
	 ufdbLogError( "ufdb_pthread_locking_callback: mutex_unlock[%d] failed with code %d", type, ret );
#else
         ;
#endif
   }
   else
      ufdbLogError( "ufdb_pthread_locking_callback: no LOCK|UNLOCK for type %d", type );
}


static unsigned long ufdb_pthread_id_callback( void )
{
   unsigned long id = (unsigned long) pthread_self();

   return id;
}


typedef struct CRYPTO_dynlock_value {
   pthread_mutex_t lock;
} CRYPTO_dynlock_value;


static CRYPTO_dynlock_value * ufdb_ssl_dyn_create(
   const char * file, 
   int          line )
{
   CRYPTO_dynlock_value * lp;

   lp = (CRYPTO_dynlock_value *) ufdbMalloc( sizeof(CRYPTO_dynlock_value) );
   pthread_mutex_init( &(lp->lock), NULL );
   return lp;
}


static void ufdb_ssl_dyn_lock(
   int                    mode, 
   CRYPTO_dynlock_value * lp, 
   const char *           file,
   int                    line )
{
   int                    ret;		/* must ignore warning about variable set but not used */

#if 0
   if (UFDBglobalDebug)
   {
      ufdbLogMessage( "      ufdb_ssl_dyn_lock %6s %s%s", 
                      (mode & CRYPTO_LOCK) ? "lock" : "unlock", 
		      (mode & CRYPTO_READ) ? "read" : "",
		      (mode & CRYPTO_WRITE) ? "write" : "" );
   }
#endif

   if (mode & CRYPTO_LOCK)
   {
      ret = pthread_mutex_lock( &(lp->lock) );
      if (ret != 0)
#ifdef UFDB_DEBUG
	 ufdbLogError( "ufdb_ssl_dyn_lock: mutex_lock failed with code %d", ret );
#else
         ;
#endif
   }
   else if (mode & CRYPTO_UNLOCK)
   {
      ret = pthread_mutex_unlock( &(lp->lock) );
      if (ret != 0)
#ifdef UFDB_DEBUG
	 ufdbLogError( "ufdb_ssl_dyn_lock: mutex_unlock failed with code %d", ret );
#else
         ;
#endif
   }
   else
      ufdbLogError( "ufdb_ssl_dyn_lock: no LOCK|UNLOCK in mode" );
}


static void ufdb_ssl_dyn_destroy(
   CRYPTO_dynlock_value * lp,
   const char *           file,
   int                    line )
{
   pthread_mutex_destroy( &(lp->lock) );
}


#if UFDB_USE_EXPORT_CIPHERS
/* tmp_rsa_cb - call-back to generate ephemeral RSA key */
static RSA * tmp_rsa_cb( 
   SSL * unused_ssl,
   int   unused_export,
   int   keylength )
{
   static RSA * rsa_tmp;

   /* Code adapted from OpenSSL apps/s_cb.c */

   if (rsa_tmp == 0)
     rsa_tmp = RSA_generate_key( keylength, RSA_F4, NULL, NULL );
   return rsa_tmp;
}
#endif


/* openssl_bug_bits - SSL bug compatibility bits for this OpenSSL version */
static long openssl_bug_bits( void )
{
    long    bits = SSL_OP_ALL | SSL_OP_NO_SSLv3;          /* Work around all known bugs and prevent SSLv3 */

    return bits;
}


/* tls_version_split - Split OpenSSL version number into major, minor, ... */

static void tls_version_split( 
   long        version, 
   TLS_VINFO * info )
{
    /*
     * OPENSSL_VERSION_NUMBER(3):
     * 
     * OPENSSL_VERSION_NUMBER is a numeric release version identifier:
     * 
     * MMNNFFPPS: major minor fix patch status
     * 
     * The status nibble has one of the values 0 for development, 1 to e for
     * betas 1 to 14, and f for release. Parsed OpenSSL version number. for
     * example
     * 
     * 0x000906000 == 0.9.6 dev 
     * 0x000906023 == 0.9.6b beta 3 
     * 0x00090605f == 0.9.6e release
     * 
     * Versions prior to 0.9.3 have identifiers < 0x0930.  Versions between
     * 0.9.3 and 0.9.5 had a version identifier with this interpretation:
     * 
     * MMNNFFRBB major minor fix final beta/patch
     * 
     * for example
     * 
     * 0x000904100 == 0.9.4 release 
     * 0x000905000 == 0.9.5 dev
     * 
     * Version 0.9.5a had an interim interpretation that is like the current
     * one, except the patch level got the highest bit set, to keep continu-
     * ity.  The number was therefore 0x0090581f.
     */

    if (version < 0x0930) {
	info->status = 0;
	info->patch = version & 0x0f;
	version >>= 4;
	info->micro = version & 0x0f;
	version >>= 4;
	info->minor = version & 0x0f;
	version >>= 4;
	info->major = version & 0x0f;
    } else if (version < 0x00905800L) {
	info->patch = version & 0xff;
	version >>= 8;
	info->status = version & 0xf;
	version >>= 4;
	info->micro = version & 0xff;
	version >>= 8;
	info->minor = version & 0xff;
	version >>= 8;
	info->major = version & 0xff;
    } else {
	info->status = version & 0xf;
	version >>= 4;
	info->patch = version & 0xff;
	version >>= 8;
	info->micro = version & 0xff;
	version >>= 8;
	info->minor = version & 0xff;
	version >>= 8;
	info->major = version & 0xff;
	if (version < 0x00906000L)
	    info->patch &= ~0x80;
    }
}


static void OpenSSLversionCheck( void )
{
   TLS_VINFO hdr_info;
   TLS_VINFO lib_info;

   tls_version_split( OPENSSL_VERSION_NUMBER, &hdr_info );
   tls_version_split( SSLeay(), &lib_info );

   if ( lib_info.major != hdr_info.major ||
        lib_info.minor != hdr_info.minor ||
        lib_info.micro != hdr_info.micro)
   {
      ufdbLogMessage( "WARNING: TLS/SSL run-time library vs. compile-time header version mismatch:  *****\n"
		      "OpenSSL %d.%d.%d may not be compatible with OpenSSL %d.%d.%d  %s",
		      lib_info.major, lib_info.minor, lib_info.micro, 
		      hdr_info.major, hdr_info.minor, hdr_info.micro, OPENSSL_VERSION_TEXT );
   }
   else
   {
      char status[16];
      char rev[2];

      if (lib_info.status == 0x0f)
         strcpy( status, "R" );
      else
         sprintf( status, "beta %d", lib_info.status );
      if (lib_info.patch == 0)
         rev[0] = '\0';
      else
      {
         rev[0] = lib_info.patch - 1 + 'a';
	 rev[1] = '\0';
      }

      ufdbLogMessage( "using OpenSSL library %d.%d.%d%s %s (%s)",
		      lib_info.major, lib_info.minor, lib_info.micro, rev, status, OPENSSL_VERSION_TEXT );
   }
}


static void waitForTLSinitilisation( void )
{
   static volatile int didwait = 0;
   int i;
   
   if (didwait)
      return;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "waitForTLSinitilisation: tls_inited is %d  ssl_ctx is %sNULL ...",
		      tls_inited, ssl_ctx != NULL ? "not " : "" );

   if (tls_inited >= 3)
   {
      didwait = 1;
      return;
   }

   if (tls_inited == 2  &&  ssl_ctx == NULL) 
   {
      didwait = 1;
      return;
   }

   /* the HTTPS initialisation may be delayed by the sgReadConfig() which may take several minutes to complete */
   /* the crash report uploader may be faster than the TLS/SSL initialisation so we sleep max 30 seconds */
   for (i = 0;  i < 30;  i++)
   {
      sleep( 1 );
      if (tls_inited >= 3)
	 break;
   }
   didwait = 1;

   if (ssl_ctx == NULL)
   {
      ufdbLogError( "waitForTLSinitilisation: after waiting: tls_inited is %d, ssl_ctx is NULL  *****\n"
                    "No HTTPS connections can be set up yet.",
		    tls_inited );
   }
}


int UFDBinitHTTPSchecker( void )
{
   struct stat  statres;

   if (tls_inited == 0)
   {
      int    i;
      char * dbdirectory;

      pthread_mutex_lock( &init_mutex );
      if (tls_inited != 0)
      {
	 pthread_mutex_unlock( &init_mutex );
	 usleep( 10000 );
	 return UFDB_API_OK;
      }
      tls_inited = 1;

      if (UFDBglobalDebug)
         ufdbLogMessage( "UFDBinitHTTPSchecker started" );

      myht = UFDBcreateHashtable( 317, hostname2hash, mystrcmp );
      lastHTTPScachePurgeTime = time( NULL );

      srandom( ((getpid() << 8) ^ lastHTTPScachePurgeTime) + ((unsigned long)myht << 26) );

      OpenSSLversionCheck();

      SSL_load_error_strings();
      SSL_library_init();
      /* 
       * The SSL_library_init man page does not mention which "old" version... 
       * But some "old" versions need to call OpenSSL_add_all_algorithms()
       */
      OpenSSL_add_all_algorithms();	
      OPENSSL_config( NULL );
      ENGINE_load_builtin_engines();

      CRYPTO_set_id_callback( ufdb_pthread_id_callback );	
      num_static_ssl_locks = CRYPTO_num_locks();
      crypto_mutexes = (pthread_mutex_t *) ufdbMalloc( (num_static_ssl_locks+1) * sizeof(pthread_mutex_t) );
      for (i = 0; i <= num_static_ssl_locks; i++)
         pthread_mutex_init( &crypto_mutexes[i], NULL );
      CRYPTO_set_locking_callback( ufdb_pthread_locking_callback );

      CRYPTO_set_dynlock_create_callback( ufdb_ssl_dyn_create );
      CRYPTO_set_dynlock_lock_callback( ufdb_ssl_dyn_lock );
      CRYPTO_set_dynlock_destroy_callback( ufdb_ssl_dyn_destroy );

      ssl_ctx = SSL_CTX_new( SSLv23_client_method() );
      SSL_CTX_set_options( ssl_ctx, SSL_OP_NO_SSLv2 );
      SSL_CTX_set_options( ssl_ctx, SSL_OP_NO_SSLv3 );
#if 0
      if (UFDBglobalPeek)
      {
         SSL_CTX_set_options( ssl_ctx, SSL_OP_NO_TLSv1 );
         SSL_CTX_set_options( ssl_ctx, SSL_OP_NO_TLSv1_1 );
      }
#endif

      if (ssl_ctx != NULL)
      {
	 SSL_CTX_set_options( ssl_ctx, openssl_bug_bits() );
	 SSL_CTX_set_session_cache_mode( ssl_ctx, SSL_SESS_CACHE_OFF );
	 SSL_CTX_set_timeout( ssl_ctx, 120 );
	 SSL_CTX_set_default_verify_paths( ssl_ctx );
	 SSL_CTX_set_verify_depth( ssl_ctx, 12 );
         tls_inited = 2;
      }

      if (UFDBglobalDebug)
         ufdbLogMessage( "UFDBinitHTTPSchecker: tls_inited is %d", tls_inited );

      if (ssl_ctx == NULL)
      {
	 UFDBglobalHttpsOfficialCertificate = 0;
	 tls_inited = 5;
	 errno = EINVAL;
	 pthread_mutex_unlock( &init_mutex );
         return UFDB_API_ERR_ERRNO;
      }

      if (UFDBglobalCAcertsFile[0] == '\0'  &&  UFDBglobalCAcertsDir[0] == '\0')
      {
         ufdbLogMessage( "CA certificates are not defined, using default file" );
	 dbdirectory = ufdbSettingGetValue( "dbhome" );
	 if (dbdirectory == NULL)
	    dbdirectory = (char *) DEFAULT_DBHOME;
	 strcpy( UFDBglobalCAcertsFile, dbdirectory );
	 strcat( UFDBglobalCAcertsFile, "/security/cacerts" );
      }
      if (UFDBglobalDebug)
	 ufdbLogMessage( "UFDBinitHTTPSchecker: CA certficates are in file \"%s\" and/or directory \"%s\"", 
	                 UFDBglobalCAcertsFile, UFDBglobalCAcertsDir==NULL ? "none" : UFDBglobalCAcertsDir );
      if (UFDBglobalCAcertsFile[0] != '\0'  &&   stat( UFDBglobalCAcertsFile, &statres ) < 0)
      {
         ufdbLogError( "SSL library initialisation failed because of a problem with the CA certificate file:  *****\n"
	               "file: \"%s\", error: %s", UFDBglobalCAcertsFile, strerror(errno) );
	 if (UFDBglobalHttpsOfficialCertificate)
	 {
	    ufdbLogFatalError( "Cannot perform mandatory check of TLS/SSL certificates." );
	    ufdbLogMessage( "Fix the problem  ***** " );
	    ufdbLogMessage( "In case that there is no cacerts file, option enforce-https-official-certificate must be \"off\"" );
	 }
	 tls_inited = 6;
	 pthread_mutex_unlock( &init_mutex );
	 errno = ENOENT;
         return UFDB_API_ERR_ERRNO;
      }
      
      ufdbCacertsLoaded = SSL_CTX_load_verify_locations( ssl_ctx, 
                             UFDBglobalCAcertsFile[0] == '\0' ? NULL : UFDBglobalCAcertsFile, 
			     UFDBglobalCAcertsDir[0] == '\0' ? NULL : UFDBglobalCAcertsDir  ); 
      if (ufdbCacertsLoaded != 1)
      {
         ufdbLogError( "Failure to load the CA database; TLS/SSL certificates cannot be verified  *****\n"
                       "CA file is \"%s\" and CA directory is \"%s\"", 
                             UFDBglobalCAcertsFile[0] == '\0' ? "none" : UFDBglobalCAcertsFile, 
			     UFDBglobalCAcertsDir[0] == '\0' ? "none" : UFDBglobalCAcertsDir  ); 
	 if (UFDBglobalHttpsOfficialCertificate)
	 {
	    ufdbLogFatalError( "Cannot perform mandatory check of TLS/SSL certificates." );
	    ufdbLogMessage( "ABORTING.  Fix the problem and start ufdbguardd again." );
	    ufdbLogMessage( "In case that there is no cacerts file, option enforce-https-official-certificate must be \"off\"" );
	 }
	 tls_inited = 7;
	 pthread_mutex_unlock( &init_mutex );
	 errno = ENOENT;
         return UFDB_API_ERR_ERRNO;
      }
      else
      {
	 ufdbLogMessage( "HTTPS/SSL verification with trusted certificates from file \"%s\" and directory \"%s\"", 
                             UFDBglobalCAcertsFile[0] == '\0' ? "none" : UFDBglobalCAcertsFile, 
			     UFDBglobalCAcertsDir[0] == '\0' ? "none" : UFDBglobalCAcertsDir  ); 
      }

#if UFDB_USE_EXPORT_CIPHERS
      /*
       * According to the OpenSSL documentation, temporary RSA key is needed
       * when export ciphers are in use. 
       */
      SSL_CTX_set_tmp_rsa_callback( ssl_ctx, tmp_rsa_cb );
#endif

      /* use SSL_VERIFY_NONE and verify the certificate in ssl_check_certificate */
      SSL_CTX_set_verify( ssl_ctx, SSL_VERIFY_NONE, NULL );

#if 0
      SSL_CTX_set_mode( ssl_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE );
#endif
      SSL_CTX_set_mode( ssl_ctx, SSL_MODE_AUTO_RETRY );

      tls_inited = 3;
      if (UFDBglobalDebug)
         ufdbLogMessage( "UFDBinitHTTPSchecker finished: tls_inited is %d", tls_inited );
      pthread_mutex_unlock( &init_mutex );
   }

   return UFDB_API_OK;
}


static const char * sslver2string( 
   int version )
{
   switch (version)
   {
      case SSL2_VERSION:   return "SSLv2";
      case SSL3_VERSION:   return "SSLv3";
      case TLS1_VERSION:   return "TLSv1.0";

#ifdef TLS1_1_VERSION
      case TLS1_1_VERSION: return "TLSv1.1";
#endif

#ifdef TLS1_2_VERSION
      case TLS1_2_VERSION: return "TLSv1.2";
#endif

#ifdef TLS1_3_VERSION
      case TLS1_3_VERSION: return "TLSv1.3";
#endif

#ifdef TLS2_0_VERSION
      case TLS2_0_VERSION: return "TLSv2.0";
#endif

#ifdef DTLS1_VERSION
      case DTLS1_VERSION:  return "DTLSv1";
#endif
   }

   return "TLSv0";
}


/* status return value is one of
 * UFDB_API_OK
 * UFDB_API_ERR_INVALID_CERT
 * UFDB_API_ERR_IS_AIM  UFDB_API_ERR_IS_YAHOOMSG  UFDB_API_ERR_IS_SKYPE  UFDB_API_ERR_IS_GTALK  UFDB_API_ERR_IS_FBCHAT
 * UFDB_API_ERR_IS_CITRIXONLINE
 * UFDB_API_ERR_IS_TUNNEL
 * UFDB_API_ERR_UNKNOWN_PROTOCOL
 * UFDB_API_ERR_NULL
 * UFDB_API_ERR_SOCKET
 * UFDB_API_ERR_IP_ADDRESS
 */
static char * httpsGETroot( 
   int          s,
   const char * hostname, 
   int          portnumber, 
   int *        certErrors,
   char *       cn,
   int *        status )
{
   int          n;
   int          m;
   SSL_MUTEX_FN_INIT
   SSL *        ssl;
   const char * method;
   char         request[2048];
   char         reply[32*1024+4];

   /*******************************
   GET / HTTP/1.0
   User-Agent: Mozilla/4.0 (xxx) Gecko/20100722 Firefox/3.6.8
   Host: www.urlfilterdb.com:9443
   Accept: * / *
   Connection: Keep-Alive
   ********************************/

   if (hostname == NULL)
   {
      *status = UFDB_API_ERR_FATAL;
      ufdbLogError( "httpsGETroot: hostname is NULL" );
      return NULL;
   }

   waitForTLSinitilisation();		/* wait for UFDBinitHTTPSchecker() to finish */
   if (ssl_ctx == NULL)
   {
      *status = UFDB_API_ERR_FATAL;
      ufdbLogError( "httpsGETroot: ssl_ctx is NULL" );
      return NULL;
   }

   /* UFDBopenssl_connect returns OK (have SSL), SKYPE (detected Skype), or UNKNOWN_PROTOCOL or TUNNEL */
   request[0] = '\0';
   reply[0] = '\0';
   ssl = NULL;
   n = UFDBopenssl_connect( hostname, portnumber, s, &ssl );
   if (n != UFDB_API_OK)
   {
      if (n == UFDB_API_ERR_TLS  &&  !UFDBglobalReconfig)
      {
         n = probePort( hostname, portnumber );
         if (n == UFDB_API_OK)
            n = UFDB_API_ERR_TLS;
      }

      ufdbLogMessage( "SSL connect failure to %s:%d.  The server %s.", 
		      hostname, portnumber,
		      (n==UFDB_API_ERR_IS_AIM) ? "has AIM" : 
			 (n==UFDB_API_ERR_IS_GTALK) ? "has Google Talk" : 
			    (n==UFDB_API_ERR_IS_SKYPE) ? "has Skype" : 
			       (n==UFDB_API_ERR_IS_YAHOOMSG) ? "has Yahoo IM" : 
				  (n==UFDB_API_ERR_IS_FBCHAT) ? "has Facebook Chat" : 
				     (n==UFDB_API_ERR_IS_CITRIXONLINE) ? "has CitrixOnline" : 
					(n==UFDB_API_ERR_IS_ANYDESK) ? "has AnyDesk" : 
					   (n==UFDB_API_ERR_IS_TEAMVIEWER) ? "has Teamviewer" : 
					      (n==UFDB_API_ERR_UNKNOWN_PROTOCOL) ? "uses an unknown protocol" :
						 (n==UFDB_API_ERR_NULL) ? "has a NULL connection" :
						    (n==UFDB_API_ERR_TUNNEL) ? "has a tunnel" :
						       (n==UFDB_API_ERR_SOCKET) ? "has a connection error" : 
                                                          (n==UFDB_API_ERR_TLS) ? "does not accept a TLS connection" : 
                                                             (n==UFDB_API_ERR_FATAL) ? "has a fatal connection error" : 
                                                                (n==UFDB_API_ERR_IP_ADDRESS) ? "has no FQDN but an IP address" : 
                                                                   "does not speak SSL+HTTP" );
      *status = n;
      return NULL;
   }

   method = sslver2string( ssl->version );
   if (UFDBglobalDebug)
   {
      int                cipherbits;
      const SSL_CIPHER * cipher;
      const char *       ciphername;

      cipher = SSL_get_current_cipher( ssl );
      ciphername = SSL_CIPHER_get_name( (SSL_CIPHER *) cipher );	/* cast is required for Solaris 10 */
      cipherbits = SSL_CIPHER_get_bits( (SSL_CIPHER *) cipher, &m );	/* idem */
      ufdbLogMessage( "TLS/SSL connection to %s:%d established  method %s  cipher %s  %d-bits",
		      hostname, portnumber, method, ciphername, cipherbits );
   }

   if (UFDBglobalReconfig)
   {
      ufdbLogMessage( "certificate check of %s connection to %s:%d is aborted due to reconfiguration in process", 
                      method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = UFDB_API_ERR_OUTDATED;
      return NULL;
   }

   n = ssl_check_certificate( ssl, hostname, portnumber, cn );
   if (n == UFDB_API_ERR_INVALID_CERT)
      *certErrors = UFDB_API_ERR_INVALID_CERT;
#if 1
   /* n = OK|ERR_INVALID_CERT|ERR_IS_YAHOOMSG|ERR_IS_AIM */
   if (!UFDBglobalHttpsOfficialCertificate)
   {
      if (n == UFDB_API_ERR_INVALID_CERT)
      {
	 ufdbLogMessage( "%s certificate of %s:%d has an issue but enforce-https-official-certificate is off", 
	                 method, hostname, portnumber );
	 n = UFDB_API_OK;
      }
   }
#endif

   if (n == UFDB_API_ERR_IS_AIM)
   {
      ufdbLogMessage( "%s connection to %s:%d has AIM", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_YAHOOMSG)
   {
      ufdbLogMessage( "%s connection to %s:%d has Yahoo IM", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_SKYPE)
   {
      ufdbLogMessage( "%s connection to %s:%d has Skype", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_GTALK)
   {
      ufdbLogMessage( "%s connection to %s:%d has Google Talk", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_FBCHAT)
   {
      ufdbLogMessage( "%s connection to %s:%d has Facebook Chat", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_CITRIXONLINE)
   {
      ufdbLogMessage( "%s connection to %s:%d has CitrixOnline", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_ANYDESK)
   {
      ufdbLogMessage( "%s connection to %s:%d has Anydesk", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_IS_TEAMVIEWER)
   {
      ufdbLogMessage( "%s connection to %s:%d has Teamviewer", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n == UFDB_API_ERR_TUNNEL)
   {
      ufdbLogMessage( "%s connection to %s:%d has a tunnel", method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = n;
      return NULL;
   }

   if (n != UFDB_API_OK && !UFDBglobalHttpsEnforceContentPeek)
   {
      ufdbLogMessage( "%s connection to %s:%d has error code %d. It is marked as a TLS/SSL certificate issue", 
                      method, hostname, portnumber, n );
      UFDBopenssl_close( ssl );
      *status = UFDB_API_ERR_INVALID_CERT;
      return NULL;
   }

   if (UFDBglobalReconfig)
   {
      ufdbLogMessage( "content check of %s connection to %s:%d is aborted due to reconfiguration in process", 
                      method, hostname, portnumber );
      UFDBopenssl_close( ssl );
      *status = UFDB_API_ERR_OUTDATED;
      return NULL;
   }

   *status = n;

   /* TO-DO: if there is a proxy, "CONNECT" must be used */
   /* TODO: implement Accept-Encoding */
   sprintf( request, "GET / HTTP/1.1\r\n"
                     "User-Agent: " UFDB_USER_AGENT "\r\n"
		     "Host: %s:%d\r\n"
		     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                     "Accept-Language: en-US,en;q=0.5\r\n"
                     "DNT: 1\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
		     // "Connection: close\r\n"
		     "\r\n",
		     hostname, portnumber
		     );
   n = UFDBopenssl_write( request, strlen(request), ssl );
   if (n < 0)
   {
      if (n == UFDB_API_OK)
	 *status = UFDB_API_ERR_SOCKET;
      strcpy( reply, "failed to send GET / with TLS/SSL" );
   }
   else
   {
      reply[0] = '\0';
      n = UFDBopenssl_read( reply, sizeof(reply)-4, ssl );
      if (n > 0)
      {
	 reply[n] = '\0';
#if UFDB_VALGRINDING
	 reply[n+1] = '\0';	/* to make valgrind happy */
	 reply[n+2] = '\0';
	 reply[n+3] = '\0';
#endif
         if ((UFDBglobalDebug > 1  ||  UFDBglobalPeek)  && 
             strstr( reply, " 200 " ) != NULL  &&               // HTTP code 200 must be present
             strstr( reply, "Content-Length: 0" ) == NULL)
	 {
	    /* in many cases we only got the header but we also want to see content */
	    usleep( 150000 );
	    m = UFDBopenssl_read( reply+n, sizeof(reply)-4-n, ssl );
            ufdbLogMessage( "httpsGETroot: read %d + %d bytes of content", n, m );
	    if (m > 0)
	    {
	       n += m;
	       reply[n] = '\0';
#if UFDB_VALGRINDING
	       reply[n+1] = '\0';	/* to make valgrind happy */
	       reply[n+2] = '\0';
	       reply[n+3] = '\0';
#endif
            }
	 }
      }
      else
      {
         if (n == UFDB_API_OK)
            *status = UFDB_API_ERR_SOCKET;
#if UFDB_VALGRINDING
	 reply[0+1] = '\0';	/* to make valgrind happy */
	 reply[0+2] = '\0';
	 reply[0+3] = '\0';
#endif
      }
   }
   UFDBopenssl_close( ssl );

   return ufdbStrdup( reply );
}


/* Check for a tunnel.		*** used by ufdbpeek and the API ***
 *
 * Valid flags are:
 * UFDB_API_ALLOW_QUEUING
 * UFDB_API_VERBOSE_OUTPUT
 *
 * return values are:
 * UFDB_API_OK:                   regular https traffic
 * UFDB_API_REQ_QUEUED:           request is queued for an other thread
 * UFDB_API_ERR_TUNNEL:           https channel is tunneled
 * UFDB_API_ERR_UNKNOWN_PROTOCOL: https channel is tunneled
 * UFDB_API_ERR_IS_SKYPE:         https channel is used by Skype
 * UFDB_API_ERR_IS_GTALK:         https channel is used by Gtalk
 * UFDB_API_ERR_IS_FBCHAT:        https channel is used by Facebook Chat
 * UFDB_API_ERR_IS_CITRIXONLINE:  https channel is used by CitrixOnline
 * UFDB_API_ERR_IS_ANYDESK:       https channel is used by Anydesk
 * UFDB_API_ERR_IS_TEAMVIEWER:    https channel is used by Teamviewer
 * TODO: implement UFDB_API_ERR_IS_PANDO: https channel is used by Pando Client
 * UFDB_API_ERR_CERTIFICATE:      TLS/SSL certificate is self-signed or has wrong common name
 * UFDB_API_ERR_IP_ADDRESS:       hostname is an IP address (only when flag UFDB_API_NO_IP is used)
 * UFDB_API_ERR_NULL:             tunnel verification is OFF.
 */
int UFDBcheckForHTTPStunnel( 
   const char *       hostname, 
   int                portnumber,
   int                flags )
{
   int                ret;
   struct httpsInfo * info;

   if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_CHECK_OFF)
      return UFDB_API_ERR_NULL;

   if (UFDBglobalDebug)
      ufdbLogMessage( "UFDBcheckForHTTPStunnel  %s:%d  %04x", hostname, portnumber, flags );

   if (flags & UFDB_API_ALLOW_QUEUING)
   {
      int   status;
      struct httpsInfo * hinfo;
      char  key[1024+16];

      status = lookupHTTPScache( hostname, portnumber, 1, &hinfo );
      if (status == UFDB_API_ERR_NULL)
      {
	 /* The hostname:port is entered in the hash *NOW* so that future calls
	  * know that it is already queued.
	  */
	 sprintf( key, "%s:%d", hostname, portnumber );
	 info = (struct httpsInfo *) ufdbMalloc( sizeof(struct httpsInfo) );
	 info->t = time( NULL );
	 info->status = UFDB_API_REQ_QUEUED;
	 info->content = NULL;
	 info->category[0] = '\0';
	 info->cn[0] = '\0';
	 UFDBinsertHashtable( myht, ufdbStrdup(key), (void *) info, 1 );

	 ret = ufdbHttpsQueueRequest( hostname, portnumber );
	 if (ret != UFDB_API_OK)
	 {  
	    if (ret == UFDB_API_ERR_FULL)
	    {
	       /* The queue is full. The status in the hashtable is updated to "socket error" 
	        * because this forces a removal from the cache in 5 minutes and a retry.
		*/
	       info->status = UFDB_API_ERR_SOCKET;
	       return UFDB_API_ERR_SOCKET;
	    }
	    else if (ret == UFDB_API_REQ_QUEUED)
	    {
	       /* The HTTP verification request was already queued... */
	       return UFDB_API_BEING_VERIFIED;
	    }
	    else
	       ufdbLogError( "UFDBcheckForHTTPStunnel: ufdbHttpsQueueRequest( %s:%d ) returned with code %d", 
	                     hostname, portnumber, ret );
	 }

	 /* We just queued a request for a verification thread. Be nice and yield the CPU. */
#ifdef _POSIX_PRIORITY_SCHEDULING
	 sched_yield();
#endif
	 return UFDB_API_BEING_VERIFIED;
      }

      /* TODO: deal with status == UFDB_API_ERR_OUTDATED */
      return status;
   }

   /* requests are not queued, so we perform the verification ourselves */
   return UFDBverifyPortHasHTTPS( hostname, portnumber, flags );
}


/*
 * in case that the caller of UFDBcheckForHTTPStunnel uses the UFDB_API_ALLOW_QUEUING flag,
 * the caller MUST have created 1-14 threads with UFDBhttpsTunnelVerifier() as main.
 */
void * UFDBhttpsTunnelVerifier( 
   void *    ptr )
{
   sigset_t  signals;
   int       portnumber;
   char      hostname[1028];

   /* 
    * All signals must be blocked.  
    */
   sigemptyset( &signals );
   sigaddset( &signals, SIGHUP );
   sigaddset( &signals, SIGUSR1 );
   sigaddset( &signals, SIGCHLD );
   sigaddset( &signals, SIGTERM );
   sigaddset( &signals, SIGINT );
   pthread_sigmask( SIG_BLOCK, &signals, NULL );

   ufdbSetThreadCPUaffinity( (int) ((long) ptr) );

   waitForTLSinitilisation();		/* wait for UFDBinitHTTPSchecker() to finish */

   while (!UFDBglobalTerminating)
   {
      pthread_testcancel();

      ufdbGetHttpsRequest( hostname, &portnumber );

      if (((volatile int) UFDBglobalTunnelCheckMethod) == UFDB_API_HTTPS_CHECK_OFF)
	 continue;

      pthread_testcancel();

      if (UFDBglobalDebug)
	 ufdbLogMessage( "T%02ld: UFDBhttpsTunnelVerifier: start TLS/SSL verification %s:%d ...", 
	                 (long) ptr, hostname, portnumber );
      (void) UFDBverifyPortHasHTTPS( hostname, portnumber, 0 );

      if (UFDBglobalTerminating)
	 pthread_exit( (void *) 0 );
      if (UFDBglobalReconfig)
	 usleep( 400000 );
   }

   return NULL;
}


/* Probe a port (often 443) to see what protocol is used.
 *
 * NOTE: This function may take up to 22 seconds to complete !
 */
static int UFDBverifyPortHasHTTPS( 		/* TODO: GET RID OF UFDBverifyPortHasHTTPS() and use UFDBsslPeekServer() instead *****************************************/
   const char *   hostname, 
   int            portnumber, 
   int            flags )
{
   /* When a HTTPS port is used, we need to check the following:
    * if SSL+HTTP is spoken
    * if not SSL, is XMPP/Jabber/Google Talk
    * if not SSL, is Skype or an unknown protocol
    * if SSL has a valid certificate
    * if SSL+HTTP, detect known tunnels
    * TO-DO: if SSL+HHTP, detect proxy with content analysis (NEW)
    */
   int                s;
   int                status;
   char *             content;
   struct timeval     tv;
   struct httpsInfo * hinfo;
   int                certErrors;
   char               cn[1024];

   if (UFDBglobalDebug  ||  flags & UFDB_API_VERBOSE_OUTPUT)
      ufdbLogMessage( "UFDBverifyPortHasHTTPS( %s, %d )", hostname, portnumber );

   /* aargghh!! with Squid interception mode, youtube-education and google sites now also use HTTPS with IPv4 */
   if (UFDBglobalHttpsWithHostname  &&  
       !UFDBglobalSquidUsesSSLbump  &&
       !UFDBglobalUnknownProtocolOverHttps  &&  
       !UFDBglobalSkypeOverHttps  &&  !UFDBglobalGtalkOverHttps  &&  !UFDBglobalFBchatOverHttps  &&
       !UFDBglobalYahooMsgOverHttps  &&  !UFDBglobalAimOverHttps  &&
       !UFDBglobalCitrixOnlineOverHttps  &&
       hostnameIsIP(hostname) )
   {
      ufdbLogError( "IP address in URL is not allowed for the HTTPS protocol. IP=%s", hostname );
      status = UFDB_API_ERR_IP_ADDRESS;
      updateHTTPScache( hostname, portnumber, status, 0, &hinfo );
      return status;
   }

   if (UFDBglobalReconfig || UFDBglobalTerminating)
      return UFDB_API_ERR_SOCKET;       /* try again in 5 minutes */

   status = lookupHTTPScache( hostname, portnumber, 1, &hinfo );
   if (status == UFDB_API_ERR_OUTDATED)
   {
      /* this thread is going to do the verification now */
      updateHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 0, &hinfo );
   }
   else
   if (status == UFDB_API_ERR_NULL) 					/* not found in cache, fresh entry */
   {
      /* this thread is going to do the verification now */
      insertHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 1, &hinfo );
   }
   else if (status == UFDB_API_REQ_QUEUED)				/* found in cache queue */
   {
      /* this thread is going to do the verification now */
      updateHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 0, &hinfo );
   }
   else
   {
      if (status != UFDB_API_BEING_VERIFIED)				/* found in cache, probe is concluded */
	 return status;
      
      									/* found in cache, being verified at this moment */
#ifdef UFDB_HTTPS_CACHE_DEBUG
      ufdbLogMessage( "   UFDBverifyPortHasHTTPS %s:%d status is BEING_VERIFIED", hostname, portnumber );
#endif

      /* The status is UFDB_API_BEING_VERIFIED...
       * When the global mode is "aggressive" we use an idle-wait loop waiting
       * for the other thread to finish its job, otherwise the flags (UFDB_API_ALLOW_QUEUING)
       * may indicate if UFDB_API_BEING_VERIFIED is returned or the idle-wait loop is done.
       */
      if (UFDBglobalTunnelCheckMethod != UFDB_API_HTTPS_CHECK_AGGRESSIVE  &&
          (flags & UFDB_API_ALLOW_QUEUING) != 0)
      {
	 if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_CHECK_QUEUE_CHECKS)
	    usleep( 100000 );		/* give 0.1 sec CPU to other threads */
         return UFDB_API_BEING_VERIFIED;
      }

      /* The method is aggressive so this thread is going to wait for another thread to terminate its probe */

      usleep( 35000 + (random() % 19999) );
      s = 0;

      while (status == UFDB_API_BEING_VERIFIED)			
      {
	 if (UFDBglobalReconfig)
	    return UFDB_API_ERR_SOCKET;

	 status = lookupHTTPScache( hostname, portnumber, 0, &hinfo );
	 if (status == UFDB_API_BEING_VERIFIED)
	 {
	    usleep( 80000 + (random() % 27777) );
	    s++;
	    if (s == 10)
	    {
	       if (UFDBglobalDebug > 1  ||  flags & UFDB_API_VERBOSE_OUTPUT)
		  ufdbLogMessage( "   UFDBverifyPortHasHTTPS: waiting for other thread on status for %s:%d", 
				  hostname, portnumber );
	    }
	 }
	 else
	 {
	    if (s >= 10)
	    {
	       if (UFDBglobalDebug > 1  ||  flags & UFDB_API_VERBOSE_OUTPUT)
		  ufdbLogMessage( "   UFDBverifyPortHasHTTPS: finished waiting %d intervals for other thread on %s:%d", 
				  s, hostname, portnumber );
	    }
	    return status;
	 }

	 if (s == 150)		/* Aaaargh. I am tired of waiting! */
	 {
	       if (UFDBglobalDebug > 1  ||  flags & UFDB_API_VERBOSE_OUTPUT)
		  ufdbLogMessage( "   UFDBverifyPortHasHTTPS: waited too long for other thread on status for %s:%d",
				  hostname, portnumber );
	       return UFDB_API_BEING_VERIFIED;
	 }
      }
   }

#ifdef UFDB_HTTPS_CACHE_DEBUG
   if (UFDBglobalDebug  ||  (flags & UFDB_API_VERBOSE_OUTPUT))
      ufdbLogMessage( "   UFDBverifyPortHasHTTPS: %s:%d  opening socket...", hostname, portnumber );
#endif

   /************************** THE ACTUAL HTTPS PORT PROBE STARTS HERE **********************************/

   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      ufdbLogError( "HTTPS protocol verification for %s:%d FAILED: cannot open communication socket",
                     hostname, portnumber );
      updateHTTPScache( hostname, portnumber, UFDB_API_ERR_SOCKET, 0, &hinfo );
      return UFDB_API_ERR_SOCKET;
   }

   if (UFDBglobalReconfig)
   {
      close( s );
      return UFDB_API_ERR_SOCKET;
   }

   if (UFDBglobalDebug  ||  (flags & UFDB_API_VERBOSE_OUTPUT)  ||
       UFDBglobalDebugAim || UFDBglobalDebugGtalk || UFDBglobalDebugSkype || UFDBglobalDebugYahooMsg || 
       UFDBglobalDebugFBchat || UFDBglobalDebugCitrixOnline)
   {
      ufdbLogMessage( "UFDBverifyPortHasHTTPS: socket to %s:%d is opened successfully. fd=%d", hostname, portnumber, s );
   }

   content = NULL;
   status = UFDB_API_OK;

   if (status == UFDB_API_OK)
   {
      /* TODO: check for vtunnel */

      tv.tv_sec = 3;		/* NOTE: ufdbgclient times out in 21 seconds! but we also have Skype,Gtalk,SSH probes */
      tv.tv_usec = 500000;
      (void) setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
      (void) setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

      /* Setup a TLS connection to connect to the HTTPS port, do a "GET /", 
       * and see what the server has to say.  An error is returned if
       * the other end does not speak SSL.
       */
      status = UFDB_API_OK;
      content = httpsGETroot( s, hostname, portnumber, &certErrors, cn, &status );
      if (UFDBglobalDebug  ||  (flags & UFDB_API_VERBOSE_OUTPUT)  || 
          UFDBglobalDebugAim || UFDBglobalDebugGtalk || UFDBglobalDebugSkype || UFDBglobalDebugYahooMsg || 
	  UFDBglobalDebugFBchat || UFDBglobalDebugCitrixOnline)
      {
         ufdbLogMessage( "   httpsGETroot for %s:%d returned status %s, content is %sNULL", 
	 		 hostname, portnumber, ufdbAPIstatusString(status),  content==NULL ? "" : "not " );
      }

      if (content == NULL  ||  *content == '\0')
      {
	 if (status == UFDB_API_OK)
	 {
	    /* We did not read anything from the server...  so we cannot draw a conclusion.
	     * Therefore we return "OK" and hope that the next check gives an answer.
	     */
	    ufdbLogMessage( "HTTPS server %s:%d has an empty or no response.", hostname, portnumber );
	    close( s );
	    if (content != NULL)
	       ufdbFree( content );
	    return UFDB_API_OK;
	 }
      }
      else
      if (status != UFDB_API_OK)
      {
         /* we have already an error condition or ERR_IS_AIM|GTALK|SKYPE|YAHOOMSG|FBCHAT ; do no more checks */
	 ;
      }
      else
      if (strncmp( content, "HTTP/", 5 ) == 0  ||
          strncmp( content, "<?xml", 5 ) == 0)		/* detect known tunnels */
      {
         if (UFDBglobalDebug > 1)
	 {
	    int   i, j;
	    char  debuginfo[2600];
	    /* TO-DO: put this debug code in a separate function */

	    for (j = 0, i = 0;  content[i] != '\0' && i < 2599;  i++)
	    {
	       if (content[i] == '\r'  ||  content[i] == '\n')
		  debuginfo[j++] = '_';
	       else if (isprint(content[i]))
		  debuginfo[j++] = content[i];
	       else
		  debuginfo[j++] = '.';
	    }
	    debuginfo[j] = '\0';
	    ufdbLogMessage( "   HTTPS protocol reply for %s:%d:\n   %s", hostname, portnumber, debuginfo );
	 }

	 /* TODO: if header has PIWIK_SESSID= the category must be "ads" */
#if 0
         if (strstr( content, "X-Kazaa-" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses Kazaa P2P",
	    		  hostname, portnumber );
	    status = UFDB_API_ERR_P2P;
	 }
#endif
	 /* TODO: investigate nomachine.com */
	 if (strstr( content, "logmein.com/" ) != NULL  ||
	     strstr( content, "hamachi.cc/" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses a hamachi/logmein TUNNEL", 
	                  hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( content, "Set-Cookie: SSLX_SSESHID=" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses an SSL-Explorer TUNNEL", 
	                  hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( content, "BarracudaServer.com" ) != NULL   ||
	     strstr( content, "barracudaserver.com" ) != NULL   ||
	     strstr( content, "BarracudaDrive" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses a BARRACUDA proxy TUNNEL",
	     		  hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( content, "  index.vnc -" ) != NULL   ||
	     strstr( content, "  used with Xvnc" ) != NULL   ||
	     strstr( content, "TightVNC Java viewer applet" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses a VNC proxy TUNNEL",
	                  hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( content, "X-Webtunnel-Status" ) != NULL   ||
	     strstr( content, "X-webtunnel-status" ) != NULL  ||
	     strstr( content, "X-webtunnel-version" ) != NULL  ||
	     strstr( content, "X-Webtunnel-Version" ) != NULL)
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses Webtunnel TUNNEL",
	                  hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
      }
      else					/* server does not speak HTTP */
      {
	 int   i,j;
	 char  debuginfo[2400];

	 ufdbLogMessage( "HTTPS protocol on %s:%d encapsulates a non-HTTP protocol %s",
		         hostname, portnumber,
		         UFDBglobalUnknownProtocolOverHttps ? "and an unknown protocol is allowed" : 
		            "and is considered a PROXY TUNNEL because allow-unknown-protocol-over-https is OFF"
		       );
         if (UFDBglobalDebug > 1)
	 {
	    /* TO-DO: put this code in a separate function */
	    for (j = 0, i = 0;  content[i] != '\0' && i < 2399;  i++)
	    {
	       if (content[i] == '\r'  ||  content[i] == '\n')
		  debuginfo[j++] = '_';
	       else if (isprint(content[i]))
		  debuginfo[j++] = content[i];
	       else
		  debuginfo[j++] = '.';
	    }
	    debuginfo[j] = '\0';
	    ufdbLogMessage( "   HTTPS protocol reply for %s:%d:\n   %s", hostname, portnumber, debuginfo );
	 }

	 status = UFDBglobalUnknownProtocolOverHttps ? UFDB_API_ERR_UNKNOWN_PROTOCOL : UFDB_API_ERR_TUNNEL;
      }
   }

   close( s );

   if (UFDBglobalDebug  ||  UFDBglobalLogAllRequests  ||
       UFDBglobalDebugAim || UFDBglobalDebugGtalk || UFDBglobalDebugSkype || UFDBglobalDebugYahooMsg || 
       UFDBglobalDebugFBchat || UFDBglobalDebugCitrixOnline)
   {
      ufdbLogMessage( "HTTPS protocol on %s:%d has been verified and status is %s", 
      		      hostname, portnumber, ufdbAPIstatusString(status) );
   }

   if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_LOG_ONLY)
      status = UFDB_API_OK;

   updateHTTPScache( hostname, portnumber, status, 0, &hinfo );

   ufdbFree( content );
   return status;
}


/*
 * return values:
 *    UFDB_API_ERR_NULL
 *    UFDB_API_ERR_OUTDATED
 *    status found in cache
 */
static int lookupHTTPScache( 
   const char *       hostname, 
   int                portnumber,
   int                keepLockForInsert,
   struct httpsInfo ** hinfo )
{
   struct httpsInfo * info;
   int                status;
   time_t             now;
   char               key[1024+16];

   sprintf( key, "%s:%d", hostname, portnumber );

   info = (struct httpsInfo *) UFDBsearchHashtable( myht, (void *) key, keepLockForInsert );
   if (info == NULL)
   {
#ifdef UFDB_HTTPS_CACHE_DEBUG
      ufdbLogMessage( "   lookupHTTPScache %s%s is NULL", key, keepLockForInsert ? " keeplock" : "" );
#endif
      return UFDB_API_ERR_NULL;
   }

   now = time( NULL );
   status = info->status;
   *hinfo = info;

   if (info->t < UFDBglobalDatabaseLoadTime)
   {
#ifdef UFDB_HTTPS_CACHE_DEBUG
      ufdbLogMessage( "   lookupHTTPScache %s%s is removed from hash table because the configuration changed", 
		      key, keepLockForInsert ? " keeplock" : "" );
#endif
      return UFDB_API_ERR_OUTDATED;
   }
   else if (status == UFDB_API_ERR_SOCKET)
   {
      if (info->t < now - 1800  ||  
          (info->t < now - 600  &&  UFDBhttpsVerificationQueued() < UFDB_NUM_HTTPS_VERIFIERS  &&  UFDBgetTunnelCheckMethod() != UFDB_API_HTTPS_CHECK_AGGRESSIVE))
      {
#ifdef UFDB_HTTPS_CACHE_DEBUG
	 ufdbLogMessage( "   lookupHTTPScache %s%s is removed from hash table because it has aged", 
			 key, keepLockForInsert ? " keeplock" : "" );
#endif
	 return UFDB_API_ERR_OUTDATED;
      }
   }

   info->t = now;

#ifdef UFDB_HTTPS_CACHE_DEBUG
   ufdbLogMessage( "   lookupHTTPScache %s%s is %d", 
                   key, keepLockForInsert ? " keeplock" : "", status );
#endif

   return status;
}


static void updateHTTPScache( 
   const char *        hostname, 
   int                 portnumber, 
   int                 status,
   int                 lockSetBySearch,
   struct httpsInfo ** hinfo  )
{
   time_t              now;
   struct httpsInfo *  linfo;
   char                key[1024+16];

   sprintf( key, "%s:%d", hostname, portnumber );

#ifdef UFDB_HTTPS_CACHE_DEBUG
   ufdbLogMessage( "   updateHTTPScache  %s  %d", key, status );
#endif

   /* The hostname:portnumber is most likely already in the hashlist with status QUEUED.
    * So we only have to update the status...
    * Unless the cache was rebuilt, we need to add the entry to the hash.
    */
   if (lockSetBySearch)
      UFDBunlockHashtable( myht );

   now = time( NULL );
   linfo = (struct httpsInfo *) UFDBsearchHashtable( myht, (void *) key, 1 );
   if (linfo == NULL)
   {
      linfo = (struct httpsInfo *) ufdbMalloc( sizeof(struct httpsInfo) );
      linfo->t = now;
      linfo->status = status;
      linfo->content = NULL;
      linfo->category[0] = '\0';
      linfo->cn[0] = '\0';
      UFDBinsertHashtable( myht, (void *) ufdbStrdup(key), (void *) linfo, 1 );
   }
   else
   {
      /* note that the hashtable is not locked any more but that is allright since linfo will not change */
      linfo->status = status;
      linfo->t = now;
   }
   *hinfo = linfo;
}


/*
 *
 */
static void insertHTTPScache( 
   const char *        hostname, 
   int                 portnumber, 
   int                 status,
   int                 lockSetbySearch,  /* always 1 */
   struct httpsInfo ** hinfo  )
{
   time_t              now;
   struct httpsInfo *  info;
   char                key[1024+16];

   sprintf( key, "%s:%d", hostname, portnumber );

#ifdef UFDB_HTTPS_CACHE_DEBUG
   ufdbLogMessage( "   insertHTTPScache  %s  %d  %s", key, status, lockSetbySearch ? "lockSetBySearch" : "" );
#endif

   now = time( NULL );
   info = (struct httpsInfo *) ufdbMalloc( sizeof(struct httpsInfo) );
   info->t = now;
   info->status = status;
   info->content = NULL;
   info->category[0] = '\0';
   info->cn[0] = '\0';
   *hinfo = info;

   /* before calling UFDBinsertHashtable, check if the cache needs a purge and 
    * take advantage of the hash table already being locked.
     */
   if ((myht->nEntries > 15000  &&  now > lastHTTPScachePurgeTime + 60*60)  ||  (now > lastHTTPScachePurgeTime + 3*60*60))
   {
      int oldnEntries;

      oldnEntries = myht->nEntries;
      if (oldnEntries > 15000)
	 OldestInHTTPScache = now - 2 * 60 * 60;         /* 2 hours */
      else
	 OldestInHTTPScache = now - 3 * 60 * 60;         /* 3 hours */
      UFDBpurgeHashtable( myht, purgeHTTPSinfo );
      lastHTTPScachePurgeTime = now;
      if (UFDBglobalDebug)
	 ufdbLogMessage( "      insertHTTPScache: HTTPS cache %08lx was purged: %d of %d entries were removed",
			 (unsigned long) myht, oldnEntries - myht->nEntries, oldnEntries );
   }

   UFDBinsertHashtable( myht, (void *) ufdbStrdup(key), (void *) info, lockSetbySearch );
}


#if 0
int select_fd( 
   int      fd, 
   double   maxtime, 
   int      wait_for )
{
   int      result;
   fd_set   fdset;
   fd_set * rd;
   fd_set * wr;
   struct timeval tmout;

   FD_ZERO( &fdset );
   FD_SET( fd, &fdset );
   rd = &fdset;
   wr = &fdset;

   tmout.tv_sec = (long) maxtime;
   tmout.tv_usec = 1000000 * (maxtime - (long) maxtime);

   errno = 0;
   do
      result = select( fd+1, rd, wr, NULL, &tmout );
   while (result < 0  &&  errno == EINTR);

   return result;
}
#endif


/* Perform the SSL handshake on file descriptor FD, which is assumed
   to be connected to an SSL server.  The SSL handle provided by
   OpenSSL is registered with the file descriptor FD using
   fd_register_transport, so that subsequent calls to fd_read,
   fd_write, etc., will use the corresponding SSL functions.

   Returns UFDB_API_OK on success  */

int UFDBopenssl_connect( 
   const char * hostname, 
   int          portnumber, 
   int          fd, 
   SSL **       ssl )
{
   int    ret;
   int    saved_errno;
   int    SSL_error;
   long   state;
   char   syserrstring[256];
   char   errstring[256];

   if (UFDBglobalDebug)
      ufdbLogMessage( "UFDBopenssl_connect %s %d %d ...", hostname, portnumber, fd );

   waitForTLSinitilisation();

   ERR_clear_error();
   errno = 0;

   *ssl = SSL_new( ssl_ctx );
   if (*ssl == NULL)
   {
      ufdbLogError( "UFDBopenssl_connect: SSL_new failed fd=%d", fd );
      print_errors();
      return UFDB_API_ERR_FATAL;
   }

#if 0
   (void) SSL_set_options( *ssl, SSL_OP_SINGLE_DH_USE|SSL_OP_CIPHER_SERVER_PREFERENCE|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3 );
#endif

   ret = SSL_set_fd( *ssl, fd );
   if (ret == 0)
   {
      ufdbLogError( "UFDBopenssl_connect: SSL_set_fd failed fd=%d", fd );
      print_errors();
      UFDBopenssl_close( *ssl );
      return UFDB_API_ERR_FATAL;
   }

   SSL_set_connect_state( *ssl );

#if defined(SSL_CTRL_SET_TLSEXT_HOSTNAME)
   /* NOTE: also call SSL_set_tlsext_host_name() if the hostname is an IPv4 or IPv6 address */
   if (!SSL_set_tlsext_host_name( *ssl, hostname ))
   {
      ufdbLogMessage( "UFDBopenssl_connect: SSL_set_tlsext_host_name failed for '%s'", hostname );
   }
#endif

   errno = 0;
   ret = SSL_connect( *ssl );
   saved_errno = errno;
   state = (*ssl)->state;
   if (ret > 0  &&  state == SSL_ST_OK)
   {
      if ((*ssl)->version == SSL2_VERSION  &&  UFDBglobalHttpsNoSSLv2)
      {
	 ufdbLogError( "HTTPS/SSL connection for %s:%d has an insecure SSLv2 protocol. It is blocked.  *****",
		       hostname, portnumber );
	 UFDBopenssl_close( *ssl );
	 return UFDB_API_ERR_INVALID_CERT;
      }
      if ((*ssl)->version == SSL3_VERSION  &&  UFDBglobalHttpsNoSSLv3)
      {
	 ufdbLogError( "HTTPS/SSL connection for %s:%d has an insecure SSLv3 protocol. It is blocked.  *****",
		       hostname, portnumber );
	 UFDBopenssl_close( *ssl );
	 return UFDB_API_ERR_INVALID_CERT;
      }
      return UFDB_API_OK;
   }

   if (saved_errno == 0)
      strcpy( syserrstring, "no error" );
   else
      strerror_r( saved_errno, syserrstring, sizeof(syserrstring) );
   SSL_error = SSL_get_error( *ssl, ret );
   ERR_error_string_n( SSL_error, errstring, sizeof(errstring) );
   ufdbLogError( "UFDBopenssl_connect: SSL_connect to %s:%d failed:\n"
                 "ret=%d  errno=%d(%s)  SSLerr=%d(%s)  state=0x%08lx  %s",
		 hostname, portnumber, ret, saved_errno, syserrstring, SSL_error, sslerr2str(SSL_error), 
                 (long) state, errstring );
   print_errors();
   UFDBopenssl_close( *ssl );

   /* do not call probePort if the server reset the connection */
   if (saved_errno == ECONNRESET)
      return UFDB_API_ERR_FATAL;

   /* SSL_connect failed */
   return UFDB_API_ERR_TLS;
}


/* Return 1 if hostname (case-insensitively) matches common, 0
   otherwise.  The recognized wildcard character is "*", which matches
   any character in common except ".".  

   This is used to match of hosts as indicated in rfc2818: "Names may
   contain the wildcard character * which is considered to match any
   single domain name component or component fragment. E.g., *.a.com
   matches foo.a.com but not bar.foo.a.com. f*.com matches foo.com but
   not bar.com [or foo.bar.com]."

   If the pattern contain no wildcards, matchHostname(a,b) is
   equivalent to !strcasecmp(a,b)
*/

static int matchHostname( 
   const char * common, 
   const char * hostname )
{
   const char * c = common;
   const char * h = hostname;
   char         ch;

#if 0
   if (UFDBglobalDebug)
      ufdbLogMessage( "      matchHostname( %s, %s )", common, hostname );
#endif

   for (; (ch = tolower(*c)) != '\0'; h++)
   {
      if (ch == '*')
      {
	 c++;
	 for (ch = tolower(*c);  ch == '*';  c++, ch = tolower(*c))
	    ;
	 for (; *h != '\0'; h++)
	 {
	    if (tolower(*h) == ch  &&  matchHostname(c,h))	/* TO-DO: make non-recursive */
	       return 1;
	    else if (*h == '.')
	       return 0;
	 }
	 return ch == '\0';
      }
      else
      {
	 if (ch != tolower(*h))
	    return 0;
	 c++;
      }
   }

   return (*h == '\0');
}


#ifdef UFDB_LOAD_CERT_CHAIN

static X509 * convert_bytes_to_cert(
   unsigned char * bytes,
   int             nbytes )
{
   X509 *          cert;

   if (UFDBglobalDebug)
      ufdbLogMessage( "      convert_bytes_to_cert: certificate has %d bytes", nbytes );

#if 0
   BIO *  in;
   if (!(in = BIO_new_mem_buf( bytes, nbytes ))
   {
      ufdbLogError( "cannot convert bytes to certificate. BIO_new_mem_buf failed" );
      return NULL;
   }
#endif

   /* certificate format is FORMAT_ASN1 or FORMAT_PEM or FORMAT_NETSCAPE */
   if (bytes[0] == '-'  &&  bytes[1] == '-')				/* FORMAT_PEM */
   {
      /* x = PEM_read_bio_X509_AUX(cert,NULL, (pem_password_cb *)password_callback, NULL); */
      ufdbLogError( "cannot convert bytes to certificate. PEM_read_bio_X509_AUX not yet implemented" );
   }
   else									/* FORMAT_ASN1 (DER) */
   {
      /* d2i = DER/BER to internal */
      cert = d2i_X509( NULL, (unsigned char **) &bytes, nbytes );
      if (cert == NULL)
      {
	 ufdbLogError( "cannot convert bytes to certificate. d2i_X509 failed" );
	 print_errors();
	 return NULL;
      }
      return cert;
   }

   {
      int i;
      if (nbytes > 100) nbytes = 100;
      for (i = 0; i < nbytes; i++)
	 ufdbLogMessage( "      byte %2d, 0x%02x  %c", i, bytes[i], (bytes[i] & 0x7f)|0x20 );
   }

   ufdbLogError( "cannot convert bytes to certificate" );
   return NULL;
}
#endif


/* see also https://en.wikipedia.org/wiki/Extended_Validation_Certificate */
static const char * knownEVcertIssuerOIDTable[] =
{
	"0.4.0.2042.1.4",				/* ETSI */
	"0.4.0.2042.1.5",				/* ETSI */
	"1.2.40.0.17.1.22",				/* A-Trust */
	"1.2.392.200091.100.721.1",			/* SECOM Trust Systems */
	"1.3.6.1.4.1.311.60.2.1.3",                     /* Symantec */
	"1.3.6.1.4.1.782.1.2.1.8.1",			/* Network Solutions */
	"1.3.6.1.4.1.4146.1.1",				/* GlobalSign */
	"1.3.6.1.4.1.6449.1.2.1.5.1",			/* Comodo */
	"1.3.6.1.4.1.6334.1.100.1",			/* Verizon Business (was: Cybertrust) */
	"1.3.6.1.4.1.8024.0.2.100.1.2",		        /* QuoVadis */
	"1.3.6.1.4.1.13177.10.1.3.10",			/* Firmaprofesional */
	"1.3.6.1.4.1.14370.1.6",			/* GeoTrust */
	"1.3.6.1.4.1.14777.6.1.1",			/* Izenpe */
	"1.3.6.1.4.1.17326.10.14.2.1.2",		/* Camerfirma */
	"1.3.6.1.4.1.17326.10.8.12.1.2",		/* Camerfirma */
	"1.3.6.1.4.1.22234.2.5.2.3.1",			/* Keynectis */
	"1.3.6.1.4.1.23223.2",				/* Startcom Certification Authority */
	"1.3.6.1.4.1.23223.1.1.1",			/* Startcom Certification Authority */
	"1.3.6.1.4.1.34697.2.1",			/* AffirmTrust */
	"1.3.6.1.4.1.34697.2.2",			/* AffirmTrust */
	"1.3.6.1.4.1.34697.2.3",			/* AffirmTrust */
	"1.3.6.1.4.1.34697.2.4",			/* AffirmTrust */
	"1.3.6.1.4.1.36305.2",				/* Wosign */
	"2.16.578.1.26.1.3.3",				/* BuyPass */
	"2.16.756.1.83.21.0",				/* Swisscom */
	"2.16.756.1.89.1.2.1.1",			/* SwissSign */
	"2.16.792.1.2.1.1.5.7.1.9",			/* Kamu Sertifikasyon Merkezi */
	"2.16.792.3.0.4.1.1.4",				/* E-Tugra */
	"2.16.840.1.114412.1.3.0.2",			/* DigiCert */
	"2.16.840.1.114412.2.1",			/* DigiCert */
	"2.16.840.1.114028.10.1.2",			/* Entrust */
	"2.16.840.1.114413.1.7.23.3",			/* Go Daddy */
	"2.16.840.1.114414.1.7.23.3",			/* Starfield Technologies */
	"2.16.840.1.113733.1.7.48.1",			/* Thawte */
	"2.16.840.1.113733.1.7.23.6",			/* VeriSign */
	"2.16.840.1.114171.500.9",			/* Wells Fargo */
	"2.16.840.1.114404.1.1.2.4.1",			/* Trustwave */
	NULL
};


int certificateIsEV(
   X509 *                  cert )
{
   POLICYINFO *            pinfo;
   STACK_OF(POLICYINFO) *  policies;
   char *                  oid;
   int                     idlen;
   int                     i, j;
   
   /* EV certificates are certificates with a well-known OID in the "Certificate Policies" 
    * extension field.
    */
   
   if ((policies = (STACK_OF(POLICYINFO) *) X509_get_ext_d2i( cert, NID_certificate_policies, NULL, NULL)) == NULL)
   {
      return 0;
   }
   
   for (i = 0; i < sk_POLICYINFO_num(policies); i++)
   {
      if ((pinfo = sk_POLICYINFO_value(policies,i)) == NULL)
      {
         continue;
      }
      
      if ((idlen = i2t_ASN1_OBJECT( NULL, 0, pinfo->policyid )) <= 0)
      {
         continue;
      }
      
      oid = (char *) ufdbMalloc( idlen + 1 );
      if (i2t_ASN1_OBJECT( oid, idlen + 1, pinfo->policyid ) != idlen)
      {
         ufdbLogError( "cannot convert certificate OID" );
      }
      
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "X.509 policy extension OID = %s", oid );
         
      for (j = 0; knownEVcertIssuerOIDTable[j] != NULL; j++)
      {
         if( strcmp( oid, knownEVcertIssuerOIDTable[j] ) == 0)
         {
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "Certificate is EV. Issuer is %s", oid );
	    ufdbFree( oid );
	    sk_POLICYINFO_pop_free( policies, POLICYINFO_free );
	    return 1;
         }
      }
      
      ufdbFree( oid );
   }

   sk_POLICYINFO_pop_free( policies, POLICYINFO_free );
   return 0;
}


#ifdef UFDB_LOAD_CERT_CHAIN

static X509 * LoadCertificateByURL(
   char * URL )
{
   int    s;
   int    n;
   int    port;
   char * path;
   char * p;
   X509 * cert;
   char   protocol[16];
   char   domain[1024];
   char   strippedUrl[UFDB_MAX_URL_LENGTH];
   char   request[1024+UFDB_MAX_URL_LENGTH];
   char   certbuffer[1024*16];

   port = 80;
   UFDBstripURL( URL, strippedUrl, domain, protocol, &port );
   if (strcmp(protocol,"http") != 0  &&  port != 80)
   {
      ufdbLogError( "cannot load certificate of %s since it has no HTTP protocol", URL );
      return NULL;
   }

   s = UFDBopenSocket( domain, port );
   if (s < 0)
   {
      ufdbLogError( "cannot load certificate of %s", URL );
      return NULL;
   }

   path = strchr( URL, ':' );		/* strip http:// */
   if (path == NULL)
      path = URL;
   else
      path += 3;
   path = strchr( path, '/' );		/* strip domainname */
   if (path == NULL)
      path = "/";

   /*******************************
   GET / HTTP/1.0
   User-Agent: Mozilla/5.0 (xxx) Gecko/20100722 Firefox/3.6.8
   Host: www.urlfilterdb.com:9443
   Accept: * / *
   Connection: Keep-Alive
   ********************************/

   if (UFDBglobalDebug)
      ufdbLogMessage( "   LoadCertificateByURL %s %s", domain, path );

   /* TO-DO: if there is a proxy, "CONNECT" must be used */
   sprintf( request, "GET %s HTTP/1.1\r\n"
                     "User-Agent: " UFDB_USER_AGENT "\r\n"
		     "Host: %s:%d\r\n"
		     "Accept: */*\r\n"
		     "Connection: Close\r\n"
		     "\r\n",
		     path,
		     domain, port );
   n = strlen( request );
   if (write( s, request, n ) != n)
   {
      ufdbLogError( "cannot retrieve certificate for %s: write failed: %s", URL, strerror(errno) );
      close( s );
      return NULL;
   }

   n = UFDBread( s, certbuffer, sizeof(certbuffer) );
   if (n < 0)
   {
      ufdbLogError( "cannot retrieve certificate for %s: read failed: %s", URL, strerror(errno) );
      close( s );
      return NULL;
   }
   close( s );
   p = strstr( certbuffer, "\r\n\r\n" );
   if (p != NULL)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "      downloaded file from %s has no <CR><LF<CR><LF> separator", URL );
      n -= (p - certbuffer) - 4;
      p += 4;
   }
   else
      p = certbuffer;
   if (UFDBglobalDebug)
      ufdbLogMessage( "      certificate has %d bytes", n );

   /* And now begins the real magic: add the certificate to the certificate list.
    * It is a recursive process since a EV chain usually adds more than one certificate.
    */
   cert = convert_bytes_to_cert( (unsigned char *) p, n );
   if (cert == NULL)
      return cert;

   if (UFDBglobalDebug)
   {
      char issuer[1024];
      char subject[1024];

      issuer[0] = '\0';
      (void) X509_NAME_oneline( X509_get_issuer_name(cert), issuer, 1023 );
      issuer[1023] = '\0';

      subject[0] = '\0';
      (void) X509_NAME_oneline( X509_get_subject_name(cert), subject, 1023 );
      subject[1023] = '\0';

      ufdbLogMessage( "         issuer is %s\nsubject is %s", issuer, subject );
   }
   UFDBloadIntermediateCertificates( cert );

   return cert;
   /* TO-DO: after validation: add URL to a cache to prevent multiple downloads */
}
#endif


#ifdef UFDB_LOAD_CERT_CHAIN

static void CAaddURL( 
   STACK_OF(STRING) ** list, 
   ASN1_IA5STRING * url )
{
   X509 * cert;
   char * tmp;

   if (url->type != V_ASN1_IA5STRING) 
      return;
   if (!url->data || !url->length) 
      return;

#ifdef UFDB_STORE_X509_URIS
   if (!*list) 
      *list = sk_ASN1_STRING_TABLE_new( *list );
   if (!*list) 
      return;

   /* prevent duplicates */
   if (sk_ASN1_STRING_TABLE_find( *list, (char *) url->data ) != -1) 
      return;
   tmp = ufdbStrdup( (char *) url->data );
#else
   tmp = (char *) url->data;
#endif

   if (UFDBglobalDebug)
      ufdbLogMessage( "      CAaddURL %s", tmp );

   if (!ufdbCacertsLoaded)
      return;

#if 1
   return;   /* just for now: do not load the certificate */
#endif

   cert = LoadCertificateByURL( tmp );
   if (cert == NULL)
   {
      ufdbLogMessage( "error parsing certificate from %s", tmp );
   }
   else
   {
      X509_free( cert );
   }

#ifdef UFDB_STORE_X509_URIS
   if (!tmp || !sk_ASN1_STRING_TABLE_push(*list, tmp)) 
   {
      X509_email_free( *list );
      *list = NULL;
   }
#endif
}

#endif


#ifdef UFDB_LOAD_CERT_CHAIN

static int UFDBloadIntermediateCertificates(
   X509 * cert )
{
   AUTHORITY_INFO_ACCESS * aia_data;
   ACCESS_DESCRIPTION *    aia_descr;
   STACK_OF(STRING) *      ocsp_list;
   int                     i;
   int                     is_ev;

   aia_data = X509_get_ext_d2i( cert, NID_info_access, NULL, NULL );

   if (UFDBglobalDebug)
      ufdbLogMessage( "UFDBloadIntermediateCertificates: aia_data %s", (aia_data==NULL?"is NULL":"has data") );

   if (aia_data == NULL)
      return 0;

   is_ev = certificateIsEV( cert );
   ocsp_list = NULL;
   if (UFDBglobalDebug)
      ufdbLogMessage( "   #aia_data = %d", sk_ACCESS_DESCRIPTION_num(aia_data) );
   for (i = 0; i < sk_ACCESS_DESCRIPTION_num(aia_data); i++)
   {
      int nid;
      aia_descr = sk_ACCESS_DESCRIPTION_value( aia_data, i );
      nid = OBJ_obj2nid(aia_descr->method);
#if 0
      if (UFDBglobalDebug && nid != NID_ad_OCSP && nid != NID_ad_ca_issuers)
	 ufdbLogMessage( "   OBJ_obj2nid = %d", nid );
#endif
      /* We do not do OCSP.  This is left for the browser. */
      if (nid == NID_ad_ca_issuers)
      {
         if (aia_descr->location->type == GEN_URI)
	 {
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "   certificate AIA CA-ISSUERS URI:" );
	    CAaddURL( &ocsp_list, aia_descr->location->d.uniformResourceIdentifier );
	    is_ev = 1;
         }
      }
   }

   /* TODO: download and verify intermediate certificates: UFDB_STORE_X509_URIS SSL_CTX_use_certificate() */
#ifdef UFDB_STORE_X509_URIS
   X509_email_free( ocsp_list );
#endif

   return is_ev;
}

#endif


#if UFDB_LOOK_AT_AIA || 1
static int CertificatesHasAIA(
   X509 * cert )
{
   AUTHORITY_INFO_ACCESS * aia_data;
   ACCESS_DESCRIPTION *    aia_descr;
   int                     i;
   int                     has_uri;

   aia_data = (AUTHORITY_INFO_ACCESS *) X509_get_ext_d2i( cert, NID_info_access, NULL, NULL );

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "   CertificatesHasAIA: aia_data %s", (aia_data==NULL?"is NULL":"has data") );

   if (aia_data == NULL)
      return 0;

   has_uri = 0;
   for (i = 0; i < sk_ACCESS_DESCRIPTION_num(aia_data); i++)
   {
      int nid;
      aia_descr = sk_ACCESS_DESCRIPTION_value( aia_data, i );
      nid = OBJ_obj2nid(aia_descr->method);
      if (nid == NID_ad_ca_issuers)
      {
         if (aia_descr->location->type == GEN_URI)
         {
	    if (UFDBglobalPeek || UFDBglobalDebug > 1)
	       ufdbLogMessage( "   AIA CA Issuers URI: %s", aia_descr->location->d.uniformResourceIdentifier->data );
	    if (strncasecmp( "http", (char*) aia_descr->location->d.uniformResourceIdentifier->data, 4 ) == 0)
	       has_uri = 1;
	 }
      }
   }

   sk_ACCESS_DESCRIPTION_pop_free( aia_data, ACCESS_DESCRIPTION_free );

   return has_uri;
}
#endif


#if 0
static int my_verify_cert( 
   X509_STORE *     trustedCertStore, 
   X509 *           cert, 
   STACK_OF(X509) * certStack )
{
   X509_STORE_CTX * csc;
   int              retval;

   if (UFDBglobalDebug)
      ufdbLogMessage( "my_verify_cert" );

   csc = X509_STORE_CTX_new();
   if (csc == NULL)
   {
      ufdbLogError( "cannot verify certificate.  X509_STORE_CTX_new failed" );
      /* we simply don't know if the certificate is OK or not... OK */
      return X509_V_OK;
   }
   if (!X509_STORE_CTX_init( csc, trustedCertStore, cert, certStack ))
   {
      ufdbLogError( "cannot verify certificate.  X509_STORE_CTX_init failed" );
      /* we simply don't know if the certificate is OK or not... OK */
      return X509_V_OK;
   }
   /* X509_VERIFY_PARAM_set_flags( X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL ); */
   X509_STORE_CTX_set_purpose( csc,  X509_PURPOSE_SSL_SERVER );		/* or ??? X509_PURPOSE_SSL_SERVER */

#if 0
   X509_STORE_CTX_set_verify_cb( csc, certVerifycallback );
#endif

   retval = X509_verify_cert( csc );
   if (retval > 0)
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "my_verify_cert: certificate is OK" );
      retval = X509_V_OK;
   }
   else
   {
      retval = X509_STORE_CTX_get_error( csc );
      if (UFDBglobalDebug)
      {
	 ufdbLogError( "my_verify_cert: certificate verification failed\n"
	               "certificate error code is %d", 
		       retval );
	 print_errors();
      }
   }
   X509_STORE_CTX_free( csc );

   return retval;
}
#endif


/* ssl_dns_name - Extract valid DNS name from subjectAltName value */
static const char * ssl_dns_name( 
   const GENERAL_NAME * gn )
{
   const char * dnsname;
   int          len;

   /*
    * We expect the OpenSSL library to construct GEN_DNS extension objects as
    * ASN1_IA5STRING values. Check we got the right union member.
    */
   if (ASN1_STRING_type(gn->d.ia5) != V_ASN1_IA5STRING) 
   {
      ufdbLogError( "ssl_dns_name: invalid ASN1 value type in subjectAltName of TLS/SSL certificate" );
      return NULL;
   }

#define TRIM0(s, l) do { while ((l) > 0 && (s)[(l)-1] == 0) --(l); } while (0)
   /*
    * Safe to treat as an ASCII string possibly holding a DNS name
    */
   dnsname = (char *) ASN1_STRING_data( gn->d.ia5 );
   len = ASN1_STRING_length( gn->d.ia5 );
   TRIM0( dnsname, len );

   /*
    * Per Dr. Steven Henson of the OpenSSL development team, ASN1_IA5STRING
    * values can have internal ASCII NUL values in this context because
    * their length is taken from the decoded ASN1 buffer, a trailing NUL is
    * always appended to make sure that the string is terminated, but the
    * ASN.1 length may differ from strlen().
    */
   if (len != (int) strlen(dnsname)) 
   {
      ufdbLogError( "ssl_dns_name: internal NUL in subjectAltName of TLS/SSL certificate" );
      return "error-converting-name";
   }

   return dnsname;
}


/* return values:    UFDB_API_OK
 *                   UFDB_API_ERR_INVALID_CERT
 *		     UFDB_API_ERR_IS_YAHOOMSG
 *		     UFDB_API_ERR_IS_AIM
 *                   UFDB_API_ERR_IS_TUNNEL	(ssh, tor, logme.in)
 */
static int ssl_check_certificate( 
   SSL *         ssl, 
   const char *  hostname,
   int           portnumber,
   char *        cn )
{
   const char *  altPtr;
   char *        matchedAltPtr;
   X509 *        cert;
   long          vresult;
   int           success;
   int           cert_stack_size;
   int		 is_chained;
   int           is_ev;
   int           is_ip;
   int           match;
   int           altNameSeen;
   char * 	 _cneq;
   const char *  protocol;
   char          issuer[1024];
   char 	 subject[1024];
   STACK_OF(X509) * certStack;
   STACK_OF(GENERAL_NAME) * altNames;

   success = 2;
   issuer[0] = '\0';
   subject[0] = '\0';

   protocol = sslver2string( ssl->version );
   cert = SSL_get_peer_certificate( ssl );
   certStack = SSL_get_peer_cert_chain( ssl );

   if (hostname == NULL)
   {
      success = 0;
      ufdbLogError( "ssl_check_certificate: hostname=NULL" );
      goto no_cert;		/* must bail out since hostname is NULL */
   }

   if (cert == NULL)
   {
      success = 0;
      ufdbLogError( "site %s:%d has NO TLS/SSL certificate", hostname, portnumber );
      goto no_cert;		/* must bail out since CERT is NULL */
   }

   (void) X509_NAME_oneline( X509_get_issuer_name(cert), issuer, 1023 );
   issuer[1023] = '\0';
   if (issuer[0] == '\0')
   {
      
      strcpy( issuer, "--no-issuer-found-in-certificate--" );
      success = 0;
      ufdbLogError( "site %s:%d certificate has no issuer", hostname, portnumber );
   }
   (void) X509_NAME_oneline( X509_get_subject_name(cert), subject, 1023 );
   subject[1023] = '\0';
   if (subject[0] == '\0')
   {
      strcpy( subject, "--no-subject-found-in-certificate--" );
      success = 0;
      ufdbLogError( "site %s:%d certificate has no subject", hostname, portnumber );
   }

#if 0
   UFDBloadIntermediateCertificates( cert );
#endif

   if (UFDBglobalDebug > 1 || UFDBglobalPeek)
      CertificatesHasAIA( cert );

   is_ev = certificateIsEV( cert );
   cert_stack_size = sk_X509_num( certStack );
   is_chained = (cert_stack_size > 1);
   is_ip = hostnameIsIP( hostname );

#if 0
   if (UFDBglobalDebug || UFDBglobalDebugAim)
   {
      ufdbLogMessage( "   issuer: %s\nhostnameIsIP(%s): %d", issuer, hostname, is_ip );
   }
#endif

   /* Yuck! AIM connects with a (chained) certificate but does not allow HTTP/1.1 */
   /* TODO: investigate client.web.aol.com */
   if (strstr( issuer, "/O=America Online Inc./CN=AOL Member CA" ) != NULL)
   {
      ufdbLogMessage( "%s certificate on %s:%d is used by AOL or AIM since issuer is %s", protocol, hostname, portnumber, issuer );
      X509_free( cert );
      return UFDB_API_ERR_IS_AIM;
   }

#if 0
   if (certStack != NULL)
   {
      AUTHORITY_INFO_ACCESS * aia_data;

      /* the server uses a chain of certificates ? */
      if (UFDBglobalDebug)
      {
	 int i;
         ufdbLogMessage( "size of stack of certificates is %d", sk_X509_num(certStack) );
	 for (i = 0; i < sk_X509_num(certStack); i++)
	 {
	    X509 * x;
	    char   buffer[512];

	    x = sk_X509_value( certStack, i );
	    (void) X509_NAME_oneline( X509_get_subject_name(x), buffer, sizeof(buffer) );
	    ufdbLogMessage( "   %s", buffer );
	 }
      }
      aia_data = X509_get_ext_d2i( cert, NID_info_access, NULL, NULL );
      is_ev = (aia_data != NULL);
   }
#endif

   _cneq = strstr( subject, "/CN=" );
   if (_cneq != NULL)
   {
      ufdbStrncpy( cn, _cneq+4, 1023 );
   }
   else
   {
      if (subject[0] == 'C'  &&  subject[1] == 'N'  &&  subject[2] == '=')
      {
         _cneq = subject;
         ufdbStrncpy( cn, _cneq+3, 1023 );
      }
   }
   if (_cneq != NULL)
   {
      char * sep;
      sep = strchr( cn, '/' );
      if (sep != NULL)
	 *sep = '\0';
   }
   else
      strcpy( cn, "certificate subject has no CommonName (/CN=)" );

   if (UFDBglobalDebug)
   {
      if (cert_stack_size > 1)
         ufdbLogMessage( "   received %d certificates from %s", cert_stack_size, hostname );
      ufdbLogMessage( "   %s%s%s certificate for %s has issuer '%s'", 
		      is_chained ? "chained " : "",
                      is_ev ? "EV " : "",
		      protocol,
		      hostname, issuer );
      if (UFDBglobalDebug > 1)
      {
	 ufdbLogMessage( "   %s%s%s certificate for %s has subject '%s' (category=%s)",
			 is_chained ? "chained " : "",
			 is_ev ? "EV " : "",
			 protocol,
			 hostname, subject,
			 _cneq != NULL && *cn != '\0' ? ufdbCategoryName(cn) : "unknown" );
      }
      else
	 ufdbLogMessage( "   %s%s%s certificate for %s has subject '%s'", 
			 is_chained ? "chained " : "",
			 is_ev ? "EV " : "",
			 protocol,
			 hostname, subject );
   }

   if (!ufdbCacertsLoaded)
   {
      ufdbLogMessage( "No CA certificates are loaded.  Cannot verify signature of "
                      "%s%scertificate for %s:%d.  Marking it as invalid.", 
		      is_chained ? "chained " : "",
		      is_ev ? "EV " : "",
		      hostname, portnumber );
      /* TODO: investigate free cert chain */
      X509_free( cert );
      return UFDB_API_ERR_INVALID_CERT;
   }

   if (strcmp( cn, "*.uc.cn" ) == 0  ||  strcmp( cn, "*.ucweb.com" ) == 0)
   {
      ufdbLogMessage( "%s certificate for %s:%d: has a certificate with \"ucweb.com\" or \"uc.cn\" and "
                      "is considered a proxy  *****\n"
                      "issuer: %s\n"
                      "subject: %s",
                      protocol,
                      hostname, portnumber, issuer, subject );
      X509_free( cert );
      return UFDB_API_ERR_TUNNEL;
   }

   if (_cneq != NULL)
   {
      if (strstr( _cneq, ".logme.in" ) != NULL  ||  strstr( _cneq, ".logmein.com" ) != NULL)
      {
	 ufdbLogMessage( "%s certificate for %s:%d: has a certificate with \"logme.in\" and "
			 "is considered a proxy  *****\n"
			 "issuer: %s\n"
			 "subject: %s",
			 protocol,
			 hostname, portnumber, issuer, subject );
	 X509_free( cert );
	 return UFDB_API_ERR_TUNNEL;
      }
   }

   /* TODO: check certificate for p2p sites and return new status code UFDB_API_ERR_P2P */
   /* TODO: dediseedbox.com ... */

   matchedAltPtr = NULL;
   vresult = SSL_get_verify_result( ssl );

   if (vresult == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
   {
      if (ufdbCacertsLoaded)
      {
	 if (is_ev || is_chained)
	 {
	    success = 1;
	    if (UFDBglobalDebug || UFDBglobalHttpsOfficialCertificate)
	       ufdbLogMessage( "%s%s%s certificate signature of %s:%d cannot be verified, assuming it is OK", 
			       is_chained ? "chained " : "",
			       is_ev ? "EV " : "",
			       protocol,
			       hostname, portnumber );
	 }
	 else
	 {
	    /* Tor uses certificates where the issuer only has /CN=www.BOGUS.net and the subject also only has /CN=www.BOGUS.net
	     * So when the Country and Organization are missing from the issuer and subject, it is marked as a Tor Tunnel.
	     */
	    if (strstr( issuer, "/C=" ) == NULL  &&
	        strstr( issuer, "/O=" ) == NULL  &&
	        strstr( issuer, "/DC=" ) == NULL  &&
		strstr( subject, "/C=" ) == NULL  &&
		strstr( subject, "/OU=" ) == NULL  &&
		strstr( subject, "/O=" ) == NULL  &&
		strstr( issuer, "/CN=" ) != NULL  &&
		strstr( subject, "/CN=" ) != NULL  &&
		issuer[0] != '-'  &&
                strchr( subject, '.' ) != NULL  &&
                strchr( issuer, '.' ) != NULL)
	    {
	       ufdbLogMessage( "%s certificate for %s:%d: has an UNRECOGNISED ISSUER and "
	                       "has the characteristics of a Tor proxy  *****\n"
			       "issuer: %s\n"
			       "subject: %s",
			       protocol, hostname, portnumber, issuer, subject );
	       X509_free( cert );
	       return UFDB_API_ERR_TUNNEL;
	    }

	    if (UFDBglobalDebug || UFDBglobalHttpsOfficialCertificate)
	       ufdbLogMessage( "%s%s%s certificate for %s:%d: UNRECOGNISED ISSUER  (maybe a certificate chain issue)  *****\nissuer: %s\nsubject: %s",
			       is_chained ? "chained " : "",
			       is_ev ? "EV " : "",
			       protocol, hostname, portnumber,
			       issuer, subject );
	    success = 0;
	 }
      }
      else
      {
	 ufdbLogError( "SSL_get_verify_result() for %s:%d is X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY (ignored)\n"
		       "Check the existence and file permissions of %s", 
		       hostname, portnumber,
		       UFDBglobalCAcertsFile );
      }
   }
   else if (vresult == X509_V_OK)
   {
      if (strstr( issuer, "/DC=microsoft/" ) != NULL  &&
          strstr( subject, "skype.com" ) != NULL)
      {
	 ufdbLogMessage( "%s certificate for %s:%d: has a certificate for Skype  *****\n"
	 	         "issuer: %s\n"
		         "subject: %s",
		         protocol, hostname, portnumber, issuer, subject );
	 X509_free( cert );
	 return UFDB_API_ERR_IS_SKYPE;
      }

      if (is_chained &&
	  strstr( subject, "citrixonline.com" ) != NULL)
      {
	 ufdbLogMessage( "%s certificate for %s:%d: has a certificate for CitrixOnline  *****\n"
	 	         "issuer: %s\n"
		         "subject: %s",
		         protocol, hostname, portnumber, issuer, subject );
	 X509_free( cert );
	 return UFDB_API_ERR_IS_CITRIXONLINE;
      }
   }
   else
   {
      success = 0;
      switch (vresult)
      {
      case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
	 if (strstr( subject, "/CN=AnyNet Relay" ) != NULL)
	 {
	    ufdbLogMessage( "%s certificate for %s:%d: has a certificate for AnyDesk  *****\n"
	                    "issuer: %s\nsubject: %s",
			    protocol, hostname, portnumber, issuer, subject );
	    X509_free( cert );
	    return UFDB_API_ERR_IS_ANYDESK;
	 }
	 else 
	 if (is_chained &&
	     strstr( subject, "citrixonline.com" ) != NULL)
	 {
	    ufdbLogMessage( "%s certificate for %s:%d: has a certificate for CitrixOnline  *****\n"
	                    "issuer: %s\nsubject: %s",
			    protocol, hostname, portnumber, issuer, subject );
	    X509_free( cert );
	    return UFDB_API_ERR_IS_CITRIXONLINE;
	 }
	 ufdbLogError( "%s certificate for %s:%d has a SELF-SIGNED certificate in chain  *****\nissuer: %s",
		       protocol, hostname, portnumber, issuer );
	 break;
      case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
	 /* Yuck! Yahoo IM connect to addresses like <IP>:443 with a self-signed certificate. */
	 if (strstr( issuer, "/O=Yahoo/OU=Messenger/CN=undermine.corp/" ) != NULL)
	 {
	    ufdbLogMessage( "%s certificate on %s:%d is used for Yahoo IM since issuer is %s", protocol, hostname, portnumber, issuer );
	    X509_free( cert );
	    return UFDB_API_ERR_IS_YAHOOMSG;
	 }
	 else
	 {
	    ufdbLogError( "%s certificate for %s:%d has a DEPTH-ZERO SELF-SIGNED certificate  *****\nissuer: %s",
			  protocol, hostname, portnumber, issuer );
	 }
	 break;
      case X509_V_ERR_CERT_NOT_YET_VALID:
	 ufdbLogError( "%s certificate for %s:%d has a NOT YET VALID DATE  *****", protocol, hostname, portnumber );
	 break;
      case X509_V_ERR_CERT_HAS_EXPIRED:
	 ufdbLogError( "%s certificate for %s:%d has EXPIRED  *****", protocol, hostname, portnumber );
	 break;
      case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
	 ufdbLogError( "%s certificate for %s:%d has an UNRECOGNISED ISSUER ?!?  *****\nissuer: %s", 
		       protocol, hostname, portnumber, issuer );
	 break;
      case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
         ufdbLogMessage( "%s%s%s certificate for %s:%d cannot be verified.", 
			 is_chained ? "chained " : "",
			 is_ev ? "EV " : "",
	                 protocol, hostname, portnumber );
	 break;
      case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
         ufdbLogMessage( "%s%s%s certificate for %s:%d CANNOT DECRYPT SIGNATURE.", 
			 is_chained ? "chained " : "",
			 is_ev ? "EV " : "",
	                 protocol, hostname, portnumber );
	 break;
      case X509_V_ERR_CERT_SIGNATURE_FAILURE:
	 if (is_chained || is_ev)
	 {
	    success = 1;
	    ufdbLogMessage( "%s%s%s certificate for %s:%d has an unresolvable certificate signature failure.\n"
	    		    "assuming that the signature is OK.", 
			    is_chained ? "chained " : "",
			    is_ev ? "EV " : "",
			    protocol, hostname, portnumber );
	 }
	 else
	    ufdbLogMessage( "%s%s%s certificate for %s:%d CERTIFICATE SIGNATURE FAILURE.", 
			    is_chained ? "chained " : "",
			    is_ev ? "EV " : "",
			    protocol, hostname, portnumber );
	 break;
      default:
	 ufdbLogError( "%s certificate VERIFICATION ERROR for %s:%d %ld %s  *****",
		       protocol, hostname, portnumber, vresult, X509_verify_cert_error_string(vresult) );
      }
      /* Fall through */
   }

   if (success>1 && UFDBglobalDebug)
      ufdbLogMessage( "   %s%s%s certificate for %s:%d is signed by a CA (OK)", 
		      is_chained ? "chained " : "",
		      is_ev ? "EV " : "",
                      protocol, hostname, portnumber );

   /* 
    * Check that hostname matches the common name in the certificate.
    * rfc2818:
    * "If a subjectAltName extension of type dNSName is present, 
    * that MUST be used as the identity."
    * If not, we will use the commonName as the identity.
    */
   match = 0;
   
   altNameSeen = 0;

   altNames = (STACK_OF(GENERAL_NAME) *) X509_get_ext_d2i( cert, NID_subject_alt_name, NULL, NULL );
   if (altNames)
   {
      const GENERAL_NAME * check;
      int numAltNames;
      int i;

      altPtr = "-";
      numAltNames = sk_GENERAL_NAME_num( altNames );
      for (i = 0; i < numAltNames; i++)
      {
	 check = sk_GENERAL_NAME_value( altNames, i );
	 if (check->type == GEN_DNS)
	 {
	    const char * d;

	    altPtr = ssl_dns_name( check );
	    altNameSeen = 1;

	    /* google uses a very large array of altNames for google and youtube */

	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "      %s certificate for %s:%d has subjectAltName %s (category=%s)", 
	                       protocol, hostname, portnumber, altPtr, ufdbCategoryName(altPtr) );
	    else if (UFDBglobalDebug)
	       ufdbLogMessage( "      %s certificate for %s:%d has subjectAltName %s", protocol, hostname, portnumber, altPtr );

	    if (is_ip  &&  
	        (UFDBglobalYoutubeEdufilter || UFDBglobalAllowGoogleHTTPSusingIP)  &&  
	        strstr( issuer, "/O=Google Inc/" ) != NULL  &&  
		((d = strstr( altPtr, "google.com" )) != NULL  &&  strcmp( d, "google.com" ) == 0))
	    {
	       if (UFDBglobalDebug)
	          ufdbLogMessage( "%s certificate for %s:%d is used by %s (allowed)", protocol, hostname, portnumber, altPtr );
	       sk_GENERAL_NAME_pop_free( altNames, GENERAL_NAME_free );
	       X509_free( cert );
	       return UFDB_API_OK;
	    }
		
	    if (matchHostname( altPtr, hostname ))
	    {
	       match = 1;
	       if (!UFDBglobalDebug)
		  break;
	       matchedAltPtr = ufdbStrdup( altPtr );
	    }
	 }
      }

      if (altNameSeen)
      {
	 if (!match)
	 {
	    if (UFDBglobalDebug || UFDBglobalHttpsOfficialCertificate)
	       ufdbLogMessage( "%s certificate has subjectAltName \"%s\" which does NOT MATCH hostname \"%s\".  Certificate is invalid.",
			       protocol, altPtr, hostname );
	    success = 0;
	 }
	 else
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "%s certificate with subjectAltName \"%s\" matches hostname \"%s\"",
	                       protocol, matchedAltPtr, hostname );
      }
      sk_GENERAL_NAME_pop_free( altNames, GENERAL_NAME_free );
   }
   
   if (matchedAltPtr != NULL)
      ufdbFree( matchedAltPtr );

   if (!altNameSeen)
   {
      int             i, j;
      X509_NAME *     name;
      unsigned char * nulstr = (unsigned char *) "";
      unsigned char * commonName = nulstr;

      if (UFDBglobalDebug)
	 ufdbLogMessage( "%s certificate for %s:%d has no subjectAltName", protocol, hostname, portnumber );

      i = -1;
      name = X509_get_subject_name( cert );
      if (name != NULL)
         while ((j=X509_NAME_get_index_by_NID(name,NID_commonName,i)) >= 0)
	    i = j;

      /* now we have the name entry and convert it to a string */
      if (i >= 0)
      {
         ASN1_STRING * tmp;

	 tmp = X509_NAME_ENTRY_get_data( X509_NAME_get_entry(name,i) );

	 /* In OpenSSL 0.9.7d and earlier ASN1_STRING_to_UTF8 fails if string is already UTF8 :-) */
#if defined(OPENSSL_VERSION_NUMBER)
#if OPENSSL_VERSION_NUMBER <= 0x0090704fL
	 if (UFDBglobalDebug)
	    ufdbLogMessage( "   OpenSSL library version is %08X", OPENSSL_VERSION_NUMBER );
	 if (tmp != NULL  &&  ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING)
	 {
	    j = ASN1_STRING_length( tmp );
	    if (j >= 0)
	    {
	       commonName = ufdbMalloc( j+1 );
	       memcpy( commonName, ASN1_STRING_data(tmp), j );
	       commonName[j] = '\0';
	    }
	 }
	 else
	 {
	    j = ASN1_STRING_to_UTF8( &commonName, tmp );
	 }
#else
         j = ASN1_STRING_to_UTF8( &commonName, tmp );
#endif
#else
         j = ASN1_STRING_to_UTF8( &commonName, tmp );
#endif
      }

      if (commonName == nulstr)
	 commonName = NULL;
      if (commonName == NULL)
      {
	 ufdbLogError( "%s certificate for %s:%d has no CommonName (CN).  Certificate is invalid.", protocol, hostname, portnumber );
	 success = 0;
      }
      else
      {
	 if ( !matchHostname( (char *) commonName, hostname ))
	 {
	    ufdbLogError( "%s certificate CommonName '%s' does NOT MATCH hostname `%s'.  Certificate is invalid.",
			  protocol, commonName, hostname );
	    success = 0;
	 }
         OPENSSL_free( commonName );
      }
   }

   print_errors();

   if (success && UFDBglobalDebug)
      ufdbLogMessage( "%s certificate matches hostname %s", protocol, hostname );

   /* TODO: investigate free cert chain */
   X509_free( cert );

no_cert:
   return success ? UFDB_API_OK : UFDB_API_ERR_INVALID_CERT;
}


#if 0
int UFDBfindContentCategory( char * content, char * category )
{
   /* content scanning is something for icapd ... */
   return 0;
}
#endif


/*
 *  Probe a port (often 443) to see what protocol is used.
 *
 *  NOTE: This function may take up to 15 seconds to complete !
 *
 *  return values:
 *	function:    UFDB_API_OK / UFDB_API_ERR_SOCKET / UFDB_API_BEING_VERIFIED / status(UFDB_API_ERR_TUNNEL etc)
 *      cn:          CN of the TLS/SSL certificate
 *	certErrors:  UFDB_API_OK / UFDB_API_ERR_INVALID_CERT
 *      content:     caller must free() it.
 *	
 */
int UFDBsslPeekServer(
   const char *  hostname,
   int           portnumber,
   char *        cn,
   int *         certErrors,
   char **       content,
   int           worker   )
{
   struct httpsInfo * info;
   int                s;
   int                status;
   int                n;
   struct timeval     tv;

   *cn = '\0';
   *certErrors = UFDB_API_OK;
   *content = NULL;
   s = -9999;

   if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_CHECK_OFF)
      return UFDB_API_OK;

   if (UFDBglobalReconfig || UFDBglobalTerminating)
      return UFDB_API_OK;	
      /* note that after a database reload this result is removed from the cache */

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBsslPeekServer  %s:%d  start", worker, hostname, portnumber );

   /* When a HTTPS port is used, we need to check the following:
    * if SSL+HTTP is spoken
    * if not SSL, is XMPP/Jabber/Google Talk
    * if not SSL, is Skype or an unknown protocol
    * if SSL has a valid certificate
    *    detect categories based on CommonName and/or patterns in the certificate
    * if SSL+HTTP, detect known tunnels based on content
    */

   status = lookupHTTPScache( hostname, portnumber, 1, &info );

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBsslPeekServer  %s:%d  cached status %s", 
                      worker, hostname, portnumber, ufdbAPIstatusString(status) );

   if (status == UFDB_API_ERR_OUTDATED)
   {
      /* this thread is going to do the verification now */
      updateHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 0, &info );
      info->category[0] = '\0';
      info->cn[0] = '\0';
      ufdbFree( info->content );
      info->content = NULL;
   }
   else
   if (status == UFDB_API_ERR_NULL) 					/* not found in cache, fresh entry */
   {
      /* this thread is going to do the verification now */
      insertHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 1, &info );
   }
   else if (status == UFDB_API_REQ_QUEUED)				/* found in cache queue */
   {
      /* this thread is going to do the verification now */
      updateHTTPScache( hostname, portnumber, UFDB_API_BEING_VERIFIED, 0, &info );
   }
   else
   {
      if (status != UFDB_API_BEING_VERIFIED)				/* found in cache, probe is concluded */
	 return status;
      
      									/* found in cache, being verified at this moment */
#ifdef UFDB_HTTPS_CACHE_DEBUG
      ufdbLogMessage( "W%03d: UFDBsslPeekServer %s:%d status is BEING_VERIFIED", worker, hostname, portnumber );
#endif

      /* UFDBsslPeekServer cannot return UFDB_API_BEING_VERIFIED, so wait for the other thread to terminate its probe
       * and retrieve the probe results from the cache.
       */

      usleep( 35000 + (random() % 19999) );
      s = 0;

      while (status == UFDB_API_BEING_VERIFIED)			
      {
	 status = lookupHTTPScache( hostname, portnumber, 0, &info );
	 if (status == UFDB_API_BEING_VERIFIED)
	 {
	    usleep( 80000 + (random() % 27777) );
	    s++;
	    if (s == 10)
	    {
	       if (UFDBglobalDebug > 1)
		  ufdbLogMessage( "W%03d: UFDBsslPeekServer: waiting for other thread on status for %s:%d", 
				  worker, hostname, portnumber );
	    }
	 }
	 else
	 {
	    if (s >= 10)
	    {
	       if (UFDBglobalDebug > 1)
		  ufdbLogMessage( "W%03d: UFDBsslPeekServer: finished waiting %d intervals for other thread on %s:%d  status %d",
				  worker, s, hostname, portnumber, status );
	    }
	    return status;
	 }

	 if (UFDBglobalReconfig  ||  UFDBglobalTerminating)	/* this thread must release the readlock asap */
	 {
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBsslPeekServer: stopped waiting for other thread peeking %s:%d since reconfig=true",
			       worker, hostname, portnumber );
	    return UFDB_API_BEING_VERIFIED;
	 }

	 if (s == 200)				/* 200 * 0.08+ sec = 16+ sec.   Aaaargh. I am tired of waiting! */
	 {
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "W%03d: UFDBsslPeekServer: waited too long for other thread on status for %s:%d",
			       worker, hostname, portnumber );
	    return UFDB_API_BEING_VERIFIED;
	 }
      }
   }

   /************************** THE ACTUAL HTTPS PORT PROBE STARTS HERE **********************************/

#ifdef UFDB_HTTPS_CACHE_DEBUG
   if (UFDBglobalDebug  ||  (flags & UFDB_API_VERBOSE_OUTPUT))
      ufdbLogMessage( "W%03d: UFDBsslPeekServer: %s:%d  opening socket...", worker, hostname, portnumber );
#endif

   /* TO-DO: UFDBopenSocket may take a long time doing hostname lookups and trying to open a socket on various addresses */
   s = UFDBopenSocket( hostname, portnumber );
   if (s < 0)
   {
      ufdbLogMessage( "W%03d: HTTPS protocol verification for %s:%d FAILED for peek-server: "
                      "cannot open communication socket",
                      worker, hostname, portnumber );
      updateHTTPScache( hostname, portnumber, UFDB_API_ERR_SOCKET, 0, &info );
      return UFDB_API_ERR_SOCKET;
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBsslPeekServer: socket to %s is opened successfully. fd=%d", worker, hostname, s );

   if (UFDBglobalReconfig)		/* this thread must release the readlock asap */
   {
      if (UFDBglobalDebug)
	 ufdbLogMessage( "W%03d: UFDBsslPeekServer: interrupted peeking %s:%d since reconfig=true",
			 worker, hostname, portnumber );
      close( s );
      updateHTTPScache( hostname, portnumber, UFDB_API_ERR_OUTDATED, 0, &info );
      return UFDB_API_BEING_VERIFIED;
   }

   *content = NULL;
   status = UFDB_API_OK;

   if (status == UFDB_API_OK)
   {
      /* TODO: check for vtunnel */

      tv.tv_sec = 3;		/* NOTE: ufdbgclient times out in 21 seconds! but we also have Skype,Gtalk,SSH probes */
      tv.tv_usec = 500000;
      n = setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
#if 0
      if (n < 0)
         ufdbLogError( "UFDBsslPeekServer: cannot set socket timeout to %ld seconds: %s", 
	               (long) tv.tv_sec, strerror(errno) );
#else
      if (n < 0)
         ;
#endif
      (void) setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

      /* Setup a TLS connection to connect to the HTTPS port, do a "GET /", 
       * and see what the server has to say.  An error is returned if
       * the other end does not speak SSL nor speaks one of the protocols that we are able to detect.
       */

      status = UFDB_API_OK;
      *content = httpsGETroot( s, hostname, portnumber, certErrors, cn, &status );
      if (UFDBglobalDebug  ||
          UFDBglobalDebugAim || UFDBglobalDebugGtalk || UFDBglobalDebugSkype || UFDBglobalDebugYahooMsg || 
	  UFDBglobalDebugFBchat || UFDBglobalDebugCitrixOnline)
      {
         ufdbLogMessage( "W%03d: UFDBsslPeekServer: httpsGETroot for %s:%d returned status %s, content is %sNULL, CN is \"%s\"",
	 		 worker, hostname, portnumber, ufdbAPIstatusString(status),  *content==NULL ? "" : "not ", cn );
      }

      if (*content == NULL  ||  (*content)[0] == '\0')
      {
	 if (status == UFDB_API_OK)
	 {
	    /* We did not read anything from the server...  so we cannot draw a conclusion.
	     * Therefore we return "OK" and hope that the next check gives an answer.
	     */
	    ufdbLogMessage( "W%03d: HTTPS server %s:%d did not send any content.", 
                            worker, hostname, portnumber );
	    close( s );
	    if (*content != NULL)
	       ufdbFree( *content );
	    *content = NULL;
	    return UFDB_API_OK;
	 }

	 if (status == UFDB_API_ERR_NULL)
            /* there was some kind of error. Use UFDB_API_ERR_SOCKET instead of UFDB_API_ERR_NULL */
            status = UFDB_API_ERR_SOCKET;
      }
      else
      if (status != UFDB_API_OK)
      {
         /* we have already an error condition or ERR_IS_AIM|GTALK|SKYPE|YAHOOMSG|FBCHAT ; do no more checks */
	 if (status == UFDB_API_ERR_NULL)
            status = UFDB_API_ERR_SOCKET;
      }
      else
      if (strncmp( *content, "HTTP/", 5 ) == 0  ||
          strncmp( *content, "<?xml", 5 ) == 0)		/* detect known tunnels */
      {
         if (UFDBglobalDebug > 1)
	 {
	    int    i, j;
	    char * c;
	    char   debuginfo[2600];
	    /* TO-DO: put this debug code in a separate function */

	    c = *content;
	    for (j = 0, i = 0;  c[i] != '\0' && i < 2599;  i++)
	    {
	       if (c[i] == '\r'  ||  c[i] == '\n')
		  debuginfo[j++] = '_';
	       else if (isprint(c[i]))
		  debuginfo[j++] = c[i];
	       else
		  debuginfo[j++] = '.';
	    }
	    debuginfo[j] = '\0';
	    ufdbLogMessage( "W%03d: HTTPS protocol reply for %s:%d:\n   %s", 
                            worker, hostname, portnumber, debuginfo );
	 }

#if 0
         if (strstr( *content, "X-Kazaa-" ) != NULL)			/* TO-DO */
	 {
	    ufdbLogError( "HTTPS protocol on %s:%d uses Kazaa P2P",
	    		  hostname, portnumber );
	    status = UFDB_API_ERR_P2P;
	 }
#endif
	 /* TODO: investigate nomachine.com */
	 if (strstr( *content, "logmein.com/" ) != NULL  ||
	     strstr( *content, "hamachi.cc/" ) != NULL)
	 {
	    ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d uses a hamachi/logmein TUNNEL", 
	                    worker, hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( *content, "Set-Cookie: SSLX_SSESHID=" ) != NULL)
	 {
	    ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d uses an SSL-Explorer TUNNEL", 
	                    worker, hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( *content, "BarracudaServer.com" ) != NULL   ||
	     strstr( *content, "barracudaserver.com" ) != NULL   ||
	     strstr( *content, "BarracudaDrive" ) != NULL)
	 {
	    ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d uses a BARRACUDA proxy TUNNEL",
	     		    worker, hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( *content, "  index.vnc -" ) != NULL   ||
	     strstr( *content, "  used with Xvnc" ) != NULL   ||
	     strstr( *content, "TightVNC Java viewer applet" ) != NULL)
	 {
	    ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d uses a VNC proxy TUNNEL",
	                    worker, hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
	 else
	 if (strstr( *content, "X-Webtunnel-Status" ) != NULL   ||
	     strstr( *content, "X-webtunnel-status" ) != NULL  ||
	     strstr( *content, "X-webtunnel-version" ) != NULL  ||
	     strstr( *content, "X-Webtunnel-Version" ) != NULL)
	 {
	    ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d uses Webtunnel TUNNEL",
	                    worker, hostname, portnumber );
	    status = UFDB_API_ERR_TUNNEL;
	 }
      }
      else					/* server does not speak HTTP */
      {
	 int    i,j;
	 char * c;
	 char   debuginfo[2400];

	 ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d encapsulates a non-HTTP protocol %s",
		         worker, hostname, portnumber,
		         UFDBglobalUnknownProtocolOverHttps ? "and an unknown protocol is allowed" : 
		            "and is considered a PROXY TUNNEL because allow-unknown-protocol-over-https is OFF"
		       );
         if (UFDBglobalDebug > 1)
	 {
	    /* TO-DO: put this code in a separate function */
	    c = *content;
	    for (j = 0, i = 0;  c[i] != '\0' && i < 2399;  i++)
	    {
	       if (c[i] == '\r'  ||  c[i] == '\n')
		  debuginfo[j++] = '_';
	       else if (isprint(c[i]))
		  debuginfo[j++] = c[i];
	       else
		  debuginfo[j++] = '.';
	    }
	    debuginfo[j] = '\0';
	    ufdbLogMessage( "W%03d: HTTPS protocol reply for %s:%d:\n%s", 
                            worker, hostname, portnumber, debuginfo );
	 }

	 status = UFDBglobalUnknownProtocolOverHttps ? UFDB_API_ERR_UNKNOWN_PROTOCOL : UFDB_API_ERR_TUNNEL;
      }
   }

   (void) close( s );

   if (UFDBglobalDebug  ||  UFDBglobalLogAllRequests  ||
       UFDBglobalDebugAim || UFDBglobalDebugGtalk || UFDBglobalDebugSkype || UFDBglobalDebugYahooMsg || 
       UFDBglobalDebugFBchat || UFDBglobalDebugCitrixOnline)
   {
      ufdbLogMessage( "W%03d: HTTPS protocol on %s:%d has been verified and status is %s", 
      		      worker, hostname, portnumber, ufdbAPIstatusString(status) );
   }

   if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_LOG_ONLY)
      status = UFDB_API_OK;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBsslPeekServer  %s:%d  status updated to %s", 
                      worker, hostname, portnumber, ufdbAPIstatusString(status) );

   updateHTTPScache( hostname, portnumber, status, 0, &info );

   return status;
}

