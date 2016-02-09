#!/bin/bash

#
#Creates Pikalert directories for the requested state.
#Example Usage: ./create_pika_dirs.sh minnesota mn /d2/dicast/rctm/logicast
#

if [ -z "$1" -o -z "$2" ]; then
    echo "Usage: $0 state-name state-abbreviation [logicast-dir]"
    exit 1
fi

if [ -z $VDT_DATA ]; then
    echo "Set the VDT_DATA environment variable before running"
    exit 1
fi

state="$1"
state_abbrev="$2"
logicast_dir="$3"

echo "Creating directories for $state..."

#
#Create general directories and links
#

mkdir -p ${VDT_DATA}
mkdir -p ${VDT_DATA}/derived
mkdir -p ${VDT_DATA}/forecast

if [ -n "$logicast_dir" ]; then
    linkfile=${VDT_DATA}/forecast/merge_cur_tmt
    [ ! -L $linkfile ] && ln -s ${logicast_dir}/merge_cur_tmt/ $linkfile
    linkfile=${VDT_DATA}/forecast/merge_no_tmt 
    [ ! -L $linkfile ] && ln -s ${logicast_dir}/merge_no_tmt/ $linkfile 
    linkfile=${VDT_DATA}/forecast/merge_rec_tmt 
    [ ! -L $linkfile ] && ln -s ${logicast_dir}/merge_rec_tmt/ $linkfile
    linkfile=${VDT_DATA}/forecast/rdwx_fcst 
    [ ! -L $linkfile ] && ln -s ${logicast_dir}/rdwx_fcst/  $linkfile 
fi

mkdir -p ${VDT_DATA}/history
mkdir -p ${VDT_DATA}/lock
mkdir -p ${VDT_DATA}/log
mkdir -p ${VDT_DATA}/processed
mkdir -p ${VDT_DATA}/processed/processed_log
mkdir -p ${VDT_DATA}/processed/rtma_cdf
mkdir -p ${VDT_DATA}/raw
mkdir -p ${VDT_DATA}/raw/metar
mkdir -p ${VDT_DATA}/raw/metar/ascii
mkdir -p ${VDT_DATA}/raw/metar/netcdf
mkdir -p ${VDT_DATA}/raw/radar
mkdir -p ${VDT_DATA}/raw/radar/bref
mkdir -p ${VDT_DATA}/raw/radar/bref/binary
mkdir -p ${VDT_DATA}/raw/radar/bref/binary/mosaic2d
mkdir -p ${VDT_DATA}/raw/radar/bref/netcdf
mkdir -p ${VDT_DATA}/raw/radar/bref/netcdf/mosaic2d
mkdir -p ${VDT_DATA}/raw/rtma
mkdir -p ${VDT_DATA}/static
mkdir -p ${VDT_DATA}/static/cdl
mkdir -p ${VDT_DATA}/static/config
mkdir -p ${VDT_DATA}/static/site_list
mkdir -p ${VDT_DATA}/tmp

#
#Handle state specific directories and links
#

#MN Ingest
mkdir -p ${VDT_DATA}/raw/ameritrak
#CO Ingest
mkdir -p ${VDT_DATA}/raw/wcloud

linkfile=${VDT_DATA}/${state_abbrev}_history 
[ ! -L $linkfile ] && ln -s ${VDT_DATA}/history $linkfile
mkdir -p ${VDT_DATA}/processed/${state}
mkdir -p ${VDT_DATA}/processed/${state}/district_alerts_vdt
mkdir -p ${VDT_DATA}/processed/${state}/latest_vehicles
mkdir -p ${VDT_DATA}/processed/${state}/maw_json
mkdir -p ${VDT_DATA}/processed/${state}/obs_stats_vdt
mkdir -p ${VDT_DATA}/processed/${state}/plot_data_vdt
mkdir -p ${VDT_DATA}/processed/${state_abbrev}_probe_message
mkdir -p ${VDT_DATA}/processed/${state_abbrev}_rwh_output
mkdir -p ${VDT_DATA}/processed/${state_abbrev}_vdt_output
mkdir -p ${VDT_DATA}/derived/${state_abbrev}_time_range

echo "Finished"
