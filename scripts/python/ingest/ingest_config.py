#!/usr/bin/env python
#
# Domain configuration file for defining path variables. This one is
# for the CONUS region. If this file is executed as main(), then a list
# of top-level directories are created under System_root_dir.
#
#----------------------------------------------------------------------
#
#

import os
import vii_paths as paths
import vii_defs as defs

# Define the root directory for this system.
System_root_dir = "/d2/vii/data"

# Define the ldm directory for this system
Ldm_dir = "/d2/ldm/data"

# Additional directories
Analysis_satellite_dir = os.path.join(System_root_dir, "processed", "satellite")
Lock_dir = os.path.join(System_root_dir, "lock")
Log_dir  = os.path.join(System_root_dir, "log")
Raw_awc_gini_dir = os.path.join(Ldm_dir, "gini")
Raw_dir = os.path.join(System_root_dir, "raw")
Raw_gini_dir = os.path.join(Ldm_dir, "gini")
Raw_metar_dir = os.path.join(Raw_dir, "metar")
Raw_metar_ascii_dir = os.path.join(Raw_metar_dir, "ascii")
Raw_metar_netcdf_dir = os.path.join(Raw_metar_dir, "netcdf")

Satellite_cloud_mask_base_dir = os.path.join(Analysis_satellite_dir, "goes/cloud_mask")
Satellite_cloud_mask_netcdf = os.path.join(Satellite_cloud_mask_base_dir, "netcdf")
Satellite_cloud_mask_blend = os.path.join(Satellite_cloud_mask_netcdf, "blend")
Satellite_west_blend_in_dir = os.path.join(Satellite_cloud_mask_netcdf, "west")
Satellite_east_blend_in_dir = os.path.join(Satellite_cloud_mask_netcdf, "east")
Satellite_goes_history = os.path.join(Analysis_satellite_dir,"goes/history")
Satellite_goes_grid_convert = os.path.join(Analysis_satellite_dir,"goes/grid_convert")
Satellite_goes_combine = os.path.join(Analysis_satellite_dir,"goes/ch_combine")
Satellite_goes_init = os.path.join(Analysis_satellite_dir,"goes/init")
Satellite_goes_init_awc = os.path.join(Analysis_satellite_dir,"goes/init")
Satellite_cloud_mask_dir = Satellite_cloud_mask_blend

Static_dir = os.path.join(System_root_dir, "static")
Static_proj_dir = os.path.join(Static_dir, "proj")
Static_cdl_dir = os.path.join(Static_dir, "cdl")
Static_site_list_dir = os.path.join(Static_dir, "site_list")
Static_var_names_dir = os.path.join(Static_dir, "var_names")
Tmp_dir  = os.path.join(System_root_dir, "tmp")

# System files
Ameritrak_ingest_pid_file = os.path.join(Lock_dir, "ameritrak_ingest.pid")
Combined_site_fcst_cdl = os.path.join(Static_cdl_dir, "combined_site_fcst.cdl")
Gini_east_cdl = os.path.join(Static_cdl_dir,"east_gini.cdl")
Gini_west_cdl = os.path.join(Static_cdl_dir,"west_gini.cdl")
Gini_cdl_list = [Gini_east_cdl, Gini_west_cdl]
Metar2nc_cdl_file = os.path.join(Static_cdl_dir, "metar.cdl")
NA_site_list_file = os.path.join(Static_site_list_dir, "northamerica_sites.asc")
Satellite_list = ["east", "west"]
Satellite_scan_type_list = ["e", "w"]
Satellite_config_list = ["config_east","config_west"]
Satellite_west_in_base = "west_cloud"
Satellite_east_in_base = "east_cloud"
Satellite_cloud_mask_text = os.path.join(Satellite_cloud_mask_base_dir, "text")
Satellite_blend = True
Sat_convert_cdl_file = os.path.join(Static_cdl_dir, "sat_ndfd_conus_5km.cdl")
Sat_cloud_mask_gini_cdl_file = os.path.join(Static_cdl_dir, "cloud_mask_gini.cdl")
Sat_cloud_mask_gini_interm_cdl_file = os.path.join(Static_cdl_dir, "cloud_matrices_gini.cdl")
Sat_cloud_mask_blend_cdl_file = os.path.join(Static_cdl_dir, "cloud_blend.cdl")
Sat_vars_file = os.path.join(Static_var_names_dir, "sat_vars.asc")
Static_satellite = os.path.join(Static_dir, "satellite")
Terrain_file = os.path.join(Static_proj_dir, "topo_ndfd_conus_5km.nc")
Water_intervals_file = os.path.join(Static_proj_dir, "water_intervals_ndfd_conus_5km.asc")

if __name__ == "__main__":

    sys_dirs = [
        Analysis_satellite_dir,
        Lock_dir,
        Log_dir ,
        Raw_awc_gini_dir,
        Raw_dir,
        Raw_gini_dir,
        Raw_metar_dir,
        Raw_metar_ascii_dir,
        Raw_metar_netcdf_dir,
        Satellite_cloud_mask_base_dir,
        Satellite_cloud_mask_netcdf,
        Satellite_cloud_mask_blend,
        Satellite_west_blend_in_dir,
        Satellite_east_blend_in_dir,
        Satellite_goes_history,
        Satellite_goes_grid_convert,
        Satellite_goes_combine,
        Satellite_goes_init,
        Satellite_goes_init_awc,
        Static_dir,
        Static_proj_dir,
        Static_cdl_dir,
        Static_site_list_dir,
        Static_var_names_dir,
        Tmp_dir
        ]

    for dirpath in sys_dirs:
        if not os.path.exists(dirpath):
            os.makedirs(dirpath)
            
