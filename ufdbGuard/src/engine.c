/*
 * engine.c - URLfilterDB
 *
 * ufdbGuard is copyrighted (C) 2005-2018 by URLfilterDB B.V. with all rights reserved.
 *
 * Parts of ufdbGuard are based on squidGuard.
 * This module is NOT based on squidGuard.
 *
 * RCS $Id: engine.c,v 1.37 2018/07/25 13:41:46 root Exp root $
 */


#include "ufdb.h"
#include "sg.h"
#include "ufdblib.h"
#include "ufdbdb.h"
#include "ufdbchkport.h"

#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define UFDB_DEBUG_ACL_ACCESS 0


UFDB_GCC_ALIGN64 pthread_rwlock_t  TheDynamicSourcesLock;
UFDB_GCC_ALIGN64 pthread_rwlock_t  TheDynamicCategoriesLock;
UFDB_GCC_ALIGN64 int __dummy0;


void UFDBaclEngineInit( void )
{
   pthread_rwlockattr_t mylock_attr;

   pthread_rwlockattr_init( &mylock_attr );

#if HAVE_PTHREAD_RWLOCK_PREFER_WRITER_NP
   pthread_rwlockattr_setkind_np( &mylock_attr, PTHREAD_RWLOCK_PREFER_WRITER_NP );
#else
   ufdbLogMessage( "WARNING: could not set writer lock preference for the dynamic source lock.  *****\n"
                   "This may delay updates for sources with execuserlist and execdomainlist   *****" );
#endif

   pthread_rwlock_init( &TheDynamicSourcesLock, &mylock_attr );
   pthread_rwlock_init( &TheDynamicCategoriesLock, &mylock_attr );
}


UFDB_GCC_HOT UFDB_GCC_INLINE
static struct Source * matchSourceAND(
   struct Source *    s, 
   struct SquidInfo * si )
{
   int                found;
   struct Ipv4 *      i4;
   struct Ipv6 *      i6;

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: matchSourceAND: trying source %s", si->worker, s->name );
#endif

   if (si->source_isipv4)
   {
      if (s->ipv4 == NULL  &&  s->ipv6 != NULL)
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND:    request has IPv4 but source %s only has IPv6 list",
                            si->worker, s->name );
#endif
         return NULL;
      }
   }
   if (si->source_isipv6)
   {
      if (s->ipv6 == NULL  &&  s->ipv4 != NULL)
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND:    request has IPv6 but source %s only has IPv4 list",
                            si->worker, s->name );
#endif
         return NULL;
      }
   }

   if (s->ipv4 != NULL)
   {
      if (si->source_isipv4)
      {
         found = 0;
         for (i4 = s->ipv4;  i4 != NULL;  i4 = i4->next)
         {
            if (!i4->net_is_set)
               continue;

            if (i4->type == SG_IPTYPE_RANGE) 
            {
               if (si->source_ipv4 >= i4->net  &&  si->source_ipv4 <= i4->mask)
               {
                  strcpy( si->matchedBy, "ipv4-range" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceAND:    ipv4 \"%s\" found in ipv4 range of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  found = 1;
                  break;
               }
            }
            else				/* CIDR or HOST */
            { 
               if ((si->source_ipv4 & i4->mask) == i4->net)
               {
                  strcpy( si->matchedBy, "ipv4-subnet" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceAND:    ipv4 \"%s\" found in ipv4 subnet of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  found = 1;
                  break;
               }
            }
         }
         if (found == 0)
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    ipv4 \"%s\" NOT found in ipv4 hosts of source %s",
                               si->worker, si->srcIP, s->name );
#endif
            return NULL;
         }
      }
      else
      {
         if (si->source_isipv4)
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no IPv4 addresses but the request has",
                               si->worker, s->name );
#endif
            return NULL;
         }
      }
   }
   else
   {
      if (si->source_isipv4)
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no IPv4 addresses", si->worker, s->name );
#endif
      }
   }

   if (s->ipv6 != NULL)
   {
      if (si->source_isipv6)
      {
         found = 0;
         for (i6 = s->ipv6;  i6 != NULL;  i6 = i6->next)
         {
            if (i6->type == SG_IPV6TYPE_CIDR)           /* CIDR */
            {
               if (ufdbIPv6hostInNet( &si->source_ipv6, &i6->ipv6, i6->cidr ))
               {
                  strcpy( si->matchedBy, "ipv6-net" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceAND:    ipv6 \"%s\" found in ipv6 subnets of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  found = 1;
                  break;
               }
            }
            else				        /* HOST */
            { 
               if (memcmp( (void*) &si->source_ipv6, (void*) &i6->ipv6, sizeof(si->source_ipv6)) == 0)
               {
                  strcpy( si->matchedBy, "ipv6-host" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceAND:    ipv6 \"%s\" found in ipv6 hosts of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  found = 1;
                  break;
               }
            }
         }
         if (found == 0)
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    ipv6 \"%s\" NOT found in ipv6 hosts of source %s",
                               si->worker, si->srcIP, s->name );
#endif
            return NULL;
         }
      }
      else
      {
         if (si->source_isipv6)
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no IPv6 addresses but the request has",
                               si->worker, s->name );
#endif
            return NULL;
         }
      }
   }
   else
   {
      if (si->source_isipv6)
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no IPv6 addresses", si->worker, s->name );
#endif
      }
   }

   if (s->userDb != NULL)
   {
      if (si->ident[0] == '\0'  ||  (si->ident[0] == '-'  &&  si->ident[1] == '\0'))
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND  source %s has a userlist but no username exists",
                            si->worker, s->name==NULL ? "nosrc" : s->name );
#endif
         return NULL;
      }
      else 
      {
         struct UserQuotaInfo * q = NULL;
         int ret;

         if (s->userDb->type == SGDBTYPE_EXECUSERLIST)
         {
            ret = pthread_rwlock_rdlock( &TheDynamicSourcesLock );
            if (ret != 0)
               ufdbLogError( "W%03d: matchSourceAND:    pthread_rwlock_rdlock TheDynamicSourcesLock failed "
                             "with code %d", 
                             si->worker, ret );
         }

         found = UFDBmemDBfind( (struct UFDBmemDB *) s->userDb->dbcp, si->ident, (char **) (char *) &q );

         if (s->userDb->type == SGDBTYPE_EXECUSERLIST)
         {
            ret = pthread_rwlock_unlock( &TheDynamicSourcesLock );
            if (ret != 0)
               ufdbLogError( "W%03d: matchSourceAND:    pthread_rwlock_unlock TheDynamicSourcesLock failed "
                             "with code %d", 
                             si->worker, ret );
         }

         if (found)
         {
            strcpy( si->matchedBy, "username" );
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    user \"%s\" found in userlist of source \"%s\"",
                               si->worker, si->ident, s->name==NULL ? "NULL" : s->name );
#endif
         }
         else
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    user \"%s\" NOT found in userlist of source \"%s\"",
                               si->worker, si->ident, s->name==NULL ? "NULL" : s->name );
#endif
            return NULL;
         }
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no user list", si->worker, s->name );
#endif
   }

   if (s->domainDb != NULL)
   {
      if (si->srcDomain[0] == '\0'  ||  (si->srcDomain[0] == '-'  &&  si->srcDomain[1] == '\0'))
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceAND  source %s has a domainlist but no domain exists",
                            si->worker, s->name==NULL ? "nosrc" : s->name );
         return NULL;
#endif
      }
      else 
      {
         if (UFDBmemDBfindDomain( (struct UFDBmemDB *) s->domainDb, si->srcDomain ))
         {
            strcpy( si->matchedBy, "hostname" );
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    domain \"%s\" found in domains of source \"%s\"",
                               si->worker, si->srcDomain, s->name );
#endif
         }
         else
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceAND:    domain \"%s\" NOT found in domains of source \"%s\"",
                               si->worker, si->srcDomain, s->name );
#endif
            return NULL;
         }
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceAND:    source %s has no domain list", si->worker, s->name );
#endif
   }

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: matchSourceAND:    source \"%s\" matched ALL", 
                      si->worker, s->name==NULL ? "NULL" : s->name );
   return s;
}



UFDB_GCC_HOT UFDB_GCC_INLINE
static struct Source * matchSourceOR(
   struct Source *    s, 
   struct SquidInfo * si )
{
   int                found;
   struct Ipv4 *      i4;
   struct Ipv6 *      i6;

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: matchSourceOR: trying source %s", si->worker, s->name );
#endif

   if (s->ipv4 != NULL)
   {
      if (si->source_isipv4)
      {
         for (i4 = s->ipv4;  i4 != NULL;  i4 = i4->next)
         {
            if (!i4->net_is_set)
               continue;

            if (i4->type == SG_IPTYPE_RANGE) 
            {
               if (si->source_ipv4 >= i4->net  &&  si->source_ipv4 <= i4->mask)
               {
                  strcpy( si->matchedBy, "ipv4-range" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceOR:    ipv4 \"%s\" found in ipv4 ranges of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  return s;
               }
            }
            else				/* CIDR or HOST */
            { 
               if ((si->source_ipv4 & i4->mask) == i4->net)
               {
                  strcpy( si->matchedBy, "ipv4-subnet" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceOR:    ipv4 \"%s\" found in ipv4 subnets of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  return s;
               }
            }
         }
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    ipv4 \"%s\" NOT found in ipv4 hosts of source %s",
                            si->worker, si->srcIP, s->name );
#endif
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceOR:    source %s has no ipv4 list", si->worker, s->name );
#endif
   }

   if (s->ipv6 != NULL)
   {
      if (si->source_isipv6)
      {
         for (i6 = s->ipv6;  i6 != NULL;  i6 = i6->next)
         {
            if (i6->type == SG_IPV6TYPE_CIDR)           /* CIDR */
            {
               if (ufdbIPv6hostInNet( &si->source_ipv6, &i6->ipv6, i6->cidr ))
               {
                  strcpy( si->matchedBy, "ipv6-net" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceOR:    ipv6 \"%s\" found in ipv6 subnets of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  return s;
               }
            }
            else				        /* HOST */
            { 
               if (memcmp( (void*) &si->source_ipv6, (void*) &i6->ipv6, sizeof(si->source_ipv6)) == 0)
               {
                  strcpy( si->matchedBy, "ipv6-host" );
#if UFDB_DEBUG_ACL_ACCESS
                  if (UFDBglobalDebug > 2)
                     ufdbLogMessage( "W%03d: matchSourceOR:    ipv6 \"%s\" found in ipv6 hosts of source %s",
                                     si->worker, si->srcIP, s->name );
#endif
                  return s;
               }
            }
         }
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    ipv6 \"%s\" NOT found in ipv6 hosts of source %s",
                            si->worker, si->srcIP, s->name );
#endif
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceOR:    source %s has no ipv6 list", si->worker, s->name );
#endif
   }

   if (s->userDb != NULL)
   {
      if (si->ident[0] == '\0'  ||  (si->ident[0] == '-'  &&  si->ident[1] == '\0'))
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    source %s has a userlist but no username exists",
                            si->worker, s->name==NULL ? "nosrc" : s->name );
#endif
      }
      else 
      {
         struct UserQuotaInfo * q = NULL;
         int ret;

#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    matching user \"%s\" to userlist of \"%s\"", 
                            si->worker, si->ident, s->name==NULL ? "NULL" : s->name );
#endif
         if (s->userDb->type == SGDBTYPE_EXECUSERLIST)
         {
            ret = pthread_rwlock_rdlock( &TheDynamicSourcesLock );
            if (ret != 0)
               ufdbLogError( "W%03d: matchSourceOR:    pthread_rwlock_rdlock TheDynamicSourcesLock failed "
                             "with code %d", 
                             si->worker, ret );
         }

         found = UFDBmemDBfind( (struct UFDBmemDB *) s->userDb->dbcp, si->ident, (char **) (char *) &q );

         if (s->userDb->type == SGDBTYPE_EXECUSERLIST)
         {
            ret = pthread_rwlock_unlock( &TheDynamicSourcesLock );
            if (ret != 0)
               ufdbLogError( "W%03d: matchSourceOR:    pthread_rwlock_unlock TheDynamicSourcesLock failed "
                             "with code %d", 
                             si->worker, ret );
         }

         if (found)
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceOR:    user \"%s\" found in source \"%s\"",
                               si->worker, si->ident, s->name==NULL ? "NULL" : s->name );
#endif
            strcpy( si->matchedBy, "username" );
            return s;
         }
         else
         {
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceOR:    user \"%s\" NOT found in source \"%s\"",
                               si->worker, si->ident, s->name==NULL ? "NULL" : s->name );
#endif
         }
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceOR:    source %s has no user list", si->worker, s->name );
#endif
   }

   if (s->domainDb != NULL)
   {
      if (si->srcDomain[0] == '\0'  ||  (si->srcDomain[0] == '-'  &&  si->srcDomain[1] == '\0'))
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    source %s has a domainslist but there is no domain",
                            si->worker, s->name );
#endif
      }
      else 
      {
#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: matchSourceOR:    matching domain \"%s\" to domains of source %s",
                            si->worker, si->srcDomain, s->name );
#endif
         if (UFDBmemDBfindDomain( (struct UFDBmemDB *) s->domainDb, si->srcDomain ))
         {
            strcpy( si->matchedBy, "hostname" );
#if UFDB_DEBUG_ACL_ACCESS
            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: matchSourceOR:    domain \"%s\" found in domains of source %s",
                               si->worker, si->srcDomain, s->name );
#endif
            return s;
         }
      }
   }
   else
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: matchSourceOR:    source %s has no domain list",
                         si->worker, s->name== NULL ? "NULL" : s->name  );
#endif
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: matchSourceOR:    source \"%s\" not matched", 
                      si->worker, s->name==NULL ? "NULL" : s->name );
#endif
   return NULL;
}


/*
 *  called by main loop: find a source definition based on the user ident/domain/ip
 */
UFDB_GCC_HOT
struct Source * UFDBfindSource( 
   struct Source *    allSrcs, 
   struct SquidInfo * si )
{
   struct Source *    s;
   struct Source *    retval;
 
   if (UFDBglobalFatalError  ||  UFDBglobalReconfig)
   {
#if UFDB_DEBUG_ACL_ACCESS
      ufdbLogMessage( "W%03d: UFDBfindSource: fatal-error or reconfig: returning NULL", si->worker );
#endif
      return NULL;
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: UFDBfindSource   id '%s'  srcIP '%s'  source_isipv4 %d  source_isipv6 %d",
                      si->worker,
                      si->ident, 
		      si->srcIP,
                      si->source_isipv4, 
                      si->source_isipv6 );
#endif

   if (allSrcs == NULL)
   {
      /* allSrcs may be NULL if there is a configuration with only the default rule */
      return NULL;
   }

   si->matchedBy[0] = '\0';

   for (s = allSrcs;  s != NULL;  s = s->next)
   {
      if (s->active == 0)
         continue;

      if (s->evaluationMethod == UFDB_EVAL_OR)
         retval = matchSourceOR( s, si );
      else
         retval = matchSourceAND( s, si );
      if (retval != NULL)
      {
         if (UFDBglobalDebug > 2)
            ufdbLogMessage( "W%03d: UFDBfindSource: found source %s",
                            si->worker, s->name == NULL ? "noname" : s->name  );
         return s;
      }
   }

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: UFDBfindSource: NO source matched", si->worker );
   return NULL;
}


/*
 *  called by main loop (in firewall-mode): find an acl by one of sources
 */
UFDB_GCC_HOT
struct Acl * UFDBfindACLbySourceList(
   struct Acl *       allAcls,
   struct Source *    allSrcs,
   struct SquidInfo * si )
{
   struct Source *    src;
   struct Acl *       acl;
   struct Acl *       retval = NULL;
 
   if (UFDBglobalFatalError  ||  UFDBglobalReconfig)
   {
#if UFDB_DEBUG_ACL_ACCESS
      ufdbLogMessage( "W%03d: UFDBfindACLbySourceList: fatal-error or reconfig: returning NULL", si->worker );
#endif
      return NULL;
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: UFDBfindACLbySourceList  id '%s'  srcIP '%s'  source_isipv4 %d  source_isipv6 %d",
                      si->worker,
                      si->ident, 
		      si->srcIP,
                      si->source_isipv4, 
                      si->source_isipv6 );
#endif

   if (allAcls == NULL || allSrcs == NULL)
   {
      /* allSrcs and allAcls may be NULL if there is a configuration with only the default rule */
      return NULL;
   }

   for (acl = allAcls;  acl != NULL;  acl = acl->next)
   {
      for (src = allSrcs; src != NULL; src = UFDBfindSource(src->next, si))
      {
         if (acl->source == src)
         {
            retval = acl;

            if (UFDBglobalDebug > 2)
               ufdbLogMessage( "W%03d: UFDBfindACLbySourceList: found acl %s for source %s",
                               si->worker, acl->name == NULL ? "noname" : acl->name, src->name == NULL ? "noname" : src->name );
            return retval;
         }
      }
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug)
      ufdbLogMessage( "W%03d: UFDBfindACLbySourceList: no active ACL for sourceList found, returning defaultAcl", 
                      si->worker, src->name );
#endif

   if (UFDBglobalDebug > 2)
      ufdbLogMessage( "W%03d: UFDBfindACLbySourceList: NO ACL matched", si->worker );
   return NULL;
}


/*
 * called by main loop: find the first active ACL that matches a source
 */
UFDB_GCC_HOT
struct Acl * UFDBfindACLbySource( 
   struct Source *     source,
   struct SquidInfo *  si  )
{
   struct Acl *    acl;

   if (source == NULL)
   {
      if (UFDBglobalReconfig  ||  UFDBglobalFatalError)
         return NULL;

#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug)
#else
      if (UFDBglobalDebug > 1)
#endif
	 ufdbLogMessage( "W%03d: UFDBfindACLbySource: source=NULL returning defaultACL", si->worker );

      /* the username and IP address are not used to make the decision to use the ACL "default"
       * but for logging and reporting purposes, the username or IP address is given here.
       */
      if (si->ident[0] != '-'  &&  si->ident[1] != '\0')
      {
         strcpy( si->matchedBy, "username" );
      }
      else if (si->srcIP[0] != '-'  &&  si->srcIP[1] != '\0')
      {
         strcpy( si->matchedBy, "ip" );
      }
      else if (si->srcDomain[0] != '-'  &&  si->srcDomain[1] != '\0')
      {
         strcpy( si->matchedBy, "hostname" );
      }
      else
	 strcpy( si->matchedBy, "default" );

      return defaultAcl;
   }

   for (acl = AclList;  acl != NULL;  acl = acl->next)
   {
      if (acl->source == source  &&  acl->active)
      {
#if UFDB_DEBUG_ACL_ACCESS
	 if (UFDBglobalDebug)
	    ufdbLogMessage( "W%03d: UFDBfindACLbySource: active ACL for source '%s' found", si->worker, source->name );
#endif

	 return acl;
      }
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug)
      ufdbLogMessage( "W%03d: UFDBfindACLbySource: no active ACL for source '%s' found, returning defaultAcl", 
                      si->worker, source->name );
#endif

   return defaultAcl;
}

/*
 * called by main loop: find the next active ACL that matches a source
 */
UFDB_GCC_HOT
struct Acl * UFDBfindNextACLforSource( 
   struct Acl *        current,
   struct Source *     source,
   struct SquidInfo *  si  )
{
   struct Acl *    acl = current->next;

   if (source == NULL)
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug)
#else
      if (UFDBglobalDebug > 1)
#endif
	 ufdbLogMessage( "W%03d: UFDBfindNextACLforSource: source=NULL returning NULL", si->worker );

      return NULL;
   }

   for (;  acl != NULL;  acl = acl->next)
   {
      if (acl->source == source  &&  acl->active)
      {
#if UFDB_DEBUG_ACL_ACCESS
	 if (UFDBglobalDebug)
	    ufdbLogMessage( "W%03d: UFDBfindNextACLforSource: next active ACL for source '%s' found", si->worker, source->name );
#endif

	 return acl;
      }
   }

#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug)
      ufdbLogMessage( "W%03d: UFDBfindNextACLforSource: no next active ACL for source '%s' found, returning NULL", 
                      si->worker, source->name );
#endif

   return NULL;
}


void UFDBmakeRedirectString( 
   struct SquidInfo * si,
   struct Acl *       acl,
   struct Category *  category,
   char *             redirectBuffer )
{
   char * format;

   if (category != NULL  &&  category->redirect != NULL)
      format = category->redirect;
   else if (acl->redirect != NULL)
      format = acl->redirect;
   else
   {
      if (defaultAcl == NULL)
	 /* TO-DO: copy the redirect rule for the security or proxy category */
	 /* TO-DO: instead of referring to cgibin.urlfilterdb.com */
	 format = (char *) UFDBglobalRedirectNoDefaultACLClientCategoryURL;
      else
      {
	 format = defaultAcl->redirect;
	 if (format == NULL)
	    format = (char *) UFDBglobalRedirectNoDefaultRedirectClientCategoryURL;
      }
   }

   sgParseRedirect( format, si, acl, si->aclpass, redirectBuffer );
}


UFDB_GCC_INLINE
static int hasNoneTerminator( struct AclCategory * ac )
{
   while (ac != NULL)
   {
      if (ac->type == ACL_TYPE_TERMINATOR  &&  !ac->access)
         return 1;
      ac = ac->next;
   }
   return 0;
}


UFDB_GCC_INLINE
static int CategoryInAcl( struct Acl * acl, struct Category * category )
{
   struct AclCategory * ac;
   struct Category * cat;

   for (ac = acl->pass;  ac != NULL;  ac = ac->next)
   {
      cat = ac->cat;
      if (cat != NULL  &&  cat->active  &&  strcmp( cat->name, category->name ) == 0)
         return 1;
   }

   return 0;
}


UFDB_GCC_INLINE
static void addCatToImplicit( struct Acl * acl, struct Category * cat )
{
   struct AclCategory *  ac;
   struct AclCategory ** implicit;

   ac = (struct AclCategory *) ufdbMalloc( sizeof(struct AclCategory) );
   ac->name = ufdbStrdup( cat->name );
   ac->cat = cat;
   ac->access = UFDB_ACL_ACCESS_ALLOW;
   ac->type = ACL_TYPE_DEFAULT;
   ac->nblocks = 0;
   ac->nmatches = 0;
   ac->next = NULL;

   implicit = &(acl->implicitPass);
   while (*implicit != NULL)
   {
      implicit = &((*implicit)->next);
   }
   *implicit = ac;
}


void BuildImplicitPassLists( void )
{
   struct Acl *      acl;
   struct Category * cat;

   /*
    * Build the implicitPass list for all ACLs.
    */
   for (acl = AclList;  acl != NULL;  acl = acl->next)
   {
      acl->hasTerminatorNone = hasNoneTerminator( acl->pass );
      if (!acl->hasTerminatorNone)
      {
	 for (cat = Cat;  cat != NULL;  cat = cat->next)
	 {
	    if (cat->domainlist != NULL  &&  strcmp( cat->name, "checked" ) != 0  &&  !CategoryInAcl( acl, cat ))
	       addCatToImplicit( acl, cat );
	 }
      }
   }
}


UFDB_GCC_HOT 
int UFDBdecideBasedOnSpecificCategory( 
   struct Acl *         acl,
   struct AclCategory * aclpass, 
   struct SquidInfo *   si,
   UFDBrevURL *         revURL,
   char *               redirectURL,
   int                  matchRE  )
{
   int                  decision;
   struct Category *    category;

   decision = UFDB_ACL_ACCESS_DUNNO;
   category = aclpass->cat;
   if (category == NULL  ||  !category->active)
      return decision;

   /* category SafeSearch */
   if (category->options & UFDB_OPT_SAFE_SEARCH)
   {
      if (UFDBaddSafeSearch( si->domain, si->surl, si->orig )
	      == UFDB_API_MODIFIED_FOR_SAFESEARCH)
      {
	 si->blockReason = UFDB_API_BLOCKR_SAFESEARCH;
	 si->aclpass = aclpass;
	 strcpy( redirectURL, si->orig );
#if UFDB_DEBUG_ACL_ACCESS
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideBasedOnSpecificCategory: category option SafeSearch matched, return ALLOW",
                            si->worker );
#endif
	 decision = UFDB_ACL_ACCESS_ALLOW;
	 return decision;
      }
   }

   /* domain name lookup for this source */
   if (category->domainlistDb != NULL) 
   {
      int result;
      int use_rw_lock;
      struct UFDBmemTable * mt;

      if (UFDBglobalDebug > 2)
         ufdbLogMessage( "W%03d: UFDBdecideBasedOnSpecificCategory: using acl \"%s\" with category \"%s\" to search RevUrl",
                         si->worker, acl->name, category->name  );

      use_rw_lock = (category->execdomainlist != NULL);
      if (use_rw_lock)					/* LOCK ====================================== */
      {
	 int ret;
#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
	 ret = pthread_rwlock_rdlock( &TheDynamicCategoriesLock );
	 if (ret != 0)
	    ufdbLogError( "W%03d: UFDBdecideBasedOnSpecificCategory: pthread_rwlock_rdlock TheDynamicCategoriesLock failed with code %d", 
                          si->worker, ret );
#else
	 ret = pthread_mutex_lock( &TheDynamicCategoriesLock );
	 if (ret != 0)
	    ufdbLogError( "W%03d: UFDBdecideBasedOnSpecificCategory: pthread_mutex_lock TheDynamicCategoriesLock failed with code %d", 
                          si->worker, ret );
#endif
      }

      mt = (struct UFDBmemTable *) category->domainlistDb->dbcp;
      result = UFDBlookupRevUrl( &(mt->table.nextLevel[0]), revURL );

      if (use_rw_lock)					/* UNLOCK ==================================== */
      {
	 int ret;
#ifdef UFDB_HAVE_NATIVE_RWLOCK_MUTEX
	 ret = pthread_rwlock_unlock( &TheDynamicCategoriesLock );
	 if (ret != 0)
	    ufdbLogError( "W%03d: UFDBdecideBasedOnSpecificCategory: pthread_rwlock_unlock TheDynamicCategoriesLock failed with code %d", 
                          si->worker, ret );
#else
	 ret = pthread_mutex_unlock( &TheDynamicCategoriesLock );
	 if (ret != 0)
	    ufdbLogError( "W%03d: UFDBdecideBasedOnSpecificCategory: pthread_mutex_unlock TheDynamicCategoriesLock failed with code %d", 
                          si->worker, ret );
#endif
      }

      if (result)
      {
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideBasedOnSpecificCategory: acl \"%s\" with category \"%s\": returned a match by domain",
			    si->worker, acl->name, category->name  );

	 si->blockReason = UFDB_API_BLOCKR_ACL;
	 si->aclpass = aclpass;
	 decision = aclpass->access;
	 if (decision == UFDB_ACL_ACCESS_BLOCK)
	    UFDBmakeRedirectString( si, acl, category, redirectURL );
	 return decision;
      }
   }

   /* regular expression lookup for this category */
   if (matchRE  &&  category->regExp != NULL)
   {
      int result;

      result = ufdbRegExpMatch( category->regExp, si->surl );
      if (result) 
      {
	 if (UFDBglobalDebug > 1  ||  UFDBglobalDebugRegexp)
	    ufdbLogMessage( "W%03d: UFDBdecideBasedOnSpecificCategory:  acl \"%s\" with category \"%s\": returned a match by expression",
			    si->worker, acl->name, category->name  );
	 si->blockReason = UFDB_API_BLOCKR_ACL;
	 si->aclpass = aclpass;
	 decision = aclpass->access;
	 if (decision == UFDB_ACL_ACCESS_BLOCK)
	    UFDBmakeRedirectString( si, acl, category, redirectURL );
	 return decision;
      }
   }

   return decision;
}


int UFDBdecideAccessBasedOnCategory( 
   struct Acl * acl, 
   struct SquidInfo * si, 
   char * category, 
   char * redirectURL )
{
   struct AclCategory * aclpass;

   for (aclpass = acl->pass;  aclpass != NULL;  aclpass = aclpass->next)
   {
      struct Category * cat;

      cat = aclpass->cat;
      if (cat != NULL  &&  cat->active  &&  strcmp( cat->name, category ) == 0)
      {
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnCategory: category %s %sallowed", 
                            si->worker,
	                    category, 
			    aclpass->access == UFDB_ACL_ACCESS_BLOCK ? "not " : "" );
	 if (!aclpass->access)
	    UFDBmakeRedirectString( si, acl, cat, redirectURL );
	 return aclpass->access == 0 ? UFDB_ACL_ACCESS_BLOCK : UFDB_ACL_ACCESS_ALLOW;
      }
   }

   for (aclpass = acl->implicitPass;  aclpass != NULL;  aclpass = aclpass->next)
   {
      struct Category * cat;

      cat = aclpass->cat;
      if (cat != NULL  &&  cat->active  &&  strcmp( cat->name, category ) == 0)
      {
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnCategory: category %s (implicitly) allowed", 
                            si->worker, category );
         return UFDB_ACL_ACCESS_ALLOW;
      }
   }

   return UFDB_ACL_ACCESS_DUNNO;
}


UFDB_GCC_HOT
int UFDBurlIsCheckedCategory( struct SquidInfo * si )
{
   if (UFDBglobalCheckedDB.table.nNextLevels > 0)
   {
      if (UFDBlookupRevUrl( &(UFDBglobalCheckedDB.table.nextLevel[0]), si->revUrl ))
      {
	 si->blockReason = UFDB_API_BLOCKR_CHECKED;
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBurlIsCheckedCategory: implicit category \"checked\" was matched for URL %s", 
	                    si->worker, si->url2log );
	 return 1;
      }
      if (UFDBglobalCheckedExpressions != NULL)
      {
	 if (ufdbRegExpMatch( UFDBglobalCheckedExpressions, si->orig ) == UFDB_API_MATCH)
         {
	    si->blockReason = UFDB_API_BLOCKR_CHECKED;
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBurlIsCheckedCategory: implicit category \"checked\" was matched for URL %s", 
			       si->worker, si->url2log );
	    return 1;
	 }
      }
   }
   return 0;
}


/*
 * decide-implicit-based-on-url:
 *      Decide on dunno/allow based on the URL and the implicitpass list of the ACL.
 *      The decision is based on the URL tables and not expressions.
 *
 * Return value:
 * UFDB_ACL_ACCESS_DUNNO:  don't know.
 * UFDB_ACL_ACCESS_ALLOW:  explicit allow.
 */
UFDB_GCC_HOT
int UFDBdecideImplicitAccessBasedOnURL(
   int                  reconfiguring,
   struct Acl *         acl, 
   struct SquidInfo *   si,
   char *               redirectURL )
{
   int                  decision;
   struct Category *    category;
   struct AclCategory * aclpass;

   if (reconfiguring)
      return UFDB_ACL_ACCESS_DUNNO;

#if 0
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   UFDBdecideImplicitAccessBasedOnURL :" );
#endif

   for (aclpass = acl->implicitPass;  aclpass != NULL;  aclpass = aclpass->next)	/* walk all other not-named categories */
   {
      category = aclpass->cat;
      if (category != NULL  &&  !category->active)
	 continue;

      if (UFDBglobalDebug > 1  &&  category != NULL)
	 ufdbLogMessage( "W%03d: UFDBdecideImplicitAccessBasedOnURL: testing acl \"%s\" with implicit category \"%s\" for URL %s", 
			 si->worker, acl->name, category->name, si->url2log );

      decision = UFDBdecideBasedOnSpecificCategory( acl, aclpass, si, si->revUrl, redirectURL, 0 );
      if (decision != UFDB_ACL_ACCESS_DUNNO)
      {
	 si->blockReason = UFDB_API_BLOCKR_PASS;			/* implicit ACL is always "PASS" */
	 si->aclpass = aclpass;
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideImplicitAccessBasedOnURL: acl \"%s\" matched implicit category \"%s\" for URL %s", 
	                    si->worker, acl->name, aclpass->name, si->url2log );
	 return UFDB_ACL_ACCESS_ALLOW;
      }
   }

   if (UFDBurlIsCheckedCategory( si ))
   {
      if (UFDBglobalDebug > 1)
	 ufdbLogMessage( "W%03d: UFDBdecideImplicitAccessBasedOnURL: implicit category \"checked\" was matched in acl %s for URL %s", 
			 si->worker, acl->name, si->url2log );
      return UFDB_ACL_ACCESS_ALLOW;
   }

   return UFDB_ACL_ACCESS_DUNNO;
}


/*
 * decide-based-on-url:
 *      Decide on block/allow based on the URL and the ACL.
 *      The decision is based on the URL tables and expressions, so there is no HTTPS peeking here.
 *	This function also includes checking for the safesearch-category.
 *
 * Return value:
 * UFDB_ACL_ACCESS_DUNNO:  don't know.
 * UFDB_ACL_ACCESS_ALLOW:  explicit allow.
 * UFDB_ACL_ACCESS_BLOCK:  explicit block; a redirection string is returned also.
 */
UFDB_GCC_HOT
int UFDBdecideAccessBasedOnURL(
   int                  reconfiguring,
   struct Acl *         acl, 
   struct SquidInfo *   si, 
   char *               redirectURL )
{
   int                  decision;
   char *               redirect;
   struct sgRewrite *   rewrite = NULL;
   struct AclCategory * aclpass = NULL;
   struct Category *    category = NULL;
   char                 newstring[UFDB_MAX_URL_LENGTH];
   char                 newredir[UFDB_MAX_URL_LENGTH + UFDB_MAX_URL_LENGTH];

   si->blockReason = UFDB_API_BLOCKR_PASS;
   si->aclpass = NULL;
   si->matchedAny = 0;
   si->matchedNone = 0;

#if 0
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "   UFDBdecideAccessBasedOnURL :" );
#endif

   if (reconfiguring)
   {
#ifdef _POSIX_PRIORITY_SCHEDULING
      static volatile int yieldcounter = 0;
      if (++yieldcounter > 8)
      {
         yieldcounter = 0;
         sched_yield();		
      }
#endif
      si->blockReason = UFDB_API_BLOCKR_LOADING_DB;
      if (UFDBglobalURLlookupResultDBreload == UFDB_DENY)
      {
         /* TODO: for HTTPS connections use UFDBglobalRedirectHttps/UFDBglobalRedirectBumpedHttps ***** */
         strcpy( redirectURL, UFDBglobalLoadingDatabaseRedirect );
         strcpy( si->matchedBy, "loading-db" );
         return UFDB_ACL_ACCESS_BLOCK;
      }
      *redirectURL = '\0';
      return UFDB_ACL_ACCESS_ALLOW;
   }

   if (UFDBglobalFatalError)
   {
      si->blockReason = UFDB_API_BLOCKR_FATAL_ERROR;
      if (UFDBglobalURLlookupResultFatalError == UFDB_DENY)
      {
         /* TODO: for HTTPS connections use UFDBglobalRedirectHttps/UFDBglobalRedirectBumpedHttps ***** */
         strcpy( redirectURL, UFDBglobalFatalErrorRedirect );
         strcpy( si->matchedBy, "fatal-error" );
         return UFDB_ACL_ACCESS_BLOCK;
      }
      *redirectURL = '\0';
      return UFDB_ACL_ACCESS_ALLOW;
   }

   if (acl == NULL)
   {
#if UFDB_DEBUG_ACL_ACCESS
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL  acl = NULL,  return DUNNO", si->worker );
#endif
      return UFDB_ACL_ACCESS_DUNNO;
   }

   si->aclpass = NULL;
   if (acl->pass == NULL)
      acl->pass = defaultAcl->pass;

   if (acl->pass == NULL)
   {  
      decision = UFDB_ACL_ACCESS_BLOCK;
      if (defaultAcl == NULL  ||  defaultAcl->redirect == NULL)
         redirect = (char *) UFDBglobalRedirectNoDefaultACLCategoryURL;
      else
	 redirect = defaultAcl->redirect;
   }
   else
   {
      decision = UFDB_ACL_ACCESS_DUNNO;
      redirect = NULL;

      /* walk all explicitly named categories */
      for (aclpass = acl->pass;  aclpass != NULL;  aclpass = aclpass->next)
      {
         category = aclpass->cat;
         if (category != NULL  &&  !category->active)
  	    continue;

#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 2  &&  category != NULL)
  	    ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: testing acl \"%s\" with category \"%s\" for %s %s",
	                    si->worker, acl->name, category->name, si->method, si->url2log );
#endif

         if (aclpass->type == ACL_TYPE_TERMINATOR)  				/*  pass any/none  */
         {
            si->aclpass = aclpass;
	    if (aclpass->access)	/* ANY */
	    {
	       /* Be careful here: "any" is matched, but if port==443 or method==CONNECT we must do 
                * HTTPS probing as well to decide if the URL is allowed or not.
		*/
	       if (si->port == 80)
	       {
		  if (UFDBglobalDebug > 1)
		     ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: pass; acl \"%s\" matched pseudo category \"any\" port=80 for %s %s",
		                     si->worker, acl->name, si->method, si->url2log );
		  si->matchedAny = 1;
		  si->blockReason = UFDB_API_BLOCKR_PASS;
                  decision = UFDB_ACL_ACCESS_ALLOW;
	       }
	       else
	       {
		  /* here we cannot always make a decision about allowing the URL. We might need a HTTPS probe */
                  if (UFDBglobalDebug > 1)
                     ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: dunno/pass; acl \"%s\" matched pseudo category \"any\" port!=80 for %s %s",
                                     si->worker, acl->name, si->method, si->url2log );
		  si->matchedAny = 2;
                  if (UFDBglobalTunnelCheckMethod == UFDB_API_HTTPS_CHECK_OFF)
                  {
                     si->blockReason = UFDB_API_BLOCKR_PASS;
                     decision = UFDB_ACL_ACCESS_ALLOW;
                  }
                  /* if the acl has "pass ... proxies ... any" an HTTPS probe must be made and is done by an other part of the engine (not here) */
	       }
	    }
	    else			/* NONE */
	    {
	       if (UFDBglobalDebug > 1)
	          ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: block; acl \"%s\" matched category "
                                  "\"none\" for %s %s",
		                  si->worker, acl->name, si->method, si->url2log );
	       decision = UFDB_ACL_ACCESS_BLOCK;
	       si->matchedNone = 1;
	       si->blockReason = UFDB_API_BLOCKR_ACL_NONE;
	    }
	    break;
         }
	 else
         if (aclpass->type == ACL_TYPE_INADDR) 					/*  pass [!]in-addr  */
         {
	    if (si->url_has_ipv4 || si->url_has_ipv6) 
	    {
	       si->aclpass = aclpass;
	       decision = aclpass->access == 0 ? UFDB_ACL_ACCESS_BLOCK : UFDB_ACL_ACCESS_ALLOW;
	       if (decision == UFDB_ACL_ACCESS_BLOCK)
		  si->blockReason = UFDB_API_BLOCKR_ACL;
	       if (UFDBglobalDebug > 1)
	          ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: %s; acl \"%s\" matched category "
                                  "\"in-addr\" for %s %s",
		                  si->worker, 
                                  decision == UFDB_ACL_ACCESS_BLOCK ? "block" : "pass",
                                  acl->name, si->method, si->url2log );
	       break;
	    }
	    continue;
         }
  
         /*  pass [!]category  */
	 decision = UFDBdecideBasedOnSpecificCategory( acl, aclpass, si, si->revUrl, redirectURL, 1 );	
	 if (decision != UFDB_ACL_ACCESS_DUNNO)
	 {
	    si->aclpass = aclpass;
	    if (decision == UFDB_ACL_ACCESS_BLOCK)
	       si->blockReason = UFDB_API_BLOCKR_ACL;
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: %s; acl \"%s\" matched category \"%s\" "
                               "for  port %d  protocol %s  method %s  URL %s", 
	                       si->worker, 
                               decision == UFDB_ACL_ACCESS_BLOCK ? "block" : "pass", 
			       acl->name, aclpass->name, si->port, si->protocol, si->method, si->url2log );
	    break;
	 }
	 /* TO-DO: look at rtsp and mms protocols */
      }

      if (decision == UFDB_ACL_ACCESS_DUNNO)
      {
         if (!si->matchedAny  &&  acl->source != NULL  &&  acl->source->cont_search)
         {
            if (UFDBglobalDebug > 1)
               ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: acl \"%s\" did not match and "
                               "the source has 'continue' set so implicit categories are not used",
                               si->worker, acl->name );
            return decision;
         }

	 if (si->port == 80)
	 {
            /* walk all other not-named categories */
	    for (aclpass = acl->implicitPass;  aclpass != NULL;  aclpass = aclpass->next)
	    {
	       category = aclpass->cat;
	       if (category != NULL  &&  !category->active)
		  continue;
	
#if UFDB_DEBUG_ACL_ACCESS
	       if (UFDBglobalDebug > 2  &&  category != NULL)
		  ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: testing acl \"%s\" with implicit category \"%s\" for %s %s",
		                  si->worker, acl->name, category->name, si->method, si->url2log );
#endif

	       decision = UFDBdecideBasedOnSpecificCategory( acl, aclpass, si, si->revUrl, redirectURL, 0 );
	       if (decision != UFDB_ACL_ACCESS_DUNNO)
	       {
		  si->blockReason = UFDB_API_BLOCKR_PASS;		/* implicit ACL is always "PASS" */
	          si->aclpass = aclpass;
		  if (UFDBglobalDebug > 1)
		     ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: pass; acl \"%s\" matched implicit category \"%s\" for %s %s",
		                     si->worker, acl->name, aclpass->name, si->method, si->url2log );
		  break;
	       }
	    }
	 }
	 else
	 {
#if 1
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: acl %s: matching against implicit categories is skipped since port=%d", 
	                       si->worker, acl->name, si->port );
#endif
	 }
      }

      if (decision == UFDB_ACL_ACCESS_DUNNO)
      {
         if (UFDBurlIsCheckedCategory( si ))
	 {
	    static struct AclCategory static_aclpass_checked = { 0, "checked", NULL, 0, 0, NULL, "", 0 };

	    decision = UFDB_ACL_ACCESS_ALLOW;
	    si->aclpass = &static_aclpass_checked;
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: pass; acl \"%s\" matched implicit category \"checked\" for %s %s",
			       si->worker, acl->name, si->method, si->url2log );
	 }
      }

      if (decision == UFDB_ACL_ACCESS_BLOCK) 
      {
         if (category != NULL  &&  category->redirect != NULL)
	    redirect = category->redirect;
         else if (category != NULL  &&  category->rewrite != NULL  &&  strcmp( si->protocol, "https" ) != 0  &&
  	          (redirect = sgRewriteExpression( category->rewrite, si->orig, newstring )) != NULL)
         {
  	    ;
         }
         else if (acl->redirect == NULL)
	    redirect = defaultAcl->redirect;
         else
	    redirect = acl->redirect;
         if (redirect == NULL)
         {
#if UFDB_DEBUG_ACL_ACCESS
            /* suppress many errors. we only want it once! */
            ufdbLogError( "!!!!! there is no \"redirect\" rule for ACL %s/%s  *****", 
  		          acl->name, aclpass->name );
#endif
            /* TO-DO: copy the redirect rule for the security or proxy category */
            /* TO-DO: instead of referring to cgibin.urlfilterdb.com */
            /* TODO: for HTTPS connections use UFDBglobalRedirectHttps/UFDBglobalRedirectBumpedHttps ***** */
            redirect = (char *) UFDBglobalRedirectClientCategoryURL;
         }
	 if (UFDBglobalDebug > 1)
	    ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: block; redirect is now '%s'", 
                            si->worker, redirect );
      }
   }

   if (decision == UFDB_ACL_ACCESS_BLOCK)
   {
      if (UFDBglobalSquidUsesSSLbump  &&  strcmp( si->method, "CONNECT" ) == 0)
      {
	 /* For some categories it is necessary to block the CONNECT since the server does not have SSL+HTTP */
	 if (category != NULL  &&  category->blockBumpedConnect)
	 {
	    si->BlockedBumpedConnectAllowed = 0;
	    redirect = UFDBglobalRedirectBumpedHttps;
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: block; %s %s redirected to %s since block-bumped-connect is on",
	                       si->worker, si->method, si->url2log, redirect );
	 }
         else
         {
            /* squid-ssl-bump + CONNECT : 
             * the default decision is to allow the CONNECT and the block the next GET/POST, *BUT*
             * this does not work for CONNECT to non-HTTP-based sites where the CONNECT itself must be blocked.
             * So probe the server to see if it uses HTTP which we subsequently can block.
             * Note that a probe is not necessary for Teamviewer, Skype and others.  TODO: skip probe for these sites.
             */
            /* TODO: if (si->sni[0] != '\0' || (serverProto = probeServer[Cached](si->domain)) != HTTP) 
             * TODO:    { block-this-URL } */
            if (UFDBglobalDebug > 1)
               ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: dunno; blocked CONNECT to '%s' is skipped because squid-uses-active-bumping; SNI '%s'", 
                               si->worker, si->orig, si->sni );
            redirect = NULL;
            /* do not return ALLOW since we might want to check the certificate */
            decision = UFDB_ACL_ACCESS_DUNNO;
            si->BlockedBumpedConnectAllowed = 1;
         }
      }
      else
      {
	 /* TO-DO: what if CONNECT to port other then 443 ? */
	 if (si->port == 443  ||  strcmp( si->protocol, "https" ) == 0)
	 {
	    if (strcmp( si->method, "CONNECT" ) == 0  ||  !UFDBglobalSquidUsesSSLbump)
	       redirect = UFDBglobalRedirectHttps;
	    else
	       redirect = UFDBglobalRedirectBumpedHttps;
	    if (UFDBglobalDebug > 1)
	       ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: block; %s %s redirected to %s", 
                               si->worker, si->method, si->url2log, redirect );
	 }
      }
   }

   if (acl->rewrite == NULL)
      rewrite = defaultAcl->rewrite;
   else
      rewrite = acl->rewrite;
   if (rewrite != NULL  &&  decision == UFDB_ACL_ACCESS_DUNNO)
   {
      redirect = sgRewriteExpression( rewrite, si->orig, newstring );
   }
   else if (redirect != NULL) 
   {
      redirect = sgParseRedirect( redirect, si, acl, aclpass, newredir );
   }

   if (redirect != NULL)
   {
      strcpy( redirectURL, redirect );
      si->aclpass = aclpass;
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL: block;  return BLOCK  redirect: %s", 
                         si->worker, redirectURL );
      return UFDB_ACL_ACCESS_BLOCK;
   }

   *redirectURL = '\0';
#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnURL  end, return %s", 
                      si->worker,
                      decision == UFDB_ACL_ACCESS_DUNNO ? "DUNNO" : "PASS" );
#endif

   return decision;
}


UFDB_GCC_HOT
int UFDBdecideAccessByPeekStatus(
   int                  status,
   struct Acl *         acl, 
   struct SquidInfo *   si, 
   char *               redirectURL )
{
   int                  decision;
   char *               redirect;
   struct AclCategory * aclpass = NULL;
   struct Category *    category = NULL;
   char                 newstring[UFDB_MAX_URL_LENGTH];
   char                 newredir[UFDB_MAX_URL_LENGTH + UFDB_MAX_URL_LENGTH];

   if (status == UFDB_API_ERR_SOCKET)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus  status UFDB_API_ERR_SOCKET  return DUNNO",
                         si->worker );
      return UFDB_ACL_ACCESS_DUNNO;
   }

   if (status == UFDB_API_ERR_TLS)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus  status UFDB_API_ERR_TLS  return DUNNO",
                         si->worker );
      return UFDB_ACL_ACCESS_DUNNO;
   }

   if (acl == NULL)
   {
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus  acl = NULL,  return DUNNO", si->worker );
      return UFDB_ACL_ACCESS_DUNNO;
   }

   si->aclpass = NULL;
   if (acl->pass == NULL)
      acl->pass = defaultAcl->pass;

   if (acl->pass == NULL)
   {  
      decision = UFDB_ACL_ACCESS_BLOCK;
      if (defaultAcl == NULL  ||  defaultAcl->redirect == NULL)
         redirect = (char *) UFDBglobalRedirectNoDefaultACLCategoryURL;
      else
	 redirect = defaultAcl->redirect;

      if (si->port == 443  ||  strcmp( si->protocol, "https" ) == 0)
      {
         if (strcmp( si->method, "CONNECT" ) == 0  ||  !UFDBglobalSquidUsesSSLbump)
            redirect = UFDBglobalRedirectHttps;
         else
            redirect = UFDBglobalRedirectBumpedHttps;
      }
      if (UFDBglobalDebug)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: block; no default ACL; %s %s redirected to %s", 
                         si->worker, si->method, si->url2log, redirect );
   }
   else
   {
#if 1
      if (UFDBglobalDebug > 1)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus \"%s\"", si->worker, si->url2log );
#endif

      decision = UFDB_ACL_ACCESS_DUNNO;
      redirect = NULL;

      /* walk all explicitly named categories */
      for (aclpass = acl->pass;  aclpass != NULL;  aclpass = aclpass->next)
      {
         category = aclpass->cat;
         if (category == NULL  ||  category->name == NULL)      // note: terminators have a NULL name
            continue;
         if (!category->active)
  	    continue;

#if UFDB_DEBUG_ACL_ACCESS
         if (UFDBglobalDebug > 1)
         {
            const char * catname;
            catname = category->name;
  	    ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: testing acl \"%s\" with category \"%s\" "
                            "for %s %s and status %d",
	                    si->worker, acl->name, catname, si->method, si->url2log, status );
         }
#endif

         if (status == UFDB_API_ERR_IS_SKYPE  &&   
             (strcmp( category->name, "skype" ) == 0  ||
              strcmp( category->name, "chat/skype" ) == 0))
         {
            decision = UFDBglobalSkypeOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: skype detected for %s and "
                                  "allow-skype-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_GTALK  &&   
             (strcmp( category->name, "gtalk" ) == 0  ||
              strcmp( category->name, "chat/google" ) == 0))
         {
            decision = UFDBglobalGtalkOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: gtalk detected for %s and "
                                  "allow-gtalk-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_FBCHAT  &&   
             (strcmp( category->name, "facebookchat" ) == 0  ||
              strcmp( category->name, "chat/facebook" ) == 0))
         {
            decision = UFDBglobalFBchatOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: facebook chat detected for %s and "
                                  "allow-fb-chat-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_AIM  &&   
             (strcmp( category->name, "aim" ) == 0  ||
              strcmp( category->name, "chat/aim" ) == 0))
         {
            decision = UFDBglobalAimOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: aim detected for %s and "
                                  "allow-aim-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_YAHOOMSG  &&   
             (strcmp( category->name, "yahoomsg" ) == 0  ||
              strcmp( category->name, "chat/yahoo" ) == 0))
         {
            decision = UFDBglobalYahooMsgOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: Yahoo Messenger detected for %s and "
                                  "allow-yahoomsg-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_TEAMVIEWER  &&
             (strcmp( category->name, "teamviewer" ) == 0  ||
              strcmp( category->name, "proxies/teamviewer" ) == 0))
         {
            decision = UFDBglobalTeamviewerOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: teamviewer detected for %s and "
                                  "allow-teamviewer-over-https is off", si->worker, si->url2log );
            }
            break;
         }
         if ((status == UFDB_API_ERR_TUNNEL  ||  status == UFDB_API_ERR_IS_TEAMVIEWER)  &&  
             strcmp( category->name, "proxies" ) == 0)
         {
            decision = aclpass->access;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: tunnel detected for %s and "
                                  "proxies category is blocked", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_ANYDESK  &&   
             (strcmp( category->name, "anydesk" ) == 0  ||
              strcmp( category->name, "proxies/anydesk" ) == 0))
         {
            decision = UFDBglobalAnydeskOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: Anydesk detected for %s and "
                                  "allow-anydesk-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_IS_CITRIXONLINE  &&   
             (strcmp( category->name, "citrixonline" ) == 0  ||
              strcmp( category->name, "extappl/citrixonline" ) == 0))
         {
            decision = UFDBglobalCitrixOnlineOverHttps ? UFDB_ACL_ACCESS_ALLOW : UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_ACL;
               si->aclpass = aclpass;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: Anydesk detected for %s and "
                                  "allow-citrixonline-over-https is off", si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_UNKNOWN_PROTOCOL  &&  
             UFDBglobalUnknownProtocolOverHttps == 0)
         {
            decision = UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_HTTPS_OPTION;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: unknown protocol detected for %s and "
                                  "allow-unknown-protocol-over-https is off", 
                                  si->worker, si->url2log );
            }
            break;
         }

         if (status == UFDB_API_ERR_INVALID_CERT  &&  
             UFDBglobalHttpsOfficialCertificate)
         {
            decision = UFDB_ACL_ACCESS_BLOCK;
            if (decision == UFDB_ACL_ACCESS_BLOCK)
            {
               si->blockReason = UFDB_API_BLOCKR_HTTPS_OPTION;
               if (UFDBglobalDebug)
                  ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: invalid certificate detected for %s and "
                                  "enforce-https-official-certificate is on", 
                                  si->worker, si->url2log );
            }
            break;
         }
      }

      if (decision == UFDB_ACL_ACCESS_BLOCK) 
      {
         if (category != NULL  &&  category->redirect != NULL)
	    redirect = category->redirect;
         else if (category != NULL  &&  category->rewrite != NULL  &&  strcmp( si->protocol, "https" ) != 0  &&
  	          (redirect = sgRewriteExpression( category->rewrite, si->orig, newstring )) != NULL)
         {
  	    ;
         }
         else if (acl->redirect == NULL)
	    redirect = defaultAcl->redirect;
         else
	    redirect = acl->redirect;
         if (redirect == NULL)
         {
#if UFDB_DEBUG_ACL_ACCESS
            /* suppress many errors. we only want it once! */
            ufdbLogError( "!!!!! there is no \"redirect\" rule for ACL %s/%s  *****", 
  		          acl->name, aclpass->name );
#endif
            /* TO-DO: copy the redirect rule for the security or proxy category */
            /* TO-DO: instead of referring to cgibin.urlfilterdb.com */
            /* TO-DO: for HTTPS connections use UFDBglobalRedirectHttps/UFDBglobalRedirectBumpedHttps ***** */
            redirect = (char *) UFDBglobalRedirectClientCategoryURL;
         }
         if (si->port == 443  ||  strcmp( si->protocol, "https" ) == 0)
         {
            if (strcmp( si->method, "CONNECT" ) == 0  ||  !UFDBglobalSquidUsesSSLbump)
               redirect = UFDBglobalRedirectHttps;
            else
               redirect = UFDBglobalRedirectBumpedHttps;
         }
         if (UFDBglobalDebug)
            ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: block; %s %s redirected to %s", 
                            si->worker, si->method, si->url2log, redirect );
      }
   }

   if (redirect != NULL) 
   {
      redirect = sgParseRedirect( redirect, si, acl, aclpass, newredir );
   }
   if (redirect != NULL)
   {
      strcpy( redirectURL, redirect );
      si->aclpass = aclpass;
      si->matchedAny = 0;
      if (UFDBglobalDebug)
         ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus: block;  return BLOCK  redirect: %s", 
                         si->worker, redirectURL );
      return UFDB_ACL_ACCESS_BLOCK;
   }

   *redirectURL = '\0';
#if UFDB_DEBUG_ACL_ACCESS
   if (UFDBglobalDebug)
      ufdbLogMessage( "W%03d: UFDBdecideAccessByPeekStatus  end, return %s", 
                      si->worker,
                      decision == UFDB_ACL_ACCESS_DUNNO ? "DUNNO" : "PASS" );
#endif

   return decision;
}


UFDB_GCC_HOT
int UFDBdecideAccessBasedOnHostname(
   int                   reconfiguring,
   struct Acl *          acl, 
   struct SquidInfo *    si, 
   UFDBthreadAdmin *     admin,
   char *                hostname,
   char *                redirectURL )
{
   struct AclCategory *  ac;
   UFDBrevURL *          revurl;
   int                   decision;

   if (reconfiguring || UFDBglobalReconfig)
      return UFDB_ACL_ACCESS_DUNNO;

   /* the hostname may be a CommonName that starts with "*." */
   if (*hostname == '*'  &&  *(hostname+1) == '.')
      hostname += 2;
   else if (*hostname == 'w'  &&  *(hostname+1) == 'w'  &&  *(hostname+2) == 'w'  &&  *(hostname+3) == '.')
      hostname += 4;

   if (UFDBglobalDebug > 1)
      ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnHostname: possible decision is based on hostname \"%s\"", 
                      si->worker, hostname );

   if (*hostname == '\0')
      return UFDB_ACL_ACCESS_DUNNO;

   decision = UFDB_ACL_ACCESS_DUNNO;
   revurl = UFDBgenRevURL( admin, (unsigned char *) hostname );		/* note: cannot use si->revurl */

   for (ac = acl->pass;  ac != NULL;  ac = ac->next)
   {
      decision = UFDBdecideBasedOnSpecificCategory( acl, ac, si, revurl, redirectURL, 1 );
      if (decision != UFDB_ACL_ACCESS_DUNNO)
      {
	 si->aclpass = ac;
	 break;
      }
   }

   if (decision == UFDB_ACL_ACCESS_DUNNO  &&  !(acl->source != NULL && acl->source->cont_search))
   {
      for (ac = acl->implicitPass;  ac != NULL;  ac = ac->next)
      {
	 decision = UFDBdecideBasedOnSpecificCategory( acl, ac, si, revurl, redirectURL, 0 );
	 if (decision != UFDB_ACL_ACCESS_DUNNO)
	 {
	    si->aclpass = ac;
	    break;
	 }
      }
   }

   if (UFDBglobalDebug > 1)
   {
      if (ac == NULL)
	 ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnHostname: hostname %s did not match any category", 
                         si->worker, hostname );
      else
	 ufdbLogMessage( "W%03d: UFDBdecideAccessBasedOnHostname: hostname %s matched by category %s, decision is %s",
	                 si->worker, hostname, ac->name, decision == UFDB_ACL_ACCESS_BLOCK ? "block" : "allow" );
   }

   UFDBfreeRevURL( admin, revurl );

   return decision;
}

