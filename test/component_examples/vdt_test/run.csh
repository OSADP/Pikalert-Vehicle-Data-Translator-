#!/bin/csh

gunzip input/madis.20161211.2300.nc
./vdt45 -s -l log/vdt4_mn -m input/metar.20161211.2355.nc -h input/HHC.20161211.2355.nc -w input/madis.20161211.2300.nc -p input/COMP_REFL.2d.merged.20161211.2356.nc -r input/BASE_REFL.2d.merged.20161211.2356.nc -H input/history_probe_msg_qc.20161211.2250.60.nc -g output/grid_cell_statistics.20161211.2355.nc input/vdt_mn_config_new.cfg 201612112355 201612120000 input/mn_roads.nc input/probe_message.20161211.2355.nc output/vdt.20161211.2355.nc output/segment_statistics.20161211.2355.nc
gzip input/madis.20161211.2300.nc
