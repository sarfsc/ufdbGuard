/*
 * crypt.c
 *
 * ufdbGuard is copyrighted (C) 2005-2013 by URLfilterDB with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * Encryption module
 *
 * $Id: crypt.c,v 1.12 2017/05/27 21:34:57 root Exp root $
 */

/* do not overprotect these tested functions. We need speed! */
#undef _FORTIFY_SOURCE

#include "ufdb.h"


/* We want agressive optimisations: */
/* These pragmas only work for GCC 4.4 and above */
#if (__GNUC__ > 4)  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 4)
#pragma GCC optimize ("O3")
#pragma GCC optimize ("inline-functions")
#endif


void ufdbCryptInit( ufdbCrypt * uc, const unsigned char * key, unsigned int keySize )
{
   int             i;
   unsigned int    t, u;
   unsigned int    ki;
   unsigned int    sti;
   unsigned char * state;
      
   state = uc->state;

   uc->x = 0;
   uc->y = 0;
   for (i = 0; i < 256; i++)
      state[i] = i;

   ki = 0;
   sti = 0;
   for (i = 0; i < 256; i++)
   {
      t = state[i];
      sti = (sti + key[ki] + t) & 0x00FF;
      u = state[sti];
      state[sti] = t;
      state[i] = u;
      ki = (ki + 1) % keySize;
   }
}

 
static UFDB_GCC_INLINE unsigned int _nextByte( ufdbCrypt * uc )
{
   unsigned int x;
   unsigned int y;
   unsigned int sx, sy;
   unsigned char * state;
      
   state = uc->state;

   x = (uc->x + 1) & 0x00FF;
   sx = state[x];

   y = (sx + uc->y) & 0x00FF;
   sy = state[y];

   uc->x = x;
   uc->y = y;
   state[y] = sx;
   state[x] = sy;

   return state[(sx + sy) & 0x00FF];
}


void ufdbEncryptText( 
   ufdbCrypt *           uc, 
   unsigned char *       dest, 
   const unsigned char * src, 
   unsigned int          len )
{
   while (len > 0)
   {
      *dest = *src ^ _nextByte( uc );
      src++;
      dest++;
      len--;
   }
}


