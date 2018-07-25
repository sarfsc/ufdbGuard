/*
 * genTable.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * Generate a binary table file (.ufdb) from unordered ASCII files 
 * with domains and urls.
 *
 * usage: ufdbGenTable [-F 1.2|2.0|2.1|2.2] [-V] [-n] [-C | -Z] [-X] [-k <key>] -t <tableName> -d <domains> [-u <urls>]
 *
 * RCS $Id: genTable.c,v 1.130 2018/03/18 18:54:35 root Exp root $
 */

#define UFDB_DO_DEBUG 0

/* ufdbGenTable needs speed! */
#undef _FORTIFY_SOURCE

#if UFDB_OVERRIDE_GCC_OPT  &&  !UFDB_DO_DEBUG && ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC optimize ("O3")
#endif

#if !UFDB_DO_DEBUG && defined(__OPTIMIZE__) && 0
#define __USE_STRING_INLINES 1
#endif


#include "ufdb.h"
#include "ufdblib.h"
#include "ufdbdb.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "bzlib.h"
#include "zlib.h"

#define strmatch2(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == '\0')
#define strmatch3(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == '\0')
#define strmatch4(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == '\0')
#define strmatch5(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  && (a)[5] == '\0')
#define strmatch6(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == '\0')
#define strmatch7(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == '\0')
#define strmatch8(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == '\0')
#define strmatch9(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == (b)[8]  &&  (a)[9] == '\0')
#define strmatch10(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == (b)[8]  &&  (a)[9] == (b)[9]  &&  (a)[10] == '\0')
#define strmatch11(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == (b)[8]  &&  (a)[9] == (b)[9]  &&  (a)[10] == (b)[10]  &&  (a)[11] == '\0')
#define strmatch12(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == (b)[8]  &&  (a)[9] == (b)[9]  &&  (a)[10] == (b)[10]  &&  (a)[11] == (b)[11]  &&  (a)[12] == '\0')
#define strmatch13(a,b)    ((a)[0] == (b)[0]  &&  (a)[1] == (b)[1]  &&  (a)[2] == (b)[2]  &&  (a)[3] == (b)[3]  &&  (a)[4] == (b)[4]  &&  (a)[5] == (b)[5]  &&  (a)[6] == (b)[6]  &&  (a)[7] == (b)[7]  &&  (a)[8] == (b)[8]  &&  (a)[9] == (b)[9]  &&  (a)[10] == (b)[10]  &&  (a)[11] == (b)[11]  &&  (a)[12] == (b)[12]  &&  (a)[13] == '\0')


/* This is an attempt to define a new algorithm for generating a table.
 * The reason for the need of a new algorithm is performance.
 * In December 2013 the adult category has 3076456 domains, 276707 urls,
 * 3353163 leaf nodes and 207536 index nodes:
 * #sub1:     156411   #sub2:      23879   #sub3:      11226   #sub4:       4923
 * #sub5:       2275   #sub6:       1269   #sub7:        847
 * #sub8+:      6443   #sub255+:     260   #sub64K+:       3
 *
 * On a Intel E5-2420 processor ufdbGenTable takes 32 seconds to generate a .ufdb table file
 * where 3 seconds are needed for composing an in-memory table of the 3 million domains
 * and 29 seconds are needed to insert the 275000 urls.
 * The total time to generate is too much and will only grow when the adult table grows.
 * The time to insert the relatively small amount of urls is simply too much and
 * caused by inserting elements in an array with 500000+ elements.  The array shift
 * is taking 90+ per cent of all CPU time.
 * The .com TLD has 539320 child nodes and the tumblr.com domain has 691892 child nodes.
 * Note that the L3 cache of the E5-2420 is 15 MB and that 700000 * 32 bytes (array element size)
 * is 21.3 MB and hence significantly larger than the L3 cache.
 *
 * The input files are 99% sorted so processing them uses a lot of fast tail insertions
 * except for IP addresses.  Therefore, processing the domains file is fast.
 * The processing of the urls file is slow since it causes inserts at random points
 * in the table and therefore causes a lot of array shifts.
 *
 * The new algorithm must be much faster: the target is to generate the adult table
 * in under 5 seconds.
 *
 * The new data structure is based upon the old one with the difference that
 * the large array will be divided in a list of small arrays which are fast to manipulate.
 * The list of arrays is a kind of a btree.  Each array has a variable size with a maximum of N.
 * N is choosen to be 2000 since 2000*32 = 64000 and is sufficiently small to not
 * cause much stress for the L2 cache (the L2 cache size is often 256K).
 * The array size must be small to make sure that an insert in the array is fast so
 * maybe N must be reduced to 1000 or less.  Experiments with different values were done
 * and showed a slight performance degradation with smaller and higher values.
 * Each array is filled and when it becomes full, it is split into 2 arrays where
 * the two new arrays each have 50% of the elements of the split arrays, EXCEPT
 * when the array is the last array of the list.  Since there is a lot of tail insertion,
 * the last array of the list, when it becomes full, is split into one array with N-1
 * elements and the new last array will have 1 element.
 * 
 * With N=2000 and a list of arrays with a total of 700000 elements has on average
 * 700 arrays.
 *
 * The data structure used by ufdbGenTable is for ufdbGenTable only and is not used by
 * the database load functions of ufdbguardd.
 *
 * The results are good:
 * ufdbGentable version   time (s)
 * 1.31                   32.5
 * 1.32 new algorithm      3.0
 */

#define MinNodesPerSTA		   1
#define MaxNodesPerSTA		2000

struct UFDBgentableNode;
struct UFDBgentableSTA;


struct UFDBgentableNode						/* 32 bytes */
{
   unsigned int               totalSubNodes;			/* sum(stas.nSubNodes) */
   unsigned int               nSTA;				/* number of STAs */
   unsigned char *            tag;				/* tag */
   struct UFDBgentableSTA *   stas;				/* Subnodes Table Arrays (STAs) */
};

struct UFDBgentableSTA						/* 16 bytes */
{
   unsigned int               nSubNodes;			/* #used nodes; n<nodeArraySize */
   unsigned int               nodeArraySize;			/* 0, 1, 8, 256 or 2000 */
   struct UFDBgentableNode *  subNodes;				/* not a pointer but an array! */
};

static FILE * fin;
static char * inFileName;
static char * urlsFileName;
static char * tableName;

static struct UFDBgentableNode * table = NULL;

#if 0
int    UFDBglobalDebug = 0;
#endif

static int  utf8support = 1;
static long numEntries = 0;
static long numNodes = 0;
static long numLeafNodes = 0;
static long numSub1 = 0;
static long numSub2 = 0;
static long numSub3 = 0;
static long numSub4 = 0;
static long numSub5 = 0;
static long numSub6 = 0;
static long numSub7 = 0;
static long numSub8plus = 0;
static long numSub255plus = 0;
static long numSub64Kplus = 0;
static long numIndexNodes = 0;
static int  numErrors = 0;
static int  doCrypt = 1;
static int  doBZ2compress = 0;
static int  doZLIBcompress = 0;
static int  doProd = 0;
static int  doPadding = 0;
static int  doWarnings = 1;
static int  doSanityCheck = 1;
static int  stripWWW = 0;
static int  skipOptimisations = 0;
static int  URLparamSupport = 0;
static char * format = UFDBdefaultdbVersion;

#if UFDB_DO_DEBUG || 0
#define DEBUG(x) fprintf x 
#else
#define DEBUG(x) 
#endif

#define ROUNDUPBY      16
#define ROUNDUP(i)     ( (i) + (ROUNDUPBY - ((i)%ROUNDUPBY) ) )

#define BIGROUNDUPBY   128
#define BIGROUNDUP(i)  ( (i) + (BIGROUNDUPBY - ((i)%BIGROUNDUPBY) ) )

#define ROUNDUPBYCUTOFF BIGROUNDUPBY

#include "strcmpurlpart.static.c"



#if HAVE_PUTC_UNLOCKED

#define myfast_putc(c,fp)   putc_unlocked(c,fp)
#define myfast_getc(fp)     getc_unlocked(fp)

#if defined(__linux__) && defined(_GNU_SOURCE)
#define fast_puts(s,fp)     fputs_unlocked(s,fp)
#else
   UFDB_GCC_INLINE int fast_puts( const char * s, FILE * fp )
   {
      int retval;

      retval = 1;
      while (*s != '\0'  &&  ((retval = myfast_putc(*s,fp)) != EOF))
	 s++;

      return retval;
   }
#endif

#else

#define myfast_putc(c,fp)  fputc(c,fp)
#define fast_puts(s,fp)    fputs(s,fp)

#define myfast_getc(fp)    fgetc(fp)
#endif


static void usage( void )
{
   fprintf( stderr, "usage: ufdbGenTable [-nNqV] [-C | -Z] [-k <key>] -t <tableName> -d <domains> [-u <urls>]\n" );
   fprintf( stderr, "flags: -n  no encryption\n" );
   fprintf( stderr, "       -k  16-char encryption key\n" );
   fprintf( stderr, "       -F  1.2|2.0|2.1|2.2 (default is %s)\n", UFDBdefaultdbVersion );
   fprintf( stderr, "       -t  tablename\n" );
   fprintf( stderr, "       -d  domains\n" );
   fprintf( stderr, "       -u  urls\n" );
   fprintf( stderr, "       -C  use bzip2 compression\n" );
   fprintf( stderr, "       -D  debug\n" );
   fprintf( stderr, "       -N  NO UTF8 support (skip URLs with UTF8 characters)\n" );
   fprintf( stderr, "       -q  be quiet (suppress warnings)\n" );
   fprintf( stderr, "       -s  sanity check for domain names (obsolete option - check is always done)\n" );
   fprintf( stderr, "       -V  print version and exit\n" );
   fprintf( stderr, "       -W  strip \"www.\" from URLs\n" );
   fprintf( stderr, "       -X  skip table optimisations - only for expert users\n" );
   fprintf( stderr, "       -Z  use zlib compression - up to 5 times faster than bzip2 but slightly less compression\n" );
   exit( 1 );
}


UFDB_GCC_INLINE static unsigned char randomChar( void )
{
   static unsigned char * a = (unsigned char *) "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
   return a[random() % 62];
}


static void randomisebuf64( unsigned char * buf )
{
   int    n;
   unsigned int   seed;
   FILE * fp;

   seed = 0x05ac7326;

   if (doProd)
   {
      fp = popen( "who", "r" );
      if (fp != NULL)
      {
	 int ch;
	 while ((ch = fgetc(fp)) != EOF)
	 {
	    seed = (seed >> 11) + 23 + 7 * (seed ^ ch);
	 }
	 fclose( fp );
      }
   }

#if 0
   printf( "random seed is %08x\n", seed );
#endif

   seed = (seed + 1309) ^ (((getpid() << 3) ^ (time(NULL) << 19)) + (getppid() << 26));
   srandom( seed );

   for (n = 0; n < 64; n++)
   {
      *buf++ = randomChar();
   }
}


void ufdbLogError( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];

   va_start( ap, format );
   if (vsnprintf(msg, UFDB_MAX_URL_LENGTH-1, format, ap) > (UFDB_MAX_URL_LENGTH - 2))
      msg[UFDB_MAX_URL_LENGTH-1] = '\0';
   va_end( ap );

   fprintf( stderr, "%s\n", msg );
}


void ufdbLogMessage( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];

   va_start( ap, format );
   if (vsnprintf(msg, UFDB_MAX_URL_LENGTH-1, format, ap) > (UFDB_MAX_URL_LENGTH - 2))
      msg[UFDB_MAX_URL_LENGTH-1] = '\0';
   va_end( ap );

   fprintf( stderr, "%s\n", msg );
}


void ufdbLogFatalError( const char * format, ... )
{
   va_list ap;
   char    msg[UFDB_MAX_URL_LENGTH];

   va_start( ap, format );
   if (vsnprintf(msg, UFDB_MAX_URL_LENGTH-1, format, ap) > (UFDB_MAX_URL_LENGTH - 2))
      msg[UFDB_MAX_URL_LENGTH-1] = '\0';
   va_end( ap );

   fprintf( stderr, "%s *****\n", msg );
}


void ufdbSetGlobalErrorLogFile( int mutex_is_used )
{
}



UFDB_GCC_HOT UFDB_GCC_INLINE 
static unsigned char * skipProtocol( unsigned char * URL )
{
   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;
   URL++;

   if (*URL == '\0')
      return NULL;
   if (*URL == ':'  &&  *(URL+1) == '/'  &&  *(URL+2) == '/')
      return URL+3;

   return NULL;
}


/* perform lookup of revUrl in the table pointed by its top node.
 * return 1 iff found, 0 otherwise.
 */
UFDB_GCC_HOT 
static int UFDBgentableLookupRevUrl( 
   struct UFDBgentableNode * node, 
   UFDBrevURL *              revUrl )
{
   int b, e;
   int i;
   int cmp;

   struct UFDBgentableSTA *  sta;
#if UFDB_DO_DEBUG
   struct UFDBgentableNode * origtable = node;
#endif

   /* there is a 2-level search (both binary search):
    * first, search the STA and second search within the STA.
    */

begin:
   DEBUG(( stderr, "    UFDBgentableLookupRevUrl:  table %-14s [%d]  tag %14s : %s\n", 
                   origtable->tag, origtable->nSTA, node->tag, revUrl->part ));

   if (node->nSTA == 0)
      return 0;

   i = 0;
   e = node->nSTA - 1;
   if (e == 0)
   {
      sta = &node->stas[0];
   }
   else
   {
      sta = NULL;       /* suppress compiler warning */
      b = 0;
      while (b <= e)
      {
	 i = (b + e) / 2;
	 sta = &node->stas[i];
	 cmp = strcmpURLpart( (char *) revUrl->part, (char *) sta->subNodes[0].tag );
	 DEBUG(( stderr, "      last node tag in STA %-3d (nnodes is %d)  %s \n", 
                         i, sta->nSubNodes, sta->subNodes[sta->nSubNodes-1].tag ));
	 DEBUG(( stderr, "      first node tag in STA %-3d  strcmpURLpart( %s, %s )  is %d\n", 
                         i, (char *) revUrl->part, (char *) sta->subNodes[0].tag, cmp ));
	 if (cmp < 0)
	    e = i - 1;
	 else if (cmp > 0)
	    b = i + 1;
	 else
	    break;
      }
      /* the above comparisons were done against sta[i].subnodes[0].tag and
       * we may be one STA too far so compare the tag with sta[-1].subnodes[last].tag
       */
      if (i > 0)
      {
         struct UFDBgentableSTA *  prevsta;
         prevsta = &node->stas[i-1];
         cmp = strcmpURLpart( (char *) revUrl->part, (char *) prevsta->subNodes[prevsta->nSubNodes-1].tag );
         if (cmp <= 0)
         {
            i = i - 1;
            sta = prevsta;
            DEBUG(( stderr, "      tag is in previous STA (%d)", i ));
         }
      }
   }
   DEBUG(( stderr, "      UFDBgentableLookupRevUrl  STA %d:  nSubNodes %d\n", i, sta->nSubNodes ));

   b = 0;
   e = sta->nSubNodes - 1;
   while (b <= e)
   {
      i = (b + e) / 2;
      cmp = strcmpURLpart( (char *) revUrl->part, (char *) sta->subNodes[i].tag );
      DEBUG(( stderr, "         i:%-3d  strcmpURLpart( %s, %s )  is %d\n", 
              i, (char *) revUrl->part, (char *) sta->subNodes[i].tag, cmp ));
      if (cmp < 0)
	 e = i - 1;
      else if (cmp > 0)
	 b = i + 1;
      else
      {
	 node = &(sta->subNodes[i]);
	 if (node->totalSubNodes == 0)		/* no more levels in table -> MATCH */
         {
            DEBUG(( stderr, "            no more levels in table -> MATCH\n" ));
	    return 1;
         }
	 revUrl = revUrl->next;
	 if (revUrl == NULL)			/* no more levels in URL -> NO match */
         {
            DEBUG(( stderr, "            no more levels in URL -> NO match\n" ));
	    return 0;
         }

	 goto begin;
      }
   }

   DEBUG(( stderr, "            not found\n" ));
   return 0;  /* not found */
}


static unsigned char * mem = NULL;
static unsigned long mem_i = 0;
static unsigned long mem_size = 0;

static void init_membuf( void )
{
   mem_size = 256 * 1024;
   mem = (unsigned char *) malloc( mem_size );

   /* starting with format 2.1 there are 64 random bytes inserted at the beginning 
    * of the crypted table as a defense against brute force decryption attempts.
    */
   if (doCrypt  &&  strcmp( format, "2.1") >= 0)
   {
      randomisebuf64( mem );
      mem_i += 64;
      if (UFDBglobalDebug)
	 fprintf( stderr, "inserted 64 random bytes at the start of the URL table\n" );
   }
}


UFDB_GCC_HOT 
static void mem_putc( unsigned char c )
{
   if (mem_i == mem_size)
   {
      if (mem_size < 1024 * 1024)
	 mem_size += 256 * 1024;
      else
	 mem_size += 2 * 1024 * 1024;
      mem = (unsigned char *) realloc( mem, mem_size );
      if (mem == NULL)
      {
         fprintf( stderr, "cannot allocate %lu bytes for the in-memory table.\n", mem_size );
	 exit( 1 );
      }
   }
   mem[mem_i++] = c;
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static void mem_pad( void )
{
   if (mem_i + 8 >= mem_size)
   {
      if (mem_size < 1024 * 1024)
	 mem_size += 256 * 1024;
      else
	 mem_size += 2 * 1024 * 1024;
      mem = (unsigned char *) realloc( mem, mem_size );
      if (mem == NULL)
      {
         fprintf( stderr, "cannot allocate %lu bytes for the in-memory table.\n", mem_size );
	 exit( 1 );
      }
   }

   while (mem_i % 8 > 0)
      mem[mem_i++] = UFDBpadTable;
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static void mem_puts( unsigned char * s )
{
   if (mem_i + sizeof(UFDBurlPart) >= mem_size)
   {
      if (mem_size < 1024 * 1024)
	 mem_size += 256 * 1024;
      else
	 mem_size += 2 * 1024 * 1024;
      mem = (unsigned char *) realloc( mem, mem_size );
      if (mem == NULL)
      {
         fprintf( stderr, "cannot allocate %lu bytes for the in-memory table.\n", mem_size );
	 exit( 1 );
      }
   }

   while (*s != '\0')
      mem[mem_i++] = *s++;
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static void mem_putsnt( unsigned char * s )
{
   if (mem_i + sizeof(UFDBurlPart) >= mem_size)
   {
      if (mem_size < 1024 * 1024)
	 mem_size += 256 * 1024;
      else
	 mem_size += 2 * 1024 * 1024;
      mem = (unsigned char *) realloc( mem, mem_size );
      if (mem == NULL)
      {
         fprintf( stderr, "cannot allocate %lu bytes for the in-memory table.\n", mem_size );
	 exit( 1 );
      }
   }

   while (*s != '\0')
      mem[mem_i++] = *s++;
   mem[mem_i++] = '\0';
}


#if 0
#define _STRDUP(s) ufdbStrdup(s)
#else
/* 
 *  Small speed optimisation: allocate memory for strdupped strings in large blocks since they are never freed.
 */
UFDB_GCC_HOT UFDB_GCC_INLINE
static char * _STRDUP( 
   char * s )
{
   static char * freeMem = NULL;
   static char * last = NULL;

   char * f;
   char * p;

   if ((int) (last - freeMem) < (int) sizeof(UFDBurlPart))
   {
      freeMem = (char *) malloc( 256 * 1024 );
      last = freeMem + 256 * 1024 - 1;
   }
   f = freeMem;

   p = (char *) memccpy( freeMem, s, '\0', sizeof(UFDBurlPart) );
   if (p == NULL)
   {
      freeMem += sizeof(UFDBurlPart);
      *freeMem++ = '\0';
   }
   else
      freeMem = p;

   return f;
}
#endif


UFDB_GCC_HOT
static int UFDBsanityCheckDomainname( char * url )
{
   unsigned char * s;
   char * first_slash;
   char * tld;
   int    retval;
   char * oldBracket;

#if 0
   fprintf( stderr, "UFDBsanityCheckDomainname: %s\n", url );
#endif

   if (*url == '[')			/* IPv6 address */
   {
      char normalisedDomain[64];

      oldBracket = strchr( url, ']' );
      if (oldBracket == NULL)
      {
         fprintf( stderr, "error: IPv6 address has no closing ']': %s\n", url );
	 return 0;
      }
      *oldBracket = '\0';
      if (UFDBparseIPv6address( url+1, normalisedDomain ) == NULL)
      {
	 *oldBracket = ']';
	 return 0;
      }
      else
      {
	 *oldBracket = ']';
	 UFDBupdateURLwithNormalisedDomain( url, normalisedDomain );
         return 1;
      }
   }

   first_slash = strchr( url, '/' );
   if (first_slash != NULL)
      *first_slash = '\0';

   tld = strrchr( url, '.' );
   if (tld == NULL)
      tld = url;
   else
      tld++;

   retval = 1;
   if (*tld >= '0'  &&  *tld <= '9')
      ;
   else
   if (
       strmatch2( tld, "ac" ) != 0  &&
       strmatch2( tld, "ad" ) != 0  &&
       strmatch3( tld, "ads" ) != 0  &&
       strmatch5( tld, "adult" ) != 0  &&
       strmatch2( tld, "ae" ) != 0  &&
       strmatch4( tld, "aero" ) != 0  &&
       strmatch2( tld, "af" ) != 0  &&
       strmatch2( tld, "ag" ) != 0  &&
       strmatch6( tld, "agency" ) != 0  &&
       strmatch2( tld, "ai" ) != 0  &&
       strmatch2( tld, "al" ) != 0  &&
       strmatch2( tld, "am" ) != 0  &&
       strmatch9( tld, "amsterdam" ) != 0  &&
       strmatch2( tld, "an" ) != 0  &&
       strmatch2( tld, "ao" ) != 0  &&
       strmatch3( tld, "app" ) != 0  &&
       strmatch5( tld, "apple" ) != 0  &&
       strmatch2( tld, "aq" ) != 0  &&
       strmatch2( tld, "ar" ) != 0  &&
       strmatch4( tld, "arpa" ) != 0  &&
       strmatch2( tld, "as" ) != 0  &&
       strmatch4( tld, "asia" ) != 0  &&
       strmatch2( tld, "at" ) != 0  &&
       strmatch2( tld, "au" ) != 0  &&
       strmatch5( tld, "audio" ) != 0  &&
       strmatch2( tld, "aw" ) != 0  &&
       strmatch2( tld, "ax" ) != 0  &&
       strmatch2( tld, "az" ) != 0  &&
       strmatch2( tld, "ba" ) != 0  &&
       strmatch6( tld, "bayern" ) != 0  &&
       strmatch2( tld, "bb" ) != 0  &&
       strmatch2( tld, "bd" ) != 0  &&
       strmatch2( tld, "be" ) != 0  &&
       strmatch6( tld, "berlin" ) != 0  &&
       strmatch2( tld, "bf" ) != 0  &&
       strmatch2( tld, "bg" ) != 0  &&
       strmatch2( tld, "bh" ) != 0  &&
       strmatch6( tld, "bharat" ) != 0  &&
       strmatch2( tld, "bi" ) != 0  &&
       strmatch4( tld, "bike" ) != 0  &&
       strmatch3( tld, "biz" ) != 0  &&
       strmatch2( tld, "bj" ) != 0  &&
       strmatch4( tld, "blue" ) != 0  &&
       strmatch2( tld, "bm" ) != 0  &&
       strmatch2( tld, "bn" ) != 0  &&
       strmatch10( tld, "bnpparibas" ) != 0  &&
       strmatch2( tld, "bo" ) != 0  &&
       strmatch2( tld, "br" ) != 0  &&
       strmatch8( tld, "bradesco" ) != 0  &&
       strmatch8( tld, "brussels" ) != 0  &&
       strmatch2( tld, "bs" ) != 0  &&
       strmatch2( tld, "bt" ) != 0  &&
       strmatch2( tld, "bv" ) != 0  &&
       strmatch2( tld, "bw" ) != 0  &&
       strmatch2( tld, "by" ) != 0  &&
       strmatch2( tld, "bz" ) != 0  &&
       strmatch3( tld, "bzh" ) != 0  &&
       strmatch2( tld, "ca" ) != 0  &&
       strmatch4( tld, "cafe" ) != 0  &&
       strmatch6( tld, "camera" ) != 0  &&
       strmatch6( tld, "casino" ) != 0  &&
       strmatch3( tld, "cat" ) != 0  &&
       strmatch2( tld, "cc" ) != 0  &&
       strmatch2( tld, "cd" ) != 0  &&
       strmatch6( tld, "center" ) != 0  &&
       strmatch2( tld, "cf" ) != 0  &&
       strmatch2( tld, "cg" ) != 0  &&
       strmatch2( tld, "ch" ) != 0  &&
       strmatch5( tld, "cheap" ) != 0  &&
       strmatch2( tld, "ci" ) != 0  &&
       strmatch2( tld, "ck" ) != 0  &&
       strmatch2( tld, "cl" ) != 0  &&
       strmatch4( tld, "club" ) != 0  &&
       strmatch2( tld, "cm" ) != 0  &&
       strmatch2( tld, "cn" ) != 0  &&
       strmatch2( tld, "co" ) != 0  &&
       strmatch5( tld, "coach" ) != 0  &&
       strmatch5( tld, "codes" ) != 0  &&
       strmatch3( tld, "com" ) != 0  &&
       strmatch7( tld, "company" ) != 0  &&
       strmatch4( tld, "coop" ) != 0  &&
       strmatch7( tld, "country" ) != 0  &&
       strmatch2( tld, "cr" ) != 0  &&
       strmatch7( tld, "cricket" ) != 0  &&
       strmatch2( tld, "cu" ) != 0  &&
       strmatch2( tld, "cu" ) != 0  &&
       strmatch2( tld, "cv" ) != 0  &&
       strmatch2( tld, "cx" ) != 0  &&
       strmatch2( tld, "cy" ) != 0  &&
       strmatch2( tld, "cz" ) != 0  &&
       strmatch6( tld, "dating" ) != 0  &&
       strmatch2( tld, "de" ) != 0  &&
       strmatch6( tld, "degree" ) != 0  &&
       strmatch4( tld, "desi" ) != 0  &&
       strmatch9( tld, "directory" ) != 0  &&
       strmatch2( tld, "dj" ) != 0  &&
       strmatch2( tld, "dk" ) != 0  &&
       strmatch2( tld, "dm" ) != 0  &&
       strmatch2( tld, "do" ) != 0  &&
       strmatch2( tld, "do" ) != 0  &&
       strmatch2( tld, "dz" ) != 0  &&
       strmatch2( tld, "ec" ) != 0  &&
       strmatch3( tld, "edu" ) != 0  &&
       strmatch2( tld, "ee" ) != 0  &&
       strmatch2( tld, "eg" ) != 0  &&
       strmatch5( tld, "email" ) != 0  &&
       strmatch2( tld, "er" ) != 0  &&
       strmatch2( tld, "es" ) != 0  &&
       strmatch2( tld, "et" ) != 0  &&
       strmatch2( tld, "eu" ) != 0  &&
       strmatch3( tld, "eus" ) != 0  &&
       strmatch6( tld, "expert" ) != 0  &&
       strmatch2( tld, "fi" ) != 0  &&
       strmatch7( tld, "fitness" ) != 0  &&
       strmatch2( tld, "fj" ) != 0  &&
       strmatch2( tld, "fk" ) != 0  &&
       strmatch2( tld, "fm" ) != 0  &&
       strmatch2( tld, "fm" ) != 0  &&
       strmatch2( tld, "fo" ) != 0  &&
       strmatch8( tld, "football" ) != 0  &&
       strmatch10( tld, "foundation" ) != 0  &&
       strmatch2( tld, "fr" ) != 0  &&
       strmatch2( tld, "fr" ) != 0  &&
       strmatch2( tld, "ga" ) != 0  &&
       strmatch7( tld, "gallery" ) != 0  &&
       strmatch2( tld, "gb" ) != 0  &&
       strmatch2( tld, "gd" ) != 0  &&
       strmatch2( tld, "ge" ) != 0  &&
       strmatch4( tld, "gent" ) != 0  &&
       strmatch2( tld, "gf" ) != 0  &&
       strmatch2( tld, "gg" ) != 0  &&
       strmatch2( tld, "gg" ) != 0  &&
       strmatch2( tld, "gh" ) != 0  &&
       strmatch2( tld, "gi" ) != 0  &&
       strmatch2( tld, "gi" ) != 0  &&
       strmatch2( tld, "gl" ) != 0  &&
       strmatch5( tld, "glass" ) != 0  &&
       strmatch2( tld, "gm" ) != 0  &&
       strmatch5( tld, "gmail" ) != 0  &&
       strmatch2( tld, "gn" ) != 0  &&
       strmatch4( tld, "golf" ) != 0  &&
       strmatch3( tld, "gov" ) != 0  &&
       strmatch2( tld, "gp" ) != 0  &&
       strmatch2( tld, "gq" ) != 0  &&
       strmatch2( tld, "gr" ) != 0  &&
       strmatch8( tld, "graphics" ) != 0  &&
       strmatch6( tld, "gratis" ) != 0  &&
       strmatch2( tld, "gs" ) != 0  &&
       strmatch2( tld, "gt" ) != 0  &&
       strmatch2( tld, "gt" ) != 0  &&
       strmatch2( tld, "gu" ) != 0  &&
       strmatch5( tld, "guide" ) != 0  &&
       strmatch4( tld, "guru" ) != 0  &&
       strmatch2( tld, "gw" ) != 0  &&
       strmatch2( tld, "gy" ) != 0  &&
       strmatch4( tld, "help" ) != 0  &&
       strmatch2( tld, "hk" ) != 0  &&
       strmatch2( tld, "hm" ) != 0  &&
       strmatch2( tld, "hn" ) != 0  &&
       strmatch8( tld, "holdings" ) != 0  &&
       strmatch4( tld, "host" ) != 0  &&
       strmatch3( tld, "how" ) != 0  &&
       strmatch2( tld, "hr" ) != 0  &&
       strmatch2( tld, "ht" ) != 0  &&
       strmatch2( tld, "hu" ) != 0  &&
       strmatch2( tld, "id" ) != 0  &&
       strmatch2( tld, "ie" ) != 0  &&
       strmatch2( tld, "il" ) != 0  &&
       strmatch2( tld, "im" ) != 0  &&
       strmatch2( tld, "in" ) != 0  &&
       strmatch4( tld, "info" ) != 0  &&
       strmatch3( tld, "ink" ) != 0  &&
       strmatch3( tld, "int" ) != 0  &&
       strmatch13( tld, "international" ) != 0  &&
       strmatch11( tld, "investments" ) != 0  &&
       strmatch2( tld, "io" ) != 0  &&
       strmatch2( tld, "iq" ) != 0  &&
       strmatch2( tld, "ir" ) != 0  &&
       strmatch2( tld, "is" ) != 0  &&
       strmatch2( tld, "it" ) != 0  &&
       strmatch2( tld, "je" ) != 0  &&
       strmatch5( tld, "jetzt" ) != 0  &&
       strmatch2( tld, "jm" ) != 0  &&
       strmatch2( tld, "jo" ) != 0  &&
       strmatch4( tld, "jobs" ) != 0  &&
       strmatch2( tld, "jp" ) != 0  &&
       strmatch6( tld, "kaufen" ) != 0  &&
       strmatch2( tld, "ke" ) != 0  &&
       strmatch2( tld, "kg" ) != 0  &&
       strmatch2( tld, "kh" ) != 0  &&
       strmatch2( tld, "ki" ) != 0  &&
       strmatch2( tld, "km" ) != 0  &&
       strmatch2( tld, "kn" ) != 0  &&
       strmatch5( tld, "koeln" ) != 0  &&
       strmatch2( tld, "kr" ) != 0  &&
       strmatch2( tld, "kw" ) != 0  &&
       strmatch2( tld, "ky" ) != 0  &&
       strmatch2( tld, "kz" ) != 0  &&
       strmatch2( tld, "la" ) != 0  &&
       strmatch4( tld, "land" ) != 0  &&
       strmatch2( tld, "lb" ) != 0  &&
       strmatch2( tld, "lc" ) != 0  &&
       strmatch2( tld, "li" ) != 0  &&
       strmatch4( tld, "link" ) != 0  &&
       strmatch2( tld, "lk" ) != 0  &&
       strmatch3( tld, "lol" ) != 0  &&
       strmatch6( tld, "london" ) != 0  &&
       strmatch4( tld, "love" ) != 0  &&
       strmatch2( tld, "lr" ) != 0  &&
       strmatch2( tld, "ls" ) != 0  &&
       strmatch2( tld, "lt" ) != 0  &&
       strmatch2( tld, "lu" ) != 0  &&
       strmatch2( tld, "lv" ) != 0  &&
       strmatch2( tld, "ly" ) != 0  &&
       strmatch2( tld, "ma" ) != 0  &&
       strmatch2( tld, "mc" ) != 0  &&
       strmatch2( tld, "md" ) != 0  &&
       strmatch2( tld, "me" ) != 0  &&
       strmatch5( tld, "media" ) != 0  &&
       strmatch8( tld, "memorial" ) != 0  &&
       strmatch4( tld, "menu" ) != 0  &&
       strmatch2( tld, "mg" ) != 0  &&
       strmatch2( tld, "mh" ) != 0  &&
       strmatch3( tld, "mil" ) != 0  &&
       strmatch2( tld, "mk" ) != 0  &&
       strmatch2( tld, "ml" ) != 0  &&
       strmatch2( tld, "mm" ) != 0  &&
       strmatch2( tld, "mn" ) != 0  &&
       strmatch2( tld, "mo" ) != 0  &&
       strmatch4( tld, "mobi" ) != 0  &&
       strmatch6( tld, "mobile" ) != 0  &&
       strmatch4( tld, "moda" ) != 0  &&
       strmatch3( tld, "moe" ) != 0  &&
       strmatch2( tld, "mp" ) != 0  &&
       strmatch2( tld, "mr" ) != 0  &&
       strmatch2( tld, "ms" ) != 0  &&
       strmatch2( tld, "mt" ) != 0  &&
       strmatch2( tld, "mt" ) != 0  &&
       strmatch2( tld, "mu" ) != 0  &&
       strmatch6( tld, "museum" ) != 0  &&
       strmatch5( tld, "music" ) != 0  &&
       strmatch2( tld, "mv" ) != 0  &&
       strmatch2( tld, "mw" ) != 0  &&
       strmatch2( tld, "mx" ) != 0  &&
       strmatch2( tld, "my" ) != 0  &&
       strmatch2( tld, "mz" ) != 0  &&
       strmatch2( tld, "mz" ) != 0  &&
       strmatch2( tld, "na" ) != 0  &&
       strmatch4( tld, "name" ) != 0  &&
       strmatch2( tld, "nc" ) != 0  &&
       strmatch2( tld, "ne" ) != 0  &&
       strmatch3( tld, "net" ) != 0  &&
       strmatch7( tld, "network" ) != 0  &&
       strmatch2( tld, "nf" ) != 0  &&
       strmatch2( tld, "ng" ) != 0  &&
       strmatch3( tld, "ngo" ) != 0  &&
       strmatch2( tld, "ni" ) != 0  &&
       strmatch2( tld, "ni" ) != 0  &&
       strmatch5( tld, "ninja" ) != 0  &&
       strmatch2( tld, "nl" ) != 0  &&
       strmatch2( tld, "no" ) != 0  &&
       strmatch2( tld, "np" ) != 0  &&
       strmatch2( tld, "nr" ) != 0  &&
       strmatch3( tld, "nrw" ) != 0  &&
       strmatch2( tld, "nu" ) != 0  &&
       strmatch2( tld, "nu" ) != 0  &&
       strmatch2( tld, "nz" ) != 0  &&
       strmatch2( tld, "om" ) != 0  &&
       strmatch3( tld, "ong" ) != 0  &&
       strmatch3( tld, "ooo" ) != 0  &&
       strmatch3( tld, "org" ) != 0  &&
       strmatch3( tld, "ovh" ) != 0  &&
       strmatch2( tld, "pa" ) != 0  &&
       strmatch5( tld, "paris" ) != 0  &&
       strmatch5( tld, "party" ) != 0  &&
       strmatch2( tld, "pe" ) != 0  &&
       strmatch2( tld, "pf" ) != 0  &&
       strmatch2( tld, "pg" ) != 0  &&
       strmatch2( tld, "ph" ) != 0  &&
       strmatch8( tld, "pharmacy" ) != 0  &&
       strmatch5( tld, "photo" ) != 0  &&
       strmatch6( tld, "photos" ) != 0  &&
       strmatch4( tld, "pics" ) != 0  &&
       strmatch4( tld, "pink" ) != 0  &&
       strmatch2( tld, "pk" ) != 0  &&
       strmatch2( tld, "pl" ) != 0  &&
       strmatch2( tld, "pm" ) != 0  &&
       strmatch2( tld, "pn" ) != 0  &&
       strmatch5( tld, "poker" ) != 0  &&
       strmatch4( tld, "post" ) != 0  &&
       strmatch2( tld, "pr" ) != 0  &&
       strmatch5( tld, "press" ) != 0  &&
       strmatch3( tld, "pro" ) != 0  &&
       strmatch2( tld, "ps" ) != 0  &&
       strmatch2( tld, "pt" ) != 0  &&
       strmatch3( tld, "pub" ) != 0  &&
       strmatch2( tld, "pw" ) != 0  &&
       strmatch2( tld, "py" ) != 0  &&
       strmatch2( tld, "py" ) != 0  &&
       strmatch2( tld, "qa" ) != 0  &&
       strmatch2( tld, "re" ) != 0  &&
       strmatch3( tld, "red" ) != 0  &&
       strmatch7( tld, "reviews" ) != 0  &&
       strmatch10( tld, "restaurant" ) != 0  &&
       strmatch2( tld, "ro" ) != 0  &&
       strmatch5( tld, "rocks" ) != 0  &&
       strmatch2( tld, "rs" ) != 0  &&
       strmatch2( tld, "ru" ) != 0  &&
       strmatch4( tld, "ruhr" ) != 0  &&
       strmatch2( tld, "rw" ) != 0  &&
       strmatch2( tld, "sa" ) != 0  &&
       strmatch2( tld, "sb" ) != 0  &&
       strmatch2( tld, "sc" ) != 0  &&
       strmatch7( tld, "science" ) != 0  &&
       strmatch6( tld, "school" ) != 0  &&
       strmatch4( tld, "scot" ) != 0  &&
       strmatch2( tld, "sd" ) != 0  &&
       strmatch2( tld, "se" ) != 0  &&
       strmatch4( tld, "sexy" ) != 0  &&
       strmatch2( tld, "sg" ) != 0  &&
       strmatch2( tld, "sh" ) != 0  &&
       strmatch2( tld, "si" ) != 0  &&
       strmatch7( tld, "singles" ) != 0  &&
       strmatch2( tld, "sj" ) != 0  &&
       strmatch2( tld, "sk" ) != 0  &&
       strmatch3( tld, "ski" ) != 0  &&
       strmatch2( tld, "sl" ) != 0  &&
       strmatch2( tld, "sm" ) != 0  &&
       strmatch2( tld, "sn" ) != 0  &&
       strmatch2( tld, "so" ) != 0  &&
       strmatch6( tld, "social" ) != 0  &&
       strmatch5( tld, "sport" ) != 0  &&
       strmatch2( tld, "sr" ) != 0  &&
       strmatch2( tld, "st" ) != 0  &&
       strmatch2( tld, "su" ) != 0  &&
       strmatch2( tld, "sv" ) != 0  &&
       strmatch2( tld, "sx" ) != 0  &&
       strmatch2( tld, "sy" ) != 0  &&
       strmatch7( tld, "systems" ) != 0  &&
       strmatch2( tld, "sz" ) != 0  &&
       strmatch2( tld, "tc" ) != 0  &&
       strmatch2( tld, "td" ) != 0  &&
       strmatch4( tld, "tech" ) != 0  &&
       strmatch10( tld, "technology" ) != 0  &&
       strmatch3( tld, "tel" ) != 0  &&
       strmatch2( tld, "tf" ) != 0  &&
       strmatch2( tld, "tg" ) != 0  &&
       strmatch2( tld, "th" ) != 0  &&
       strmatch4( tld, "tips" ) != 0  &&
       strmatch5( tld, "tirol" ) != 0  &&
       strmatch2( tld, "tj" ) != 0  &&
       strmatch2( tld, "tk" ) != 0  &&
       strmatch2( tld, "tl" ) != 0  &&
       strmatch2( tld, "tm" ) != 0  &&
       strmatch2( tld, "tn" ) != 0  &&
       strmatch2( tld, "to" ) != 0  &&
       strmatch5( tld, "today" ) != 0  &&
       strmatch3( tld, "top" ) != 0  &&
       strmatch2( tld, "tp" ) != 0  &&
       strmatch2( tld, "tr" ) != 0  &&
       strmatch8( tld, "training" ) != 0  &&
       strmatch6( tld, "travel" ) != 0  &&
       strmatch2( tld, "tt" ) != 0  &&
       strmatch2( tld, "tv" ) != 0  &&
       strmatch2( tld, "tw" ) != 0  &&
       strmatch2( tld, "tz" ) != 0  &&
       strmatch2( tld, "ua" ) != 0  &&
       strmatch2( tld, "ug" ) != 0  &&
       strmatch2( tld, "uk" ) != 0  &&
       strmatch2( tld, "um" ) != 0  &&
       strmatch3( tld, "uno" ) != 0  &&
       strmatch2( tld, "us" ) != 0  &&
       strmatch2( tld, "uy" ) != 0  &&
       strmatch2( tld, "uz" ) != 0  &&
       strmatch2( tld, "va" ) != 0  &&
       strmatch2( tld, "vc" ) != 0  &&
       strmatch2( tld, "ve" ) != 0  &&
       strmatch5( tld, "vegas" ) != 0  &&
       strmatch12( tld, "versicherung" ) != 0  &&
       strmatch2( tld, "vg" ) != 0  &&
       strmatch2( tld, "vi" ) != 0  &&
       strmatch10( tld, "vlaanderen" ) != 0  &&
       strmatch2( tld, "vn" ) != 0  &&
       strmatch6( tld, "voyage" ) != 0  &&
       strmatch2( tld, "vu" ) != 0  &&
       strmatch4( tld, "wang" ) != 0  &&
       strmatch5( tld, "wales" ) != 0  &&
       strmatch6( tld, "webcam" ) != 0  &&
       strmatch7( tld, "website" ) != 0  &&
       strmatch2( tld, "wf" ) != 0  &&
       strmatch7( tld, "whoswho" ) != 0  &&
       strmatch4( tld, "wifi" ) != 0  &&
       strmatch4( tld, "wiki" ) != 0  &&
       strmatch4( tld, "work" ) != 0  &&
       strmatch5( tld, "world" ) != 0  &&
       strmatch4( tld, "wpad" ) != 0  &&
       strmatch2( tld, "ws" ) != 0  &&
       strmatch3( tld, "wtf" ) != 0  &&
       strmatch4( tld, "xn--" ) != 0  &&		/* e.g. xn--p1ai */
       strmatch3( tld, "xxx" ) != 0  &&
       strmatch3( tld, "xyz" ) != 0  &&
       strmatch2( tld, "ye" ) != 0  &&
       strmatch2( tld, "yt" ) != 0  &&
       strmatch2( tld, "yu" ) != 0  &&
       strmatch2( tld, "za" ) != 0  &&
       strmatch2( tld, "zm" ) != 0  &&
       strmatch4( tld, "zone" ) != 0  &&
       strmatch2( tld, "zw" ) != 0 
       )
   {
      if (!doWarnings)
	 fprintf( stderr, "warning: possibly incorrect domain name: %s\n", url );
      retval = 1;
   }

   /* allowed characters:
    * 0 - 9
    * A - Z    a - z
    * [ ] :
    * - . % 
    * illegal but common :
    * _
    * RFC3986 is superceded by RFC5890-5895
    * According to the new RFCs labels can contain UTF characters
    */
   for (s = (unsigned char *) url;  *s != '\0';  s++)
   {
      if (!( (*s >= 'a' && *s <= 'z')  ||
             (*s >= 'A' && *s <= 'Z')  ||
	     (*s >= '0' && *s <= '9')  ||
	     (*s == '.'  ||  *s == '-'  ||  *s == ':'  ||  *s == '['  ||  *s == ']'  ||  *s == '%')  ||
	     (*s == '_')  || *s >= 0x80 ))
      {
         fprintf( stderr, "error: domainname has illegal characters: %s\n", url );
         retval = 0;
	 break;
      }
   }

   if (first_slash != NULL)
      *first_slash = '/';

   return retval;
}


void initTable( char * tableName )
{
   table = (struct UFDBgentableNode *) malloc( sizeof( struct UFDBgentableNode ) );
   table->tag = (unsigned char *) _STRDUP( tableName );
   table->totalSubNodes = 0;
   table->nSTA = 0;
   table->stas = NULL;
   numNodes++;

   numIndexNodes = 0;
}


#if 0
static UFDB_GCC_MALLOC_ATTR UFDB_GCC_INLINE void * _trealloc( void * p, int n )
{
   int nup;

   if (n == 2)
      return realloc( p, ROUNDUP(2) * sizeof(struct UFDBgentableNode) );

   if (n < ROUNDUPBYCUTOFF)
   {
      nup = ROUNDUP(n);
      if (nup == ROUNDUP(n-1))
         return p;
   }
   else
   {
      nup = BIGROUNDUP(n);
      if (nup == BIGROUNDUP(n-1))
         return p;
   }

   return realloc( p, nup * sizeof(struct UFDBgentableNode) );
}
#endif


#if UFDB_OVERRIDE_GCC_OPT  &&  ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC push_options
#pragma GCC optimize ("O3")
/* do not use loop unrolling since it is slower */
#endif


/*
 * UFDBinsertURL: insert revURL into table t.
 * return values: 1 if revURL already exists, 0 if revURL was inserted.
 */
UFDB_GCC_HOT
static int UFDBinsertURL( 
   struct UFDBgentableNode * node, 
   UFDBrevURL *              revURL, 
   char *                    origURL,
   UFDBurlType               type )
{
   /*
    * find the index where our URL has to be inserted before or is equal to
    * e.g. the level "net" is either "< nl" or "= net".
    */
   int  cmp;
   int  sta_i, i, j;
   int  b, e;
   int  rv;
   int  tailInsert;
   struct UFDBgentableSTA * sta;

   tailInsert = rv = 0;
   cmp = 0;
   i = 0;
   sta_i = -1;

nextlevel:
   DEBUG(( stderr, "UFDBinsertURL( 0x%08lx, 0x%08lx, %d )  tag='%s' \n", 
           (long) node, (long) revURL, type, node != NULL ? (char *) node->tag : "---" ));

   if (revURL == NULL)
   {
      if (node != NULL)
      {
         DEBUG(( stderr, "   revURL=NULL t: totalSubNodes=%d, nSTA=%d  tag='%s'\n", 
	         node->totalSubNodes, node->nSTA, node->tag ));
	 if (node->totalSubNodes > 0)
	 {
	    /* interesting... we are trying to insert "xxx.com" while the tree already
	     * has one or more members with subdomains like sss.xxx.com.
	     * Lets optimise this and get rid of the subdomains !
	     */
            DEBUG(( stderr, "   inserted URL has subdomains, first subdomain is '%s'\n", 
                    node->stas->subNodes[0].tag ));
	    if (!skipOptimisations)
	    {
               DEBUG(( stderr, "   removing subdomains of node '%s'\n", node->tag ));
	       rv = 1;
	       node->totalSubNodes = 0;
	       node->nSTA = 0;
	       free( node->stas );		/* TO-DO: should free() a tree ! */
	       node->stas = NULL;
	    }
	 }
      }
      else
         DEBUG(( stderr, "        revURL=NULL node=NULL\n" ));
      return rv;
   }

   /* there is a 2-level insert:
    * first find the appropriate STA for the insert and then insert the revURL into nodes[].
    * If the STA is full, split it into 2 STAs.
    *
    * if the input file is already mostly sorted, almost all insertions take place at the end.
    * So lets optimise this by first looking at the end before doing a binary search.
    *
    * We may have to delete URLs is we are inserting a URL with a shorter path and
    * URLs with a longer path (and same path as the one being inserted) exists.
    */

   if (node->nSTA == 0)		/* the very first entry at this level */
   {
      DEBUG(( stderr, "   UFDBinsertURL nSTA=0 creating first STA for node '%s' and subNode '%s'\n", 
	      node->tag, (revURL==NULL ? (unsigned char *)"NULL" : revURL->part) ));

      numNodes++;
      node->totalSubNodes = 1;
      node->nSTA = 1;
      node->stas = (struct UFDBgentableSTA *) malloc( 1 * sizeof(struct UFDBgentableSTA) );
      sta = node->stas;
      sta->nSubNodes = 1;
      sta->nodeArraySize = MinNodesPerSTA;
      sta->subNodes = (struct UFDBgentableNode *) malloc( MinNodesPerSTA * sizeof(struct UFDBgentableNode) );
      sta->subNodes->totalSubNodes = 0;
      sta->subNodes->nSTA = 0;
      sta->subNodes->stas = NULL;
      sta->subNodes->tag = (unsigned char *) _STRDUP( (char *) revURL->part );

      node = &( sta->subNodes[0] );  
      revURL = revURL->next;  
      DEBUG(( stderr, "   UFDBinsertURL going to nextlevel\n" ));
      goto nextlevel;
   }

   /* check for tail insert: compare with last element of last STA */
   sta_i = node->nSTA - 1;
   sta = &( node->stas[sta_i] );
   i = sta->nSubNodes - 1;
   DEBUG(( stderr, "   going to insert '%s' and last node of the last STA(0x%08lx:%d:%d) is '%s'\n", 
           (char *) revURL->part, (long) sta, sta_i, sta->nSubNodes, (char *) sta->subNodes[i].tag ));

   cmp = strcmpURLpart( (char *) revURL->part, (char *) sta->subNodes[i].tag );
   DEBUG(( stderr, "      strcmpURLpart( %s, %s ) = %d\n", (char *) revURL->part, 
                   (char *) sta->subNodes[i].tag, cmp ));
   if (cmp == 0)		/* node match; there will be no insert at this level */
      goto node_match;
   if (cmp > 0)			/* it is a tail insert; we are done with the search for STA and STA index */
   {
      tailInsert = 1;
      goto sta_found;
   }

   /* there is no tail insert, so start with finding the right STA */
   e = node->nSTA - 1;
   if (e == 0)
      goto sta_found;	/* there is only one STA; sta and sta_i are already set */
   b = 0;
   while (b <= e)
   {
      sta_i = (b + e) / 2;
      sta = &( node->stas[sta_i] );
      i = 0;
      cmp = strcmpURLpart( (char *) revURL->part, (char *) sta->subNodes[i].tag );
      if (cmp < 0)
	 e = sta_i - 1;
      else if (cmp == 0)
         goto node_match;
      else /* cmp>0 */
      {
	 i = sta->nSubNodes - 1;
	 cmp = strcmpURLpart( (char *) revURL->part, (char *) sta->subNodes[i].tag );
	 if (cmp == 0)
	    goto node_match;
	 if (cmp < 0)
	    break;
	 b = sta_i + 1;
      }
   }
sta_found:
   DEBUG(( stderr, "      inserting in STA sta_i=%d sta=0x%08lx\n", sta_i, (long) sta ));

   /* The STA is found, check if an enlargement or a split is necessary */
   if (sta->nSubNodes == MaxNodesPerSTA)
   {
      struct UFDBgentableSTA * newsta;

      /* split an STA into two STAs with both MaxNodesPerSTA subNodes.
       * The existing subNodes are divided 50/50 amongst the current STA and the new STA
       * EXCEPT when the current STA is the last STA due to very frequent tail insertion.
       */
      newsta = (struct UFDBgentableSTA *) malloc( sizeof(struct UFDBgentableSTA) );
      newsta->nodeArraySize = MaxNodesPerSTA;
      newsta->subNodes = (struct UFDBgentableNode *) malloc( MaxNodesPerSTA * sizeof(struct UFDBgentableNode) );
      if (sta_i == node->nSTA - 1)		/* is the STA the last one of this node ? */
      {
	 DEBUG(( stderr, "++ splitting last STA sta_i=%d\n", sta_i ));
	 /* only transfer one subNode to the new STA */
	 newsta->nSubNodes = 1;
	 newsta->subNodes[0] = sta->subNodes[MaxNodesPerSTA-1];
	 sta->nSubNodes--;
	 /* insert the new STA in the node (at the end) */
	 node->nSTA++;
	 node->stas = (struct UFDBgentableSTA *) 
                         realloc( node->stas, node->nSTA * sizeof(struct UFDBgentableSTA) );
	 node->stas[node->nSTA-1] = *newsta;
	 /* must reassign sta because node->stas is realloced */
	 sta = &node->stas[sta_i];
	 newsta = &node->stas[node->nSTA-1];
      }
      else					/* not the last STA of this node */
      {
	 DEBUG(( stderr, "++ splitting STA sta_i=%d\n", sta_i ));
	 /* divide subNodes 50/50 */
	 newsta->nSubNodes = MaxNodesPerSTA/2;
	 for (i = 0; i < MaxNodesPerSTA/2; i++)
	 {
	    newsta->subNodes[i] = sta->subNodes[i + MaxNodesPerSTA/2];
	 }
	 sta->nSubNodes -= MaxNodesPerSTA/2;
	 /* insert the new STA in the node (not at the end) */
	 node->nSTA++;
	 node->stas = (struct UFDBgentableSTA *) 
                         realloc( node->stas, node->nSTA * sizeof(struct UFDBgentableSTA) );
	 for (j = node->nSTA-1; j > sta_i+1; j--)
	    node->stas[j] = node->stas[j-1];
	 node->stas[sta_i+1] = *newsta;
	 /* must reassign sta because node->stas is realloced */
	 sta = &node->stas[sta_i];
	 newsta = &node->stas[sta_i+1];
      }

      /* determine if sta has to point to the new STA */
      i = 0;
      cmp = strcmpURLpart( (char *) revURL->part, (char *) newsta->subNodes[i].tag );
      if (cmp >= 0)
         sta = newsta;
      if (cmp == 0)
         goto node_match;
   }
   else if (sta->nSubNodes >= sta->nodeArraySize)
   {
      /* extend STA */
      unsigned int  newSize;
      if (sta->nodeArraySize == 0)
         newSize = 1;
      else if (sta->nodeArraySize == 1)
         newSize = 8;
      else if (sta->nodeArraySize == 8)
         newSize = 256;
      else
         newSize = MaxNodesPerSTA;
      DEBUG(( stderr, "++ sta->nSubNodes is %d, extending subNodes array from %d to %d elements\n", 
              sta->nSubNodes, sta->nodeArraySize, newSize ));
      sta->nodeArraySize = newSize;
      sta->subNodes = (struct UFDBgentableNode *) 
                         realloc( sta->subNodes, newSize * sizeof(struct UFDBgentableNode) );
   }

   /* the STA is found, subNodes is guaranteed to have space for one more element; search now inside this STA */
   if (tailInsert)
   {
      i = sta->nSubNodes - 1;
      cmp = 1;
   }
   else
   {
      b = 0;
      e = sta->nSubNodes - 1;
      while (b <= e)			/* binary search STA */
      {
	 i = (b + e) / 2;
	 cmp = strcmpURLpart( (char *) revURL->part, (char *) sta->subNodes[i].tag );
	 if (cmp < 0)
	    e = i - 1;
	 else if (cmp > 0)
	    b = i + 1;
	 else
	    goto node_match;
      }
      DEBUG(( stderr, "         NOTFOUND after bsearch in STA: part='%s'  cmp=%d  i=%d  b=%d  e=%d  totalSubNodes=%d\n", 
	      (revURL==NULL ? (unsigned char *)"NULL" : revURL->part), cmp, i, b, e, node->totalSubNodes ));
   }

   if (cmp < 0  &&  revURL != NULL  &&  revURL->part[0] == '/')
   {
      size_t partlen = strlen( (char*) revURL->part );

      if (strncmp( (char*) revURL->part, (char*) sta->subNodes[i].tag, partlen ) == 0)
      { 
         ufdbLogMessage( "URL with longer path is already in the table:\n"
                         "   inserting URL %s\n"
                         "   removing URL with path %s", 
                         origURL, sta->subNodes[i].tag );
         node = &sta->subNodes[i];
         node->tag[partlen] = '\0';     /* truncate the longer path effectively inserting the shorter path */
         if (node->totalSubNodes > 0)
         {
            /* the longer path most likely has no children but remove them if they exist */
            node->totalSubNodes = 0;
            node->nSTA = 0;
            free( node->stas );		/* TO-DO: should free() a tree ! */
            node->stas = NULL;
         }

         /* remove other longer paths */
         i++;
         while (i < sta->nSubNodes  &&
                strncmp( (char*) revURL->part, (char*) sta->subNodes[i].tag, partlen ) == 0)
         {
            ufdbLogMessage( "   also removing URL with path %s", sta->subNodes[i].tag );
            for (j = i + 1;  j < sta->nSubNodes;  j++)
               sta->subNodes[j-1] = sta->subNodes[j];
            sta->nSubNodes--;
         }

         /* abort the insertion */
         return 1;
      }
   }

   node->totalSubNodes++;
   DEBUG(( stderr, "         totalsubnodes of \"%s\" incremented to %d\n", node->tag, node->totalSubNodes ));
   numNodes++;

   /* implemented optimisations: 
    * do not add subdom.abc.com/aurl if abc.com is already in the tree
    * do not add subdom.abc.com if abc.com is already in the tree
    * remove subdom.abc.com from tree if abc.com is being inserted
    */

   if (cmp > 0)					/* this entry > subNodes[i] */
   {
      DEBUG(( stderr, "         cmp>0 after bsearch: part='%s'  tag='%s'  i=%d  nSubNodes=%d  shifting %d\n", 
	      (revURL==NULL ? (unsigned char *)"NULL" : revURL->part),
              sta->subNodes[i].tag, i, sta->nSubNodes, sta->nSubNodes - 1 - i ));

      i++;
      
      sta->nSubNodes++;

      /* make space in the array */
      for (j = sta->nSubNodes - 1;  j > i;  j--)
	 sta->subNodes[j] = sta->subNodes[j-1];

      /* insert the current revURL into the array */
      sta->subNodes[i].totalSubNodes = 0;
      sta->subNodes[i].nSTA = 0;
      sta->subNodes[i].stas = NULL;
      sta->subNodes[i].tag = (unsigned char *) _STRDUP( (char *) revURL->part );

      /* process the tail of revURL */
      node = &( sta->subNodes[i] );  
      revURL = revURL->next;  
      goto nextlevel;
   }
   else if (cmp < 0)				/* this entry < subNodes[i] */
   {
      DEBUG(( stderr, "      cmp<0 after bsearch: part='%s'  tag='%s'  i=%d  nSubNodes=%d  shifting %d\n", 
	      (revURL==NULL ? (unsigned char *)"NULL" : revURL->part),
              sta->subNodes[i].tag, i, sta->nSubNodes, sta->nSubNodes - 1 - i ));

      sta->nSubNodes++;

      /* make space in the array */
      for (j = sta->nSubNodes - 1;  j > i;  j--)
	 sta->subNodes[j] = sta->subNodes[j-1];

      /* insert the current revURL into the array */
      sta->subNodes[i].totalSubNodes = 0;
      sta->subNodes[i].nSTA = 0;
      sta->subNodes[i].stas = NULL;
      sta->subNodes[i].tag = (unsigned char *) _STRDUP( (char *) revURL->part );

      /* process the tail of revURL */
      node = &( sta->subNodes[i] );  
      revURL = revURL->next;  
      goto nextlevel;
   }
   else if (cmp == 0)				/* an exact match at this level */
   {
node_match:
      DEBUG(( stderr, "         cmp==0 after bsearch: part='%s'  nSubNodes=%d  i=%d\n", 
	      (revURL==NULL ? (unsigned char *)"NULL" : revURL->part),
              sta->nSubNodes, i ));

      /* do not add revURL example.com/foo if example.com is in the table */
      if (type == UFDBurl)
      {
         if (skipOptimisations  ||  sta->subNodes[i].totalSubNodes != 0)
	 {
	    node = &( sta->subNodes[i] );  
	    revURL = revURL->next;  
	    goto nextlevel;
	 }
      }
      else
      {
	 node = &( sta->subNodes[i] );  
	 revURL = revURL->next;  
	 DEBUG(( stderr, "               node match: going to nextlevel\n" ));
	 goto nextlevel;
      }
   }

   return rv;
}

#if UFDB_OVERRIDE_GCC_OPT && ((__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4))
#pragma GCC pop_options
#endif


/* Generate a binary table file format v1.2
 */
void createMemTable_1_2( struct UFDBgentableNode * node )
{
   int    i, j;
   struct UFDBgentableSTA * sta;

   mem_puts( node->tag );

   if (node->totalSubNodes == 0)
      numLeafNodes++;
   if (node->totalSubNodes > 0)
   {
      mem_putc( UFDBsubLevel );
      numSub1++;
      if (node->totalSubNodes > 15000000)
	 fprintf( stderr, "**** LARGE number of subnodes: %d for node %s\n", node->totalSubNodes, node->tag );
   }

   for (i = 0; i < node->nSTA; i++)
   {
      sta = &node->stas[i];
      for (j = 0;  j < sta->nSubNodes;  j++)
      {
	 createMemTable_1_2( &sta->subNodes[j] );

	 if (sta->subNodes[j].totalSubNodes == 0)
	 {
	    if (i != node->nSTA - 1  ||  j != sta->nSubNodes - 1)
	       mem_putc( UFDBsameLevel );
	 }
	 else
	    mem_putc( UFDBprevLevel );
      }
   }
}


#if 0
static void calcIndexSize( struct UFDBgentableNode * node )
{
   int i, j;
   struct UFDBgentableSTA * sta;

   numNodes++;
   if (node->totalSubNodes == 0)
      numLeafNodes++;

   for (i = 0; i < node->nSTA; i++)
   {
      sta = &node->stas[i];
      for (j = 0; j < sta->nSubNodes; j++)
	 calcIndexSize( &sta->subNodes[i] );
   }
}
#endif


/* generate a binary table file, database table format 2.0
 */
void createMemTable_2_0( struct UFDBgentableNode * node )
{
   int i, j;
   struct UFDBgentableSTA * sta;

   DEBUG(( stderr, "tag=%s  totalSubNodes=%d #STAs=%d\n", node->tag, node->totalSubNodes, node->nSTA ));
   mem_puts( node->tag );

   if (node->totalSubNodes > 0)
   {
      DEBUG(( stderr, "sublevel " ));
      mem_putc( UFDBsubLevel );

      /* write the number of subnodes in a 1-byte or 4-byte code */
      if (node->totalSubNodes <= 255)
      {
	 DEBUG(( stderr, "#1byte-subNodes=%d\n", node->totalSubNodes ));
         mem_putc( node->totalSubNodes );
	 numSub7++;
      }
      else
      {
	 DEBUG(( stderr, "#4byte-subNodes=%d\n", node->totalSubNodes ));
         mem_putc( 0 );
         i = node->totalSubNodes;
	 if (i >= 256*256)
	    numSub64Kplus++;
	 else
	    numSub255plus++;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 if (i > 32 && doWarnings)
	    fprintf( stderr, "Warning: LARGE number of subnodes: %d for tag %s\n", node->totalSubNodes, node->tag );
      }
      DEBUG(( stderr, "      tag = %-18s sub-level   %d subnode(s)\n", node->tag, node->totalSubNodes ));
      if (UFDBglobalDebug > 1)
      {
         ufdbLogMessage( "node '%s' has %d subnodes in %d STAs", node->tag, node->totalSubNodes, node->nSTA );
	 if (node->nSTA > 1)
	    for (i = 0; i < node->nSTA; i++)
	       ufdbLogMessage( "   STA %d has %d subnodes", i, node->stas[i].nSubNodes );
      }
   }
   else
   {
      numLeafNodes++;
      DEBUG(( stderr, "      tag = %-18s leaf (no-next-level)\n", node->tag ));
   }

   for (i = 0; i < node->nSTA; i++)
   {
      sta = &node->stas[i];
      for (j = 0; j < sta->nSubNodes; j++)
      {
	 DEBUG(( stderr, "recursive-call STA %d subnode %d\n", i, j ));
	 createMemTable_2_0( &sta->subNodes[j] );

	 if (sta->subNodes[j].totalSubNodes == 0)
	 {
	    if (i != node->nSTA - 1  ||  j != sta->nSubNodes - 1)
	    {
	       DEBUG(( stderr, "samelevel\n" ));
	       mem_putc( UFDBsameLevel );
	    }
	 }
	 else
	 {
	    DEBUG(( stderr, "prevlevel\n" ));
	    mem_putc( UFDBprevLevel );
	 }
      }
   }
}


/* generate a binary table file, database table format 2.1
 */
UFDB_GCC_HOT
void createMemTable_2_1( struct UFDBgentableNode * node )
{
   int        i, j;
   static int indent = 0;
   struct UFDBgentableSTA * sta;

   if (doPadding)
      mem_pad();
   mem_puts( node->tag );

   if (node->totalSubNodes == 1)
   {
      mem_putc( UFDBsubLevel1 );
      numSub1++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   1 subnode\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 2)
   {
      mem_putc( UFDBsubLevel2 );
      numSub2++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   2 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 3)
   {
      mem_putc( UFDBsubLevel3 );
      numSub3++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   3 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 4)
   {
      mem_putc( UFDBsubLevel4 );
      numSub4++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   4 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 5)
   {
      mem_putc( UFDBsubLevel5 );
      numSub5++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   5 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 6)
   {
      mem_putc( UFDBsubLevel6 );
      numSub6++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   6 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 7)
   {
      mem_putc( UFDBsubLevel7 );
      numSub7++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   7 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes > 0)
   {
      /* write the number of subnodes in a 2-byte, 3-byte or 5-byte code */
      if (node->totalSubNodes < 256)
      {
	 mem_putc( UFDBsubLevel );
         mem_putc( node->totalSubNodes );		/* between 8 and 255 */
	 numSub8plus++;
      }
      else if (node->totalSubNodes < 256*256)
      {
	 mem_putc( UFDBsubLevelNNN );			/* more than 255 and less than 65536 */
         i = node->totalSubNodes;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 numSub255plus++;
      }
      else
      {
	 mem_putc( UFDBsubLevelNNNNN );			/* more than 65535 */
         i = node->totalSubNodes;
	 if (doWarnings  &&  i > 15000000)
	    fprintf( stderr, "LARGE number of subnodes: %d for node %s  *****\n", i, node->tag );
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 numSub64Kplus++;
      }
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   %d subnodes\n", indent, "", node->tag, node->totalSubNodes ));
   }
   else
   {
      numLeafNodes++;
      DEBUG(( stderr, "%*s      tag = %-18s leaf node\n", indent, "", node->tag ));
   }

   for (i = 0; i < node->nSTA; i++)
   {
      sta = &node->stas[i];
      for (j = 0; j < sta->nSubNodes; j++)
      {
	 indent += 3;
	 createMemTable_2_1( &(sta->subNodes[j]) );
	 indent -= 3;

	 if (sta->subNodes[j].totalSubNodes == 0)
	 {
	    if (i != node->nSTA - 1  ||  j != sta->nSubNodes - 1)
	       mem_putc( UFDBsameLevel );
	 }
	 else
	    mem_putc( UFDBprevLevel );
      }
   }
}


/* generate a binary table file, database table format 2.2
 */
UFDB_GCC_HOT
void createMemTable_2_2( struct UFDBgentableNode * node )
{
   int        i, j;
   static int indent = 0;
   struct UFDBgentableSTA * sta;

   if (doPadding)
      mem_pad();
   mem_putsnt( node->tag );

   if (node->totalSubNodes == 1)
   {
      mem_putc( UFDBsubLevel1 );
      numSub1++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   1 subnode\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 2)
   {
      mem_putc( UFDBsubLevel2 );
      numSub2++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   2 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 3)
   {
      mem_putc( UFDBsubLevel3 );
      numSub3++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   3 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 4)
   {
      mem_putc( UFDBsubLevel4 );
      numSub4++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   4 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 5)
   {
      mem_putc( UFDBsubLevel5 );
      numSub5++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   5 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 6)
   {
      mem_putc( UFDBsubLevel6 );
      numSub6++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   6 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes == 7)
   {
      mem_putc( UFDBsubLevel7 );
      numSub7++;
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   7 subnodes\n", indent, "", node->tag ));
   }
   else if (node->totalSubNodes > 0)
   {
      /* write the number of subnodes in a 2-byte, 3-byte or 5-byte code */
      if (node->totalSubNodes < 256)
      {
	 mem_putc( UFDBsubLevel );
         mem_putc( node->totalSubNodes );		/* between 8 and 255 */
	 numSub8plus++;
      }
      else if (node->totalSubNodes < 256*256)
      {
	 mem_putc( UFDBsubLevelNNN );		/* more than 255 and less than 65536 */
         i = node->totalSubNodes;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 numSub255plus++;
      }
      else
      {
	 mem_putc( UFDBsubLevelNNNNN );		/* more than 65535 */
         i = node->totalSubNodes;
	 if (doWarnings  &&  i > 15000000)
	    fprintf( stderr, "LARGE number of subnodes: %d for node %s  *****\n", i, node->tag );
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 i = i / 256;
         mem_putc( i % 256 );
	 numSub64Kplus++;
      }
      DEBUG(( stderr, "%*s      tag = %-18s sub-level   %d subnodes\n", indent, "", node->tag, node->totalSubNodes ));
   }
   else
   {
      /* node->totalSubNodes == 0 */
      numLeafNodes++;
      DEBUG(( stderr, "%*s      tag = %-18s leaf node\n", indent, "", node->tag ));
   }

   for (i = 0; i < node->nSTA; i++)
   {
      sta = &node->stas[i];
      for (j = 0; j < sta->nSubNodes; j++)
      {
	 indent += 3;
	 createMemTable_2_2( &(sta->subNodes[j]) );
	 indent -= 3;

	 if (sta->subNodes[j].totalSubNodes == 0)
	 {
	    if (i != node->nSTA - 1  ||  j != sta->nSubNodes - 1)
	       mem_putc( UFDBsameLevel );
	 }
	 else
	    mem_putc( UFDBprevLevel );
      }
   }
}


/* need a forward declaration since addDomain() and addOtherYoutubeURLs() call eachother */
UFDB_GCC_HOT UFDB_GCC_INLINE
static void addDomain( 
   UFDBthreadAdmin * admin,
   char *   	     domain, 
   UFDBurlType       type );


UFDB_GCC_HOT
static void addOtherYoutubeURLs( 
   UFDBthreadAdmin * admin,
   char *            id )
{
   char ** u;
   char    tmpURL[128];
   char *  otherURLs[] = {
      "m.youtube.com/watch?v=%s",
      "youtube.com/embed/%s",
      "youtube-nocookie.com/embed/%s",
      "youtube.com/get_video_info?video_id=%s",
      "youtube.com/get_video_info?content_v=%s",
      "youtube-nocookie.com/get_video_info?video_id=%s",
      "youtube-nocookie.com/get_video_info?content_v=%s",
      "ytimg.googleusercontent.com/vi/%s/",
      "i.ytimg.com/vi/%s/",
      "i1.ytimg.com/vi/%s/",
      "i2.ytimg.com/vi/%s/",
      "i3.ytimg.com/vi/%s/",
      "i4.ytimg.com/vi/%s/",
      "i5.ytimg.com/vi/%s/",
      "i6.ytimg.com/vi/%s/",
      "i7.ytimg.com/vi/%s/",
      "i8.ytimg.com/vi/%s/",
      "i9.ytimg.com/vi/%s/",
      "i.ytimg.com/vi_webp/%s/",
      "i1.ytimg.com/vi_webp/%s/",
      "i2.ytimg.com/vi_webp/%s/",
      "i3.ytimg.com/vi_webp/%s/",
      "i4.ytimg.com/vi_webp/%s/",
      "i5.ytimg.com/vi_webp/%s/",
      "i6.ytimg.com/vi_webp/%s/",
      "i7.ytimg.com/vi_webp/%s/",
      "i8.ytimg.com/vi_webp/%s/",
      "i9.ytimg.com/vi_webp/%s/",
      "i.ytimg.com/sb/%s/",
      "i1.ytimg.com/sb/%s/",
      "i2.ytimg.com/sb/%s/",
      "i3.ytimg.com/sb/%s/",
      "i4.ytimg.com/sb/%s/",
      "i5.ytimg.com/sb/%s/",
      "i6.ytimg.com/sb/%s/",
      "i7.ytimg.com/sb/%s/",
      "i8.ytimg.com/sb/%s/",
      "i9.ytimg.com/sb/%s/",
      NULL
   };

   for (u = otherURLs;  *u != NULL;  u++)
   {
      sprintf( tmpURL, *u, id );
      addDomain( admin, tmpURL, UFDBurl );
   }
}


UFDB_GCC_HOT
static void addDomain( 
   UFDBthreadAdmin * admin,
   char *   	     domain, 
   UFDBurlType       type )
{
   char *            t;
   UFDBrevURL *      revUrl;
   int               rv;
   int               portnumber;
   char              protocol[16];
   char              strippedURL[UFDB_MAX_URL_LENGTH];
   char              strippedDomain[1024];
   
   /* strip starting and trailing whitespace */
   while (*domain == ' ' || *domain == '\t')
      domain++;
   for (t = domain;  *t != '\0' && *t != '\n';  t++)
      ;
   t--;
   while (t > domain  &&  (*t == ' ' || *t == '\t'))
   {
      *t = '\0';
      t--;
   }
   /* skip empty lines */
   if (*domain == '\0')
      return;

   if (UFDBglobalDebug > 1)
      fprintf( stderr, "addDomain( %s )\n", domain );
      
   numEntries++;

   UFDBstripURL2( (char *) domain, stripWWW, strippedURL, strippedDomain, protocol, &portnumber );

   /* we do not check illegal domain names, but must check for '..' because that
    * generates zero-length tags and may cause a lot of evil.
    */
   for (t = strippedDomain;  *t != '\0';  t++)
   {
      if (*t == '.'  &&  *(t+1) == '.')
      {
         ufdbLogError( "found '..' in domain.  Bad URL is not added: %s", domain );
	 return;
      }
   }

   if (!utf8support)
   {
      unsigned char * s;

      for (s = (unsigned char *) strippedURL;  *s != '\0';  s++)
      {
         if (*s >= 0x80)
         {
            if (doWarnings)
               fprintf( stderr, "warning: must use format 2.2 or later for URL with UTF8 characters.\n"
                        "   skipping URL %s\n", domain );
            return;
         }
      }
   }

#if 1
   if (UFDBglobalDebug)
      ufdbLogMessage( "domain: %s\nstrippedurl: %s\nprotocol: %s\nport: %d", 
                      domain, strippedURL, protocol, portnumber );
#endif

   revUrl = UFDBgenRevURL4table( admin, (unsigned char *) strippedURL );

   /* first do a lookup of the domain, and if it already matches, it should not be added !  */
   if (skipOptimisations)
      rv = 0;
   else
      rv = UFDBgentableLookupRevUrl( table, revUrl );
   if (rv)
   {
      if (doWarnings)
         ufdbLogMessage( "URL %s is not added because it was already matched by a previous URL", domain );
   }
   else
   {
      rv = UFDBinsertURL( table, revUrl, domain, type );
      if (rv)
      {
         if (doWarnings)
            ufdbLogMessage( "URL %s has optimised subdomains or paths", domain );
      }
   }

   UFDBfreeRevURL( admin, revUrl );

   if (strncmp( strippedURL, "youtube.com/watch?v=", 20 ) == 0)
   {
      char * id;
      char * end;
      id = strippedURL + 20;
      if (*id != '\0'  &&  strlen(id) > 10)
      {
         /* remove other parameters after the video ID */
         end = id;
         while (*end != '\0'  &&  *end != '&')
            end++;
         *end = '\0';
         addOtherYoutubeURLs( admin, id );
      }
   }
}


static void generateRandomKey( char * encryptKey )
{
   srandom( (getpid() << 12) + time(NULL) );

   encryptKey[0]  = randomChar();
   encryptKey[1]  = randomChar();
   encryptKey[2]  = randomChar();
   encryptKey[3]  = randomChar();
   encryptKey[4]  = randomChar();
   encryptKey[5]  = randomChar();
   encryptKey[6]  = randomChar();
   encryptKey[7]  = randomChar();
   encryptKey[8]  = randomChar();
   encryptKey[9]  = randomChar();
   encryptKey[10] = randomChar();
   encryptKey[11] = randomChar();
   encryptKey[12] = randomChar();
   encryptKey[13] = randomChar();
   encryptKey[14] = randomChar();
   encryptKey[15] = randomChar();
   encryptKey[16] = '\0';
}


static void copyKey( char * key, char * encryptKey )
{
   key[0]  = encryptKey[0];
   key[1]  = encryptKey[1];
   key[2]  = encryptKey[2];
   key[3]  = encryptKey[3];
   key[4]  = '-';
   key[5]  = encryptKey[4];
   key[6]  = encryptKey[5];
   key[7]  = encryptKey[6];
   key[8]  = encryptKey[7];
   key[9]  = '-';
   key[10] = encryptKey[8];
   key[11] = encryptKey[9];
   key[12] = encryptKey[10];
   key[13] = encryptKey[11];
   key[14] = '-';
   key[15] = encryptKey[12];
   key[16] = encryptKey[13];
   key[17] = encryptKey[14];
   key[18] = encryptKey[15];
   key[19] = '\0';
}


static void encryptMemory( unsigned char * to, unsigned char * from, long n, unsigned char * key )
{
   ufdbCrypt uc;

   if (UFDBglobalDebug)
      fprintf( stderr, "encryptMemory( %08lx %08lx %ld %16.16s )\n", (unsigned long)to, (unsigned long)from, n, key );

   ufdbCryptInit( &uc, key, 16 );
   ufdbEncryptText( &uc, to, from, n );
}


static long BZ2compressMemory( unsigned char * to, unsigned char * from, long size )
{
   unsigned int new_size;

   new_size = (unsigned int) (size + 2048);
   if (BZ_OK != BZ2_bzBuffToBuffCompress( (char *) to, &new_size, (char *) from, size, 7, 0, 30 ))
   {
      fprintf( stderr, "compression failed.\n" );
      exit( 1 );
   }

   if (UFDBglobalDebug)
      fprintf( stderr, "BZIP2 compression: from size %ld to %d\n", size, new_size );

   return new_size;
}


static long ZLIBcompressMemory( unsigned char * to, unsigned char * from, long size )
{
   int       retval;
   z_stream  zs;

   zs.zalloc = Z_NULL;
   zs.zfree  = Z_NULL;
   zs.opaque = Z_NULL;

   zs.next_in  = from;
   zs.avail_in = size;

   zs.next_out  = to;
   zs.avail_out = size + 2048;

   retval = deflateInit( &zs, 7 );
   if (Z_OK != retval)
   {
      fprintf( stderr, "ZLIB initialisation failed: error %d  *****\n", retval );
      exit( 1 );
   }
   retval = deflate( &zs, Z_FINISH );
   if ((Z_STREAM_END != retval && Z_OK != retval)  ||  zs.avail_out == 0)
   {
      fprintf( stderr, "ZLIB compression failed: error %d  avail_out %u  *****\n", retval, zs.avail_out );
      exit( 1 );
   }

   if (UFDBglobalDebug)
      fprintf( stderr, "ZLIB compression: from size %ld to %lu\n", size, zs.total_out );

   size = (long) zs.total_out;
   deflateEnd( &zs );
   return size;
}


static void doCryptCompress( 
   FILE * f,
   char * encryptKey,
   char * format )
{
   long   hdr_size;
   long   size;
   long   orig_size;
   unsigned char * buffer2;
#if 0
   int    cksum = 0;
#endif

   /* The table is in mem[]; doCrypt and/or doZLIBcompress or doBZ2compress is 1.
    * The result of this function is in mem[] and the size (mem_i) is adjusted.
    */

   orig_size = size = mem_i;

   if (UFDBglobalDebug)
      fprintf( stderr, "doCryptCompress orig_size %ld bytes  doCrypt=%d  doZLIBcompress=%d  doBZ2compress=%d  format=%s\n", 
               orig_size, doCrypt, doZLIBcompress, doBZ2compress, format );

   buffer2 = (unsigned char *) malloc( size + 2048 );
   if (buffer2 == NULL)
   {
      fprintf( stderr, "cannot allocate memory for encryption and/or compression (size=%ld)\n", size );
      exit( 1 );
   }

   /* make sure the 'result' is in buffer2 */
   if (doBZ2compress)
   {
      size = BZ2compressMemory( buffer2, mem, orig_size );
      if (UFDBglobalDebug)
         fprintf( stderr, "BZIP2 compressed %ld bytes to %ld bytes in buffer2\n", orig_size, size );
   }
   else if (doZLIBcompress)
   {
      size = ZLIBcompressMemory( buffer2, mem, orig_size );
      if (UFDBglobalDebug)
         fprintf( stderr, "ZLIB compressed %ld bytes to %ld bytes in buffer2\n", orig_size, size );
   }
   else
   {
      memcpy( buffer2, mem, size );
      if (UFDBglobalDebug)
         fprintf( stderr, "copied %ld bytes to buffer2 (no compression)\n", size );
   }

   /* crypt from buffer2 into mem */
   if (doCrypt)
   {
      encryptMemory( mem, buffer2, size, (unsigned char *) encryptKey );
      if (UFDBglobalDebug)
         fprintf( stderr, "crypted %ld bytes from buffer2 to mem\n", size );
   }
   else
   {
      memcpy( mem, buffer2, size );
      if (UFDBglobalDebug)
         fprintf( stderr, "copied %ld bytes from buffer2 to mem\n", size );
   }

#if 0
   /* TODO fix the problem with 2.1 cksum */
   if (strcmp( format, "2.1" ) >= 0)
   {
      cksum = UFDBcalcCksum( buffer1, size );
   }
#endif

   hdr_size = doPadding ? sizeof(struct UFDBfileHeader21) : sizeof(struct UFDBfileHeader);
   /* write mem to the file */
   fseek( f, (long) hdr_size, SEEK_SET );

   if (1 != fwrite( mem, size, 1, f ))
   {
      fprintf( stderr, "cannot write crypted/compressed table to file: fwrite failed.\n" );
      exit( 2 );
   }
   fflush( f );

   if (UFDBglobalDebug)
      fprintf( stderr, "%ld bytes written to file\n", size );

   /* truncate the file (if we did compression) */
   if ((doZLIBcompress || doBZ2compress)  &&  size < orig_size)
   {
      if (ftruncate( fileno(f), size + hdr_size ) < 0)
         fprintf( stderr, "failed to truncate compressed file to size %ld", (long) size + hdr_size );
   }

   free( buffer2 );
}


#if 0
static void UFDB_GCC_HOT convertSpecialCharacters( unsigned char * domain )
{
   unsigned char * s;
   unsigned char * d;

   for (s = domain, d = domain;  *s != '\0';  s++)
   {
      if (*s == '%')
      {
         unsigned int hex;
	 unsigned int h1, h2;

	 h1 = *(s+1);
	 h2 = *(s+2);
	 if (isxdigit(h1) && isxdigit(h2))
	 {
	    hex  = (h1 <= '9') ? h1 - '0' : h1 - 'a' + 10;
	    hex *= 16;
	    hex += (h2 <= '9') ? h2 - '0' : h2 - 'a' + 10;
	    if (hex == 0)
	    {
	       s += 2;
	       continue;
	    }
	    else if (hex <= 0x20)
	    {
	       if (hex != '\t'  &&  hex != '\r'  &&  hex != '\n'  &&  hex != '\f')
		  hex = ' ';
	    }
	    else
	    {
	       if (hex == 0x7f  ||  hex == 0xff)
		  hex = ' ';
	       else
		  if (hex <= 'Z'  &&  hex >= 'A')
		     hex += 'a' - 'A';
	    }
	    *d++ = hex;
	    s += 2;
	 }
	 else
	    *d++ = *s;
      }
      else
      {
         *d++ = *s;
      }
   }
   *d = '\0';
}
#endif


static int calcpathlen( char * p )
{
   int len = 0;
   while (*p != '\0')
   {
      if (*p == '%'  &&  isxdigit( *(p+1) )  &&  isxdigit( *(p+2) ))
        p += 2;
      p++;
      len++;
   }
   return len;
}


#define MAXPARAMS       200

static int sortstrcmp( const void * a, const void * b )
{
   unsigned char ** ppa;
   unsigned char ** ppb;
   unsigned char *  pa;
   unsigned char *  pb;
   int              diff, va, vb;

   ppa = (unsigned char **) a;
   ppb = (unsigned char **) b;
   pa = *ppa;
   pb = *ppb;

   diff = 0;
   while (*pa != '\0')
   {
      va = (int) *pa;
      vb = (int) *pb;
      if (va == '=')
         va = 0;
      pa++;
      if (vb == '=')
         vb = 0;
      diff = va - vb;
      pb++;
      if (va == 0  ||  vb == 0  ||  diff != 0)
         return diff;
   }

   return (int) *pa - (int) *pb;
}


static void sortURLparams( unsigned char * parlist )
{
   char *          p;
   char *          sep;
   char *          pp[MAXPARAMS];
   unsigned int    i;
   unsigned int    n;

   if (strchr( (char*) parlist, '&' ) == NULL)
      return;

   p = (char*) parlist;
   n = 0;
   while ((sep = strchr( p, '&' )) != NULL)
   {
      *sep = '\0';
      pp[n] = ufdbStrdup( p );
      p = sep + 1;
      n++;
      if (n == MAXPARAMS-1)
      {
         ufdbLogError( "cannot sort parameters of %s", parlist );
         for (i = 0;  i < n;  i++)
            ufdbFree( pp[i] );
         return;
      }
   }
   if (n == 0)
      return;
   /* assign the last parameter to pp */
   pp[n] = ufdbStrdup( p );

   ++n;

   qsort( pp, (size_t) n, (size_t) sizeof(char*), sortstrcmp );

   p = (char*) parlist;
   for (i = 0;  i < n;  i++)
   {
      p += sprintf( p, "%s", pp[i] );
      if (i < n-1)
         *p++ = '&';
      ufdbFree( pp[i] );
   }
}


UFDB_GCC_HOT 
int main( int argc, char * argv[] )
{
   int               n;
   int               opt;
   time_t            now;
   struct tm         tm;
   unsigned char *   d;
   char              encryptKey[16+1];
   char              key[16+3+1];
   char              flags[8+1];
   FILE *            fout;
   char *            fout_buffer;
   UFDBthreadAdmin * admin;
   int		     hdr_size;
   struct UFDBfileHeader21 header;
   char              date[64];
   char              outFileName[512];
   char              tempOutFileName[512];
   unsigned char     domain[4096];

   UFDBappInit();
   admin = UFDBallocThreadAdmin();
   inFileName = NULL;
   urlsFileName = NULL;
   tableName = "defaulttable";
   date[0] = flags[0] = key[0] = encryptKey[0] = '\0';

   while ((opt = getopt( argc, argv, "DF:k:t:d:u:NnCqpPsVW?XZ" )) > 0)
   {
      switch (opt)
      {
      case 'D':
      	 UFDBglobalDebug++;
	 break;
      case 'F':
         format = optarg;
	 if (!strmatch3( format, "1.2" )  &&
	     !strmatch3( format, "2.0" )  &&
	     !strmatch3( format, "2.1" )  &&
	     !strmatch3( format, "2.2" ))
	 {
	    fprintf( stderr, "-F option only accepts 1.2, 2.0, 2.1 and 2.2 as format specifiers\n" );
	    usage();
	 }
	 if (strmatch3( format, "1.2" )  ||
	     strmatch3( format, "2.0" )  ||
	     strmatch3( format, "2.1" ))
         {
            utf8support = 0;
         }
	 break;
      case 't':
         tableName = optarg;
	 break;
      case 'd':
         inFileName = optarg;
	 break;
      case 's':
      	 doSanityCheck = 1;
	 break;
      case 'u':
         urlsFileName = optarg;
	 break;
      case 'k':
         strncpy( encryptKey, optarg, 16 );
	 encryptKey[16] = '\0';
	 if (strlen( encryptKey ) != 16)
	 {
	    fprintf( stderr, "key \"%s\" is not valid.\n", encryptKey );
	    usage();
	 }
	 break;
      case 'n':
         doCrypt = 0;
	 break;
      case 'N':
         utf8support = 0;
         break;
      case 'p':
         doPadding = 1;
	 break;
      case 'P':
         doProd = 1;
	 break;
      case 'C':
         doBZ2compress = 1;
	 break;
      case 'q':
         doWarnings = 0;
	 break;
      case 'V':
         printf( "ufdbGenTable version " UFDB_VERSION "\n" );
	 printf( "ufdbGuard for Squid is free and Open Source Software.\n" );
	 printf( "Copyright (C) 2005-2018 by URLfilterDB B.V. and others.\n" );
	 exit( 0 );
      case 'W':
         stripWWW = 1;
	 break;
      case 'X':
         skipOptimisations = 1;
	 break;
      case 'Z':
         doZLIBcompress = 1;
	 break;
      case '?':
	 fprintf( stderr, "help:\n" );
         usage();
	 break;
      default:
	 fprintf( stderr, "internal error: getopt returned \"%c\"\n", opt );
         usage();
	 break;
      }
   }

   if (strlen(tableName) > 15)
   {
      tableName[15] = '\0';
      fprintf( stderr, "warning: the tableName is truncated to \"%s\"\n", tableName );
   }

   if (inFileName == NULL)
   {
      fprintf( stderr, "the input file name is not specified: use the -d option\n" );
      usage();
   }

   if (doZLIBcompress && doBZ2compress)
   {
      fprintf( stderr, "use -C or -Z but not both.\n" );
      usage();
   }

   fin = fopen( inFileName, "r" );
   if (fin == NULL)
   {
      fprintf( stderr, "cannot read from \"%s\": %s\n", inFileName, strerror(errno) );
      usage();
   }
   if (UFDBglobalDebug)
      fprintf( stderr, "processing domains from file \"%s\"\n", inFileName );

   strcpy( outFileName, inFileName );
   strcat( outFileName, UFDBfileSuffix );

   strcpy( tempOutFileName, outFileName );
   strcat( tempOutFileName, ".temp" );

   fout = fopen( tempOutFileName, "w+" );
   if (fout == NULL)
   {
      fprintf( stderr, "cannot write to \"%s\": %s\n", tempOutFileName, strerror(errno) );
      usage();
   }
   if (UFDBglobalDebug)
      fprintf( stderr, "opened temporary file \"%s\"\n", tempOutFileName );
   fout_buffer = (char *) malloc( 64*1024 );
   setvbuf( fout, fout_buffer, _IOFBF, 64*1024 );

   /* setlinebuf( stderr ); */
   initTable( tableName );

   URLparamSupport = 0;
   if (strcmp( format, "2.2" ) >= 0)
      URLparamSupport = 1;


   /* process the domains ********************************************/
   n = 0;
readdomains:
   while (!feof(fin))
   {
      int             last_char;
      unsigned char * ptr;

      ptr = domain;

      while ((*ptr = last_char = myfast_getc(fin)) != '\n')
      {
	 /* check for a last line without \n */
         if (last_char == EOF)
	 {
	    if (ptr != domain)
	       break;
	    goto eof;
	 }
	 if (last_char == '\r')
	    continue;
	 ptr++;
	 if (ptr > &domain[4090])
	 {
	    *ptr = '\0';
	    fprintf( stderr, "line too long: %s\n", domain );
	    while (!feof(fin) && myfast_getc(fin) != '\n')
	       ;
	    goto readdomains;
	 }
      }
      *ptr = '\0';

      d = domain;
      while (*d == ' ')
	 d++;

      if (d[0] != '\0'  &&  d[0] != '#')
      {
	 char * first_slash;

         d = skipProtocol( d );
         if (d == NULL)
            d = domain;

	 /* TODO: use has_tld(domain+4) */
         if (d[0] == 'w' && d[1] == 'w' && d[2] == 'w' && d[3] == '.' && strchr( (char*) d+4, '.' ) != NULL)	
	 {
	    if (stripWWW)
	    {
	       if (doWarnings)
		  fprintf( stderr, "notice: \"www.\" is stripped for %s\n", d );
	    }
	    else if (doWarnings)
	    {
	       fprintf( stderr, "warning: domain name starts with \"www.\": %s (use -W option ?)\n", d );
	    }
	 }

	 if (doWarnings)
	 {
	    if (ptr - d > 66)
	       fprintf( stderr, "warning: long domain name: %s\n", d );
	 }

	 first_slash = strchr( (char *) d, '/' );
	 if (first_slash != NULL)
	    fprintf( stderr, "warning: domain name (%s) has a '/' ?!?!\n", d );
	 if (UFDBsanityCheckDomainname( (char *) d ))
	    addDomain( admin, (char *) d, UFDBdomain );
      }
   }
eof:
   fclose( fin );

   /* process the urls ***********************************************/
   if (urlsFileName != NULL)
   {
      fin = fopen( urlsFileName, "r" );
      if (fin == NULL)
      {
	 fprintf( stderr, "cannot read from \"%s\": %s\n", urlsFileName, strerror(errno) );
	 usage();
      }
      if (UFDBglobalDebug)
	 fprintf( stderr, "processing urls from file \"%s\"\n", urlsFileName );

readurls:
      while (!feof(fin))
      {
         int             last_char;
	 unsigned char * qm;
	 unsigned char * ptr;
	 unsigned char * first_slash;

	 qm = NULL;
	 ptr = domain;

	 while ((*ptr = last_char = myfast_getc(fin)) != '\n')
	 {
	    /* check for a last line without \n */
	    if (last_char == EOF)
	    {
	       if (ptr != domain)
		  break;
	       goto eof2;
	    }
	    if (last_char == '\r')			/* Skip '\r' */
	       continue;
	    if (qm == NULL  &&  last_char == '?')	/* remember the first question mark */
	       qm = ptr;
	    if (last_char < ' ')			/* illegal control character in URL */
	    {
	       if (doWarnings)
		  fprintf( stderr, "illegal control character in URL: %s\n", domain );
	       *ptr = '\0';
	       while (!feof(fin) && myfast_getc(fin) != '\n')
		  ;
	       break;
	    }
	    ptr++;
	    if (ptr > &domain[4090])
	    {
	       *ptr = '\0';
	       fprintf( stderr, "URL too long: %s\n", domain );
	       while (!feof(fin) && myfast_getc(fin) != '\n')
		  ;
	       goto readurls;
	    }
	 }
	 *ptr = '\0';

	 d = domain;
	 while (*d == ' ')
	    d++;
	 d = skipProtocol( d );
	 if (d == NULL)
	    d = domain;

	 if (d[0] != '\0'  &&  d[0] != '#')
	 {
	    first_slash = (unsigned char *) strchr( (char *) d, '/' );
	    if (first_slash == NULL)
	    {
	       if (doWarnings) 
	       {
		  fprintf( stderr, "warning: URL has no '/': %s\n", d );
		  if (strlen( (char *) d ) > 66)
		     fprintf( stderr, "warning: long domainname in URL: %s\n", d );
	       }
	    }
	    else
	    {
	       int    pathlen;

	       if (qm != NULL)
	       {
		  /* make sure that the database format is 2.2 when parameters are used */
		  if (!URLparamSupport)
		  {
		     fprintf( stderr, "error: URL with parameters is only supported in format 2.2 and newer.  URL: %s\n", d );
		     numErrors++;
		     *qm = '\0';
		     qm = NULL;
		  }
		  else
		  {
		     if (*(qm+1) == '\0')
		     {
                        if (doWarnings) 
                           fprintf( stderr, "warning: URL with '?' has no parameters and '?' is removed from URL %s\n", d );
                        *qm = '\0';
                        qm = NULL;
		     }
		  }
	       }

	       pathlen = calcpathlen( (char *) first_slash );
	       if (qm != NULL)
	       {
		  int params_len;

                  /* TODO: the following check on the length of all parameters is not ok. 
                   * TODO: instead must check length of each parameter and each value.
                   */
		  params_len = calcpathlen( (char *) qm );
	          pathlen -= params_len;
		  if (params_len >= sizeof(UFDBurlPart))
		  {
		     if (doWarnings)
			fprintf( stderr, "warning: parameter list exceeds %d characters and may be too long: %s\n", 
                                 (int) sizeof(UFDBurlPart)-1, d );
		  }
                  sortURLparams( qm+1 );
	       }
	       if (doWarnings) 
	       {
		  if (first_slash - d > 66)
		     fprintf( stderr, "warning: long domainname in URL: %s\n", d );
		  if (pathlen > 127)
		     fprintf( stderr, "warning: long path in URL: %s\n", d );
                  else if (pathlen >= UFDB_REVURL_LABEL_SIZE)
		     fprintf( stderr, "warning: very long URL: %s\n", d );
	       }
	    }

	    /* does URL start with "www." ? */
            if (d[0] == 'w'  &&  d[1] == 'w'  &&  d[2] == 'w'  &&  d[3] == '.' && strchr( (char*) d+4, '.' ) != NULL)
	    {
	       if (stripWWW)
	       {
		  if (doWarnings)
		     fprintf( stderr, "notice: \"www.\" is stripped for %s\n", d );
	       }
	       else if (doWarnings)
	       {
		  fprintf( stderr, "warning: URL name starts with \"www.\": %s (use -W option ?)\n", d );
	       }
	    }

	    if (UFDBsanityCheckDomainname( (char *) d ))
	       addDomain( admin, (char *) d, UFDBurl );
	 }
      }
eof2:
      fclose( fin );
   }

   if (encryptKey[0] == '\0')
      generateRandomKey( encryptKey );
   copyKey( key, encryptKey );

   /* write the table in binary format to the memory buffer */
   init_membuf();
   if (strmatch3( format, "1.2" ))
   {
      DEBUG(( stderr, "writing table in DB 1.2 format\n" ));
      createMemTable_1_2( table );
   }
   else if (strmatch3( format, "2.0" ))
   {
      DEBUG(( stderr, "writing table in DB 2.0 format\n" ));
      createMemTable_2_0( table );
   }
   else if (strmatch3( format, "2.1" ))
   {
      DEBUG(( stderr, "writing table in DB 2.1 format\n" ));
      createMemTable_2_1( table );
   }
   else
   {
      DEBUG(( stderr, "writing table in DB 2.2 format\n" ));
      createMemTable_2_2( table );
   }
   mem_putc( UFDBendTable );

   /* when SSE and AVX/AVX2 instructions are used we need a 32-byte safeguard */
   for (n = 0; n < 32; n++)
      mem_putc( UFDBendTable );

#if 0
   /* various performance tests have showed that using strcmp() or SSE4.2 is slower than byte-by-byte
    * string comparison so strcmp() is not used in the performance-critical strcmpURLpart().
    */
   mem_puts( (unsigned char *) "64 padding characters since strcmp might use 256-bit lookahead..." );
#endif

   numIndexNodes = numNodes - numLeafNodes;

   /* write the table header to the output file */
   strcpy( flags, "--------" );
   if (doBZ2compress)
      flags[0] = 'C';
   if (doZLIBcompress)
      flags[0] = 'Z';
   if (doProd)
      flags[1] = 'P';
   if (doCrypt)
      flags[2] = 'Q';
   if (doPadding)
      flags[3] = 'p';
   now = time( NULL );
   gmtime_r( &now, &tm );
   sprintf( date, "%4d%02d%02d.%02d%02d", 
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min );
   sprintf( header.string, "%s %s %s %ld key=%s date=%s %8s %ld %d\n\n",
            "UFDB", format, tableName, mem_i, key, date, flags, numNodes, 0 );
   fprintf( fout, "%s", header.string );
   hdr_size = doPadding ? sizeof(struct UFDBfileHeader21) : sizeof (struct UFDBfileHeader);
   for (n = hdr_size - strlen(header.string); n > 0; n--)
      myfast_putc( '\0', fout );

   if (UFDBglobalDebug)
   {
      fprintf( stderr, "table header: %s", header.string );
      fprintf( stderr, "#nodes: %9ld   #leafs: %9ld   #index: %9ld\n",
	       numNodes, numLeafNodes, numIndexNodes );
      fprintf( stderr, "#sub1:  %9ld   #sub2:  %9ld   #sub3:  %9ld   #sub4:  %9ld\n",
	       numSub1, numSub2, numSub3, numSub4 );
      fprintf( stderr, "#sub5:  %9ld   #sub6:  %9ld   #sub7:  %9ld\n",
	       numSub5, numSub6, numSub7 );
      fprintf( stderr, "#sub8+: %9ld   #sub255+: %7ld   #sub64K+: %7ld\n",
	       numSub8plus, numSub255plus, numSub64Kplus );
   }

   /* encrypt and compress the table: rewind, read, compress, crypt and write */
   if (doCrypt || doBZ2compress || doZLIBcompress)
   {
      doCryptCompress( fout, encryptKey, format );
   }
   else
   {
      fwrite( mem, mem_i, 1, fout );
   }

   fclose( fout );
   free( fout_buffer );

   /* to get around some permission problems: unlink before rename */
   if (unlink( outFileName ) < 0  &&  errno != ENOENT)
   {
      fprintf( stderr, "cannot remove \"%s\": %s\n", outFileName, strerror(errno) );
   }
   if (rename( tempOutFileName, outFileName ) != 0)
   {
      fprintf( stderr, "cannot rename '%s' into '%s': %s\n", tempOutFileName, outFileName, strerror(errno) );
      (void) unlink( tempOutFileName );
      exit( 1 );
   }
   else if (UFDBglobalDebug)
      fprintf( stderr, "temporary file \"%s\" renamed to \"%s\"\n", tempOutFileName, outFileName );

   if (numErrors)
      exit( 2 );

   return 0;
}


/* since ufdbguard (single-threaded) and ufdbguardd (multi-threaded)
 * share source code, we put some pthread dummys here since we don't need/want pthreads.
 */
int pthread_mutex_init( pthread_mutex_t * mutex, const pthread_mutexattr_t * attr )
{
   return 0;
}

int pthread_mutex_lock( pthread_mutex_t * mutex )
{
   return 0;
}

int pthread_mutex_trylock( pthread_mutex_t * mutex )
{
   return 0;
}

int pthread_mutex_unlock( pthread_mutex_t * mutex )
{
   return 0;
}


int pthread_cond_signal(pthread_cond_t *cond)
{
   return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
   return 0;
}

