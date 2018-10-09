%{

/*
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * squidGuard is copyrighted (C) 1998 by
 * ElTele Øst AS, Oslo, Norway, with all rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (version 2) as
 * published by the Free Software Foundation.  It is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License (GPL) for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * (GPL2) along with this program.
 */

#define YYMALLOC  ufdbMalloc
#define YYFREE    ufdbFree

#define UFDB_LOG_USER_QUOTA   0   /* TODO remove */

#undef UFDB_DEBUG_ACL_ACCESS

#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"

#define UFDB_DEBUG 0

#ifdef UFDB_DEBUG
#undef YYDEBUG
#define YYDEBUG 1
#endif

#define UFDB_TIME_DEBUG 0

#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <grp.h>
#include <syslog.h>
#include <signal.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>


FILE * yyin;
FILE * yyout;
int    lineno;

extern struct ufdbSetting * lastSetting;
extern struct ufdbSetting * Setting;

extern struct Source * lastSource;

extern struct Category * lastCat;
extern struct Category * Cat;

extern struct sgRewrite * lastRewrite;
extern struct sgRewrite * Rewrite;
extern struct ufdbRegExp *  lastRewriteRegExec;

extern struct ufdbTime * lastTime;
extern struct ufdbTime * TimeList;

extern struct LogFile * globalLogFile;

extern struct LogFileStat * lastLogFileStat;
extern struct LogFileStat * LogFileStat;

extern struct TimeElement * lastTimeElement;
extern struct TimeElement * TimeElement;

extern struct Acl * lastAcl;
extern struct Acl * defaultAcl;
extern struct Aclcategory * lastAclcategory;
 
extern struct ufdbRegExp * lastRegExpDest;

extern int    httpsConnectionCacheSize;

extern int    globalDebugTimeDelta;

static int   numTimeElements = 0;
static int * TimeElementsEvents = NULL;

static int   time_switch = 0;
static int   date_switch = 0;

static void setRefreshUserlist( int nmin );
static void setRefreshDomainlist( int nmin );
static void ufdbSourceGroup( int groupType, char * groupName );
static void ufdbSourceUserQuota( const char * seconds, const char * sporadic, const char * renew );
void sgAclSetValue( const char * what, char * value, int allowed );
void sgIpv4( const char * name );
void ufdbFreeDomainDb( struct sgDb * dbp );

%}

%union {
  char * string;
  char * tval;
  char * dval;
  char * dvalcron;
  int    integer;
}

%token ADMINISTRATOR QSTRING
%token CHECK_PROXY_TUNNELS QUEUE_CHECKS AGGRESSIVE LOG_ONLY 
%token ON OFF 
%token CHAR_MINUS CHAR_I CHAR_EXCLAMATION
%token IGNORECASE
%token COMMA EQUAL PORT
%token HTTP_SERVER INTERFACE IMAGES 
%token HTTPS_PROHIBIT_INSECURE_SSLV2 HTTPS_PROHIBIT_INSECURE_SSLV3
%token HTTPS_CONNECTION_CACHE_SIZE
%token IDENTIFIER WORD END START_BRACKET STOP_BRACKET WEEKDAY
%token CATEGORY REWRITE ACL CPUS TIME TVAL DVAL DVALCRON
%token BLOCK_BUMPED_CONNECT EVALUATE_AND EVALUATE_OR
%token SOURCE CIDR IPV4CLASS CONTINUE
%token IPV4ADDR IPV6ADDR IPV6NET
%token DBHOME DOMAINLIST EXECDOMAINLIST REFRESHDOMAINLIST 
%token URLLIST EXPRESSIONLIST CACERTS CACERTSDIR 
%token IPV4 IPV4LIST IPV6 IPV6LIST
%token DOMAIN UNIX LDAP USER USERLIST EXECUSERLIST REFRESHUSERLIST 
%token USERQUOTA GROUP 
%token NL NUMBER NUMBERS
%token PASS REDIRECT SUBST CHAR MINUTELY HOURLY DAILY WEEKLY DATE
%token REDIRECT_FATAL_ERROR REDIRECT_LOADING_DATABASE REDIRECT_HTTPS REDIRECT_BUMPED_HTTPS
%token WITHIN OUTSIDE ELSE ANONYMOUS SPORADIC
%token PIDFILE LOGFILE LOGDIR LOGPASS LOGBLOCK LOGALL LOGALL_HTTPD
%token MAIL_SERVER MY_HOSTNAME ADMIN_EMAIL SENDER_EMAIL EXTERNAL_STATUS_COMMAND
%token TOKEN_ALLOW TOKEN_DENY
%token YOUTUBE_EDUFILTER YOUTUBE_EDUFILTER_ID ALLOW_GOOGLE_HTTPS_USING_IP 
%token URL_LOOKUP_RESULT_DB_RELOAD URL_LOOKUP_RESULT_FATAL_ERROR
%token URL_LOOKUP_DELAY_DB_RELOAD
%token OPTION UFDB_SHOW_URL_DETAILS UFDB_LOG_URL_DETAILS
%token SQUID_VERSION SQUID_USES_SSL_BUMP
%token UPLOAD_CRASH_REPORTS LOOKUP_REVERSE_IP USE_IPV6_ON_WAN
%token STRIP_DOMAIN_FROM_USERNAME
%token UFDB_DEBUG_FILTER
%token UFDB_DEBUG_SKYPE_PROBES UFDB_DEBUG_GTALK_PROBES 
%token UFDB_DEBUG_YAHOOMSG_PROBES UFDB_DEBUG_AIM_PROBES UFDB_DEBUG_FBCHAT_PROBES
%token UFDB_DEBUG_CITRIXONLINE_PROBES
%token UFDB_EXPRESSION_OPTIMISATION UFDB_EXPRESSION_DEBUG UFDB_DEBUG_EXTERNAL_SCRIPTS
%token UFDB_NUM_WORKER_THREADS
%token ENFORCE_HTTPS_WITH_HOSTNAME ENFORCE_HTTPS_OFFICAL_CERTIFICATE
%token ALLOW_SKYPE_OVER_HTTPS ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS
%token ALLOW_GTALK_OVER_HTTPS ALLOW_YAHOOMSG_OVER_HTTPS
%token ALLOW_AIM_OVER_HTTPS ALLOW_FBCHAT_OVER_HTTPS ALLOW_CITRIXONLINE_OVER_HTTPS
%token ALLOW_ANYDESK_OVER_HTTPS ALLOW_TEAMVIEWER_OVER_HTTPS
%token ANALYSE_UNCATEGORISED LOG_UNCATEGORISED_URLS UPLOAD_STATS
%token SAFE_SEARCH MAX_LOGFILE_SIZE
%token DENY_MODE


%type <string> IDENTIFIER
%type <string> WORD 
%type <string> QSTRING
%type <string> WEEKDAY
%type <string> NUMBER
%type <string> NUMBERS
%type <tval>   TVAL
%type <string> DVAL
%type <string> DVALCRON
%type <string> CHAR
%type <string> SUBST 
%type <string> IPV4ADDR
%type <string> IPV6ADDR
%type <string> IPV6NET
%type <string> DBHOME LOGDIR
%type <string> CIDR
%type <string> IPV4CLASS
%type <string> dval
%type <string> dvalcron
%type <string> tval
%type <string> date
%type <string> ttime
%type <string> qidentifier

%type <integer> check_proxy_tunnel_option
%type <integer> allow_or_deny
%type <integer> on_or_off

%%

start: statements
       ; 

qidentifier:
	   	  IDENTIFIER    { $$ = $1; }
	 	| QSTRING       { $$ = $1; }
	 	;

https_cache_size:
           	  HTTPS_CONNECTION_CACHE_SIZE NUMBER   { httpsConnectionCacheSize = atoi( $2 ); ufdbFree( $2 ); }
	 	;

allow_or_deny:
		  TOKEN_ALLOW   { $$ = UFDB_ALLOW; }
	        | TOKEN_DENY    { $$ = UFDB_DENY; }
		;

on_or_off:	
	   	  ON    { $$ = 1; }
	 	| OFF   { $$ = 0; }
		| error { ufdbLogFatalError( "syntax error at line %d: expected 'on' or 'off'", lineno );   }
	 	;

log_pass:
	   	  LOGPASS on_or_off   	{ UFDBglobalLogPass = $2; }
	 	;

log_block:
	   	  LOGBLOCK on_or_off   	{ UFDBglobalLogBlock = $2; }
	 	;

log_all:
	   	  LOGALL on_or_off     	{ UFDBglobalLogAllRequests = $2; }
	 	;

logall_httpd:
	   	  LOGALL_HTTPD on_or_off     	 { UFDBglobalDebugHttpd = $2; }
	 	;

youtube_edufilter:
		  YOUTUBE_EDUFILTER on_or_off    { UFDBglobalYoutubeEdufilter = $2; }
	        ;

youtube_edufilter_id:
		  YOUTUBE_EDUFILTER_ID QSTRING   { UFDBglobalYoutubeEdufilterID = $2; }
	        ;

allow_google_https_using_ip:
		  ALLOW_GOOGLE_HTTPS_USING_IP on_or_off   { UFDBglobalAllowGoogleHTTPSusingIP = $2; }
	        ;

deny_mode:
		  DENY_MODE on_or_off   { UFDBglobalDenyMode = $2; }
		  ;

debug_filter:
		  UFDB_DEBUG_FILTER on_or_off   { UFDBglobalDebug = $2; }
	        | UFDB_DEBUG_FILTER NUMBER      { UFDBglobalDebug = atoi( $2 ); }
	        ;

enforce_https_with_hostname:
	   	  ENFORCE_HTTPS_WITH_HOSTNAME on_or_off         
	   	    { ufdbLogError( "line %d: option enforce-https-with-hostname must be part of the security category", lineno );  }
	 	;

enforce_https_offical_certificate:
           	ENFORCE_HTTPS_OFFICAL_CERTIFICATE on_or_off   
	   	  { ufdbLogError( "line %d: option enforce-https-official-certificate must be part of the security category", lineno );  }
	 	;

https_prohibit_insecure_sslv2:
	   	  HTTPS_PROHIBIT_INSECURE_SSLV2 on_or_off       
	   	    { ufdbLogError( "line %d: option https-prohibit-insecure-sslv2 must be part of the security category", lineno );  }
	 	;

https_prohibit_insecure_sslv3:
	   	  HTTPS_PROHIBIT_INSECURE_SSLV3 on_or_off       
	   	    { ufdbLogError( "line %d: option https-prohibit-insecure-sslv3 must be part of the security category", lineno );  }
	 	;


check_proxy_tunnel_option:
	   	  OFF           { $$ = UFDB_API_HTTPS_CHECK_OFF; }
	 	| QUEUE_CHECKS  { $$ = UFDB_API_HTTPS_CHECK_QUEUE_CHECKS; }
	 	| AGGRESSIVE    { $$ = UFDB_API_HTTPS_CHECK_AGGRESSIVE; }
	 	| LOG_ONLY      { $$ = UFDB_API_HTTPS_LOG_ONLY; }
	 	;

check_proxy_tunnels:
           	  CHECK_PROXY_TUNNELS check_proxy_tunnel_option { UFDBsetTunnelCheckMethod( $2 ); }
	 	;

admin_spec:    
		  ADMINISTRATOR QSTRING  { ufdbSetting("administrator",$2); }
	   	;

dbhome:    
		  DBHOME qidentifier { ufdbSetting("dbhome",$2); }
		| DBHOME WORD        { ufdbSetting("dbhome",$2); }
           	;

refreshuserlist:
                  REFRESHUSERLIST  NUMBER     { setRefreshUserlist( atoi($2) ); }
	        ;

refreshdomainlist:
                  REFRESHDOMAINLIST  NUMBER   { setRefreshDomainlist( atoi($2) ); }
	        ;

url_lookup_result_db_reload:
		  URL_LOOKUP_RESULT_DB_RELOAD allow_or_deny
		  			{ UFDBglobalURLlookupResultDBreload = $2; }

url_lookup_result_fatal_error:
		  URL_LOOKUP_RESULT_FATAL_ERROR allow_or_deny
		  			{ UFDBglobalURLlookupResultFatalError = $2; }

url_lookup_delay_db_reload:
		  URL_LOOKUP_DELAY_DB_RELOAD on_or_off
		  			{ UFDBglobalURLlookupDelayDBreload = $2; }

squid_version:
		  SQUID_VERSION QSTRING
		  			{ ufdbFree( UFDBglobalSquidVersion );  UFDBglobalSquidVersion = $2;  }
	        ;

num_worker_threads:
		  UFDB_NUM_WORKER_THREADS NUMBER
		  			{ int new_n_workers;
					  /* ONLY increase #workers */
					  new_n_workers = atoi( $2 );  
					  if (new_n_workers < 32)
					  {
					     ufdbLogError( "num-worker-threads must be at least 32" );
					     new_n_workers = 32;
					  }
					  else if (new_n_workers > UFDB_MAX_THREADS)
					  {
					     ufdbLogError( "num-worker-threads can be at most %d", UFDB_MAX_THREADS );
					     new_n_workers = UFDB_MAX_THREADS;
					  }
					  if (new_n_workers > UFDBglobalNworkers)
					     UFDBglobalNworkers = new_n_workers;
					}
	        ;

upload_crash_reports:
		  UPLOAD_CRASH_REPORTS on_or_off
		  			{ UFDBglobalUploadCrashReports = $2;  }
	        ;

lookup_reverse_ip:
		  LOOKUP_REVERSE_IP on_or_off
		  			{ UFDBglobalLookupReverseIP = $2;  }
	        ;

use_ipv6_on_wan:
		  USE_IPV6_ON_WAN on_or_off
		  			{ UFDBglobalUseAlsoIPv6onWan = $2;  }
	        ;

squid_uses_ssl_bump:
		  SQUID_USES_SSL_BUMP on_or_off
		  			{ UFDBglobalSquidUsesSSLbump = $2;  }
	        ;

ufdb_log_url_details:
		  UFDB_LOG_URL_DETAILS on_or_off  { UFDBglobalLogURLdetails = $2; }
	        ;

ufdb_show_url_details:
		  UFDB_SHOW_URL_DETAILS on_or_off  { UFDBglobalShowURLdetails = $2; }
	        ;

ufdb_debug_skype_probes:
		  UFDB_DEBUG_SKYPE_PROBES on_or_off  { UFDBglobalDebugSkype = $2; }
	        ;

ufdb_debug_gtalk_probes:
		  UFDB_DEBUG_GTALK_PROBES on_or_off  { UFDBglobalDebugGtalk = $2; }
	        ;

ufdb_debug_yahoomsg_probes:
		  UFDB_DEBUG_YAHOOMSG_PROBES on_or_off  { UFDBglobalDebugYahooMsg = $2; }
	        ;

ufdb_debug_aim_probes:
		  UFDB_DEBUG_AIM_PROBES on_or_off  { UFDBglobalDebugAim = $2; }
	        ;

ufdb_debug_fbchat_probes:
		  UFDB_DEBUG_FBCHAT_PROBES on_or_off  { UFDBglobalDebugFBchat = $2; }
	        ;

ufdb_debug_citrixonline_probes:
		  UFDB_DEBUG_CITRIXONLINE_PROBES on_or_off  { UFDBglobalDebugCitrixOnline = $2; }
	        ;

ufdb_expression_optimisation:
		  UFDB_EXPRESSION_OPTIMISATION on_or_off  { UFDBglobalExpressionOptimisation = $2; }
	        ;

ufdb_expression_debug:
		  UFDB_EXPRESSION_DEBUG on_or_off  { UFDBglobalDebugRegexp = $2; }
	        ;

ufdb_debug_external_scripts:
		  UFDB_DEBUG_EXTERNAL_SCRIPTS on_or_off  { UFDBglobalDebugExternalScripts = $2; }
	        ;

mail_server:
		  MAIL_SERVER QSTRING  { ufdbFree( UFDBglobalEmailServer ); UFDBglobalEmailServer = $2; }
	        ;

my_hostname:
		  MY_HOSTNAME QSTRING  { ufdbFree( UFDBglobalMyHostname ); UFDBglobalMyHostname = $2; }
	        ;

admin_email:
		  ADMIN_EMAIL QSTRING  { ufdbFree( UFDBglobalAdminEmail ); UFDBglobalAdminEmail = $2; }
	        ;

sender_email:
		  SENDER_EMAIL QSTRING { ufdbFree( UFDBglobalSenderEmail ); UFDBglobalSenderEmail = $2; }
	        ;

external_status_command:
		  EXTERNAL_STATUS_COMMAND  QSTRING  {  ufdbFree( UFDBglobalExternalStatusCommand );  UFDBglobalExternalStatusCommand = $2; }
	        ;

logdir:    
		  LOGDIR qidentifier   { ufdbSetting("logdir",$2); }
		| LOGDIR WORD          { ufdbSetting("logdir",$2); }
            	;

pidfile:    
		  PIDFILE qidentifier  { ufdbSetting("pidfile",$2); }
		| PIDFILE WORD         { ufdbSetting("pidfile",$2); }
            	;

port:	   
		  PORT NUMBER          { ufdbSetting( "port", $2 ); }
                ;

interface:
	          INTERFACE qidentifier    { 
#if HAVE_UNIX_SOCKETS
                                             ufdbLogError( "ufdbguardd is configured to use UNIX sockets.  \"interface\" is ignored." );
#else
		                             if (strcmp($2,"all")== 0)
						strcpy( UFDBglobalInterface, "all" );    
					     else
					        ufdbLogFatalError( "interface must be \"all\" or IP address" );
#endif
					     ufdbFree( $2 );
					   }
	        | INTERFACE IPV4ADDR { strcpy( UFDBglobalInterface, $2 );  ufdbFree( $2 ); }
		;

cpus:	   
		  CPUS NUMBER        { ufdbSetCPU( $2 ); ufdbFree( $2 ); }
         	| CPUS NUMBERS       { ufdbSetCPU( $2 ); ufdbFree( $2 ); }
           	;

upload_stats: 
               	  UPLOAD_STATS on_or_off
		     { UFDBglobalUploadStats = $2;   }
	        ;

redirect_https:
		  REDIRECT_HTTPS  QSTRING 
		     { strcpy( UFDBglobalRedirectHttps, $2 );  ufdbFree( $2 );  }
	        ;

redirect_bumped_https:
		  REDIRECT_BUMPED_HTTPS  QSTRING 
		     { strcpy( UFDBglobalRedirectBumpedHttps, $2 );  ufdbFree( $2 );  }
	        ;

redirect_loading_database:
		  REDIRECT_LOADING_DATABASE  QSTRING 
		     { strcpy( UFDBglobalLoadingDatabaseRedirect, $2 );  ufdbFree( $2 );  }
	        ;

redirect_fatal_error:
		  REDIRECT_FATAL_ERROR  QSTRING 
		     { strcpy( UFDBglobalFatalErrorRedirect, $2 );  ufdbFree( $2 );  }
	        ;

log_uncategorised_urls: 
                  LOG_UNCATEGORISED_URLS on_or_off
                     {
			UFDBglobalLogUncategorisedURLs = $2;   
		     }
	        ;

analyse_uncategorised: 
               	  ANALYSE_UNCATEGORISED on_or_off
		     { 
			UFDBglobalAnalyseUncategorisedURLs = $2;   
		     }
	        |
               	  ANALYSE_UNCATEGORISED NUMBER
		     {
			UFDBglobalAnalyseUncategorisedURLs = atoi( $2 ); 
			ufdbFree( $2 ); 
		     }
	       	;

strip_domain_from_username:
		  STRIP_DOMAIN_FROM_USERNAME on_or_off
		     {
		        UFDBglobalStripDomainFromUsername = $2;
		     }
	       ;

safe_search:
		  SAFE_SEARCH on_or_off
		     { UFDBglobalSafeSearch = $2; }
	        ;

max_logfile_size:
		  MAX_LOGFILE_SIZE NUMBER
		     { 
		       UFDBglobalMaxLogfileSize = strtoul( $2, NULL, 10 );
		       ufdbFree( $2 );
		       if (UFDBglobalMaxLogfileSize < 2 * 1024 * 1024)		/* minimum is 2 MB */
		          UFDBglobalMaxLogfileSize = 2 * 1024 * 1024;
		       if (UFDBglobalMaxLogfileSize > 2000000000)		/* maximum is 2 GB */
		          UFDBglobalMaxLogfileSize = 2000000000;
		     }
	        ;

httpd_option:
		  PORT EQUAL NUMBER        { UFDBglobalHttpdPort = atoi( $3 ); ufdbFree( $3 ); }
	        | INTERFACE EQUAL qidentifier { if (strcmp($3,"all")== 0)
						strcpy( UFDBglobalHttpdInterface, "all" );    
					     else
					        ufdbLogFatalError( "http-server interface must be \"all\" or IP address" );
					     ufdbFree( $3 );
					   }
	        | INTERFACE EQUAL WORD     { if (strcmp($3,"all")== 0)
						strcpy( UFDBglobalHttpdInterface, "all" );    
					     else
					        ufdbLogFatalError( "http-server interface must be \"all\" or IP address" );
					     ufdbFree( $3 );
					   }
	        | INTERFACE EQUAL IPV4ADDR { strcpy( UFDBglobalHttpdInterface, $3 );       ufdbFree( $3 ); }
		| IMAGES EQUAL qidentifier { strcpy( UFDBglobalHttpdImagesDirectory, $3 ); ufdbFree( $3 ); }
		| IMAGES EQUAL WORD        { strcpy( UFDBglobalHttpdImagesDirectory, $3 ); ufdbFree( $3 ); }
		;

httpd_options:
		  httpd_option COMMA httpd_options
		| httpd_option
	        ;

http_server_def:
		  HTTP_SERVER START_BRACKET httpd_options STOP_BRACKET
	        ;

category: 
		  CATEGORY qidentifier { ufdbCategory( $2 ); }
		| CATEGORY AGGRESSIVE  { ufdbCategory( ufdbStrdup("aggressive") );
		                         yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" ); }
		| CATEGORY TOKEN_ALLOW { ufdbCategory( ufdbStrdup("allow") );
		                         yyerror( (char *) "\"allow\" is a keyword and must be surrounded by quotes" ); }
		| CATEGORY TOKEN_DENY  { ufdbCategory( ufdbStrdup("deny") );
		                         yyerror( (char *) "\"deny\" is a keyword and must be surrounded by quotes" ); }
		| CATEGORY error       { ufdbCategory( ufdbStrdup("syntax-error") );
		                         yyerror( (char *) "erroneous category definition.  Perhaps the category ID is a reserved word?" ); }
             	;

category_block: 
		  category START_BRACKET category_contents STOP_BRACKET 
                       { ufdbCategoryEnd(); }
                ;

category_contents:
                | category_contents category_content
		;

category_content:  
 	      	  DOMAINLIST qidentifier         { ufdbCategoryDomainList( $2 ); }
 	      	| DOMAINLIST WORD                { ufdbCategoryDomainList( $2 ); }
            	| DOMAINLIST CHAR_MINUS          { ufdbCategoryDomainList( NULL ); }
		| EXECDOMAINLIST qidentifier     { ufdbCategoryExecDomainList( $2 ); }
            	| URLLIST qidentifier            { ufdbCategoryUrlList( $2 ); }
            	| URLLIST WORD                   { ufdbCategoryUrlList( $2 ); }
            	| URLLIST CHAR_MINUS             { ufdbCategoryUrlList( NULL ); }
            	| EXPRESSIONLIST qidentifier     { ufdbCategoryExpressionList( $2, "n" ); }
            	| EXPRESSIONLIST WORD            { ufdbCategoryExpressionList( $2, "n" ); }
            	| EXPRESSIONLIST CHAR_MINUS      { ufdbCategoryExpressionList( NULL, NULL ); }
            	| EXPRESSIONLIST CHAR_I qidentifier { ufdbCategoryExpressionList( $3, "i" ); }
            	| EXPRESSIONLIST CHAR_I WORD     { ufdbCategoryExpressionList( $3, "i" ); }
            	| EXPRESSIONLIST IGNORECASE qidentifier { ufdbCategoryExpressionList( $3, "i" ); }
            	| EXPRESSIONLIST IGNORECASE WORD { ufdbCategoryExpressionList( $3, "i" ); }
		| CACERTS qidentifier            { ufdbCategoryCACertsFile( $2 ); }
		| CACERTS WORD                   { ufdbCategoryCACertsFile( $2 ); }
		| CACERTSDIR qidentifier         { ufdbCategoryCACertsDir( $2 ); }
		| CACERTSDIR WORD                { ufdbCategoryCACertsDir( $2 ); }
            	| REDIRECT qidentifier           { ufdbCategoryRedirect( $2 ); }
            	| REDIRECT WORD                  { ufdbCategoryRedirect( $2 ); }
            	| REWRITE qidentifier            { ufdbCategoryRewrite( $2 ); }
            	| REWRITE WORD                   { ufdbCategoryRewrite( $2 ); }
            	| WITHIN qidentifier             { ufdbCategoryTime( $2, UFDB_ACL_WITHIN ); }
            	| OUTSIDE qidentifier            { ufdbCategoryTime( $2, UFDB_ACL_OUTSIDE ); }
		| OPTION BLOCK_BUMPED_CONNECT on_or_off              { ufdbCategoryBlockConnect( $3 ); }
		| OPTION SAFE_SEARCH on_or_off                       { ufdbCategoryOption( $3, UFDB_OPT_SAFE_SEARCH );  }
		| OPTION YOUTUBE_EDUFILTER on_or_off                 { ufdbCategoryOption( $3, UFDB_OPT_YOUTUBE_EDUFILTER );  }
		| OPTION ENFORCE_HTTPS_WITH_HOSTNAME                 { ufdbCategoryOption(  1, UFDB_OPT_HTTPS_WITH_HOSTNAME );  }
		| OPTION ENFORCE_HTTPS_WITH_HOSTNAME on_or_off       { ufdbCategoryOption( $3, UFDB_OPT_HTTPS_WITH_HOSTNAME );  }
		| OPTION ENFORCE_HTTPS_OFFICAL_CERTIFICATE           { ufdbCategoryOption(  1, UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE );  }
		| OPTION ENFORCE_HTTPS_OFFICAL_CERTIFICATE on_or_off { ufdbCategoryOption( $3, UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE );  }
		| OPTION HTTPS_PROHIBIT_INSECURE_SSLV2 on_or_off     { ufdbCategoryOption( $3, UFDB_OPT_PROHIBIT_INSECURE_SSLV2 );  }
		| OPTION HTTPS_PROHIBIT_INSECURE_SSLV3 on_or_off     { ufdbCategoryOption( $3, UFDB_OPT_PROHIBIT_INSECURE_SSLV3 );  }
		| OPTION ALLOW_SKYPE_OVER_HTTPS on_or_off	     { ufdbCategoryOption( $3, UFDB_OPT_SKYPE_OVER_HTTPS );  }
		| OPTION ALLOW_GTALK_OVER_HTTPS on_or_off	     { ufdbCategoryOption( $3, UFDB_OPT_GTALK_OVER_HTTPS );  }
		| OPTION ALLOW_YAHOOMSG_OVER_HTTPS on_or_off	     { ufdbCategoryOption( $3, UFDB_OPT_YAHOOMSG_OVER_HTTPS );  }
		| OPTION ALLOW_AIM_OVER_HTTPS on_or_off	             { ufdbCategoryOption( $3, UFDB_OPT_AIM_OVER_HTTPS );  }
		| OPTION ALLOW_FBCHAT_OVER_HTTPS on_or_off	     { ufdbCategoryOption( $3, UFDB_OPT_FBCHAT_OVER_HTTPS );  }
		| OPTION ALLOW_CITRIXONLINE_OVER_HTTPS on_or_off     { ufdbCategoryOption( $3, UFDB_OPT_CITRIXONLINE_OVER_HTTPS );  }
		| OPTION ALLOW_ANYDESK_OVER_HTTPS on_or_off          { ufdbCategoryOption( $3, UFDB_OPT_ANYDESK_OVER_HTTPS );  }
		| OPTION ALLOW_TEAMVIEWER_OVER_HTTPS on_or_off       { ufdbCategoryOption( $3, UFDB_OPT_TEAMVIEWER_OVER_HTTPS );  }
		| OPTION ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS on_or_off { ufdbCategoryOption( $3, UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS );  }
            	| LOGFILE ANONYMOUS qidentifier { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
		                                  ufdbFree( $3 ); }
            	| LOGFILE ANONYMOUS WORD        { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
		                                  ufdbFree( $3 ); }
            	| LOGFILE qidentifier           { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
		                                  ufdbFree( $2 ); }
            	| LOGFILE WORD                  { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
		                                  ufdbFree( $2 ); }
            	;

source:      
		  SOURCE qidentifier { defSource( $2 ); }
             	;

source_block: 
		  source START_BRACKET source_contents STOP_BRACKET { defSourceEnd(); }
             	;

source_contents:
		  /* empty */
		| source_contents source_content
		;

source_content:       
		  DOMAIN domain
                | USER user 	
                | UNIX USER user 		     
                | USERLIST qidentifier 		 { ufdbSourceUserList( $2 ); } 
                | USERLIST WORD 		 { ufdbSourceUserList( $2 ); } 
                | UNIX USERLIST qidentifier      { ufdbSourceUserList( $3 ); } 
                | UNIX USERLIST WORD 	         { ufdbSourceUserList( $3 ); } 
                | EXECUSERLIST qidentifier 	 { ufdbSourceExecUserList( $2 ); } 
		| GROUP qidentifier              { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, $2 ); }
		| GROUP WORD                     { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, $2 ); }
		| UNIX GROUP qidentifier         { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, $3 ); }
		| UNIX GROUP WORD                { ufdbSourceGroup( UFDB_GROUPTYPE_UNIX, $3 ); }
                | USERQUOTA NUMBER NUMBER HOURLY { ufdbSourceUserQuota( $2, $3, "3600" );  
		                                   ufdbFree( $2 ); ufdbFree( $3 ); }
                | USERQUOTA NUMBER NUMBER DAILY  { ufdbSourceUserQuota( $2, $3, "86400" );  
		                                   ufdbFree( $2 ); ufdbFree( $3 ); }  
                | USERQUOTA NUMBER NUMBER WEEKLY { ufdbSourceUserQuota( $2, $3, "604800" );  
		                                   ufdbFree( $2 ); ufdbFree( $3 ); } 
                | USERQUOTA NUMBER NUMBER NUMBER { ufdbSourceUserQuota( $2, $3, $4 );  
		                                   ufdbFree( $2 ); ufdbFree( $3 ); ufdbFree( $4 ); } 
                | IPV4 ipv4s
                | IPV4LIST qidentifier         	 { defSourceIPV4List( $2 ); }
                | IPV4LIST WORD             	 { defSourceIPV4List( $2 ); }
                | IPV6 ipv6s
                | IPV6LIST qidentifier         	 { defSourceIPV6List( $2 ); }
                | IPV6LIST WORD         	 { defSourceIPV6List( $2 ); }
		| EVALUATE_AND                   { ufdbSourceEval( UFDB_EVAL_AND ); }
		| EVALUATE_OR                    { ufdbSourceEval( UFDB_EVAL_OR ); }
                | WITHIN qidentifier           	 { defSourceTime( $2, UFDB_ACL_WITHIN ); }
                | OUTSIDE qidentifier          	 { defSourceTime( $2, UFDB_ACL_OUTSIDE ); }
                | LOGFILE ANONYMOUS qidentifier	 { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
						   ufdbFree( $3 ); }
                | LOGFILE ANONYMOUS WORD  	 { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
						   ufdbFree( $3 ); }
                | LOGFILE qidentifier          	 { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
						   ufdbFree( $2 ); }
                | LOGFILE WORD            	 { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
						   ufdbFree( $2 ); }
                | CONTINUE                	 { lastSource->cont_search = 1; }
                ;


domain:		    
		  /* empty */
		| domain qidentifier  	{ defSourceDomain( $2 ); }
		| domain WORD  		{ defSourceDomain( $2 ); }
                | domain COMMA
		;

user:		    
		  /* empty */
		| user qidentifier	{ ufdbSourceUser( $2 ); }
		| user WORD    		{ ufdbSourceUser( $2 ); }
                | user COMMA
		;

acl_block: 
		  ACL START_BRACKET acl_contents STOP_BRACKET 
                ;

acl_contents:     /* empty */
                | acl_contents acl_content
	        ;

acl_header:       
		  qidentifier                     START_BRACKET  { sgAcl( $1, NULL, UFDB_ACL_NONE );  }
		| qidentifier WITHIN qidentifier  START_BRACKET  { sgAcl( $1, $3, UFDB_ACL_WITHIN );  }
                | qidentifier OUTSIDE qidentifier START_BRACKET  { sgAcl( $1, $3, UFDB_ACL_OUTSIDE ); } 
                ;

acl_content:      
		  acl_header access_contents STOP_BRACKET
                | acl_header access_contents STOP_BRACKET ELSE
                     { sgAcl( NULL, NULL, UFDB_ACL_ELSE );    } 
                     START_BRACKET access_contents STOP_BRACKET
                ;

access_contents:  
		  /* empty */
                | access_contents access_content
                ;

access_content:     
		  PASS access_pass              { if (lastAcl != NULL  &&  lastAcl->pass == NULL) 
                                                  {
                                                     ufdbLogMessage( "line %d: acl has an empty pass statement.  Adding 'any'.", lineno );
                                                     sgAclSetValue( "pass", ufdbStrdup("any"), 1 ); 
                                                  }
                                                }
                | REWRITE qidentifier           { sgAclSetValue( "rewrite", $2, 0 ); }
                | REWRITE WORD                  { sgAclSetValue( "rewrite", $2, 0 ); }
                | REDIRECT qidentifier          { sgAclSetValue( "redirect", $2, 0 ); }
                | REDIRECT WORD                 { sgAclSetValue( "redirect", $2, 0 ); }
                | LOGFILE ANONYMOUS qidentifier { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
						  ufdbFree( $3 ); }
                | LOGFILE ANONYMOUS WORD        { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 
						  ufdbFree( $3 ); }
                | LOGFILE qidentifier           { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
						  ufdbFree( $2 ); }
                | LOGFILE WORD                  { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 
						  ufdbFree( $2 ); }
                ;

access_pass:      
		  /* empty */
		| access_pass AGGRESSIVE		    { yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" );  }
                | access_pass qidentifier     		    { sgAclSetValue( "pass", $2, 1 ); }
                | access_pass CHAR_EXCLAMATION AGGRESSIVE   { yyerror( (char *) "\"aggressive\" is a keyword and must be surrounded by quotes" );  }
                | access_pass CHAR_EXCLAMATION qidentifier  { sgAclSetValue( "pass", $3, 0 ); }
		| access_pass COMMA
                ;

cidr:             
		  CIDR 		                { sgIpv4( $1 ); ufdbFree( $1 ); }
                ;

ipv4class:          
		  IPV4CLASS 	                { sgIpv4( $1 ); ufdbFree( $1 ); }
                ;

ipv4s: 		  
		  /* empty */
                | ipv4s ipv4         	        { sgIpv4( "255.255.255.255" );  sgSetIpType( SG_IPTYPE_HOST, NULL, 0 ); }
                | ipv4s ipv4 cidr    	        { sgSetIpType( SG_IPTYPE_CIDR, NULL, 0 ); }
                | ipv4s ipv4 ipv4class 	        { sgSetIpType( SG_IPTYPE_CLASS, NULL, 0 ); }
                | ipv4s ipv4 CHAR_MINUS ipv4    { sgSetIpType( SG_IPTYPE_RANGE, NULL, 0 ); }
                | ipv4s COMMA
		;

ipv4:  		  
		  IPV4ADDR 		        { sgIpv4( $1 );  ufdbFree( $1 );  }
     		;

ipv6s: 		  
		  /* empty */
                | ipv6s ipv6  
                | ipv6s COMMA
		;

ipv6:  		  
		  IPV6ADDR 		        { sgIpv6( $1, SG_IPV6TYPE_HOST, UFDBglobalConfigFile, lineno );
                                                  ufdbFree( $1 );  }
                | IPV6NET                       { sgIpv6( $1, SG_IPV6TYPE_CIDR, UFDBglobalConfigFile, lineno );
                                                  ufdbFree( $1 );  }
     		;

rew:       	  
		  REWRITE qidentifier  	{ sgRewrite( $2 ); }
		| REWRITE WORD 		{ sgRewrite( $2 ); } 
		;

rew_block:  	  
		  rew START_BRACKET rew_contents STOP_BRACKET 
             	;

rew_contents:     
		  /* empty */
		| rew_contents rew_content
		;


rew_content:      
		  SUBST                         { sgRewriteSubstitute( $1 ); ufdbFree( $1 ); }
                | WITHIN qidentifier            { sgRewriteTime( $2, UFDB_ACL_WITHIN ); }
                | OUTSIDE qidentifier           { sgRewriteTime( $2, UFDB_ACL_OUTSIDE ); }
                | LOGFILE ANONYMOUS qidentifier { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 							      ufdbFree( $3 ); }
                | LOGFILE ANONYMOUS WORD        { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $3 ); 							      ufdbFree( $3 ); }
                | LOGFILE qidentifier           { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 							      ufdbFree( $2 ); }
                | LOGFILE WORD                  { ufdbLogError( "line %d: unsupported logfile context for %s", lineno, $2 ); 							      ufdbFree( $2 ); }
                ;


time:       	  
		  TIME qidentifier 		{ sgTime( $2 ); } 
		;

time_block:  	  
		  time START_BRACKET time_contents STOP_BRACKET 
                ;

time_contents:    
		  /* empty */
		| time_contents time_content
		;


time_content:     
                  WEEKLY { sgTimeElementInit(); } WEEKDAY     { sgTimeElementAdd($3,T_WEEKDAY); } ttime
		| WEEKLY { sgTimeElementInit(); } qidentifier { sgTimeElementAdd($3,T_WEEKLY); }  ttime
		| WEEKLY { sgTimeElementInit(); } WORD        { sgTimeElementAdd($3,T_WEEKLY); }  ttime
                | DATE   { sgTimeElementInit(); } date        { sgTimeElementEnd(); }
		| error  { ufdbLogFatalError( "invalid time specification at line %d", lineno );   }
                ;

ttime:            
		  ttime  { sgTimeElementClone(); }  tval CHAR_MINUS tval
		| tval CHAR_MINUS tval 
                ;

date:             
		  dval ttime
                | dval 
                | dval  CHAR_MINUS dval ttime
                | dval  CHAR_MINUS dval 
                | dvalcron  ttime
                | dvalcron
                ;

dval:		  DVAL 		{ sgTimeElementAdd( $1, T_DVAL ); }
                ;

tval:		  TVAL 		{ sgTimeElementAdd( $1, T_TVAL ); }
                ;

dvalcron:	  DVALCRON 	{ sgTimeElementAdd( $1, T_DVALCRON ); }
                ;

an_error:
		  error  	{  yyerror( (char *) "syntax error" );  }
	        ;

statements:       /* empty */
       		| statements statement
       		;

statement:   
                  category
	     	| source_block
	     	| category_block
		| http_server_def
	     	| admin_spec
	     	| https_cache_size
	     	| check_proxy_tunnels
	     	| log_pass
	     	| log_block
	     	| log_all
		| logall_httpd
		| debug_filter
		| enforce_https_with_hostname
		| enforce_https_offical_certificate
		| https_prohibit_insecure_sslv2
		| https_prohibit_insecure_sslv3
             	| dbhome
	     	| logdir
		| pidfile
		| mail_server
		| my_hostname
		| admin_email
		| sender_email
		| external_status_command
		| url_lookup_delay_db_reload
		| url_lookup_result_db_reload
		| url_lookup_result_fatal_error
		| squid_version
		| num_worker_threads
		| upload_crash_reports
		| lookup_reverse_ip
		| use_ipv6_on_wan
		| squid_uses_ssl_bump
		| ufdb_log_url_details
		| ufdb_show_url_details
		| ufdb_debug_skype_probes
		| ufdb_debug_gtalk_probes
		| ufdb_debug_yahoomsg_probes
		| ufdb_debug_aim_probes
		| ufdb_debug_fbchat_probes
		| ufdb_debug_citrixonline_probes
		| ufdb_expression_optimisation
		| ufdb_expression_debug
		| refreshuserlist
		| refreshdomainlist
		| ufdb_debug_external_scripts
		| interface
	     	| port
	     	| cpus
	     	| upload_stats
		| redirect_https
		| redirect_bumped_https
		| redirect_loading_database
		| redirect_fatal_error
	     	| analyse_uncategorised
		| log_uncategorised_urls
		| safe_search
		| strip_domain_from_username
		| youtube_edufilter
		| allow_google_https_using_ip
		| youtube_edufilter_id
		| deny_mode
		| max_logfile_size
	     	| acl_block
	     	| rew_block
	     	| time_block
	     	| NL
	     	| an_error
             	;

%%

int sgReadConfig( 
   char *            file )
{
   char *            defaultFile = (char *) DEFAULT_CONFIGFILE;
   struct Source *   source;
   struct Category * cat;
   struct Acl *      acl;

   lineno = 1;
   ufdbResetCPUs();

   UFDBglobalConfigFile = file;
   if (UFDBglobalConfigFile == NULL)
      UFDBglobalConfigFile = defaultFile;

   yyin = fopen( UFDBglobalConfigFile, "r" );
   if (yyin == NULL) 
   {
      syslog( LOG_ALERT, "%s: cannot open configuration file %s", UFDBprogname, UFDBglobalConfigFile );
      ufdbLogFatalError( "cannot open configuration file %s", UFDBglobalConfigFile );
      return 0;
   }

   ufdbLogMessage( "configuration file: %s", UFDBglobalConfigFile );

   /* keep the old HTTPS redirection URL.  It is most likely that we need it during the database reload */
   /* dot not reset UFDBglobalRedirectHttps and UFDBglobalRedirectBumpedHttps. */

   /* UFDBglobalDebug = 0; */
   strcpy( UFDBglobalInterface, "all" );
   /* UFDBglobalPortNum = UFDB_DAEMON_PORT;  the port can be set with a command line switch so do not reset it */
   /* UFDBglobalNworkers = 68;  do not reset this variable. */
   UFDBglobalDebugHttpd = 0;
   UFDBglobalLogPass = 0;
   UFDBglobalLogBlock = 0;
   UFDBglobalLogAllRequests = 0;
   UFDBglobalYoutubeEdufilter = 0;
   UFDBglobalYoutubeEdufilterID = (char *) "none";
   UFDBglobalAllowGoogleHTTPSusingIP = 0;
   UFDBglobalDenyMode = 1;

#if 0
   /* When a database is reloaded, these variables may NOT be reset since they are used during the reload !! */
   UFDBglobalURLlookupDelayDBreload = 0;
   UFDBglobalURLlookupResultDBreload = UFDB_ALLOW;
   UFDBglobalURLlookupResultFatalError = UFDB_ALLOW;
   strcpy( UFDBglobalLoadingDatabaseRedirect, "a-blocking-url" );
   ufdbFree( UFDBglobalSquidVersion );
#endif
   if (UFDBglobalSquidVersion == NULL)
      UFDBglobalSquidVersion = ufdbStrdup( UFDB_DEFAULT_SQUID_VERSION );

   UFDBglobalShowURLdetails = 0;
   UFDBglobalLogURLdetails = 0;
   UFDBglobalUploadCrashReports = 1;
   UFDBglobalLookupReverseIP = 0;
   UFDBglobalUseAlsoIPv6onWan = 1;
   UFDBglobalDebugSkype = 0;
   UFDBglobalDebugGtalk = 0;
   UFDBglobalDebugYahooMsg = 0;
   UFDBglobalDebugAim = 0;
   UFDBglobalDebugFBchat = 0;
   UFDBglobalDebugCitrixOnline = 0;
   UFDBglobalDebugRegexp = 0;
   UFDBglobalDebugExternalScripts = 0;
   UFDBglobalExpressionOptimisation = 1;
   UFDBglobalRefreshUserlistInterval = UFDB_DEFAULT_REFRESH_USERLIST;
   UFDBglobalRefreshDomainlistInterval = UFDB_DEFAULT_REFRESH_DOMAINLIST;
   UFDBglobalAnalyseUncategorisedURLs = 1;
   UFDBglobalLogUncategorisedURLs = 0;
   UFDBglobalUploadStats = 1;
   UFDBglobalSafeSearch = 1;
   UFDBglobalSkipSafeCategory = 0;
   UFDBglobalStripDomainFromUsername = 0;
   UFDBglobalTunnelCheckMethod = UFDB_API_HTTPS_CHECK_OFF;
   /* do not reset: UFDBglobalSquidUsesSSLbump = 0; */
   UFDBglobalHttpsWithHostname = 0;
   UFDBglobalHttpsOfficialCertificate = 0;
   UFDBglobalUnknownProtocolOverHttps = 1;
   UFDBglobalHttpsNoSSLv2 = 1;
   UFDBglobalHttpsNoSSLv3 = 0;
   UFDBglobalHttpdPort = 0;
   UFDBglobalDateOfCheckedDB[0] = '\0';
   strcpy( UFDBglobalHttpdInterface, "all" );
   strcpy( UFDBglobalHttpdImagesDirectory, "." );
 
   UFDBglobalDatabaseStatus = UFDB_API_STATUS_DATABASE_OK;

   (void) yyparse();		/* parse the configuration file */
   fclose( yyin );

   UFDBglobalDatabaseLoadTime = time( NULL );

   /*
    * For analysis of uncategorised URLs we also load a "checked" category
    * that contains URLs that are reviewed and checked not to be part of
    * any other category.
    */
   {
      char * dbhome;
      char   dbfname[1024];

      dbhome = ufdbSettingGetValue( (char *) "dbhome" );
      if (dbhome == NULL)
         dbhome = (char *) DEFAULT_DBHOME;

      /* TODO: check if "checked" was not already loaded... */
      if (UFDBglobalAnalyseUncategorisedURLs > 0)
      {
	 sprintf( dbfname, "%s/checked/domains.ufdb", dbhome );
	 if (UFDBloadDatabase( &UFDBglobalCheckedDB, dbfname ) != UFDB_API_OK)
	 {
	    UFDBglobalCheckedDB.mem = NULL;
	    UFDBglobalCheckedDB.index = NULL;
	    UFDBglobalCheckedDB.table.nNextLevels = 0;
	    if (UFDBglobalDebug)
	       ufdbLogMessage( "The URL database appears not be from URLfilterDB. No problem." );
	 }
	 else
	 {
	    sprintf( dbfname, "%s/checked/expressions", dbhome );
	    (void) UFDBloadExpressions( &UFDBglobalCheckedExpressions, dbfname );
	    ufdbLogMessage( "The implicitly allowed URL category 'checked' is loaded." );
	 }
      }
   }

   if (defaultAcl == NULL)
      ufdbLogFatalError( "\"default\" ACL is not defined" );
   else if (defaultAcl->pass == NULL)
      ufdbLogError( "\"default\" ACL is empty: by default all URLs are blocked." );

   /* A configuration file may have a source definition which is not used in the ACL list.
    * This raises the question "what to do with this ?".
    * If we do nothing, the source is matched but has no ACL definition and is silently skipped.
    * Most likely the administrator overlooked something so a warning is useful.
    */
   source = UFDBgetSourceList();  
   if (source == NULL  &&  defaultAcl == NULL)
      ufdbLogFatalError( "There are no sources and there is no default ACL" );

   for ( ; source != NULL;  source = source->next)
   {
      int found;

      found = 0;
      for (acl = AclList;  acl != NULL;  acl = acl->next)
      {
         if (strcmp( acl->name, source->name ) == 0)
	 {
	    found = 1;
	    break;
	 }
      }
      if (!found)
      {
         ufdbLogError( "source \"%s\" has no ACL.  *****\n"
	               "This most likely may lead to unexpected results when the source is matched.  *****\n"
		       "It is strongly suggested to remove the source definition OR add the source to the acl.  *****",
		       source->name );
	 source->active = 0;
      }
      else if (acl->pass == NULL)
      {
         ufdbLogMessage( "source \"%s\" has an empty \"pass\" in its ACL *****", source->name );
      }
   }

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      if (cat->options == 0  &&  cat->domainlist == NULL  &&  cat->execdomainlist == NULL  && 
          cat->expressionlist == NULL  &&  cat->rewrite == NULL  &&  cat->name != NULL)
      {
         ufdbLogError( "category \"%s\" has no content definition  *****", cat->name );
      }
   }

   if (UFDBglobalAdminEmail != NULL  &&  UFDBglobalEmailServer == NULL)
   {
      ufdbLogError( "No email server is defined; cannot send email to \"%s\"  *****", UFDBglobalAdminEmail );
   }
   if (UFDBglobalAdminEmail != NULL  &&  UFDBglobalEmailServer != NULL  &&  UFDBglobalSenderEmail == NULL)
   {
      ufdbLogMessage( "WARNING: No sender email address is defined; using \"%s\" as sender", UFDBglobalAdminEmail );
   }

   if (strncmp( UFDBglobalSquidVersion, "5.", 2 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "4.", 2 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.5", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.4", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.3", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.2", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.1", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "3.0", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "2.7", 3 ) != 0  &&
       strncmp( UFDBglobalSquidVersion, "2.6", 3 ) != 0)
   {
      ufdbLogFatalError( "Unsupported Squid version \"%s\"  *****\n"
                         "The supported values for squid-version are: "
                         "5.x, 4.x, 3.5, 3.4, 3.3, 3.2, 3.1, 3.0, 2.7, 2.6  *****\n"
			 "If the version of Squid is higher, it is recommended to upgrade ufdbGuard.  *****\n"
			 "Alternatively set squid-version to the highest supported version",
			 UFDBglobalSquidVersion );
   }
   if (strncmp( UFDBglobalSquidVersion, "5.", 2 ) == 0  ||
       strncmp( UFDBglobalSquidVersion, "4.", 2 ) == 0  ||  
       strncmp( UFDBglobalSquidVersion, "3.5", 3 ) == 0  ||  
       strncmp( UFDBglobalSquidVersion, "3.4", 3 ) == 0)
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL3;
   else if (strncmp( UFDBglobalSquidVersion, "2.6", 3 ) == 0  ||  
            strncmp( UFDBglobalSquidVersion, "2.7", 3 ) == 0)
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL1;
   else
      UFDBglobalSquidHelperProtocol = UFDB_SQUID_HELPER_PROTOCOL2;	/* 3.0 - 3.3 */

   BuildImplicitPassLists();

   return 1;
}


static void setRefreshUserlist( int nmin )
{
   if (nmin < 5  ||  nmin > 24*60)
   {
      ufdbLogError( "refreshuserlist must have a value between 5 and %d, resetting to %d.", 
                    24*60, UFDB_DEFAULT_REFRESH_USERLIST );
      nmin = UFDB_DEFAULT_REFRESH_USERLIST;
   }
   UFDBglobalRefreshUserlistInterval = nmin;
}


static void setRefreshDomainlist( int nmin )
{
   if (nmin < 5  ||  nmin > 24*60)
   {
      ufdbLogError( "refreshdomainlist must have a value between 5 and %d, resetting to %d.", 24*60, UFDB_DEFAULT_REFRESH_DOMAINLIST );
      nmin = UFDB_DEFAULT_REFRESH_DOMAINLIST;
   }
   UFDBglobalRefreshDomainlistInterval = nmin;
}


/*
 * Source functions
 */

void defSource( 
   char *          source )
{
   struct Source * sp;

   if (UFDBglobalDebug)
      ufdbLogMessage( "defSource: defining source \"%s\"", source );

   if ((struct Source *) defSourceFindName(source) != NULL)
   {
      ufdbLogFatalError( "line %d: source %s is already defined in configuration file %s",
			 lineno, source, UFDBglobalConfigFile );
      ufdbFree( source );
      return;
   }

   sp = (struct Source *) ufdbMallocAligned( 64, sizeof(struct Source) );
   sp->name = source;
   sp->active = 1;
   sp->evaluationMethod = UFDBglobalDenyMode ? UFDB_EVAL_AND : UFDB_EVAL_OR;
   sp->ipv4 = NULL;
   sp->ipv6 = NULL;
   sp->lastipv4 = NULL;
   sp->lastipv6 = NULL;
   sp->domainDb = NULL;
   sp->userDb = NULL;
   sp->time = NULL;
   sp->within = UFDB_ACL_NONE;
   sp->cont_search = UFDBglobalDenyMode ? 1 : 0;
   sp->sarg0 = NULL;
   sp->userquota.seconds = 0;
   sp->userquota.renew = 0;
   sp->userquota.sporadic = 0;
   sp->next = NULL;
   sp->nblocks = 0;
   sp->nmatches = 0;

   if (UFDBgetSourceList() == NULL)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "defSource: SourceList is set to point to \"%s\"", source );
      UFDBsetSourceList( sp );
      lastSource = sp;
   }
   else
   {
      lastSource->next = sp;
      lastSource = sp;
   }
}


void defSourceEnd( void )
{
   struct Source * s;

   s = lastSource;
   if (s->ipv4 == NULL  &&  s->ipv6 == NULL  &&  s->domainDb == NULL  &&  s->userDb == NULL)
   {
      ufdbLogError( "source \"%s\" missing active content, set inactive  *****", s->name );
      s->time = NULL;
      s->active = 0;
   }
}


void ufdbSourceUser( 
   char *               user )
{
   char *               lc;
   struct Source *      sp;

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "user \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         user, sp->name, lineno );
      return;
   }
   if (sp->userDb == NULL)
   {
      sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      sp->userDb->dbhome = NULL;
      sp->userDb->dbcp = (void *) UFDBmemDBinit();
      sp->userDb->type = SGDBTYPE_USERLIST;
      sp->userDb->entries = 0;
   }

   for (lc = user; *lc != '\0'; lc++)    /* convert username to lowercase chars */
   {
      if (*lc <= 'Z'  &&  *lc >= 'A')
         *lc += 'a' - 'A';
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbSourceUser: adding user \"%s\"", user );

   sp->userDb->entries++;
   UFDBmemDBinsert( (struct UFDBmemDB *) sp->userDb->dbcp, user, NULL );
   ufdbFree( user );
}


static void ufdbSourceUnixGroup(
   char *          groupName  )
{
   struct Source * sp;
   struct group *  grp;
   int             n;
   char *          user;

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "unix group \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         groupName, sp->name, lineno );
      return;
   }
   if (sp->userDb == NULL) 
   {
      sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      sp->userDb->dbhome = NULL;
      sp->userDb->dbcp = (void *) UFDBmemDBinit();
      sp->userDb->type = SGDBTYPE_USERLIST;
      sp->userDb->entries = 0;
   }
 
   n = 0;
   setgrent();
   errno = 0;
   grp = getgrnam( groupName );
   if (grp != NULL)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "found group \"%s\"", grp->gr_name );

      while ((user = grp->gr_mem[n]) != NULL)
      {
	 char * lc;

	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "group \"%s\" has user \"%s\"", grp->gr_name, user );
	 for (lc = user; *lc != '\0'; lc++)  	/* convert username to lowercase chars */
	 {
	    if (*lc <= 'Z'  &&  *lc >= 'A')
	       *lc += 'a' - 'A';
	 }
	 UFDBmemDBinsert( (struct UFDBmemDB *) sp->userDb->dbcp, user, NULL );
	 sp->userDb->entries++;
	 n++;
      }
      ufdbLogMessage( "source \"%s\": unix group \"%s\" has %d members", sp->name, groupName, n );
      if (n == 0)
	 ufdbLogMessage( "WARNING: source \"%s\": \"%s\" is an empty group  +++++", sp->name, groupName );
   }
   else
   {
      ufdbLogFatalError( "source \"%s\": \"%s\" is not a unix group", sp->name, groupName );
      if (errno)
	 ufdbLogError( "ufdbSourceUnixGroup: getgrnam returned error %d: %s", errno, strerror(errno) );
   }
   endgrent();
}


static void ufdbSourceGroup( 
   int    groupType,
   char * groupName  )
{
   switch (groupType)
   {
   case UFDB_GROUPTYPE_UNIX:
      ufdbSourceUnixGroup( groupName );
      break;
   default:     
      ufdbLogFatalError( "ufdbSourceGroup: unknown group type %d", groupType );
   }

   ufdbFree( groupName );
}


struct sgDb * UFDBretrieveExecDomainlist( struct Category * cat )
{
   char *        t;
   FILE *        fp;
   struct sgDb * newdb;
   int           exitcode;
   struct stat   Stat0;
   struct stat   Stat1;
   char          line[1024];
   char          basefilename[1024];
   char          fullfilename[1024];
   char          command[3000];

   if (cat == NULL)
   {
      ufdbLogFatalError( "UFDBretrieveExecDomainlist: category is NULL" );
      return NULL;
   }

   if (cat->domainlist == NULL)
   {
      ufdbLogError( "will not execute command for category %s since domainlist is undefined", cat->name );
      return NULL;
   }

   if (cat->execdomainlist == NULL)
   {
      ufdbLogError( "can't execute command for category %s since execdomainlist is undefined", cat->name );
      return NULL;
   }

   if (cat->domainlist[0] == '/')
   {
      strcpy( basefilename, cat->domainlist );
   }
   else
   {
      char * dbhome;
      dbhome = ufdbSettingGetValue( "dbhome" );
      if (dbhome == NULL)
	 dbhome = (char *) DEFAULT_DBHOME;
      sprintf( basefilename, "%s/%s", dbhome, cat->domainlist );
   }
   strcpy( fullfilename, basefilename );
   strcat( fullfilename, UFDBfileSuffix );

   if (stat( fullfilename, &Stat0 ) != 0)
   {
      Stat0.st_mtime = 0;
      ufdbLogMessage( "category %s: pre-execdomainlist: domainlist does not exist (%s)", cat->name, fullfilename );
   }

   /* The script that we will call needs the directory where the files reside,
    * so we set the environment variable UFDB_TABLEDIR for it to change its working directory.
    * (note that we are multithreaded and cannot use chdir() here)
    */
   strcpy( command, "cd " );
   strcat( command, basefilename );
   t = strrchr( command, '/' );
   if (t != NULL)
      *t = '\0';
   strcat( command, " ; " );
   strcat( command, cat->execdomainlist );

   errno = 0;
   if ((fp = popen(command,"r")) == NULL)
   {
      ufdbLogFatalError( "category %s: can't execute command of execdomainlist \"%s\": popen failed: %s  *****", 
                         cat->name, cat->execdomainlist, strerror(errno) );
      return NULL;
   }

   while (UFDBfgetsNoNL(line,sizeof(line),fp) != NULL)
   {
      ufdbLogMessage( "execdomainlist for %s produced: %s", cat->name, line );
   }
   errno = 0;
   exitcode = pclose( fp );

   if (exitcode == -1  &&  errno == ECHILD)
      exitcode = 0;
      /* most likely the child handler thread already did a wait() */

   if (exitcode == 0)
   {
      if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
         ufdbLogMessage( "   execdomainlist for %s: exit 0 (OK)", cat->name );
   }
   else if (exitcode == -1)
   {
      ufdbLogError( "execdomainlist for %s: pclose error: %d (%s)", cat->name, errno, strerror(errno) );
      return NULL;
   }
   else if (exitcode > 0)
   {
      ufdbLogError( "execdomainlist for %s: command (%s) terminated with an error exit code %d", 
                    cat->name, cat->execdomainlist, exitcode );
      return NULL;
   }

   /* The command defined by 'execdomainlist' has been executed and the exit code was 0 (OK), so
    * now it is verified if the file timestamp of domains.ufdb is newer.
    */

   if (stat( fullfilename, &Stat1 ) != 0)
   {
      ufdbLogError( "execdomainlist for %s: command failed to produce a .ufdb file (%s)", cat->name, fullfilename );
      return NULL;
   }

   newdb = (struct sgDb *) ufdbCalloc( sizeof(struct sgDb), 1 );
   newdb->dbhome = NULL;
   newdb->dbcp = NULL;
   newdb->type = SGDBTYPE_DOMAINLIST;

   sgDbInit( newdb, basefilename );
   if (newdb->dbcp == NULL)
   {
      ufdbLogError( "execdomainlist for %s: command produced a .ufdb file but I failed to load it", cat->name );
      ufdbFree( newdb );
      return NULL;
   }

   if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
      ufdbLogMessage( "execdomainlist for %s: new URL table is loaded", cat->name );

   return newdb;
}


void ufdbSourceExecUserList(
   char *          command )
{
   time_t          t0, te;
   struct Source * sp;

   /* execuserlist is used to dynamically retrieve a list of usernames and
    * is executed every 15 minutes to refresh the list of usernames.
    */

   sp = lastSource;
   if (sp->userDb != NULL  &&  sp->userDb->type != SGDBTYPE_EXECUSERLIST)
   {
      ufdbLogFatalError( "execuserlist \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         command, sp->name, lineno );
      return;
   }
   if (sp->userDb != NULL)
   {
      ufdbLogFatalError( "execuserlist \"%s\" in source \"%s\" on line %d: "
                         "more than one execuserlist is not supported.  Merge the userlists in the script.", 
                         command, sp->name, lineno );
      return;
   }
   ufdbLogMessage( "execuserlist \"%s\"", command );

   t0 = time( NULL );
   sp->userDb = UFDBretrieveExecUserlist( command );
   sp->sarg0 = command;				/* command is already malloc-ed */
   te = time( NULL );

   if (te - t0 > 6)
      ufdbLogMessage( "WARNING: it took %ld seconds to execute \"%s\"", te - t0, command );
 
   if (UFDBglobalDebug || UFDBglobalDebugExternalScripts)
   {
      UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) sp->userDb->dbcp );
   }
}


void ufdbSourceUserList( 
   char * file  )
{
   char * dbhome;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   char * s;
   char * lc;
   char * lineptr;
   int    ullineno;
   struct sgDb * udb;
   struct Source * sp;
   char   line[10000];
 
   sp = lastSource;
   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbSourceUserList: source \"%s\"  file \"%s\"", 
       		      sp->name != NULL ? sp->name : "nosource", file );
   udb = sp->userDb;
   if (udb != NULL  &&  udb->type != SGDBTYPE_USERLIST)
   {
      ufdbLogFatalError( "userlist \"%s\" in source \"%s\" on line %d: "
                         "found mix of exec and non-exec userlists", 
                         file, sp->name, lineno );
      return;
   }
   if (udb == NULL)
   {
      udb = sp->userDb = (struct sgDb *) ufdbMalloc( sizeof(struct sgDb) );
      udb->dbhome = NULL;
      udb->dbcp = (void *) UFDBmemDBinit();
      udb->type = SGDBTYPE_USERLIST;
      udb->entries = 0;
   }
 
   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;
 
   if (file[0] == '/')
      f = file;
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }
 
   if ((fp = fopen(f,"r")) == NULL)
   {
      ufdbLogError( "line %d: can't open userlist %s: %s  *****", lineno, f, strerror(errno) );
      ufdbFree( f );
      return;
   }
   ullineno = 0;
 
   ufdbLogMessage( "userlist \"%s\"", f );
 
   while (fgets(line,sizeof(line),fp) != NULL)
   {
      ullineno++;
      if (line[0] == '#')			/* skip comments */
         continue;
 
      p = strchr( line, '\n' );
      if (p != NULL)
      {
         *p = '\0';
         if (p != line) 
         {
  	    if (*(p - 1) == '\r') 		/* remove ^M  */
  	       *(p-1) = '\0';
         }
      }
  
      if (line[0] == '\0')
      {
         ufdbLogError( "userlist %s: line %d: line is empty", f, ullineno );
         continue;
      }
  
      c = strchr( line, '#' );		/* remove comment */
      if (c != NULL)
         *c = '\0';
  
      p = strtok_r( line, " \t,", &lineptr );
      if (p == NULL  ||  *p == '\0')
         continue;
  
      /* we may have a passwd-style formatted file. Ignore ':' and everything else that follows. */
      if ((s = strchr(p,':')) != NULL) 
         *s = '\0';
  
      do
      {
         for (lc = p;  *lc != '\0';  lc++)   /* convert username to lowercase chars */
         {
  	    if (*lc <= 'Z'  &&  *lc >= 'A')
  	       *lc += 'a' - 'A';
         }
         if (*p != '\0')
 	 {
	    udb->entries++;
  	    UFDBmemDBinsert( (struct UFDBmemDB *) udb->dbcp, p, NULL );
         }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL  &&  *p != '\0');
   }
   fclose( fp );

   if (UFDBglobalDebug)
   {
      ufdbLogMessage( "just read userlist \"%s\"", f );
      UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) udb->dbcp );
   }

   ufdbFree( f );
}


static void ufdbSourceUserQuota( 
   const char * seconds, 
   const char * sporadic,  
   const char * renew )
{
   int    s;
   struct UserQuota * uq;
   struct Source * sp;
 
   sp = lastSource;
   uq = &sp->userquota;
   s = atoi( seconds );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->seconds = s; 
   s = atoi( sporadic );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->sporadic = s; 
   s = atoi( renew );
   if (s <= 0)
      ufdbLogError( "line %d: userquota seconds sporadic hourly|daily|weekly", lineno );
   uq->renew = s;
}


void defSourceDomain( 
   char * domain )
{
   struct Source * sp;

   sp = lastSource;
   if (sp->domainDb == NULL) 
      sp->domainDb = (struct sgDb *) UFDBmemDBinit();

   if (UFDBglobalDebug)
      ufdbLogMessage( "defSourceDomain \"%s\"", domain );

   UFDBmemDBinsert( (struct UFDBmemDB *) sp->domainDb, domain, NULL );

   ufdbFree( domain );
}


void ufdbSourceEval( 
   int method  )
{
   if (lastSource != NULL)
   {
      if (UFDBglobalDebug)
         ufdbLogMessage( "ufdbSourceEval %d: %s", method, 
	                 method == UFDB_EVAL_OR ? "evaluate-or" : "evaluate-and" );
      lastSource->evaluationMethod = method;
   }
}


void defSourceTime( 
   char *         name, 
   int            within )
{
   struct ufdbTime *  t;

   if ((t = sgTimeFindName(name)) == NULL)
   {
      ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
   }

   lastSource->within = within;
   lastSource->time = t;
   ufdbFree( name );
}


struct Source * defSourceFindName( 
   char *          name )
{
   struct Source * p;

   for (p = UFDBgetSourceList();  p != NULL;  p = p->next)
   {
      if (strcmp(name,p->name) == 0)
         return p;
   }

   return NULL;
}


void defSourceIPV4List( 
   char * file )
{
   char * dbhome = NULL;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   char * cidr;
   int    i;
   int    l = 0;
   char * lineptr;
   char   line[UFDB_MAX_URL_LENGTH];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;

   if (file[0] == '/') 
   {
      f = file;
   }
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }

   if ((fp = fopen(f,"r")) == NULL) 
   {
      ufdbLogError( "line %d: can't open ipv4list %s: %s", lineno, f, strerror(errno) );
      ufdbFree( f );
     return;
   }

   ufdbLogMessage( "ipv4list \"%s\"", file );

   while (fgets(line,sizeof(line),fp) != NULL)
   {
      l++;
      if (*line == '#')
         continue;
      p = strchr( line, '\n' );
      if (p != NULL && p != line) 
      {
         if (*(p - 1) == '\r') 		/* removing ^M  */
            p--;
         *p = '\0';
      }
      c = strchr( line, '#' );
      p = strtok_r( line, " \t,", &lineptr );
      do {
         if (c != NULL && p >= c) 		/* find the comment */
            break;
         i = strspn( p, ".0123456789/-" );
         if (i == 0)
            break;
         *(p + i) = '\0';
         if ((cidr = strchr(p,'/')) != NULL) 
         {
            *cidr = '\0';
            cidr++;
            sgIpv4( p );
            sgIpv4( cidr );
            if (strchr(cidr,'.') == NULL)
               sgSetIpType( SG_IPTYPE_CIDR, f, l );
            else 
               sgSetIpType( SG_IPTYPE_CLASS, f, l );
          }
          else if ((cidr = strchr(p,'-')) != NULL) 
          {
             *cidr = '\0';
             cidr++;
             sgIpv4( p );
             sgIpv4( cidr );
             sgSetIpType( SG_IPTYPE_RANGE, f, l );
          }
          else
          {
             sgIpv4( p );
             sgIpv4( ufdbStrdup("255.255.255.255") );
             sgSetIpType( SG_IPTYPE_HOST, f, l );
          }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL);
   }

   fclose( fp );
   ufdbFree( f );
}


void defSourceIPV6List( 
   char * file )
{
   /* TODO IPV6 */
   char * dbhome;
   char * f;
   FILE * fp;
   char * p;
   char * c;
   int    i;
   int    l = 0;
   char * lineptr;
   char   line[UFDB_MAX_URL_LENGTH];

   dbhome = ufdbSettingGetValue( "dbhome" );
   if (dbhome == NULL)
      dbhome = (char *) DEFAULT_DBHOME;

   if (file[0] == '/') 
   {
      f = file;
   }
   else
   {
      f = (char *) ufdbMalloc( strlen(dbhome) + strlen(file) + 2 );
      strcpy( f, dbhome );
      strcat( f, "/" );
      strcat( f, file );
      ufdbFree( file );
   }

   if ((fp = fopen(f,"r")) == NULL) 
   {
      ufdbLogError( "line %d: can't open ipv6list %s: %s", lineno, f, strerror(errno) );
      ufdbFree( f );
      return;
   }

   ufdbLogMessage( "ipv6list \"%s\"", f );

   while (fgets(line,sizeof(line),fp) != NULL)
   {
      l++;
      if (*line == '#')
         continue;
      p = strchr( line, '\n' );
      if (p != NULL && p != line) 
      {
         if (*(p - 1) == '\r') 		/* removing ^M  */
            p--;
         *p = '\0';
      }
      c = strchr( line, '#' );
      p = strtok_r( line, " \t,", &lineptr );
      do {
         if (c != NULL && p >= c) 		/* find the comment */
            break;
         i = strspn( p, ":.0123456789ABCDEFabcdef/" );
         if (i == 0)
            break;
         *(p + i) = '\0';
         if (strchr(p,'/') != NULL) 
         {
            sgIpv6( p, SG_IPV6TYPE_CIDR, f, l );
         }
         else
         {
            sgIpv6( p, SG_IPV6TYPE_HOST, f, l );
         }
      } while ((p = strtok_r(NULL," \t,",&lineptr)) != NULL);
   }

   fclose( fp );
   ufdbFree( f );
}


/* category block functions */

void ufdbCategory( 
  char *            cat )
{
  struct Category * sp;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategory %s", cat );
#endif

  if (Cat != NULL) 
  {
    if ((struct Category *) ufdbCategoryFindName(cat) != NULL)
    {
      ufdbLogFatalError( "line %d: category %s is already defined in configuration file %s",
		         lineno, cat, UFDBglobalConfigFile );
      ufdbFree( cat );
      return;
    }
  }

  sp = (struct Category *) ufdbMallocAligned( 64, sizeof(struct Category) );
  sp->active = 1;
  sp->options = 0;
  sp->within = UFDB_ACL_NONE;
  sp->blockBumpedConnect = 0;
  sp->name = cat;
  sp->domainlist = NULL;
  sp->domainlistDb = NULL;
  sp->execdomainlist = NULL;
  sp->expressionlist = NULL;
  sp->regExp = NULL;
  sp->redirect = NULL;
  sp->time = NULL;
  sp->rewrite = NULL;
  sp->cse = NULL;
  sp->cse2 = NULL;
  sp->next = NULL;
  sp->nblocks = 0;
  sp->nmatches = 0;

  if (Cat == NULL) {
    Cat = sp;
    lastCat = sp;
  } else {
    lastCat->next = sp;
    lastCat = sp;
  }

   /* category "security" is a special case: the options cannot be set default to 0
    * because the default value for allow-unknown-protocol-over-https is ON.
    */
   if (strcmp( cat, "security" ) == 0)
      sp->options = UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS;
}


void ufdbCategoryEnd( void )
{
  struct Category * d;
 
  d = lastCat;
  if (d->domainlist == NULL  &&  d->expressionlist == NULL  && 
      d->redirect == NULL  &&  d->rewrite == NULL  &&
      d->options == 0)
  {
    ufdbLogError( "category \"%s\" is missing content, set inactive  *****", d->name );
    d->time = NULL;
    d->active = 0;
  }
}


void ufdbCategoryOption( int value, int option )
{
   struct Category * sp;

   sp = lastCat;
   if (value)
      sp->options = sp->options | option;
   else
      sp->options = sp->options & (~option);

   if (option == UFDB_OPT_HTTPS_WITH_HOSTNAME)
      UFDBglobalHttpsWithHostname = value;
   else if (option == UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE)
      UFDBglobalHttpsOfficialCertificate = value;
   else if (option == UFDB_OPT_SKYPE_OVER_HTTPS)
      UFDBglobalSkypeOverHttps = value;
   else if (option == UFDB_OPT_GTALK_OVER_HTTPS)
      UFDBglobalGtalkOverHttps = value;
   else if (option == UFDB_OPT_YAHOOMSG_OVER_HTTPS)
      UFDBglobalYahooMsgOverHttps = value;
   else if (option == UFDB_OPT_AIM_OVER_HTTPS)
      UFDBglobalAimOverHttps = value;
   else if (option == UFDB_OPT_FBCHAT_OVER_HTTPS)
      UFDBglobalFBchatOverHttps = value;
   else if (option == UFDB_OPT_CITRIXONLINE_OVER_HTTPS)
      UFDBglobalCitrixOnlineOverHttps = value;
   else if (option == UFDB_OPT_ANYDESK_OVER_HTTPS)
      UFDBglobalAnydeskOverHttps = value;
   else if (option == UFDB_OPT_TEAMVIEWER_OVER_HTTPS)
      UFDBglobalTeamviewerOverHttps = value;
   else if (option == UFDB_OPT_PROHIBIT_INSECURE_SSLV2)
      UFDBglobalHttpsNoSSLv2 = value;
   else if (option == UFDB_OPT_PROHIBIT_INSECURE_SSLV3)
      UFDBglobalHttpsNoSSLv3 = value;
   else if (option == UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS)
      UFDBglobalUnknownProtocolOverHttps = value;
   else if (option == UFDB_OPT_SAFE_SEARCH)
      ;
   else if (option == UFDB_OPT_YOUTUBE_EDUFILTER)
      ;
   else
      ufdbLogError( "ufdbCategoryOption: unrecognised option %d *****", option );

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbCategoryOption: %s: option %d set to %d", sp->name, option, value );
}


void ufdbCategoryDomainList( 
  char * domainlist )
{
  struct Category * sp;
  char * dbhome;
  char * dl = domainlist;
  char * name;

  if (UFDBglobalDebug > 1)
     ufdbLogMessage( "ufdbCategoryDomainList %s", domainlist );

  if (UFDBglobalTerminating)
     return;

  sp = lastCat;

  if (sp->domainlistDb != NULL)
  {
     ufdbLogFatalError( "line %d: specify only one domainlist per category in configuration file %s",
                        lineno, UFDBglobalConfigFile );
     return;
  }

  dbhome = ufdbSettingGetValue("dbhome");
  if (dbhome == NULL)
    dbhome = (char *) DEFAULT_DBHOME;

  if (domainlist == NULL)
  {
    name = sp->name;
    dl = (char *) ufdbMalloc( sizeof("/dest/") + strlen(name) + sizeof("/domainlist") + 2 );
    strcpy(dl,"/dest/");
    strcat(dl,name);
    strcat(dl,"/domainlist");

    sp->domainlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(dl) + 2 );
    strcpy(sp->domainlist,dbhome);
    strcat(sp->domainlist,"/");
    strcat(sp->domainlist,dl);
  }
  else
  {
    if (domainlist[0] == '/') 
      sp->domainlist = domainlist;
    else
    {
       sp->domainlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(domainlist) + 2 );
       strcpy( sp->domainlist, dbhome );
       strcat( sp->domainlist, "/" );
       strcat( sp->domainlist, domainlist );
       ufdbFree( domainlist );
    }
  }

  sp->domainlistDb = (struct sgDb *) ufdbCalloc( 1, sizeof(struct sgDb) );
  sp->domainlistDb->type = SGDBTYPE_DOMAINLIST;
  sgDbInit( sp->domainlistDb, sp->domainlist );
  ufdbLogMessage( "loading URL table from \"%s\"", sp->domainlist );
  if (sp->domainlistDb->dbcp == NULL  ||
      ((struct UFDBmemTable *) sp->domainlistDb->dbcp)->table.nNextLevels == 0)
  {
    ufdbLogError( "URL database table \"%s\" is empty and is ignored   *****", sp->domainlist );
    ufdbFreeDomainDb( sp->domainlistDb );
    sp->domainlistDb = NULL;
  }
}


void ufdbCategoryExecDomainList( 
   char * command )
{
   struct Category * sp;

   if (UFDBglobalTerminating)
      return;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryExecDomainList %s", command );
#endif

   sp = lastCat;

   if (sp == NULL)
   {
      ufdbLogFatalError( "execdomainlist is not within a URL category" );
      return;
   }
   sp->execdomainlist = command;

   if (UFDBglobalDebug)
      ufdbLogMessage( "execdomainlist for category %s: \"%s\"", sp->name, command );
}


void ufdbFreeDomainDb( 
   struct sgDb * dbp )
{
   struct UFDBmemTable * mt;

   if (dbp == NULL)
      return;

   mt = (struct UFDBmemTable *) dbp->dbcp;
   if (mt != NULL)
   {
      if (mt->index != NULL)
	 ufdbFree( mt->index );				/* version 2.x */
      else
	 UFDBfreeTableIndex_1_2( &(mt->table) ); 	/* version 1.2 */
      ufdbFree( (void *) mt->mem );
      ufdbFree( mt );
   }
   ufdbFree( dbp );
}


void ufdbFreeIpv4List( struct Ipv4 * p )
{
   struct Ipv4 * tmp;

   while (p != NULL)
   {
      ufdbFree( p->str );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeIpv6List( struct Ipv6 * p )
{
   struct Ipv6 * tmp;

   while (p != NULL)
   {
      ufdbFree( p->str );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeSourceList( struct Source * p )
{
   struct Source * tmp;

   while (p != NULL)
   {
      ufdbFreeIpv4List( p->ipv4 );
      ufdbFreeIpv6List( p->ipv6 );
      ufdbFree( p->name );
      if (p->domainDb != NULL)
	 UFDBmemDBdeleteDB( (struct UFDBmemDB *) p->domainDb );

      /* execuserlist content is saved during a reload */
      if (p->userDb != NULL  &&  p->userDb->type != SGDBTYPE_EXECUSERLIST)
      {
	 UFDBmemDBdeleteDB( (struct UFDBmemDB *) p->userDb->dbcp );
	 ufdbFree( p->userDb );
      }
      ufdbFree( p->sarg0 );

      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAclCategoryList( struct AclCategory * p )
{
   struct AclCategory * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeAclCategoryList %08lx  %08lx  %s", (long) p, (long) p->name, p->name ); */
      ufdbFree( p->name );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAclList( struct Acl * p )
{
   struct Acl * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeAclList %08lx  %08lx  %s  %s", (long) p, (long) p->name, p->name, p->within == UFDB_ACL_ELSE ? "else" : "" ); */
      ufdbFree( p->name );
      /* ufdbLogMessage( "   freeing pass aclcategory list" ); */
      ufdbFreeAclCategoryList( p->pass );
      /* ufdbLogMessage( "   freeing implicit pass aclcategory list" ); */
      ufdbFreeAclCategoryList( p->implicitPass );
      ufdbFree( p->redirect );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeSettingList( struct ufdbSetting * p )
{
   struct ufdbSetting * tmp;
 
   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeSettingList %08lx  %08lx", (long) p, (long) p->name ); */
      ufdbFree( p->name );
      ufdbFree( p->value );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


static void ufdbFreeCategoryList( struct Category * p )
{
   struct Category * tmp;

   while (p != NULL)
   {
      /* ufdbLogMessage( "ufdbFreeCategoryList %08lx  %08lx  %s", (long) p, (long) p->name, p->name ); */
      if (UFDBglobalDebug > 2  ||  UFDBglobalDebugRegexp)
         ufdbLogMessage( "ufdbFreeCategoryList: freeing regexp of category %s", p->name );
      ufdbFreeRegExprList( p->regExp );
      ufdbFree( p->name );
      ufdbFree( p->domainlist );
      ufdbFreeDomainDb( p->domainlistDb );
      ufdbFree( p->execdomainlist );
      ufdbFree( p->cse );
      ufdbFree( p->cse2 );
      ufdbFree( p->expressionlist );
      ufdbFree( p->redirect );
      /* ufdbFree( p->rewrite );  freed in ufdbFreeRewriteList() */
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeRewriteList( 
   struct sgRewrite * p )
{
   struct sgRewrite * tmp;

   while (p != NULL)
   {
      ufdbFreeRegExprList( p->rewrite );
      ufdbFree( p->name );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeTimeElement( 
   struct TimeElement * p )
{
   struct TimeElement * tmp;

   while (p != NULL)
   {
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeTime( 
   struct ufdbTime * p )
{
   struct ufdbTime * tmp;

   while (p != NULL)
   {
      ufdbFree( p->name );
      ufdbFreeTimeElement( p->element );
      tmp = p->next;
      ufdbFree( p );
      p = tmp;
   }
}


void ufdbFreeAllMemory( void )
{
#if 0
   /* we cannot stop using the log file. we never close it */
   if (UFDBglobalLogDir != NULL)
      ufdbFree( UFDBglobalLogDir );
   UFDBglobalLogDir = NULL;
   globalLogFile = NULL;
#endif

#if 0
   /* we may need these variables during a reload */
   if (UFDBglobalPidFile != NULL)
   {
      ufdbFree( UFDBglobalPidFile );
      UFDBglobalPidFile = NULL;
   }
   if (UFDBglobalEmailServer != NULL)
   {
      ufdbFree( UFDBglobalEmailServer );
      UFDBglobalEmailServer = NULL;
   }
   if (UFDBglobalMyHostname != NULL)
   {
      ufdbFree( UFDBglobalMyHostname );
      UFDBglobalMyHostname = NULL;
   }
   if (UFDBglobalAdminEmail != NULL)
   {
      ufdbFree( UFDBglobalAdminEmail );
      UFDBglobalAdminEmail = NULL;
   }
   if (UFDBglobalSenderEmail != NULL)
   {
      ufdbFree( UFDBglobalSenderEmail );
      UFDBglobalSenderEmail = NULL;
   }
   if (UFDBglobalExternalStatusCommand != NULL)
   {
      ufdbFree( UFDBglobalExternalStatusCommand );
      UFDBglobalExternalStatusCommand = NULL;
   }
#endif

   ufdbFreeSettingList( Setting );
   Setting = NULL;
   ufdbFreeRewriteList( Rewrite );
   Rewrite = NULL;
   ufdbFreeCategoryList( Cat );
   Cat = NULL;
   ufdbFreeAclList( AclList );
   AclList = NULL;

   ufdbFreeSourceList( (struct Source *) SourceList );
   UFDBsetSourceList( NULL );
   lastSource = NULL;

   /* free UFDBglobalCheckedDB */
   if (UFDBglobalCheckedDB.table.nNextLevels > 0  &&  UFDBglobalCheckedDB.mem != NULL)
   {
      if (UFDBglobalCheckedDB.index != NULL)
         ufdbFree( (void *) UFDBglobalCheckedDB.index );
      else
	 UFDBfreeTableIndex_1_2( &(UFDBglobalCheckedDB.table) ); 		/* .nextLevel[0]) ); */
      ufdbFree( (void *) UFDBglobalCheckedDB.mem );
   }
   UFDBglobalCheckedDB.table.nNextLevels = 0;
   UFDBglobalCheckedDB.mem = NULL;
   UFDBglobalCheckedDB.index = NULL;

   if (UFDBglobalCheckedExpressions != NULL)
   {
      ufdbFreeRegExprList( UFDBglobalCheckedExpressions );
      UFDBglobalCheckedExpressions = NULL;
   }

   lastSetting = NULL;
   Setting = NULL;

   lastCat = NULL;
   Cat = NULL;

   lastRewrite = NULL;
   Rewrite = NULL;
   lastRewriteRegExec = NULL;

   ufdbFreeTime( TimeList );
   TimeList = NULL;
   lastTime = NULL;

   ufdbFree( TimeElementsEvents );
   TimeElementsEvents = NULL;
   numTimeElements = 0;

   lastTimeElement = NULL;
   TimeElement = NULL;

   lastAcl = NULL;
   defaultAcl = NULL;
   AclList = NULL;
   lastAclCategory = NULL;

   lastRegExpDest = NULL;

   ufdbFree( UFDBglobalSquidVersion );
   UFDBglobalSquidVersion = NULL;
}


void ufdbCategoryUrlList( 
   char * urllist )
{
   if (urllist == NULL)
      urllist = (char *) "-";
   ufdbLogError( "line %d: \"urllist %s\" is deprecated and ignored *****\n"
                 "ufdbGenTable should be called with the -u option to include URLs\n"
		 "ufdbGenTable combines URLs and domains in one table file so only the domainlist is required",
		 lineno, urllist );
   if (urllist[0] != '-')
      ufdbFree( urllist );
}


void ufdbCategoryExpressionList( 
  char * exprlist, 
  const char * chcase  )
{
  FILE * fp;
  char * dbhome;
  char * dl;
  char * name;
  char * p;
  int    flags;
  struct stat         statbuf;
  struct Category *   sp;
  struct ufdbRegExp * regexp;
  char   buf[UFDB_MAX_URL_LENGTH];
  char   errbuf[256];

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryExpressionList %s", exprlist );
#endif

  if (UFDBglobalTerminating)
     return;

  flags = REG_EXTENDED | REG_NOSUB;
  dbhome = ufdbSettingGetValue( "dbhome" );
  sp = lastCat;
  if (dbhome == NULL)
    dbhome = (char *) DEFAULT_DBHOME;

  if (exprlist == NULL)
  {
    name = sp->name;
    dl = (char *) ufdbMalloc( sizeof("/dest/") + strlen(name) + sizeof("/expressionlist") + 2 );
    strcpy(dl,"/dest/");
    strcat(dl,name);
    strcat(dl,"/expressionlist");

    flags |= REG_ICASE; 	/* default case insensitive */

    sp->expressionlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(dl) + 2 );
    strcpy(sp->expressionlist,dbhome);
    strcat(sp->expressionlist,"/");
    strcat(sp->expressionlist,dl);
  }
  else
  {
    if (exprlist[0] == '/') 
    {
      sp->expressionlist = exprlist;
    }
    else
    {
       sp->expressionlist = (char *) ufdbMalloc( strlen(dbhome) + strlen(exprlist) + 2 );
       strcpy( sp->expressionlist, dbhome );
       strcat( sp->expressionlist, "/" );
       strcat( sp->expressionlist, exprlist );
       ufdbFree( exprlist );
    }
    if (*chcase == 'i')
       flags |= REG_ICASE;     /* set case insensitive */
  }

  ufdbLogMessage( "loading regular expressions from \"%s\"", sp->expressionlist );

  if ((fp = fopen(sp->expressionlist, "r")) == NULL) 
  {
    ufdbLogFatalError( "cannot open expression list %s: %s", sp->expressionlist, strerror(errno) );
    return;
  }

  if (0 == fstat( fileno(fp), &statbuf ))
  {
     if (!S_ISREG(statbuf.st_mode))
     {
        ufdbLogFatalError( "expression list %s: not a regular file", sp->expressionlist );
	fclose( fp );
	return;
     }
  }

  while (!feof(fp)  &&  fgets(buf, sizeof(buf), fp) != NULL) 
  {
    if (buf[0] == '#')
       continue;

    p = (char *) strchr( buf, '\n' );
    if (p != NULL  &&  p != buf) 
    {
      if (*(p-1) == '\r') 	/* removing ^M  */
	p--;
      *p = '\0';
    }
    /* TO-DO: warn about leading and trailing spaces */
    regexp = ufdbNewPatternBuffer( buf, flags );
    if (regexp->error) 
    {
      regerror( regexp->error, regexp->compiled[0], errbuf, sizeof(errbuf) );
      ufdbLogError( "regular expression error in %s:\n%s: %s   *****", sp->expressionlist, errbuf, buf );
    }
    if (lastCat->regExp == NULL) 
    {
      lastCat->regExp = regexp;
      lastRegExpDest = regexp;
    }
    else
    {
      lastRegExpDest->next = regexp;
      lastRegExpDest = regexp;
    }
  }

  fclose( fp );

  if (UFDBglobalExpressionOptimisation)
     lastCat->regExp = UFDBoptimizeExprList( sp->expressionlist, lastCat->regExp );
}


void ufdbCategoryCACertsFile(
   char *              cacertsFile )
{
   struct Category *   cat;

   cat = lastCat;
   if (cat == NULL  ||  strcmp( cat->name, "security" ) != 0)
   {
      ufdbLogFatalError( "cacerts can only be defined inside the \"security\" category" );
      return;
   }

   cat->cse = cacertsFile;
   if (*cacertsFile == '/')
      strcpy( UFDBglobalCAcertsFile, cacertsFile );
   else
   {
      char * dbh;
      dbh = ufdbSettingGetValue( "dbhome" );
      if (dbh == NULL)
         dbh = (char *) DEFAULT_DBHOME;
      strcpy( UFDBglobalCAcertsFile, dbh );
      strcat( UFDBglobalCAcertsFile, "/" );
      strcat( UFDBglobalCAcertsFile, cacertsFile );
   }
}


void ufdbCategoryCACertsDir(
   char *              cacertsDir )
{
   struct Category *   cat;

   cat = lastCat;
   if (cat == NULL  ||  strcmp( cat->name, "security" ) != 0)
   {
      ufdbLogFatalError( "cacerts-dir can only be defined inside the \"security\" category" );
      return;
   }

   cat->cse2 = cacertsDir;
   if (*cacertsDir == '/')
      strcpy( UFDBglobalCAcertsDir, cacertsDir );
   else
   {
      char * dbh;
      dbh = ufdbSettingGetValue( "dbhome" );
      if (dbh == NULL)
        dbh = (char *) DEFAULT_DBHOME;
      strcpy( UFDBglobalCAcertsDir, dbh );
      strcat( UFDBglobalCAcertsDir, "/" );
      strcat( UFDBglobalCAcertsDir, cacertsDir );
   }
}


void ufdbCategoryRedirect( 
   char * value )
{
   struct Category * sp;

#if UFDB_DEBUG
   ufdbLogMessage( "ufdbCategoryRedirect %s", value );
#endif

   sp = lastCat;
   sp->redirect = value;
   /* TODO: check that "localhost" is not here if no 302 is used */
}


void ufdbCategoryRewrite( 
  char *              value )
{
  struct sgRewrite *  rewrite;

  if ((rewrite = sgRewriteFindName(value)) == NULL)
  {
    ufdbLogFatalError( "line %d: rewrite %s is not defined in configuration file %s",
		       lineno, value, UFDBglobalConfigFile );
    return;
  }

  lastCat->rewrite = rewrite;
}


void ufdbCategoryBlockConnect( 
   int flag  )
{
   lastCat->blockBumpedConnect = flag;

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbCategoryBlockConnect: category \"%s\" : block-bumped-connect %s",
                      lastCat->name, flag ? "on" : "off" );
}


void ufdbCategoryTime(  
  char *        name, 
  int  		within )
{
  struct ufdbTime * t;

  if ((t = sgTimeFindName(name)) == NULL) 
  {
    ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		       lineno, name, UFDBglobalConfigFile );
    ufdbFree( name );
    return;
  }

  lastCat->within = within;
  lastCat->time = t;
  ufdbFree( name );
}


struct Category * ufdbCategoryFindName( 
  char *               name )
{
  struct Category * p;

  for (p = Cat; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


void sgRewrite( 
  char * rewrite )
{
  struct sgRewrite * rew;

#if UFDB_DEBUG
   ufdbLogMessage( "sgRewrite %s", rewrite );
#endif

  if (Rewrite != NULL)
  {
    if ((struct sgRewrite *) sgRewriteFindName(rewrite) != NULL)
    {
      ufdbLogFatalError( "line %d: rewrite \"%s\" is not defined in configuration file %s",
		       lineno, rewrite, UFDBglobalConfigFile );
      ufdbFree( rewrite );
      return;
    }
  }

  rew = (struct sgRewrite *) ufdbMalloc( sizeof(struct sgRewrite) );
  rew->name = rewrite;
  rew->active = 1;
  rew->rewrite = NULL;
  rew->time = NULL;
  rew->within = UFDB_ACL_NONE;
  rew->next = NULL;

  if (Rewrite == NULL) 
  {
    Rewrite = rew;
    lastRewrite = rew;
  }
  else
  {
    lastRewrite->next = rew;
    lastRewrite = rew;
  }
}


void sgRewriteTime( 
  char * name, 
  int    within )
{
  struct ufdbTime * t;

#if UFDB_DEBUG
   ufdbLogMessage( "sgRewriteTime %s %d", name, within );
#endif

  if ((t = sgTimeFindName(name)) == NULL)
  {
    ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		       lineno, name, UFDBglobalConfigFile );
    ufdbFree( name );
    return;
  }

  lastRewrite->within = within;
  lastRewrite->time = t;
  ufdbFree( name );
}


void sgRewriteSubstitute( 
  char * string )
{
  char * pattern; 
  char * subst = NULL;
  char * p;
  int    flags = REG_EXTENDED;
  int    global = 0;
  char * httpcode = NULL;
  struct ufdbRegExp * regexp;
  char   errbuf[256];

  pattern = string + 2; 	/* skipping s@ */
  p = pattern;
  while ((p = strchr(p,'@')) != NULL)
  {
    if (*(p - 1) != '\\') 
    {
      *p = '\0';
      subst = p + 1;
      break;
    }
    p++;
  }

  p = strrchr( subst, '@' );
  while (p != NULL  &&  *p != '\0')
  {
    if (*p == 'r' )
      httpcode =  (char *) REDIRECT_TEMPORARILY;
    if (*p == 'R' )
      httpcode =  (char *) REDIRECT_PERMANENT;
    if (*p == 'i' || *p == 'I')
      flags |= REG_ICASE;
    if (*p == 'g')
      global = 1;
    *p = '\0'; 		/* removes @i from string */
    p++;
  } 

  regexp = ufdbNewPatternBuffer( pattern, flags );
  if (regexp->error) 
  {
      regerror( regexp->error, regexp->compiled[0], errbuf, sizeof(errbuf) );
      ufdbLogError( "line %d: regular expression error in %s:\n%s   *****", lineno, pattern, errbuf );
  }
  else {
    regexp->substitute = ufdbStrdup( subst );
  }

  if (lastRewrite->rewrite == NULL)
    lastRewrite->rewrite = regexp;
  else 
    lastRewriteRegExec->next = regexp;
  regexp->httpcode = httpcode;
  regexp->global = global;
  lastRewriteRegExec = regexp;
}


struct sgRewrite * sgRewriteFindName( 
  char * name )
{
  struct sgRewrite * p;

  for (p = Rewrite; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


/*
 * Time functions
 */

/*
 * sgTime - parse configuration time statement.
 */
void sgTime( 
  char *        name )
{
  struct ufdbTime * t;

  if (UFDBglobalDebug)
     ufdbLogMessage( "sgTime %s", name );

  if (TimeList != NULL)
  {
    if ((struct ufdbTime *) sgTimeFindName(name) != NULL)
    {
      ufdbLogFatalError( "line %d: time \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
    }
  }
  else 
    numTimeElements = 0;

  t = (struct ufdbTime *) ufdbMalloc( sizeof(struct ufdbTime) );
  t->name = name;
  t->active = 1;
  t->element = NULL;
  t->next = NULL;

  TimeElement = NULL;
  lastTimeElement = NULL;
  if (TimeList == NULL) 
  {
    TimeList = t;
    lastTime = t;
  }
  else
  {
    lastTime->next = t;
    lastTime = t;
  }
}


/*
 * sgTimeElementInit - initialise parsing of a configuration time element.
 */
void sgTimeElementInit( void )
{
   struct TimeElement * te;

   te = (struct TimeElement *) ufdbCalloc( 1, sizeof(struct TimeElement) );
   numTimeElements++;

   if (lastTime->element == NULL)
     lastTime->element = te;
   if (lastTimeElement != NULL)
     lastTimeElement->next = te;
   lastTimeElement = te;
}


/*
 * sgTimeElementEnd - finalise parsing of configuration time element.
 */
void sgTimeElementEnd( void )
{
  time_switch = 0;
  date_switch = 0;

  if (lastTimeElement->fromdate != 0)
  {
    if (lastTimeElement->todate == 0)
      lastTimeElement->todate = lastTimeElement->fromdate + 86399;
    else 
      lastTimeElement->todate = lastTimeElement->todate + 86399;
  }

  if (lastTimeElement->from == 0  &&  lastTimeElement->to == 0)
    lastTimeElement->to = 1439;  /* set time to 23:59 */
}


/*
 * sgTimeElementAdd - add configuration time element.
 */
void sgTimeElementAdd( 
  char * element, 
  char   type ) 
{
  struct TimeElement * te;
  char * p;
  char   wday;
  int    h, m, Y, M, D;
  time_t sec;
  char * lineptr;

  wday = 0;
  M = 0;
  D = -1;
  te = lastTimeElement;

  switch (type)
  {
  case T_WEEKDAY:
    p = strtok_r( element, " \t,", &lineptr );
    do {
      if (*p == '*') {
	wday = 0x7F;
      } else if (!strncmp(p,"sun",3)) {
	wday = wday | 0x01;
      } else if (!strncmp(p,"mon",3)) {
	wday = wday | 0x02;
      } else if (!strncmp(p,"tue",3)) {
	wday = wday | 0x04;
      } else if (!strncmp(p,"wed",3)) {
	wday = wday | 0x08;
      } else if (!strncmp(p,"thu",3)) {
	wday = wday | 0x10;
      } else if (!strncmp(p,"fri",3)) {
	wday = wday | 0x20;
      } else if (!strncmp(p,"sat",3)) {
	wday = wday | 0x40;
      }
      p = strtok_r( NULL, " \t,", &lineptr );
    } while (p != NULL);
    te->wday = wday;
    break;

  case T_TVAL:
    h = -1;
    m = -1;
    sscanf( element, "%d:%d", &h, &m );
    if ((h < 0 || h > 24) || (m < 0 || m > 59))
    {
      ufdbLogFatalError( "line %d: time format error in %s", lineno, UFDBglobalConfigFile );
      h = 0;
      m = 0;
    }
    if (time_switch == 0) 
    {
      time_switch++;
      te->from = (h * 60) + m ;
    }
    else
    {
      time_switch = 0;
      te->to = (h * 60) + m ;
    }
    break;

  case T_DVAL:
    sec = date2sec( element );
    if (sec == -1) 
    {
      ufdbLogFatalError( "line %d: date format error in %s", lineno, UFDBglobalConfigFile );
      sec = 1;
    }
    if (date_switch == 0) {
      date_switch++;
      te->fromdate = sec;
    } else {
      date_switch = 0;
      te->todate = sec;
    }
    break;

  case T_DVALCRON:
    p = strtok_r( element, "-./", &lineptr );
    Y = atoi(p);
    if (*p == '*')
      Y = -1;
    else
      Y = atoi(p);
    while ((p=strtok_r(NULL,"-./",&lineptr)) != NULL) 
    {
      if (*p == '*')
	if (M == 0)
	  M = -1;
	else 
	  D = -1;
      else
	if (M == 0)
	  M = atoi(p);
	else
	  D = atoi(p);
    }
    te->y = Y;
    te->m = M;
    te->d = D;
    break;

  case T_WEEKLY:
    p = element;
    while (*p != '\0') 
    {
      switch (*p) {
      case 'S':
      case 's':
	wday = wday | 0x01;
	break;
      case 'M':
      case 'm':
	wday = wday | 0x02;
	break;
      case 'T':
      case 't':
	wday = wday | 0x04;
	break;
      case 'W':
      case 'w':
	wday = wday | 0x08;
	break;
      case 'H':
      case 'h':
	wday = wday | 0x10;
	break;
      case 'F':
      case 'f':
	wday = wday | 0x20;
	break;
      case 'A':
      case 'a':
	wday = wday | 0x40;
	break;
      default:
	ufdbLogFatalError( "line %d: weekday format error in %s", lineno, UFDBglobalConfigFile );
	break;
      }
      p++;
    }
    te->wday = wday;
    break;
  }

  ufdbFree( element );
}


/* 
 * lookup a ufdbTime element by name.
 */
struct ufdbTime * sgTimeFindName( 
  char *        name )
{
  struct ufdbTime * p;

  for (p = TimeList; p != NULL; p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


/*
 * sgTimeCmp - Time array sort function.
 */
static int sgTimeCmp( const void * a, const void * b )
{
   const int * aa = (const int *) a;
   const int * bb = (const int *) b;

   return *aa - *bb;
}


/*
 * getSortedTimeElementTimes - produce a sorted array of time element times
 */
static void getSortedTimeElementTimes( void )
{
   struct ufdbTime *    p;
   struct TimeElement * te;
   int                  i, j;
   int                  totalNumElems;
 
   if (UFDBglobalDebug)
      ufdbLogMessage( "getSortedTimeElementEvents" );

   if (TimeList == NULL)
      return;

   /* find total number of time elements */
   totalNumElems = 0;
   for (p = TimeList;  p != NULL;  p = p->next) 
      for (te = p->element;  te != NULL;  te = te->next) 
         totalNumElems++;

   TimeElementsEvents = (int *) ufdbCalloc( totalNumElems * 2 , sizeof(int) ); 

   i = 0;
   for (p = TimeList;  p != NULL;  p = p->next) 
   {
      for (te = p->element;  te != NULL;  te = te->next) 
      {
         TimeElementsEvents[i++] = te->from == 0 ? 1440 : te->from;
         TimeElementsEvents[i++] = te->to == 0   ? 1440 : te->to;
      }
   }
 
   qsort( TimeElementsEvents, totalNumElems * 2, sizeof(int), sgTimeCmp );

   if (UFDBglobalDebug)
      ufdbLogMessage( "   getSortedTimeElementEvents: after qsort" );

   /* remove identical time elements */
   for (i=1,j=1;  i < totalNumElems * 2;  i++) 
   {
      if (TimeElementsEvents[i] > TimeElementsEvents[i-1])
      {
	 TimeElementsEvents[j] = TimeElementsEvents[i];
         j++;
      }
   }

   numTimeElements = j;			/* #unique time elements */
}


/*
 * _TimeEvaluateElements - evaluate all elements if they match the current time/date and mark them active.
 */
static void _TimeEvaluateElements( 
  struct tm * tm_now, 
  time_t      now )
{
  struct ufdbTime *    tlist;
  struct TimeElement * te;
  int                  min;

  if (UFDBglobalDebug)
     ufdbLogMessage( "_TimeEvaluateElements" );

  for (tlist = TimeList;  tlist != NULL;  tlist = tlist->next)
  {
    tlist->active = 0;
    for (te = tlist->element;  te != NULL;  te = te->next)
    {
      if (te->wday != 0) 			/* check wday */
      {
	if (((1 << tm_now->tm_wday) & te->wday) != 0)  
	{
	  min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	  if (min >= te->from  &&  min < te->to) 
	  {
	    tlist->active = 1;
	    break;
	  }
	}
      }
      else if (te->fromdate != 0)		/* check date */
      {
	if (now >= te->fromdate  &&  now <= te->todate) 
	{
	  min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	  if (min >= te->from  &&  min < te->to) 
	  {
	    tlist->active = 1;
	    break;
	  }
	}
      }
      else					/* check crondate */
      {
	if (te->y == -1  ||  te->y == (tm_now->tm_year + 1900)) 
	{
	  if (te->m == -1  ||  te->m == (tm_now->tm_mon + 1)) 
	  {
	    if (te->d == -1  ||  te->d == (tm_now->tm_mday)) 
	    {
	      min = (tm_now->tm_hour * 60 ) + tm_now->tm_min;
	      if (min >= te->from  &&  min < te->to) 
	      {
		tlist->active = 1;
		break;
	      }
	    }
	  }
	}
      }
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeEvaluateElements: time %s is %sactive", tlist->name, tlist->active?"":"not " );
  }
}


/*
 * _TimeSetAclSrcDestRew - mark all acl/source/dest/rew (in)active.
 */
static void _TimeSetAclSrcDestRew( void )
{
  struct Acl *         acl;
  struct Category *    cat;
  struct Source *      s;
  struct sgRewrite *   rew;
  int                  a;

  if (UFDBglobalDebug)
     ufdbLogMessage( "_TimeSetAclSrcDestRew" );

  for (acl = AclList;  acl != NULL;  acl = acl->next)
  {
    if (acl->time != NULL  &&  acl->within != UFDB_ACL_ELSE) 
    {
      /* Be careful here: we are multithreaded and other threads use the value 
       * of acl->active at the same time.
       */
      a = acl->time->active;
      if (acl->within == UFDB_ACL_OUTSIDE)
	a = !a;
      if (acl->next != NULL  &&  acl->next->within == UFDB_ACL_ELSE) 
	acl->next->active = !a;
      acl->active = a;
    }
#if 0
    if (acl->pass == NULL)
       acl->active = 0;			/* it can have a 'continue' so do not make it inactive */
#endif
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: acl %s %s is %sactive", acl->name, 
		       acl->within==UFDB_ACL_ELSE ? "ELSE" :
			  acl->within==UFDB_ACL_WITHIN ? "WITHIN" :
			     acl->within==UFDB_ACL_OUTSIDE ? "OUTSIDE" : "",
		       acl->active?"":"not " );
  }

  for (cat = Cat;  cat != NULL;  cat = cat->next)
  {
    if (cat->time != NULL) 
    {
      cat->active = cat->time->active;
      if (cat->within == UFDB_ACL_OUTSIDE)
	cat->active = !cat->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: category %s is %sactive", cat->name, cat->active?"":"not " );
  }

  for (s = UFDBgetSourceList();  s != NULL;  s = s->next)
  {
    if (s->time != NULL) 
    {
      s->active = s->time->active;
      if (s->within == UFDB_ACL_OUTSIDE)
	s->active = !s->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: source %s is %sactive", s->name, s->active?"":"not " );
  }

  for (rew = Rewrite; rew != NULL; rew = rew->next)
  {
    if (rew->time != NULL) 
    {
      rew->active = rew->time->active;
      if (rew->within == UFDB_ACL_OUTSIDE)
	 rew->active = !rew->active;
    }
    if (UFDBglobalDebug)
       ufdbLogMessage( "      _TimeSetAclSrcDestRew: rewite %s is %sactive", rew->name, rew->active?"":"not " );
  }
}


/*
 *  ufdbHandleAlarmForTimeEvents - the alarm for the next time event went off so 
 *                                 recalculate the time-dependent active state.
 */
void ufdbHandleAlarmForTimeEvents( 
   int        why )
{
   time_t     now;
   struct tm  tm_now;
   int        m;
   int        tindex;
   int        lastval;

   if (UFDBglobalDebug)
      ufdbLogMessage( "ufdbHandleAlarmForTimeEvents" );

   if (why == UFDB_PARAM_INIT)
      ufdbLogMessage( "time definitions are used; evaluating current ACLs" );
   else
   {
      ufdbLogMessage( "alarm went off to recalculate time ACLs" );
      if (UFDBglobalTerminating)
      {
	 ufdbLogMessage( "This alarm is ignored because ufdbguardd is exiting" );
	 return;
      }
      if (UFDBglobalReconfig)
      {
	 ufdbLogMessage( "This alarm is ignored because the configuration is being reloaded"
	                 " and a new alarm is set to go off in 15 seconds" );
	 alarm( 15 );
	 return;
      }
   }

   if (TimeList == NULL)
   {
      return;
   }

   getSortedTimeElementTimes();

   now = time(NULL) + 30 + globalDebugTimeDelta;
   localtime_r( &now, &tm_now ); 
   m = (tm_now.tm_hour * 60) + tm_now.tm_min;
  
   lastval = 0;
   for (tindex = 0;  tindex < numTimeElements;  tindex++)
   {
#if UFDB_TIME_DEBUG
      if (UFDBglobalDebug)
         ufdbLogMessage( "   TimeElementsEvents[%d] = %d", tindex, TimeElementsEvents[tindex] );
#endif
      lastval = TimeElementsEvents[tindex];
      if (TimeElementsEvents[tindex] >= m)
         break;
   }

   if (UFDBglobalDebug)
      ufdbLogMessage( "   ufdbHandleAlarmForTimeEvents: m = %d  tindex = %d, lastval = %d", m, tindex, lastval );

   if (lastval < m)
      m = (((1440 - m) + TimeElementsEvents[0]) * 60) - tm_now.tm_sec;
   else
      m = ((lastval - m) * 60) - tm_now.tm_sec;

   if (m <= 0)
      m = 30;

   _TimeEvaluateElements( &tm_now, now );
   _TimeSetAclSrcDestRew();

   ufdbFree( TimeElementsEvents );
   TimeElementsEvents = NULL;

   ufdbLogMessage( "next alarm is in %d seconds", (unsigned int) m );
   ufdbSetSignalHandler( SIGALRM, sgAlarm );
   (void) alarm( (unsigned int) m );
}


/*
 * sgTimeElementClone - copy a time specification.
 */
void sgTimeElementClone( void )
{
  struct TimeElement * te;
  struct TimeElement * tmp;

  te = lastTimeElement;
  if (te == NULL) 
  {
    ufdbLogFatalError( "No previous TimeElement in sgTimeElementClone !" );
    return;
  }
  else
  {
    sgTimeElementInit();
    lastTimeElement->wday = te->wday;
    lastTimeElement->from = te->from;
    lastTimeElement->to = te->to;
    lastTimeElement->y = te->y;
    lastTimeElement->m = te->m;
    lastTimeElement->d = te->d;
    lastTimeElement->fromdate = te->fromdate;
    lastTimeElement->todate = te->todate;
    tmp = lastTimeElement;
    lastTimeElement = te;
    sgTimeElementEnd();
    lastTimeElement = tmp;
  }
}


/*
 * IP functions
 */

void sgSetIpType( 
  int           type, 
  char *        file, 
  int           line )
{
  struct Ipv4 * ip;
  char *        p;
  char *        f;
  int           l;
  unsigned int   octet;
  unsigned int   op;

  ip = sgIpv4Last( lastSource );
  f = (file == NULL) ? UFDBglobalConfigFile : file;
  l = (line == 0) ? lineno : line;
  if (type == SG_IPTYPE_HOST)
    ip->mask = 0xffffffff;

  if (type == SG_IPTYPE_RANGE)
  {
    if (sgConvDot(ip->str,&op) == NULL)
    {
      ufdbLogFatalError( "IP address error in %s line %d", f, l );
      ip->mask = 0;
    }
    else 
      ip->mask = op;
    if ((unsigned int) ip->net > (unsigned int) ip->mask)
    {
      ufdbLogFatalError( "IP range error in %s line %d   %08x %08x", f, l, ip->net, ip->mask );
    }
  }

  if (type == SG_IPTYPE_CLASS)
  {
    p = ip->str;
    if (*p == '/')
      p++;
    if (sgConvDot(p,&op) == NULL)
    {
      ufdbLogFatalError( "IP address error in %s line %d", f, l );
      ip->mask = 0;
    }
    else 
      ip->mask = op;
  }

  if (type == SG_IPTYPE_CIDR)
  {
    p = ip->str;
    if (*p == '/')
      p++;
    octet = (unsigned long) atoi( p );
    if (octet > 32) 
    {
      ufdbLogFatalError( "IP address CIDR out of range \"%s\" in %s line %d", p, f, l );
      octet = 32;
    }
    if (octet == 32)
      ip->mask = 0xffffffff;
    else
      ip->mask = 0xffffffff ^ (0xffffffff >> octet);
    ip->net = ip->net & ip->mask;
  }

  ip->type = type;
  ip->next = (struct Ipv4 *) ufdbCalloc( 1, sizeof(struct Ipv4) );

#if 0
  ufdbLogMessage( "   sgSetIpType %2d  %08lx  %08lx", ip->type, ip->net, ip->mask );
#endif

  /* TO-DO: fix messy code where the last struct must contain zeros */
}


void sgIpv4( 
  const char * name )
{
  struct Ipv4 *  ipv4;
  unsigned int   op;

#if 0
  ufdbLogMessage( "   sgIpv4( %s )", name );
#endif

  if (lastSource->ipv4 == NULL) 
  {
    ipv4 = (struct Ipv4 *) ufdbCalloc( 1, sizeof(struct Ipv4) );
#if 0
    ip->str = NULL;
    ip->next = NULL;
#endif
    lastSource->ipv4 = ipv4;
    lastSource->lastipv4 = ipv4;
  }
  else
  {
    ipv4 = sgIpv4Last( lastSource );
  }

  if (ipv4->net_is_set == 0) 
  {
    ipv4->net_is_set = 1;
    if (sgConvDot(name,&op) == NULL) 
    {
      ufdbLogFatalError( "line %d: IP address error in %s", lineno, UFDBglobalConfigFile );
      ipv4->net = 0;
    }
    else 
      ipv4->net = op;
  }
  else
  {
    ipv4->str = ufdbStrdup( name );
  }

#if 0
  ufdbLogMessage( "   sgIpv4 %2d  %08lx  %08lx  %s", 
                  ipv4->type, ipv4->net, ipv4->mask, ipv4->str==NULL?"NULL":ipv4->str );
#endif

}


struct Ipv4 * sgIpv4Last( 
   struct Source * s )
{
   struct Ipv4 * ipv4;
   struct Ipv4 * last;

   last = NULL;
   for (ipv4 = s->ipv4;  ipv4 != NULL;  ipv4 = ipv4->next)
      last = ipv4;

   return last;
}


struct Ipv6 * sgIpv6Last( 
   struct Source * s )
{
   struct Ipv6 * ipv6;
   struct Ipv6 * last;

   last = NULL;
   for (ipv6 = s->ipv6;  ipv6 != NULL;  ipv6 = ipv6->next)
      last = ipv6;

   return last;
}



void sgIpv6( 
   const char * name,
   int          type,
   char *       file,
   int          line  )
{
   struct Ipv6 *  p;

#if 0
   ufdbLogMessage( "   sgIpv6( \"%s\", %d )", name, type );
#endif

   if (lastSource->ipv6 == NULL) 
   {
      p = (struct Ipv6 *) ufdbCalloc( 1, sizeof(struct Ipv6) );
      lastSource->ipv6 = p;
      lastSource->lastipv6 = p;
   }
   else
   {
      p = sgIpv6Last( lastSource );
   }

   p->str = ufdbStrdup( name );

   if (type == SG_IPV6TYPE_HOST)
   {
      if (!sgValidateIPv6( p->str, &p->ipv6 ))
      {
         ufdbLogFatalError( "incorrect IPv6 address \"%s\" in %s line %d", 
                            p->str, file, line );
      }
   }
   else if (type == SG_IPV6TYPE_CIDR)
   {
      char * s = strchr( p->str, '/' );
      if (s == NULL)
      {
         ufdbLogFatalError( "IPv6 net has no '/' in \"%s\" in %s line %d", 
                            p->str, file, line );
      }
      else
      {
         *s = '\0';
         p->cidr = (unsigned long) atoi( s+1 );
         if (p->cidr > 128) 
         {
            ufdbLogFatalError( "IPv6 address CIDR out of range \"%s\" in %s line %d", 
                               p->str, file, line );
            p->cidr = 128;
         }
         if (!sgValidateIPv6( p->str, &p->ipv6 ))
         {
            ufdbLogFatalError( "incorrect IPv6 address \"%s\" in %s line %d", 
                               p->str, file, line );
         }
         *s = '/';
      }
   }
   else
   {
      ufdbLogFatalError( "sgIpv6 called with unsupported type %d", type );
   }

   p->type = type;
   p->next = (struct Ipv6 *) ufdbCalloc( 1, sizeof(struct Ipv6) );

#if 1
   if (UFDBglobalDebug)
   {
      char newaddr[INET6_ADDRSTRLEN+1];

      newaddr[0] = '\0';
      inet_ntop( AF_INET6, (void *) &p->ipv6, newaddr, INET6_ADDRSTRLEN );
      ufdbLogMessage( "   sgIpv6 %2d  \"%s\"  %s %d", p->type, 
                      p->str==NULL ? "NULL" : p->str, newaddr, p->cidr );
   }
#endif
}


/*
 * ACL functions
 */

void sgAcl( 
   char *          name,
   char *          value, 
   int             within )
{
   struct Acl *    acl;
   struct Source * source;

#if UFDB_DEBUG
   ufdbLogMessage( "sgAcl name=\"%s\" value=\"%s\" within=%d line=%d", 
                   name==NULL?"NULL":name,  value==NULL?"NULL":value, within, lineno );
#endif

   if (AclList != NULL) 
   {
      if (sgAclFindName(name) != NULL)
      {
	 ufdbLogFatalError( "line %d: ACL \"%s\" is already defined in configuration file %s",
			    lineno, name, UFDBglobalConfigFile );
      }
   }

  if (within == UFDB_ACL_ELSE)
  {
     if (lastAcl == NULL)
     {
        ufdbLogFatalError( "line %d: ACL \"else\" has no parent ACL", lineno );
        return;
     }
     if (name == NULL)
        name = ufdbStrdup( lastAcl->name );
  }

  acl = (struct Acl *) ufdbMalloc( sizeof(struct Acl) );

  source = NULL;
  if (strcmp(name,"default") == 0)
  {
    defaultAcl = acl;
  }
  else
  {
    if ((source = defSourceFindName(name)) == NULL)
    {
      ufdbLogFatalError( "line %d: ACL source \"%s\" is not defined in configuration file %s",
		         lineno, name, UFDBglobalConfigFile );
      ufdbFree( name );
      return;
    }
  }

  acl->name = name;
  acl->active = within == UFDB_ACL_ELSE ? 0 : 1;
  acl->source = source;
  acl->pass = NULL;
  acl->implicitPass = NULL;
  acl->hasTerminatorNone = 0;
  acl->rewriteDefault = 1;
  acl->rewrite = NULL;
  acl->redirect = NULL;
  acl->time = NULL;
  acl->within = within;
  acl->next = NULL;

  if (value != NULL) 
  {
    struct ufdbTime * t;
    if ((t = sgTimeFindName(value)) == NULL) 
    {
      ufdbLogFatalError( "line %d: ACL time %s is not defined in configuration file %s",
		         lineno, value, UFDBglobalConfigFile );
      return;
    }
    acl->time = t;
  }

  if (AclList == NULL) 
  {
    AclList = acl;
    lastAcl = acl;
  }
  else
  {
    lastAcl->next = acl;
    lastAcl = acl;
  }
}


void sgAclSetValue( 
  const char *         what,
  char *               value, 
  int                  allowed ) 
{
  struct Category *    cat = NULL;
  struct AclCategory * aclcat;
  int                  type;
  
#if UFDB_DEBUG
   ufdbLogMessage( "sgAclSetValue %s %s%s", what, allowed?"":"!", value );
#endif

  if (lastAcl == NULL)
  {
     ufdbLogError( "error in configuration file on line %d: cannot set value for \"%s\" because there is no defined ACL", 
                   lineno, what );
     ufdbFree( value );
     return;
  }

  type = ACL_TYPE_TERMINATOR;

  if (strcmp(what,"pass") == 0)
  {
    if (strcmp(value,"any")==0 || strcmp(value,"all")==0)
    {
       if (!allowed)
          ufdbLogFatalError( "error in configuration file on line %d: do not use '!any' or '!all'.  Use 'none' instead.", lineno );
       allowed = 1;
    }
    else if (strcmp(value,"none") == 0)
    {
       if (!allowed)
          ufdbLogFatalError( "error in configuration file on line %d: do not use '!none'.  Use 'any' instead.", lineno );
       allowed = 0;
    }
    else if (strcmp(value,"in-addr") == 0) {
      type = ACL_TYPE_INADDR;
    }
    else
    {
      if ((cat = ufdbCategoryFindName(value)) == NULL) 
      {
	ufdbLogFatalError( "ACL category \"%s\" (line %d) is not defined in configuration file %s",
			   value, lineno, UFDBglobalConfigFile );
	ufdbFree( value );
        return;
      } 
      type = ACL_TYPE_DEFAULT;
    }

    aclcat = (struct AclCategory *) ufdbMallocAligned( 64, sizeof(struct AclCategory) );
    aclcat->name = value;
    aclcat->cat = cat;
    aclcat->access = allowed;
    aclcat->type = type;
    aclcat->next = NULL;
    aclcat->nblocks = 0;
    aclcat->nmatches = 0;

    if (lastAcl->pass == NULL) {
      lastAcl->pass = aclcat;
    } else {
      lastAclCategory->next = aclcat;
    }
    lastAclCategory = aclcat;
  }
  else if (strcmp(what,"redirect") == 0)
  {
    if (strcmp(value,"default") != 0)
    {
      lastAcl->redirect = value;
    }
    else 
    {
      lastAcl->redirect = NULL;
      ufdbFree( value );
    }
  }
  else if (strcmp(what,"rewrite") == 0)
  {
    if (strcmp(value,"none") == 0)
    {
      lastAcl->rewriteDefault = 0;
      lastAcl->rewrite = NULL;
    }
    else
    {
      struct sgRewrite * rewrite;

      if ((rewrite = sgRewriteFindName(value)) == NULL) 
      {
	ufdbLogFatalError( "rewrite %s is not defined in configuration file %s",
			   value, UFDBglobalConfigFile );
      }
      lastAcl->rewriteDefault = 0;
      lastAcl->rewrite = rewrite;
    }
    ufdbFree( value );
  }
}


struct Acl * sgAclFindName( 
  char *       name )
{
  struct Acl * p;

  if (name == NULL)
     return NULL;

  for (p = AclList;  p != NULL;  p = p->next)
  {
    if (strcmp(name,p->name) == 0)
      return p;
  }

  return NULL;
}


void yyerror( char * s )
{
   ufdbLogFatalError( "line %d: %s in configuration file %s", lineno, s, UFDBglobalConfigFile );
}


int yywrap()
{
  return 1;
}

