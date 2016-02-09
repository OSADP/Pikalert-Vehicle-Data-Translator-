#!/bin/bash

#A simple wrapper that calls the tocsv.py script with a netcdf file containing road segments

if [ -z "$1" -o -z "$2" ]; then
    echo "USAGE: $(basename $0) ncin csvout"
    exit 1
fi

tocsv.py aux_id,seg_name,elevation,mid_point_latitude,mid_point_longitude s,s,f,d,d $1 $2 
