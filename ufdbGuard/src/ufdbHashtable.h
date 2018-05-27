/*
 * ufdbHashtable.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2009 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is entirely written by URLfilterDB.
 *
 * RCS $Id: ufdbHashtable.h,v 1.4 2015/08/29 21:14:13 root Exp root $
 */

#ifndef UFDB_UFDBHASHTABLE_H_INCLUDED
#define UFDB_UFDBHASHTABLE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>


struct UFDBhte
{
   void *            key;
   void *            value;
   unsigned int      hash;
   struct UFDBhte *  next;
};

struct UFDBhashtable
{
   unsigned int      tableSize;
   unsigned int      nEntries;
   unsigned int      optimalMaxEntries;
   struct UFDBhte ** table;
   unsigned int      (*hashFunction)( const void * );
   int               (*keyEqFunction)( const void *, const void * );
   pthread_mutex_t   mutex;
};


struct UFDBhashtable *
UFDBcreateHashtable( 
   unsigned int           tableSize,
   unsigned int           (*hashFunction)( const void * ),
   int                    (*keyEqFunction)( const void *, const void * ) );

void
UFDBinsertHashtable( 
   struct UFDBhashtable * ht, 
   void *                 key, 
   void *                 value,
   int                    lockSetBySearch );

void *
UFDBsearchHashtable( 
   struct UFDBhashtable * ht,
   void *                 key,
   int                    keepLockForInsert );

void * 
UFDBremoveHashtable( 
   struct UFDBhashtable * ht,
   void *                 key );

int
UFDBpurgeHashtable( 
   struct UFDBhashtable * ht,
   int (*purge_test)(const void * key, const void * value)  );

void
UFDBdestroyHashtable( 
   struct UFDBhashtable * ht,
   int                    freeValues );

void
UFDBlockHashtable( 
   struct UFDBhashtable * ht  );

void
UFDBunlockHashtable( 
   struct UFDBhashtable * ht  );

#ifdef __cplusplus
}
#endif

#endif 

