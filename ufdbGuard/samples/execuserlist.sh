#!/bin/sh
#
# execuserlist.sh - example script by URLfilterDB
# RCS: $Id: execuserlist.sh,v 1.2 2016/09/10 00:36:13 root Exp root $
#
# Example script called by execuserlist to generate a list of users.
# This example uses ldapsearch to generate a list of usernames.
#
# ufdbguardd only parses stdout, so anything sent to stderr is lost.
# The author of the script should decide what to do with stderr.
# It is suggested to send stderr to a logfile and only remove the logfile at
# the end of the script if it is empty.
#
# During initialisation ufdbguardd waits for the completion of scripts defined by 
# "execuserlist" so make sure that the script does not hang or takes a 
# long time to execute.
# ufdbGuard executes the scripts defined by execuserlist every N minutes
# where N is between 5 and 1440. N defaults to 15 and can be set by
# the parameter 'refreshuserlist'.
#
# The configuration file of ufdbguardd may contain source definitions
# like
#    source {
#       execuserlist "/local/ufdbguardd/bin/getgroupmembers-ldap finance"
#    }
# so we can receive parameters in $1, $2, ...
# In the above example $1 has the value "finance".
# So if we receive a parameter we use it in the LDAP search, otherwise we search for OU=people.
LDAP_OU=${1:-people}

# send stderr to a logfile:
LOGFILE=/tmp/execuserlist.stderr.$$.log
exec 2> $LOGFILE

# If necessary, set the PATH
# PATH=/my/path/bin:$PATH
# export PATH

# Set the following parameters to the correct values for your environment:
LDAP_URI="ldap://activedirectory.example.com"
LDAP_SEARCHBASE="ou=$LDAP_OU,dc=example,dc=com"
LDAP_PASSWORD="secret"

# In very large environments ldapsearch may get a "Size limit exceeded" error and
# needs the following parameter:
LDAP_QUERYPARAMS="-E pr=1000/noprompt"

# Here the magic is done:
# =======================
# Connect to the LDAP server and query its database for uid's and use a filter for OU.
# See also "man ldapsearch".
# The sed command is to strip the output and only have the uid printed.
# The final output is a list of usernames, one username per line.
ldapsearch -LLL $LDAP_QUERYPARAMS -H "$LDAP_URI" -b "$LDAP_SEARCHBASE" -x -w "$LDAP_PASSWORD" "uid"  |   \
sed -e 's,^uid: ,,'

# Alternatively, one can use the following example for ldapsearch :
# LDAP_URI="ldap://dc1.com"
# LDAP_SEARCHBASE="DC=dc1,DC=com"
# LDAP_FILTER="(&(objectCategory=Person)(objectClass=user) ((memberof:1.2.840.113556.1.4.1941:=CN=$LDAP_OU,OU=Users,$LDAP_SEARCHBASE))"
# LDAP_USER="CN=userldap,OU=Users,DC=dc1,DC=com"
# LDAP_PASSWORD="123"
# ldapsearch -LLL $LDAP_QUERYPARAMS -H "$LDAP_URI" -b "$LDAP_SEARCHBASE" -x -D "$LDAP_USER" -w "$LDAP_PASSWORD" "$LDAP_FILTER" |
#    grep "sAMAccountName:" |
#    cut -d" " -f2



# remove the logfile if it is empty, otherwise send a comment about it to ufdbguardd
if test -s $LOGFILE
then
   echo "# logfile $LOGFILE has content and is not removed"
else
   rm -f $LOGFILE
fi

exit 0

