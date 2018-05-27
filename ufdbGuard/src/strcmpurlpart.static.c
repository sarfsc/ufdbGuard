/* 
 * strcmpurlpart.static.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * $Id: strcmpurlpart.static.c,v 1.21 2017/03/08 19:15:05 root Exp root $
 */

/* =================================================================================
 * NOTE: with more recent versions of glibc, strcmp uses SSE4.2 instructions and is 
 * ultrafast but still slower than our loop - the SSE4.2 version is only faster for longer strings.
 */


#if UFDB_OVERRIDE_GCC_OPT  &&  ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif

/* must use gcc -msse4.2 in order to get __SSE4_2__ defined : */

/*
#ifdef __SSE4_2__
#include "inline-strcmp-sse42.c"
#endif
*/

#define UFDB_USE_LARGE_LOOP_UNROLL 0


UFDB_GCC_INLINE UFDB_GCC_HOT 
static int strcmpURLpart( const char * url, const char * table )
{
   unsigned char * u = (unsigned char *) url;
   unsigned char * t = (unsigned char *) table;

   if (*table != '/')
   {
#if 0
      /* inlined: return strcmp( url, table ); */
      while (*url != '\0'  &&  (*url - *table) == 0)
      {
	 url++;
	 table++;
      }
      return *url - *table;
#else

      do
      {
	 int d;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

#ifdef UFDB_USE_LARGE_LOOP_UNROLL
	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;

	 d = ((int) *u) - ((int) *t);
	 if (d != 0  ||  *u == '\0')
	    return d;
	 u++;
	 t++;
#endif
      } while (1);
#endif

   }
   else
   {
      while (*t != '\0')
      {
	 if ((((int) *u) - ((int) *t)) != 0)
	 {
	    if (*t == '|'  &&  *u == '\0')
	       return 0;
	    else
	       return ((int) *u) - ((int) *t);
	 }
	 u++;
	 t++;
      }
   }

   return 0;
}

#if UFDB_OVERRIDE_GCC_OPT  &&  ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC pop_options
#endif

