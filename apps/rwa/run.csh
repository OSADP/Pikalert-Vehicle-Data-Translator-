#!/bin/csh

#./rwa 20131106.0620 rwa.cfg MN_logicast_road_sites.asc mn_rwh.20131106.0620.nc out.json

#./rwa 20131106.0620 -t merge_rec_tmt.20131106.0636.nc rwa.cfg MN_logicast_road_sites.asc mn_rwh.20131106.0620.nc rdwx_out.json treatment_out.json

#./rwa 20131121.1750 rwa.cfg MN_logicast_road_sites.asc mn_rwh.20131121.1750.nc rdwx_out.json treatment_out.json

#./rwa -t merge_rec_tmt.20131122.2140.nc 20131122.2155 mn_rwa.cfg MN_logicast_road_sites.asc mn_rwh.20131122.2155.nc district_alerts.20131122.2155.json plot_data.20131122.2155 obs_stats_data.20131122.2155

./rwa -s segment_statistics.20140114.2045.nc -t merge_rec_tmt.20140114.2019.nc 20140114.2045 mn_rwa.cfg MN_logicast_road_sites.asc mn_rwh.20140114.2045.nc district_alerts.20140114.2045.json plot_data.20140114.2045 obs_stats_data.20140114.2045 maw_output.json

