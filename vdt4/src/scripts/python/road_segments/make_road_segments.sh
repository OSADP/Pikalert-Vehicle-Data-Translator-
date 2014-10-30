#!/bin/bash

#A simple script to create road segments from a shapefile and csv list of RWIS site info
#USAGE: ./make_road_segments.sh

DEBUG=
SHAPEIN=./nhpn/minnesota/NHPN_STFIPS_27.shp
SEGLEN=1.6
STATE=MN
TEMPLATE=state_roads.cdl
RWIS_SITES=MN_rwis_sites.asc

if [ -z $DEBUG ]; then
	python road_segments.py $SHAPEIN $SEGLEN $STATE $TEMPLATE out.nc > road_segments_debug.out
	outf=$(find . -name "out.*.nc" -mmin -1)
	echo "Road segment file $outf has been created. Adding RWIS site data."
	python add_rwis_road_seg.py $outf $RWIS_SITES $TEMPLATE
	echo "Generating ${outf}.cdl"
	ncdump $outf > ${outf}.cdl
else
	#The pudb module must be available in the PYTHONPATH
	python -m pudb.run road_segments.py $SHAPEIN $SEGLEN $STATE $TEMPLATE out.nc 
	outf=$(find . -name "out.*.nc" -mmin -1)
	echo "Road segment file $outf has been created. Adding RWIS site data."
	python add_rwis_road_seg.py $outf $RWIS_SITES $TEMPLATE
	echo "Generating ${outf}.cdl"
	#Dump to cdl with limited precision for diff purposes
	ncdump -d 7,7 $outf > ${outf}.cdl
fi

