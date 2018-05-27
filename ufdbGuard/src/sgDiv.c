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
 * RCS: $Id: sgDiv.c,v 1.120 2017/06/01 14:18:44 root Exp root $
 */


#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"
#include "ufdbdb.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>

extern int sig_other;           
extern char ** globalArgv;           
extern char ** globalEnvp;           
extern struct Acl * defaultAcl;



void sgHandlerSigHUP( int s )
{
   sig_hup = s;
}


void sgSignalHandler( int s )
{
   static int num = 0;

   if (sig_other == s)
   {
      num++;
      if (num >= 3)
	 abort();
   }
   else
      num = 0;

   sig_other = s;
}


void sgAlarm( int s )
{
}


#if 0
static void printSquidInfo( struct SquidInfo * s )
{
   ufdbLogMessage( "   protocol = %s", s->protocol     );
   ufdbLogMessage( "   port     = %d", s->port         );
   ufdbLogMessage( "   isip     = %d", s->isip         );
   ufdbLogMessage( "   isipv6   = %d", s->isipv6       );
   ufdbLogMessage( "   localnet = %d", s->islocalnet   );
   UFDBprintRevURL( s->revUrl );
   ufdbLogMessage( "   orig     = %s", s->orig         );
   ufdbLogMessage( "   url2displ= %s", s->url2display  );
   ufdbLogMessage( "   url2log  = %s", s->url2log      );
   ufdbLogMessage( "   url      = %s", s->url          );
   ufdbLogMessage( "   surl     = %s", s->surl         );
   ufdbLogMessage( "   domain   = %s", s->domain       );
   ufdbLogMessage( "   orgdomain= %s", s->orig_domain  );
   ufdbLogMessage( "   src      = %s", s->src          );
   ufdbLogMessage( "   srcDom   = %s", s->srcDomain    );
   ufdbLogMessage( "   ident    = %s", s->ident        );
   ufdbLogMessage( "   method   = %s", s->method       );
}
#endif


UFDB_GCC_HOT UFDB_GCC_INLINE 
static int domain_has_tld( char * p )
{
   while (*p != '\0')
   {
      if (*p == '/')
         return 0;
      if (*p == '.')
         return 1;
      p++;
   }
   return 0;
}


#if 0
UFDB_GCC_HOT UFDB_GCC_INLINE
static int squeeze_html_char( char * p, int * hex )
{
   int length;

   length = 0;
   *hex = 0;
   while (*p != '\0'  &&  isxdigit( (int) *p ))
   {
      int h;
      h = (*p <= '9') ? *p - '0' : *p - 'a' + 10;
      *hex = *hex * 16 + h;
      p++;
      length++;
   }

#if 0
   ufdbLogMessage( "   squeeze_html_char hex=%02x  length=%d  *p=%c", *hex, length, *p );
#endif

   if (*p == ';')
   {
      if (*hex == 0)
         return length;
      if (*hex < 0x20)
      {
         if (*hex != '\t'  &&  *hex != '\n'  &&  *hex != '\r'  &&  *hex != '\f')
	    *hex = ' ';
      }
      else if (*hex == 0x7f  ||  *hex >= 0xff)
      {
         *hex = ' ';
      }
      else if (*hex <= 'Z'  &&  *hex >= 'A')
	 *hex += 'a' - 'A';
      return length;
   }
   
   /* '&#xxx' without trailing ';' is not a valid HTML character */
   return -1;
}
#endif


UFDB_GCC_HOT UFDB_GCC_INLINE
static char * findDomainEnd( char * url )
{
   return url + strcspn( url, "/?&;#" );
}


UFDB_GCC_HOT 
static char * parse_value( char * key, char * v, char ** next )
{
   char * value_start;
   char   quote;

   while (*v == ' ')
      v++;
   if (*v == '"' || *v == '\'')
   {
      quote = *v;

      /* the value is quoted and may contain whitespace */
      v++;
      value_start = v;
      while (*v != quote  &&  *v != '\0')
         v++;
      if (*v == '\0')
      {
         ufdbLogError( "key '%s' has value '%s' without a terminating quote.   *****", key, value_start );
	 *next = NULL;
      }
      else
      {
         *v++ = '\0';
	 while (*v == ' '  ||  *v == '\t'  ||  *v == '\n')
	    v++;
	 if (*v != '\0')
	    *next = v;
	 else
	    *next = NULL;
      }
   }
   else
   {
      value_start = v;
      /* the value is unquoted and may not contain whitespace */
      while (*v != ' '  &&  *v != '\t'  &&  *v != '\n'  &&  *v != '\0')
         v++;
      if (*v == '\0' || *v == '\n')
      {
	 *v = '\0';
         *next = NULL;
      }
      else
      {
	 *v++ = '\0';
	 while (*v == ' '  ||  *v == '\t'  ||  *v == '\n')
	    v++;
	 if (*v != '\0')
	    *next = v;
	 else
	    *next = NULL;
      }
   }

   if (strcmp( value_start, "-" ) == 0  ||  strcmp( value_start, "none" ) == 0)
      *value_start = '\0';

#if 0
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "      key '%s' has value '%s'  next: '%s'", key, value_start, *next==NULL ? "NULL" : *next );
#endif

   return value_start;
}


UFDB_GCC_HOT 
static void copyReferer( char * referer, char * s, int maxsize )
{
   char * r;

   r = referer;
   while (*s != '\0'  &&  r < referer + maxsize - 1)
   {
      if (*s == '?' || *s == ';' || *s == '&' || *s == '%')
         break;
      *r++ = *s++;
   }
   *r = '\0';
}


/*
 * parse the line received from Squid -- \n must be stripped :
 * URL ip-address/fqdn ident method
 * The URL is normalised/rewritten.
 * Squid pre-2.6:      http://www.sex.com 10.1.1.44/- - GET
 * Squid 2.6 and 2.7:  http://www.sex.com 10.1.1.44/- - GET urlgroup
 * Squid 3.0 - 3.3:    http://www.sex.com 10.1.1.44/- - GET
 * Squid 3.4:          http://www.sex.com 10.1.1.44/- - GET [key=value foo=bar]
 *
 * If Squid is configured to use concurrency Squid sends a channel ID:
 *          channel-ID http://www.sex.com 10.1.1.44/- - GET [key=value foo=bar]
 *
 * If Squid sends the SNI (in ssl-bump mode) and the URL is an IP address, the URL is overwritten with the SNI.
 */
UFDB_GCC_HOT 
int parseLine( 
   UFDBthreadAdmin *  admin, 
   char *             line, 
   struct SquidInfo * s )
{
   char * p;
   char * d;
   char * a;
   char * e;
   char * o;
   char * field;
   char * upp;
   int    i; 
   int    maybeObfuscatedIPv4;
   int    insideParams;
   char * lineptr;
 
   s->channelid[0] = '\0';
   s->revUrl = NULL;
   s->url_has_ipv4 = 0;
   s->url_has_ipv6 = 0;
   s->source_isipv4 = 0;
   s->source_isipv6 = 0;
   s->islocalnet = 0;
   s->port = 80;
   s->matchedAny = 0;
   s->bumping = 0;
   s->BlockedBumpedConnectAllowed = 0;
   s->bump_mode[0] = '\0';
   s->sni[0] = '\0';
   s->cert_subject[0] = '\0';
   s->cert_issuer[0] = '\0';
   s->referer[0] = '\0';
   s->orig[0] = 
      s->url[0] = s->domain[0] = s->orig_domain[0] = s->srcIP[0] = s->ident[0] = s->ident[1] =
      s->protocol[0] = s->method[0] = s->urlgroup[0] = s->srcDomain[0] = s->surl[0] =  
      s->url2display[0] = s->url2log[0] = '\0';
 
#if 0
   ufdbLogMessage( "W%03d:   parseLine: %s\n", s->worker, line );
#endif
 
   field = strtok_r( line, "\t ", &lineptr );		/* TO-DO: use strcspn() */
   if (field == NULL)
   {
      ufdbLogError( "parseLine: input does not have multiple fields for %s", line );
      return 0;
   }
 
#if 0
   ufdbLogMessage( "W%03d:   field: %s\n", s->worker, field );
#endif
 
   /* detect optional channel-ID */
   if (field[0] <= '9'  &&  field[0] >= '0')
   {
      p = field;
      i = 0;
      while (i < 15  &&  *p >= '0'  &&  *p <= '9')
      {
         s->channelid[i] = *p;
         i++;
         p++;
      }
      s->channelid[i] = '\0';
      if (*p == '\0')
      {
         /* parsed the channel-ID, go to the next token (the URL) */
         field = strtok_r( NULL, " \t", &lineptr );
         if (field == NULL)
         {
            ufdbLogError( "W%03d: received a line with only a channel-ID", s->worker );
            return 0;
         }
      }
      else
      {
         /* oops: line does not start with a channel-ID; must be a URL with digits */
         s->channelid[0] = '\0';
      }
   }

#if 0
   ufdbLogMessage( "W%03d:   channel-ID: %s\n", s->worker, s->channelid );
#endif
 
   /* prevent loops with regular expression matching: */
   p = strstr( field, "/URLblocked.cgi" );
   if (p != NULL)
      *(p+15) = '\0';           /* TODO: do not zero-terminate but put spaces in the arguments */
 
   ufdbStrncpy( s->orig, field, sizeof(s->orig)-2 );

   if (UFDBglobalShowURLdetails)
   {
      ufdbStrncpy( s->url2display, field, sizeof(s->url2display)-1 );
      p = &( s->url2display[sizeof(s->url2display)-5-1] );
   }
   else
   {
      ufdbStrncpy( s->url2display, field, 300 );
      p = strchr( s->url2display, '?' );
      if (p == NULL)
      {
 	 p = strchr( s->url2display, ';' );
 	 if (p == NULL)
 	 {
 	    p = strstr( s->url2display, "%3F" );
 	    if (p == NULL)
 	       p = &(s->url2display[300]);
 	 }
      }
   }
   strcpy( p, "...." );
 
   if (UFDBglobalLogURLdetails)
   {
      ufdbStrncpy( s->url2log, field, sizeof(s->url2log)-1 );
      p = &( s->url2log[sizeof(s->url2log)-5-1] );
   }
   else
   {
      ufdbStrncpy( s->url2log, field, 300 );
      p = strchr( s->url2log, '?' );
      if (p == NULL)
      {
 	 p = strchr( s->url2log, ';' );
 	 if (p == NULL)
 	 {
 	    p = strstr( s->url2log, "%3F" );
 	    if (p == NULL)
 	       p = &(s->url2log[300]);
 	 }
      }
   }
   strcpy( p, "...." );
 
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: parseLine: channel-ID %s  url2display = %s", 
                      s->worker, s->channelid, s->url2display );
 
   /* convert domain+path (not parameters) of the URL to lowercase */
   for (p = field;  *p != '\0';  p++) 
   {
      if (*p == '?')
         break;
      if (*p <= 'Z'  &&  *p >= 'A')
         *p += 'a' - 'A';
   }
 
   p = strstr( field, "://" );
   if (p == NULL)			/* no protocol, defaults to http */
   {
      strcpy( s->protocol, "http" );
      p = field;
   }
   else
   {
      i = p - field;
      if (i > 15)					/* note that squiclient uses protocol 'cache_object' */
      {
         /* protocol IDs are not longer than 15 characters so we probably found a '://' inside the URL */
         o = strchr( field, '/' );
         if (o != NULL  &&  o < p)
         {
            /* yes, it is confirmed, '://' is after the first '/' */
  	    strcpy( s->protocol, "http" );
  	    p = field;
         }
         else
         {
  	    i = 15;				/* found a large protocol name. truncate it */
  	    memcpy( s->protocol, field, i );
  	    s->protocol[i] = '\0';
  	    p += 3;				/* skip '://' */
  	    field = p;
         }
      }
      else
      {
         memcpy( s->protocol, field, i );
         s->protocol[i] = '\0';
         if (i == 5  &&  strcmp( s->protocol, "https" ) == 0)
	 {
	    increment_UFDB_API_num_https();
            s->port = 443;
	 }
         p += 3;				/* skip '://' */
         field = p;
      }
   }
 
#if 0
    ufdbLogMessage( "W%03d: parseLine: proto = %s", s->worker, s->protocol );
#endif
 
   /* skip the initial optional username:password@ part */
   /* anticipate:   user:pw@www.domain.com		 */
   /*               user:pw@www.domain.com/path	 */
   /*               user:pw@www.domain.com?a=0&...      */
   /*               www.domain.com			 */
   /*               www.domain.com/path		 */
   /*               www.domain.com?a=0&...		 */
   i = 0;
   while (*p != '\0')
   {
      if (*p == '/'  ||  *p == '?')
         break;
      if (*p == '@')
      {
         p++;
         i = 1;
         break;
      }
      p++;
   }
   if (i == 0)
      p = field;
 
   /* Do some character decoding: translate '//' '/./' '&#xx' and '%xx'. 
    * Only convert %HH after a '?' 
    */
   insideParams = 0;
   upp = s->url;
   while (*p != '\0') 
   {
      if (*p == '?')
      {
         insideParams = 1;
         *upp++ = *p++;
         continue;
      }
      else if (*p == ':'  &&  *(p+1) == '/'  &&  *(p+2) == '/')     /* do not replace :// by :/  */
      {
         *upp++ = *p++;
         *upp++ = *p++;
         *upp++ = *p++;
         continue;
      }
      else if (*p == '%')			/* convert '%HH' into character */
      {
         if (isxdigit((int) *(p+1))  &&  isxdigit( (int) *(p+2) ))
         {
   	    int    hex;
   	    char   h1, h2;
   	    h1 = *(p+1);
   	    h2 = *(p+2);
   	    if (h1 <= '9')
               hex = (h1 - '0') * 16;
            else if (h1 <= 'F')
               hex = (h1 - 'A' + 10) * 16;
            else
               hex = (h1 - 'a' + 10) * 16;
   	    if (h2 <= '9')
               hex += h2 - '0';
            else if (h2 <= 'F')
               hex += h2 - 'A' + 10;
            else
               hex += h2 - 'a' + 10;
   	    /* be careful with conversion of control characters */
   	    if (hex == 0)
   	    {
   	       p += 3;
   	       continue;
   	    }
   	    else if (hex < 0x20)
   	    {
   	       if (hex != '\t'  &&  hex != '\r'  &&  hex != '\n'  &&  hex != '\f')
   	       {
   	          hex = ' ';
   	       }
   	    }
   	    else
   	    { 
   	       if (hex == 0x7f)
   	       {
   	          hex = ' ';
   	       }
   	       else if (!insideParams  &&  hex <= 'Z'  &&  hex >= 'A')
   	       {
   	          hex += 'a' - 'A';
   	       }
   	    }
   	    *upp++ = hex;
   	    p += 2;
         }
         else
         { 						/* an erroneous hex code, we ignore it */
            *upp++ = *p;				/* just copy one character */
         }
      }
      else
      {
         while (*p == '/')
         {
            if (*(p+1) == '/')							/* substitute // by / */
  	       p++;
  	    else if (*(p+1) == '.'  &&  *(p+2) == '/')				/* substitute /./ by / */
  	       p += 2;
  	    else if (*(p+1) == '.'  &&  *(p+2) == '.'  &&  *(p+3) == '/')    	/* substitute /xxxx/../ by / */
  	    {
  	       /* try to find the previous directory... */
  	       char * prevdir;
  	       prevdir = upp - 1;
  	       while (*prevdir != '/'  &&  prevdir > s->url)
  	          prevdir--;
  	       if (prevdir > s->url)
  	       {
  	          upp = prevdir;
  	          p += 3;
  	       }
  	       else
  	          break;
  	    }
  	    else
  	       break;
         }
         *upp++ = *p;
      }
      p++;
   }
   *upp++ = *p;
   *upp = '\0';
 
#if 0
   ufdbLogMessage( "W%03d: parseLine: s->url = %s", s->worker, s->url );
#endif
 
   p = s->url;
 
   s->url_has_ipv6 = 0;
   if (*p == '[')			/* IPv6 address */
   {
      char * end;
      char * oldBracket;
 
      s->url_has_ipv6 = 1;
      s->orig_domain[0] = '\0';
      oldBracket = strchr( p, ']' );
      if (oldBracket != NULL)
         *oldBracket = '\0';
      end = UFDBparseIPv6address( p, s->orig_domain );
      if (oldBracket != NULL)
         *oldBracket = ']';
      if (end != NULL)
      {
         UFDBupdateURLwithNormalisedDomain( p, s->orig_domain );
 	 /* uh-oh: the normalised domain is usually smaller and our pointers have moved */
      }
      if (end == NULL)		/* invalid IPv6 address */
      {
         end = oldBracket;
         if (end == NULL)
 	 {
 	    end = strchr( p, '/' );
 	    if (end == NULL)
 	    {
 	       /* I give up */
 	       d = e = a = NULL;
 	       goto parse2;
 	    }
         }
      }
      d = strchr( end, '/' );
      e = d;
      a = strchr( end, ':' ); 		/* find port */
   }
   else					/* not IPv6 */
   {
      d = findDomainEnd( p ); 		/* find domain end */
      e = d;
      a = strchr( p, ':' ); 		/* find port */
   }
 
parse2:
   if (a != NULL  &&  (a < d || d == NULL))
   {
      i = 0;
      o = a + 1;
      while (*o <= '9'  &&  *o >= '0')
      {
         i = i * 10 + (*o - '0');
         o++;
      }
      if (i == 443  &&  s->port != 443)
      {
	 increment_UFDB_API_num_https();
	 strcpy( s->protocol, "https" );
      }
      s->port = i;
      memmove( a, o, strlen(o)+1 );	/* remove the port number from the URL */
      e = a;
   }
 
   if (!s->url_has_ipv6)
   {
      if (e == NULL)
 	 strcpy( s->orig_domain, p );
      else
      {
 	 i = e - p + 1;
 	 ufdbStrncpy( s->orig_domain, p, i );
      }
   }
   
   /* strip prefix www[0-9]{0,2}   */
   o = p;
   if (p[0] == 'w' && p[1] == 'w' && p[2] == 'w')		/* match "www" */
   {
      p += 3;
 
      if (p[0] <= '9'  &&  p[0] >= '0')              		/* match wwwN. */
         p++;
      if (p[0] <= '9'  &&  p[0] >= '0')              		/* match wwwNN. */
         p++;
     								/* skip www_xxx.com and www99.com */
      if (*p != '.'  ||  !domain_has_tld(p+1))
         p = o;                                                 /* not a hostname */
      else
         p++;
   }
 
   if (e == NULL)
   {
      ufdbStrncpy( s->domain, p, sizeof(s->domain)-1 );	
 
      /* The original URL will have &foo=bar appended when safe-search is ON
       * and therefore we must make sure that URLs that only have the domain name,
       * have a / before the &foo=bar, i.e the result becomes hotbot.com/&adf=on 
       */
      strcat( s->orig, "/" );
   }
   else 
   {
      if (e - p >= sizeof(s->domain)-1)
      {
         ufdbStrncpy( s->domain, p, sizeof(s->domain)-1 );
      }
      else
      {
         memcpy( s->domain, p, e - p );	
         *(s->domain + (e - p)) = '\0';
      }
   }
 
   ufdbStrncpy( s->surl, p, sizeof(s->surl) );
 
#if 0
    ufdbLogMessage( "W%03d: parseLine: domain = %s  s->surl = %s", s->worker, s->domain, s->surl );
#endif
 
   /* Determine if the domainname is an IPv4 address (s->url_has_ipv4 == 1 iff IPv4 address).
    * At the same time scan for IP address obfuscations, e.g. 066.1.2.3 or 98.0x3f.00010.5
    */
   /* TODO: use new code for IPv4 detection and obfuscation removal ***** */
   if (!s->url_has_ipv6)
   {
      maybeObfuscatedIPv4 = 0;
      o = s->domain;
      while (1)
      {
         if (*o == '\0')
         {
            s->url_has_ipv4 = 1;
            break;
         }
         if (*o == '0')
         {
            o++;
            if (*o == 'x')
            {
               o++;
               while (*o != '\0'  &&  isxdigit(*o))
                  o++;
               maybeObfuscatedIPv4 = 1;
            }
            else if (*o >= '0' && *o <= '7')
            {
               o++;
               while (*o != '\0'  &&  (*o >= '0' && *o <= '7'))
                  o++;
               maybeObfuscatedIPv4 = 1;
            }
            if (*o != '\0'  &&  *o != '.')
               break;
            if (*o == '.')
               o++;
         }
         else if (*o >= '1'  &&  *o <= '9')
         {
            o++;
            while (*o != '\0'  &&  (*o >= '0' && *o <= '9'))
               o++;
            if (*o == '.')
               o++;
         }
         else
         {
            /* not an (obfuscated) IP address) */
            break;
         }
      }
   }
 
   if (s->url_has_ipv6)
   {
      if ((strncmp( s->surl, "[::1]", 5 ) == 0) ||
          (strncmp( s->surl, "[fc", 3 ) == 0)   ||
          (strncmp( s->surl, "[fe80:", 6 ) == 0))
      {
 	 s->islocalnet = 1;
      }
   }
   else if (s->url_has_ipv4)
   {
      if (maybeObfuscatedIPv4)
      {
	 if (UFDBglobalDebug > 1)
 	    ufdbLogMessage( "W%03d: URL has obfuscated IPv4 address: %s", s->worker, s->surl );
         /* rewrite s->surl to remove the obfuscation */
 	 d = p = s->surl;
 	 UFDBnormaliseIPv4( s->domain );
 	 for (p = s->domain;  *p != '\0';  )
 	    *d++ = *p++;

 	 p = s->surl;
 	 while (*p != '\0'  &&  (isxdigit(*p) || *p == '.' || *p == 'x'))
 	    p++;

	 /* copy the URI */
 	 while (*p != '\0')
 	 {
 	    *d = *p;
 	    d++;
 	    p++;
         }
         *d = '\0';
 	 if (UFDBglobalDebug)
 	    ufdbLogMessage( "W%03d: URL with obfuscated IPv4 address is rewritten to %s (was: %s)", 
                            s->worker, s->surl, s->orig );
      }
      if ((strncmp( s->surl, "10.", 3 )==0) ||
 	  (strncmp( s->surl, "0.", 2 )==0) ||
 	  (strncmp( s->surl, "127.", 4 )==0) ||
 	  (strncmp( s->surl, "169.254.", 8 )==0) ||
 	  (strncmp( s->surl, "192.168.", 8 )==0) )
      {
 	 s->islocalnet = 1;
      }
      else if ((strncmp( s->surl, "100.", 4 )==0) ||
 	       (strncmp( s->surl, "172.", 4 )==0))
      {
 	 int o1, o2;
 	 sscanf( s->surl, "%d.%d.", &o1, &o2 );
 	 if (o1 == 100  &&  (o2 >= 64  &&  o2 <= 127))
 	    s->islocalnet = 1;
 	 else if (o1 == 172  &&  (o2 >= 16  &&  o2 <= 31))
 	    s->islocalnet = 1;
      }
   }
 
   if ((p = strtok_r(NULL," \t\n",&lineptr)) != NULL)
   {
      o = strchr( p, '/' );					/* parse IP/FQDN */
      if (o != NULL) 
      {
         if ((o-p) > sizeof(s->srcIP)-1)
         {
            strncpy( s->srcIP, p, sizeof(s->srcIP)-1 );
            s->srcIP[sizeof(s->srcIP)-1] = '\0';
         }
         else
         {
            memcpy( s->srcIP, p, o-p );
            s->srcIP[o-p] = '\0';
         }
         if ( ! (*(o+1) == '-'  &&  *(o+2) == '\0'))
         {
  	    ufdbStrncpy( s->srcDomain, o+1, sizeof(s->srcDomain)-1 );
         }
         if (inet_pton( AF_INET6, s->srcIP, (void *) &s->source_ipv6 ) > 0)
            s->source_isipv6 = 1;
         else if (sgConvDot( s->srcIP, &s->source_ipv4 ) != NULL)
            s->source_isipv4 = 1;
      }
      else
      {
         strcpy( s->srcIP, p );
      }
  
      if ((p = strtok_r(NULL," \t\n",&lineptr)) != NULL)	/* parse ident */
      {
         if (p[0] != '-'  ||  p[1] != '\0')
         {
  	    ufdbStrncpy( s->ident, p, sizeof(s->ident) );
  	    for (p = s->ident;  *p != '\0';  p++) 		/* convert ident to lowercase chars */
  	    {
  	       if (*p <= 'Z'  &&  *p >= 'A')
  	          *p += 'a' - 'A';
  	       else if (*p == '@'  &&  UFDBglobalStripDomainFromUsername)
  	       {
  	          *p = '\0';
  	          break;
  	       }
            }
         }
  
         if ((p = strtok_r(NULL," \t\n",&lineptr)) != NULL)	/* parse method, e.g. GET/HEAD/POST/OPTIONS/PROPFIND/CONNECT */
         {
  	    ufdbStrncpy( s->method, p, sizeof(s->method) );
  	    if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL3)
  	    {
  	       strcpy( s->urlgroup, "#" );
  
  	       /* Squid 3.4 and higher can have a variable number of key-extras (defined with url_rewrite_extras)
  	        * The keys bump_mode, cert_subject, cert_issuer and sni are saved.
  	        * The keys myip, myport and all other keys are skipped.
  	        * See also: http://wiki.squid-cache.org/Features/Redirectors
  	        */
     
  	       /* strtok_r cannot be used to parse the key-extras since they have quoted values and may 
                * have spaces inside the values.
  	        * WARNING: The man page of strtok_r does not define the value of lineptr but we assume 
                * it points to the next token.
  	        */
  	       p = lineptr;
  	       if (UFDBglobalDebug > 2)
  	          ufdbLogMessage( "W%03d: parsing optional keys: #%s#", s->worker, p );
  	       while (p != NULL)
  	       {
  	          char * v;
  	          char * value;
  	          char * next;
     
  	          v = strchr( p, '=' );
  	          if (v == NULL)
  	             break;
  	          *v++ = '\0';
  	          next = NULL;
  	          value = parse_value( p, v, &next );
  	          if (value == NULL)
  	             break;
  	          else if (strcmp( p, "username" ) == 0)
		  {
		     if (value[0] != '-'  ||  value[1] != '\0')
		     {
		        char * id;
			ufdbStrncpy( s->ident, value, sizeof(s->ident) );
			for (id = s->ident;  *id != '\0';  id++) 	/* convert ident to lowercase chars */
			{
			   if (*id <= 'Z'  &&  *id >= 'A')
			      *id += 'a' - 'A';
			   else if (*id == '@'  &&  UFDBglobalStripDomainFromUsername)
			   {
			      *id = '\0';
			      break;
			   }
			}
		     }
		  }
  	          else if (strcmp( p, "bump_mode" ) == 0)
  		     { ufdbStrncpy( s->bump_mode, value, sizeof(s->bump_mode) ); }
  	          else if (strcmp( p, "sni" ) == 0)
  		     { ufdbStrncpy( s->sni, value, sizeof(s->sni) ); }
  	          else if (strcmp( p, "cert_subject" ) == 0)
  		     { ufdbStrncpy( s->cert_subject, value, sizeof(s->cert_subject) ); }
  	          else if (strcmp( p, "cert_issuer" ) == 0)
  		     { ufdbStrncpy( s->cert_issuer, value, sizeof(s->cert_issuer) ); }
  	          else if (strcmp( p, "referer" ) == 0)
  		     { copyReferer( s->referer, value, sizeof(s->referer) ); }
  	          else if (UFDBglobalDebug)
		  {
		     if (strcmp( p, "myip" ) != 0  &&  strcmp( p, "myport" ) != 0  &&  strcmp( p, "proxy" ) != 0)
			ufdbLogMessage( "IGNORED: key '%s' value '%s'.   "
                                        "See the UfdbGuard Reference Manual about using Squid parameter url_rewrite_extras",
					p, value );
		  }
  	          p = next;
  	       }
  	       s->bumping = *(s->bump_mode) || *(s->sni) || *(s->cert_subject) || *(s->cert_issuer);
  	       if (UFDBglobalDebug > 1)
  	          ufdbLogMessage( "W%03d: parsed keys: '%s' '%s' '%s' '%s' '%s'  bumping %d", 
  	                          s->worker, s->bump_mode, s->sni, s->cert_subject, 
                                  s->cert_subject, s->referer, s->bumping );
  	    }
  	    else if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL1)
  	    {
  	       /* Squid 2.6 and 2.7 use here a urlgroup, but Squid 3.x does not! */
  	       if ((p = strtok_r(NULL," \t\n",&lineptr)) != NULL)
  	          strcpy( s->urlgroup, p );
  	       else
  	          strcpy( s->urlgroup, "#" );
  	    }
         }
         else if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL1)
  	    strcpy( s->urlgroup, "#" );
      }
   }
   else if (UFDBglobalSquidHelperProtocol == UFDB_SQUID_HELPER_PROTOCOL1)
   {
      strcpy( s->urlgroup, "#" );
   }
 
   if (s->domain[0] == '\0')
   {
      ufdbLogError( "W%03d: cannot find domainname parsing %s", s->worker, s->orig );
      return 0;
   }
   if (s->method[0] == '\0')
   {
      ufdbLogError( "W%03d: cannot find request method for %s", s->worker, s->orig );
      /* we must assume a buffer overflow in an attempt to circumvent the filter */
      strcpy( s->method, "GET" );
   }

   if (s->sni[0] != '\0')
   {
      if (s->url_has_ipv4)
      {
         for (o = s->sni;  *o != '\0';  o++)
         {
            if ((*o > '9' || *o < '0')  &&  *o != '.')
            {
               s->url_has_ipv4 = 0;
               strcpy( s->url, s->sni );
               strcpy( s->surl, s->sni );
               strcat( s->url2display, "/SNI:" );
               strcat( s->url2display, s->sni );
               break;
            }
         }
      }
      else if (s->url_has_ipv6)
      {
         if (s->sni[0] != '[')
         {
            s->url_has_ipv6 = 0;
            strcpy( s->url, s->sni );
            strcpy( s->surl, s->sni );
            strcat( s->url2display, "/SNI:" );
            strcat( s->url2display, s->sni );
         }
      }
   }
 
   /* s->revUrl is pointing to malloced memory and must be freed at the end of the main loop ! */
   s->revUrl = UFDBgenRevURL( admin, (unsigned char *) s->surl );
 
#if 0
   printSquidInfo( s );
#endif
 
   return 1;
}


/*
 * checks the vality of an IPv4 dotted address. 
 */
UFDB_GCC_HOT
unsigned int * sgConvDot( 
   const char *   dot, 
   unsigned int * ipaddr )
{
   const char *   t;
   unsigned int   octet;
   int            shift;

   *ipaddr = 0;
   shift = 24;
   t = dot;
   while (*t != '\0')
   {
      if (*t > '9'  ||  *t < '0')
         return NULL;

      octet = 0;
      while (*t >= '0'  &&  *t <= '9') 
      {
	 octet = octet*10 + (*t - '0');
         t++;
      }

      if (*t == '.') 
         t++;
      else if (*t != '\0') 
         return NULL;

      if (shift < 0)
         return NULL;

      if (octet > 255)
         return NULL;

      *ipaddr |= octet << shift;
      shift -= 8;
   }

   return ipaddr;
}


UFDB_GCC_HOT
int sgValidateIPv6(
   const char *      str, 
   struct in6_addr * ipv6addr )
{
   int retval;

   retval = inet_pton( AF_INET6, str, (void *) ipv6addr );
   if (retval > 0)
      return 1;

   return 0;
}


UFDB_GCC_HOT
int ufdbIPv6hostInNet( struct in6_addr * host, struct in6_addr * net, int cidr )
{
   int     i;
   uint8_t mask;

   i = 0;
   while (cidr >= 8)
   {
      if (host->s6_addr[i] != net->s6_addr[i])
         return 0;
      cidr -= 8;
      i++;
   }

   if (cidr == 0)
      return 1;

   mask = ((uint8_t) 0xFF) << (8 - cidr);
   if ((host->s6_addr[i] & mask) == (net->s6_addr[i] & mask))
      return 1;

   return 0;
}


/*
 *  Regex functions
 */

char * ufdbRegExpSubst( struct ufdbRegExp * regexp, char * pattern, char * newstring )
{
  struct ufdbRegExp * re;
  regmatch_t        pm;
  char *            result = NULL;
  char *            p;
  int               substlen;

  newstring[0] = '\0';
  for (re = regexp;  re != NULL;  re = re->next)
  {
    if (UFDBregexec(re->compiled[0], pattern, 1, &pm, 0) != 0) 
    {
      result = NULL;
    }
    else
    {
      substlen = strlen( re->substitute );
      if (re->httpcode != NULL)
	strcpy( newstring, re->httpcode );
      else
	*newstring = '\0';
      p = newstring;
      do {
	if ((p - newstring) + pm.rm_so >= UFDB_MAX_URL_LENGTH)
	  break;
	p = strncat( newstring, pattern, pm.rm_so );
	if ((p - newstring) + substlen >= UFDB_MAX_URL_LENGTH)
	  break;
	p = strcat( newstring, re->substitute );	
	pattern = pattern + pm.rm_eo;
      } while (UFDBregexec(re->compiled[0], pattern, 1, &pm, REG_NOTBOL) == 0 && re->global);

      if ((p - newstring) + strlen(pattern) <= UFDB_MAX_URL_LENGTH)
	p = strcat( newstring, pattern );
      result = newstring;
      break;
    }
  }

  return result;
}


char * sgRewriteExpression( struct sgRewrite * rewrite, char * subst, char * newstring )
{
   char * result;

   result = ufdbRegExpSubst( rewrite->rewrite, subst, newstring );
   return result;
}


static void encodeURIvalue( 
   char * d,
   char * t  )
{
   size_t len;
   char   tbuf[4096];

   len = strlen( t );
   if (len >= sizeof(tbuf)-1)
   {
      memcpy( tbuf, t, sizeof(tbuf) );
      tbuf[sizeof(tbuf)-5] = '.';
      tbuf[sizeof(tbuf)-4] = '.';
      tbuf[sizeof(tbuf)-3] = '.';
      tbuf[sizeof(tbuf)-2] = '.';
      tbuf[sizeof(tbuf)-1] = '\0';
      t = tbuf;
   }

   while (*t != '\0')
   {
      /* See also RFC 3986 */
      /* TODO: deal with UTF8 characters in URL */
      if (strchr( ":/?#[]@"  "!$&'()*+,;="  " \"%\\", *t ) != NULL)
      {
         char h;

         *d++ = '%';
         h = *t / 16;
         *d++ = (h >= 10) ? ('A' - 10 + h) : ('0' + h);
         h = *t % 16;
         *d++ = (h >= 10) ? ('A' - 10 + h) : ('0' + h);
      }
      else
         *d++ = *t;
      t++;
   }
   *d = '\0';
}


UFDB_GCC_HOT 
static void allCategories( 
   char *                buf, 
   struct SquidInfo *    si,
   struct Acl *          acl,
   struct AclCategory *  ap  )
{
   int                   inImplicit;
   int                   emitComma;
   int                   decision;
   int                   access;
   struct AclCategory *  ac;
   struct UFDBmemTable * mt;

   inImplicit = 0;
   emitComma = 0;
   ac = acl->pass;
   while (ac != NULL)
   {
      if (ac->name != NULL  &&  ac->type != ACL_TYPE_TERMINATOR)
      {
	 decision = 0;
	 access = 1;
	 if (ac->cat != NULL  &&  ac->cat->domainlistDb != NULL)
	 {
	    mt = ac->cat->domainlistDb->dbcp;
	    if (UFDBlookupRevUrl( &(mt->table.nextLevel[0]), si->revUrl ))
	    {
	       access = ac->access;
	       decision = 1;
	    }
	 }
	 if (!decision  &&  ac->cat != NULL  && ac->cat->regExp != NULL)
	 {
	    if (ufdbRegExpMatch( ac->cat->regExp, si->surl ))
	    {
	       access = ac->access;
	       decision = 1;
	    }
	 }

	 if (decision)
	 {
	    if (emitComma)
	    {
	       *buf++ = ',';
	       *buf = '\0';
	    }
	    if (!access)
	    {
	       *buf++ = '!';
	       *buf = '\0';
	    }
	    strcpy( buf, ac->name );
	    buf += strlen( ac->name );
	    emitComma = 1;
	 }
      }

      if (ac->next == NULL  &&  !inImplicit)
      {
	 inImplicit = 1;
	 ac = acl->implicitPass;
      }
      else
	 ac = ac->next;
   }
}


/*
 *  Expand a redirection URL string: fill in the %i, %n etc.
 */
UFDB_GCC_HOT 
char * sgParseRedirect( 
  char *               redirect,
  struct SquidInfo *   req,
  struct Acl *         acl, 
  struct AclCategory * aclpass,
  char *               buf,
  int                  force_302 )
{
  char * p;
  char * q;
  char * t;
  char * d;

  if (aclpass == NULL)
    aclpass = defaultAcl->pass;

  q = p = redirect;
  buf[0] = '\0';

  if (force_302  &&  strncmp(redirect,"302:",4) != 0)
  {
     strcpy( buf, "302:" );
  }

  while ((p = strchr(p,'%')) != NULL)
  {
    strncat( buf, q,  p - q );

    /* TODO: insert "version=UFDB_VERSION" whenever the first parameter is encountered */

    p++;
    switch (*p)
    {
    case 'a': 			        /* Source Address */
      encodeURIvalue( buf + strlen(buf), req->srcIP );
      break;

    case 'A':
       t = ufdbSettingGetValue( "administrator" );
       if (t == NULL)
          t = (char *) "your system administrator or helpdesk";
       encodeURIvalue( buf + strlen(buf), t );
       break;

    case 'C':                           /* all categories */
       t = buf + strlen( buf );
       allCategories( t, req, acl, aclpass );
       break;

    case 'i': 			        /* Source User Ident */
      if ((req->ident[0] == '\0')  ||  (req->ident[0] == '-' && req->ident[1] == '\0'))
      {
	strcat( buf, "unknown" );
      }
      else
      {
	strcat( buf, req->ident );
      }
      break;

    case 'q': 				/* userquota info */
      strcat( buf, "noquota" );
      break;

    case 'n': 		                /* Source Domain Name */
      if ((req->srcDomain[0] == '\0')  ||  (req->srcDomain[0] == '-' && req->srcDomain[1] == '\0'))
      {
	 strcat( buf, "unknown" );
      }
      else
      {
         encodeURIvalue( buf + strlen(buf), req->srcDomain );
      }
      break;

    case 'p': 		                /* The url path */
      if ((t = strstr(req->orig,"://")) != NULL)
      {
        t += 3;
        if ((t = strchr(t,'/')) != NULL)
	{
	   t++;
           encodeURIvalue( buf + strlen(buf), t );
        }
      }
      break;

    case 'f': 		                /* The url file */
      if ((t = strrchr(req->orig,'/')) != NULL)
      {
         t++;
	 encodeURIvalue( buf + strlen(buf), t );
      }
      break;

    case 's': 		                /* Source Class Matched */
      if (acl->source == NULL  ||  acl->source->name == NULL)
	strcat( buf, "default" );
      else
      {
	if (acl->within == UFDB_ACL_NONE)
	   encodeURIvalue( buf + strlen(buf), acl->name );
	else
	{
	   struct Acl * acl0;

	   acl0 = acl;
	   encodeURIvalue( buf + strlen(buf), acl0->name );
	   if (acl0->within == UFDB_ACL_ELSE)
	   {
	      strcat( buf, "-not" );  
	      acl0 = sgAclFindName( acl0->name );
	      if (acl0 == NULL)
	      {
	         strcat( buf, "-ERROR-find-acl" );
		 acl0 = acl;
	      }
	   }
	   switch (acl0->within)
	   {
	   case UFDB_ACL_WITHIN:     
	      strcat( buf, "-within-" );  
	      break;
	   case UFDB_ACL_OUTSIDE:     
	      strcat( buf, "-outside-" );  
	      break;
	   }
	   encodeURIvalue( buf + strlen(buf), acl0->time->name );
	}
      }
      break;

    case 't': 		                /* Category (Target Class) Matched */
      if (aclpass == NULL)
	 strcat( buf, "unknown" );
      else if (aclpass->name == NULL)
	 if (aclpass->type == ACL_TYPE_INADDR)
	    strcat( buf, "in-addr" );
	 else if (aclpass->type == ACL_TYPE_TERMINATOR)
	    strcat( buf, "none" );
	 else
	    strcat( buf, "unknown" );
      else
	 encodeURIvalue( buf + strlen(buf), aclpass->name );
      break;

    case 'u': 		                /* Requested URL */
      d = buf + strlen( buf );
      encodeURIvalue( d, req->url2display );
      break;

    case 'U': 		                /* parsed URL */
      d = buf + strlen( buf );
      if (!UFDBglobalShowURLdetails)
      {
         t = req->surl + strcspn( req->surl, "?&" );
	 if (*t != '\0')
	 {
	    *t++ = '.';
	    *t++ = '.';
	    *t++ = '.';
	    *t = '\0';
	 }
      }
      encodeURIvalue( d, req->surl );
      break;

    default:
      strcat( buf, "%" );
    }
    p++;
    q = p;
  }

  if (q == redirect)   	/* no '%' found in redirect string */
  {
    if (force_302  &&  strncmp( redirect, "302:", 4 ) != 0)
    {
       strcpy( buf, "302:" );
       strcat( buf, redirect );
    }
    else
       strcpy( buf, redirect );
  }

  return buf;
}


/*
 *  converts yyyy.mm.ddTHH:MM:SS to seconds since EPOC
 */

time_t iso2sec( char * date )
{
  struct tm *t;
  int y,m,d,H,M,S;

  t = (struct tm *) ufdbCalloc(1,sizeof(struct tm));
  sscanf(date,"%4d%*[.-]%2d%*[.-]%2d%*[T]%2d%*[:-]%2d%*[:-]%2d",
	 &y,&m,&d,&H,&M,&S);
  m--; 
  y = y - 1900;
  if (y < 0 || m < 0 || m > 11 || d < 1 || d > 31 || H < 0 || H > 23 
     || M < 0 || M > 59 || S < 0 || S > 59)
    return (time_t) -1;
  t->tm_year = y;
  t->tm_mon =  m;
  t->tm_mday = d;
  t->tm_hour = H;
  t->tm_min = M;
  t->tm_sec = S;

  return (time_t) mktime(t);
}

/*
 *  converts yyyy.mm.dd to seconds since EPOC
 */
time_t date2sec( char * date )
{
  struct tm *t;
  int y,m,d;

  t = (struct tm *) ufdbCalloc(1,sizeof(struct tm));
  sscanf(date,"%4d%*[.-]%2d%*[.-]%2d",&y,&m,&d);
  m--; 
  y = y - 1900;
  if (y < 0 || m < 0 || m > 11 || d < 1 || d > 31)
    return (time_t) -1;
  t->tm_year = y;
  t->tm_mon =  m;
  t->tm_mday = d;

  return (time_t) mktime(t);
}

