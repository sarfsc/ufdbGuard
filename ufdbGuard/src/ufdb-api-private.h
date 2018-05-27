/*
 * ufdb-api-private.h - URLfilterDB
 *
 * ufdbGuard API is copyrighted (C) 2007-2013 by URLfilterDB with all rights reserved.
 *
 * ufdbGuard API is used to integrate the functionality of ufdbGuard into
 * programs of 3rd parties.
 *
 * RCS $Id: ufdb-api-private.h,v 1.7 2015/08/29 12:43:47 root Exp root $
 */

#ifndef UFDB_UFDBAPI_PRIVATE_H_INCLUDED
#define UFDB_UFDBAPI_PRIVATE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>

extern volatile unsigned long UFDB_API_num_threads;
extern volatile unsigned long UFDB_API_num_url_lookups;
extern volatile unsigned long UFDB_API_num_url_matches;
extern volatile unsigned long UFDB_API_num_url_localnet;
extern volatile unsigned long UFDB_API_num_protocol_matches;
extern volatile unsigned long UFDB_API_num_https;
extern volatile unsigned long UFDB_API_num_bumps;
extern volatile unsigned long UFDB_API_num_regexp_matches;
extern volatile unsigned long UFDB_API_num_uncategorised_URL;
extern volatile unsigned long UFDB_API_num_safesearch;
extern volatile unsigned long UFDB_API_upload_seqno;

extern struct timeval UFDBcurrentTime;

extern double UFDBmaxTPS;
extern struct timeval UFDBmaxTPStime;

void UFDBresetTPScounters( void );

#ifdef __cplusplus
}
#endif

#endif

