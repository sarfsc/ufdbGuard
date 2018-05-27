/*
 * httpserver.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2013 by URLfilterDB with all rights reserved.
 *
 * RCS $Id: httpserver.h,v 1.8 2014/11/09 18:10:35 root Exp root $
 */

#ifndef UFDB_HTTPSERVER_H_INCLUDED
#define UFDB_HTTPSERVER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ufdb.h"

#define UFDBHTTPD_PID_FILE	DEFAULT_PIDDIR "/ufdbhttpd.pid"

extern char * globalHttpdPidFile;

void writeHttpdPidFile( void );
void removeHttpdPidFile( void );

void ufdbSimulateHttpServer( 
	const char * interface, 
	int port, 
	const char * username,
	const char * imagesDirectory, 
	int flags );

#ifdef __cplusplus
}
#endif

#endif

