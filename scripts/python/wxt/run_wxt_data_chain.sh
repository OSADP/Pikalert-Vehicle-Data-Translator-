#!/bin/bash

#A simple runner for wxt_data_chain.py

#set -x

if [ -z "$1" ];
then
    echo "Usage: $0 logfile"
    exit 1
fi

outbase=/d2/vii/data/wxt_probe_data/$(date +%Y%m%d)
begindate=$(date -u --date="-2 minutes" +%Y%m%d%H%M00)
enddate=$(date -u --date="-1 minutes" +%Y%m%d%H%M00)

if [ ! -d "$outbase" ];
then
    mkdir $outbase
fi
  
wxt_data_chain.py -l "$1" $begindate $enddate $outbase
