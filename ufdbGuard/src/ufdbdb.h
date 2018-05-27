/*
 * ufdbdb.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. and others with all rights reserved.
 *
 * RCS $Id: ufdbdb.h,v 1.5 2017/01/12 16:13:45 root Exp $
 */

#ifndef UFDB_UFDBDB_H_INCLUDED
#define UFDB_UFDBDB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ufdb.h"
#include <time.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/utsname.h>

UFDBrevURL * UFDBgenRevURL( UFDBthreadAdmin * admin, unsigned char * URL );
UFDBrevURL * UFDBgenRevURL4table( UFDBthreadAdmin * admin, unsigned char * URL );

void UFDBprintRevURL( UFDBrevURL * revURL );
void UFDBfreeRevURL( UFDBthreadAdmin * admin, UFDBrevURL * revURL );

int UFDBparseTableHeader( struct UFDBmemTable * memTable );
void UFDBparseTable( struct UFDBmemTable * memTable );
void UFDBfreeTableIndex_1_2( struct UFDBtable * t );

int UFDBlookup( UFDBthreadAdmin * admin, struct UFDBmemTable * mt, char * request );
int UFDBlookupRevUrl( struct UFDBtable * t, UFDBrevURL * revUrl );

int UFDBloadDatabase( struct UFDBmemTable * mtable, char * file );

const char * ufdbCategoryName( const char * domain );

#ifdef __cplusplus
}
#endif

#endif

