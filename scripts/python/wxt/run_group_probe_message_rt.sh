#!/bin/bash

#A simple runner for group_probe_message_files.py

#set -x

if [ -z "$1" -o -z "$2" -o -z "$3" -o -z "$4" -o -z "$5" -o -z "$6" ];
then
    echo "Usage: $0 indir outdir nameschema minutesago groupsec logfile"
    exit 1
fi

indir="$1"
outdir="$2"
nameschema="$3"
minago="$4"
timeinterval="$5"
logfile="$6"
#timeago and timeinterval should typically be changed in concert
timeago=$(date -u --date="-"$minago" minutes" +%Y%m%d%H%M00)
cdlfile=/d2/vii/data/static/cdl/probe_message.cdl
config=vii_paths.py
workdir=$VII_DATA/tmp

group_probe_message_files.py -l $logfile -f $timeago $config $cdlfile $workdir $indir $outdir $timeinterval $nameschema