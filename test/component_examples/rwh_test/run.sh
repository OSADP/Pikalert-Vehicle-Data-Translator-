#!/bin/bash

set -x

gunzip input/rdwx_fcst.20161212.1936.nc
gunzip input/merge_rec_tmt.20161212.1948.nc
./rwh input/rwh_config_new.cfg 201612121950 201612151900 input/MI_logicast_road_sites.asc input/mi_roads.nc output/mi_rwh.20161212.1940.nc -l log/run_rwh_mi -w input/rdwx_fcst.20161212.1936.nc -r input/merge_rec_tmt.20161212.1948.nc -s input/segment_statistics.20161212.1940.nc -p input/previous_rwh_conf.20161212.1940.asc
gzip input/merge_rec_tmt.20161212.1948.nc
gzip input/rdwx_fcst.20161212.1936.nc
