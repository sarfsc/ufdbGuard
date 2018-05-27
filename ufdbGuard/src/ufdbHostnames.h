/*
 * ufdbHostnames.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2013 by URLfilterDB with all rights reserved.
 *
 * Cached IP and hostname lookups.
 *
 * RCS $Id$
 */


#ifndef UFDB_HOSTNAMES_H_INCLUDED
#define UFDB_HOSTNAMES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

int UFDBfindCorrectHostNameForIP( char * ip, char * host );

#ifdef __cplusplus
}
#endif

#endif

