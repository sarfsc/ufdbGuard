/*
 * ufdbchkport.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2009 by URLfilterDB with all rights reserved.
 * 
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: ufdbchkport.h,v 1.11 2018/05/24 19:56:23 root Exp root $
 */

#ifndef UFDB_UFDBCHKPORT_H_INCLUDED
#define UFDB_UFDBCHKPORT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ufdb.h"

#include <openssl/ssl.h>


int UFDBloadAllowedHTTPSsites( char * filename );

#define UFDBgetTunnelCheckMethod()		UFDBglobalTunnelCheckMethod
void UFDBsetTunnelCheckMethod( int method );

int UFDBinitHTTPSchecker( void );

int UFDBopenssl_connect( const char * hostname, int portnumber, int fd, SSL ** ssl );
int UFDBopenssl_read( char * buf, int bufsize, SSL * ssl );
int UFDBopenssl_write( char * buf,  int bufsize,  SSL * ssl );
void UFDBopenssl_close( SSL * ssl );

/* Check for a tunnel.
 *
 * Valid flags are:
 * UFDB_API_ALLOW_QUEUING
 *
 * return values are:
 * UFDB_API_OK:          regular https traffic
 * UFDB_API_REQ_QUEUED:  request to queued for an other thread
 * UFDB_API_ERR_TUNNEL:  https channel is tunneled
 *
 * NOTE: This function may take 3 seconds to complete when the "agressive" option is used !
 */
int UFDBcheckForHTTPStunnel( const char * hostname, int portnumber, int flags );

void * UFDBhttpsTunnelVerifier( void * ptr );

int UFDBsslPeekServer(
   const char *  hostname,
   int           portnumber,
   char *        cn,
   int *         certErrors,
   char **       content,
   int           worker  );

struct httpsInfo 
{
   int     status;
   time_t  t;
   char *  content;
   char    category[64];
   char    cn[1024];
};

#ifdef __cplusplus
}
#endif

#endif

