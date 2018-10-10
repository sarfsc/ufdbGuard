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
 * (GPL) along with this program.
 *
 * RCS: $Id: sg.l,v 1.75 2017/06/01 14:20:10 root Exp root $
 */


/* %pointer */

%{

#include "ufdb.h"
#include "sg.h"
#include "y.tab.h"

#include <stdio.h>
#include <string.h>

#if defined(UFDB_DEBUG) && 0
#define YY_USER_ACTION  ufdbLogMessage( "yytext line %d: \"%s\"", lineno, yytext );
#endif

#ifdef FLEX_SCANNER
#define ECHO  (void) fprintf( yyout, "%*.*s", (int) yyleng, (int) yyleng, yytext );
#endif

%}

d        [0-9]

ipv4     {d}{1,3}\.{d}{1,3}\.{d}{1,3}\.{d}{1,3}

ipv6field 	([[:xdigit:]]{1,4})
ipv6seq       	({ipv6field}(:{ipv6field})*)
ipv6fields     	({ipv6field}(:{ipv6field}){7})
ipv6addr      	({ipv6fields}|({ipv6seq}::({ipv6seq}?))|::{ipv6seq}|::)

ipv6net      	{ipv6addr}\/{d}{1,3}

tval     {d}{1,2}[:]{d}{d}
dval     {d}{d}{d}{d}[-/\.]{d}{1,2}[-/\.]{d}{1,2}
dvalcron ({d}{d}{d}{d}|[\*])[-/\.]({d}{1,2}|[\*])[-/\.]({d}{1,2}|[\*])

s        [\t ]
weekday  (mon|mondays?|tue|tuesdays?|wed|wednesdays?|thu|thursdays?|fri|fridays?|sat|saturdays?|sun|sundays?)
weekdays (({weekday}{s}*[, \t]+{s}*)*{weekday})|[\*]

%e 14000
%p 30000
%n 4000
%a 8000

%x REDIRECT_STATE

%%

#[^\n]*         { /* comment */ }
[ \t\f\r]+  	{ /* whitespace */ }
"\n"      	{ lineno++; }

{d}+        	{ yylval.string = ufdbStrdup(yytext); return NUMBER; }
{d}+","[0-9,]+  { yylval.string = ufdbStrdup(yytext); return NUMBERS; }

category    	return CATEGORY;
cat         	return CATEGORY;
destination 	return CATEGORY;
dest        	return CATEGORY;
source      	return SOURCE;
src         	return SOURCE;

rewrite     	return REWRITE;
rew         	return REWRITE;
time        	return TIME;
acl         	return ACL;

http-server 	{ return HTTP_SERVER; }
images      	{ return IMAGES; }
interface   	{ return INTERFACE; }
port        	{ return PORT; }

dbhome     	             { return DBHOME; }
logdir     	             { return LOGDIR; }
mail-server                  { return MAIL_SERVER; }
admin-email     	     { return ADMIN_EMAIL; }
sender-email		     { return SENDER_EMAIL; }
my-hostname   	             { return MY_HOSTNAME; }
external-status-command      { return EXTERNAL_STATUS_COMMAND; }
cpus       		     { return CPUS; }
analyse-uncategorised-urls   { return ANALYSE_UNCATEGORISED; }
analyze-uncategorised-urls   { return ANALYSE_UNCATEGORISED; }
log-uncategorised-urls       { return LOG_UNCATEGORISED_URLS; }
upload-stats                 { return UPLOAD_STATS; }

administrator               return ADMINISTRATOR;

safe-search          	    return SAFE_SEARCH;
max-logfile-size       	    return MAX_LOGFILE_SIZE;
logpass			    return LOGPASS;
logblock		    return LOGBLOCK;
logdeny			    return LOGBLOCK;
logall-httpd  		    return LOGALL_HTTPD;
logall  		    return LOGALL;
off                         return OFF;
on                          return ON;
deny-mode		    return DENY_MODE;
reuse-acl-names		    return REUSE_ACL_NAMES;

option			    return OPTION;
ufdb-expression-optimisation return UFDB_EXPRESSION_OPTIMISATION;
ufdb-expression-debug       return UFDB_EXPRESSION_DEBUG;
ufdb-debug-external-scripts return UFDB_DEBUG_EXTERNAL_SCRIPTS;
ufdb-show-url-details       return UFDB_SHOW_URL_DETAILS;
ufdb-log-url-details        return UFDB_LOG_URL_DETAILS;
ufdb-debug-filter      	    return UFDB_DEBUG_FILTER;
ufdb-debug-skype-probes	    return UFDB_DEBUG_SKYPE_PROBES;
ufdb-debug-gtalk-probes	    return UFDB_DEBUG_GTALK_PROBES;
ufdb-debug-yahoomsg-probes  return UFDB_DEBUG_YAHOOMSG_PROBES;
ufdb-debug-aim-probes       return UFDB_DEBUG_AIM_PROBES;
ufdb-debug-fb-chat-probes   return UFDB_DEBUG_FBCHAT_PROBES;
ufdb-debug-citrixonline-probes   return UFDB_DEBUG_CITRIXONLINE_PROBES;
url-lookup-delay-during-database-reload return URL_LOOKUP_DELAY_DB_RELOAD;
url-lookup-result-during-database-reload return URL_LOOKUP_RESULT_DB_RELOAD;
url-lookup-result-when-fatal-error       return URL_LOOKUP_RESULT_FATAL_ERROR;
block-bumped-connect   	    return BLOCK_BUMPED_CONNECT;
evaluate-and       	    return EVALUATE_AND;
evaluate-or        	    return EVALUATE_OR;
deny			    return TOKEN_DENY;
allow			    return TOKEN_ALLOW;
check-proxy-tunnels         return CHECK_PROXY_TUNNELS;
queue-checks	            return QUEUE_CHECKS;
agressive                   return AGGRESSIVE;
aggressive                  return AGGRESSIVE;
log-only                    return LOG_ONLY;
youtube-edufilter	    		return YOUTUBE_EDUFILTER;
youtube-edufilter-id	    		return YOUTUBE_EDUFILTER_ID;
allow-google-https-using-ip		return ALLOW_GOOGLE_HTTPS_USING_IP;
allow-skype-over-https      		return ALLOW_SKYPE_OVER_HTTPS;
allow-gtalk-over-https      		return ALLOW_GTALK_OVER_HTTPS;
allow-yahoomsg-over-https      		return ALLOW_YAHOOMSG_OVER_HTTPS;
allow-aim-over-https      		return ALLOW_AIM_OVER_HTTPS;
allow-fb-chat-over-https      		return ALLOW_FBCHAT_OVER_HTTPS;
allow-citrixonline-over-https 		return ALLOW_CITRIXONLINE_OVER_HTTPS;
allow-teamviewer-over-https 		return ALLOW_TEAMVIEWER_OVER_HTTPS;
allow-anydesk-over-https 		return ALLOW_ANYDESK_OVER_HTTPS;
allow-unknown-protocol-over-https	return ALLOW_UNKNOWN_PROTOCOL_OVER_HTTPS;
enforce-https-with-hostname 		return ENFORCE_HTTPS_WITH_HOSTNAME;
enforce-https-offical-certificate 	return ENFORCE_HTTPS_OFFICAL_CERTIFICATE;
enforce-https-official-certificate 	return ENFORCE_HTTPS_OFFICAL_CERTIFICATE;
https-prohibit-insecure-sslv2 		return HTTPS_PROHIBIT_INSECURE_SSLV2;
https-prohibit-insecure-sslv3 		return HTTPS_PROHIBIT_INSECURE_SSLV3;
https-connection-cache-size 		return HTTPS_CONNECTION_CACHE_SIZE;
strip-domain-from-username		return STRIP_DOMAIN_FROM_USERNAME;
squid-uses-ssl-bump			return SQUID_USES_SSL_BUMP;
squid-uses-active-bumping		return SQUID_USES_SSL_BUMP;
squid-version				return SQUID_VERSION;
num-worker-threads			return UFDB_NUM_WORKER_THREADS;
upload-crash-reports			return UPLOAD_CRASH_REPORTS;
lookup-reverse-ip			return LOOKUP_REVERSE_IP;
use-ipv6-on-wan				return USE_IPV6_ON_WAN;

domainlist         	return DOMAINLIST;
execdomainlist     	return EXECDOMAINLIST;
refreshdomainlist  	return REFRESHDOMAINLIST;
urllist            	return URLLIST;
expressionlist     	return EXPRESSIONLIST;

cacerts		return CACERTS;
cacerts-dir	return CACERTSDIR;

ignorecase	return IGNORECASE;

unix        	return UNIX;
ldap        	return LDAP;
userlist    	return USERLIST;
user        	return USER;
group       	return GROUP;
execuserlist   	return EXECUSERLIST;
refreshuserlist return REFRESHUSERLIST;

userquota   	return USERQUOTA;
domain      	return DOMAIN;
pass        	return PASS;
redirect    	{ BEGIN REDIRECT_STATE; return REDIRECT; }

redirect-fatal-error        return REDIRECT_FATAL_ERROR;
redirect-loading-database   return REDIRECT_LOADING_DATABASE;

redirect-https              return REDIRECT_HTTPS;
redirect-bumped-https       return REDIRECT_BUMPED_HTTPS;

minutely    	return MINUTELY;
hourly      	return HOURLY;
daily       	return DAILY;
weekly      	return WEEKLY;
continue    	return CONTINUE;
sporadic    	return SPORADIC;
date        	return DATE;

anonymous   	{ return ANONYMOUS; }
pidfile		{ return PIDFILE; }
logfile     	{ return LOGFILE; }
log         	{ return LOGFILE; }
{weekdays}  	{ yylval.string = ufdbStrdup(yytext); return WEEKDAY; }
within      	{ return WITHIN; }
outside     	{ return OUTSIDE; }
else        	{ return ELSE; }

ip          	{ return IPV4; }
ipv4          	{ return IPV4; }
iplist          { return IPV4LIST; }
ipv4list        { return IPV4LIST; }
ipv6            { return IPV6; }
ipv6list        { return IPV6LIST; }
{ipv4}        	{ yylval.string = ufdbStrdup(yytext); return IPV4ADDR; }
{ipv6addr}	{ yylval.string = ufdbStrdup(yytext); return IPV6ADDR; }
{ipv6net}	{ yylval.string = ufdbStrdup(yytext); return IPV6NET; }

[/]{d}+     	{ yylval.string = ufdbStrdup(yytext+1); return CIDR; }

[/]{ipv4}     	{ yylval.string = ufdbStrdup(yytext+1); return IPV4CLASS; }

{tval}      	{ yylval.string = ufdbStrdup(yytext); return TVAL; }
{dval}      	{ yylval.string = ufdbStrdup(yytext); return DVAL; }
{dvalcron}  	{ yylval.string = ufdbStrdup(yytext); return DVALCRON; }

\"[^"]*\"       { yylval.string = ufdbStrdup(yytext+1); *(strchr(yylval.string,'"')) = '\0'; return QSTRING; }

"{"          	{ return START_BRACKET; }
"}"          	{ return STOP_BRACKET;  }
","	     	{ return COMMA; }
"="	     	{ return EQUAL; }
"!"	  	{ return CHAR_EXCLAMATION; }
"i"	  	{ return CHAR_I; }
"-"	  	{ return CHAR_MINUS; }

[a-zA-Z][-a-zA-Z0-9_]*         			  { yylval.string = ufdbStrdup(yytext); return IDENTIFIER; }
[a-zA-Z\/\.][a-zA-Z0-9/_\-\.\/\:\%\+\?&]*         { yylval.string = ufdbStrdup(yytext); return WORD; }

s@(([^@}]|\\@|\\\})+)@(([^@}]|\\@|\\\})+)@[girR]* { yylval.string = ufdbStrdup(yytext); return SUBST; }

.         	{ ufdbLogError( "Unrecognized character on line %d: 0x%02x", lineno, ((int) yytext[0]) & 0xff ); }


<REDIRECT_STATE>\"[^"]*\"     	{ yylval.string = ufdbStrdup(yytext+1); *(strchr(yylval.string,'"')) = '\0'; BEGIN 0; return WORD; }
<REDIRECT_STATE>[0-9a-zA-Z\/\.][a-zA-Z0-9/_\-\.\/\:\%\+\?=&]*  { yylval.string = ufdbStrdup(yytext); BEGIN 0; return WORD; }
<REDIRECT_STATE>[ \t\f\r]+    ;
<REDIRECT_STATE>\n            	{ lineno++; }
<REDIRECT_STATE>.             	{ return yytext[0]; }

<<EOF>>	  			{ yyterminate(); }

%%


void UFDBdummyToSuppressCompilerWarnings( void )
{
   (void) unput( '\n' );

#ifdef __cplusplus
   (void) yyinput();
#else
   (void) input();
#endif
}

