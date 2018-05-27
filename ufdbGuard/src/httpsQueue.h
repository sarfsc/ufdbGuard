/*
 * httpsQueue.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2012 by URLfilterDB with all rights reserved.
 * 
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: httpsQueue.h,v 1.8 2018/05/24 19:52:51 root Exp root $
 */

#ifndef UFDB_HTTPSQUEUE_H_INCLUDED
#define UFDB_HTTPSQUEUE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ufdb.h"

/* A HTTPS/SSL verifier thread mostly waits for network I/O
 * and uses relatively little CPU.  To have a high throughput
 * in the SSL verification, a "high" number of threads are used.
 * Individual threads may wait for eachother when the agressive
 * mode is used OR when internal routines do not use UFDB_API_ALLOW_QUEUING.
 */
#define UFDB_NUM_HTTPS_VERIFIERS   32

void initHttpsQueue( void );
int ufdbHttpsQueueRequest( const char * hostname, int portnumber );
void ufdbGetHttpsRequest( char * hostname, int * portnumber );
int UFDBhttpsVerificationQueued( void );

#ifdef __cplusplus
}
#endif

#endif

