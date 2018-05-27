#!/usr/bin/sh
#
# hashtest.sh

PATH=.:$PATH
export PATH

let s1=0
while [ $s1 -le 10 ]
do
   let s2=0
   while [ $s2 -le 255 ]
   do
      ufdbgclient -p 3978 -l . https://192.168.$s1.$s2/foo  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . http://192.168.$s1.$s2/foo  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . https://$s1.$s2.snif.ccc/bar  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . http://$s1.$s2.snif.ccc/bar  10.1.1.1 johndoe > /dev/null &
      let s2=s2+1
      wait
   done
   let s1=s1+1
done

let s1=0
while [ $s1 -le 20 ]
do
   let s2=0
   while [ $s2 -le 255 ]
   do
      ufdbgclient -p 3978 -l .         192.168.$s1.$s2:443  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . https://192.168.$s1.$s2/bar  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . http://192.168.$s1.$s2/bar  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . https://$s1.$s2.snif.ccc/bar  10.1.1.1 johndoe > /dev/null &
      ufdbgclient -p 3978 -l . http://$s1.$s2.snif.ccc/bar  10.1.1.1 johndoe > /dev/null &
      let s2=s2+1
      wait
   done
   let s1=s1+1
done
