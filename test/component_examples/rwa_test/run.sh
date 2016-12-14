#!/bin/bash

set -x

gunzip input/merge_rec_tmt.20161211.2349.nc
./rwa -s input/segment_statistics.20161212.0005.nc -l log/rwa_mi -t input/merge_rec_tmt.20161211.2349.nc 20161212.0005 input/mi_rwa.cfg input/MI_logicast_road_sites_extended.asc input/mi_rwh.20161212.0005.nc output/district_alerts.20161212.0005.json output/plot_data.20161212.0005 output/obs_stats_data.20161212.0005.json output/maw.20161212.0005.json
gzip input/merge_rec_tmt.20161211.2349.nc
