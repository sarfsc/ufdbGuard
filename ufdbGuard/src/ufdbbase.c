/*
 * ufdbbase.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 * 
 * RCS $Id: ufdbbase.c,v 1.116 2018/03/18 18:57:17 root Exp root $
 */

#undef _FORTIFY_SOURCE

#define UFDB_MALLOC_DEBUG     	 0



#include "ufdb.h"
#include "ufdblib.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#if HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pwd.h>

#ifndef UFDB_API_NO_THREADS
#include <pthread.h>
#endif

#if HAVE_SETRESUID
int setresuid( uid_t ruid, uid_t euid, uid_t suid );
int getresuid( uid_t * ruid, uid_t * euid, uid_t * suid );
#endif


static long          num_cpus;
static unsigned long cpu_mask = 0UL;
#define UFDB_MAX_CPUS 64


#if defined(__GLIBC__)
#if (__GLIBC__ > 2) || (__GLIBC__ == 2  &&  __GLIBC_MINOR__ >= 4)
#define NEED_REGEXEC_MUTEX 0
#else
#define NEED_REGEXEC_MUTEX 1
#endif
#else
#define NEED_REGEXEC_MUTEX 0
#endif

#if NEED_REGEXEC_MUTEX
pthread_mutex_t ufdb_regexec_mutex = UFDB_STATIC_MUTEX_INIT;
#endif

#ifdef UFDB_DEBUG
pthread_mutex_t ufdb_malloc_mutex = UFDB_STATIC_MUTEX_INIT;
#endif


/*
 *  Some libraries use malloc() ...
 *  But they do not use a mutex, so when these libraries are called,
 *  we must use a mutex around the calls to the libraries for those
 *  platforms where malloc() is not threadsafe.
 */
void __inline__ ufdbGetMallocMutex( char * fn )
{
#ifdef UFDB_DEBUG
   int res;
#endif

#ifdef UFDB_MALLOC_IS_THREADSAFE
   return;
#endif

#ifdef UFDB_DEBUG
   res = pthread_mutex_lock( &ufdb_malloc_mutex );
   if (res != 0)
      ufdbLogError( "ufdbGetMallocMutex: mutex_lock failed with code %d in %s", res, fn );
#endif
}


void __inline__ ufdbReleaseMallocMutex( char * fn )
{
#ifdef UFDB_DEBUG
   int res;
#endif

#ifdef UFDB_MALLOC_IS_THREADSAFE
   return;
#endif

#ifdef UFDB_DEBUG
   res = pthread_mutex_unlock( &ufdb_malloc_mutex );
   if (res != 0)
      ufdbLogError( "ufdbReleaseMallocMutex: mutex_unlock failed with code %d in %s", res, fn );
#endif
}


UFDB_GCC_MALLOC_ATTR
void * ufdbMalloc( size_t elsize )
{
   void * p;

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      ufdbMalloc %ld", (long) elsize );
#endif

   p = (void *) malloc( elsize );
   if (p == NULL  &&  elsize > 0)
   {
      ufdbLogFatalError( "cannot allocate %ld bytes memory", (long) elsize );
   }

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "         ufdbMalloc %ld -> 0x%08lx", (long) elsize, (long) p );
#endif

   return p;
}


void * UFDB_GCC_MALLOC_ATTR ufdbMallocAligned( int alignment, size_t elsize )
{
   void * p;
   int    retval;

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      ufdbMallocAligned %d, %ld", alignment, (long) elsize );
#endif

#if HAVE_POSIX_MEMALIGN
   retval = posix_memalign( &p, alignment, elsize );
#else
   retval = 0;
   p = memalign( alignment, elsize );
#endif

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "         ufdbMallocAligned with posix_memalign(%d):%d %ld -> 0x%08lx",
		      alignment, retval, (long) elsize, (long) p );
#endif
   if (retval == 0)
      return p;

   p = (void *) malloc( elsize );
   if (p == NULL  &&  elsize > 0)
   {
      ufdbLogFatalError( "cannot allocate %ld bytes %d-aligned memory", (long) elsize, alignment );
   }

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "         ufdbMallocAligned not-aligned %ld -> 0x%08lx", (long) elsize, (long) p );
#endif

   return p;
}


void * ufdbCalloc( size_t n, size_t num )
{
   void * p;

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      ufdbCalloc %ld %ld", (long) n, (long) num );
#endif

   p = (void *) calloc( n, num );
   if (p == NULL  &&  n > 0)
   {
      ufdbLogFatalError( "cannot allocate %ld bytes zeroed memory", (long) (n*num) );
   }

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "         ufdbCalloc %ld %ld  = %ld  -> 0x%08lx", (long) n, (long) num, (long) n*num, (long) p );
#endif

   return p;
}



void * ufdbRealloc( void * ptr, size_t elsize )
{
   void * p;

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      ufdbRealloc 0x%08lx %ld", (long) ptr, (long) elsize );
#endif

   p = (void *) realloc( ptr, elsize );
   if (p == NULL  &&  elsize > 0)
   {
      ufdbLogFatalError( "cannot reallocate %ld bytes memory", (long) elsize );
   }

#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "         ufdbRealloc 0x%08lx %ld  ->  %08lx", (long) ptr, (long) elsize, (long) p );
#endif

   return p;
}


void ufdbFree( void * ptr )
{
#if UFDB_MALLOC_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      ufdbFree 0x%08lx", (long) ptr );
#endif

   if (ptr != NULL)
   {
      free( ptr );
#if UFDB_MALLOC_DEBUG
      if (UFDBglobalDebug)
	 ufdbLogMessage( "      ufdbFree 0x%08lx freed", (long) ptr );
#endif
   }
}


char * ufdbStrdup( const char * s )
{
   int size;

#if defined(UFDB_DEBUG)
   ufdbLogMessage( "   ufdbStrdup %-60.60s", s );
#endif

   size = strlen( s ) + 1;
   return strcpy( (char *) ufdbMalloc(size), s );
}


int ufdbStrStrEnd( const char * s, const char * end )
{
   int n;

   n = strlen( end );
   while (s != NULL  &&  *s != '\0')
   {
      s = strstr( s, end );
      if (s != NULL)
      {
         if (*(s+n) == '\0')
	    return 1;
	 else
	    s++;
      }
   }
   return 0;
}



int UFDBregcomp( regex_t * preg, const char * regex, int cflags )
{
   int retval;

   retval = regcomp( preg, regex, cflags );
#if 0
   ufdbLogMessage( "REGCOMP  %16lx  %d\n", (unsigned long) preg, retval );
#endif

   return retval;
}


UFDB_GCC_INLINE
void UFDBregfree( regex_t * preg )
{
#if 0
   ufdbLogMessage( "REGFREE  %16lx\n", (unsigned long) preg );
#endif
   regfree( preg );
}


/* TO-DO: make a macro for UFDBregexec() */
UFDB_GCC_HOT UFDB_GCC_INLINE
int UFDBregexec( const regex_t * preg, const char * string, size_t nmatch, regmatch_t pmatch[], int eflags )
{
   int retval;

#if NEED_REGEXEC_MUTEX
   pthread_mutex_lock( &ufdb_regexec_mutex );
#endif

   retval = regexec( preg, string, nmatch, pmatch, eflags );

#if NEED_REGEXEC_MUTEX
   pthread_mutex_unlock( &ufdb_regexec_mutex );
#endif

   return retval;
}


struct ufdbRegExp * ufdbNewPatternBuffer( 
   char * pattern, 
   int    flags )
{
   struct ufdbRegExp * re;
   int i;

   re = (struct ufdbRegExp *) ufdbMallocAligned( 64, sizeof(struct ufdbRegExp) );

#ifndef UFDB_API_NO_THREADS
   re->next_nregex_i = 0;
   pthread_mutex_init( &(re->lock), NULL );
#endif

   re->compiled[0] = (regex_t *) ufdbCalloc( 1, sizeof(regex_t) );
   re->error = UFDBregcomp( re->compiled[0], pattern, flags );
   if (!re->error)
   {
      for (i = 1; i < UFDB_NREGEX; i++)
      {
	 re->compiled[i] = (regex_t *) ufdbCalloc( 1, sizeof(regex_t) );
	 (void) UFDBregcomp( re->compiled[i], pattern, flags );
      }
   }
   re->pattern = ufdbStrdup( pattern );
   re->substitute = NULL;
   re->flags = flags;
   re->global = 0;
   re->httpcode = NULL;
   re->next = NULL;
 
   return re;
}


/* 
 *  optimise list of RE1 ... REn into one RE with (RE1)| ... |(REn) 
 */
struct ufdbRegExp * UFDBoptimizeExprList( 
   char *              reSource,
   struct ufdbRegExp * reList )
{
   struct ufdbRegExp * re;
   int    n;
   int    totalStrlen;

   n = 0;
   totalStrlen = 0;

   for (re = reList;  re != NULL;  re = re->next)
   {
      if (re->error == 0)
      {
         n++;
	 totalStrlen += strlen( re->pattern );
      }
   }

   if (n == 0)
   {
      if (UFDBglobalDebugRegexp)
	 ufdbLogMessage( "   UFDBoptimizeExprList: %s has no REs.", reSource );
      return NULL;
   }

   if (UFDBglobalDebug || UFDBglobalDebugRegexp)
      ufdbLogMessage( "   UFDBoptimizeExprList: %s has %d REs", reSource, n );
   if (UFDBglobalDebugRegexp > 1)
   {
      ufdbLogMessage( "   UFDBoptimizeExprList: %d REs of %s are not optimised due to debugging", n, reSource );
      return reList;
   }

   if (n > 1)
   {
      char * newpattern;
      newpattern = (char *) ufdbMalloc( totalStrlen + 3*n + 1 );
      newpattern[0] = '\0';
      for (re = reList;  re != NULL;  re = re->next)
      {
         if (re->error == 0)
	 {
#ifdef UFDB_USE_OPTIM_RE_BRACKETS
	    strcat( newpattern, "(" );
	    strcat( newpattern, re->pattern );
	    if (re->next == NULL)
	       strcat( newpattern, ")" );
	    else
	       strcat( newpattern, ")|" );
#else
	    strcat( newpattern, re->pattern );
	    if (re->next != NULL)
	       strcat( newpattern, "|" );
#endif
	 }
      }
      if (UFDBglobalDebug > 2  ||  UFDBglobalDebugRegexp)
         ufdbLogMessage( "going to optimise complex expression of %d subexpressions and %d characters",
                         n, totalStrlen + 3*n + 1 );
      if (n > 500)
         ufdbLogMessage( "WARNING: the expressionlist has %d expressions and may use many resources  *****\n"
			 "Note that large numbers of expressions may impact performance considerably",
	                 n );
      re = ufdbNewPatternBuffer( newpattern, reList->flags );
      if (re->error) 
      {
         ufdbLogError( "UFDBoptimizeExprList: unable to optimise %d expressions of %s", n, reSource );
	 ufdbFree( newpattern );
	 ufdbFreeRegExprList( re );
	 return reList;
      }

      ufdbLogMessage( "the %d expressions of %s have been optimised", n, reSource );
      if (UFDBglobalDebugRegexp)
         ufdbLogMessage( "optimised expression: %s", newpattern );

      ufdbFree( newpattern );
      ufdbFreeRegExprList( reList );
      return re;
   }
   else if (n == 1)
   {
      if (UFDBglobalDebugRegexp || UFDBglobalDebug)
	 ufdbLogMessage( "the expressions of %s has one RE and does not need optimisation", reSource );
   }

   return reList;
}


/*
 * initialize an expression list (read them from a file and do the regexp compilation)
 */
int UFDBloadExpressions( 
   struct ufdbRegExp ** exprList,
   char *               file  )
{
   FILE *               fin;
   char *               eoln;
   struct ufdbRegExp *  re;
   struct ufdbRegExp *  last;
   int                  retCode;
   char                 line[1024];

   if (exprList == NULL)
      return UFDB_API_ERR_NULL;
   *exprList = NULL;

   if (file == NULL)
      return UFDB_API_ERR_NULL;

   fin = fopen( file, "r" );
   if (fin == NULL)
      return UFDB_API_ERR_NOFILE;

   retCode = UFDB_API_OK;
   last = NULL;
   re = NULL;

   while (fgets( line, sizeof(line), fin ) != NULL)
   {
      if (line[0] == '#')         /* skip comments */
         continue;

      eoln = strchr( line, '\n' );
      if (eoln == NULL  ||  eoln == &line[0])
         continue;	/* skip empty lines and lines without a newline */
      else
      {
         if (*(eoln-1) == '\r')
	    eoln--;
      }
      *eoln = '\0';	/* get rid of the newline */

      re = ufdbNewPatternBuffer( line, REG_EXTENDED | REG_NOSUB );	/* URLs are folded to lowercase so we do not use REG_ICASE any more */
      if (re->error)
      {
	 ufdbLogError( "could not compile regular expression from %s: \"%s\"\n", file, line );
         retCode = UFDB_API_ERR_EXPR;
      }

      re->next = last;
      last = re;
   }

   if (UFDBglobalExpressionOptimisation)
   {
      *exprList = UFDBoptimizeExprList( file, re );
   }
   else
      *exprList = re;

   fclose( fin );

   return retCode;
}


/*
 * match a URL with a compiled RE.
 * return 0 if no match, 1 if there is a match.
 */
UFDB_GCC_HOT 
int ufdbRegExpMatch(   
   struct ufdbRegExp * regexp, 
   const char *        str )
{
   struct ufdbRegExp * rp;
   int                 error;
   int                 i;

   if (UFDBglobalDebugRegexp)
      ufdbLogMessage( "ufdbRegExpMatch \"%s\" %s", str, regexp==NULL ? "no REs" : "RE to test" );

   for (rp = regexp;  rp != NULL;  rp = rp->next)
   {
      if (UFDBglobalDebugRegexp)
	 ufdbLogMessage( "   ufdbRegExpMatch  %s  %s  error=%d", str, rp->pattern, rp->error );
      if (rp->error)
         continue;

#if defined(UFDB_API_NO_THREADS)
      i = 0;
#else
      pthread_mutex_lock( &(rp->lock) );
      i = rp->next_nregex_i;
      rp->next_nregex_i = (i + 1) % UFDB_NREGEX;
      pthread_mutex_unlock( &(rp->lock) );
#endif

      error = UFDBregexec( rp->compiled[i], str, 0, NULL, 0 );
      if (error == 0) 	/* match */
      {
	 if (UFDBglobalDebugRegexp)
	    ufdbLogMessage( "   RE match:  %s  %s", rp->pattern, str );
         return UFDB_API_MATCH;
      }
      if (error != REG_NOMATCH) 
      {
	 if (UFDBglobalDebugRegexp)
	    ufdbLogMessage( "   RE error %d:  %s  %s  error=%d", error, rp->pattern, str, error );
         return UFDB_API_ERR_EXPR;
      }
   }

   return 0;
}


void ufdbFreeRegExprList( struct ufdbRegExp * re )
{
   struct ufdbRegExp * tmp;
   int                 i;

   while (re != NULL)
   {
      tmp = re->next;
      if (!re->error)
      {
         if (UFDBglobalDebugRegexp)
            ufdbLogMessage( "ufdbFreeRegExprList: freeing regular expression %s", re->pattern );
	 for (i = 0;  i < UFDB_NREGEX;  i++)
	 {
	    UFDBregfree( re->compiled[i] );
	    ufdbFree( re->compiled[i] );
	 }
      }
      ufdbFree( re->pattern );
      ufdbFree( re->substitute );
      ufdbFree( re );
      re = tmp;
   }
}


void ufdbResetCPUs( void )
{
   cpu_mask = 0UL;
}


/*
 * Linux support binding threads to a set of CPUs which prevents cache contention.
 * Freebsd has no support, but the 5.x series is recommended for best multithreaded performance.
 * On Solaris it is recommended to start ufdbguardd in a processor set.
 */
int ufdbSetThreadCPUaffinity( int thread_num )
{
#if defined(__linux__) && defined(SYS_sched_setaffinity)
   int retval;

   if (cpu_mask != 0UL)
   {
      /* TODO: use sched_setaffinity() */
      retval = syscall( SYS_sched_setaffinity, 0, 4, &cpu_mask );
      if (retval < 0)
         return UFDB_API_ERR_ERRNO;
   }
#endif

   return UFDB_API_OK;
}


/*
 * Bind my processes and threads to a set of CPUs.
 * This increases the cache efficiency for all programs.
 */
int ufdbSetCPU( 
   char * CPUspec )		/* comma-separated CPU numbers (starting with CPU 0) */
{
   int    cpu;

#if defined(_SC_NPROCESSORS_ONLN)
   num_cpus = sysconf( _SC_NPROCESSORS_ONLN );
   /* printf( "this system has %ld CPUs\n", num_cpus ); */

#elif defined(__linux__) && defined(SYS_sched_getaffinity)
   /* sched_getaffinity() is buggy on linux 2.4.x so we use syscall() instead */
   cpu = syscall( SYS_sched_getaffinity, getpid(), 4, &cpu_mask );
   /* printf( "sched_getaffinity returned %d %08lx\n", cpu, cpu_mask ); */
   if (cpu >= 0)
   {
      num_cpus = 0;
      for (cpu = 0; cpu < UFDB_MAX_CPUS; cpu++)
	 if (cpu_mask & (1 << cpu))
	    num_cpus++;
      /* printf( "   found %d CPUs in the cpu mask\n", num_cpus ); */
   }
   else
#else
      num_cpus = UFDB_MAX_CPUS;
#endif

   cpu_mask = 0;
   while (*CPUspec)
   {
      if (sscanf(CPUspec,"%d",&cpu) == 1)
      {
	 if (cpu >= 0 && cpu < num_cpus)
	 {
	    cpu_mask |= (1 << cpu);
	 }
	 else
	    return UFDB_API_ERR_RANGE;
      }
      else
         return UFDB_API_ERR_RANGE;

      /* find the next CPU number */
      while (isdigit( (int) *CPUspec))
         CPUspec++;
      while (*CPUspec == ' '  ||  *CPUspec == ',')
         CPUspec++;
   }

   return UFDB_API_OK;
}


/*
 * A bitmap with IP addresses of clients is used to count the number of clients.
 * To keep the bitmap small (2 MB) the first octet of the IP address is ignored.
 *
 * This code assumes that there are 8 bits per byte.
 */

#define N_IPBITMAP   (256U * 256U * 256U)
#define IPBITMAPSIZE (N_IPBITMAP / 8)

static volatile unsigned char * IPbitmap = NULL;
static pthread_mutex_t IPbitmapMutex = PTHREAD_MUTEX_INITIALIZER;

/* We can receive from Squid an IP address (most common) or a FQDN.
 * In case that we receive a FQDN, we calculate a hash and use this 
 * as a psuedo IP address.
 */
void UFDBregisterCountedIP( const char * address )
{
   unsigned char * a;
   unsigned int    i, o1, o2, o3;
   unsigned int    nshift;

   if (IPbitmap == NULL)
      UFDBinitializeIPcounters();

   a = (unsigned char *) address;

   /* extremely simple way of looking if the parameter is an IPv4 address... */
   if (*a <= '9' && *a >= '0')
   {
      /* first skip the first octect */
      while (*a != '.'  && *a != '\0')
	 a++;
      if (*a == '.') a++;

      o1 = 0;
      while (*a >= '0' && *a <= '9' && *a != '\0')
      {
	 o1 = o1 * 10U + (*a - '0');
	 a++;
      }
      if (*a == '.') a++;

      o2 = 0;
      while (*a >= '0' && *a <= '9' && *a != '\0')
      {
	 o2 = o2 * 10U + (*a - '0');
	 a++;
      }
      if (*a == '.') a++;

      o3 = 0;
      while (*a >= '0' && *a <= '9' && *a != '\0')
      {
	 o3 = o3 * 10U + (*a - '0');
	 a++;
      }
      o1 = (o1 << 16) + (o2 << 8) + o3;

      /* if we got a non-existent IP address, we might go out of bounds... */
      if (o1 >= N_IPBITMAP)
      {
	 o1 = N_IPBITMAP;
      }
   }
   else 	/* no IPv4 a, calculate a hash */
   {
      o1 = 104729U;
      while (*a != '\0')
      {
         o1 = (o1 << 4) + o1 + (*a - ' ') - 53U;
	 a++;
      }
      o1 = o1 % (256U * 256U * 256U);
   }

   i = o1 / 8U;

#if 0
   nshift = o1 - i * 8U;
#endif
   nshift = o1 & 07;

   /* 
    * To be thread-safe we should use a semaphore here.
    * But since this counter is not critical and a lost bit-set 
    * will probably be covered by another call to this function,
    * we choose for superfast code and skip the use of a semaphore
    * or __sync_fetch_and_or().
    */
   IPbitmap[i] |= (1 << nshift);
}


void UFDBinitializeIPcounters( void )
{
   int i;
   unsigned char * b;

   if (IPbitmap == NULL)
   {
      pthread_mutex_lock( &IPbitmapMutex );
      if (IPbitmap == NULL)
         IPbitmap = ufdbMallocAligned( 64, IPBITMAPSIZE );
      pthread_mutex_unlock( &IPbitmapMutex );
   }

   b = (unsigned char *) IPbitmap;
   for (i = 0;  i < IPBITMAPSIZE;  i++)
      b[i] = 0;
   UFDBlastIPcounterResetDate = time( NULL );
}


unsigned long UFDBgetNumberOfRegisteredIPs( void )
{
   unsigned char * b;
   unsigned long   n;
   unsigned char   v;
   int             i;

   b = (unsigned char *) IPbitmap;
   if (b == NULL)
      return 0;
   n = 0;
   for (i = 0;  i < IPBITMAPSIZE;  i++)
   {
      v = b[i];
      while (v != 0)
      {
         if (v & 1)
	    n++;
	 v >>= 1;
      }
   }

   return n;
}


/*
 * Usernames are hashed and the a bitmap for hashvalues is administered
 * to keep track of the number of users.
 * To support up to 50,000 users, a bitmap that supports 500,000 users
 * is used to prevent collisions where two or more users are counted as one.
 *
 * This code assumes that there are 8 bits per byte.
 */

/* USERBITMAPSIZE is 64KB for 512*1024 hashvalues */
#define N_USERBITMAP	(512 * 1024)
#define USERBITMAPSIZE  (N_USERBITMAP / 8)

static volatile unsigned char * UserBitmap = NULL;
static pthread_mutex_t UserBitmapMutex = PTHREAD_MUTEX_INITIALIZER;

/* We can receive from Squid a username or '-'.
 * In case that we receive a username, we calculate a hashvalue and use this 
 * as an index in UserBitmap to set a bit to 1.
 */
void UFDBregisterCountedUser( const char * username )
{
   unsigned char * a;
   unsigned int    i, o1;
   unsigned int    nshift;

   if (UserBitmap == NULL)
      UFDBinitializeUserCounters();

   a = (unsigned char *) username;

   o1 = 104729U;
   while (*a != '\0')
   {
      o1 = (o1 << 4) + o1 + (*a - ' ') - 53U;
      a++;
   }
   o1 = o1 % N_USERBITMAP;

   i = o1 / 8U;

#if 0
   nshift = o1 - i * 8U;
#endif
   nshift = o1 & 07;

   /* 
    * To be thread-safe we should use a semaphore here.
    * But since this counter is not critical and a lost bit-set 
    * will probably be covered by another call to this function,
    * we choose for superfast code and skip the use of a semaphore
    * or __sync_fetch_and_or().
    */
   UserBitmap[i] |= (1 << nshift);
}


void UFDBinitializeUserCounters( void )
{
   int i;
   unsigned char * b;

   if (UserBitmap == NULL)
   {
      pthread_mutex_lock( &UserBitmapMutex );
      if (UserBitmap == NULL)
         UserBitmap = ufdbMallocAligned( 64, USERBITMAPSIZE );
      pthread_mutex_unlock( &UserBitmapMutex );
   }

   b = (unsigned char *) UserBitmap;
   for (i = 0;  i < USERBITMAPSIZE;  i++)
      b[i] = 0;
}


unsigned long UFDBgetNumberOfRegisteredUsers( void )
{
   unsigned char * b;
   unsigned long   n;
   unsigned char   v;
   int             i;

   b = (unsigned char *) UserBitmap;
   if (b == NULL)
      return 0;
   n = 0;
   for (i = 0;  i < USERBITMAPSIZE;  i++)
   {
      v = b[i];
      while (v != 0)
      {
         if (v & 1)
	    n++;
	 v >>= 1;
      }
   }

   return n;
}


void ufdbSetSignalHandler( int signal, void (*handler)(int) )
{
#if HAVE_SIGACTION
   struct sigaction act;

#ifndef SA_NODEFER
#define SA_NODEFER 0
#endif

#ifndef SA_NOCLDSTOP
#define SA_NOCLDSTOP 0
#endif

   act.sa_handler = handler;
   act.sa_flags = SA_RESTART;
   if (signal == SIGCHLD)
      act.sa_flags |= SA_NOCLDSTOP;
   if (signal == SIGALRM)
      act.sa_flags |= SA_NODEFER;
   sigemptyset( &act.sa_mask );
   sigaction( signal, &act, NULL );

#else

#if HAVE_SIGNAL
   signal( signal, handler );
#else
   ufdbLogError( "ufdbSetSignalHandler: cannot set handler for signal %d", signal );
#endif

#endif
}


static pid_t   ruid = -1;
static pid_t   euid;


void UFDBdropPrivileges( 
   const char *    username )
{
   struct passwd * fieldptrs;
#if HAVE_GETPWNAM_R
   struct passwd   pws;
   char            pws_fields[1024];
#endif

   if (ruid == -1)
   {
      ruid = getuid();
      euid = geteuid();

#ifdef UFDB_WORLD_READABLE_LOGFILES
      /* umask 133: no x for user, no wx for group, no wx for others */
      umask( S_IXUSR | S_IWGRP|S_IXGRP | S_IWOTH|S_IXOTH );
#else
      /* umask 137: no x for user, no wx for group, no rwx for others */
      umask( S_IXUSR | S_IWGRP|S_IXGRP | S_IROTH|S_IWOTH|S_IXOTH );
#endif
   }

   if (username == NULL  ||  username[0] == '\0')
      return;

#if HAVE_GETPWNAM_R
   if (getpwnam_r( username, &pws, pws_fields, sizeof(pws_fields)-1, &fieldptrs ) != 0)
#else
   fieldptrs = getpwnam( username );
   if (fieldptrs == NULL)
#endif
   {
      ufdbLogError( "Cannot get info on user '%s' so cannot change to this user  *****\n"
      		    "User '%s' probably does not exist.", 
		    username, username );
      return;
   }

   /* 
    * We have already written to the log file which may have been created as user root.
    * and need to change the ownership of the log file to be able to continue logging...
    */
   if (globalErrorLog != NULL)
   {
      if (fchown( fileno(globalErrorLog), fieldptrs->pw_uid, -1 ))
         ;
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "going to call seteuid(%d) and setegid(%d) to set euid to '%s'", 
                      fieldptrs->pw_uid, fieldptrs->pw_gid, username );

   if (setegid( fieldptrs->pw_gid ))
      ;

#if HAVE_SETRESUID  && 0
   if (setresuid( fieldptrs->pw_uid, fieldptrs->pw_uid, 0 ) != 0)
#endif

   if (seteuid( fieldptrs->pw_uid ) != 0)
   {
      ufdbLogError( "Cannot drop privileges and become user '%s': %s  *****", username, strerror(errno) );
      if (geteuid() != 0)
	 ufdbLogError( "I am not root so cannot drop/change privileges to user '%s'", username );
      return;
   }
   else
      ufdbLogMessage( "dropped privileges and became user '%s'", username );

#if 0 && defined(__linux__)
   if (UFDBglobalDebug || UFDBglobalDebugHttpd)
   {
      uid_t rid, eid, sid;
      (void) getresuid( &rid, &eid, &sid );
      ufdbLogMessage( "Privileges are dropped: now running as user '%s'  %d %d %d", username, rid, eid, sid );
      #define FF "/tmp/test.creat"
      unlink( FF );
      creat( FF, 0666 );
      ufdbLogMessage( "created test file %s", FF );
   }
#endif
}


void UFDBraisePrivileges( 
   const char *    username,
   const char *    why )
{
   if (username == NULL  ||  *username == '\0')
      return;

   ufdbLogMessage( "raising privileges to %s", why );

   if (seteuid( 0 ) != 0)	  
      ufdbLogError( "Cannot raise privileges *****" );

#if 0
   {
      uid_t rid, eid, sid;
      (void) getresuid( &rid, &eid, &sid );
      ufdbLogMessage( "Privileges are raised: now running with ids %d %d %d", rid, eid, sid );
      #define FF2 "/tmp/test.raised"
      unlink( FF2 );
      creat( FF2, 0666 );
      ufdbLogMessage( "created test file %s", FF );
   }
#endif
}


const char * ufdbAPIstatusString( int api_code )
{
   switch (api_code)
   {
   case UFDB_API_OK:                       return "OK";
   case UFDB_API_ERR_NULL:                 return "ERR_NULL";
   case UFDB_API_ERR_NOFILE:               return "ERR_NOFILE";
   case UFDB_API_ERR_READ:                 return "ERR_READ";
   case UFDB_API_ERR_EXPR:                 return "ERR_EXPR";
   case UFDB_API_ERR_RANGE:                return "ERR_RANGE";
   case UFDB_API_ERR_ERRNO:                return "ERR_ERRNO";
   case UFDB_API_ERR_SOCKET:               return "ERR_SOCKET";
   case UFDB_API_ERR_NOMEM:                return "ERR_NOMEM";
   case UFDB_API_REQ_QUEUED:               return "REQ_QUEUED";
   case UFDB_API_ERR_TUNNEL:               return "ERR_TUNNEL";
   case UFDB_API_ERR_INVALID_CERT:         return "ERR_INVALID_CERT";
   case UFDB_API_ERR_IP_ADDRESS:           return "ERR_IP_ADDRESS";
   case UFDB_API_ERR_OLD_TABLE:            return "ERR_OLD_TABLE";
   case UFDB_API_ERR_INVALID_TABLE:        return "ERR_INVALID_TABLE";
   case UFDB_API_ERR_INVALID_KEY:          return "ERR_INVALID_KEY";
   case UFDB_API_ERR_IS_SKYPE:             return "ERR_IS_SKYPE";
   case UFDB_API_ERR_FULL:                 return "ERR_FULL";
   case UFDB_API_ERR_UNKNOWN_PROTOCOL:     return "ERR_UNKNOWN_PROTOCOL";
   case UFDB_API_ERR_IS_GTALK:             return "ERR_IS_GTALK";
   case UFDB_API_ERR_IS_YAHOOMSG:          return "ERR_IS_YAHOOMSG";
   case UFDB_API_ERR_IS_AIM:               return "ERR_IS_AIM";
   case UFDB_API_ERR_IS_FBCHAT:            return "ERR_IS_FBCHAT";
   case UFDB_API_ERR_IS_CITRIXONLINE:      return "ERR_IS_CITRIXONLINE";
   case UFDB_API_ERR_IS_ANYDESK:           return "ERR_IS_ANYDESK";
   case UFDB_API_ERR_IS_TEAMVIEWER:        return "ERR_IS_TEAMVIEWER";
   case UFDB_API_ERR_CKSUM_NOT_VALID:      return "ERR_CKSUM_NOT_VALID";
   case UFDB_API_ERR_OUTDATED:             return "ERR_OUTDATED";
   case UFDB_API_ERR_FATAL:                return "ERR_FATAL";
   case UFDB_API_ERR_TLS:                  return "ERR_TLS";
   case UFDB_API_BEING_VERIFIED:           return "BEING_VERIFIED";
   case UFDB_API_MODIFIED_FOR_SAFESEARCH:  return "MODIFIED_FOR_SAFESEARCH";
   case -1:				   return "INTERNAL_ERROR_MINUS_ONE";
   }

   return "INTERNAL_ERROR_UNKNOWN_CODE";
}


const char * ufdbDBstat2string( int status )
{
   switch (status)
   {
      case UFDB_API_STATUS_DATABASE_OK:       return "up to date";
      case UFDB_API_STATUS_DATABASE_OLD:      return "one or more tables are more than 4 days old.  Check cron job for ufdbUpdate.";
      case UFDB_API_STATUS_DATABASE_EXPIRED:  return "one or more tables are EXPIRED.  Check licenses and cron job for ufdbUpdate.";
   }
   return "internal-error";
}


const char * ufdbStatus2string( int status )
{
   switch (status)
   {
      case UFDB_API_STATUS_VIRGIN:          return "virgin";
      case UFDB_API_STATUS_STARTED_OK:      return "started";
      case UFDB_API_STATUS_TERMINATED:      return "terminated";
      case UFDB_API_STATUS_RELOADING:       return "reloading";
      case UFDB_API_STATUS_RELOAD_OK:       return "reloaded";
      case UFDB_API_STATUS_FATAL_ERROR:     return "error";
      case UFDB_API_STATUS_ROLLING_LOGFILE: return "rolling-logfile";
      case UFDB_API_STATUS_UPDATE: 	    return "status-update";
      case UFDB_API_STATUS_CRASH_REPORT_UPLOADED:     	return "crash-report-uploaded";
      case UFDB_API_STATUS_CRASH_REPORT_NOT_UPLOADED:   return "upload-crash-reports-off-prohibits-upload";
   }
   return "internal-error-unknown-status";
}


void ufdbSendEmailToAdmin( int newStatus )
{
   int       mx;
   int       n;
   time_t    now_t;
   struct tm t;
   struct    timeval      tv;
   char      hostname[256+1];
   char      line[2048];

   if (UFDBglobalEmailServer == NULL  ||  UFDBglobalAdminEmail == NULL)
      return;

   if (UFDBglobalMyHostname != NULL)
      strcpy( hostname, UFDBglobalMyHostname );
   else
   {
      gethostname( hostname, sizeof(hostname) );
      hostname[256] = '\0';
   }

   mx = UFDBopenSocket( UFDBglobalEmailServer, 25 );
   if (mx < 0)
   {
      ufdbLogError( "cannot open connection to mail server '%s' to inform status change: %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      return;
   }

   /*
    *  Prevent that the read() takes ages.  Use an agressive timeout.
    */
   tv.tv_sec = 6;
   tv.tv_usec = 0;
   setsockopt( mx, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt( mx, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof(tv) );

   if (UFDBglobalDebug)
      ufdbLogMessage( "   mail: opened socket 25 to mail server '%s'", UFDBglobalEmailServer );

   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 4  ||  strncmp( line, "220", 3 ) != 0)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server %s did not send 220 welcome message: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   mail: read welcome lines from mail server '%s'", UFDBglobalEmailServer );

#if 0
   /* The welcome message may be long and sent in two chunks...
    * There is a 2 second timeout so no worries if there is no more input.
    */
   tv.tv_sec = 2;
   tv.tv_usec = 0;
   setsockopt( mx, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
#endif

   /*
    * From now on we wait maximum 20 seconds for responses of the mail server
    */
   tv.tv_sec = 20;
   tv.tv_usec = 0;
   setsockopt( mx, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof(tv) );
   
   sprintf( line, "HELO %s\r\n", hostname );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send HELO to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   do {
      n = read( mx, line, 2047 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 1)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply to HELO: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }

   if (UFDBglobalDebug > 1)
   {
      line[n] = '\0';
      ufdbLogMessage( "   mail: done with HELO handshake with mail server '%s'\nreply is %s", UFDBglobalEmailServer, line );
   }
   
   sprintf( line, "MAIL FROM:<%s>\r\n", UFDBglobalSenderEmail != NULL ? UFDBglobalSenderEmail : UFDBglobalAdminEmail );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send MAIL FROM to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 1)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply to MAIL FROM: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   if (strncmp( line, "25", 2 ) != 0)
   {
      char * newline;

      newline = strchr( line, '\r' );
      if (newline == NULL)
	 newline = strchr( line, '\n' );
      if (newline != NULL)
         *newline ='\0';
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply with OK to MAIL FROM:\n%s", 
      		    UFDBglobalEmailServer, line );
      close( mx );
      return;
   }
   
   sprintf( line, "RCPT TO:<%s>\r\n", UFDBglobalAdminEmail );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send RCPT TO to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 1)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply to RCPT TO: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   if (strncmp( line, "25", 2 ) != 0)
   {
      char * newline;

      newline = strchr( line, '\r' );
      if (newline == NULL)
	 newline = strchr( line, '\n' );
      if (newline != NULL)
         *newline ='\0';
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply with OK to RCPT TO:\n%s", 
      		    UFDBglobalEmailServer, line );
      close( mx );
      return;
   }
   
   /* To support mail servers with plain authentication:
    *
    * According to RFC 2595 the client must send: [authorize-id] \0 authenticate-id \0 password. 
    * pwcheck_method has been set to sasldb for the following example.
    * 
    * >>> AUTH PLAIN dGVzdAB0ZXN0QHdpei5leGFtcGxlLmNvbQB0RXN0NDI=
    * 235 2.0.0 OK Authenticated
    * Decoded:
    * test\000test@wiz.example.com\000tEst42
    * With patch for lib/checkpw.c or a pwcheck_method that doesn't support realms:
    * >>> AUTH PLAIN dGVzdAB0ZXN0AHRFc3Q0Mg==
    * Decoded:
    * test\000test\000tEst42
    */

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   mail: sending DATA to mail server '%s'", UFDBglobalEmailServer );
   
   sprintf( line, "DATA\r\n" );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send DATA to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 1)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply to DATA: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   if (strncmp( line, "354", 3 ) != 0)
   {
      char * newline;

      newline = strchr( line, '\r' );
      if (newline == NULL)
	 newline = strchr( line, '\n' );
      if (newline != NULL)
         *newline ='\0';
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply with OK to DATA:\n%s", 
      		    UFDBglobalEmailServer, line );
      close( mx );
      return;
   }

   now_t = time( NULL );
   gmtime_r( &now_t, &t );

   sprintf( line, "From: ufdbGuard daemon <%s>\r\n"
		  "To: URL filter administrator <%s>\r\n"
                  "Subject: ufdbGuard on %s has a new status: %s\r\n"
		  "Date: %3.3s, %02d %3.3s %4d %02d:%02d:%02d GMT\r\n"
		  "X-Mailer: ufdbguardd " UFDB_VERSION
		  "\r\n"
		  "ufdbGuard with pid %d on %s has a new status: %s\n"
		  "database status: %s\n"
		  "license status: %s\n"
		  "configuration file: %s\n"
		  "version: " UFDB_VERSION "\n"
		  "\r\n.\r\n",
		  UFDBglobalSenderEmail != NULL ? UFDBglobalSenderEmail : UFDBglobalAdminEmail,
		  UFDBglobalAdminEmail,
		  hostname, ufdbStatus2string(newStatus),
		     &"SunMonTueWedThuFriSat"[t.tm_wday*3],
		     t.tm_mday,
		     &"JanFebMarAprMayJunJulAugSepOctNovDec"[t.tm_mon*3],
		     t.tm_year + 1900,
		     t.tm_hour, t.tm_min, t.tm_sec,
		  globalPid, hostname, ufdbStatus2string(newStatus),
		  ufdbDBstat2string(UFDBglobalDatabaseStatus),
		  UFDBglobalLicenseStatus,
		  UFDBglobalConfigFile
	  );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send CONTENT to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   do {
      n = read( mx, line, 2048 );
   } while (n < 0  &&  errno == EINTR);
   if (n < 1)
   {
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply to DATA END: %s", 
      		    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
   if (strncmp( line, "25", 2 ) != 0)
   {
      char * newline;

      newline = strchr( line, '\r' );
      if (newline == NULL)
	 newline = strchr( line, '\n' );
      if (newline != NULL)
         *newline ='\0';
      ufdbLogError( "ufdbSendEmailToAdmin: mail server '%s' did not send reply with OK to DATA END:\n%s", 
      		    UFDBglobalEmailServer, line );
      close( mx );
      return;
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   mail: sending QUIT to %s", UFDBglobalEmailServer );
   
   sprintf( line, "QUIT\r\n" );
   n = write( mx, line, strlen(line) );
   if (n != (int) strlen(line))
   {
      ufdbLogError( "ufdbSendEmailToAdmin: failed to send QUIT to mail server '%s': %s", 
                    UFDBglobalEmailServer, strerror(errno) );
      close( mx );
      return;
   }
#if 0
   n = read( mx, line, 2048 );
   /* ignore errors */
#endif
   close( mx );

   ufdbLogMessage( "sent email with status update to '%s' using mail server '%s'", UFDBglobalAdminEmail, UFDBglobalEmailServer );
}


static void ufdbExecuteExternalCommand( int status )
{
   pid_t pid;

   if (UFDBglobalExternalStatusCommand == NULL)
      return;

   ufdbLogMessage( "going to execute: %s -s %s -d %s -l %s -v %s",
                   UFDBglobalExternalStatusCommand, 
		   ufdbStatus2string(status), 
		   ufdbDBstat2string(UFDBglobalDatabaseStatus),
		   UFDBglobalLicenseStatus,
		   UFDB_VERSION  );

   pid = fork();
   if (pid == 0)	/* we are the forked child */
   {
      int    i;
      const char * argv[12];

      for (i = 0; i < 2*UFDB_MAX_THREADS + 32;  i++)
         close( i );
      i = 0;
      argv[i++] = UFDBglobalExternalStatusCommand;
      argv[i++] = "-s";
      argv[i++] = ufdbStatus2string( status );
      argv[i++] = "-d";
      argv[i++] = ufdbDBstat2string( UFDBglobalDatabaseStatus );
      argv[i++] = "-l";
      argv[i++] = UFDBglobalLicenseStatus;
      argv[i++] = "-v";
      argv[i++] = UFDB_VERSION;
      argv[i] = NULL;
      execv( UFDBglobalExternalStatusCommand, (char* const*) argv );
      _exit( 2 );       /* exit after failed execve */
   }
   else
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "   %s has pid %ld", UFDBglobalExternalStatusCommand, (long) pid );
   }

   if (pid < 0)
   {
      ufdbLogError( "fork failed: cannot execute external status command '%s': %s", 
                    UFDBglobalExternalStatusCommand, strerror(errno) );
      return;
   }

   /* we could do a waitpid() here but there is an other thread that does that */
}


int UFDBchangeStatus( int status )
{
   int    oldStatus;
   char * dbhome;
   FILE * fp;
   char   licStatFileName[1024];

   if (status == UFDBglobalStatus)
      return status;

   ufdbLogMessage( "Changing daemon status to \"%s\"", ufdbStatus2string(status) );

   /* prevent changing the ERROR state into an OK state */
   if (UFDBglobalStatus == UFDB_API_STATUS_FATAL_ERROR)
   {
      if (status == UFDB_API_STATUS_RELOAD_OK)
         return UFDB_API_STATUS_FATAL_ERROR;
   }

   /* update the license status */
   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;

   strcpy( licStatFileName, dbhome );
   strcat( licStatFileName, "/" );
   strcat( licStatFileName, UFDB_LICENSE_STATUS_FILENAME );
   fp = fopen( licStatFileName, "r" );
   if (fp != NULL)
   {
      char * newline;
      if (fgets( UFDBglobalLicenseStatus, sizeof(UFDBglobalLicenseStatus)-1, fp ))
         ;
      fclose( fp );
      if ((newline = strchr( UFDBglobalLicenseStatus, '\n' )) != NULL)
         *newline = '\0';
      if (UFDBglobalLicenseStatus[0] == '\0')
	 strcpy( UFDBglobalLicenseStatus, "unknown" );
   }
   else
      strcpy( UFDBglobalLicenseStatus, "unknown" );

   ufdbSendEmailToAdmin( status );
   ufdbExecuteExternalCommand( status );

   oldStatus = UFDBglobalStatus;
   UFDBglobalStatus = status;

   if (UFDBglobalDebug)
      ufdbLogMessage( "UFDBchangeStatus done" );
   
   return oldStatus;
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


static void interruptPstack( int dummy )
{
   ufdbLogError( "waited too long for output of pstack.... aborting now!" );
   UFDBchangeStatus( UFDB_API_STATUS_FATAL_ERROR );
   _exit( 5 );  /* pstack took too long; exit now */
}


void ufdbExecutePstack( const char * reason )
{
   FILE * fp;
   char   cmd[512];
   char   buffer[2048];

#if 0
   /* popen causes SIGCHLD signals but we want to ignore them */
   ufdbSetSignalHandler( SIGCHLD, SIG_IGN );
#endif

   ufdbSetSignalHandler( SIGALRM, interruptPstack );
   alarm( 15 );

   if (UFDBglobalUserName[0] != '\0')
      sprintf( cmd, DEFAULT_BINDIR "/ufdb-pstack %d -U %s %s 2>&1", globalPid, UFDBglobalUserName, reason );
   else
      sprintf( cmd, DEFAULT_BINDIR "/ufdb-pstack %d %s 2>&1", globalPid, reason );

   /* gdb sometimes needs root permissions, so the popen() is done as root */
   UFDBraisePrivileges( UFDBglobalUserName, "open pipe for ufdb-pstack" );
   errno = 0;
   fp = popen( cmd, "r" );
   if (fp == NULL)
      ufdbLogError( "could not open pipe and execute \"%s\": %s", cmd, strerror(errno) );
   UFDBdropPrivileges( UFDBglobalUserName );

   while (my_fast_fgets_nonl( buffer, 2040, fp ) != NULL)
   {
      ufdbLogMessage( "pstack  %s", buffer );
   }
   pclose( fp );
   ufdbLogMessage( "pstack  END" );
   alarm( 0 );
}


#ifndef ufdbStrncpy

UFDB_GCC_HOT void ufdbStrncpy( char * dest, const char * src, size_t n )
{
   if (memccpy( dest, src, '\0', n ) == NULL)
      dest[n-1] = '\0';
}

#endif

