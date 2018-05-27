#!/bin/sh
#
# makedist.sh

# make ufdbGuard-<PACKAGE_VERSION>.tar.gz from directory ufdbGuard-<VERSION>

shortversion="$1"
longversion="$2"

shortname="ufdbGuard-$shortversion"
longname="ufdbGuard-$longversion"

cd ..

if [ ! -h "$longname"  -a  ! -d "$longname" ]
then
   ln -s $shortname $longname
fi

exclusions=/var/tmp/exclusions.$$

sed -e "s,VERSION,$longversion," < $longname/dist-exclusions > $exclusions

find $shortname -name Makefile.in -exec touch --date=yesterday {} \;

tar -h -X $exclusions -cf - $longname | gzip --best > $longname.tar.gz

if [ $shortname != $longname ]
then
   rm $longname
fi

rm $exclusions

exit 0

