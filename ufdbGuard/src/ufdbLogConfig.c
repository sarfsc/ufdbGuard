/*
 * ufdbLogConfig.c
 *
 * ufdbGuard is copyrighted (C) 2005-2017 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of the ufdbGuard daemon are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * write the configuration to the log file.
 *
 * RCS $Id: ufdbLogConfig.c,v 1.9 2018/03/18 19:02:40 root Exp root $
 */

#undef _FORTIFY_SOURCE

#include "ufdb.h"
#include "ufdblib.h"
#include "sg.h"

#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>


extern pthread_rwlock_t  TheDynamicSourcesLock;


char * ip2str( 
   char *          buffer, 
   unsigned int    i )
{
   sprintf( buffer, "%u.%u.%u.%u",  
   	    (i>>24)&0xff, (i>>16)&0xff, (i>>8)&0xff, (i)&0xff  );
   return buffer;
}


int mask2cidr( unsigned long mask )
{
   int cidr;

   cidr = 32;
   while (cidr > 0)
   {
      if (mask & 1)
         break;
      mask >>= 1;
      cidr--;
   }

   return cidr;
}


void UFDBlogIPv4( struct Ipv4 * ipv4 )
{
   int         cidr;
   char        ipbuffer[16];
   char        ipbuffer2[16];

   if (ipv4 == NULL)
      return;

   for (;  ipv4 != NULL;  ipv4 = ipv4->next)
   {
      switch (ipv4->type) 
      {
      case SG_IPTYPE_HOST:
         if (ipv4->net == 0)
            ufdbLogMessage( "   ipv4  0.0.0.0    #  WARNING: this is a no-op" );
         else
            ufdbLogMessage( "   ipv4  %u.%u.%u.%u", 
               (ipv4->net>>24)&0xff, (ipv4->net>>16)&0xff, (ipv4->net>>8)&0xff, (ipv4->net)&0xff );
         break;

      case SG_IPTYPE_RANGE:
	 (void) ip2str( ipbuffer, ipv4->net );
	 ufdbLogMessage( "   ipv4  %s - %s", ipbuffer, ip2str(ipbuffer2,ipv4->mask) );
         break;

      case SG_IPTYPE_CIDR:
         cidr = mask2cidr( ipv4->mask );
         if (cidr == 0)
            ufdbLogMessage( "   ipv4  %u.%u.%u.%u/0   #  WARNING: this is a no-op",
               (ipv4->net>>24)&0xff, (ipv4->net>>16)&0xff, (ipv4->net>>8)&0xff, (ipv4->net)&0xff );
         else
            ufdbLogMessage( "   ipv4  %u.%u.%u.%u/%u",    
               (ipv4->net>>24)&0xff, (ipv4->net>>16)&0xff, (ipv4->net>>8)&0xff, (ipv4->net)&0xff,
               cidr );
	 break;

      case SG_IPTYPE_CLASS:
	 ufdbLogMessage( "   ipv4  %u.%u.%u.%u/%s", 
	    	(ipv4->net>>24)&0xff, (ipv4->net>>16)&0xff, (ipv4->net>>8)&0xff, (ipv4->net)&0xff, 
		ip2str(ipbuffer2,ipv4->mask) );
         break;

      default:
	 break;
      }
   }
}


void UFDBlogIPv6( struct Ipv6 * ipv6 )
{
   char        newaddr[INET6_ADDRSTRLEN+1];

   if (ipv6 == NULL)
      return;

   for (;  ipv6 != NULL;  ipv6 = ipv6->next)
   {
      switch (ipv6->type) 
      {
      case SG_IPV6TYPE_HOST:
         newaddr[0] = '\0';
         inet_ntop( AF_INET6, (void *) &ipv6->ipv6, newaddr, INET6_ADDRSTRLEN );
         ufdbLogMessage( "   ipv6  %s", newaddr );
         break;

      case SG_IPV6TYPE_CIDR:
         newaddr[0] = '\0';
         inet_ntop( AF_INET6, (void *) &ipv6->ipv6, newaddr, INET6_ADDRSTRLEN );
         if (ipv6->cidr == 0)
            ufdbLogMessage( "   ipv6  %s/0   #  WARNING:  this is a no-op", newaddr );
         else
            ufdbLogMessage( "   ipv6  %s/%d", newaddr, ipv6->cidr );
	 break;

      case 0:
         /* there is a record with nulls at the end of the list :-( */
         break;

      default:
	 ufdbLogMessage( "   ipv6  unknown type %d", ipv6->type );
	 break;
      }
   }
}


static struct Source * findSource( char * name )
{
   struct Source *      src;

   for (src = UFDBgetSourceList();  src != NULL;  src = src->next)
      if (src->name != NULL  &&  strcmp( src->name, name ) == 0)
         return src;

   return NULL;
}


void UFDBlogACL( struct Acl * acl )
{
   int                  terminatorSeen;
   struct AclCategory * aclcat;
   struct Source *      src;
   char                 lbuf[4096];

   if (acl->within == UFDB_ACL_ELSE)
      ufdbLogMessage( "   else {" );
   else
   {
      lbuf[0] = '\0';
      if (acl->within == UFDB_ACL_WITHIN)
	 sprintf( lbuf, "within \"%s\" ", acl->time->name );
      else if (acl->within == UFDB_ACL_OUTSIDE)
	 sprintf( lbuf, "outside \"%s\" ", acl->time->name );
      ufdbLogMessage( "   \"%s\" %s{", acl->name, lbuf );
   }
   src = findSource( acl->name );

   if (acl->pass == NULL)
   {
      ufdbLogMessage( "      pass  " );
      ufdbLogMessage( "      This ACL is empty  *****" );
      if (src != NULL)
      {
         if (src->cont_search)
            ufdbLogMessage( "      # source \"%s\" has continued search", acl->name );
         else
            ufdbLogMessage( "      # empty ACLs without \"continue\" are blocked and the redirection rule of "
                            "the default ACL is used  *****" );
      }
   }
   else
   {
      terminatorSeen = 0;
      lbuf[0] = '\0';
      for (aclcat = acl->pass;  aclcat != NULL;  aclcat = aclcat->next)
      {
	 switch (aclcat->type)
	 {
	 case ACL_TYPE_TERMINATOR:
	    if (aclcat->access)
	       strcat( lbuf, " any" );
	    else
	       strcat( lbuf, " none" );
	    terminatorSeen = 1;
	    break;
	 case ACL_TYPE_DEFAULT:
	    strcat( lbuf, " " );
	    if (!aclcat->access)
	       strcat( lbuf, "!" );
	    strcat( lbuf, aclcat->name );
	    break;
	 case ACL_TYPE_INADDR:
	    if (!aclcat->access)
	       strcat( lbuf, " !in-addr" );
	    else
	       strcat( lbuf, " in-addr" );
	    break;
	 }
      }
      ufdbLogMessage( "      pass %s", lbuf );
      if (!terminatorSeen  &&  src != NULL  &&  src->cont_search == 0)
	 ufdbLogMessage( "      # there is no terminator; \"any\" is implied."  );

      if (UFDBglobalDebug)
      {
	 if (acl->hasTerminatorNone)
	    ufdbLogMessage( "      implicit-pass void    # pass has \"none\"" );
	 else
	 {
	    lbuf[0] = '\0';
	    for (aclcat = acl->implicitPass;  aclcat != NULL;  aclcat = aclcat->next)
	    {
	       strcat( lbuf, " " );
	       strcat( lbuf, aclcat->name );
	    }
	    if (UFDBglobalCheckedDB.table.nNextLevels > 0)
	       strcat( lbuf, " [checked]" );
            if (lbuf[0] == '\0')
               strcpy( lbuf, "any" );
	    ufdbLogMessage( "      implicit-pass %s", lbuf );
	 }
      }

      if (src != NULL  &&  src->cont_search)
         ufdbLogMessage( "      # source \"%s\" has continued search", acl->name );
   }

   if (acl->rewrite != NULL)
      ufdbLogMessage( "      rewrite \"%s\"", acl->rewrite->name );
   if (acl->redirect != NULL)
      ufdbLogMessage( "      redirect \"%s\"", acl->redirect );
   ufdbLogMessage( "   }" );
}


static const char * code2tunnelcheck( 
   int code )
{
   switch (code) 
   {
      case UFDB_API_HTTPS_CHECK_OFF:   	        return "off";
      case UFDB_API_HTTPS_CHECK_QUEUE_CHECKS:   return "queue-checks";
      case UFDB_API_HTTPS_CHECK_AGGRESSIVE:     return "aggressive";
      case UFDB_API_HTTPS_LOG_ONLY:             return "log-only";
   }
   return "unknown";
}


static char * t2hhmm( int t0 )
{
   static char hhmm[16];

   sprintf( hhmm, "%02d:%02d", t0 / 60, t0 % 60 );
   return hhmm;
}


static int SourceIsInAcl(
   struct Source *       src,
   struct Acl *          acl  )
{
   while (acl != NULL)
   {
      if (strcmp( acl->name, src->name ) == 0)
         return 1;
      acl = acl->next;
   }
   return 0;
}


void UFDBlogConfig( void )
{
   int                   numSubSourceDefs;
   int                   catsec;
   struct Category *     cat;
   struct Source *       src;
   struct Acl *          acl;
   struct ufdbTime *     t;
   struct TimeElement *  te;
   struct sgRewrite *    rew;
   struct ufdbRegExp *   re;
   char *                param;
   struct utsname        sysinfo;

   ufdbGetSysInfo( &sysinfo );
   ufdbLogMessage( "configuration on host %s:  -------------------------------------------", sysinfo.nodename );

   param = ufdbSettingGetValue( "dbhome" );
   if (param == NULL)
      ufdbLogMessage( "# default: dbhome %s", DEFAULT_DBHOME );
   else
      ufdbLogMessage( "dbhome \"%s\"", param );

   param = ufdbSettingGetValue( "logdir" );
   if (param == NULL)
      ufdbLogMessage( "# default: logdir \"%s\"", DEFAULT_LOGDIR );
   else
      ufdbLogMessage( "logdir \"%s\"", param );

   param = ufdbSettingGetValue( "pidfile" );
   if (param == NULL)
      ufdbLogMessage( "# default: pidfile \"%s\"", UFDBGUARDD_PID_FILE );
   else
      ufdbLogMessage( "pidfile \"%s\"", param );

   ufdbLogMessage( "port %d", UFDBglobalPortNum );
#if HAVE_UNIX_SOCKETS
   ufdbLogMessage( "# interface \"%s\"", UFDBglobalInterface );
#else
   ufdbLogMessage( "interface \"%s\"", UFDBglobalInterface );
#endif
   
   if (UFDBglobalEmailServer == NULL)
      ufdbLogMessage( "# default: email-server \"none\"" );
   else
      ufdbLogMessage( "email-server \"%s\"", UFDBglobalEmailServer );

   if (UFDBglobalMyHostname == NULL)
      ufdbLogMessage( "# default: my-hostname \"none\"" );
   else
      ufdbLogMessage( "my-hostname \"%s\"", UFDBglobalMyHostname );

   if (UFDBglobalAdminEmail == NULL)
      ufdbLogMessage( "# default: admin-email \"none\"" );
   else
      ufdbLogMessage( "admin-email \"%s\"", UFDBglobalAdminEmail );

   if (UFDBglobalSenderEmail == NULL)
      ufdbLogMessage( "# default: sender-email \"%s\"", UFDBglobalAdminEmail == NULL ? "none" : UFDBglobalAdminEmail );
   else
      ufdbLogMessage( "sender-email \"%s\"", UFDBglobalSenderEmail );

   if (UFDBglobalExternalStatusCommand == NULL)
      ufdbLogMessage( "# default: external-status-command \"none\"" );
   else
      ufdbLogMessage( "external-status-command \"%s\"", UFDBglobalExternalStatusCommand );

   ufdbLogMessage( "logpass %s", UFDBglobalLogPass ? "on" : "off" );
   ufdbLogMessage( "logblock %s", UFDBglobalLogBlock ? "on" : "off" );
   if (UFDBglobalShowURLdetails)
      ufdbLogMessage( "ufdb-show-url-details on" );
   if (UFDBglobalLogURLdetails)
      ufdbLogMessage( "ufdb-log-url-details on" );
   ufdbLogMessage( "logall %s", UFDBglobalLogAllRequests ? "on" : "off" );
   ufdbLogMessage( "upload-crash-reports %s", UFDBglobalUploadCrashReports ? "on" : "off" );
   ufdbLogMessage( "lookup-reverse-ip %s", UFDBglobalLookupReverseIP ? "on" : "off" );
   ufdbLogMessage( "use-ipv6-on-wan %s", UFDBglobalUseAlsoIPv6onWan ? "on" : "off" );

   /* TODO: remove ufdbLogMessage( "skip-safe-category %s", UFDBglobalSkipSafeCategory ? "on" : "off" ); */
   ufdbLogMessage( "num-worker-threads %d", UFDBglobalNworkers );
   ufdbLogMessage( "squid-version \"%s\"", UFDBglobalSquidVersion );
   if (UFDBglobalDebug)
      ufdbLogMessage( "# squid helper protocol %d", UFDBglobalSquidHelperProtocol );
   ufdbLogMessage( "squid-uses-active-bumping %s", UFDBglobalSquidUsesSSLbump ? "on" : "off" );
   ufdbLogMessage( "redirect-https \"%s\"  # NO bumping (may splice)", UFDBglobalRedirectHttps );
   ufdbLogMessage( "redirect-bumped-https \"%s\"  # active bumping", UFDBglobalRedirectBumpedHttps );

   ufdbLogMessage( "youtube-edufilter %s", UFDBglobalYoutubeEdufilter ? "on" : "off" );
   ufdbLogMessage( "youtube-edufilter-id \"%s\"", UFDBglobalYoutubeEdufilterID );
#if 0
   ufdbLogMessage( "allow-google-https-using-ip %s", UFDBglobalAllowGoogleHTTPSusingIP ? "on" : "off" );
#endif
   ufdbLogMessage( "deny-mode %s", UFDBglobalDenyMode ? "on" : "off" );
   ufdbLogMessage( "reuse-acl-names %s", UFDBglobalReuseAclNames ? "on" : "off" );

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "ufdb-debug-filter %d", UFDBglobalDebug );
   else
      ufdbLogMessage( "ufdb-debug-filter %s", UFDBglobalDebug ? "on" : "off" );
   ufdbLogMessage( "ufdb-expression-optimisation %s", UFDBglobalExpressionOptimisation ? "on" : "off" );
   ufdbLogMessage( "ufdb-expression-debug %s", UFDBglobalDebugRegexp ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-external-scripts %s", UFDBglobalDebugExternalScripts ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-skype-probes %s", UFDBglobalDebugSkype ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-gtalk-probes %s", UFDBglobalDebugGtalk ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-yahoomsg-probes %s", UFDBglobalDebugYahooMsg ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-aim-probes %s", UFDBglobalDebugAim ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-fb-chat-probes %s", UFDBglobalDebugFBchat ? "on" : "off" );
   ufdbLogMessage( "ufdb-debug-citrixonline-probes %s", UFDBglobalDebugCitrixOnline ? "on" : "off" );
   ufdbLogMessage( "refreshuserlist %d", UFDBglobalRefreshUserlistInterval );
   ufdbLogMessage( "refreshdomainlist %d", UFDBglobalRefreshDomainlistInterval );
   ufdbLogMessage( "max-logfile-size %lu", UFDBglobalMaxLogfileSize );
   if (UFDBglobalAnalyseUncategorisedURLs == 0)
      ufdbLogMessage( "analyse-uncategorised-urls off     # this is NOT recommended " );
   else
      ufdbLogMessage( "analyse-uncategorised-urls on" );
   ufdbLogMessage( "log-uncategorised-urls %s", UFDBglobalLogUncategorisedURLs ? "on" : "off" );
   if (!UFDBglobalUploadStats || UFDBglobalDebug)
      ufdbLogMessage( "upload-stats %s", UFDBglobalUploadStats ? "on" : "off" );
   ufdbLogMessage( "check-proxy-tunnels %s", code2tunnelcheck(UFDBglobalTunnelCheckMethod) );
   ufdbLogMessage( "safe-search %s", UFDBglobalSafeSearch ? "on" : "off" );
   if (UFDBglobalHttpdPort <= 0)
      ufdbLogMessage( "# no http-server defined" );
   else
      ufdbLogMessage( "http-server { port = %d, interface = %s, images = \"%s\" }", 
                      UFDBglobalHttpdPort, UFDBglobalHttpdInterface, UFDBglobalHttpdImagesDirectory );

   ufdbLogMessage( "url-lookup-delay-during-database-reload %s", 
                   UFDBglobalURLlookupDelayDBreload ? "on" : "off" );
   ufdbLogMessage( "url-lookup-result-during-database-reload %s", 
                   UFDBglobalURLlookupResultDBreload==UFDB_ALLOW ? "allow" : "deny" );

   ufdbLogMessage( "redirect-loading-database \"%s\"", UFDBglobalLoadingDatabaseRedirect );
   ufdbLogMessage( "url-lookup-result-when-fatal-error %s", 
                   UFDBglobalURLlookupResultFatalError==UFDB_ALLOW ? "allow" : "deny" );
   ufdbLogMessage( "redirect-fatal-error \"%s\"", UFDBglobalFatalErrorRedirect );

   ufdbLogMessage( "redirect-default-url \"%s\"", UFDBglobalRedirectURL );
   ufdbLogMessage( " - \"%s\"", UFDBglobalRedirectCategoryURL );
   ufdbLogMessage( " - \"%s\"", UFDBglobalRedirectClientCategoryURL );
   ufdbLogMessage( " - \"%s\"", UFDBglobalRedirectNoDefaultACLCategoryURL );
   ufdbLogMessage( " - \"%s\"", UFDBglobalRedirectNoDefaultACLClientCategoryURL );
   ufdbLogMessage( " - \"%s\"", UFDBglobalRedirectNoDefaultRedirectClientCategoryURL );

   ufdbLogMessage( " " );

   for (t = TimeList;  t != NULL;  t = t->next)
   {
      ufdbLogMessage( "time \"%s\" {", t->name );
      for (te = t->element;  te != NULL;  te = te->next)
      {
	 char line[128];
	 line[0] = '\0';

	 if (te->wday)
	 {
	    if ((te->wday & 0x7F) == 0x7F) 
	       strcat( line, "*   " );
	    else
	    {
	       if (te->wday & 0x01) strcat( line, "sun " );
	       if (te->wday & 0x02) strcat( line, "mon " );
	       if (te->wday & 0x04) strcat( line, "tue " );
	       if (te->wday & 0x08) strcat( line, "wed " );
	       if (te->wday & 0x10) strcat( line, "thu " );
	       if (te->wday & 0x20) strcat( line, "fri " );
	       if (te->wday & 0x40) strcat( line, "sat " );
	    }
	    strcat( line, "  " );
	    strcat( line, t2hhmm(te->from) );
	    strcat( line, " - " );
	    strcat( line, t2hhmm(te->to) );
	    ufdbLogMessage( "   weekly  %s", line );
	 }
	 else if (te->fromdate)
	 {
	    char from_str[24];
	    char to_str[24];

	    niso( te->fromdate, from_str );
	    niso( te->todate, to_str );
	    ufdbLogMessage( "   date    %s  -  %s", from_str, to_str );
	 }
	 else /* y m d */
	 {
	    char nb[8];

	    if (te->y < 0)   
	       strcat( line, "*" );
	    else
	       strcat( line, (sprintf(nb,"%d",te->y),nb) );

	    strcat( line, "-" );

	    if (te->m < 0)   
	       strcat( line, "*" );
	    else
	       strcat( line, (sprintf(nb,"%02d",te->m),nb) );

	    strcat( line, "-" );

	    if (te->d < 0)   
	       strcat( line, "*" );
	    else
	       strcat( line, (sprintf(nb,"%02d",te->d),nb) );

	    strcat( line, "   " );

	    strcat( line, t2hhmm(te->from) );
	    strcat( line, " - " );
	    strcat( line, t2hhmm(te->to) );

	    ufdbLogMessage( "   date    %s", line );
	 }
      }
      ufdbLogMessage( "}" );
   }
   ufdbLogMessage( " " );

   for (rew = Rewrite;  rew != NULL;  rew = rew->next)
   {
      ufdbLogMessage( "rewrite \"%s\" {", rew->name );
      for (re = rew->rewrite;  re != NULL;  re = re->next)
      {
         char * restatus;
         if (re->error)
            restatus = "RE has error: ";
         else
            restatus = "";
         ufdbLogMessage( "   %ss@ %s @ %s @     # spaces inserted for readability", 
                         restatus, re->pattern, re->substitute );
      }
      if (rew->time != NULL)
         ufdbLogMessage( "   time %s", rew->time->name == NULL ? "defined" : rew->time->name );
      ufdbLogMessage( "}" );
   }
   ufdbLogMessage( " " );

   for (cat = Cat;  cat != NULL;  cat = cat->next)
   {
      ufdbLogMessage( "category \"%s\" {", cat->name );
      if (cat->domainlist != NULL)
	 ufdbLogMessage( "   domainlist     \"%s\"", cat->domainlist );
      if (cat->execdomainlist != NULL)
	 ufdbLogMessage( "   execdomainlist \"%s\"", cat->execdomainlist );
      if (cat->expressionlist != NULL)
	 ufdbLogMessage( "   expressionlist \"%s\"", cat->expressionlist );
      catsec = (strncmp( cat->name, "security", 8 ) == 0);
      if (catsec)
      {
         if (cat->cse != NULL)
	    ufdbLogMessage( "   cacerts        \"%s\"", cat->cse );
         if (cat->cse2 != NULL)
	    ufdbLogMessage( "   cacerts-dir    \"%s\"", cat->cse2 );
      }
      else
      {
	 if (cat->options == 0  &&  cat->domainlist == NULL  &&  cat->execdomainlist == NULL  &&
	     cat->expressionlist == NULL  &&  cat->rewrite == NULL)
	 {
	    ufdbLogMessage( "   # category \"%s\" has no content definition  *****", cat->name );
	 }
      }
      if (cat->redirect != NULL)
	 ufdbLogMessage( "   redirect       \"%s\"", cat->redirect );
      if (cat->rewrite != NULL)
      {
         for (rew = cat->rewrite;  rew != NULL;  rew = rew->next)
         {
            ufdbLogMessage( "   rewrite        \"%s\"", rew->name );
         }
      }
      if (UFDBglobalSquidUsesSSLbump)
	 ufdbLogMessage( "   option         block-bumped-connect %s  # squid-uses-active-bumping is on", 
	                 cat->blockBumpedConnect ? "on" : "off" );
      else
      {
	 if (cat->blockBumpedConnect)
	 ufdbLogMessage( "   option         block-bumped-connect on  # WARNING: ignored since squid-uses-active-bumping is off" );
      }
      if (catsec  ||  cat->options != 0)
      {
	 if (catsec  ||  ((cat->options & ~(UFDB_OPT_SAFE_SEARCH|UFDB_OPT_YOUTUBE_EDUFILTER)) != 0))
	 {
	    ufdbLogMessage( "   option         enforce-https-with-hostname %s   %s", 
	                    cat->options & UFDB_OPT_HTTPS_WITH_HOSTNAME ? "on" : "off",
			    (cat->options & UFDB_OPT_HTTPS_WITH_HOSTNAME) && UFDBglobalSquidUsesSSLbump ? "WARNING: IGNORED because squid-uses-active-bumping is on  *****" : "" );
	    ufdbLogMessage( "   option         enforce-https-official-certificate %s", cat->options & UFDB_OPT_HTTPS_OFFICAL_CERTIFICATE ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-skype-over-https %s", cat->options & UFDB_OPT_SKYPE_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-gtalk-over-https %s", cat->options & UFDB_OPT_GTALK_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-yahoomsg-over-https %s", cat->options & UFDB_OPT_YAHOOMSG_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-aim-over-https %s", cat->options & UFDB_OPT_AIM_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-fb-chat-over-https %s", cat->options & UFDB_OPT_FBCHAT_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-citrixonline-over-https %s", cat->options & UFDB_OPT_CITRIXONLINE_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-anydesk-over-https %s", cat->options & UFDB_OPT_ANYDESK_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-teamviewer-over-https %s", cat->options & UFDB_OPT_TEAMVIEWER_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         allow-unknown-protocol-over-https %s", cat->options & UFDB_OPT_UNKNOWN_PROTOCOL_OVER_HTTPS ? "on" : "off" );
	    ufdbLogMessage( "   option         https-prohibit-insecure-sslv2 %s", cat->options & UFDB_OPT_PROHIBIT_INSECURE_SSLV2 ? "on" : "off" );
	    ufdbLogMessage( "   option         https-prohibit-insecure-sslv3 %s", cat->options & UFDB_OPT_PROHIBIT_INSECURE_SSLV3 ? "on" : "off" );
	 }
         if (cat->options & UFDB_OPT_SAFE_SEARCH)
	    ufdbLogMessage( "   option         safe-search on" );
         if (cat->options & UFDB_OPT_YOUTUBE_EDUFILTER)
	    ufdbLogMessage( "   option         youtube-edufilter on" );
      }
      ufdbLogMessage( "}" );
   }
   ufdbLogMessage( " " );

   for (src = (struct Source *) SourceList;  src != NULL;  src = src->next)
   {
      if (!src->active)
         ufdbLogMessage( "*****  WARNING: source '%s' is inactive  *****", src->name );
      if (!SourceIsInAcl( src, AclList ))
         ufdbLogMessage( "*****  WARNING: source '%s' is not in the acl.  *****\n"
	                 "*****  Note that when this source is matched, it will use the default acl rule  *****",
			 src->name );
      ufdbLogMessage( "source \"%s\" {", src->name );
      numSubSourceDefs = (src->ipv6 != NULL  ||  src->ipv4 != NULL) + 
                         (src->userDb != NULL) + (src->domainDb != NULL);
      if (numSubSourceDefs > 1)
      {
	 if (src->evaluationMethod == UFDB_EVAL_OR)
	    ufdbLogMessage( "   evaluate-or" );
	 else
	    ufdbLogMessage( "   evaluate-and" );
      }
      else
      {
	 if (src->evaluationMethod == UFDB_EVAL_OR)
	    ; /* ufdbLogMessage( "#  evaluate-or    # not relevant since only one user/ip/domain definition" ); */
	 else
	    ufdbLogMessage( "#  evaluate-and   # not relevant since only one user/ip/domain definition" );
      }
      UFDBlogIPv4( src->ipv4 );
      UFDBlogIPv6( src->ipv6 );
      UFDBmemDBprintUserDB( "domain", (struct UFDBmemDB *) src->domainDb );
      if (src->userDb != NULL)
      {
	 if (src->userDb->type == SGDBTYPE_EXECUSERLIST)
	 {
	    ufdbLogMessage( "   execuserlist \"%s\"", src->sarg0 );
	    if (UFDBglobalDebug > 1)
	    {
	       pthread_rwlock_rdlock( &TheDynamicSourcesLock );
	       UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) src->userDb->dbcp );
	       pthread_rwlock_unlock( &TheDynamicSourcesLock );
	    }
	 }
	 else
	    UFDBmemDBprintUserDB( "user", (struct UFDBmemDB *) src->userDb->dbcp );
      }
      if (src->time != NULL)
	 ufdbLogMessage( "   time %s", src->time->name == NULL ? "defined" : src->time->name );
      if (src->cont_search)
         ufdbLogMessage( "   continue" );
      ufdbLogMessage( "}" );
   }

   ufdbLogMessage( " " );

   ufdbLogMessage( "acl {" );
   for (acl = AclList;  acl != NULL;  acl = acl->next)
   {
      UFDBlogACL( acl );
   }
   ufdbLogMessage( "}" );

   ufdbLogMessage( " " );

   ufdbLogMessage( "database status: %s", ufdbDBstat2string(UFDBglobalDatabaseStatus) );
   ufdbLogMessage( "license status: %s", UFDBglobalLicenseStatus );
   ufdbLogMessage( "configuration status: %s", UFDBglobalFatalError ? "fatal error(s)" : "ok" );

   ufdbLogMessage( " " );
}

