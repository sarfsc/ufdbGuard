/*
 * ufdblib.h - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2013 by URLfilterDB with all rights reserved.
 *
 * RCS $Id: ufdblib.h,v 1.98 2017/02/13 18:48:15 root Exp root $
 */

#ifndef UFDB_UFDBLIB_H_INCLUDED
#define UFDB_UFDBLIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "ufdb.h"

#include <time.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/utsname.h>


extern volatile int    UFDBglobalStatus;
extern volatile int    UFDBglobalDatabaseStatus;
extern volatile int    UFDBglobalTerminating;
extern volatile int    UFDBglobalReconfig;
extern          int    UFDBglobalNworkers;
extern          int    UFDBglobalDebug;
extern          int    UFDBglobalCrashOnFatal;
extern          int    UFDBglobalLogAllRequests;
extern          int    UFDBglobalLogPass;
extern          int    UFDBglobalLogBlock;
extern          int    UFDBglobalSafeSearch;
extern          int    UFDBglobalSkipSafeCategory;
extern          int    UFDBglobalPeek;
extern          int    UFDBglobalStripDomainFromUsername;
extern          int    UFDBglobalDebugRedirect;
extern          int    UFDBglobalUploadStats;
extern          int    UFDBglobalYoutubeEdufilter;
extern          int    UFDBglobalAllowGoogleHTTPSusingIP;

extern time_t          UFDBglobalDatabaseLoadTime;
extern char            UFDBglobalDateOfCheckedDB[32];
extern int    	       UFDBglobalFatalError;
extern char            UFDBglobalLicenseStatus[512];
extern char            UFDBglobalRedirectHttps[1024];
extern char            UFDBglobalRedirectBumpedHttps[1024];
extern char * UFDBglobalConfigFile;
extern int    UFDBglobalDebugSkype;
extern int    UFDBglobalDebugGtalk;
extern int    UFDBglobalDebugYahooMsg;
extern int    UFDBglobalDebugAim;
extern int    UFDBglobalDebugFBchat;
extern int    UFDBglobalDebugCitrixOnline;
extern int    UFDBglobalAnydeskOverHttps;
extern int    UFDBglobalTeamviewerOverHttps;
extern int    UFDBglobalDebugHttpd;
extern int    UFDBglobalDebugRegexp;
extern int    UFDBglobalSilentLogMessage;
extern int    UFDBglobalExpressionOptimisation;
extern int    UFDBglobalRefreshUserlistInterval;
extern int    UFDBglobalRefreshDomainlistInterval;
extern int    UFDBglobalDebugExternalScripts;
extern char   UFDBglobalUserName[31+1];
extern char * UFDBglobalPidFile;
extern char * UFDBglobalLogDir;
extern char * UFDBglobalEmailServer;
extern char * UFDBglobalMyHostname;
extern char * UFDBglobalAdminEmail;
extern char * UFDBglobalSenderEmail;
extern char * UFDBglobalExternalStatusCommand;
extern char * UFDBglobalYoutubeEdufilterID;
extern int    UFDBglobalURLlookupDelayDBreload;
extern int    UFDBglobalURLlookupResultDBreload;
extern int    UFDBglobalURLlookupResultFatalError;
extern int    UFDBglobalAnalyseUncategorisedURLs;
extern int    UFDBglobalLogUncategorisedURLs;
extern int    UFDBglobalPortNum;
extern int    UFDBglobalShowURLdetails;
extern int    UFDBglobalLogURLdetails;
extern int    UFDBglobalUploadCrashReports;
extern int    UFDBglobalLookupReverseIP;
extern int    UFDBglobalUseAlsoIPv6onWan;
extern int    UFDBglobalTunnelCheckMethod;
extern char * UFDBglobalSquidVersion;
extern int    UFDBglobalSquidHelperProtocol;
extern int    UFDBglobalSquidUsesSSLbump;
extern int    UFDBglobalHttpsWithHostname;
extern int    UFDBglobalHttpsOfficialCertificate;
extern int    UFDBglobalHttpsEnforceContentPeek;
extern int    UFDBglobalSkypeOverHttps;
extern int    UFDBglobalGtalkOverHttps;
extern int    UFDBglobalYahooMsgOverHttps;
extern int    UFDBglobalAimOverHttps;
extern int    UFDBglobalFBchatOverHttps;
extern int    UFDBglobalCitrixOnlineOverHttps;
extern int    UFDBglobalUnknownProtocolOverHttps;
extern int    UFDBglobalHttpsNoSSLv2;
extern int    UFDBglobalHttpsNoSSLv3;
extern int    UFDBglobalHttpdPort;
extern char   UFDBglobalHttpdInterface[256];
extern char   UFDBglobalHttpdImagesDirectory[256];
extern char   UFDBglobalInterface[256];
extern char   UFDBglobalCAcertsFile[1024];
extern char   UFDBglobalCAcertsDir[1024];
extern char   UFDBglobalFatalErrorRedirect[1024];
extern char   UFDBglobalLoadingDatabaseRedirect[1024];
extern struct UFDBmemTable UFDBglobalCheckedDB;
extern struct ufdbRegExp * UFDBglobalCheckedExpressions;
extern unsigned long UFDBglobalMaxLogfileSize;
volatile extern unsigned long UFDBglobalTunnelCounter;

extern struct ufdbSetting * lastSetting;
extern struct ufdbSetting * Setting;

extern struct Source * lastSource;
extern struct Source * saveSource;
extern volatile struct Source * SourceList;

extern struct Category * lastCat;
extern struct Category * Cat;

extern struct sgRewrite *   lastRewrite;
extern struct sgRewrite *   Rewrite;
extern struct ufdbRegExp *  lastRewriteRegExec;

extern struct ufdbTime * lastTime;
extern struct ufdbTime * TimeList;

extern FILE * globalErrorLog;
extern struct LogFile * globalLogFile;

extern struct LogFileStat * lastLogFileStat;
extern struct LogFileStat * LogFileStat;

extern struct TimeElement *lastTimeElement;
extern struct TimeElement *TimeElement;

extern struct Acl * lastAcl;
extern struct Acl * defaultAcl;
extern struct AclCategory * lastAclCategory;
extern struct Acl * AclList;

extern struct ufdbRegExp * lastRegExpDest;

extern char ** globalArgv;
extern char ** globalEnvp;
extern int globalDebugTimeDelta;
extern int globalPid;
extern int globalUpdate;
extern char * globalCreateDb;
extern int failsafe_mode;
extern int sig_hup;
extern int sig_other;
extern int httpsConnectionCacheSize;
extern char UFDBprogname[80];

extern time_t  UFDBlastIPcounterResetDate;

void increment_UFDB_API_num_https( void );

void UFDBdropPrivileges( const char * username );
void UFDBraisePrivileges( const char * username, const char * why );

typedef unsigned long ufdb_memdb_hash_t;

typedef struct UFDBmemDBkv
{
   char *               key;
   char *               value;
   ufdb_memdb_hash_t    hash;
   struct UFDBmemDBkv * next;
} UFDBmemDBkv;

struct UFDBmemDB
{
   unsigned int      tableSize;
   unsigned int      nEntries;
   unsigned int      optimalMaxEntries;
   UFDBmemDBkv **    table;
};

/* The following functions are not all threadsafe !
 * A memDB is created by one thread while other threads have no access.
 * After the creation and initialisation a memDB is static and
 * multiple threads can search in the memDB.
 */
struct UFDBmemDB * UFDBmemDBinit( void );
void UFDBmemDBinsert( struct UFDBmemDB * db, char * key, char * value );
int  UFDBmemDBfind( struct UFDBmemDB * db, char * key, char ** value );
int  UFDBmemDBfindDomain( struct UFDBmemDB * db, char * domain );
void UFDBmemDBdeleteDB( struct UFDBmemDB * db );
void UFDBmemDBprintUserDB( char * prefix, struct UFDBmemDB * db );

void UFDBappInit( void );
unsigned long UFDBappMemUsage( void );

void UFDBtimerInit( struct tms * t );
void UFDBtimerStop( struct tms * t );
void UFDBtimerPrint( struct tms * t, const char * tag );
void UFDBtimerPrintString( char * line, struct tms * t, const char * tag );
void UFDBlogTimer( struct tms * t, char * tag );

int UFDBloadDatabase( 
   struct UFDBmemTable * mtable, 
   char *                file    );

int UFDBloadExpressions( 
   struct ufdbRegExp ** exprList,
   char *               file     );

struct ufdbRegExp * UFDBoptimizeExprList( 
   char *               reSource,
   struct ufdbRegExp *  reList );

void ufdbFreeRegExprList( struct ufdbRegExp * re );

int  UFDBregcomp( regex_t * preg, const char * regex, int cflags );
void UFDBregfree( regex_t * preg );
int  UFDBregexec( const regex_t * preg, const char * string, size_t nmatch, regmatch_t pmatch[], int eflags );

struct Source * UFDBgetSourceList( void );
void UFDBsetSourceList( struct Source * sl );

int  ufdbSetCPU( char * CPUspec );
int  ufdbSetThreadCPUaffinity( int thread_num );
void ufdbResetCPUs( void );

const char * ufdbAPIstatusString( int api_code );
const char * ufdbDBstat2string( int status );
const char * ufdbStatus2string( int status );

#define URL_HIST_SIZE 180000

int    ufdbVerifyURLallCats( UFDBrevURL * revURL, char * URL );
int    ufdbRegisterUnknownURL( char * webserver, int portnumber, char * referer );
char * ufdbGetUnknownURLs( void );
void   ufdbResetUnknownURLs( void );

int UFDBopenSocket( const char * serverName, int port );

void UFDBregisterCountedIP( const char * address );
void UFDBinitializeIPcounters( void );
unsigned long UFDBgetNumberOfRegisteredIPs( void );

void UFDBregisterCountedUser( const char * username );
void UFDBinitializeUserCounters( void );
unsigned long UFDBgetNumberOfRegisteredUsers( void );

struct ufdbSetting {
  char *           name;
  char *           value;
  struct ufdbSetting * next;
};

void   ufdbSetting( const char * name, char * value );
struct ufdbSetting * ufdbSettingFindName( const char * );
char * ufdbSettingGetValue( const char * );

void   ufdbLogFile( int, int, char * );
void   ufdbGlobalSetLogging( int logging );
void   ufdbRotateLogfile( char * );
void   UFDBrotateLogfile( void );
void   ufdbSetGlobalErrorLogFile( int mutex_is_used );
void   UFDBcloseFilesNoLog( void );

void   ufdbLogMessage( const char * format, ... )
#ifdef __GNUC__
__attribute__ ((format(printf,1,2)))
#endif
;
void   ufdbLogError( const char *, ... )
#ifdef __GNUC__
__attribute__ ((format(printf,1,2)))
#endif
;
void   ufdbLogFatalError( const char *, ... )
#ifdef __GNUC__
__attribute__ ((format(printf,1,2)))
#endif
;

typedef void (*ufdb_crashfun_t)(void);
void ufdbRegisterFatalErrorCrashfun( ufdb_crashfun_t crfun );

void ufdbGetSysInfo( struct utsname * si );
long ufdbGetNumCPUs( void );

void UFDBlogConfig( void );
int  UFDBchangeStatus( int status );

int UFDBfindCorrectHostNameForIP( char * ip, char * host );

char * UFDBparseIPv6address( char * url, char * domain );
void   UFDBupdateURLwithNormalisedDomain( char * url, char * newDomain );
void   UFDBnormaliseIPv4( char * domain );

/*
 * strip a URL;
 * remove http:// prefix, remove www[0-9*]. prefix,
 * remove port number, remove username and password
 */
void UFDBstripURL(
   char * URL,                  /* input URL string */
   char * strippedUrl,          /* output char array (must be 1024 bytes) */
   char * domain,               /* output char array (must be 1024 bytes) */
   char * protocol,             /* output char array (must be 16 bytes) */
   int  * portnumber );         /* output integer */

void UFDBstripURL2( 
   char * URL, 			/* input URL string */
   int    stripwwwprefix,	/* input flag for stripping "www." prefix from URL */
   char * strippedUrl,  	/* output char array (must be UFDB_MAX_URL_LENGTH bytes) */
   char * domain,       	/* output char array (must be 1024 bytes) */
   char * protocol,		/* output char array (must be 16 bytes) */
   int  * portnumber );		/* output integer */

char * UFDBprintable( char * string );

char * UFDBfgets( 
   char * requestBuffer, 
   int    bufferSize, 
   FILE * fp );

char * UFDBfgetsNoNL( char * s, int size, FILE * stream );

int UFDBcalcCksum( char * mem, long size );

void ufdbExecutePstack( const char * reason );

#ifdef __cplusplus
}
#endif

#endif

