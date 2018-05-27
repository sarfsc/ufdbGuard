/*
 * ufdblib.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdblib.c,v 1.147 2018/03/18 19:01:59 root Exp root $
 */

/* This module is well tested and stable for a long time.
 * For maximum performance _FORTIFY_SOURCE is undefined.
 */
#undef _FORTIFY_SOURCE

#if (__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif

/* to inline string functions with gcc : */
#if defined(__OPTIMIZE__) && defined(__GNUC__)  &&  defined(GCC_INLINE_STRING_FUNCTIONS_ARE_FASTER)
#define __USE_STRING_INLINES  1
#endif

#include "ufdb.h"
#include "ufdblib.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <syslog.h>
/* TODO: evaluate use of syslog() */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#if HAVE_UNIX_SOCKETS
#include <sys/un.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>

#if 0
#define strmatch2(a,b) (strcmp(a,b) == 0)
#define strmatch3(a,b) (strcmp(a,b) == 0)
#define strmatch4(a,b) (strcmp(a,b) == 0)
#define strmatch5(a,b) (strcmp(a,b) == 0)
#define strmatch6(a,b) (strcmp(a,b) == 0)
#define strmatch7(a,b) (strcmp(a,b) == 0)
#define strmatch8(a,b) (strcmp(a,b) == 0)
#else
#define strmatch2(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == '\0')
#define strmatch3(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == '\0')
#define strmatch4(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == '\0')
#define strmatch5(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  && (a)[5] == '\0')
#define strmatch6(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == '\0')
#define strmatch7(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == '\0')
#define strmatch8(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == '\0')
#endif

/* #define  UFDB_DEBUG_IPV6 */

/* #define UFDB_DO_DEBUG 1 */
#define UFDB_DO_DEBUG 0

#if UFDB_DO_DEBUG || 0
#define DEBUG(x) fprintf x 
#else
#define DEBUG(x) 
#endif

struct unknownURLentry;

#define UFDB_UUE_DOMAIN_LENGTH   (128)
#define UFDB_UUE_PATH_LENGTH     (128)
#define UFDB_UUE_REFERER_LENGTH	 (128 - sizeof(struct unknownURLentry *) - sizeof(long))

struct unknownURLentry 
{
   struct unknownURLentry * next;
   long    nhits;
   char    domain[UFDB_UUE_DOMAIN_LENGTH];
   char    firstpath[UFDB_UUE_PATH_LENGTH];
   char    referer[UFDB_UUE_REFERER_LENGTH];
};



UFDB_GCC_HOT UFDB_GCC_INLINE 
static char * findDomainEnd( char * url )
{
   return url + strcspn( url, "/?&;#" );
}


UFDB_GCC_HOT UFDB_GCC_INLINE 
static char * strchr_before( char * str, char * maxstr, char chr )
{
   return (char *) memchr( str, chr, maxstr - str );
}


UFDB_GCC_HOT UFDB_GCC_INLINE 
static char * findProtocolEnd( char * URL )
{
#if 0
   int i;

   for (i = 15; i > 0; i--)
   {
      URL++;	/* protocol name must have at least 1 character */

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
   }
#else
   {
      URL++;	/* protocol name must have at least 1 character */

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
      URL++;

      if (*URL == '\0')
         return NULL;
      if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
         return URL;
   }
#endif

   return NULL;
}


void UFDBappInit( void )
{
   ;
}


void UFDBtimerInit( struct tms * t )
{
   (void) times( t );
}


void UFDBtimerStop( struct tms * t )
{
   struct tms te;

   (void) times( &te );

   t->tms_utime  = te.tms_utime  - t->tms_utime;
   t->tms_stime  = te.tms_stime  - t->tms_stime;
   t->tms_cutime = te.tms_cutime - t->tms_cutime;
   t->tms_cstime = te.tms_cstime - t->tms_cstime;
}


void UFDBtimerPrintString( char * line, struct tms * t, const char * tag )
{
   double  numTicks;

   numTicks = (double) sysconf( _SC_CLK_TCK );

   if (tag == NULL)
      tag = "UFDB timer";

   sprintf( line, "%s:  %5.2f user  %5.2f sys  %5.2f total", 
	    tag,
            (double) t->tms_utime / numTicks, 
	    (double) t->tms_stime / numTicks, 
	    (double) (t->tms_utime+t->tms_stime) / numTicks );
}


void UFDBtimerPrint( struct tms * t, const char * tag )
{
   char    line[256];

   UFDBtimerPrintString( line, t, tag );
   ufdbLogMessage( "%s", line );
}


static void removeObfuscations( char * domain, char * URL )
{
   char * d;
   char * s;
   char * db;
   int    n, r;
   int    distance;
   int    oldDomainLength;
   int    newDomainLength;
   int    octetStart;
   char   newDomain[256];
   char   decimalBuf[16];

   /*
    *  Rewrite the domain and URL in buffer to remove the obfuscation
    *  First put a normalised domain in newDomain,
    *  then copy back newDomain to domain and rewrite a new URL in buffer.
    */
   s = domain;
   d = newDomain;

   /* remove obfuscations: parse hex and octal numbers in the octects and rewrite them in decimal notation */
   octetStart = 1;
   while (*s != '\0')
   {
      if (*s == '0'  &&  octetStart)
      {
	 s++;
	 n = 0;

	 /* interpret octal or hex ? */
	 if (*s == 'x')
	 {
	    s++;
	    while (isxdigit(*s))
	    {
	       n = n * 16 + ((*s <= '9') ? (*s - '0') : (*s - 'a' + 10));
	       s++;
	    }
	 }
	 else if (isdigit(*s))
	 {
	    while (isdigit(*s))
	    {
	       n = n * 8 + (*s - '0');
	       s++;
	    }
	 }
	 else
	    ;	/* it was a single '0' */


	 db = &decimalBuf[ sizeof(decimalBuf) - 1 ];
	 *db = '\0';
	 do			/* write the number in decimal notation (from right to left) */
	 {
	    db--;
	    r = n % 10;
	    *db = r + '0';
	    n = n / 10;
	 } while (n > 0);
	 while (*db != '\0')	/* copy the decimal number to the newDomain */
	    *d++ = *db++;
      }
      else
      {
	 octetStart = (*s == '.');
	 *d++ = *s++;
      }
   }
   *d = '\0';

   oldDomainLength = s - domain;
   newDomainLength = d - newDomain;

   /* update the domain */
   for (s = newDomain, d = domain;  *s != '\0';  s++)
      *d++ = *s;
   *d = '\0';

   /* update the URL */
   if (newDomainLength < oldDomainLength)
   {
      s = domain;
      d = URL;
      while (*s != '\0')
         *d++ = *s++;
      /* the shorter new domain is copied to the URL; now move the path inside the URL */
      distance = oldDomainLength - newDomainLength;
      while (*(d+distance) != '\0')
      {
         *d = *(d+distance);
	 d++;
      }
      *d = '\0';
   }
   else if (newDomainLength == oldDomainLength)
   {
      s = domain;
      d = URL;
      while (*s != '\0')
         *d++ = *s++;
      /* the path does not need any updating */
   }
   else
   {
      /* move the path and then copy in the larger domain (extremely unlikely) */
      int pathLength;
      pathLength = strlen( URL + oldDomainLength );
      memmove( URL+newDomainLength, URL+oldDomainLength, pathLength );

      s = domain;
      d = URL;
      while (*s != '\0')
         *d++ = *s++;
   }
}


UFDB_GCC_ALIGN64 
static pthread_mutex_t mutex_url_history = UFDB_STATIC_MUTEX_INIT;

#define UUE_MAX_ENTRIES		20000
#define UUE_HASH_TABLESIZE	25643
#define UUE_LIST_SIZE           (UUE_MAX_ENTRIES *  \
                                    (UFDB_UUE_DOMAIN_LENGTH + UFDB_UUE_PATH_LENGTH + 1 + 7 + 1 +  \
                                     UFDB_UUE_REFERER_LENGTH + 1))

static struct unknownURLentry * uues = NULL;			/* malloc-ed array with UUE_MAX_ENTRIES hashtable entry structs */
static char *                   uue_list = NULL;
static volatile int 	        n_uue = -1;

UFDB_GCC_ALIGN64 
static struct unknownURLentry * uueHash[UUE_HASH_TABLESIZE];


/* Store the domainname of a URL in the uue hashtable.
 * If it already exists, increment the nhits counter.
 *
 * Return 1 if the buffer is full, 0 otherwise.
 */
int ufdbRegisterUnknownURL( 
   char * webserver,
   int    portnumber,
   char * referer )
{
   int                       length;
   unsigned int              hv;
   char *                    path;
   struct unknownURLentry  * he;
   struct unknownURLentry ** prev;
   char                      domain[UFDB_UUE_DOMAIN_LENGTH];

   if (n_uue >= UUE_MAX_ENTRIES)
      return 1;

   /* webserver names that do not have a dot (.) have no domain name          */
   /* and can never be incorporated in the URL database and are skipped here. */
   if (webserver == NULL  ||  strchr( webserver, '.' ) == NULL)
      return 0;

   path = findDomainEnd( webserver );
   length = path - webserver;
   if (length == 0)
      return 0;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "ufdbRegisterUnknownURL: %s:%d%s", webserver, portnumber, path );

   if (length < UFDB_UUE_DOMAIN_LENGTH - 1)
   {
      strncpy( domain, webserver, length );
      domain[length] = '\0';
      if (portnumber != 80  &&  length < UFDB_UUE_DOMAIN_LENGTH - 6)
         sprintf( &domain[length], ":%d", portnumber );
   }
   else
   {
      /* the domainname is huge :-( */
      /* reduce the length of the domainname by stripping as many subdomains as required */
      webserver += (length - (UFDB_UUE_DOMAIN_LENGTH - 6 - 2 - 1));
      while (*webserver != '\0'  &&  *webserver != '.')
         webserver++;
      if (*webserver == '\0')
      {
         ufdbLogError( "ufdbRegisterUnknownURL: cannot store very long domainname %s", webserver );
         return 0;
      }
      path = findDomainEnd( webserver );
      length = path - webserver;
      strcpy( domain, "*." );
      strncpy( domain+2, webserver, length );
      domain[length+2] = '\0';
      if (portnumber != 80  &&  length < UFDB_UUE_DOMAIN_LENGTH - 6 - 2)
         sprintf( &domain[length+2], ":%d", portnumber );
   }

   /* calculate hash */
   hv = 100003;
   {
      char * t;
      for (t = domain;  *t != '\0';  t++)
         hv = (hv ^ (hv << 11)) ^ (*t * 17);
   }
   hv = hv % UUE_HASH_TABLESIZE;

   pthread_mutex_lock( &mutex_url_history );		/* ======================================= */

   if (uues == NULL)
   {
      uues = ufdbCalloc( sizeof(struct unknownURLentry), UUE_MAX_ENTRIES );
      n_uue = 0;
   }

   /* check again for full table */
   if (n_uue >= UUE_MAX_ENTRIES)
   {
      pthread_mutex_unlock( &mutex_url_history );				/* ============= */
      return 1;
   }

   /* find domain in uue table */
   prev = &uueHash[hv];
   he = uueHash[hv];
   while (he != NULL)
   {
      if (strcmp( domain, he->domain ) == 0)	
      {
	 /* found in hashtable */
         /* The URL path is saved in the hashtable, but the first one is never interesting,
          * so save number 2 and 3 also.
          */
         if (he->nhits < 3)   
         {
            ufdbStrncpy( he->firstpath, path, sizeof(he->firstpath) );
            if (referer != NULL)
               ufdbStrncpy( he->referer, referer, sizeof(he->referer) );
         }
         he->nhits++;
	 pthread_mutex_unlock( &mutex_url_history );				/* ============= */
	 return 0;
      }
      prev = &(he->next);
      he = he->next;
   }

   /* domain was not found in hashtable; prev points to pointer to assign new entry */
   he = &uues[n_uue];
   n_uue++;
   he->next = NULL;
   he->nhits = 1;
   strcpy( he->domain, domain );
   strcpy( he->firstpath, path );
   if (referer == NULL)
      he->referer[0] = '\0';
   else
      ufdbStrncpy( he->referer, referer, sizeof(he->referer) );

   *prev = he;

   pthread_mutex_unlock( &mutex_url_history );		/* ======================================= */
   return 0;
}


/*
 * Retrieve all registered uncategorised URLs.
 * This must be followed by a ufdbResetUnknownURLs().
 */
char * ufdbGetUnknownURLs( void )
{
   int    i;
   char * tail;
   struct unknownURLentry * he;

   pthread_mutex_lock( &mutex_url_history );		/* ======================================= */

   n_uue = UUE_MAX_ENTRIES;				/* prevent additions until call to ufdbResetUnknownURLs() */

   if (uue_list == NULL)
      uue_list = ufdbMalloc( UUE_LIST_SIZE );
   tail = uue_list;

   *tail++ = '|';
   *tail++ = 'N';					/* "N1" signals the receiver that the list has "<url> # <nhits>" */
   *tail++ = '1';
   *tail++ = '|';
   for (i = 0;  i < UUE_HASH_TABLESIZE;  i++)
   {
      for (he = uueHash[i];  he != NULL;  he = he->next)
      {
	 tail += sprintf( tail, "%s%s#%ld#%s|", he->domain, he->firstpath, he->nhits, he->referer );
	 if (tail > uue_list + UUE_LIST_SIZE - sizeof(struct unknownURLentry))
	    goto too_many;
      }
   }
too_many:
   *tail = '\0';
 
   pthread_mutex_unlock( &mutex_url_history );		/* ======================================= */

   return uue_list;
}


void ufdbResetUnknownURLs( void )
{
   int i;

   pthread_mutex_lock( &mutex_url_history );		/* ======================================= */

   if (uues == NULL)
   {
      uues = ufdbCalloc( sizeof(struct unknownURLentry), UUE_MAX_ENTRIES );
   }
   n_uue = 0;

   if (uue_list != NULL)
   {
      ufdbFree( uue_list );
      uue_list = NULL;
   }

   for (i = 0;  i < UUE_HASH_TABLESIZE;  i++)
      uueHash[i] = NULL;

   pthread_mutex_unlock( &mutex_url_history );		/* ======================================= */
}


int UFDBopenSocket( const char * serverName, int port )
{
   int                 s;
   int                 ret;
   int                 sock_parm;
   long                oldflags;
   time_t              t0, t;
   char                servicePort[8];
   struct addrinfo *   addrlist;
   struct addrinfo *   al;
   struct addrinfo     addrinfo_hints;
   struct timeval      tv;

   addrlist = NULL;
   sprintf( servicePort, "%d", port );

   addrinfo_hints.ai_flags = AI_NUMERICSERV;
#ifdef AI_IDN
   addrinfo_hints.ai_flags |= AI_IDN;
#endif
#ifdef AI_ADDRCONFIG
   addrinfo_hints.ai_flags |= AI_ADDRCONFIG;
#endif
   /* use AF_UNSPEC for IPv4 or IPv6; AF_INET for IPv4 only */
   addrinfo_hints.ai_family = UFDBglobalUseAlsoIPv6onWan ? AF_UNSPEC : AF_INET;
   addrinfo_hints.ai_socktype = SOCK_STREAM;
   addrinfo_hints.ai_protocol = IPPROTO_TCP;
   addrinfo_hints.ai_addrlen = 0;
   addrinfo_hints.ai_addr = NULL;
   addrinfo_hints.ai_canonname = NULL;
   addrinfo_hints.ai_next = NULL;

   t0 = time( NULL );
   ret = getaddrinfo( serverName, servicePort, &addrinfo_hints, &addrlist );
   if (ret != 0)
   {
      t = time( NULL );
      if (t - t0 >= 4)
	 ufdbLogMessage( "UFDBopenSocket: cannot resolve hostname %s: %s - %s"
	                 "getaddrinfo took %ld seconds to return.  Is the DNS server OK?  *****",
	                 serverName, gai_strerror(ret), 
                         ret == EAI_SYSTEM ? strerror(errno) : "errno is not set",
                         (t - t0) );
      else
      {
	 ufdbLogMessage( "UFDBopenSocket: cannot resolve hostname '%s': %s - %s", 
                         serverName, gai_strerror(ret),
                         ret == EAI_SYSTEM ? strerror(errno) : "errno is not set" );
      }
#ifdef EAGAIN
      errno = EAGAIN;
#endif
      return -1;
   }

   s = -1;
   al = addrlist;
   while (al != NULL)			/* try to connect to all returned addresses until one succeeds */
   {
      char addrbuf[INET6_ADDRSTRLEN+1];

      if (al->ai_family == AF_INET)
      {
	 if (inet_ntop( al->ai_family, &((struct sockaddr_in *) al->ai_addr)->sin_addr, addrbuf, 
                        sizeof(addrbuf) ) == NULL)
	    strcpy( addrbuf, "unknown" );
      }
      else
      {
	 if (inet_ntop( al->ai_family, &((struct sockaddr_in6 *) al->ai_addr)->sin6_addr, addrbuf, 
                        sizeof(addrbuf) ) == NULL)
	    strcpy( addrbuf, "unknown" );
      }
      if (UFDBglobalDebug > 1  ||  UFDBglobalPeek)
      {
	 ufdbLogMessage( "UFDBopenSocket: '%s' resolved to '%s', port %d", serverName, addrbuf, port );
      }

      s = socket( al->ai_family, SOCK_STREAM, 0 );
      if (s < 0)
      {
         if (UFDBglobalDebug > 1  ||  UFDBglobalPeek)
         {
            ufdbLogMessage( "UFDBopenSocket: could not open socket to '%s' (%s) port %d: %s - %s",
                            serverName, addrbuf, port, strerror(errno),
                            al->ai_next == NULL ? "there are no other addresses to try" : "trying next address" );
         }
         al = al->ai_next;
	 continue;	/* we may not be able to make an IPv6 socket, so continue trying other sockets */
      }

      /*
       *  Prevent that the connect takes ages.  Use an aggressive timeout of 8 seconds.
       */
      tv.tv_sec = 8;
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
      tv.tv_sec = 8;
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

      sock_parm = 200 * 1024;
      setsockopt( s, SOL_SOCKET, SO_SNDBUF, (void *) &sock_parm, sizeof(sock_parm) );

      /* The RCVTIMEO and SNDTIMEO do not work on all OSes (e.g. Solaris) so
       * we will set the socket in non-blocking mode, use select with a timeout and
       * check the socket status for successful connection.
       * After this, the socket goes back to blocking mode.
       */
      oldflags = fcntl( s, F_GETFL, NULL );
      if (oldflags < 0)
	 oldflags = 0;   /* ignore errors */
      if (fcntl( s, F_SETFL, oldflags|O_NONBLOCK ))
	 ;

      /* with anti-aliasing warnings ON, connect/bind cause compiler warnings which we may ignore */
      if (connect( s, al->ai_addr, al->ai_addrlen) < 0)
      {
	 if (errno == EINPROGRESS)
	 {
	    int rv;
	    fd_set fdset;
	    /* tv is already set to 8 seconds */
	    FD_ZERO( &fdset );
	    FD_SET( s, &fdset );
	    errno = 0;
	    rv = select( s+1, NULL, &fdset, NULL, &tv );
	    if (rv < 0)
	    {
	       /* error; fall through */
	    }
	    else if (rv == 0)
	    {
	       /* timed out; fall through */
	       errno = ETIMEDOUT;
	    }
	    else
	    {
	       /* select() signalled that there is I/O. check socket error status */
	       int errorStatus;
	       socklen_t len;
	       len = sizeof(int);
	       if (getsockopt( s, SOL_SOCKET, SO_ERROR, (void *) &errorStatus, &len ) == 0)
	       {
		  if (!errorStatus)
		  {
		     /* go back to blocking mode */
		     if (fcntl( s, F_SETFL, oldflags ))
			;
		     goto socket_ok;
		  }
		  /* error; fall through */
		  if (errno == 0)
		     errno = ECONNREFUSED;
	       }
	       else
	       {
		  /* error; fall through */
	       }
	    }
	 }
	 if (errno == EINPROGRESS)
	    errno = EAGAIN;
	 ufdbLogError( "UFDBopenSocket: cannot connect to %s/%s  port %d: %s - %s",
                       serverName, addrbuf, port, strerror(errno),
                       al->ai_next == NULL ? "there are no other addresses to try" : "trying next address" );
	 close( s );
	 s = -1;
      }

      al = al->ai_next;
   }
socket_ok:
   freeaddrinfo( addrlist );

#if 0
   if (al == NULL)
      return -1;
#endif

   if (s >= 0)
   {
      /*
       *  Prevent long blocking on communication with the other side.
       */
      tv.tv_sec = 20;
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );

      tv.tv_sec = 20;
      tv.tv_usec = 0;
      setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );
   }

   return s;
}


UFDB_GCC_INLINE
static int UFDBnormaliseIPv6( 
   const char * address,
   char *       normalised )
{
   struct in6_addr ipv6;

   *normalised = '\0';

   if (inet_pton( AF_INET6, address, (void *) &ipv6 ) <= 0)
      return 0;

   if (inet_ntop( AF_INET6, (void *) &ipv6, normalised, INET6_ADDRSTRLEN ) == NULL)
      return 0;

#ifdef UFDB_DEBUG_IPV6
   ufdbLogMessage( "      UFDBnormaliseIPv6  %s  ->  %s", address, normalised );
#endif

   return 1;	/* successful */
}


char * UFDBparseIPv6address( 
   char * url,
   char * domain )
{
   char * url_start;
   char * d;
   char   normalisedAddress[INET6_ADDRSTRLEN];

#ifdef UFDB_DEBUG_IPV6
   ufdbLogMessage( "   UFDBparseIPv6address: url: %s", url );
#endif

   url_start = url;
   *domain = '\0';
   d = domain;
   if (*url == '[')
   {
      url++;
   }
   
   while (*url != '\0')
   {
      if (*url == ']')
      {
	 *d++ = ']';
	 *d = '\0';
	 if (UFDBnormaliseIPv6( domain, normalisedAddress ))
	    strcpy( domain, normalisedAddress );
	 else
	 {
	    if (UFDBglobalDebug)
	        ufdbLogMessage( "URL has invalid IPv6 address: %s", *url_start=='[' ? url_start+1 : url_start );
	    return NULL;
	 }
	 /* TODO: handle IPv4 in IPv6 addresses e.g.  ::127.0.0.1 */
	 return url;
      }

      if (*url == ':'  ||  *url == '.'  ||  isxdigit(*url))
      {
         *d++ = *url++;
      }
      else	/* URL address error */
      {
	 *d = '\0';
	 if (UFDBglobalDebug)
	     ufdbLogMessage( "URL has invalid IPv6 address: %s", *url_start=='[' ? url_start+1 : url_start );
         return NULL;
      }
   }
   *d = '\0';

#ifdef UFDB_DEBUG_IPV6
   ufdbLogMessage( "   UFDBparseIPv6address: domain: %s", domain );
#endif

   if (UFDBnormaliseIPv6( domain, normalisedAddress ))
   {
#ifdef UFDB_DEBUG_IPV6
      ufdbLogMessage( "      IPv6 domain '%s' normalised to '%s'", domain, normalisedAddress );
#endif
      strcpy( domain, normalisedAddress );
   }
   else
   {
      if (UFDBglobalDebug)
	  ufdbLogMessage( "URL has invalid IPv6 address: %s", *url_start=='[' ? url_start+1 : url_start );
      return NULL;  /* address error */
   }

   /* TODO: handle IPv4 in IPv6 addresses e.g.  ::127.0.0.1 */
   return url;
}


void UFDBupdateURLwithNormalisedDomain(
   char * url,
   char * newDomain )
{
#ifdef UFDB_DEBUG_IPV6
   char * oldURL;
#endif
   char * oldEnd;
   int    n;
   int    nbytes;

#ifdef UFDB_DEBUG_IPV6
   oldURL = url;
   ufdbLogMessage( "      UFDBupdateURLwithNormalisedDomain: %s", url );
#endif

   if (*url != '[')
   {
      ufdbLogError( "UFDBupdateURLwithNormalisedDomain: URL does not start with '[': %s", url );
      return;
   }
   url++;

   oldEnd = strchr( url, ']' );
   if (oldEnd == NULL)
   {
      ufdbLogError( "UFDBupdateURLwithNormalisedDomain: URL does not have a ']': %s", url );
      return;
   }

   while (1)
   {
      if (*url == ']')
      {
         if (*newDomain == '\0')	/* the normalised domain name has equal length */
	    return;
	 /* the newDomain string is longer than the original */
	 n = strlen( newDomain );
	 nbytes = strlen( url ) + 1;
	 memmove( url+n, url, nbytes );
	 while (*newDomain != '\0')
	    *url++ = *newDomain++;
	 return;
      }

      if (*newDomain == '\0')
      {
         /* the newDomain string is shorter than the original */
	 nbytes = strlen( oldEnd ) + 1;
	 memmove( url, oldEnd, nbytes );
#ifdef UFDB_DEBUG_IPV6
         ufdbLogMessage( "      UFDBupdateURLwithNormalisedDomain: %s", oldURL );
#endif
	 return;
      }

      *url++ = *newDomain++;
   }
}


void UFDBnormaliseIPv4( char * domain )
{
   char *       d;
   char *       orig;
   unsigned int octetvalue;
   char         dbuf[512];

   orig = domain;
   d = dbuf;
   while (*domain != '\0')
   {
      if (*domain == '0')
      {
	 domain++;
	 octetvalue = 0;
         if (*domain == 'x')				/* obfuscated hexadecimal octet */
	 {
	    domain++;
	    while (isxdigit(*domain))
	    {
	       octetvalue *= 16;
	       if (*domain >= '0' && *domain <= '9')
	          octetvalue += (*domain - '0');
	       else
	          octetvalue += (*domain - 'a' + 10);
	       domain++;
	    }
	    if (*domain != '\0'  &&  *domain != '.')
	    {
	       ufdbLogError( "IPv4 address has illegal hexadecimal octet: %s", orig );
	       return;
	    }
	 }
	 else if (*domain >= '0'  &&  *domain <= '7')	/* obfuscated octal octet */
	 {
	    while (*domain >= '0'  &&  *domain <= '7')
	    {
	       octetvalue *= 8;
	       octetvalue += (*domain - '0');
	       domain++;
	    }
	    if (*domain != '\0'  &&  *domain != '.')
	    {
	       ufdbLogError( "IPv4 address has illegal octal octet: %s", orig );
	       return;
	    }
	 }
	 else
	 {
	    ufdbLogError( "IPv4 address has illegal octet: %s", orig );
	    return;
	 }
	 if (octetvalue > 255)
	 {
	    ufdbLogError( "obfuscated IPv4 address has illegal octet value: %s", orig );
	    return;
	 }
	 /* convert the octetvalue to a decimal string */
	 d += sprintf( d, "%u", octetvalue );
      }
      else				/* octet is not obfuscated */
      {
         while (*domain != '\0'  &&  *domain != '.')
	    *d++ = *domain++;
      }

      if (*domain == '.')
	 *d++ = *domain++;
   }
   *d = '\0';

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "obfuscated domain %s rewritten to %s", orig, dbuf );

   strcpy( orig, dbuf );
}


int UFDBaddYoutubeEdufilter(
   char * domain,
   char * strippedURL,
   char * originalURL  )
{
   char * dot;

   if (strcmp( domain, "youtube.com" ) == 0)
   {
#if 0
      ufdbLogMessage( "   YouTube Edufilter: %s %s", domain, strippedURL );
#endif
      dot = strrchr( strippedURL, '.' );
      if (dot == NULL)
      {
	 if (strchr( strippedURL, '?' ) == NULL)
	    strcat( originalURL, "?edufilter=" );
	 else
	    strcat( originalURL, "&edufilter=" );
	 strcat( originalURL, UFDBglobalYoutubeEdufilterID );

	 return UFDB_API_MODIFIED_FOR_YOUTUBE_EDUFILTER;
      }
      else
      {
	 if (strcmp( dot+1, "css" ) != 0  &&
	     strcmp( dot+1, "ico" ) != 0  &&
	     strcmp( dot+1, "gif" ) != 0  &&
	     strcmp( dot+1, "jpg" ) != 0  &&
	     strcmp( dot+1, "png" ) != 0  &&
	     strcmp( dot+1, "js" )  != 0  &&
	     strcmp( dot+1, "xml" ) != 0)
	 {
	    if (strchr( dot, '?' ) == NULL)
	       strcat( originalURL, "?edufilter=" );
	    else
	       strcat( originalURL, "&edufilter=" );
	    strcat( originalURL, UFDBglobalYoutubeEdufilterID );

	    return UFDB_API_MODIFIED_FOR_YOUTUBE_EDUFILTER;
	 }
      }
   }

   return UFDB_API_OK;
}


/*
 * UFDBaddSafeSearch - modify a URL for a search which requires SafeSearch
 *
 * return UFDB_API_OK for unmodified URLs and UFDB_API_MODIFIED_FOR_SAFESEARCH
 *
 * parameters: domain - the domainname
 *             strippedURL - the stripped URL including the domainname
 *             originalURL - the unmodified user-supplied URL
 *	       The originalURL must be of type char[UFDB_MAX_URL_LENGTH]
 *	       and may be modified to force SafeSearch.
 */
int UFDBaddSafeSearch(
   char * domain,
   char * strippedURL,
   char * originalURL  )
{
   char * slash;

   originalURL[UFDB_MAX_URL_LENGTH-28] = '\0';

   slash = strchr( strippedURL, '/' );
   if (slash == NULL)
      strippedURL = (char *) "";
   else
      strippedURL = slash;

#if 0
   printf( "   SS: %s %s\n", domain, strippedURL );
#endif

   if (strstr( domain, "similar-images.googlelabs." ) != NULL  &&	/* Google images */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "images.google." ) != NULL  &&			/* Google images */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "youtube.com" ) != NULL  &&			/* Youtube */
       strstr( strippedURL, "search_query=") != NULL)
   {
      strcat( originalURL, "&safety_mode=true" );			/* unfortunately this does not work since */
      UFDB_API_num_safesearch++;					/* also need to set Cookie: (.*) PREF=f2=8000000 */
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((domain[0] <= '9' && domain[0] >= '0')  &&			/* google-related sites like www.google-tr.info */
       strstr( strippedURL, "cx=partner" ) != NULL  &&
       strstr( strippedURL, "/cse" ) != NULL  &&
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((domain[0] <= '9' && domain[0] >= '0')     &&			/* google.com, google.de, google.ws etc. */
       strncmp( strippedURL, "/search", 7 ) == 0  &&
       strstr( strippedURL, "q=" ) != NULL        &&
       (strncmp( domain, "74.125.", 7 ) == 0  ||
        strncmp( domain, "173.194.", 8 ) == 0))
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "google." ) != NULL  ||				/* SAFESEARCH: google.* */
        strcmp( domain, "ajax.googleapis.com" ) == 0  ||
        strstr( domain, "googleusercontent.com" ) != NULL)  &&		/* Google */
       strstr( strippedURL, "q=" ) != NULL  &&
       ((strncmp( strippedURL, "/insights", 9 ) != 0  &&  strstr( strippedURL, "/search" ) != NULL)  ||
        strstr( strippedURL, "/uds/afs" ) != NULL ||
        strstr( strippedURL, "/uds/gwebsearch" ) != NULL ||
        strstr( strippedURL, "/uds/gvideosearch" ) != NULL ||
        strstr( strippedURL, "/uds/gimagesearch" ) != NULL ||
        strstr( strippedURL, "/uds/gblogsearch" ) != NULL ||
        strstr( strippedURL, "/videosearch" ) != NULL ||
        strstr( strippedURL, "/blogsearch" ) != NULL ||
        strstr( strippedURL, "/gwebsearch" ) != NULL ||
        strstr( strippedURL, "/groups" ) != NULL ||
        strstr( strippedURL, "/cse" ) != NULL ||
        strstr( strippedURL, "/products" ) != NULL ||
        strstr( strippedURL, "/images" ) != NULL ||
        strstr( strippedURL, "/custom" ) != NULL) )
   {
      char * safe;
      /* search for 'safe=off' and replace by 'safe=active' */
      safe = strstr( originalURL, "&safe=off" );
      if (safe != NULL)
      {
         safe += 6;
	 *safe++ = 'a';		/* 'o' */
	 *safe++ = 'c';		/* 'f' */
	 *safe++ = 't';		/* 'f' */
	 (void) memmove( safe+3, safe, strlen(safe)+1 );
	 *safe++ = 'i';
	 *safe++ = 'v';
	 *safe   = 'e';
      }
      strcat( originalURL, "&safe=active&safeui=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webmenu.com" ) != NULL  &&		/* SAFESEARCH: webmenu.com */
       (strstr( strippedURL, "q_or=") != NULL  ||
        strstr( strippedURL, "q_and=") != NULL  ||
        strstr( strippedURL, "ss=") != NULL  ||
        strstr( strippedURL, "keyword=") != NULL  ||
	strstr( strippedURL, "query=") != NULL) )
   {
      char * p;
      /* TODO: fix problem of cookie override; a user can set preferences to turn the filter OFF
       * in the user preferences.
       */
      while ((p = strstr( originalURL, "&ss=n" )) != NULL)
         *(p+4) = 'y';
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "blekko.com" ) != NULL &&		/* SAFESEARCH: blekko.com */
       strncmp( strippedURL, "/ws/", 4 ) == 0)
   {
      if (strchr( strippedURL, '?' ) == NULL)
	 strcat( originalURL, "?safesearch=2" );
      else
	 strcat( originalURL, "&safesearch=2" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "izito." ) != NULL  &&			/* SAFESEARCH: izito.* */
       (strstr( strippedURL, "query=" ) != NULL  ||
        strstr( strippedURL, "q=" ) != NULL))
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "zapmeta." ) != NULL  &&			/* SAFESEARCH: zapmeta.* */
       strstr( strippedURL, "vid=" ) != NULL  &&
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "bing.com" ) != NULL  &&			/* SAFESEARCH: bing. */
       strstr( strippedURL, "q=" ) != NULL)    			/* bing */
   {
      strcat( originalURL, "&ADLT=STRICT&filt=all" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "bing.co.uk" ) != NULL  &&		/* SAFESEARCH: bing.co.uk */
       strstr( strippedURL, "q=" ) != NULL)    			/* bing */
   {
      strcat( originalURL, "&ADLT=STRICT&filt=all" );
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "api.bing.net" ) == 0  &&		/* Safesearch: bing API */
       strncmp( strippedURL, "/json.aspx", 10 ) == 0  &&	/* called by searchgby.com */
       strstr( strippedURL, "query=" ) != NULL)
   {
      strcat( originalURL, "&Adult=Strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "search.searchcompletion.com" ) == 0  &&		/* SAFESEARCH: searchcompletion.com */
       strncmp( strippedURL, "/localsearchresults.aspx", 10 ) == 0  &&		/* search.searchcompletion.com/LocalSearchResults.aspx */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safe=on" );			/* TO-DO: fix this */
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "pageset.com" ) != NULL  &&		/* pageset.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      char * t;
      t = strstr( strippedURL, "adt=1" );
      if (t != NULL)
         *(t+4) = '0';
      else
	 strcat( originalURL, "&adt=0" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "trovator.com" ) != NULL  &&		/* searchcompletion.com trovator.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&fil=si" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "results.searchlock.com" ) == 0  &&	/* SAFESEARCH: searchlock.com */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&sf=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, ".yauba.com" ) != NULL  &&		/* SAFESEARCH: yauba.com */
       strstr( strippedURL, "query=") != NULL)
   {
      strcat( originalURL, "&ss=y" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "forestle.org" ) != NULL  &&		/* SAFESEARCH: forestle.org */
       (strstr( strippedURL, "settings") != NULL  ||
        strstr( strippedURL, "q=") != NULL))
   {
      strcat( originalURL, "&adultfilter=noadult" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "zombol.com" ) != NULL  &&		/* SAFESEARCH: zombol.com */
       strstr( strippedURL, "/results") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "kalooga.com" ) != NULL  &&		/* SAFESEARCH: kalooga.com */
       strstr( strippedURL, "search") != NULL  &&
       strstr( strippedURL, "query=") != NULL)
   {
      strcat( originalURL, "&filter=default" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "muuler.com" ) != NULL  &&		/* SAFESEARCH: muuler.com */
       strstr( strippedURL, "/result") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "foozir.com" ) != NULL  &&		/* SAFESEARCH: foozir.com */
       strstr( strippedURL, "/result") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "moons.it" ) != NULL  &&			/* SAFESEARCH: moons.it */
       strstr( strippedURL, "/ricerca") != NULL  &&
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&safe=active" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "wotbox.com" ) != NULL  &&		/* SAFESEARCH: wotbox.com */
       (strstr( strippedURL, "q=") != NULL  ||
        strstr( strippedURL, "op0=") != NULL) )
   {
      strcat( originalURL, "&a=true" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ant.com" ) != NULL  &&			/* SAFESEARCH: ant.com */
       strstr( strippedURL, "antq=") != NULL)
   {
      strcat( originalURL, "&safe=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "duck.co" ) != NULL  ||			/* SAFESEARCH: duck.co */
        strstr( domain, "duckduckgo.org" ) != NULL  ||          /* SAFESEARCH: duckduckgo.org  */
        strstr( domain, "duckduckgo.com" ) != NULL)  &&         /* SAFESEARCH: duckduckgo.com  */
       strstr( strippedURL, "q=") != NULL)
   {
      strcat( originalURL, "&kp=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, "qbyrd.com" ) != NULL  ||  		/* SAFESEARCH: qbyrd.com */
        strstr( domain, "search-results.com" ) != NULL)  &&	/* SAFESEARCH: search-results.com */
       strstr( strippedURL, "q=") != NULL)
   {
      char * adt;
      adt = strstr( originalURL, "adt=1" );
      if (adt != NULL)
         *(adt+4) = '0';
      else
	 strcat( originalURL, "&adt=0" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "easysearch.org.uk" ) != NULL  &&	/* SAFESEARCH: easysearch.org.uk */
       strstr( strippedURL, "search") != NULL)
   {
      strcat( originalURL, "&safe=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ecosia.org" ) != NULL  &&
       strstr( strippedURL, "q=" ) != NULL)    			/* SAFESEARCH: ecosia.org */
   {
      strcat( originalURL, "&safeSearch:1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "ask.com" ) != NULL  &&
       strchr( strippedURL, '?' ) != NULL)    			/* SAFESEARCH: ask.com */
   {
      strcat( originalURL, "&adt=0" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "api.search.yahoo.", 17 ) == 0  &&	/* SAFESEARCH: API yahoo.* */
       strstr( strippedURL, "query=" ) != NULL)
   {
      strcat( originalURL, "&adult_ok=0" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strcmp( domain, "search.aol.com" ) == 0  ||
        strstr( domain, ".aolsearch.com" ) != NULL)  &&		/* Safesearch: AOL */
       strncmp( strippedURL, "/search", 7 ) == 0  &&
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safesearch=1&sp_ss=1" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if ((strstr( domain, ".terra." ) != NULL  &&  		/* SAFESEARCH: terra.* */
        strstr( domain, "busca" ) != NULL)  &&
       (strstr( strippedURL, "query=" ) != NULL  ||  
        strstr( strippedURL, "source=" ) != NULL) )	  /* .ar .br .cl .co .ec .es */
   {
      strcat( originalURL, "&npl=%26safe%3dhigh" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "search.alot.com" ) == 0  &&		/* SAFESEARCH: alot.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&f=1" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "searchalot.com" ) != NULL  &&		/* SAFESEARCH: searchalot.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safesearch=high" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "alltheinternet.com" ) != NULL  &&	/* SAFESEARCH: alltheinternet.com */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&safesearch=high" );				
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "search.yahoo." ) != NULL  &&		/* SAFESEARCH: yahoo.* */
       strstr( strippedURL, "p=" ) != NULL)
   {
      strcat( originalURL, "&vm=r" );				
      /* TODO: investigate http://www.yahoo.com/r/sx/ *-http://search.yahoo.com/search */
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "inspsearchapi.com" ) != NULL  &&
       strncmp( strippedURL, "/search", 7 ) == 0)
   {
      strcat( originalURL, "&family-friendly=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "excite." ) != NULL  &&			/* SAFESEARCH: excite.* */
       strstr( strippedURL, "search" ) != NULL  &&
       strchr( strippedURL, '?' ) != NULL)  			/* Excite */
   {
      strcat( originalURL, "&familyfilter=1&splash=filtered" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.msn.", 11 ) == 0)		/* SAFESEARCH: msn.* */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.live.", 12 ) == 0  &&		/* SAFESEARCH: live.* */
       strstr( strippedURL, "q=" ) != NULL)
   {
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "api.search.live.net" ) == 0  &&		/* Safesearch: live API */
       strstr( strippedURL, "sources=" ) != NULL)
   {
      strcat( originalURL, "&adlt=strict" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "blinkx.com" ) != NULL  &&		/* SAFESEARCH: blinkx.com */
       strchr( strippedURL, '?' ) != NULL)
   {
      strcat( originalURL, "&safefilter=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strcmp( domain, "etools.ch" ) == 0  &&
       strncmp( strippedURL, "/search", 7 ) == 0)
   {
      strcat( originalURL, "&safeSearch=true" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strncmp( domain, "search.lycos.", 13 ) == 0)		/* SAFESEARCH: lycos.* */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&contentFilter=strict&family=on" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "dogpile.com" ) != NULL  ||		/* SAFESEARCH: dogpile.com */
       strstr( domain, "dogpile.co.uk" ) != NULL)		/* SAFESEARCH: dogpile.co.uk  */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&adultfilter=heavy" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "infospace.com" ) != NULL)		/* SAFESEARCH: infospace.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "metacrawler.com" ) != NULL)		/* SAFESEARCH: metacrawler.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webfetch.com" ) != NULL  ||		/* SAFESEARCH: webfetch.com */
       strstr( domain, "webfetch.co.uk" ) != NULL)		/* SAFESEARCH: webfetch.co.uk */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }
   else
   if (strstr( domain, "webcrawler.com" ) != NULL)		/* SAFESEARCH: webcrawler.com */
   {
      if (slash == NULL)
         strcat( originalURL, "/" );
      strcat( originalURL, "&familyfilter=1" );
      UFDB_API_num_safesearch++;
      return UFDB_API_MODIFIED_FOR_SAFESEARCH;
   }

   return UFDB_API_OK;
}


#if 0
UFDB_GCC_HOT UFDB_GCC_INLINE 
static int squeeze_html_char( 
   char * p,  
   int *  hex )
{
   int    length;

   length = 0;
   *hex = 0;
   while (*p != '\0'  &&  isxdigit( (int) *p ))
   {
      int h;
      h = (*p <= '9') ? *p - '0' : *p - 'a' + 10;
      *hex = *hex * 16 + h;
      p++;
      length++;
   }

#if 0
   ufdbLogMessage( "   squeeze_html_char hex=%04x  length=%d  *p=%c", *hex, length, *p );
#endif

   if (*p != ';')
      return -1;		/* '&#xxx' without trailing ';' is not a valid HTML character */

   if (*hex == 0)
      return length;

   if (*hex < 0x0020)
   {
      if (*hex != '\t'  &&  *hex != '\n'  &&  *hex != '\r'  &&  *hex != '\f')
	 *hex = ' ';
   }
   else if (*hex == 0x007f  ||  *hex >= 0x00ff)
   {
      *hex = ' ';
   }
   else if (*hex <= 'Z'  &&  *hex >= 'A')
   {
      *hex += 'a' - 'A';
   }

   return length;
}
#endif


UFDB_GCC_INLINE  UFDB_GCC_HOT
static void increment_UFDB_API_num_url_lookups( void )
{
#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_LONG__ == 4
   (void) __sync_add_and_fetch( &UFDB_API_num_url_lookups, 1 );
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_LONG__ == 8
   (void) __sync_add_and_fetch( &UFDB_API_num_url_lookups, 1 );
#else
   static pthread_mutex_t incrMutex = PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &incrMutex );
   UFDB_API_num_url_lookups++;
   pthread_mutex_unlock( &incrMutex );
#endif
}


UFDB_GCC_INLINE  UFDB_GCC_HOT 
void increment_UFDB_API_num_https( void )
{
#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_LONG__ == 4
   (void) __sync_add_and_fetch( &UFDB_API_num_https, 1 );
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_LONG__ == 8
   (void) __sync_add_and_fetch( &UFDB_API_num_https, 1 );
#else
   static pthread_mutex_t incrMutex = PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &incrMutex );
   UFDB_API_num_https++;
   pthread_mutex_unlock( &incrMutex );
#endif
}


/*
 * strip a URL:
 * remove http:// prefix, 
 * remove www[0-9*]. prefix,
 * remove port number, 
 * remove username and password,
 * remove IP address obfuscations (numbers with leading zeroes)
 * convert hex codes (%61 = 'a') to characters,
 * convert special characters to void or space,
 * convert characters to lower case.
 * substitute // by / in a URL
 * substitute /./ by / in a URL
 * substitute /foo/../bar by /bar in a URL
 * trim very long URLs (more than 512 letters)
 */
UFDB_GCC_HOT
void UFDBstripURL2( 
   char * URL, 			/* input URL string */
   int    stripwwwprefix,	/* input flag for stripping "www." prefix from URL */
   char * strippedUrl,  	/* output char array (must be UFDB_MAX_URL_LENGTH bytes) */
   char * domain,       	/* output char array (must be 1024 bytes) */
   char * protocol,		/* output char array (must be 16 bytes) */
   int  * portnumber )		/* output integer */
{
   char * p;
   char * tmp;
   char * domain_start;
   char * domain_end;
   char * optional_token;
   char * origStrippedUrl;
   int    port;
   int    is_ip_address;
   int    is_ipv6;
   int    obfuscated;
   int    insideParams;
   char   buffer[UFDB_MAX_URL_LENGTH];

   /*
    * This parser has the following parsing stages:
    * 1) parse protocol and save it (output).
    * 2) skip optional username:password@ and initial "www[nnn]."
    * 3) parse FQDN, stop at ':' (port number) or any of / ; ? # (start of path)
    *    if the FQDN starts with '[' the URL has an IPv6 and parsing is done by separate function.
    * 4) store optional port (output) (default is 80, or 443 if protocol is https)
    *    URL without protocol, username/password, port is now stored in temporary buffer 'buffer'
    *    NEW in v1.32: the parameters of the URL are also copied to the buffer.
    * 5) The FDQN is saved in domainname (output).
    * 6) The URL buffer is converted to lowercase but parameters are not converted.
    * 7) Check for obfuscated IP addresses and normalise them
    * 8) Copy/convert the buffer to strippedURL (output) converting %xx and &#xx; and // and /./ and /../
    */

   *portnumber = 80;
   is_ipv6 = 0;

   increment_UFDB_API_num_url_lookups();

   /* strip http:// and ftp:// protocol header */
   p = findProtocolEnd( URL );
   if (p != NULL)
   {
      int n;
      n = p - URL;
      if (n == 0 || n > 14)
      {
	 /* ERROR: an absent or very large protocol name... The URL does not start with a valid protocol. 
	  */
      	 strcpy( protocol, "http" );
	 p = URL;
      }
      else
      {
	 memcpy( protocol, URL, n );
	 protocol[n] = '\0';
	 if (n == 5  &&  strcasecmp( protocol, "https" ) == 0)
	 {
	    increment_UFDB_API_num_https();
	    *portnumber = 443;
	 }
	 p += 3;			/* skip '://' */
      }
   }
   else 
   {
      strcpy( protocol, "http" );
      p = URL;
   }

   domain_end = findDomainEnd( p );				/* might not be accurate and skipped the ':port' */

   optional_token = strchr_before( p, domain_end, '@' );	/* strip user:password@ */
   if (optional_token != NULL)
      p = optional_token + 1;

   domain_start = p;

#if 0
   if (UFDBglobalDebug)
      ufdbLogMessage( "   UFDBstripURL2: p: %s\n", p );
#endif

   if (*p == '[')			/* IPv6 URL: http://[::1]:80/index.html */
   {
      char * end;
      char * oldBracket;

      is_ipv6 = 1;
      oldBracket = strchr( p, ']' );
      if (oldBracket != NULL)
         *oldBracket = '\0';
      end = UFDBparseIPv6address( p, domain );
      if (oldBracket != NULL)
         *oldBracket = ']';
      if (end != NULL)
      {
	 UFDBupdateURLwithNormalisedDomain( p, domain );
	 /* uh-oh: the normalised domain is usually smaller and our pointers have moved */
	 domain_end = findDomainEnd( p );
	 oldBracket = strchr( p, ']' );
	 if (oldBracket == NULL)
	    oldBracket = domain_end - 1;

	 optional_token = strchr_before( oldBracket, domain_end, ':' );
#if 0
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "    UFDBstripURL2:  domain_end: %08x oldBracket: %08x  token: %08x\n", 
		            domain_end, oldBracket, optional_token );
#endif
      }
      else
	 optional_token = NULL;
   }
   else
   {
      if (stripwwwprefix)					    /* strip www[0-9]{0,2}. */
      {
	 if ((p[0] == 'w' || p[0] == 'W')  && 
	     (p[1] == 'w' || p[1] == 'W')  &&
	     (p[2] == 'w' || p[2] == 'W'))
	 {
	    tmp = p + 3;
	    if (*tmp <= '9'  &&  *tmp >= '0')
	       tmp++;
	    if (*tmp <= '9'  &&  *tmp >= '0')
	       tmp++;
	    if (*tmp == '.'  &&  strchr_before( tmp+1, domain_end, '.' ) != NULL)
	       p = tmp + 1;
	 }
      }
      optional_token = strchr_before( p, domain_end, ':' );
   }

   								/* parse-and-strip ":<portnum>" */
   if (optional_token != NULL)
   {
      tmp = buffer;						/* copy domain name */
      while (p < optional_token)
         *tmp++ = *p++;
      *tmp = '\0';

      p++;
      port = 0;
      while (*p <= '9'  &&  *p >= '0')
      {
	 port = port * 10 + (*p - '0');
         p++;
      }

      if (port == 443  &&  *portnumber != 443)			/* Squid sends "www.example.com:443" with CONNECT */
      {
	 increment_UFDB_API_num_https();
         strcpy( protocol, "https" );
      }

      *portnumber = port;
      ufdbStrncpy( tmp, p, UFDB_MAX_URL_LENGTH-256-6-1 );	/* copy rest of the URL */
   }
   else
   {
      tmp = buffer;
      while (1)
      {
         if (*p == '\0') break;
	 *tmp++ = *p++;

         if (*p == '\0') break;
	 *tmp++ = *p++;

         if (*p == '\0') break;
	 *tmp++ = *p++;

         if (*p == '\0') break;
	 *tmp++ = *p++;

         if (*p == '\0') break;
	 *tmp++ = *p++;

         if (*p == '\0') break;
	 *tmp++ = *p++;
      }
      *tmp = '\0';
   }

   if (!is_ipv6)				/* save the original domainname */
   {
      int n;

      if (optional_token != NULL)
	 domain_end = optional_token;
      n = domain_end - domain_start;
      memcpy( domain, domain_start, n );
      domain[n] = '\0';
   }

   /*
    * Now a temporary URL is in 'buffer'.
    * The temporary URL has no protocol, portnum, username/password, initial "www[nnn].".
    * Convert URL to lower case but stop at the first '?' to leave parameters untouched.
    */
   tmp = buffer;
   while (*tmp != '\0')
   {
      if (*tmp == '?')				/* TODO: also stop at ; and &  ?? */
         goto stop_lowercasing;
      if (*tmp <= 'Z'  &&  *tmp >= 'A')
	 *tmp += 'a' - 'A';
      tmp++;
   }
   *tmp++ = '\0';				/* prevent problems with % at end of URL */
   *tmp = '\0';
stop_lowercasing:

#if 0
   if (UFDBglobalDebug)
      ufdbLogMessage( "   UFDBstripURL2: after lowercasing: %s\n", buffer );
#endif

   /* scan for IP address obfuscations */
   obfuscated = 0;
   is_ip_address = 1;
   for (tmp = domain;  *tmp != '\0';  )
   {
      if (*tmp == '0')
	 obfuscated = 1;
      if (*tmp == '0' && *(tmp+1) == 'x')				/* parse 1 hex octet 0xHH */
      {
         tmp += 2;
	 while (*tmp != '\0'  &&  isxdigit(*tmp))
	    tmp++;
	 if (*tmp != '\0'  &&  *tmp != '.')
	 {
	    is_ip_address = 0;
	    break;
	 }
      }
      else if (*tmp <= '9' && *tmp >= '0')
      {
	 while (*tmp != '\0'  &&  (*tmp <= '9' && *tmp >= '0'))		/* parse 1 octal or integer octet */
	    tmp++;
	 if (*tmp != '\0'  &&  *tmp != '.')
	 {
	    is_ip_address = 0;
	    break;
	 }
      }
      else
      {
	 is_ip_address = 0;
	 break;
      }
      if (*tmp == '.')
         tmp++;
   }
   if (is_ip_address  &&  obfuscated)
      removeObfuscations( domain, buffer );

   /*
    *  Copy the buffer to strippedUrl, while converting hex codes to characters.
    *  After the first '?' we only do %HH character conversion
    */
   insideParams = 0;
   origStrippedUrl = strippedUrl;
   p = buffer;
   while (*p != '\0')
   {
      if (*p == ':'  &&  *(p+1) == '/'  &&  *(p+2) == '/')		/* do not replace :// by :/  */
      {
         *strippedUrl++ = *p++;
         *strippedUrl++ = *p++;
         *strippedUrl++ = *p++;
      }
      else if (*p == '%')				/* start of a HEX code */
      {
         if (isxdigit(*(p+1)) && isxdigit(*(p+2)))
	 {
	    char   h;
	    int    hex;

	    h = *(p+1);
            if (h <= '9')
               hex = (h - '0') * 16;
            else if (h <= 'F')
               hex = (h - 'A' + 10) * 16;
            else
               hex = (h - 'a' + 10) * 16;
	    h = *(p+2);
            if (h <= '9')
               hex += (h - '0');
            else if (h <= 'F')
               hex += (h - 'A' + 10);
            else
               hex += (h - 'a' + 10);
	    /* be careful with control characters */
	    if (hex < 0x20)
	    {
	       if (hex == 0)
	       {
	          p += 3;
		  continue;
	       }
               hex = ' ';
	       *strippedUrl++ = hex;
	       p += 3;
	    }
	    else
	    {
	       if (!insideParams  &&  hex <= 'Z'  &&  hex >= 'A')
		  hex += 'a' - 'A';
	       else if (hex == 0x7f)
	          hex = ' ';

	       *strippedUrl++ = hex;
	       p += 3;
	    }
	 }
	 else 					/* erroneous code */
	 {
	    *strippedUrl++ = *p++;		/* just copy the '%' */
	 }
      }
      else					/* plain character */
      {
	 while (*p == '/')
	 {
	    if (*(p+1) == '/')					/* substitute // by /    but not in "xxx://" */
	       p++;
	    else if (*(p+1) == '.'  && *(p+2) == '/')		/* substitute /./ by / */
	       p += 2;
	    else if (*(p+1) == '.'  &&  *(p+2) == '.'  &&  *(p+3) == '/')    /* substitute /xxxx/../ by / */
	    {
	       /* try to find the previous directory... */
	       char * tmp;
	       tmp = strippedUrl - 1;
	       while (*tmp != '/'  &&  tmp > origStrippedUrl)
		  tmp--;
	       if (tmp > origStrippedUrl)
	       {
		  strippedUrl = tmp;
		  p += 3;
	       }
	       else
		  break;
	    }
	    else
	       break;
	 }
	 if (*p == '?')		/* just copy the '?' */
	 {
            insideParams = 1;
            *strippedUrl++ = *p++;
	 }
	 else
	    *strippedUrl++ = *p++;
      }
   }
   *strippedUrl++ = '\0';
   *strippedUrl = '\0';		/* sset2 requires double \0 termination */
}


UFDB_GCC_HOT
void UFDBstripURL( 
   char * URL, 			/* input URL string */
   char * strippedUrl,  	/* output char array (must be UFDB_MAX_URL_LENGTH bytes) */
   char * domain,       	/* output char array (must be 1024 bytes) */
   char * protocol,		/* output char array (must be 16 bytes) */
   int  * portnumber )		/* output integer */
{
   UFDBstripURL2( URL, 1, strippedUrl, domain, protocol, portnumber );
}


char * UFDBprintable( char * string )
{
   char * p;

   if (string == NULL)
      return (char *) "NULL";

   p = string;
   while (*p != '\0')
   {
      if (*p < 32  ||  *p > 126)
         *p = '?';
      p++;
   }

   return string;
}


char * UFDBfgets( 
   char * requestBuffer, 
   int    bufferSize, 
   FILE * fp )
{
   char * b;
   int    ch;
   int    size;

   b = requestBuffer;
   size = 1;

   while ((ch = getc_unlocked(fp)) != EOF)
   {
      *b++ = ch;
      if (ch == '\n')
         goto end;
      if (++size == bufferSize)
         goto end;
   }

   if (b == requestBuffer  &&  (feof(fp) || ferror(fp)))
      return NULL;

end:
   *b = '\0';
   return requestBuffer;
}


/*
 * Setting functions
 */
void ufdbSetting( 
   const char * name, 
   char *       value )
{
   struct ufdbSetting * sp;

   if (strcmp( name, "administrator" ) == 0)
   {
      char * p;

      while ((p = (char *) strchr( value, '?' )) != NULL)
         *p = '_';
      while ((p = (char *) strchr( value, '&' )) != NULL)
         *p = '_';
   }

   if (strcmp( name, "port" ) == 0)
   {
      UFDBglobalPortNum = atoi( value );
      ufdbFree( (void *) value );
      if (UFDBglobalPortNum <= 0)
      {
         ufdbLogError( "port number must be > 0, using default port %d", UFDB_DAEMON_PORT );
	 UFDBglobalPortNum = UFDB_DAEMON_PORT;
      }
      return;
   }

   if (Setting != NULL)
   {
      if (ufdbSettingFindName(name) != NULL)
      {
         ufdbLogFatalError( "%s: setting %s is defined multiple times in configuration file %s",
		            UFDBprogname, name, UFDBglobalConfigFile );
	 ufdbFree( value );
         return;
      }
   }

   sp = (struct ufdbSetting *) ufdbMalloc( sizeof(struct ufdbSetting) );
   sp->name = ufdbStrdup( name );
   sp->value = value;
   sp->next = NULL;

   if (Setting == NULL) 
   {
      Setting = sp;
      lastSetting = sp;
   }
   else
   {
      lastSetting->next = sp;
      lastSetting = sp;
   }

   if (strcmp(name,"pidfile") == 0) 
   {
      if (UFDBglobalPidFile != NULL)
         ufdbFree( UFDBglobalPidFile );
      UFDBglobalPidFile = ufdbStrdup( value );
      return;
   }

   if (strcmp(name,"dbhome") == 0) 
   {
      struct stat dirbuf;

      if (stat( value, &dirbuf ) != 0)
      {
         ufdbLogFatalError( "dbhome: directory does not exist or "
	                    "access rights are insufficient (value is \"%s\")", 
		            value );
      }
      else
      {
         if (!S_ISDIR(dirbuf.st_mode))
	    ufdbLogFatalError( "dbhome: %s is not a directory", value );
      }
      return;
   }

   if (strcmp(name,"logdir") == 0) 
   {
      if (UFDBglobalLogDir != NULL)
	 ufdbFree( UFDBglobalLogDir );
      UFDBglobalLogDir = ufdbStrdup( value );
      ufdbSetGlobalErrorLogFile( 0 );
   }
}


struct ufdbSetting * ufdbSettingFindName( 
   const char * name )
{
   struct ufdbSetting * p;

   for (p = Setting;  p != NULL;  p = p->next)
   {
      if (strcmp(name,p->name) == 0)
         return p;
   }
   return NULL;
}


char * ufdbSettingGetValue( 
   const char * name )
{
   struct ufdbSetting * p;

   p = ufdbSettingFindName( name );
   if (p != NULL)
      return p->value;
   return NULL;
}


void ufdbGetSysInfo( 
   struct utsname * si )
{
   if (uname( si ) < 0)
   {
      strcpy( si->machine, "M?" );
      strcpy( si->release, "R?" );
      strcpy( si->nodename, "unknown" );
      strcpy( si->sysname, "sysname" );
   }
   else
   {
      si->machine[ sizeof(si->machine)-1 ] = '\0';
      si->release[ sizeof(si->release)-1 ] = '\0';
      si->sysname[ sizeof(si->sysname)-1 ] = '\0';

      (void) gethostname( si->nodename, sizeof(si->nodename) );
      si->nodename[ sizeof(si->nodename)-1 ] = '\0';
   }
}


long ufdbGetNumCPUs( void ) 
{
   long num_cpus;

#if defined(_SC_NPROCESSORS_ONLN)
   num_cpus = sysconf( _SC_NPROCESSORS_ONLN );

#elif defined(__NR_sched_getaffinity)
   /* sched_setaffinity() is buggy on linux 2.4.x so we use syscall() instead */
   cpu = syscall( __NR_sched_getaffinity, getpid(), 4, &cpu_mask );
   /* printf( "sched_getaffinity returned %d %08lx\n", cpu, cpu_mask ); */
   if (cpu >= 0)
   {
      num_cpus = 0;
      for (cpu = 0; cpu < 32; cpu++)
         if (cpu_mask & (1 << cpu))
            num_cpus++;
      /* printf( "   found %d CPUs in the cpu mask\n", num_cpus ); */
   }
   else
#else
      num_cpus = 0;
#endif

   return num_cpus;
}


char * UFDBfgetsNoNL( char * s, int size, FILE * stream )
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


int UFDBcalcCksum( char * mem, long size )
{
   unsigned int cksum = 17;

   while (--size >= 0)
   {
      cksum = cksum * 13 + ((unsigned int) *mem++) * 3;
   }
   return (int) (cksum % 100000);
}


#if (__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4)
#pragma GCC pop_options
#endif

