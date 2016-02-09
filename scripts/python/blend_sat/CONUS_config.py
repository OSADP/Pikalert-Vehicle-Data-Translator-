#
#
# If Satellite_cloud_mask_dir is set, minterp will look for the latest
# cloud mask product there. If None, minterp will not use a cloud mask.
# 
#!/usr/bin/env python
#
#
# Domain configuration file for defining path variables. This one is
# for the CONUS region. If this file is executed as main(), then a list
# of top-level directories are created under System_root_dir.
#
#----------------------------------------------------------------------
#
#
from os import environ, makedirs
from os.path import exists, join
#
#
# Define the root directory for this system.
#

System_root_dir = "/d2/vii/data"

Lock_dir = join(System_root_dir, "lock")
Log_dir  = join(System_root_dir, "log")
Raw_dir = join(System_root_dir, "raw")
Static_dir = join(System_root_dir, "static")
Tmp_dir  = join(System_root_dir, "tmp")
Analysis_satellite_dir = join(System_root_dir, "processed", "satellite")
Raw_gini_dir = join("/d2/ldm/data", "gini")
Raw_awc_gini_dir = join("/d2/ldm/data", "gini")
Raw_metar_dir = join(Raw_dir, "metar")
Static_cdl_dir = join(Static_dir, "cdl")
Static_satellite = join(Static_dir, "satellite")
Static_var_names_dir = join(Static_dir, "var_names")
Static_proj_dir = join(Static_dir, "proj")
Static_site_list_dir = join(Static_dir, "site_list")
Satellite_list = ["east", "west"]
Satellite_scan_type_list = ["e", "w"]
Satellite_config_list = ["config_east","config_west"]
Satellite_cloud_mask_base_dir = join(Analysis_satellite_dir, "goes/cloud_mask")
Satellite_cloud_mask_netcdf = join(Satellite_cloud_mask_base_dir, "netcdf")
Satellite_cloud_mask_blend = join(Satellite_cloud_mask_netcdf, "blend")
Satellite_west_blend_in_dir = join(Satellite_cloud_mask_netcdf, "west")
Satellite_east_blend_in_dir = join(Satellite_cloud_mask_netcdf, "east")
Satellite_west_in_base = "west_cloud"
Satellite_east_in_base = "east_cloud"
Satellite_cloud_mask_text = join(Satellite_cloud_mask_base_dir, "text")
Satellite_goes_history = join(Analysis_satellite_dir,"goes/history")
Satellite_goes_grid_convert = join(Analysis_satellite_dir,"goes/grid_convert")
Satellite_goes_combine = join(Analysis_satellite_dir,"goes/ch_combine")
Satellite_goes_init = join(Analysis_satellite_dir,"goes/init")
Satellite_goes_init_awc = join(Analysis_satellite_dir,"goes/init")
Satellite_cloud_mask_dir = Satellite_cloud_mask_blend
Satellite_blend = True
Combined_site_fcst_cdl = join(Static_cdl_dir, "combined_site_fcst.cdl")
Gini_east_cdl = join(Static_cdl_dir,"east_gini.cdl")
Gini_west_cdl = join(Static_cdl_dir,"west_gini.cdl")
Gini_cdl_list = [Gini_east_cdl, Gini_west_cdl]
Sat_convert_cdl_file = join(Static_cdl_dir, "sat_ndfd_conus_5km.cdl")
Sat_cloud_mask_gini_cdl_file = join(Static_cdl_dir, "cloud_mask_gini.cdl")
Sat_cloud_mask_gini_interm_cdl_file = join(Static_cdl_dir, "cloud_matrices_gini.cdl")
Sat_cloud_mask_blend_cdl_file = join(Static_cdl_dir, "cloud_blend.cdl")
Sat_vars_file = join(Static_var_names_dir, "sat_vars.asc")
Terrain_file = join(Static_proj_dir, "topo_ndfd_conus_5km.nc")
Water_intervals_file = join(Static_proj_dir, "water_intervals_ndfd_conus_5km.asc")

NA_site_list_file = join(Static_site_list_dir, "northamerica_sites.asc")
Metar2nc_cdl_file = join(Static_cdl_dir, "metar.cdl")
Raw_metar_ascii_dir = join(Raw_metar_dir, "ascii")
Raw_metar_netcdf_dir = join(Raw_metar_dir, "netcdf")
