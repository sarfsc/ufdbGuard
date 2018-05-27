#!/bin/sh

while read url rest 
do
   ../ufdbpeek -C /local/squid/blacklists/security/cacerts2 $url
   echo
done < https.in.500

