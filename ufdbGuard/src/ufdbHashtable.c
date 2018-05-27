/*
 * ufdbHashtable.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2015 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is entirely written by URLfilterDB.
 *
 * RCS $Id: ufdbHashtable.c,v 1.8 2016/12/11 23:44:53 root Exp root $
 */

#include "ufdb.h"
#include "ufdblib.h"
#include "ufdbchkport.h"
#include "httpsQueue.h"
#include "ufdbHashtable.h"

#include <strings.h>
#include <string.h>
#include <stdlib.h>


/* expandHashtable: 
 * on entry: ht->mutex MUST be locked.
 * on exit:  ht->mutex remains locked.
 */
static void expandHashtable(
   struct UFDBhashtable *  ht  )
{
   unsigned int            newTableSize;
   unsigned int            i, j;
   struct UFDBhte *        hte;
   struct UFDBhte **       newtable;

   /* Whenever expeandHashtable is called, the ht->mutex is already locked,
    * so we do not use it in this function.
    */

   newTableSize = ht->tableSize * 2 - 3;	/* 107, 211, 419, 835, ... */

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         expandHashtable: cache %08lx old size %d  new size %d", 
                      (unsigned long) ht, ht->tableSize, newTableSize );

   newtable = (struct UFDBhte **) ufdbCalloc( sizeof(struct UFDBhte*), newTableSize );
   if (newtable == NULL)
   {
      /* Oops, we ran out of memory.
       * Well, we just return without expanding the table
       * since the memory allocation error is in this stage
       * not severe and the application can go on.
       */

      /* horrible workaround to prevent calling expandTable all the time: */
      ht->optimalMaxEntries = (unsigned int) (ht->tableSize * 0.88);

      /* TO-DO: removeOldEntriesFromCache( ht ); */
      return;
   }

   for (i = 0; i < ht->tableSize; i++)
   {
      while ((hte = ht->table[i]) != NULL)
      {
	 /* TODO: remove old entries (>3 hours) in this copy process */
	 ht->table[i] = hte->next;

	 /* put the entry in the new table (at the new position) */
	 j = hte->hash % newTableSize;
	 hte->next = newtable[j];
	 newtable[j] = hte;
      }
   }

   ht->tableSize = newTableSize;
   ht->optimalMaxEntries = (unsigned int) (newTableSize * 0.68);

   ufdbFree( ht->table );
   ht->table = newtable;
}

   
/* UFDBcreateHashtable: 
 * on entry: ht->mutex does not exist
 * on exit:  ht->mutex is created (not locked).
 */
struct UFDBhashtable *
UFDBcreateHashtable( 
   unsigned int           tableSize,
   unsigned int           (*hashFunction) (const void*),
   int                    (*keyEqFunction) (const void*,const void*) )
{
   struct UFDBhashtable * ht;
   pthread_mutexattr_t   attr;

   ht = (struct UFDBhashtable *) ufdbMallocAligned( 64, sizeof(struct UFDBhashtable) );
   if (ht == NULL)
      return NULL;

   if (tableSize < 107)
      tableSize = 107;
   if (tableSize % 2 == 0)
      tableSize++;

   ht->tableSize = tableSize;
   ht->table = (struct UFDBhte **) ufdbCalloc( sizeof(struct UFDBhte*), tableSize );
   if (ht->table == NULL)
      return NULL;
   ht->nEntries = 0;
   ht->optimalMaxEntries = (unsigned int) (tableSize * 0.68);
   ht->hashFunction = hashFunction;
   ht->keyEqFunction = keyEqFunction;

   pthread_mutexattr_init( &attr );
   pthread_mutexattr_settype( &attr, UFDBglobalDebug>3 ? PTHREAD_MUTEX_ERRORCHECK : PTHREAD_MUTEX_NORMAL );
   pthread_mutex_init( &(ht->mutex), NULL );

   return ht;
}


/* UFDBinsertHashtable: 
 * on entry: ht->mutex is locked IFF lockSetBySearch != 0
 * on exit:  ht->mutex MUST be unlocked.
 *
 * key MUST be malloced.
 * value MUST be malloced.
 */
void
UFDBinsertHashtable( 
   struct UFDBhashtable * ht,
   void *                 key,
   void *                 value,
   int                    lockSetBySearch )
{
   unsigned int           i;
   int                    ps;
   struct UFDBhte *       hte;

   if (!lockSetBySearch)
   {
      ps = pthread_mutex_lock( &ht->mutex );			/* ========================== */
      if (ps != 0)
      {
         UFDBglobalCrashOnFatal = 1;
	 ufdbLogFatalError( "UFDBinsertHashtable: pthread_mutex_lock returned %d", ps );
      }
      /* TODO: must search again after acquiring lock !!! */
   }

   hte = (struct UFDBhte *) ufdbMalloc( sizeof(struct UFDBhte) );
   if (hte == NULL)
   {
      ps = pthread_mutex_unlock( &ht->mutex );
      if (ps != 0)
      {
         UFDBglobalCrashOnFatal = 1;
	 ufdbLogFatalError( "UFDBinsertHashtable: pthread_mutex_unlock/1 returned %d", ps );
      }
      return;
   }
   hte->hash = ht->hashFunction( key );
   hte->key = key;
   hte->value = value;

   i = hte->hash % ht->tableSize;
   hte->next = ht->table[i];
   ht->table[i] = hte;

   ht->nEntries++;

   /* TODO: implement safeguard against duplicates in Hashtable */

   /* TODO: better cache management: HTE must have lastQueryTime */

   if (ht->nEntries > ht->optimalMaxEntries)
      expandHashtable( ht );

   ps = pthread_mutex_unlock( &ht->mutex );			/* ========================== */
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBinsertHashtable: pthread_mutex_unlock/2 returned %d", ps );
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         UFDBinsertHashtable: cache %08lx  key %08lx '%s'  hash %08lx  value %08lx  nEntries %d",
                      (unsigned long) ht, (unsigned long) hte->key, (char *) hte->key, (unsigned long) hte->hash, (unsigned long) hte->value, ht->nEntries );
}


/* UFDBunlockHashtable: 
 * on entry: ht->mutex MUST be locked.
 * on exit:  ht->mutex MUST be unlocked.
 */
void UFDBunlockHashtable( 
   struct UFDBhashtable * ht  )
{
   int ps;

   ps = pthread_mutex_unlock( &ht->mutex );
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBunlockHashtable: pthread_mutex_unlock returned %d", ps );
   }
}


/* UFDBlockHashtable: 
 * on entry: ht->mutex MUST be unlocked.
 * on exit:  ht->mutex MUST be locked.
 */
void UFDBlockHashtable( 
   struct UFDBhashtable * ht  )
{
   int ps;

   ps = pthread_mutex_lock( &ht->mutex );
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBlockHashtable: pthread_mutex_lock returned %d", ps );
   }
}


/* UFDBsearchHashtable: 
 * on entry: ht->mutex MUST be unlocked.
 * on exit:  ht->mutex is locked, but unlocked IFF retval != NULL || keepLockForInsert != 0
 */
void *
UFDBsearchHashtable( 
   struct UFDBhashtable * ht,
   void *                 key,
   int                    keepLockForInsert )
{
   unsigned int  	  hash;
   unsigned int  	  i;
   int                    ps;
   void *                 retval;
   struct UFDBhte  *      hte;

   retval = NULL;
   hash = ht->hashFunction( key );

   ps = pthread_mutex_lock( &ht->mutex );		/* ===================================== */
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBsearchHashtable: pthread_mutex_lock failed with error code %d", ps );
   }

   i = hash % ht->tableSize;
   hte = ht->table[i];
   while (hte != NULL)
   {
      if (hte->hash == hash  &&  ht->keyEqFunction(key,hte->key))
      {
	 retval = hte->value;
	 break;
      }
      hte = hte->next;
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         UFDBsearchHashtable: cache %08lx  key '%s'  hash %08lx  value %08lx  keeplock %d",
                      (unsigned long) ht, (char *) key, (unsigned long) hash, (unsigned long) retval, keepLockForInsert );

   if (retval != NULL  ||  !keepLockForInsert)
   {
      ps = pthread_mutex_unlock( &ht->mutex );	/* ===================================== */
      if (ps != 0)
      {
	 UFDBglobalCrashOnFatal = 1;
	 ufdbLogFatalError( "UFDBsearchHashtable: pthread_mutex_unlock failed with error code %d", ps );
      }
   }

   return retval;
}


/* UFDBremoveHashtable: 
 * on entry: ht->mutex MUST be locked.
 * on exit:  ht->mutex remains locked.
 * return:   hte->value
 */
void * 
UFDBremoveHashtable( 
   struct UFDBhashtable * ht,
   void *                 key )
{
   unsigned int           hash;
   unsigned int           i;
   struct UFDBhte *       hte;
   struct UFDBhte **      head_hte;
   void *                 retval;

   retval = NULL;
   hash = ht->hashFunction( key );

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         UFDBremoveHashtable: cache %08lx  key %08lx %s  hash %08lx", 
                      (unsigned long) ht, (unsigned long) key, (char *) key, (unsigned long) hash );

   i = hash % ht->tableSize;
   head_hte = &( ht->table[i] );
   hte = *head_hte;
   while (hte != NULL)
   {
      if (hte->hash == hash  &&  ht->keyEqFunction(key,hte->key))
      {
	 *head_hte = hte->next;
	 retval = hte->value;
	 ufdbFree( hte->key );
	 ufdbFree( hte );
	 ht->nEntries--;
	 break;
      }
      head_hte = &hte->next;
      hte = hte->next;
   }

   if (hte == NULL)
      ufdbLogError( "UFDBremoveHashtable: did not find key %08lx in cache %08lx *****", (unsigned long) key, (unsigned long) ht );

   return retval;
}


/* UFDBpurgeHashtable: 
 * on entry: ht->mutex MUST be locked.
 * on exit:  ht->mutex remains locked.
 */
int
UFDBpurgeHashtable( 
   struct UFDBhashtable * ht,
   int (*purge_test)(const void * key, const void * value)  )
{
   unsigned int           i;
   int           	  n;
   struct UFDBhte *       hte;
   struct UFDBhte *       next;
   struct UFDBhte **      prev;

   n = 0;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         UFDBpurgeHashtable: cache %08lx  tableSize %d  nEntries %d  opt-max %d", 
                      (unsigned long) ht, ht->tableSize, ht->nEntries, ht->optimalMaxEntries );

   for (i = 0; i < ht->tableSize; i++)
   {
      prev = &ht->table[i];
      hte = *prev;
      while (hte != NULL)
      {
         next = hte->next;
	 if (purge_test( hte->key, hte->value ))
	 {
	    n++;
	    ufdbFree( hte->value );
	    ufdbFree( hte->key );
	    ufdbFree( hte );
	    *prev = next;
	    ht->nEntries--;
	 }
	 else
	    prev = &hte->next;
	 hte = next;
      }
   }

   return n;
}


/* UFDBdestroyHashtable: 
 * on entry: ht->mutex MUST be unlocked.
 * on exit:  ht->mutex is unlocked and destroyed.
 */
void
UFDBdestroyHashtable( 
   struct UFDBhashtable * ht,
   int                    freeValues )
{
   unsigned int           i;
   int                    ps;
   struct UFDBhte *       hte;
   struct UFDBhte *       next;

   /* Hmmm. The table will be destroyed and it is not
    * very useful to use the mutex.
    * But we use the mutex anyway to wait for any
    * current use of the table to terminate.
    */
   ps = pthread_mutex_lock( &ht->mutex );		/* ============================= */
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBdestroyHashtable: pthread_mutex_lock returned %d", ps );
   }

   for (i = 0; i < ht->tableSize; i++)
   {
      hte = ht->table[i];
      while (hte != NULL)
      {
	 next = hte->next;
	 ufdbFree( hte->key );
	 if (freeValues)
	    ufdbFree( hte->value );
	 ufdbFree( hte );
	 hte = next;
      }
   }
   ufdbFree( ht->table );

   ps = pthread_mutex_unlock( &ht->mutex );		/* ============================= */
   if (ps != 0)
   {
      UFDBglobalCrashOnFatal = 1;
      ufdbLogFatalError( "UFDBdestroyHashtable: pthread_mutex_unlock returned %d", ps );
   }
   pthread_mutex_destroy( &ht->mutex );

   /* ht contains the mutex, so it must be freed after the unlock(mutex) */
   ufdbFree( ht );
}

