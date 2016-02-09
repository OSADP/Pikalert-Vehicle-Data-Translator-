#!/bin/bash

#Reproject to WGS84

ogr2ogr -t_srs EPSG:4326 ./SHP/STATEWIDE/HIGHWAYS_GEO.shp ./SHP/STATEWIDE/HIGHWAYS.shp
