#!/bin/bash

#Script to convert MRMS radar data from grib2 to netcdf

#set -x

if [ -z "$1" -o -z "$2" ];
then
	echo "Usage: $0 srcgrib2 dstnetcdf"
	exit 1
fi

logfile=~/logs/bref_to_nc.log

echo $(date +%Y-%m-%dT%H:%M:%S)" Executing: $0 $*" >> $logfile
cdfdir=$(dirname $2)
cdfbase=$(basename $2)
hiddencdf=${cdfdir}/.${cdfbase} 

gzip -df $1
mkdir -p $cdfdir
wgrib2 ${1%.gz} -netcdf $hiddencdf
#Rename variable to something more readable 
ncrename -v MergedBaseReflectivityQC_0mabovemeansealevel,MergedBaseReflectivityQC $hiddencdf
#IMPORTANT: Apply compression
nccopy -d1 $hiddencdf $2 
rm $hiddencdf

echo $(date +%Y-%m-%dT%H:%M:%S)" Exiting: status = $?" >> $logfile
