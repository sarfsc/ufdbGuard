/*
 * ufdbglobals.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2016 by URLfilterDB with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 * 
 * RCS $Id: ufdbglobals.c,v 1.7 2017/02/13 18:45:06 root Exp root $
 */

#undef _FORTIFY_SOURCE

#define UFDB_MALLOC_DEBUG     	 0



#include "ufdb.h"
#include "ufdblib.h"


/* align the following data to start on a new cache line */
UFDB_GCC_ALIGN64
volatile int    UFDBglobalStatus = UFDB_API_STATUS_VIRGIN;
volatile int    UFDBglobalDatabaseStatus = UFDB_API_STATUS_DATABASE_OK;
volatile int    UFDBglobalTerminating = 0;
volatile int    UFDBglobalReconfig = 1;
	 int    UFDBglobalNworkers = 68;
         int    UFDBglobalDebug = 0;			/* TO-DO: or keep volatile ??? */
         int    UFDBglobalCrashOnFatal = 0;
         int    UFDBglobalLogAllRequests = 0;
         int    UFDBglobalLogPass = 0;
         int    UFDBglobalLogBlock = 0;
         int    UFDBglobalSafeSearch = 1;
         int    UFDBglobalSkipSafeCategory = 0;
         int    UFDBglobalPeek = 0;

         int    UFDBglobalStripDomainFromUsername = 0;
         int    UFDBglobalDebugRedirect = 0;
         int    UFDBglobalUploadStats = 1;
         int    UFDBglobalYoutubeEdufilter = 0;
         int    UFDBglobalAllowGoogleHTTPSusingIP = 0;

volatile struct Source * SourceList = NULL;

volatile unsigned long UFDBglobalTunnelCounter = 0;
/* the following counters are heavily used */
UFDB_GCC_ALIGN64
volatile unsigned long UFDB_API_num_url_lookups = 0;	/* very hot variable */
volatile unsigned long UFDB_API_upload_seqno = 0;
volatile unsigned long UFDB_API_num_safesearch = 0;
UFDB_GCC_ALIGN64
volatile unsigned long UFDB_API_num_uncategorised_URL = 0;
volatile unsigned long UFDB_API_num_url_localnet = 0;
volatile unsigned long UFDB_API_num_protocol_matches = 0;
volatile unsigned long UFDB_API_num_bumps = 0;
UFDB_GCC_ALIGN64
volatile unsigned long UFDB_API_num_https = 0;		/* very hot variable */
UFDB_GCC_ALIGN64
volatile unsigned long UFDB_API_num_url_matches = 0;	/* very hot variable */
volatile unsigned long UFDB_API_num_regexp_matches = 0;
volatile unsigned long UFDB_API_num_threads = 0;

char   UFDBglobalLicenseStatus[512] = "unknown";
char   UFDBglobalRedirectHttps[1024] = UFDB_DEFAULT_HTTPS_REDIRECTION_URL;
char   UFDBglobalRedirectBumpedHttps[1024] = UFDB_DEFAULT_BUMPED_HTTPS_REDIRECTION_URL;
time_t UFDBglobalDatabaseLoadTime = 0;
char   UFDBglobalDateOfCheckedDB[32] = "";
int    UFDBglobalFatalError = 0;
char * UFDBglobalConfigFile = (char *) DEFAULT_CONFIGFILE;
int    UFDBglobalDebugSkype = 0;
int    UFDBglobalDebugGtalk = 0;
int    UFDBglobalDebugYahooMsg = 0;
int    UFDBglobalDebugAim = 0;
int    UFDBglobalDebugFBchat = 0;
int    UFDBglobalDebugCitrixOnline = 0;
int    UFDBglobalDebugHttpd = 0;
int    UFDBglobalDebugRegexp = 0;
int    UFDBglobalSilentLogMessage = 0;
int    UFDBglobalExpressionOptimisation = 1;
int    UFDBglobalRefreshUserlistInterval = UFDB_DEFAULT_REFRESH_USERLIST;
int    UFDBglobalRefreshDomainlistInterval = UFDB_DEFAULT_REFRESH_DOMAINLIST;
int    UFDBglobalDebugExternalScripts = 0;
char   UFDBglobalUserName[31+1] = "";
char * UFDBglobalPidFile = NULL;
char * UFDBglobalLogDir = NULL;
char * UFDBglobalEmailServer = NULL;
char * UFDBglobalMyHostname = NULL;
char * UFDBglobalAdminEmail = NULL;
char * UFDBglobalSenderEmail = NULL;
char * UFDBglobalExternalStatusCommand = NULL;
char * UFDBglobalYoutubeEdufilterID = (char *) "none";
int    UFDBglobalURLlookupDelayDBreload = 0;
int    UFDBglobalURLlookupResultDBreload = UFDB_ALLOW;
int    UFDBglobalURLlookupResultFatalError = UFDB_ALLOW;
int    UFDBglobalAnalyseUncategorisedURLs = 1;
int    UFDBglobalLogUncategorisedURLs = 0;
int    UFDBglobalPortNum = UFDB_DAEMON_PORT;
int    UFDBglobalShowURLdetails = 0;
int    UFDBglobalLogURLdetails = 0;
int    UFDBglobalUploadCrashReports = 1;
int    UFDBglobalLookupReverseIP = 0;
int    UFDBglobalUseAlsoIPv6onWan = 1;

int    UFDBglobalTunnelCheckMethod = UFDB_API_HTTPS_CHECK_OFF;
char * UFDBglobalSquidVersion = NULL;
char   UFDBglobalVersion[] = UFDB_VERSION;
int    UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL2;
int    UFDBglobalSquidUsesSSLbump = 0;
int    UFDBglobalHttpsWithHostname = 0;
int    UFDBglobalHttpsOfficialCertificate = 0;
int    UFDBglobalHttpsEnforceContentPeek = 0;
int    UFDBglobalSkypeOverHttps = 0;
int    UFDBglobalGtalkOverHttps = 0;
int    UFDBglobalYahooMsgOverHttps = 0;
int    UFDBglobalAimOverHttps = 0;
int    UFDBglobalFBchatOverHttps = 0;
int    UFDBglobalCitrixOnlineOverHttps = 0;
int    UFDBglobalAnydeskOverHttps = 0;
int    UFDBglobalTeamviewerOverHttps = 0;
int    UFDBglobalHttpsNoSSLv2 = 1;
int    UFDBglobalHttpsNoSSLv3 = 0;
int    UFDBglobalUnknownProtocolOverHttps = 1;

int    UFDBglobalHttpdPort = 0;
unsigned long UFDBglobalMaxLogfileSize = 200 * 1024 * 1024;	/* 200 MB */
char   UFDBglobalInterface[256] = "all";
char   UFDBglobalCAcertsFile[1024] = "";
char   UFDBglobalCAcertsDir[1024] = "";
char   UFDBglobalFatalErrorRedirect[1024] = "http://cgibin.urlfilterdb.com/cgi-bin/URLblocked.cgi?" 
		"category=fatal-error";
char   UFDBglobalLoadingDatabaseRedirect[1024] = "http://cgibin.urlfilterdb.com/cgi-bin/URLblocked.cgi?" 
		"category=loading-database";
char   UFDBglobalHttpdInterface[256] = "all";
char   UFDBglobalHttpdImagesDirectory[256] = ".";
struct ufdbRegExp * UFDBglobalCheckedExpressions = NULL;
struct UFDBmemTable UFDBglobalCheckedDB = { NULL, { NULL, 0, NULL }, NULL, 0, 0, 0, 0, 0, "", "", "", "" };

time_t        UFDBlastIPcounterResetDate = 0;

struct ufdbSetting * lastSetting = NULL;
struct ufdbSetting * Setting = NULL;

struct Source * lastSource = NULL;

struct Category * lastCat = NULL;
struct Category * Cat = NULL;

struct sgRewrite * lastRewrite = NULL;
struct sgRewrite * Rewrite = NULL;
struct ufdbRegExp *  lastRewriteRegExec = NULL;

struct ufdbTime * lastTime = NULL;
struct ufdbTime * TimeList = NULL;

FILE * globalErrorLog = NULL;
struct LogFile * globalLogFile = NULL;

struct LogFileStat * lastLogFileStat;
struct LogFileStat * LogFileStat;

struct TimeElement * lastTimeElement = NULL;
struct TimeElement * TimeElement = NULL;

struct Acl * lastAcl = NULL;
struct Acl * defaultAcl = NULL;
struct Acl * AclList = NULL;
struct AclCategory * lastAclCategory = NULL;

struct ufdbRegExp * lastRegExpDest = NULL;

char ** globalArgv;
char ** globalEnvp;
int     globalDebugTimeDelta = 0;
int     globalPid = 0;
int     globalUpdate = 0;
char *  globalCreateDb = NULL;
int     failsafe_mode = 0;
int     sig_hup = 0;
int     sig_other = 0;
int     httpsConnectionCacheSize = UFDB_DEF_HTTPS_CONN_CACHE_SIZE;
char    UFDBprogname[80];


struct Source * UFDBgetSourceList( void )
{
   if (UFDBglobalDebug > 3)
      ufdbLogMessage( "UFDBgetSourceList: SourceList is %s", SourceList==NULL ? "NULL" : "defined" );

   return (struct Source *) SourceList;
}


void UFDBsetSourceList( struct Source * sl )
{
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "UFDBsetSourceList: SourceList set to %s", sl==NULL ? "NULL" : "defined" );

   SourceList = sl;
}


