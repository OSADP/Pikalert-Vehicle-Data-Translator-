#!/bin/bash

#Performs a reprojection of a shapefile to geographic coordinates

if [ -z $1 ]; then
    echo "USAGE $0 shapefilename"
    exit 1
fi

ogr2ogr -t_srs EPSG:4326 GEO_$1 $1
