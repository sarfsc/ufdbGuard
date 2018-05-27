/*
 * ipv6 address translation tests.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void UFDBprintIPv6( 
   char * str,
   struct in6_addr * ipv6 )
{
   printf( "%s  %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n", 
           str, 
           ipv6->s6_addr[0] * 256 + ipv6->s6_addr[1], ipv6->s6_addr[2] * 256 + ipv6->s6_addr[3], 
           ipv6->s6_addr[4] * 256 + ipv6->s6_addr[5], ipv6->s6_addr[6] * 256 + ipv6->s6_addr[7],
           ipv6->s6_addr[8] * 256 + ipv6->s6_addr[9], ipv6->s6_addr[10] * 256 + ipv6->s6_addr[11],
           ipv6->s6_addr[12] * 256 + ipv6->s6_addr[13], ipv6->s6_addr[14] * 256 + ipv6->s6_addr[15] 
           );
   
}
  

int ufdbIPv6hostInNet( struct in6_addr * host, struct in6_addr * net, int cidr )
{
   int     i;
   uint8_t mask;

#if 0
   if (cidr == 128)
   {
      return (host->s6_addr == net->s6_addr);
   }
#endif

   /* cidr is max 128, so we could unroll the next loop */
   i = 0;
   while (cidr >= 8)
   {
      printf( "host[%d] %02x  net[%d] %02x\n", i, host->s6_addr[i], i, net->s6_addr[i] );
      if (host->s6_addr[i] != net->s6_addr[i])
      {
         printf( "no match, return 0\n" );
         return 0;
      }
      cidr -= 8;
      i++;
   }

   if (cidr == 0)
   {
      printf( "%d bits were equal. return 1\n", i*8 );
      return 1;
   }

   mask = ((uint8_t) 0xFF) << (8 - cidr);
   printf( "last host[%d] %02x  net[%d] %02x  mask %02x\n", i, host->s6_addr[i], i, net->s6_addr[i], mask );

   if ((host->s6_addr[i] & mask) == (net->s6_addr[i] & mask))
   {
      printf( "%d+%d bits were equal. return 1\n", i*8, cidr );
      return 1;
   }

   printf( "no match in last %d bits.  return 0\n", cidr );
   return 0;
}


int main( int argc, char * argv[] )
{
   int          cidr;
   char *       addr;
   int          retval;
   const char * rewritten;
   char         newaddr[INET6_ADDRSTRLEN];
   struct in6_addr ipv6;
   struct in6_addr ipv6net;

   if (argc <= 1)
      exit( 2 );

   addr = argv[1];
   retval = inet_pton( AF_INET6, addr, (void *) &ipv6 );
   if (retval <= 0)
   {
      printf( "ipv6: \"%s\" cannot be converted to an IPv6 address\n", addr );
      return 1;
   }
   printf( "host: " );
   UFDBprintIPv6( addr, &ipv6 );

   rewritten = inet_ntop( AF_INET6, (void *) &ipv6, newaddr, INET6_ADDRSTRLEN );
   if (rewritten == NULL)
   {
      printf( "ipv6: problem with converting IPv6 struct into string: %s", strerror(errno) );
      return 1;
   }

   printf( "host: \"%s\" is normalised into \"%s\"\n", addr, newaddr );

   if (argc >= 3)
   {
      /* 
       * argv[1] = host address
       * argv[2] = network address
       * argv[3] = CIDR  range (1-128)
       * TEST if host address is inside the "network address/CIDR" range
       */
      addr = argv[2];
      retval = inet_pton( AF_INET6, addr, (void *) &ipv6net );
      if (retval <= 0)
      {
         printf( "\"%s\" cannot be converted to an IPv6 network address\n", addr );
         return 1;
      }
      printf( "net:  " );
      UFDBprintIPv6( addr, &ipv6net );

      cidr = atoi( argv[3] );
      if (cidr > 128 || cidr < 1)
      {
         printf( "cidr is %d\n", cidr );
         printf( "usage: ipv6 <hostaddr> <netaddr> <cidr>\n" );
         exit( 1 );
      }

      ufdbIPv6hostInNet( &ipv6, &ipv6net, cidr );
   }

   return 0;
}
