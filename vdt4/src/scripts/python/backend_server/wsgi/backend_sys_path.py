#!/usr/bin/python

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: backend_sys_path.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/07/03 17:02:03 $           #
#                                                                                #
# ============================================================================== #

"""backend_sys_path.py - the main configuration file for the backend system"""

import os
import tz

# general system directories
# note that these directories are created
# by running this script standalone
cdl_dir = "/d1/mdss_view/cdl"
config_dir = "/d1/mdss_view/config"
log_dir = "/d1/mdss_view/log"
fhwa_processed_dir = "/d1/dicast"
fhwa_vdt_processed_dir = "/var/autofs/mnt/sentinel_d2_vii/data/processed"
phone_log_dir = "/d1/vii/data/phone_log"

# log file base names
adhoc_phone_log = "adhoc_phone_log"
phone_log = "phone_log"
create_district_alerts_log = "create_district_alerts"
district_alerts_log = "district_alerts_wsgi"
latest_vehicles_log = "latest_vehicles_wsgi"
plots_log = "plots_wsgi"
data_time_log = "data_time"

# base names for input files from forecast system
rdwx_base_name = "rdwx_fcst"
site_list_base_name = "site_list"
tmt_base_name = "merge_rec_tmt"

# forecast directories that should exist prior to running vdt system 
alaska_road_sites_dir = "/d1/dicast/rctm/static_data/%s" % site_list_base_name
fhwa_dicast_dir = "/d2/dicast/rctm"
fhwa_dicast_tmt_dir = os.path.join(fhwa_dicast_dir, tmt_base_name)
fhwa_logicast_dir = "/d2/dicast/rctm/logicast"
fhwa_logicast_tmt_dir = os.path.join(fhwa_logicast_dir, tmt_base_name)
fhwa_mesonet_dir = "/var/autofs/mnt/d2_ldm/data/dec_data/obs/mesonet"
fhwa_rdwx_dicast_dir = os.path.join(fhwa_dicast_dir, rdwx_base_name)
fhwa_rdwx_logicast_dir = os.path.join(fhwa_logicast_dir, rdwx_base_name)
fhwa_road_sites_dir = "/d2/dicast/rctm/static_data/%s" % site_list_base_name
fhwa_sites_dir = "/d2/dicast/nt/static_data/%s" % site_list_base_name
fhwa_wx_obs_dir = "/d2/dicast/nt/der_data/obs/int_obs"

# degree radius for determining weather impact area around vehicle for MAW phone app
degree_radius = 0.05
#degree_radius = 1.0

# state data class
# this class contains information for each state
class StateData(object):

    def __init__(self, state, all_sites_file, road_cond_sites_file, wx_obs_sites_file, rwis_sites_file, rwis_sites_var_file, rdwx_dir, rdwx_base_name, tmt_dir, tmt_base_name, wx_obs_dir, mesonet_dir, districts, product_dictionary, met_vars, output_dir, district_alerts_dir, latest_vehicles_dir, plots_dir, rwh_dir, obs_stats_dir, obs_stats_json_dir, maw_json_dir, maw_nearest_neighbor_files, road_segments_file, timezone, radius):

        self.state = state
        self.all_sites_file = all_sites_file
        self.road_cond_sites_file = road_cond_sites_file
        self.wx_obs_sites_file = wx_obs_sites_file
        self.rwis_sites_file = rwis_sites_file
        self.rwis_sites_var_file = rwis_sites_var_file
        self.rdwx_dir = rdwx_dir
        self.rdwx_base_name = rdwx_base_name
        self.tmt_dir = tmt_dir
        self.tmt_base_name = tmt_base_name
        self.wx_obs_dir = wx_obs_dir
        self.mesonet_dir = mesonet_dir
        self.districts = districts
        self.product_dictionary = product_dictionary
        self.met_vars = met_vars
        self.output_dir = output_dir
        self.district_alerts_dir = district_alerts_dir
        self.latest_vehicles_dir = latest_vehicles_dir
        self.plots_dir = plots_dir
        self.rwh_dir = rwh_dir
        self.obs_stats_dir = obs_stats_dir
        self.obs_stats_json_dir = obs_stats_json_dir
        self.maw_json_dir = maw_json_dir
        self.maw_nearest_neighbor_files = maw_nearest_neighbor_files
        self.road_segments_file = road_segments_file
        self.timezone = timezone
        self.radius = radius
        for district in self.districts:
            if district["district_name"].lower() == self.state:
                self.state_district = district
            else:
                self.state_district = None

    def inside(self, lat, lon):
        """Check whether given lat, lon is in state"""
        if self.state_district != None:
            min_lat = self.state_district["min_lat"] 
            min_lon = self.state_district["min_lon"] 
            max_lat = self.state_district["max_lat"] 
            max_lon = self.state_district["max_lon"] 
            if min_lat <= lat and lat <= max_lat:
                if min_lon <= lon and lon <= max_lon:
                    return True
        else:
            return False
        

# meteorological variable names

class MetVars(object):
    def __init__(self):
        self.rdwx_temp_var = "TempF"
        self.rdwx_dewp_var = "dewptF"
        self.rdwx_snow_rate_var = "snow_rate_inches"
        self.rdwx_snow_accum_var = "snow_accum_inches"
        self.rdwx_precip_accum_var = "precip_accum_inches"
        self.rdwx_precip_type = "precip_type"
        self.rdwx_wind_spd_var = "wind_speed_mph"
        self.rdwx_vis_var = "visibility"
        self.tmt_road_temp_var = "road_TempF"
        self.wx_temp_var = "T"
        self.wx_dewp_var = "dewpt"
        self.wx_wind_spd_var = "wind_speed"

Met_vars = MetVars()        

State_dictionary = {}

# Alaska
State = "alaska"
Districts = [{"district_name" : "Fairbanks", "min_lat" : 64, "min_lon" : -149, "max_lat" : 66, "max_lon" : -147}, {"district_name" : "Anchorage", "min_lat" : 61, "min_lon" : -152, "max_lat" : 62, "max_lon" : -148}, {"district_name" : "Juneau", "min_lat" : 56, "min_lon" : -136, "max_lat" : 61, "max_lon" : -132}, {"district_name" : "Alaska", "min_lat" : 52, "min_lon" : -170, "max_lat" : 72, "max_lon" : -130}]

All_sites_file = "/d1/dicast/nt/static_data/site_list/non_NDFD_full_list.asc"
Road_cond_sites_file = os.path.join(alaska_road_sites_dir, "road_cond_sites.asc")
Wx_obs_sites_file = "/d1/dicast/nt/static_data/site_list/AK_metar_sites.asc"
Rwis_sites_file = os.path.join(alaska_road_sites_dir, "AK_all_madis_rwis.asc")
Rwis_sites_var_file = os.path.join(alaska_road_sites_dir, "AK_all_madis_rwis_site_var_file.asc")
Rdwx_dir = "/d1/dicast/rctm/%s" % rdwx_base_name
Tmt_dir = "/d1/dicast/rctm/%s" % tmt_base_name
Wx_obs_dir = "/d1/dicast/nt/der_data/obs/int_obs"
Mesonet_dir = "/d1/dicast/nt/dec_data/obs/mesonet"
Alaska_maw_json_dir = os.path.join(fhwa_processed_dir, "alaska/maw_json")
Alaska_output_dir = os.path.join(fhwa_processed_dir, "alaska")
Alaska_district_alerts_dir = os.path.join(fhwa_processed_dir, "alaska/district_alerts")
Alaska_latest_vehicles_dir = os.path.join(fhwa_vdt_processed_dir, "alaska/latest_vehicles")
Alaska_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "alaska_vdt_output")
Alaska_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "alaska/obs_stats_vdt")
Alaska_nearest_neighbor_files = []
Alaska_plots_dir = os.path.join(fhwa_processed_dir, "alaska/plot_data")
Alaska_road_segments_file = ""
Alaska_rwh_dir = os.path.join(fhwa_processed_dir, "alaska/alaska_rwh_output")
Alaska_timezone = tz.Alaska
Alaska_radius = 0.01

Product_dictionary = {"road_wx_dir" : Rdwx_dir, "rec_treatment" : Tmt_dir, "district_alerts" : Alaska_district_alerts_dir, "latest_vehicles" : Alaska_latest_vehicles_dir}

Alaska = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, Rdwx_dir, rdwx_base_name, Tmt_dir, tmt_base_name, Wx_obs_dir, Mesonet_dir, Districts, Product_dictionary, Met_vars, Alaska_output_dir, Alaska_district_alerts_dir, Alaska_latest_vehicles_dir, Alaska_plots_dir, Alaska_rwh_dir, Alaska_obs_stats_dir, Alaska_obs_stats_json_dir, Alaska_maw_json_dir, Alaska_nearest_neighbor_files, Alaska_road_segments_file, Alaska_timezone, Alaska_radius)
State_dictionary[State] = Alaska

# Michigan (deprecated version)
State = "michigan"
Districts = [{"district_name" : "Michigan", "min_lat" : 40.4, "min_lon" : -91.4, "max_lat" : 48.7, "max_lon" : -80.7}]
All_sites_file = os.path.join(fhwa_sites_dir, "MI_full_list.asc")
Road_cond_sites_file = "/d2/dicast/rctm/static_data/site_list/MI_road_cond_sites.asc"
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "MI_metar_sites.asc")
Rwis_sites_file = os.path.join(fhwa_road_sites_dir, "MI_rwis_sites.asc")
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "MI_rwis_site_var_file.asc")

Michigan_nearest_neighbor_files = [os.path.join(config_dir, "mi_flann_index.dat"), os.path.join(config_dir, "mi_numpy_seg_pts.npy"), os.path.join(config_dir, "mi_numpy_seg_id.npy")]
Michigan_output_dir = os.path.join(fhwa_processed_dir, "michigan")
Michigan_district_alerts_dir = os.path.join(fhwa_processed_dir, "michigan/district_alerts")
Michigan_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "michigan/latest_vehicles")
Michigan_maw_json_dir = os.path.join(fhwa_processed_dir, "michigan/maw_json")
Michigan_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "mi_vdt_output")
Michigan_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "michigan/obs_stats_vdt")
Michigan_plots_dir = os.path.join(fhwa_processed_dir, "michigan/plot_data")
Michigan_road_segments_file = os.path.join(cdl_dir, "mi_roads.20140613.nc")
print Michigan_road_segments_file
Michigan_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "mi_rwh_output")
Michigan_vdt_output_dir = os.path.join(fhwa_vdt_processed_dir, "mi_vdt_output")
Michigan_timezone = tz.Eastern
Michigan_radius = 7e-8

Product_dictionary = {"road_wx_dir" : fhwa_rdwx_dicast_dir, "rec_treatment" : fhwa_dicast_tmt_dir, "district_alerts" : Michigan_district_alerts_dir, "latest_vehicles" : Michigan_latest_vehicles_dir}

Michigan = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_dicast_dir, rdwx_base_name, fhwa_dicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary, Met_vars, Michigan_output_dir, Michigan_district_alerts_dir, Michigan_latest_vehicles_dir, Michigan_plots_dir, Michigan_rwh_dir, Michigan_obs_stats_dir, Michigan_obs_stats_json_dir, Michigan_maw_json_dir, Michigan_nearest_neighbor_files, Michigan_road_segments_file, Michigan_timezone, Michigan_radius)
State_dictionary[State] = Michigan

# Michigan_vdt
State = "michigan_vdt"

# road sites file used by forecast system
All_sites_file = os.path.join(fhwa_sites_dir, "MI_logicast_road_sites.asc")
Road_cond_sites_file = os.path.join(fhwa_sites_dir, "MI_logicast_road_sites.asc")
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "MI_logicast_road_sites.asc")
Rwis_sites_file = os.path.join(fhwa_sites_dir, "MI_logicast_road_sites.asc")

# variable names in rwis netcdf files
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "MI_rwis_site_var_file.asc")

# nearest neighbor files used by maw backend for phone application
Michigan_nearest_neighbor_files = [os.path.join(config_dir, "mi_flann_index.dat"), os.path.join(config_dir, "mi_numpy_seg_pts.npy"), os.path.join(config_dir, "mi_numpy_seg_id.npy")]

# base directory for state output
Michigan_output_dir = os.path.join(fhwa_processed_dir, "michigan")

# directory containing district alerts for all sites
Michigan_district_alerts_dir = os.path.join(fhwa_processed_dir, "michigan/district_alerts_vdt")

# directory containing latest vehicle json files
Michigan_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "michigan/latest_vehicles")

# directory containing maw json files
Michigan_maw_json_dir = os.path.join(fhwa_processed_dir, "michigan/maw_json")

# directory containing vdt segment statistics files for state
Michigan_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "mi_vdt_output")

# observation statistics directory containing json files
Michigan_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "michigan/obs_stats_vdt")

# plots directory containing json files
Michigan_vdt_plots_dir = os.path.join(fhwa_processed_dir, "michigan/plot_data_vdt")

# road weather hazard directory
Michigan_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "mi_rwh_output")

# state timezone
Michigan_timezone = tz.Eastern

Michigan_radius = 7e-8

Product_dictionary_vdt = {"road_wx_dir" : fhwa_rdwx_logicast_dir, "rec_treatment" : fhwa_logicast_tmt_dir, "district_alerts" : Michigan_district_alerts_dir, "latest_vehicles" : Michigan_latest_vehicles_dir}
Michigan_vdt = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_logicast_dir, rdwx_base_name, fhwa_logicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary_vdt, Met_vars, Michigan_output_dir, Michigan_district_alerts_dir, Michigan_latest_vehicles_dir, Michigan_vdt_plots_dir, Michigan_rwh_dir, Michigan_obs_stats_dir, Michigan_obs_stats_json_dir, Michigan_maw_json_dir, Michigan_nearest_neighbor_files, Michigan_road_segments_file, Michigan_timezone, Michigan_radius)
State_dictionary[State] = Michigan_vdt


# Minnesota (deprecated version)
State = "minnesota"
Districts = [{"district_name" : "Minnesota", "min_lat" : 43.4, "min_lon" : -97.7, "max_lat" : 49.7, "max_lon" : -88.5}]
All_sites_file = os.path.join(fhwa_sites_dir, "MN_full_list.asc")
Road_cond_sites_file = os.path.join(fhwa_road_sites_dir, "MN_road_cond_sites.asc")
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "MN_metar_sites.asc")
Rwis_sites_file = os.path.join(fhwa_road_sites_dir, "MN_rwis_sites.asc")
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "MN_rwis_site_var_file.asc")

Minnesota_nearest_neighbor_files = [os.path.join(config_dir, "mn_flann_index.dat"), os.path.join(config_dir, "mn_numpy_seg_pts.npy"), os.path.join(config_dir, "mn_numpy_seg_id.npy")]
Minnesota_output_dir = os.path.join(fhwa_processed_dir, "minnesota")
Minnesota_district_alerts_dir = os.path.join(fhwa_processed_dir, "minnesota/district_alerts")
Minnesota_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "minnesota/latest_vehicles")
Minnesota_maw_json_dir = os.path.join(fhwa_processed_dir, "minnesota/maw_json")
Minnesota_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "mn_vdt_output")
Minnesota_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "minnesota/obs_stats_vdt")
Minnesota_plots_dir = os.path.join(fhwa_processed_dir, "minnesota/plot_data")
Minnesota_road_segments_file = os.path.join(cdl_dir, "mn_roads.20131111.nc")
Minnesota_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "mn_rwh_output")
Minnesota_vdt_output_dir = os.path.join(fhwa_vdt_processed_dir, "mn_vdt_output")
Minnesota_timezone = tz.Central
Minnesota_radius = 0.01

Minnesota_flann_files = ["/d1/vii/data/static/config/mn_flann_index.20131111.dat", "/d1/vii/data/static/config/mn_numpy_seg_pts.20131111.npy", "/d1/vii/data/static/config/mn_numpy_seg_id.20131111.npy"]

Product_dictionary = {"road_wx_dir" : fhwa_rdwx_dicast_dir, "rec_treatment" : fhwa_dicast_tmt_dir, "district_alerts" : Minnesota_district_alerts_dir, "latest_vehicles" : Minnesota_latest_vehicles_dir}

Minnesota = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_dicast_dir, rdwx_base_name, fhwa_dicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary, Met_vars, Minnesota_output_dir, Minnesota_district_alerts_dir, Minnesota_latest_vehicles_dir, Minnesota_plots_dir, Minnesota_rwh_dir, Minnesota_obs_stats_dir, Minnesota_obs_stats_json_dir, Minnesota_maw_json_dir, Minnesota_nearest_neighbor_files, Minnesota_road_segments_file, Minnesota_timezone, Minnesota_radius)
State_dictionary[State] = Minnesota

# Minnesota_vdt
State = "minnesota_vdt"
All_sites_file = os.path.join(fhwa_sites_dir, "MN_logicast_road_sites.asc")
Road_cond_sites_file = os.path.join(fhwa_sites_dir, "MN_logicast_road_sites.asc")
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "MN_logicast_road_sites.asc")
Rwis_sites_file = os.path.join(fhwa_sites_dir, "MN_logicast_road_sites.asc")
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "MN_rwis_site_var_file.asc")
Minnesota_nearest_neighbor_files = [os.path.join(config_dir, "mn_flann_index.dat"), os.path.join(config_dir, "mn_numpy_seg_pts.npy"), os.path.join(config_dir, "mn_numpy_seg_id.npy")]
Minnesota_output_dir = os.path.join(fhwa_processed_dir, "minnesota")
Minnesota_district_alerts_dir = os.path.join(fhwa_processed_dir, "minnesota/district_alerts_vdt")
Minnesota_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "minnesota/latest_vehicles")
Minnesota_maw_json_dir = os.path.join(fhwa_processed_dir, "minnesota/maw_json")
Minnesota_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "mn_vdt_output")
Minnesota_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "minnesota/obs_stats_vdt")
Minnesota_vdt_plots_dir = os.path.join(fhwa_processed_dir, "minnesota/plot_data_vdt")
Minnesota_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "mn_rwh_output")
Minnesota_timezone = tz.Central
Minnesota_radius = 0.01

Minnesota_flann_files = ["/d1/vii/data/static/config/mn_flann_index.20131111.dat", "/d1/vii/data/static/config/mn_numpy_seg_pts.20131111.npy", "/d1/vii/data/static/config/mn_numpy_seg_id.20131111.npy"]

Product_dictionary_vdt = {"road_wx_dir" : fhwa_rdwx_logicast_dir, "rec_treatment" : fhwa_logicast_tmt_dir, "district_alerts" : Minnesota_district_alerts_dir, "latest_vehicles" : Minnesota_latest_vehicles_dir}
Minnesota_vdt = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_logicast_dir, rdwx_base_name, fhwa_logicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary_vdt, Met_vars, Minnesota_output_dir, Minnesota_district_alerts_dir, Minnesota_latest_vehicles_dir, Minnesota_vdt_plots_dir, Minnesota_rwh_dir, Minnesota_obs_stats_dir, Minnesota_obs_stats_json_dir, Minnesota_maw_json_dir, Minnesota_nearest_neighbor_files, Minnesota_road_segments_file, Minnesota_timezone, Minnesota_radius)
State_dictionary[State] = Minnesota_vdt

# Nevada (deprecated version)
State = "nevada"
Districts = [{"district_name" : "Nevada", "min_lat" : 34.5, "min_lon" : -120.8, "max_lat" : 42.8, "max_lon" : -113.1}]
All_sites_file = os.path.join(fhwa_sites_dir, "NV_full_list.asc")
Road_cond_sites_file = os.path.join(fhwa_road_sites_dir, "NV_road_cond_sites.asc")
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "NV_metar_sites.asc")
Rwis_sites_file = os.path.join(fhwa_road_sites_dir, "NV_rwis_sites.asc")
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "NV_rwis_site_var_file.asc")
Nevada_nearest_neighbor_files = [os.path.join(config_dir, "nv_flann_index.dat"), os.path.join(config_dir, "nv_numpy_seg_pts.npy"), os.path.join(config_dir, "nv_numpy_seg_id.npy")]
Nevada_output_dir = os.path.join(fhwa_processed_dir, "nevada")
Nevada_district_alerts_dir = os.path.join(fhwa_processed_dir, "nevada/district_alerts")
Nevada_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "nevada/latest_vehicles")
Nevada_maw_json_dir = os.path.join(fhwa_processed_dir, "nevada/maw_json")
Nevada_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "nv_vdt_output")
Nevada_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "nevada/obs_stats_vdt")
Nevada_plots_dir = os.path.join(fhwa_processed_dir, "nevada/plot_data")
Nevada_road_segments_file = os.path.join(cdl_dir, "nv_roads.20131111.nc")
Nevada_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "nv_rwh_output")
Nevada_vdt_output_dir = os.path.join(fhwa_vdt_processed_dir, "nv_vdt_output")
Nevada_timezone = tz.Pacific
Nevada_radius = 0.01

Product_dictionary = {"road_wx_dir" : fhwa_rdwx_dicast_dir, "rec_treatment" : fhwa_dicast_tmt_dir, "district_alerts" : Nevada_district_alerts_dir, "latest_vehicles" : Nevada_latest_vehicles_dir}

Nevada = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_dicast_dir, rdwx_base_name, fhwa_dicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary, Met_vars, Nevada_output_dir, Nevada_district_alerts_dir, Nevada_latest_vehicles_dir, Nevada_plots_dir, Nevada_rwh_dir, Nevada_obs_stats_dir, Nevada_obs_stats_json_dir, Nevada_maw_json_dir, Nevada_nearest_neighbor_files, Nevada_road_segments_file, Nevada_timezone, Nevada_radius)
State_dictionary[State] = Nevada

# Nevada_vdt
State = "nevada_vdt"
All_sites_file = os.path.join(fhwa_sites_dir, "NV_logicast_road_sites.asc")
Road_cond_sites_file = os.path.join(fhwa_sites_dir, "NV_logicast_road_sites.asc")
Wx_obs_sites_file = os.path.join(fhwa_sites_dir, "NV_logicast_road_sites.asc")
Rwis_sites_file = os.path.join(fhwa_sites_dir, "NV_logicast_road_sites.asc")
Rwis_sites_var_file = os.path.join(fhwa_road_sites_dir, "NV_rwis_site_var_file.asc")
Nevada_nearest_neighbor_files = [os.path.join(config_dir, "nv_flann_index.dat"), os.path.join(config_dir, "nv_numpy_seg_pts.npy"), os.path.join(config_dir, "nv_numpy_seg_id.npy")]
Nevada_output_dir = os.path.join(fhwa_processed_dir, "nevada")
Nevada_district_alerts_dir = os.path.join(fhwa_processed_dir, "nevada/district_alerts_vdt")
Nevada_latest_vehicles_dir = os.path.join(fhwa_processed_dir, "nevada/latest_vehicles")
Nevada_maw_json_dir = os.path.join(fhwa_processed_dir, "nevada/maw_json")
Nevada_obs_stats_dir = os.path.join(fhwa_vdt_processed_dir, "nv_vdt_output")
Nevada_obs_stats_json_dir = os.path.join(fhwa_processed_dir, "nevada/obs_stats_vdt")
Nevada_vdt_plots_dir = os.path.join(fhwa_processed_dir, "nevada/plot_data_vdt")
Nevada_rwh_dir = os.path.join(fhwa_vdt_processed_dir, "nv_rwh_output")
Nevada_timezone = tz.Pacific
Nevada_radius = 0.01

Product_dictionary_vdt = {"road_wx_dir" : fhwa_rdwx_logicast_dir, "rec_treatment" : fhwa_logicast_tmt_dir, "district_alerts" : Nevada_district_alerts_dir, "latest_vehicles" : Nevada_latest_vehicles_dir}
Nevada_vdt = StateData(State, All_sites_file, Road_cond_sites_file, Wx_obs_sites_file, Rwis_sites_file, Rwis_sites_var_file, fhwa_rdwx_logicast_dir, rdwx_base_name, fhwa_logicast_tmt_dir, tmt_base_name, fhwa_wx_obs_dir, fhwa_mesonet_dir, Districts, Product_dictionary_vdt, Met_vars, Nevada_output_dir, Nevada_district_alerts_dir, Nevada_latest_vehicles_dir, Nevada_vdt_plots_dir, Nevada_rwh_dir, Nevada_obs_stats_dir, Nevada_obs_stats_json_dir, Nevada_maw_json_dir, Nevada_nearest_neighbor_files, Nevada_road_segments_file, Nevada_timezone, Nevada_radius)
State_dictionary[State] = Nevada_vdt

plots_base_name = "plot_data"
district_alerts_base_name = "district_alerts"
latest_vehicles_base_name = "vdt"
latest_vehicles_suffix = "json"
Num_hours = 24

state_abbrev_to_full = {"ak" : "alaska", "mi" : "michigan", "mn" : "minnesota", "nv" : "nevada"}
state_full_to_abbrev = {}
for key, value in state_abbrev_to_full:
    state_full_to_abbrev[value] = key
state_full_to_abbrev["minnesota_vdt"] = "mn"
state_full_to_abbrev["michigan_vdt"] = "mi"
state_full_to_abbrev["nevada_vdt"] = "nv"

File_time_delta = 7200


if "__main__" == __name__:

    # make state subdirectories 
    for state in State_dictionary:
        if not os.path.exists(State_dictionary[state].output_dir):
            os.makedirs(State_dictionary[state].output_dir)
        if not os.path.exists(State_dictionary[state].district_alerts_dir):
            os.makedirs(State_dictionary[state].district_alerts_dir_dir)
        if not os.path.exists(State_dictionary[state].latest_vehicles_dir):
            os.makedirs(State_dictionary[state].latest_vehicles_dir)
        if not os.path.exists(State_dictionary[state].plots_dir):
            os.makedirs(State_dictionary[state].plots_dir)
        if not os.path.exists(State_dictionary[state].obs_stats_json_dir):
            os.makedirs(State_dictionary[state].obs_stats_json_dir)
        if not os.path.exists(State_dictionary[state].maw_json_dir):
            os.makedirs(State_dictionary[state].maw_json_dir)

    # system directory list
    dir_list = [cdl_dir,
                config_dir,
                log_dir,
                fhwa_processed_dir,
                fhwa_vdt_processed_dir,
                phone_log_dir
                ]

    for directory in dir_list:
        if not os.path.exists(directory):
            os.makedirs(directory)




