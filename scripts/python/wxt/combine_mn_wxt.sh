#!/bin/bash

#Combines probe messages with WXT data in preparation for running the VDT 
#Currently called from the crontab

#set -x

today=$(date -u --date="-5 minutes" +%Y%m%d)
catfile=probe_message.$(date -u --date="-5 minutes" +%Y%m%d.%H%M).nc
logfile='/d2/vii/data/log/group_combined_probe'
dstdir=$VII_DATA/processed/mn_combined_probe/${today}
srcdir=$VII_DATA/processed/mn_component_probe/${today}

#Sleep to allow the motown data to appear before trying to grab it
sleep 15

copy_rename_wxt.py -l /d2/vii/data/log/copy_rename wxt.probe. /var/autofs/mnt/motown_d2/vii/data/processed/mn_wxt_probe /d2/vii/data/processed/mn_component_probe "(wxt\.probe\.\d{14}\.nc)" "mn_\1" '%Y%m%d%H%M00'  5 5
copy_rename_wxt.py -l /d2/vii/data/log/copy_rename wxt.probe. /var/autofs/mnt/motown_d2/vii/data/processed/mn_wxt_probe /d2/vii/data/processed/mn_component_probe "(wxt\.probe\.\d{14}\.nc)" "mn_\1" '%Y%m%d%H%M00' 10 5
copy_rename_wxt.py -l /d2/vii/data/log/copy_rename wxt.probe. /var/autofs/mnt/motown_d2/vii/data/processed/mn_wxt_probe /d2/vii/data/processed/mn_component_probe "(wxt\.probe\.\d{14}\.nc)" "mn_\1" '%Y%m%d%H%M00' 15 5

copy_rename_wxt.py -l /d2/vii/data/log/copy_rename probe_message. /d2/vii/data/processed/mn_probe_message /d2/vii/data/processed/mn_component_probe "(probe_message\.\d{8}\.\d{4}\.nc)" "mn_\1" '%Y%m%d.%H%M' 5 5

if [ ! -d "$dstdir" ]; then
	mkdir $dstdir
fi

if [ ! "$(ls -A $srcdir)" ]; then
	python -c "import log_msg; log_msg.LogMessage('"$logfile"').write_info('${srcdir} is empty. Exiting')"
	exit 1
fi

python -c "import log_msg; log_msg.LogMessage('"$logfile"').write_info('Running ncrcat ${srcdir}/*.nc ${dstdir}/$catfile')"
ncrcat ${srcdir}/*.nc ${dstdir}/$catfile
#run_group_probe_message_rt.sh $VII_DATA/processed/mn_component_probe $VII_DATA/processed/mn_combined_probe probe_message 5 300 /d2/vii/data/log/group_combined_probe

python -c "import log_msg; log_msg.LogMessage('"$logfile"').write_info('Running rm -f ${srcdir}/mn_*.nc')"
rm -f ${srcdir}/mn_*.nc
