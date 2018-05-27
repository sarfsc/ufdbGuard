/*
 * ufdbLookup.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 * 
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * squidGuard is copyrighted (C) 1998 by
 * ElTele Øst AS, Oslo, Norway, with all rights reserved.
 *
 * $Id: ufdbLookup.c,v 1.32 2016/10/04 15:05:20 root Exp root $
 */

#include "sg.h"
#include "ufdb.h"
#include "ufdblib.h"
#include "ufdbdb.h"

#include <string.h>


UFDB_GCC_HOT UFDB_GCC_INLINE
static ufdb_memdb_hash_t memDBhash( char * k )
{
   ufdb_memdb_hash_t hash;
   ufdb_memdb_hash_t t;
   
   hash = ((10301399UL * *k) ^ (*(k+1) * 79111UL)) + ((ufdb_memdb_hash_t)*k << 30);
   while (*k != '\0')
   {
      /* hash = ((hash << 4) ^ ((ufdb_memdb_hash_t) (*k) << 12))  +  ((7 * (*k)) ^ (hash >> 10)); */
      hash += (ufdb_memdb_hash_t) *k;
      t = (((ufdb_memdb_hash_t) *k) << 9) ^ hash;
      hash = (hash << 6) ^ t;
      hash += (hash >> 11) ^ (ufdb_memdb_hash_t) *k;
      k++;
   }
   
   return ((ufdb_memdb_hash_t) hash);
}


/* 
 * create an in-memory database (kv-pairs)
 */
struct UFDBmemDB * UFDBmemDBinit( void )
{
   struct UFDBmemDB * m;

   m = (struct UFDBmemDB *) ufdbMallocAligned( 64, sizeof(struct UFDBmemDB) );
   if (m == NULL)
   {
      ufdbLogFatalError( "UFDBmemDBinit: cannot allocate memory" );
      return NULL;
   }

   m->tableSize =107;
   m->nEntries = 0;
   m->optimalMaxEntries = (unsigned int) (m->tableSize * 0.70);
   m->table = (UFDBmemDBkv **) ufdbCalloc( sizeof(UFDBmemDBkv*), m->tableSize );

   return m;
}


static void memDBexpand( 
   struct UFDBmemDB * db  )
{
   unsigned int       newTableSize;
   unsigned int       i, j;
   UFDBmemDBkv *      kv;
   UFDBmemDBkv *      next;
   UFDBmemDBkv **     newtable;
   /* http://compoasso.free.fr/primelistweb/page/prime/liste_online_en.php */
   unsigned int       primeTable[20] = { 107, 331, 967, 2887, 8663, 
                                         17333, 34667, 69313, 104723, 170047,
					 290027, 480017, 840023, 1500133, 2500151,
					 4000133, 7000121, 12001021, 20001031, 35001133 };

   if (db->tableSize >= primeTable[19])
      newTableSize = db->tableSize * 2 - 3;
   else
   {
      for (i = 0; i < 20; i++)
      {
         if (primeTable[i] > db->tableSize)
	    break;
      }
      newTableSize = primeTable[i];
   }

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "         memDBexpand: cache %08lx  old size %u  new size %u", 
                      (unsigned long) db, db->tableSize, newTableSize );

   newtable = (UFDBmemDBkv**) ufdbCalloc( sizeof(UFDBmemDBkv*), newTableSize );
   if (newtable == NULL)
   {
      ufdbLogFatalError( "memDBexpand: cannot allocate memory to expand table to %u entries", newTableSize );

      /* workaround to prevent calling expandTable all the time: */
      db->optimalMaxEntries = db->tableSize;

      return;
   }

   for (i = 0;  i < db->tableSize;  i++)
   {
      kv = db->table[i];
      while (kv != NULL)
      {
	 next = kv->next;

	 /* put the entry in the new table (at the new position) */
	 j = (unsigned int) (kv->hash % newTableSize);
	 kv->next = newtable[j];
	 newtable[j] = kv;

	 kv = next;
      }
   }

   db->tableSize = newTableSize;
   db->optimalMaxEntries = (unsigned int) (newTableSize * 0.70);

   ufdbFree( db->table );
   db->table = newtable;
}


void UFDBmemDBinsert( 
   struct UFDBmemDB * db, 
   char *             key, 
   char *             value )
{
   UFDBmemDBkv *      kv;
   unsigned int       i;

#if 0
   ufdbLogError( "  UFDBmemDBinsert( db=%08x, %s, XXX )", db, key );
#endif

   if (key == NULL)
   {
      ufdbLogError( "UFDBmemDBinsert: key is NULL. value is %s  *****", value==NULL ? "NULL" : value );
      return;
   }
   if (*key == '\0')
   {
      ufdbLogError( "UFDBmemDBinsert: key is empty. value is %s  *****", value==NULL ? "NULL" : value );
      return;
   }

   kv = (struct UFDBmemDBkv *) ufdbMalloc( sizeof(UFDBmemDBkv) );
   if (kv == NULL)
   {
      ufdbLogFatalError( "UFDBmemDBinsert: cannot allocate memory" );
      return;
   }
   kv->key = ufdbStrdup( key );
   if (value == NULL)
      kv->value = NULL;
   else
      kv->value = ufdbStrdup( value );
   kv->hash = memDBhash( key );
   /* TO-DO: prevent duplicates */

   i = (unsigned int) (kv->hash % db->tableSize);
   kv->next = db->table[i];
   db->table[i] = kv;

   db->nEntries++;
   if (db->nEntries > db->optimalMaxEntries)
      memDBexpand( db );
}


UFDB_GCC_HOT
int UFDBmemDBfind( 
   struct UFDBmemDB * db,
   char *             key,
   char **            value  )
{
   UFDBmemDBkv *      kv;
   ufdb_memdb_hash_t  hash;
   unsigned int       i;

#if 0
   ufdbLogError( "  UFDBmemDBfind( db=%08x, %s )", db, key );
#endif

   if (db == NULL)
      return 0;

   hash = memDBhash( key );
   i = (unsigned int) (hash % db->tableSize);
   kv = db->table[i];
   while (kv != NULL)
   {
      if (kv->hash == hash  &&  strcmp( kv->key, key ) == 0)
      {
         *value = kv->value;
	 return 1;
      }
      kv = kv->next;
   }

   return 0;
}


UFDB_GCC_HOT
int UFDBmemDBfindDomain( 
   struct UFDBmemDB * db,
   char *             domain  )
{
   char *        d;
   unsigned int  i;
   UFDBmemDBkv * kv;

#if 0
   ufdbLogError( "  UFDBmemDBfindDomain( db=%08x, %s )", db, domain );
#endif

   if (db == NULL)
      return 0;

   /* When finding a domain we face a problem with subdomains: subdomains must be matched 
    * but they do not appear in the hashtable: only the parent domain is in the table.
    * Strategy: since the table is very small, do a linear search.
    */

   for (i = 0; i < db->tableSize; i++)
   {
      kv = db->table[i];
      while (kv != NULL)
      {
	 d = strstr( domain, kv->key );
	 if (d != NULL)
	 {
	    if (d == domain)
	    {
	       if (strcmp( d, kv->key ) == 0)
	          return 1;
	    }
	    else
	    {
	       if (*(d-1) == '.' &&  strcmp( d, kv->key ) == 0)
		  return 1;
	    }
	 }
         kv = kv->next;
      }
   }
   return 0;
}


void UFDBmemDBdeleteDB( 
   struct UFDBmemDB * db  )
{
   unsigned int  i;
   UFDBmemDBkv * kv;
   UFDBmemDBkv * next;

   for (i = 0;  i < db->tableSize;  i++)
   {
      kv = db->table[i];
      while (kv != NULL)
      {
	 ufdbFree( kv->key );
	 ufdbFree( kv->value );
         next = kv->next;
	 ufdbFree( kv );
	 kv = next;
      }
   }
   ufdbFree( db );
}



void UFDBmemDBprintUserDB(
   char *             prefix,
   struct UFDBmemDB * db  )
{
   unsigned int  i;
   UFDBmemDBkv * kv;

   if (db == NULL)
      return;

   if (UFDBglobalDebug)
      ufdbLogMessage( "   # hash table for \"%s\" has size %u and %u entries", prefix, db->tableSize, db->nEntries );

   for (i = 0;  i < db->tableSize;  i++)
   {
      kv = db->table[i];
      while (kv != NULL)
      {
	 if (UFDBglobalDebug > 2)
	    ufdbLogMessage( "   %s  %-15s  %6u  %22lu", prefix, kv->key, i, (unsigned long) kv->hash );
	 else
	    ufdbLogMessage( "   %s  %s", prefix, kv->key );
	 kv = kv->next;
      }
   }
}

