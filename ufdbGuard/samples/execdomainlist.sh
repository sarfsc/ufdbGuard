#!/bin/sh
#
# execdomainlist.sh - example script by URLfilterDB
# RCS: $Id: execdomainlist.sh,v 1.1 2016/09/10 00:36:50 root Exp root $
#
# Example script called by execdomainlist to generate a URL category table.
# This example uses ad dummy command to generate a list of URLs.
#
# The feauture 'execdomainlist' is meant to be used for user-defined
# URL tables that may change frequently.  With execdomainlist 
# ufdbguardd only reloads user-defined URL tables and only if
# the .ufdb file has a new timestamp.
# So the recommended behaviour of the script called by execdomainlist, is:
# 1. generate new files 'newdomains' and 'newurls'.
# 2. exit if the contents of newdomains and domains are equal, and the
#    contents of newurls and urls is equal (the content did not change)
#    Consequently, ufdbguardd will not reload the .ufdb file
# 3. call ufdbGenTable to generate a new .ufdb file
#    ufdbguardd will reload the new .ufdb file because it has a new
#    timestamp.
# 4. exit
#
# ufdbguardd only catches stdout and writes it to its log file,
# so anything sent to stderr is lost.
# The author of the script should decide what to do with stderr.
# It is suggested to redirect stderr to stdout so it is logged.
#
# During initialisation ufdbguardd waits for the completion of scripts defined by 
# "execdomainlist" so make sure that the script does not hang or takes a 
# long time to execute.
# ufdbGuard executes the scripts defined by execdomainlist every N minutes
# where N is between 5 and 1440. N defaults to 15 and can be set by
# the parameter 'refreshdomainlist'.
#
# The configuration file of ufdbguardd may contain URL category definitions
# like
#    category {
#       domainlist     "always-allow/domains"
#       execdomainlist "/local/ufdbguardd/bin/generate-always-allow.sh foo bar"
#    }
# so we can receive parameters in $1, $2, ...
# In the above example $1 has the value "foo".

# redirect stderr to stdout:
exec 2>&1

# If necessary, set the PATH
# PATH=/my/path/bin:$PATH
# export PATH

# Substitute the following 12 lines by your code.
   # Generate a new list of domains.
   (
      echo "foo.com" 
      echo "bar.com"
      echo "alpha.example.com"
      echo "beta.example.com"
   ) > newdomains

   # Generate a new list of URLs.
   (
      echo "www.example.com/foobar/" > newurls
   ) > newurls


# NOTE: if there is one source for domains and urls, one may
# separate the domains from the urls as follows:
# (assume that a mix of domains and urls are in file 'newlist')
# grep -e / newlist > newurls
# grep -v -e / newlist > newdomains


# following is generic code that runs ufdbGenTable only whenever necessary
MUST_UPDATE=no

# on the very first run, the 'domains' file may not exist...
if test ! -f domains
then
   cp -p newdomains domains
fi

# on the very first run, the optional 'urls' file may not exist...
if test ! -f urls 
then
   if test -f newurls
   then
      cp -p newurls urls
   fi
fi

cmp -s domains newdomains
exitval=$?
if test $exitval -eq 1
then
   MUST_UPDATE=yes
else
   cmp -s urls newurls
   exitval=$?
   if test $exitval -eq 1
   then
      MUST_UPDATE=yes
   fi
fi


if test $MUST_UPDATE = yes
then
   OURCWD=`pwd`
   echo "The URL tables in $OURCWD have been updated so will be running ufdbGenTable now"
   mv newdomains domains
   if -f newurls
   then
      mv newurls urls
      ufdbGenTable -n -W -d domains -f urls
   else
      ufdbGenTable -n -W -d domains
   fi
else
   rm -f newdomains newurls
fi 

exit 0

