#!/bin/bash

#Copies files up to $minutesago old from base dir to dest dir
#and then renames them using a passed in Perl regex

#Example Usage:
#copy_rename_wxt.sh /var/autofs/mnt/motown_d2/vii/data/processed/mi_csv2nc /d2/vii/data/processed/mi_csv2nc 16 's/(wxt\.probe\.)(\d\d\d\d)(\d\d)(\d\d)(\d\d\d\d\d\d)(\.nc)/$1mi_$3$4$2_$5$6/' >& /dev/null

if [ -z "$1" -o -z "$2" -o -z "$3" -o -z "$4" ];
then
    echo "Usage: $0 srcbase destbase minutesago renameregex"
    exit 1
fi

#set -x

srcdir="$1"
destdir="$2"
minutesago="$3"
regex="$4"
today=$(date +%Y%m%d)

files=$(find $srcdir/$today -type f -cmin -$minutesago)
for f in $files;
do
    #echo "Processing $f"
    mkdir -p $destdir/$today
    cp $f $destdir/$today
    #Use v flag to test the renaming
    rename -f $regex $destdir/$today/$(basename $f) 
done
