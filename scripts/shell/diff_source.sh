#!/bin/bash

#Provides some rudimentary diffing of files changed in the last n days in the local dir with those in source control

#MODIFY PARAMS DEFAULTS HERE
sourceroot=~/github/pikalert/
filter="*"
daysback=1

while getopts "f:d:h" opt
do
  case $opt in
     f) filter=$OPTARG;;
     d) daysback=$OPTARG;;
     h) echo "$0 [-h] [-f "filefilter"] [-d daysback]"
        exit 1
  esac
done

for f in $(find . -type f -name "$filter" -mtime -${daysback}) 
do 
   filebase=$(basename $f)
   sf=$(find $sourceroot -name $filebase)

   if [ -z "$sf" ];
   then
 	echo -e "** $f not found in $sourceroot" 
   else
   	echo "diff $f $sf"
   	diff $f $sf
   fi

   echo
done
