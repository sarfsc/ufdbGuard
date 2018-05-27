/*
 * ufdbdb.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdbdb.c,v 1.58 2017/05/03 00:23:28 root Exp root $
 */

/* This module is well tested and stable for a long time.
 * For maximum performance _FORTIFY_SOURCE is undefined.
 */
#undef _FORTIFY_SOURCE

/* to inline string functions with gcc : */
#if defined(__OPTIMIZE__) && defined(__GNUC__)  &&  defined(GCC_INLINE_STRING_FUNCTIONS_ARE_FASTER)
#define __USE_STRING_INLINES  1
#endif


#ifndef UFDB_GEN_API
#include "sg.h"
#endif

#include "ufdb.h"
#include "ufdblib.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <syslog.h>
/* TO-DO: evaluate use of syslog() */
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
#include <arpa/inet.h>
#include <fcntl.h>

#include "bzlib.h"
#include "zlib.h"

#ifdef __GNUC__
#define GCC_PREFETCH( mem, rw, locality )  __builtin_prefetch( mem, rw, locality )
#else
#define GCC_PREFETCH( mem, rw, locality )  /* void */
#endif

/* #define  UFDB_DEBUG_IPV6 */

#define UFDB_DO_DEBUG 0
#define UFDB_DEBUG_REVURL 0

#if UFDB_DO_DEBUG || 0
#define DEBUG(x) ufdbLogMessage x 
#else
#define DEBUG(x) 
#endif

static unsigned char *    tableMemStart;
static unsigned char *    memStart;

static struct UFDBtable * tableIndex;
static int                tableIndex_i;


UFDBthreadAdmin * UFDBallocThreadAdmin( void )
{
   UFDBthreadAdmin * admin;

   admin = (UFDBthreadAdmin *) ufdbMallocAligned( 64, sizeof(UFDBthreadAdmin) );
   memset( admin, 0, sizeof(UFDBthreadAdmin) );

#if defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4  &&  __SIZEOF_LONG__ == 4
   (void) __sync_add_and_fetch( &UFDB_API_num_threads, 1 );
#elif defined(__GNUC__)  &&  __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8  &&  __SIZEOF_LONG__ == 8
   (void) __sync_add_and_fetch( &UFDB_API_num_threads, 1 );
#else
   static pthread_mutex_t incrMutex = PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &incrMutex );
   UFDB_API_num_threads++;
   pthread_mutex_unlock( &incrMutex );
#endif

   return admin;
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static char * findDomainEnd( char * url )
{
   /* works also URLs with IPv6: [a:b::0]/foo.html */
   return url + strcspn( url, "/?&;#" );
}


/* mystrchr() is faster than the standard strchr() since strings are generally short */
UFDB_GCC_HOT UFDB_GCC_INLINE
static char * mystrchr( char * string, char letter )
{
   while (*string != '\0')
   {
      if (*string == letter)
         return string;
      string++;
   }
   return NULL;
}


/* Mutex locks for _allocRevURL() are a thread synchronisation bottleneck */
/* so it is better that each thread has its own array of UFDBrevURL* */

UFDB_GCC_INLINE UFDB_GCC_HOT
static UFDBrevURL * _allocRevURL( 
   UFDBthreadAdmin * admin )
{
   int               i;
   UFDBrevURL *      new;
   
   for (i = 0; i < MAX_REVURLS; i++)
   {
      if (admin->myArrayUsage[i] == 0)
      {
         admin->myArrayUsage[i] = 1;
         return &(admin->myArray[i]);
      }
   }

   new = (UFDBrevURL *) ufdbMalloc( sizeof(UFDBrevURL) );

   return new;
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static void _freeRevURL( 
   UFDBthreadAdmin * admin, 
   UFDBrevURL *      old )
{
   int               i;

   for (i = 0; i < MAX_REVURLS; i++)
   {
      if (old == &(admin->myArray[i]))
      {
         admin->myArrayUsage[i] = 0;
         return;
      }
   }

   ufdbFree( old );
}


UFDB_GCC_HOT
static UFDBrevURL * parseIPv6URL( 
   UFDBthreadAdmin * admin,
   unsigned char *   URL )
{
   unsigned char *   oldBracket;
   unsigned int      size;
   UFDBrevURL *      newHead;
   UFDBrevURL *      newPath;

#ifdef UFDB_DEBUG_IPV6
   ufdbLogMessage( " parseIPv6URL: %s", URL );
#endif

   oldBracket = (unsigned char *) mystrchr( (char *) URL, ']' );
   if (oldBracket == NULL)
   {
      oldBracket = URL;
      while (*oldBracket != '\0'  &&  *(oldBracket+1) != '/')
	 oldBracket++;
      if (oldBracket - URL > (int) sizeof(UFDBurlPart)-2)
         oldBracket = URL + sizeof(UFDBurlPart) - 2;
   }
#if 0
   else
   {
      char normalisedDomain[64];

      *oldBracket = '\0';
      if (UFDBparseIPv6address( URL+1, normalisedDomain ) == NULL)
         *oldBracket = ']';
      else
      {
	 *oldBracket = ']';
	 UFDBnormaliseIPv6Domain( URL, normalisedDomain );
      }
   }
#endif

   newHead = _allocRevURL( admin );
   size = (unsigned char *) oldBracket - URL + 1;
   if (size > (int) sizeof(UFDBurlPart)-2)
      size = sizeof(UFDBurlPart) - 2;
   memccpy( newHead->part, URL, ']', size );
   newHead->part[ size ] = '\0';

   if (*(oldBracket+1) == '/')
   {
      newPath = _allocRevURL( admin );
      newHead->next = newPath;
      memccpy( newPath->part, oldBracket+1, '\0', sizeof(UFDBurlPart)-2 );
      newPath->part[ sizeof(UFDBurlPart)-2 ] = '\0';
      newPath->next = NULL;
   }
   else
      newHead->next = NULL;
   
   return newHead;
}


UFDB_GCC_HOT
static UFDBrevURL * parseFQDNandPath( 
   UFDBthreadAdmin * admin,
   char *            domain,
   char *            path,
   int               URL4table  )
{
   char *            d;
   char *            orig_domain;
   unsigned char *   newpart;
   UFDBrevURL *      head;
   UFDBrevURL *      prev;
   UFDBrevURL *      tail;

#if UFDB_DEBUG_REVURL
   DEBUG(( "   parseFQDNandPath  domain '%s'  path '%s'  URL4table %d", domain, path, URL4table ));
#endif

   prev = NULL;
   tail = NULL;				/* the hand-optimised unrolled loop is slightly faster */
   orig_domain = domain;

   while (1) 
   {
      head = _allocRevURL( admin );
      head->next = prev;
      if (tail == NULL)
         tail = head;

      d = (char *) head->part;
      while (1)						/* unrolled 7 times */
      {
	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

	 if (*domain == '.'  ||  *domain == '\0')
	    break;
	 *d++ = *domain++;

         /* test for error: someone uses a label/path that is way too long */
	 if (d > (char *) &head->part[sizeof(UFDBurlPart) - 7 - 4])	
	 {
	    while (*domain != '.'  &&  *domain != '\0')
	       domain++;
	    break;						   /* do not report an error; just truncate */
	 }
      }
      *d = '\0';

      if (*domain == '\0')
      {
	 if (*path != '\0')
	 {
	    int    n;
	    int    param_len;
	    int    value_len;
	    char * param_end;
	    char * orig_path;

	    tail->next = _allocRevURL( admin );
	    tail = tail->next;
	    tail->next = NULL;
	    orig_path = path;
	    newpart = tail->part;
	    *newpart = '\0';
	    while (*path != '\0')
	    {
	       if (*path == '?')
	       {
                  /* if no '/' in www.example.com?foo=bar, insert a '/'  */
		  if (path == orig_path)				
		  {
		     *newpart++ = '/';
		  }
		  *newpart = '\0';

                  /* allocate new part for the parameters */
		  tail->next = _allocRevURL( admin );		
		  tail = tail->next;
		  tail->next = NULL;
		  newpart = tail->part;
		  *newpart = '\0';
		  n = 0;

		  path++;                               /* skip '?' */
		  if (URL4table)
		  {
                     char * nextqm;
                     if ((nextqm = strchr( path, '?' )) != NULL)
                     {
                        ufdbLogError( "URL '%s%s' has two or more '?'  (truncating second)",
                                      orig_domain, orig_path );
                        *nextqm = '\0';
                     }
		     while (*path != '\0')
		     {
			param_end = path;
			while (*param_end != '\0'  &&  *param_end != '='  &&  *param_end != '&')
			   param_end++;
			if (*param_end == '\0'  ||  *param_end == '&')
			{
			   if (UFDBglobalDebug)
			      ufdbLogMessage( "URL '%s%s' parameter '%s' has no value", 
                                              orig_domain, orig_path, path );
			}
			param_len = param_end - path;
                        /* if the parameter doesn't exist, skip this part of the URL */
                        if (param_len == 0)
                        {
			   ufdbLogMessage( "URL '%s%s' :\n"
                                           "parameter list has a part without parameters (& at begin or end, <void>=value, or &&)", 
                                           orig_domain, orig_path );
                           path++;
                           continue;
                        }
                        /* if the parameter name is too large, skip it */
			if (param_len >= (int) sizeof(UFDBurlPart))	
			{
			   ufdbLogMessage( "URL '%s%s' :\n"
                                           "skipping too long parameter name '%s'", 
                                           orig_domain, orig_path, path );
			   path = param_end;
			   while (*path != '\0'  &&  *path != '&')
			      path++;
			   continue;
			}

			while (path < param_end)			/* copy parameter ID to new part */
			   *newpart++ = *path++;
			*newpart = '\0';
#if UFDB_DEBUG_REVURL
			ufdbLogMessage( "new part for parameter ID '%s'", tail->part );
#endif

			if (*param_end == '='  ||  *param_end == '&'  || *param_end == '\0')
			{
			   tail->next = _allocRevURL( admin );		/* allocate new part for the value */
			   tail = tail->next;
			   tail->next = NULL;
			   newpart = tail->part;
			   *newpart = '\0';

                           if (*param_end == '=')
                              path++;					/* skip '=' */
                              
                           param_end = path;
                           while (*param_end != '\0'  &&  *param_end != '&')
                              param_end++;
                           value_len = (int) (param_end - path);
			   if (value_len >= (int) sizeof(UFDBurlPart))	/* value is too large; truncate it */
			   {
			      ufdbLogMessage( "URL '%s%s' truncating too long value '%s'"
                                              " and skipping other parameters",
                                              orig_domain, orig_path, path );
			      param_end = path + sizeof(UFDBurlPart) - 1;
			      *param_end = '\0';
			   }

			   while (path < param_end)			/* copy value to new part */
			      *newpart++ = *path++;
			   *newpart = '\0';
#if UFDB_DEBUG_REVURL
			   ufdbLogMessage( "new part for value '%s'  path '%s'", 
                                           tail->part, path );
#endif

			   if (*path != '\0')
			   {
			      path++;                                   /* skip '&' */
                              if (*path != '\0')
                              {
                                 tail->next = _allocRevURL( admin );	/* allocate the next parameter */
                                 tail = tail->next;
                                 tail->next = NULL;
                                 newpart = tail->part;
                                 *newpart = '\0';
                              }
			   }
			}
		     }
		     return head;
		  }
		  else							/* !URL4table */
		  {
		     while (*path != '\0')
		     {
			param_end = path;
			while (*param_end != '\0'  &&  *param_end != '&')
			   param_end++;
			param_len = (int) (param_end - path);
                        /* if the parameter doesn't exist, skip this part of the URL */
                        if (param_len == 0)
                        {
#if UFDB_DEBUG_REVURL
			   ufdbLogMessage( "URL '%s%s' :\n"
                                           "parameter list has a part without parameters (& at beginning or end or &&)", 
                                           orig_domain, orig_path );
#endif
                           path++;
                           continue;
                        }
			if (param_len >= (int) sizeof(UFDBurlPart))	/* the parameter+value is too large */
			{
#if UFDB_DEBUG_REVURL
			   ufdbLogMessage( "parseFQDNandPath: skipped parameter which is too long '%s'", path );
#endif
			   path = param_end;
			   if (*path != '\0')
			      path++;
			   continue;
			}
			else if (n + param_len >= sizeof(UFDBurlPart))	/* must allocate new RevUrl part */
			{
#if UFDB_DEBUG_REVURL
			   ufdbLogMessage( "parseFQDNandPath: n=%d  strlen=%d  this part of URL: '%s'", 
                                           n, (int) strlen((char*)tail->part), tail->part );
			   ufdbLogMessage( "parseFQDNandPath: allocated new part for remaining part '%s'",
                                           path );
#endif
			   *newpart = '\0';
			   tail->next = _allocRevURL( admin );
			   tail = tail->next;
			   tail->next = NULL;
			   newpart = tail->part;
			   *newpart = '\0';
			   n = 0;
			}
                                                   /* append the parameter+value to the current RevUrl part */
                        if (n > 0)
                           *newpart++ = '&';
			n += param_len + 1;
			while (path < param_end)			/* TO-DO: unroll */
			   *newpart++ = *path++;
                        if (*path == '&')
                           path++;
                        *newpart = '\0';
#if UFDB_DEBUG_REVURL
			ufdbLogMessage( "parseFQDNandPath: n=%d  copied one parameter to '%s'",
                                        n, tail->part );
#endif
		     }
		  }
	       }
	       else     /* not yet seen a '?' */
	       {
		  if (path == orig_path + sizeof(UFDBurlPart) - 1)
		     break;
		  *newpart++ = *path++;
	       }
	    }
	    *newpart = '\0';
	 }
         return head;
      }

      domain++;
      prev = head;
   }

   return head;		/* NOTREACHED */
}


/* Parse the URL and reverse it into a linked list 
 * e.g. my.sex.com becomes "com" -> "sex" -> "my" -> NULL
 *      youtube.com/watch?v=ahuJBU7hji2K  becomes "com" -> "youtube" -> "/watch" -> "v=ahuJBU7hji2K"
 *
 * The input parameter, URL, must be a writable array of characters (size = UFDB_MAX_URL_LENGTH).
 */
UFDB_GCC_HOT 
UFDBrevURL * UFDBgenRevURL( 
   UFDBthreadAdmin * admin,
   unsigned char *   URL )
{
   char *            d;
   char              domain[UFDB_MAX_URL_LENGTH];

   if (URL[0] == '[')
      return parseIPv6URL( admin, URL );

   d = findDomainEnd( (char *) URL );

   if (d == (char *) URL)				/* domain is empty: illegal URL */
   {
      strcpy( domain, "nodomain" );
   }
   else
   {
      int dlen = d - (char *) URL;

      /* copy the domainname (the dots will be replaced by \0) and find the start of the path */
      strncpy( domain, (char *) URL, dlen );
      domain[dlen] = '\0';
      URL += dlen;

      if (domain[dlen-1] == '.')		/* remove dot after FQDN: also match www.example.com. */
	 domain[dlen-1] = '\0';
   }

   return parseFQDNandPath( admin, domain, (char *) URL, 0 );
}


/* Parse the URL and reverse it into a linked list suitable to be inserted in a table (used by ufdbGenTable)
 * e.g. my.sex.com becomes "com" -> "sex" -> "my" -> NULL
 *      youtube.com/watch?v=ahuJBU7hji2K  becomes "com" -> "youtube" -> "/watch" -> "v" -> "ahuJBU7hji2K"
 *      foo.net/cgi?a=1&b=2&c=3 becomes "net" -> "foo" -> "/cgi" -> "a" -> 1 -> "b" -> 2 -> "c" -> 3
 *
 * The input parameter, URL, must be a writable array of characters (size = UFDB_MAX_URL_LENGTH).
 */
UFDB_GCC_HOT 
UFDBrevURL * UFDBgenRevURL4table(
   UFDBthreadAdmin * admin,
   unsigned char *   URL )
{
   char *            d;
   char              domain[UFDB_MAX_URL_LENGTH];

   if (URL[0] == '[')
      return parseIPv6URL( admin, URL );

   /* copy the domainname (the dots will be replaced by \0) and find the start of the path */
   d = findDomainEnd( (char *) URL );

   if (d == (char *) URL)				/* domain is empty: illegal URL */
   {
      strcpy( domain, "empty-domain" );
   }
   else
   {
      int dlen = d - (char *) URL;

      strncpy( domain, (char *) URL, dlen );
      domain[dlen] = '\0';
      URL += dlen;

      if (domain[dlen-1] == '.')		/* remove dot after FQDN: also match www.example.com. */
	 domain[dlen-1] = '\0';
   }

   return parseFQDNandPath( admin, domain, (char *) URL, 1 );
}


/* DEBUG: log a revURL 
 */
void UFDBprintRevURL( UFDBrevURL * revURL )
{
   char buffer[UFDB_MAX_URL_LENGTH];

   buffer[0] = '\0';
   while (revURL != NULL)
   {
      strcat( buffer, (char *) revURL->part );
      if (revURL->next != NULL)
	 strcat( buffer, " . " );
      revURL = revURL->next;
   }
   ufdbLogMessage( "P  %s", buffer );
}


UFDB_GCC_HOT
void UFDBfreeRevURL( 
   UFDBthreadAdmin * admin,
   UFDBrevURL *      revURL )
{
   UFDBrevURL *      next;

   while (revURL != NULL)
   {
      next = revURL->next;
      _freeRevURL( admin, revURL );
      revURL = next;
   }
}


#if UFDB_DO_DEBUG
static void printMem( unsigned char * mem )
{
   unsigned char * m = tableMemStart;
   int i;

   ufdbLogMessage( "parsing error at %lu 0%07lo  memStart 0x%012lx  mem 0x%012lx", 
                   (unsigned long) (mem - m), (unsigned long) (mem - m), (unsigned long) m, (unsigned long) mem );
   for (i = 0; i < 16; i++)
   {
      ufdbLogMessage( "   %3d  %03o  0x%02x  %c", *mem, *mem, *mem, *mem );
      mem++;
   }
}
#endif


#define ROUNDUPBY      4
#define ROUNDUP(i)     ( (i) + (ROUNDUPBY - ((i)%ROUNDUPBY) ) )

#define BIGROUNDUPBY   (4 * ROUNDUPBY)
#define BIGROUNDUP(i)  ( (i) + (BIGROUNDUPBY - ((i)%BIGROUNDUPBY) ) )

UFDB_GCC_INLINE 
static struct UFDBtable * _reallocTableArray_1_2( 
   struct UFDBtable *  ptr,
   int                 nElem )
{
   if (nElem <= ROUNDUPBY)
   {
      ptr = (struct UFDBtable *) ufdbRealloc( ptr, (size_t) nElem * sizeof(struct UFDBtable) );
   }
   else if (nElem < 4*BIGROUNDUPBY)
   {
      if (nElem % ROUNDUPBY == 1)
	 ptr = (struct UFDBtable *) ufdbRealloc( ptr, (size_t) ROUNDUP(nElem) * sizeof(struct UFDBtable) );
   }
   else
   {
      if (nElem % BIGROUNDUPBY == 1)
	 ptr = (struct UFDBtable *) ufdbRealloc( ptr, (size_t) BIGROUNDUP(nElem) * sizeof(struct UFDBtable) );
   }

   return ptr;
}


void UFDBfreeTableIndex_1_2( struct UFDBtable * t )
{
   int i;

   if (t == NULL)
      return;

   for (i = 0; i < t->nNextLevels; i++)
   {
      UFDBfreeTableIndex_1_2( &(t->nextLevel[i]) );
   }

   ufdbFree( &(t->nextLevel[0]) );
}


int ufdbDecompressBZIP2Table( struct UFDBmemTable * memTable )
{
   char * new_mem;
   int    rv;

   if (UFDBglobalDebug)
      ufdbLogMessage( "   decompressing BZIP2 table from %ld original size of %d", 
                      memTable->fileSize - memTable->hdrSize, memTable->numEntries );

   /* add 32-byte safeguard for SSE and AVX2 instructions */
   new_mem = (char *) ufdbMalloc( memTable->hdrSize + memTable->numEntries + 32 );
   if (new_mem == NULL)
   {
      ufdbLogFatalError( "ufdbDecompressBZIP2Table: cannot allocate %ld bytes memory for BZIP2 table decompression", 
                         (long) (memTable->hdrSize + memTable->numEntries + 32) );
      return UFDB_API_ERR_NOMEM;
   }

   /* copy the table header */
   memcpy( new_mem, memTable->mem, memTable->hdrSize );

   rv = BZ2_bzBuffToBuffDecompress( new_mem + memTable->hdrSize,
                                    (unsigned int *) &(memTable->numEntries),
			            (char *) memTable->mem + memTable->hdrSize,
			            memTable->fileSize - memTable->hdrSize,
			            0,
			            0 );
   if (rv != BZ_OK)
   {
      ufdbLogFatalError( "ufdbDecompressBZIP2Table: BZIP2 decompression failed with code %d", rv );
      return UFDB_API_ERR_NOMEM;
   }

   ufdbFree( memTable->mem );
   memTable->mem = new_mem;

   return UFDB_API_OK;
}


int ufdbDecompressZLIBTable( struct UFDBmemTable * memTable )
{
   char * new_mem;
   int    rv;
   z_stream  zs;

   if (UFDBglobalDebug)
      ufdbLogMessage( "   decompressing ZLIB table from %ld original size of %d",
                      memTable->fileSize - memTable->hdrSize, memTable->numEntries );

   /* add 32-byte safeguard for SSE and AVX2 instructions */
   new_mem = (char *) ufdbMalloc( memTable->hdrSize + memTable->numEntries + 32 );
   if (new_mem == NULL)
   {
      ufdbLogFatalError( "ufdbDecompressZLIBTable: cannot allocate %ld bytes memory for ZLIB table decompression", 
                         (long) (memTable->hdrSize + memTable->numEntries + 32) );
      return UFDB_API_ERR_NOMEM;
   }

   zs.opaque = Z_NULL;
   zs.zalloc = Z_NULL;
   zs.zfree  = Z_NULL;
   zs.next_in = Z_NULL;
   zs.avail_in = Z_NULL;
   rv = inflateInit( &zs );
   if (Z_OK != rv)
   {
      ufdbLogFatalError( "ufdbDecompressZLIBTable: ZLIB decompression initialisation failed: error %d  %s",
                         rv, zs.msg == NULL ? "" : zs.msg );
      return UFDB_API_ERR_NOMEM;
   }

   /* copy the table header */
   memcpy( new_mem, memTable->mem, memTable->hdrSize );

   zs.next_in = (unsigned char *) memTable->mem + memTable->hdrSize;
   zs.avail_in = memTable->fileSize - memTable->hdrSize;
   zs.next_out = (unsigned char *) new_mem + memTable->hdrSize;
   zs.avail_out = memTable->numEntries + 0;
   rv = inflate( &zs, Z_FINISH );
   if (rv != Z_STREAM_END)
   {
      ufdbLogFatalError( "ufdbDecompressZLIBTable: ZLIB decompression failed: error %d  %s",
                         rv, zs.msg == NULL ? "" : zs.msg );
      return UFDB_API_ERR_RANGE;
   }

   inflateEnd( &zs );
   ufdbFree( memTable->mem );
   memTable->mem = new_mem;

   return UFDB_API_OK;
}


/* Parse a binary table header from a memory table.
 */
int UFDBparseTableHeader( struct UFDBmemTable * memTable )
{
   int  retval = UFDB_API_OK;
   int  rv;
   int  n;
   int  cksum;
   char prefix[32];
   char tableName[32];
   char key[32];
   char date[32];
   char flags[8+1];
   ufdbCrypt uc;
   unsigned char * mem;

   strcpy( date, "nodate" );
   strcpy( flags, "--------" );
   memTable->version[0] = '\0';
   memTable->numEntries = 0;
   memTable->indexSize = 0;
   cksum = -1;
   n = sscanf( (char *) memTable->mem, "%5s %7s %20s %11d key=%30s date=%20s %8s %d %d",
               prefix, &(memTable->version[0]), tableName, &(memTable->numEntries), 
	       key, date, flags, &(memTable->indexSize), &cksum );

   memcpy( memTable->flags, flags, 8 );
   if (flags[3] == 'p')
      memTable->hdrSize = sizeof(struct UFDBfileHeader21);
   else
      memTable->hdrSize = sizeof(struct UFDBfileHeader);

#if UFDB_DO_DEBUG
   if (UFDBglobalDebug)
      ufdbLogMessage( "      UFDBparseTableHeader: n=%d prefix=%-5.5s version=%s name=%s num=%d key=%s date=%s flags=%s indexsize=%d hdrsize=%d cksum=%05d", 
                      n, prefix, memTable->version, tableName, memTable->numEntries, key, date, flags, memTable->indexSize, memTable->hdrSize, cksum );
#endif

   if (n < 5  ||  strcmp(prefix,"UFDB") != 0)
   {
      syslog( LOG_ALERT, "URL table %s has an invalid UFDB header", tableName );
      ufdbLogFatalError( "URL table %s has an invalid UFDB header", tableName );
      ufdbLogFatalError( "contact support@urlfilterdb.com" );
      return UFDB_API_ERR_INVALID_TABLE;
   }

   if (memTable->indexSize < 0)
   {
      syslog( LOG_ALERT, "URL table %s has an erroneous UFDB header: indexsize < 0", tableName );
      ufdbLogFatalError( "URL table %s has an erroneous UFDB header: indexsize < 0", tableName );
      ufdbLogFatalError( "contact support@urlfilterdb.com" );
      return UFDB_API_ERR_INVALID_TABLE;
   }

   if (strcmp( memTable->version, UFDBdbVersion ) > 0)
   {
      syslog( LOG_ALERT, "URL table %s has an unsupported data format (%s)", tableName, memTable->version );
      ufdbLogFatalError( "UFDB file for table \"%s\" has data format version \"%s\" while "
      		         "this program\n"
                         "does not support versions higher than \"%s\"", 
		         tableName, memTable->version, UFDBdbVersion );
      ufdbLogFatalError( "Download/install a new version of this program." );
      ufdbLogFatalError( "Go to http://www.urlfilterdb.com" );
      return UFDB_API_ERR_INVALID_TABLE;
   }

   /* starting with version 2.0 we need the indexSize */
   if (strcmp( "2.0", memTable->version ) >= 0)
   {
      if (n < 7)
      {
	 syslog( LOG_ALERT, "URL table %s has an invalid UFDB2 header", tableName );
	 ufdbLogFatalError( "URL table %s has an invalid UFDB2 header", tableName );
	 ufdbLogFatalError( "contact support@urlfilterdb.com" );
	 return UFDB_API_ERR_INVALID_TABLE;
      }
      if (strcmp( "2.1", memTable->version ) >= 0)
      {
         /* starting with version 2.1 encrypted tables have
	  * an additional 64 random/dummy bytes at the start of the table.
	  */
	 ;
      }
   }

   if (strlen( key ) < 19)
   {
      ufdbLogFatalError( "UFDB file for table \"%s\" has an invalid key\n"
                         "contact support@urlfilterdb.com",
                         tableName );
      return UFDB_API_ERR_INVALID_KEY;
   }

   memTable->key[0] = key[0];
   memTable->key[1] = key[1];
   memTable->key[2] = key[2];
   memTable->key[3] = key[3];
   /* skip '-' */
   memTable->key[4] = key[5];
   memTable->key[5] = key[6];
   memTable->key[6] = key[7];
   memTable->key[7] = key[8];
   /* skip '-' */
   memTable->key[8] = key[10];
   memTable->key[9] = key[11];
   memTable->key[10] = key[12];
   memTable->key[11] = key[13];
   /* skip '-' */
   memTable->key[12] = key[15];
   memTable->key[13] = key[16];
   memTable->key[14] = key[17];
   memTable->key[15] = key[18];

   strcpy( memTable->date, date );

   memTable->age = 0;
   if (strcmp( date, "nodate" ) != 0)
   {
      struct tm   tm;

      if (5 != sscanf( date, "%4d%2d%2d.%2d%2d", 
                       &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min ))
      {
	 memTable->age = -1;
	 syslog( LOG_ALERT, "URL table %s has an invalid date", tableName );
         ufdbLogFatalError( "URL table %s has an invalid date (date=%13.13s)", tableName, date );
         ufdbLogFatalError( "contact support@urlfilterdb.com" );
	 return UFDB_API_ERR_INVALID_TABLE;
      }
      else
      {
	 time_t      t_now;
	 time_t      t_db;
	 time_t      diff;

	 tm.tm_year -= 1900;
	 tm.tm_mon  -= 1;
	 tm.tm_isdst = 0;
	 tm.tm_sec   = 0;

	 t_db = mktime( &tm );
	 t_now = time( NULL );
	 diff = t_now - t_db;
	 memTable->age = diff / (24 * 60 * 60);
	 if (memTable->age < -1)	/* allow 1 day back for various time zones */
	 {
	    ufdbLogFatalError( "table %s has an invalid date (%13.13s)", tableName, date );
	    ufdbLogFatalError( "the difference between current system time and database timestamp is %ld seconds", diff );
            ufdbLogFatalError( "contact support@urlfilterdb.com" );
	    retval = UFDB_API_ERR_INVALID_TABLE;
	 }
	 else if (memTable->age > UFDB_MAX_TABLE_AGE  &&  flags[1] == 'P')
	 {
	    ufdbLogFatalError( "table %s is dated %13.13s and has EXPIRED.  *******************\n"
	                       "Check licenses and cron job for ufdbUpdate.  "
                               "Run 'ufdbUpdate -v' to verify the URL database download.",
			       tableName, date );
	    retval = UFDB_API_STATUS_DATABASE_EXPIRED;
	    UFDBglobalDatabaseStatus = UFDB_API_STATUS_DATABASE_EXPIRED;
	 }
	 else if (memTable->age > UFDB_WARN_TABLE_AGE  &&  flags[1] == 'P')
	 {
	    /* TODO: change state to DB_WARNING */
	    ufdbLogError( "table %s is dated %13.13s and is %d days old.  *******************\n"
			  "Check licenses and cron job for ufdbUpdate.  "
                          "Run 'ufdbUpdate -v' to verify the URL database download.",
			  tableName, date, memTable->age );
	    retval = UFDB_API_STATUS_DATABASE_OLD;
	    if (UFDBglobalDatabaseStatus == UFDB_API_STATUS_DATABASE_OK)
	       UFDBglobalDatabaseStatus = UFDB_API_STATUS_DATABASE_OLD;
	 }
#if 0
	 ufdbLogMessage( "t_db  %12ld", t_db );
	 ufdbLogMessage( "t_now %12ld", t_now );
	 ufdbLogMessage( "diff  %12ld", t_now - t_db );
#endif
      }
   }

   if (retval != UFDB_API_OK  &&  retval != UFDB_API_STATUS_DATABASE_OLD)
   {
      memTable->table.tag = (unsigned char *) "NEVER";
      memTable->mem = NULL;
      memTable->hdrSize = 0;
      memTable->fileSize = 0;
      memTable->table.nNextLevels = 0;
      memTable->table.nextLevel = NULL;
   }
   else
   {
      ufdbLogMessage( "loading URL category %s with creation date %s", tableName, date );

      if (cksum > 0 && strcmp( "2.1", memTable->version ) >= 0)
      {
         int   mycksum;

	 mycksum = UFDBcalcCksum( (char *) memTable->mem + memTable->hdrSize, memTable->fileSize - memTable->hdrSize );
	 if (mycksum == 0)
	    mycksum = 1;

	 if (cksum >= 0  &&  cksum != mycksum)
	 {
	    memTable->table.tag = (unsigned char *) "NEVER";
	    memTable->mem = NULL;
	    memTable->fileSize = 0;
	    memTable->hdrSize = 0;
	    memTable->table.nNextLevels = 0;
	    memTable->table.nextLevel = NULL;
	    ufdbLogError( "URL category %s has a file cksum of %05d but a different memory cksum of %05d", tableName, cksum, mycksum );
#if 0
	    return UFDB_API_ERR_CKSUM_NOT_VALID;
#endif
	 }
      }

      if (flags[2] == 'Q')			/* encrypted table */
      {
	 mem = (unsigned char *) memTable->mem + memTable->hdrSize;
	 ufdbCryptInit( &uc, (unsigned char *) memTable->key, 16 );
	 ufdbEncryptText( &uc, mem, mem, memTable->fileSize - memTable->hdrSize );
	 if (UFDBglobalDebug)
         {
	    ufdbLogMessage( "table %s decrypted", tableName );
         }
      }

      if (memTable->flags[0] == 'Z')		/* ZLIB compressed table */
      {
	 rv = ufdbDecompressZLIBTable( memTable );
         if (rv != UFDB_API_OK)
            retval = rv;
	 if (UFDBglobalDebug)
         {
            if (retval == UFDB_API_OK)
               ufdbLogMessage( "table %s uncompressed", tableName );
            else
               ufdbLogMessage( "table %s uncompression failed with error %d", tableName, retval );
         }
      }
      else if (memTable->flags[0] == 'C')	/* BZIP2 compressed table */
      {
	 rv = ufdbDecompressBZIP2Table( memTable );
         if (rv != UFDB_API_OK)
            retval = rv;
	 if (UFDBglobalDebug)
         {
            if (retval == UFDB_API_OK)
               ufdbLogMessage( "table %s uncompressed", tableName );
            else
               ufdbLogMessage( "table %s uncompression failed with error %d", tableName, retval );
         }
      }
   }

   return retval;
}


#if UFDB_OVERRIDE_GCC_OPT  && ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC push_options
#pragma GCC optimize ("O3")
#pragma GCC optimize ("align-functions=64")
#pragma GCC optimize ("no-stack-protector")
#endif


#include "strcmpurlpart.static.c"


UFDB_GCC_INLINE
static unsigned char * parseNextTag_1_2( 
   struct UFDBtable * parent, 
   unsigned char *    mem )
{
   unsigned char *    tag;
   unsigned char      tagType;
   int                n;

   while (mem != NULL)
   {
      tag = mem;
      while (*mem >= ' ')
	 mem++;

      tagType = *mem;
      *mem++ = '\0';

      switch (tagType)
      {
      case UFDBsubLevel:
	    DEBUG(( "   parse  tag = %-10s  sub-level", tag )); /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel = _reallocTableArray_1_2( parent->nextLevel, parent->nNextLevels );
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = NULL;

	    mem = parseNextTag_1_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsameLevel:
	    DEBUG(( "   parse  tag = %-10s  same-level", tag )); /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel = _reallocTableArray_1_2( parent->nextLevel, parent->nNextLevels );
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = NULL;

	    break;

      case UFDBprevLevel:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse  tag = %-10s  prev-level", tag )); /* */

	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel = _reallocTableArray_1_2( parent->nextLevel, parent->nNextLevels );
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else 
	    {
	       DEBUG(( "   parse  tag = %-10s  prev-level", "*" )); /* */
	       ;
	    }
	    return mem;
	    break;

      case UFDBendTable:
	    DEBUG(( "   parse  tag = %-10s  end-of-table", tag[0] >= ' ' ? (char*) tag : "NONE" )); /* */
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse  tag = %-10s  end-of-table", tag )); /* */

	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel = _reallocTableArray_1_2( parent->nextLevel, parent->nNextLevels );
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    return NULL;
	    break;

      default:
            DEBUG(( "tagType = %d !", tagType ));
	    ;
      }
   }

   return NULL;
}


UFDB_GCC_INLINE
static struct UFDBtable * _allocTableIndex( int num )
{
   struct UFDBtable * t;
   
   DEBUG(( "      _allocTableIndex( %3d )   ti = %d", num, tableIndex_i ));

   t = &tableIndex[tableIndex_i];
   tableIndex_i += num;

   return t;
}


static unsigned char * parseNextTag_2_0(
   struct UFDBtable * parent, 
   unsigned char *    mem )
{
   unsigned char *    tag;
   unsigned char      tagType;
   int                n;
   unsigned int       num_children;

   while (mem != NULL)
   {
      tag = mem;
      while (*mem >= ' ')		/* find the end of the tag */
	 mem++;

      tagType = *mem;			/* the tagtype is after the tag and overwritten with a \0 */
      *mem++ = '\0';

      switch (tagType)
      {
      case UFDBsubLevel:
	    num_children = *mem++;
	    if (num_children == 0)
	    {
	       num_children = *mem  +  (*(mem+1) * 256)  +  (*(mem+2) * 65536);
	       mem += 3;
	    }

	    DEBUG(( "   parse2_0 tag = %-18s  sub-level   %d child(ren)", 
	            tag, num_children ));   /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_0( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsameLevel:
	    DEBUG(( "   parse2_0 tag = %-18s  same-level", tag )); /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = NULL;

	    break;

      case UFDBprevLevel:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_0 tag = %-18s  prev-level", tag )); /* */

	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else 
	    {
	       DEBUG(( "   parse2_0 tag = %-18s  prev-level", "*" )); /* */
	       ;
	    }
	    return mem;
	    break;

      case UFDBendTable:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_0 tag = %-18s  end-of-table", tag )); /* */
	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else
	    {
	       DEBUG(( "   parse2_0 tag = %-18s  end-of-table", "NO-TAG-HERE" )); /* */
	       ;
	    }
	    return NULL;
	    break;
      }
   }
   DEBUG(( "parse2_0 end of parsing" ));

   return NULL;
}


static unsigned char * parseNextTag_2_1(
   struct UFDBtable * parent, 
   unsigned char *    mem )
{
   unsigned char *    tag;
   unsigned char      tagType;
   int                n;
   unsigned int       num_children;

   while (mem != NULL)
   {
      tag = mem;
      while (*mem >= ' ')		/* find the end of the optional tag */
	 mem++;

      tagType = *mem;			/* the tagtype is after the tag and overwritten with a \0 */
      *mem++ = '\0';

      switch (tagType)
      {
      case UFDBsubLevel:
	    num_children = *mem++;	/* between 8 and 255 */

	    DEBUG(( "   parse2_1 tag = %-18s  sub-level   %d child(ren)", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel1:
            num_children = 1;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level1  1 child", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel2:
            num_children = 2;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level2  2 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel3:
            num_children = 3;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level3  3 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel4:
            num_children = 4;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level4  4 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel5:
            num_children = 5;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level5  5 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel6:
            num_children = 6;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level6  6 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel7:
            num_children = 7;
	    DEBUG(( "   parse2_1 tag = %-18s  sub-level7  7 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevelNNN:
	    num_children = *mem  +  (*(mem+1) * 256);           /* between 256 and 65535 children */
	    mem += 2;

	    DEBUG(( "   parse2_1 tag = %-18s  sub-level-NNN %d children", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevelNNNNN:
	    							/* between 65536 and 4 billion children */
	    num_children = *mem  +  (*(mem+1) * 256)  +  (*(mem+2) * 256*256)  +  (*(mem+3) * 256*256*256);	
	    mem += 4;

	    DEBUG(( "   parse2_1 tag = %-18s  sub-level-NNNNN %d children", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_1( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsameLevel:
	    DEBUG(( "   parse2_1 tag = %-18s  same-level", tag )); /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = NULL;

	    break;

      case UFDBprevLevel:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_1 tag = %-18s  prev-level", tag )); /* */

	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else 
	    {
	       DEBUG(( "   parse2_1 tag = %-18s  prev-level", "*" )); /* */
	       ;
	    }
	    return mem;
	    break;

      case UFDBendTable:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_1 tag = %-18s  end-of-table", tag )); /* */
	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else
	    {
	       DEBUG(( "   parse2_1 tag = %-18s  end-of-table", "END" )); /* */
	       ;
	    }
	    return NULL;
	    break;

      default:
            ufdbLogFatalError( "cannot parse 2.1 table: tag type is %03o", tagType );
	    return NULL;
	    break;
      }
   }

   return NULL;
}


static unsigned char * parseNextTag_2_2(
   struct UFDBtable * parent, 
   unsigned char *    mem )
{
   unsigned char *    tag;
   unsigned char      tagType;
   int                n;
   unsigned int       num_children;

   while (mem != NULL)
   {
      tag = mem;
      if (*tag == '\0')			/* do we have an empty tag ? */
      {
         ++mem;
      }
      else if (*mem >= ' ')		/* do we have a tag ? */
      {
	 while (*mem != '\0')		/* find the end of the tag */
	    ++mem;
	 ++mem;				/* skip the \0 delimiter of the tag */
      }

      tagType = *mem++;

      switch (tagType)
      {
      case UFDBsubLevel:
	    num_children = *mem++;	/* between 8 and 255 */

	    DEBUG(( "   parse2_2 tag = %-18s  sub-level   %d child(ren)", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel1:
            num_children = 1;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level1  1 child", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel2:
            num_children = 2;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level2  2 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel3:
            num_children = 3;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level3  3 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel4:
            num_children = 4;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level4  4 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel5:
            num_children = 5;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level5  5 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel6:
            num_children = 6;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level6  6 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevel7:
            num_children = 7;
	    DEBUG(( "   parse2_2 tag = %-18s  sub-level7  7 children", tag ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevelNNN:
	    num_children = *mem  +  (*(mem+1) * 256);		/* between 256 and 65535 children */
	    mem += 2;

	    DEBUG(( "   parse2_2 tag = %-18s  sub-level-NNN %d children", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsubLevelNNNNN:
	    							/* between 65536 and 4 billion children */
	    num_children = *mem  +  (*(mem+1) * 256)  +  (*(mem+2) * 256*256)  +  (*(mem+3) * 256*256*256);	
	    mem += 4;

	    DEBUG(( "   parse2_2 tag = %-18s  sub-level-NNNNN %d children", tag, num_children ));

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = _allocTableIndex( num_children );

	    mem = parseNextTag_2_2( &(parent->nextLevel[n]), mem );
	    break;

      case UFDBsameLevel:
	    DEBUG(( "   parse2_2 tag = %-18s  same-level", tag )); /* */

	    n = parent->nNextLevels;
	    parent->nNextLevels++;
	    parent->nextLevel[n].tag = tag;
	    parent->nextLevel[n].nNextLevels = 0;
	    parent->nextLevel[n].nextLevel = NULL;

	    break;

      case UFDBprevLevel:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_2 tag = %-18s  prev-level", tag )); /* */

	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else 
	    {
	       DEBUG(( "   parse2_2 tag = %-18s  prev-level", "*" )); /* */
	       ;
	    }
	    return mem;
	    break;

      case UFDBendTable:
	    if (tag[0] >= ' ')   /* is the code followed by a tag or another code ? */
	    {
	       DEBUG(( "   parse2_2 tag = %-18s  end-of-table", tag )); /* */
	       n = parent->nNextLevels;
	       parent->nNextLevels++;
	       parent->nextLevel[n].tag = tag;
	       parent->nextLevel[n].nNextLevels = 0;
	       parent->nextLevel[n].nextLevel = NULL;
	    }
	    else
	    {
	       DEBUG(( "   parse2_2 tag = %-18s  end-of-table", "END" )); /* */
	       ;
	    }
	    return NULL;
	    break;

      default:
            ufdbLogFatalError( "cannot parse 2.2 table: tag type is %03o", tagType );
#if UFDB_DO_DEBUG
	    printMem( mem );
#endif
	    return NULL;
	    break;
      }
   }

   return NULL;
}


/* Parse a binary table that is loaded into memory.
 */
void UFDBparseTable( struct UFDBmemTable * memTable )
{
   memTable->table.nNextLevels = 0;
   memTable->table.nextLevel = (struct UFDBtable *) ufdbCalloc( 1, sizeof(struct UFDBtable) );		/* TODO: investigate leak */

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "UFDBparseTable: version=%s  flags=%8.8s  %d  %ld  %d  hdrsize=%d",
                      memTable->version, memTable->flags, memTable->numEntries, memTable->fileSize, 
		      memTable->indexSize, memTable->hdrSize );

   if (strcmp( memTable->version, "2.2" ) >= 0)
   {
      memTable->index = (struct UFDBtable *) ufdbCalloc( memTable->indexSize+1, sizeof(struct UFDBtable) );
      tableIndex = (struct UFDBtable *) memTable->index;
      tableIndex_i = 0;
      tableMemStart = (unsigned char *) memTable->mem;
      memStart = (unsigned char *) memTable->mem + memTable->hdrSize + (memTable->flags[2] == 'Q' ? 64 : 0);
      (void) parseNextTag_2_2( &(memTable->table), memStart );
   }
   else if (strcmp( memTable->version, "2.1" ) >= 0)
   {
      memTable->index = (struct UFDBtable *) ufdbCalloc( memTable->indexSize+1, sizeof(struct UFDBtable) );
      tableIndex = (struct UFDBtable *) memTable->index;
      tableIndex_i = 0;
      (void) parseNextTag_2_1( &(memTable->table),
                               (unsigned char *) memTable->mem + memTable->hdrSize + (memTable->flags[2] == 'Q' ? 64 : 0) );
   }
   else if (strcmp( memTable->version, "2.0" ) >= 0)
   {
      memTable->index = (struct UFDBtable *) ufdbCalloc( memTable->indexSize+1, sizeof(struct UFDBtable) );
      tableIndex = (struct UFDBtable *) memTable->index;
      tableIndex_i = 0;
      (void) parseNextTag_2_0( &(memTable->table),
                               (unsigned char *) memTable->mem + memTable->hdrSize );
#if UFDB_DO_DEBUG
      ufdbLogMessage( "predicted index size: %d   last index: %d", memTable->indexSize, tableIndex_i );
#endif
   }
   else 	/* version == "1.2" */
   {
      memTable->index = NULL;
      (void) parseNextTag_1_2( &(memTable->table), 
                               (unsigned char *) memTable->mem + memTable->hdrSize );
   }
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static int mystrn2cmp( unsigned char * s1, unsigned char * s2, char * terminators )
{
   int diff;

   DEBUG(( "      mystrn2cmp( '%s' '%s' '%s' )", (char*) s1, (char*) s2, (char*) terminators ));
   diff = 0;
   while (*s2 != '\0')
   {
      diff = ((unsigned int) *s1) - ((unsigned int) *s2);
      if (diff != 0)
         return diff;
      s2++;
      s1++;
   }
   diff = ((unsigned int) *s1) - ((unsigned int) *s2);
   DEBUG(( "!        mystrn2cmp( '%s' '%s' '%s' )   diff %d", 
           (char*) s1, (char*) s2, terminators, diff ));
   while (*terminators != '\0')
   {
      if (*s1 == (unsigned char) *terminators)
      {
         DEBUG(( "         mystrn2cmp matched terminator %c  return 0", *terminators ));
         return 0;
      }
      terminators++;
   }
   DEBUG(( "         mystrn2cmp reached eos s2  return diff %d", diff ));
   return diff;
}


UFDB_GCC_HOT   /* recursive */
static int verifyURLparams( 
   struct UFDBtable * t, 
   UFDBrevURL *       revUrl )
{
   UFDBrevURL *	      origRevUrl;
   struct UFDBtable * tparam;
   unsigned char *    URLparam;
   unsigned char *    URLvalue;
   int                nl;
   int                tagLen;
   int                i, b, e, cmp;

   origRevUrl = revUrl;
   DEBUG(( "    verifyURLparams:  t.tag[0] '%s'  revURL '%s'", 
           (char*) t->nextLevel[0].tag, (char*) revUrl->part ));

   /* At this point revURL points to the list of parameters and
    * t points to a table of parameters and values with OR logic
    */
   for (nl = 0;  nl < t->nNextLevels;  nl++)
   {
      tparam = &t->nextLevel[nl];
      revUrl = origRevUrl;
      URLparam = revUrl->part;

      tagLen = 0;
      while (tparam->tag[tagLen] != '\0')	/* inlined strlen() */
         tagLen++;

      DEBUG(( "    verifyURLparams: table parameter %d '%s'  tagLen %d  nNextLevels %d  "
              "Matching against parameters %s", 
	      nl, tparam->tag, tagLen, tparam->nNextLevels, URLparam ));

      /* Find the parameter (tparam->tag) in the URL parameters (revUrl->part).
       * Note that a simple strstr() does not work since the parameter name must be right after BEGINNING or &.
       */
      while (URLparam != NULL)
      {
	 if (mystrn2cmp( URLparam, tparam->tag, "=&" ) == 0
             &&  (URLparam[tagLen] == '='  ||  URLparam[tagLen] == '&'  ||  URLparam[tagLen] == '\0'))
	 {
            /* we must return 1 iff the table has no values for this parameter !!!  */
            if (tparam->nNextLevels == 0)
            {
               DEBUG(( "    verifyURLparams: found parameter '%s' in URL and table has no values  return 1", 
                       tparam->tag ));
               return 1;
            }

            /* check valueless parameter */
            if (URLparam[tagLen] == '&'  ||  URLparam[tagLen] == '\0')
            {
               if (tparam->nNextLevels == 1  &&  tparam->nextLevel[0].tag[0] == '\0')
               {
                  DEBUG(( "    verifyURLparams: found valueless parameter '%s' check the next parameter",
                          tparam->tag ));
                  return verifyURLparams( &(tparam->nextLevel[0]), origRevUrl );
               }

               DEBUG(( "    verifyURLparams: found valueless parameter '%s' in URL and table has values  return 0",
                       tparam->tag ));
               return 0;
            }

	    URLvalue = URLparam + tagLen + 1;

	    DEBUG(( "    verifyURLparams: found parameter '%s' in URL.  value '%s'", 
                    tparam->tag, URLvalue ));

	    /* do a binary search for the URLvalue in tparam */
	    b = 0;
	    e = tparam->nNextLevels - 1;
	    DEBUG(( "    verifyURLparams: starting binary search  b=%d  e=%d", b, e ));
	    while (b <= e)
	    {
	       i = (b + e) >> 1;
	       cmp = mystrn2cmp( URLvalue, tparam->nextLevel[i].tag, "&" );
	       DEBUG(( "         mystrn2cmp[%d]  '%s'  '%s'  = %3d", 
                       i, URLvalue, tparam->nextLevel[i].tag, cmp ));
	       if (cmp < 0)
		  e = i - 1;
	       else if (cmp > 0)
		  b = i + 1;
	       else
	       {
		  /* see if there are more parameters to match */
		  if (tparam->nextLevel[i].nNextLevels > 0)
                  {
                     DEBUG(( "    verifyURLparams: check the next parameter" ));
		     return verifyURLparams( &(tparam->nextLevel[i]), origRevUrl );
                  }
                  DEBUG(( "    verifyURLparams: there are no more parameters: return 1" ));
		  return 1;
	       }
	    }
            DEBUG(( "    verifyURLparams: binary search finished: b=%d e=%d  return 0", b, e ));
	    return 0;
	 }
         else
	    DEBUG(( "    verifyURLparams: '%s' not first parameter in '%s'", 
                    (char*) tparam->tag, (char*) URLparam ));

	 URLparam = (unsigned char *) mystrchr( (char*) URLparam, '&' );
	 if (URLparam != NULL)
	    URLparam++;
	 else
	    if (revUrl->next != NULL)
	    {
	       /* TODO: CHECK THIS: loop over all RevURL parts that contain parameters */
               DEBUG(( "      verifyURLparams: goto next revUrl part" ));
	       revUrl = revUrl->next;
	       URLparam = revUrl->part;
	    }
      }
   }

   return 0;
}


/* perform lookup of revUrl in table t.
 * return 1 iff found, 0 otherwise.
 */
UFDB_GCC_HOT 
int UFDBlookupRevUrl( 
   struct UFDBtable * t, 
   UFDBrevURL *       revUrl )
{
   int b, e;
#if UFDB_DO_DEBUG
   struct UFDBtable * origtable = t;
#endif

begin:
   DEBUG(( "    UFDBlookupRevUrl:  table %-14s [%d]  tag %s  :  %s",   
           origtable->tag, origtable->nNextLevels, t->tag, revUrl->part ));

   /* binary search */
   b = 0;
   e = t->nNextLevels - 1;

   while (b <= e)
   {
      int  cmp;
      int  i, is_path;

      i = (b + e) >> 1;
      cmp = strcmpURLpart( (char *) revUrl->part, (char *) t->nextLevel[i].tag );
      DEBUG(( "      strcmpURLpart[%d]  %s  %s  = %d", i, revUrl->part, t->nextLevel[i].tag, cmp ));
      if (cmp < 0)
	 e = i - 1;
      else if (cmp > 0)
	 b = i + 1;
      else					/* URL part and tag are equal */
      {
	 is_path = (revUrl->part[0] == '/');
	 t = &(t->nextLevel[i]);
	 revUrl = revUrl->next;
	 if (t->nNextLevels == 0)		/* no more levels in table -> MATCH */
	    return 1;
	 if (revUrl == NULL)			/* no more levels in URL -> NO match */
	    return 0;
	 if (is_path)
	 {
	    int rv = verifyURLparams( t, revUrl );
	    DEBUG(( "      strcmpURLpart[%d]   verifyURLparams returned %d", i, rv ));
	    return rv;
	 }

	 /* optimise the recursive call : */
	 /* return UFDBlookupRevUrl( t, revUrl ); */
	 goto begin;
      }
   }

   return 0;  /* not found */
}


#if UFDB_OVERRIDE_GCC_OPT  &&  ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC pop_options
#endif


/*
 *  UFDBlookup: lookup a domain/URL in domain and URL databases.  
 *  return 1 if found.
 */
int UFDBlookup( 
   UFDBthreadAdmin *     admin,
   struct UFDBmemTable * mt, 
   char *                request )
{
   int                   result;
   UFDBrevURL *          revUrl;
   
   if (admin == NULL)
   {
      ufdbLogFatalError( "UFDBlookup admin=NULL" );
      return 0;
   }

   revUrl = UFDBgenRevURL( admin, (unsigned char *) request );
   result = UFDBlookupRevUrl( &(mt->table.nextLevel[0]), revUrl );
   DEBUG(( "  UFDBlookup( %s, %s ) = %d", mt->table.nextLevel[0].tag, request, result ));

   UFDBfreeRevURL( admin, revUrl );

   return result;
}


/*
 *  Initialise a database category (open and/or create a .ufdb file) 
 */
int UFDBloadDatabase( 
   struct UFDBmemTable * mtable, 
   char *                file    )
{
   int                   n;
   int                   in;
   struct stat           fileStat;
   char                  f[1024];

   if (file == NULL)
   {
      mtable->table.tag = (unsigned char *) "FILE-IS-NULL";
      mtable->table.nNextLevels = 0;
      mtable->table.nextLevel = NULL;
      mtable->mem = NULL;
      mtable->fileSize = 0;
      mtable->hdrSize = 0;
      mtable->version[0] = '\0';
      mtable->numEntries = 0;
      mtable->indexSize = 0;
      mtable->index = NULL;
      return UFDB_API_ERR_NULL;
   }

   strcpy( f, file );
   in = open( f, O_RDONLY );
   if (in < 0)
   {
      strcat( f, UFDBfileSuffix );
      in = open( f, O_RDONLY );
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   UFDBloadDatabase: file '%s' %s", file, in<0 ? "does not exist" : "exists" );

   if (in < 0)
   {
      mtable->table.tag = (unsigned char *) "NO-FILE";
      mtable->table.nNextLevels = 0;
      mtable->table.nextLevel = NULL;
      mtable->mem = NULL;
      mtable->fileSize = 0;
      mtable->hdrSize = 0;
      mtable->version[0] = '\0';
      mtable->numEntries = 0;
      mtable->indexSize = 0;
      mtable->index = NULL;
      return UFDB_API_ERR_NOFILE;
   }

   if (fstat(in,&fileStat) < 0)
   {
      close( in );
      mtable->table.tag = (unsigned char *) "FSTAT-ERROR";
      mtable->table.nNextLevels = 0;
      mtable->table.nextLevel = NULL;
      mtable->mem = NULL;
      mtable->fileSize = 0;
      mtable->hdrSize = 0;
      mtable->version[0] = '\0';
      mtable->numEntries = 0;
      mtable->indexSize = 0;
      mtable->index = NULL;
      return UFDB_API_ERR_NOFILE;
   }

   mtable->table.tag = (unsigned char *) "UNIVERSE";
   mtable->table.nNextLevels = 0;
   mtable->table.nextLevel = NULL;
   /* old user-defined tables may not have a 32-byte safeguard for SSE and AVX2 instructions */
   mtable->mem = ufdbMalloc( fileStat.st_size + 1 + 32 );	
   mtable->fileSize = fileStat.st_size;
   n = read( in, mtable->mem, fileStat.st_size );
   close( in );
   if (n != fileStat.st_size)
   {
      ufdbFree( mtable->mem );
      mtable->table.tag = (unsigned char *) "READ-ERROR";
      mtable->table.nNextLevels = 0;
      mtable->table.nextLevel = NULL;
      mtable->mem = NULL;
      mtable->fileSize = 0;
      mtable->hdrSize = 0; 
      mtable->version[0] = '\0';
      mtable->numEntries = 0;
      mtable->indexSize = 0;
      mtable->index = NULL;
      return UFDB_API_ERR_READ;
   }

   n = UFDBparseTableHeader( mtable );
#if 0
   ufdbLogMessage( "   UFDBparseTableHeader returns %d", n );
#endif
   if (n != UFDB_API_OK  &&  n != UFDB_API_STATUS_DATABASE_OLD)
   {
      ufdbFree( mtable->mem );
      mtable->table.tag = (unsigned char *) "READ-ERROR";
      mtable->table.nNextLevels = 0;
      mtable->table.nextLevel = NULL;
      mtable->mem = NULL;
      mtable->fileSize = 0;
      mtable->hdrSize = 0; 
      mtable->version[0] = '\0';
      mtable->numEntries = 0;
      mtable->indexSize = 0;
      mtable->index = NULL;
      return n;
   }
   UFDBparseTable( mtable );

   UFDBglobalDatabaseLoadTime = time( NULL );
   if (strstr( f, "checked/domains.ufdb" ) != NULL)
      strcpy( UFDBglobalDateOfCheckedDB, mtable->date );

   return n;
}


const char * ufdbCategoryName( const char * domain )
{
#ifndef UFDB_GEN_API
   UFDBrevURL *      revurl;
   struct Category * cat;
   char *            buffer;
   UFDBthreadAdmin * myadm;

   if (domain == NULL)
      return "null";
   
   if (UFDBglobalReconfig || UFDBglobalTerminating)
      return "unknown";

   myadm = UFDBallocThreadAdmin();

   buffer = ufdbStrdup( domain );
   revurl = UFDBgenRevURL( myadm, (unsigned char *) buffer );

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      if (cat->domainlistDb != NULL)
      {
	 struct UFDBmemTable * mt;
	 mt = (struct UFDBmemTable *) cat->domainlistDb->dbcp;
	 if (mt != NULL)
	 {
	    if (UFDBlookupRevUrl( &(mt->table.nextLevel[0]), revurl ))
	       break;
	 }
      }
   }

   ufdbFree( buffer );
   UFDBfreeRevURL( myadm, revurl );
   ufdbFree( myadm );

   if (cat != NULL)
      return cat->name;
#endif

   return "any";
}


#ifndef UFDB_GEN_API
/*
 * Initialise a DB (open and/or create a .ufdb file) 
 */
void sgDbInit( 
   struct sgDb *         Db, 
   char *                file )
{
   struct UFDBmemTable * tab;
   int                   n;
   int                   in;
   struct stat           fileStat;
   struct stat           basefileStat;
   char                  f[1024];

   if (file == NULL  ||  *file == '\0')
   {
      ufdbLogError( "sgDbInit: cannot load URL table since file is NULL" );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }

#if defined(UFDB_DEBUG)
   ufdbLogMessage( "  sgDbInit( 0x%08x, %s )", Db, file );
#endif

   if (strstr( file, UFDBfileSuffix ) == NULL)
      sprintf( f, "%s%s", file, UFDBfileSuffix );
   else
      strcpy( f, file );

   in = open( f, O_RDONLY );
   if (in < 0)
   {
      ufdbLogFatalError( "cannot read from \"%s\" (read-only): %s", f, strerror(errno) );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }

   if (fstat(in,&fileStat) < 0)
   {
      ufdbLogFatalError( "fstat failed on \"%s\": %s", file, strerror(errno) );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }

   if (stat( file, &basefileStat ) == 0)
   {
      if (basefileStat.st_mtime > fileStat.st_mtime)
         ufdbLogMessage( "WARNING: %s is newer than %s.  You should run ufdbGenTable *****", file, f );
   }

   tab = (struct UFDBmemTable *) ufdbMallocAligned( 64, sizeof(struct UFDBmemTable) );
   if (tab == NULL)
   {
      ufdbLogFatalError( "cannot allocate %ld bytes memory for table header", (long) sizeof(struct UFDBmemTable) );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }
   tab->fileSize = fileStat.st_size;
   tab->table.tag = (unsigned char *) "UNIVERSE";
   tab->table.nNextLevels = 0;
   tab->table.nextLevel = NULL;
   /* old user-defined tables may not have a 32-byte safeguard for SSE and AVX2 instructions */
   tab->mem = ufdbMalloc( fileStat.st_size + 1 + 32 );
   if (tab == NULL)
   {
      ufdbLogFatalError( "cannot allocate %ld bytes memory for table %s", (long) fileStat.st_size+1, file );
      ufdbFree( tab );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }
   n = read( in, tab->mem, fileStat.st_size );
   if (n != fileStat.st_size)
   {
      ufdbLogFatalError( "read failed on \"%s\" n=%d st_size=%ld %s", file, n, (long) fileStat.st_size, strerror(errno) );
      ufdbFree( tab->mem );
      ufdbFree( tab );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }
   close( in );

   n = UFDBparseTableHeader( tab );
   if (n == UFDB_API_STATUS_DATABASE_OLD)
   {
      ufdbLogMessage( "warning: the URL table \"%s\" is over 4 days old  *****", file );
      n = UFDB_API_OK;
   }
   if (n == UFDB_API_OK)
      UFDBparseTable( tab );
   else
   {
      ufdbLogFatalError( "table \"%s\" could not be parsed. error code = %d", file, n );
      ufdbFree( tab->mem );
      ufdbFree( tab );
      Db->dbcp = NULL;
      Db->entries = -1;
      return;
   }

   Db->dbcp = (void *) tab;
   Db->entries = tab->numEntries;
}


/*
 *  sgDbLookup: lookup an item in a in-memory database.
 */
int sgDbLookup( struct sgDb * Db, char * request, char ** retval )
{
#if 0
  ufdbLogError( "  sgDbLookup( 0x%08x, %s, 0x%08x )", Db, request, retval );
#endif

  return UFDBlookup( NULL, (struct UFDBmemTable *) Db->dbcp, request );
}

#endif
