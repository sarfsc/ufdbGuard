/*
 * ufdbHostnames.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB with all rights reserved.
 *
 * Cached IP and hostname lookups.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdbHostnames.c,v 1.5 2017/02/13 18:46:19 root Exp root $
 */

#include "ufdb.h"
#include "ufdbHostnames.h"
#include "ufdblib.h"
#include "ufdbHashtable.h"

#include <time.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

static struct UFDBhashtable * IPcache = NULL;
static pthread_mutex_t  IPcacheLock = UFDB_STATIC_MUTEX_INIT;
static time_t lastIPcachePurgeTime = 0;
static time_t OldestInIPcache = 0;


struct IPcacheElem {
   char * host;
   int    status;
   char * copyOfIP;
   time_t lastAccess;
};


unsigned int IPhashFunc( const void * key )
{
   unsigned char * k;
   unsigned int    value;

   value = 19;
   k = (unsigned char *) key;
   while (*k != '\0')
   {
      value = (value * 13)  ^  ((((unsigned int)(*k)) << 4) + (unsigned int)(*k));
      k++;
   }
   return value & 0x7FFFFFFF;
}


int IPkeyEqFunc( const void * av, const void * bv )
{
#if 0
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "            IPkeyEqFunc( %s, %s )", (char *) av, (char *) bv );		/* TODO: remove */
#endif

   return strcmp( (char *) av, (char *) bv ) == 0;
}


UFDB_GCC_INLINE
static int findIPinCache( char * ip, char * host, int * status )
{
   struct IPcacheElem * elem;

   if (IPcache == NULL)
      return 0;

   elem = (struct IPcacheElem *) UFDBsearchHashtable( IPcache, ip, 0 );
   if (elem == NULL)
      return 0;
      
   /* Note that we do not bother here about UFDBglobalDatabaseLoadTime since cached IP addresses do not change */

   elem->lastAccess = time( NULL );
   *status = elem->status;
   if (*status)
      strcpy( host, elem->host );

   return 1;
}


static int purgeIPcache( const void * key, const void * value )
{
   struct IPcacheElem * elem;

   elem = (struct IPcacheElem *) value;
   if (elem->lastAccess < OldestInIPcache)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "      key %s purged", (char*) key );
      ufdbFree( elem->host );
      ufdbFree( elem->copyOfIP );
      /* caller frees key and value */
      return 1;
   }

   return 0;
}


static void addIPtoCache( char * ip, char * host, int status )
{
   struct IPcacheElem * elem;
   time_t               now;

   now = time( NULL );
   if (IPcache == NULL)
   {
      pthread_mutex_lock( &IPcacheLock );
      if (IPcache == NULL)
      {
	 IPcache = UFDBcreateHashtable( 317, IPhashFunc, IPkeyEqFunc );
	 lastIPcachePurgeTime = now;
      }
      pthread_mutex_unlock( &IPcacheLock );
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "      addIPtoCache: IP %s  host %s  status %d", ip, host, status );

   elem = (struct IPcacheElem *) UFDBsearchHashtable( IPcache, ip, 1 );
   if (elem != NULL)
   {
      if (UFDBglobalDebug > 1)
	 ufdbLogMessage( "      addIPtoCache: IP %s already in cache.  updated status to %d", ip, status );
      elem->status = status;
      elem->lastAccess = now;
      return;
   }

   elem = (struct IPcacheElem *) ufdbMalloc( sizeof(struct IPcacheElem) );
   elem->host = ufdbStrdup( host );
   elem->status = status;
   elem->copyOfIP = ufdbStrdup( ip );	/* TODO: this is the same as the key ?!?! */
   elem->lastAccess = now;

   /* before calling UFDBinsertHashtable, check if the cache needs a purge and 
    * take advantage of the hash table already being locked.
    */
   if ((IPcache->nEntries > 9500  &&  now > lastIPcachePurgeTime + 60*60)  ||  (now > lastIPcachePurgeTime + 3*60*60))
   {
      int oldnEntries;

      if (UFDBglobalDebug > 1)
	 ufdbLogMessage( "      addIPtoCache: going to purge the IP cache.  nEntries %d  now %ld  lastIPcachePurgeTime %ld  diff %ld", 
			 IPcache->nEntries, now, lastIPcachePurgeTime, now - lastIPcachePurgeTime );

      oldnEntries = IPcache->nEntries;
      if (oldnEntries > 9500)
	 OldestInIPcache = now - 2 * 60 * 60;		/* 2 hours */
      else
	 OldestInIPcache = now - 3 * 60 * 60;		/* 3 hours */
      UFDBpurgeHashtable( IPcache, purgeIPcache );

      lastIPcachePurgeTime = now;
      ufdbLogMessage( "addIPtoCache: IP cache %08lx was purged: %d of %d entries were removed", 
                      (unsigned long) IPcache, oldnEntries - IPcache->nEntries, oldnEntries );
   }

   UFDBinsertHashtable( IPcache, ufdbStrdup(ip), elem, 1 );
}


static int UFDBfindCorrectHostNameForIPv6( char * ip, char * host )
{
   struct sockaddr_in6 sa;
   struct addrinfo     addrinfo_hints;
   struct addrinfo *   addrinfo;
   struct addrinfo *   list;
   int ret;

   sa.sin6_family = AF_INET6;
   sa.sin6_port = htons( 443 );
   inet_pton( AF_INET6, ip, &sa.sin6_addr );
   ret = getnameinfo( (struct sockaddr *) &sa, sizeof(struct sockaddr_in6), host, 1023, NULL, 0, NI_NAMEREQD );
   if (ret != 0)
   {
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "      UFDBfindCorrectHostNameForIPv6: reverse lookup for IPv6 %s failed: %s", ip, gai_strerror(ret) );
      addIPtoCache( ip, "unknown", 0 );
      return 0;
   }

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "      UFDBfindCorrectHostNameForIPv6: reverse hostname for IPv6 %s is %s  - going to doublecheck...", ip, host );

   addrinfo = NULL;
   addrinfo_hints.ai_flags = 0;
   addrinfo_hints.ai_family = AF_INET6;
   addrinfo_hints.ai_socktype = SOCK_STREAM;
   addrinfo_hints.ai_protocol = IPPROTO_TCP;
   addrinfo_hints.ai_addrlen = 0;
   addrinfo_hints.ai_addr = NULL;
   addrinfo_hints.ai_canonname = NULL;
   addrinfo_hints.ai_next = NULL;

   ret = getaddrinfo( host, NULL, &addrinfo_hints, &addrinfo );
   if (ret != 0)
   {
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "      UFDBfindCorrectHostNameForIPv6: cannot resolve hostname %s (reverse of IPv6 %s): %s", host, ip, gai_strerror(ret) );
      addIPtoCache( ip, host, 0 );
      return 0;
   }

   /* got a list of reverse addresses... See if one matches with the original IP */
   ret = 0;
   for (list = addrinfo;  list != NULL;  list = list->ai_next)
   {
      if (list->ai_family == AF_INET6)
      {
         if (memcmp( (void*) &((struct sockaddr_in6 *)list->ai_addr)->sin6_addr, (void*) &sa.sin6_addr, sizeof(sa.sin6_addr) ) == 0)
	 {
	    ret = 1;
	    if (UFDBglobalDebug)
	    {
	       char buf[INET6_ADDRSTRLEN];
	       ufdbLogMessage( "      UFDBfindCorrectHostNameForIPv6: reverse address match for %s and %s", 
	                       host, inet_ntop(AF_INET6,&((struct sockaddr_in6 *)list->ai_addr)->sin6_addr,buf,sizeof(buf) ) );
	    }
	    break;
	 }
      }
   }
   addIPtoCache( ip, host, ret );

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "      UFDBfindCorrectHostNameForIPv6: IPv6 %s %s a correct and verified reverse hostname %s", ip, ret ? "has" : "does not have", host );

   freeaddrinfo( addrinfo );
   return ret;
}


/*
 * Given an IP address, try to get the FQDN.
 * The result is doublechecked because not all reverse names are correct.
 * E.g. proxy.org is 174.120.113.29 and
 * 174.120.113.29 is 1d.71.78ae.static.theplanet.com
 *
 * return values:
 *    0  not found
 *    1  found,    host parameter is assigned
 */
int UFDBfindCorrectHostNameForIP( char * ip, char * host )
{
   struct sockaddr_in  sa;
   struct addrinfo     addrinfo_hints;
   struct addrinfo *   addrinfo;
   struct addrinfo *   list;
   int ret;

   if (findIPinCache( ip, host, &ret ))
      return ret;

   if (strchr( ip, ':' ) != NULL)
      return UFDBfindCorrectHostNameForIPv6( ip, host );

   sa.sin_family = AF_INET;
   sa.sin_port = htons( 443 );
   inet_pton( AF_INET, ip, &sa.sin_addr );
   ret = getnameinfo( (struct sockaddr *) &sa, sizeof(struct sockaddr_in), host, 1023, NULL, 0, NI_NAMEREQD );
   if (ret != 0)
   {
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "      UFDBfindCorrectHostNameForIP: reverse lookup for IP %s failed: %s", ip, gai_strerror(ret) );
      addIPtoCache( ip, "unknown", 0 );
      return 0;
   }

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "      UFDBfindCorrectHostNameForIP: reverse hostname for IP %s is %s  - going to doublecheck...", ip, host );

   addrinfo = NULL;
   addrinfo_hints.ai_flags = 0;
   addrinfo_hints.ai_family = AF_INET;
   addrinfo_hints.ai_socktype = SOCK_STREAM;
   addrinfo_hints.ai_protocol = IPPROTO_TCP;
   addrinfo_hints.ai_addrlen = 0;
   addrinfo_hints.ai_addr = NULL;
   addrinfo_hints.ai_canonname = NULL;
   addrinfo_hints.ai_next = NULL;

   ret = getaddrinfo( host, NULL, &addrinfo_hints, &addrinfo );
   if (ret != 0)
   {
      if (UFDBglobalDebug > 2)
	 ufdbLogMessage( "      UFDBfindCorrectHostNameForIP: cannot resolve hostname %s (reverse of IP %s): %s", host, ip, gai_strerror(ret) );
      addIPtoCache( ip, host, 0 );
      return 0;
   }

   /* got a list of reverse addresses... See if one matches with the original IP */
   ret = 0;
   for (list = addrinfo;  list != NULL;  list = list->ai_next)
   {
      if (list->ai_family == AF_INET)
      {
         if (memcmp( (void*) &((struct sockaddr_in *)list->ai_addr)->sin_addr, (void*) &sa.sin_addr, sizeof(sa.sin_addr) ) == 0)
	 {
	    ret = 1;
	    if (UFDBglobalDebug)
	    {
	       char buf[INET_ADDRSTRLEN];
	       ufdbLogMessage( "      UFDBfindCorrectHostNameForIP: reverse address match for %s and %s", 
	                       host, inet_ntop(AF_INET,&((struct sockaddr_in *)list->ai_addr)->sin_addr,buf,sizeof(buf) ) );
	    }
	    break;
	 }
      }
   }
   addIPtoCache( ip, host, ret );

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "      UFDBfindCorrectHostNameForIP: IP %s %s a correct and verified reverse hostname %s", ip, ret ? "has" : "does not have", host );

   freeaddrinfo( addrinfo );
   return ret;
}

