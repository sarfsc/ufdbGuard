#!/bin/sh
#
# install_log.sh
#
# $Id: install_log.sh,v 1.3 2015/06/09 01:26:21 root Exp root $

RUNAS=${1:-undefined}
PREFIX=${2:-undefined}
BINDIR=${3:-undefined}
LOGDIR=${4:-undefined}
CFGDIR=${5:-undefined}
IMAGESDIR=${6:-undefined}
DBHOME=${7:-undefined}
SYSCFGFILE=${8:-undefined}

echo >> install.log
date >> install.log

echo
   (
   echo "ufdbGuard is installed."

   echo "The installation parameters are:"
   echo "run with user id: $RUNAS"
   echo "installation directory prefix: $PREFIX"
   echo "directory for binaries: $BINDIR"
   echo "directory for logs: $LOGDIR"
   echo "directory for configuration files: $CFGDIR"
   echo "system configuration file: $SYSCFGFILE"
   echo "directory for image files: $IMAGESDIR"
   echo "directory for database files: $DBHOME"
   ) | tee -a install.log
echo

