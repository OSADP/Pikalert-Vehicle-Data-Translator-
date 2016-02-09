#!/bin/bash

#Performs some basic field querying in a directory of netcdf files.
#Example: ./query_nc_dir.sh /d2/vii/data/processed/mn_component_probe/20141218 vehicle_id 204523

#NOTE: This uses ncdump so it might be quite slow if the files in the directory are large.

if [ -z "$1" -o -z "$2" -o -z "$3" ];then
    echo "USAGE: $0 dir fieldname key"
    exit 1
fi

dir="$1"
shift
fieldname="$1"
shift
key="$1"

for f in ${dir}/*.nc; do
    result=$(ncdump -fc -v $fieldname $f | grep --line-number $key)
    if [ -n "$result" ]; then
	echo $f
	echo -e "$result"
    fi
done 
