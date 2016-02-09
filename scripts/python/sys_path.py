#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/02/02 20:17:38 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import os
import string

#
# Define system type strings
#
LONG_TERM  = "lt"
SHORT_TERM = "st"
NEAR_TERM = "nt"

try:
    System_name = NEAR_TERM
    System_root_dir = os.environ['RWFS_ROOT_DIR']
except:
    System_root_dir = os.path.join(os.environ['HOME'], 'data')
    
    
#
#
# Define base names
#
def dmos_emp_base(model, fcst_hour):
   return "%s%s_dmos_emp" % (model, fcst_hour)

def dmos_emp_ens_base(model, fcst_hour):
   return "%s%s_dmos_emp_ens" % (model, fcst_hour)

def dmos_emp_mec_base(model, fcst_hour):
   return "%s%s_dmos_emp_mec" % (model, fcst_hour)

def dmos_fcst_base(model, fcst_hour):
   return "%s%s_dmos_fcst" % (model, fcst_hour)

def mos_fcst_base(model, fcst_hour):
   return "%s%s_mos_fcst" % (model, fcst_hour)

def reg_base(model, fcst_hour):
   return "%s%s_reg" % (model, fcst_hour)

#
# Lookup for command names
#
c = {
     'lamp': 'lampmos_fcst',
     'mav' : 'mavmos_fcst',
     'met' : 'mavmos_fcst',
     'mex' : 'mexmos_fcst',
     'mrf' : 'mrfmos_fcst',
     'ngm' : 'ngmmos_fcst'
     }
                                               
def mos_cmd(name):
   return c[name]

Cdl_base = "cdl"                	# for cdl files
Climate_base = "climate"
Climo_fcst_base = "climo_fcst"	
Config_base = "config"   		# for command-line config files
DB_insert_base = "db_insert"		# for db insert data (not used)
Dec_base = "dec_data"			# for decoded data
Def_fcst_base = "def_fcst"		# for default grid forecasts
Der_base = "der_data"			# for derived products data
Der_obs_base = "der_obs"		# for derived observation data
Fcst_stat_base = "fcst_stat"            # for forecast statistics input files
Fcst2db_base = "fcst2db"                # for forecast DB (CSV) files
Gfs_grid_base = "gfs_grid"		# for precip grid site data
Gfs_mdl_proc_base = "gfs_mdl_proc"      # for gfs model analysis data
Grid_base = "grid"			# for forecast grid data files
Int_emp_base = "int_emp"	
Int_emp_def_base = "int_emp_def"	# for default integrator empirics data
Int_fcst_base = "int_fcst"	
Int_obs_base = "int_obs"		# for integrated observation data
Lock_base = "lock"                  	# for lock files
Log_base = "log"                    	# for logging system information
Mdl_int_obs_base = "mdl_int_obs"        # for model-filled int_obs data
Mdl_proc_base = "mdl_proc"             	# for input model analysis data
Mesh_fcst_base = "mesh_fcst"            # for meshed forecast data
Meso_base = "mesonet"               	# for input mesonet data
Meso_blend_base = "meso_blend"		# for blended mesonet data
Meso_proc_base = "meso_proc"		# for processed mesonet data
Mod_emp_base = "mod_emp"
Mod_emp_def_base = "mod_emp_def" 	# for model empirics default data
Mod_fcst_base = "mod_fcst"		# for module forecast data
Model_data_base = "model"		# for model data
Mos_input_base = "mos"			# for mos data
Mtr_base = "metar"               	# for input metar data
Mtr_blend_base = "mtr_blend"		# for blended metar data
Mtr_proc_base = "mtr_proc"		# for processed metar data
Nam_grid_base = "nam_grid"		# for precip grid site data
Obs_base = "obs"                    	# for observation data
Obs_stat_base = "obs_stat"		# for observation statisticsinput files
Params_base = "params"			# for TDRP param files
Persist_base = "persist"		# for persistence
Post_proc_base = "post_proc"		# for post processor data
Prism_base = "prism"
Raw_base = "raw_data"	                # for raw data
Sim_base = "sim"			# for simulation data
Site_list_base = "site_list"        	# for site list files
St_persist_base = "st_persist"		# for short term persistence
Static_data_base = "static_data"	# for static data
Syn_base = "synoptic"               	# for input synoptic data
Syn_blend_base = "syn_blend"		# for blended synoptic data
Syn_proc_base = "syn_proc"		# for processed synoptic data
Temp_var_base = "temp_fcst"		# for temperature grids
Ver_data_base = "ver_data"		# for verification data files
Ver_stat_base = "ver_stat"		# for verification stats

#
# Top-level directories 
#
DB_insert_dir = "%s/%s" % (System_root_dir, DB_insert_base)
Dec_data_dir = "%s/%s" % (System_root_dir, Dec_base)
Der_data_dir = "%s/%s" % (System_root_dir, Der_base)
Fcst2db_dir = "%s/%s" % (System_root_dir, Fcst2db_base)
Grid_dir = "%s/%s" % (System_root_dir, Grid_base)
Int_emp_dir = "%s/%s" % (System_root_dir, Int_emp_base)
Int_fcst_dir = "%s/%s" % (System_root_dir, Int_fcst_base)
Lock_dir = "%s/%s" % (System_root_dir, Lock_base)
Log_dir = "%s/%s" % (System_root_dir, Log_base)
Mesh_fcst_dir = "%s/%s" % (System_root_dir, Mesh_fcst_base)
Mod_emp_dir = "%s/%s" % (System_root_dir, Mod_emp_base)
Mod_fcst_dir = "%s/%s" % (System_root_dir, Mod_fcst_base)
Post_proc_dir = "%s/%s" % (System_root_dir, Post_proc_base)
Precip_grid_dir = "%s/%s" % (System_root_dir, Grid_base)
Raw_data_dir = "%s/%s" % (System_root_dir, Raw_base)
Static_data_dir = "%s/%s" % (System_root_dir, Static_data_base)
Tmp_dir = "%s/tmp" % System_root_dir
Ver_data_dir = "%s/%s" % (System_root_dir, Ver_data_base)

#
# Secondary subdirectories
#
Model_input_dir = "%s/%s" % (Dec_data_dir, Model_data_base)
Def_fcst_grid_dir = "%s/%s" % (Grid_dir, Def_fcst_base)
Der_obs_data_dir = "%s/%s" % (Der_data_dir, Obs_base)
Der_obs_dir = "%s/%s" % (Der_obs_data_dir, Der_obs_base)
Gfs_mdl_proc_dir = "%s/%s" % (Der_obs_data_dir, Gfs_mdl_proc_base)
Gfs_precip_grid_site_dir = "%s/%s"% (Model_input_dir, Gfs_grid_base)
Grid_tmp_dir = "%s/%s" % (Tmp_dir, "grid")
Grid_temperature_dir = "%s/%s" % (Grid_dir, Temp_var_base)
Int_obs_dir = "%s/%s" % (Der_obs_data_dir, Int_obs_base)
Mdl_int_obs_dir = "%s/%s" % (Der_obs_data_dir, Mdl_int_obs_base)
Meso_input_dir = "%s/%s/%s" % (Dec_data_dir, Obs_base, Meso_base)
Mos_input_dir = "%s/%s" % (Dec_data_dir, Mos_input_base)
Mtr_input_dir = "%s/%s/%s" % (Dec_data_dir, Obs_base, Mtr_base)
Nam_precip_grid_site_dir = "%s/%s"% (Model_input_dir, Nam_grid_base)
Post_proc_config_dir = "%s/%s" % (Post_proc_dir, Config_base)
Syn_input_dir = "%s/%s/%s" % (Dec_data_dir, Obs_base, Syn_base)

#
# Static data subdirectories
#
Cdl_dir = "%s/%s" % (Static_data_dir, Cdl_base)
Climate_dir = "%s/%s" % (Static_data_dir, Climate_base)
Prism_climate_dir = "%s/%s" % (Climate_dir, Prism_base)
Config_dir = "%s/%s" % (Static_data_dir, Config_base)
Int_emp_def_dir = "%s/%s" % (Static_data_dir, Int_emp_def_base)
Mod_emp_def_dir = "%s/%s" % (Static_data_dir, Mod_emp_def_base)
Params_dir = "%s/%s" % (Static_data_dir, Params_base)
Site_list_dir = "%s/%s" % (Static_data_dir, Site_list_base)

#
# Config file paths
#
def dmos_emp_config_file(model,fcst_hour):
   return "%s/%s%s_config.asc" % (Config_dir, model, fcst_hour)

#
# Neighbor file paths
#
def nbr_file(nbr_base):
   #
   # nbr_base can be mavmos,metar,mexmos,mrfmos,ngmmos,lampmos
   #
   return "%s/%s_nbr.nc" % (Site_list_dir, nbr_base)

#
# Cdl files
#
Derive_vars_cdl_name = "derive_vars.cdl"
Dmos_cdl_name = "dmos.cdl"
Fec_cdl_name = "fec.cdl"
Grid_mod_fcst_cdl_name = "grid_module_forecast.cdl"
Int_emp_cdl_name = "integrator_empirics.cdl"
Int_obs_cdl_name = "int_obs.cdl"
Mdl_proc_cdl_name = "mdl_proc.cdl"
Merge_vars_cdl_name = "merge_vars.cdl"
Mesh_fcst_cdl_name = "mesh_fcst.cdl"
Meso_blend_cdl_name = "meso_blend.cdl"
Meso_proc_cdl_name = "meso_proc.cdl"
Mod_fcst_cdl_name = "module_forecast.cdl"
Mod_fcst_emp_cdl_name = "module_forecast_empirics.cdl"
Mtr_blend_cdl_name = "mtr_blend.cdl"
Mtr_proc_cdl_name = "mtr_proc.cdl"
Post_proc_cdl_name = "post_proc.cdl"
Qc_core_cdl_name = "qc_core.cdl"
Qc_final_cdl_name = "qc_final.cdl"
Regressor_cdl_name = "regressor_output.cdl"
Spatial_interp_cdl_name = "spatial_interp.cdl"
Syn_blend_cdl_name = "syn_blend.cdl"
Syn_proc_cdl_name = "syn_proc.cdl"
T_grid_mod_fcst_cdl_name = "T_grid_module_forecast.cdl"
Temporal_interp_cdl_name = "temporal_interp.cdl"

#
# Cdl file paths
#
Derive_vars_cdl = "%s/%s" % (Cdl_dir, Derive_vars_cdl_name)
Dmos_cdl = "%s/%s" % (Cdl_dir, Dmos_cdl_name)
Fec_cdl = "%s/%s" % (Cdl_dir, Fec_cdl_name)
Grid_mod_fcst_cdl = "%s/%s" % (Cdl_dir, Grid_mod_fcst_cdl_name)
Int_emp_cdl = "%s/%s" % (Cdl_dir, Int_emp_cdl_name)
Int_obs_cdl = "%s/%s" % (Cdl_dir, Int_obs_cdl_name)
Mdl_proc_cdl = "%s/%s" % (Cdl_dir, Mdl_proc_cdl_name)
Merge_vars_cdl = "%s/%s" % (Cdl_dir, Merge_vars_cdl_name)
Mesh_fcst_cdl = "%s/%s" % (Cdl_dir, Mesh_fcst_cdl_name)
Meso_blend_cdl = "%s/%s" % (Cdl_dir, Meso_blend_cdl_name)
Meso_proc_cdl = "%s/%s" % (Cdl_dir, Meso_proc_cdl_name)
Mod_fcst_cdl = "%s/%s" % (Cdl_dir, Mod_fcst_cdl_name)
Mod_fcst_emp_cdl = "%s/%s" % (Cdl_dir, Mod_fcst_emp_cdl_name)
Mtr_blend_cdl = "%s/%s" % (Cdl_dir, Mtr_blend_cdl_name)
Mtr_proc_cdl = "%s/%s" % (Cdl_dir, Mtr_proc_cdl_name)
Post_proc_cdl = "%s/%s" % (Cdl_dir, Post_proc_cdl_name)
Qc_core_cdl = "%s/%s" % (Cdl_dir, Qc_core_cdl_name)
Qc_final_cdl = "%s/%s" % (Cdl_dir, Qc_final_cdl_name)
Regressor_cdl = "%s/%s" % (Cdl_dir, Regressor_cdl_name)
Spatial_interp_cdl = "%s/%s" % (Cdl_dir, Spatial_interp_cdl_name)
Syn_blend_cdl = "%s/%s" % (Cdl_dir, Syn_blend_cdl_name)
Syn_proc_cdl = "%s/%s" % (Cdl_dir, Syn_proc_cdl_name)
T_grid_mod_fcst_cdl = "%s/%s" % (Cdl_dir, T_grid_mod_fcst_cdl_name)
Temporal_interp_cdl = "%s/%s" % (Cdl_dir, Temporal_interp_cdl_name)

#
# TDRP params files
#
Create_int_emp_def_params_name = "create_int_emp_def.%s" % Params_base
Climo_fcst_params_name = "climo_fcst.%s" % Params_base
DB_insert_params_name = "db_insert.%s" % Params_base
Der_obs_params_name = "der_obs.%s" % Params_base
Dmos_emp_params_name = "dmos_emp.%s" % Params_base
Dmos_params_name = "dmos.%s" % Params_base
Int_emp_params_name = "int_emp.%s" % Params_base
IntEmp_params_name = "intEmp.%s" % Params_base
Int_obs_params_name = "int_obs.%s" % Params_base
Int_fcst_params_name = "int_fcst.%s" % Params_base
Mdl_int_obs_params_name = "mdlObs.%s" % Params_base
Mdl_int_obs_params_file = "%s/%s" % (Params_dir, Mdl_int_obs_params_name)
Mesh_fcst_params_name = "mesh_fcst.%s" % Params_base
Meso_proc_params_name = "meso_proc.%s" % Params_base
Mtr_proc_params_name = "mtr_proc.%s" % Params_base
Ndmos_emp_params_name = "ndmosEmpEns.%s" % Params_base
Persistence_fcst_params_name = "persistence.%s" % Params_base
Post_proc_params_name = "post_proc.%s" % Params_base
Reg_calc_params_name = "regCalc.%s" % Params_base
St_persistence_fcst_params_name = "st_persistence.%s" % Params_base
Syn_proc_params_name = "syn_proc.%s" % Params_base

#
# TDRP params file paths
#
Create_int_emp_def_params_file = "%s/%s" % (Params_dir, Create_int_emp_def_params_name)
Climo_fcst_params_file = "%s/%s" % (Params_dir, Climo_fcst_params_name)
DB_insert_params_file = "%s/%s" % (Params_dir, DB_insert_params_name)
Der_obs_params_file = "%s/%s" % (Params_dir, Der_obs_params_name)
Dmos_emp_params_file = "%s/%s" % (Params_dir, Dmos_emp_params_name)
Dmos_params_file = "%s/%s" % (Params_dir, Dmos_params_name)
Int_emp_params_file = "%s/%s" % (Params_dir, Int_emp_params_name)
IntEmp_params_file = "%s/%s" % (Params_dir, IntEmp_params_name)
Int_obs_params_file = "%s/%s" % (Params_dir, Int_obs_params_name)
Int_fcst_params_file = "%s/%s" % (Params_dir, Int_fcst_params_name)
Mesh_fcst_params_file = "%s/%s" % (Params_dir, Mesh_fcst_params_name)
Meso_proc_params_file = "%s/%s" % (Params_dir, Meso_proc_params_name)
Mtr_proc_params_file = "%s/%s" % (Params_dir, Mtr_proc_params_name)
Ndmos_emp_params_file = "%s/%s" % (Params_dir, Ndmos_emp_params_name)
Persistence_fcst_params_file = "%s/%s" % (Params_dir, Persistence_fcst_params_name)
Post_proc_params_file = "%s/%s" % (Params_dir, Post_proc_params_name)
St_persistence_fcst_params_file = "%s/%s" % (Params_dir, St_persistence_fcst_params_name)
Syn_proc_params_file = "%s/%s" % (Params_dir, Syn_proc_params_name)

def reg_params_file(model):
#   return "%s/%s.%s" % ( Params_dir, Reg_calc_params_name, model)
   return "%s/%s.%s" % ( Params_dir, model, Reg_calc_params_name)


def mos_fcst_params_file(mos_name):
   return "%s/%s_fcst.%s" % (Params_dir, mos_name, Params_base)   

#
# Constants and other stuff
#
MAX_MOD_FCST_FILES_PER_DAY = 24
MAX_INT_EMP_FILES_PER_DAY = 24

#
# Allowable maximum ages (seconds) of data files 
#
Dmos_reg_max_age = 2 * 86400
Dmos_emp_max_age = 200 * 86400
Int_fcst_max_age = 86400
Int_emp_max_age  = 100 * 86400
Mod_fcst_max_age = 2 * 86400
Mos_max_age      = 2 * 86400  # used by nwsmos_fcst.py to look for MOS files
Model_max_age    = 2 * 86400  # used by reg_calc.py to look for model files

#
# Suffixes for files
#
ASC_SUFFIX = "asc"
BIN_SUFFIX = "bin"
NC_SUFFIX = "nc"
XML_SUFFIX = "xml"
LOG_PY_SUFFIX = ".pyl"
LOG_EP_SUFFIX = ".epl"

#
# Index file thingies
#
Index_base = "index"
Index_wait = 0
Index_period = 24

#
# System-specific items
#
if System_name == SHORT_TERM:
    NUM_FCST_DAYS = 5
    FCST_HR_INTERVAL = 3
    FCST_HR_MAX = NUM_FCST_DAYS * 24 - FCST_HR_INTERVAL
elif System_name == LONG_TERM:
    NUM_FCST_DAYS = 17
    FCST_HR_INTERVAL = 6
    FCST_HR_MAX = NUM_FCST_DAYS * 24 - FCST_HR_INTERVAL
elif System_name == NEAR_TERM:
    NUM_FCST_DAYS = 8
    FCST_HR_INTERVAL = 1
    FCST_HR_MAX = NUM_FCST_DAYS * 24 - FCST_HR_INTERVAL

#
# Site list, site dictionary, climo, monitor config files
#
Climatology_file_name = "dicast_climatology.nc"
Climatology_nbr_file_name = "climo_nbr.nc"
Climo_grid_nbr_file_name = "climo_grid_nbr.asc"
Full_site_list_file_name = "full_list.asc"
Grid_site_list_file_name = "grid_metar_list.asc"
Grid_climatology_file_name = "grid_climatology.nc"
Full_grid_site_list_file_name = "grid_site_list.asc"
Monitor_config_file_name = "mon.conf"
Mos_sites_file_name = "mav_mex_sites.asc"
Post_proc_stage_file_name = "post_proc_stage.asc"
Site_dictionary_file_name = "site_dictionary.asc"
Site_list_file_name = "core_sites.asc"
Fcst_module_list_file_name = "fcst_module_list.asc"
#
Climatology_file = "%s/%s" % (Climate_dir, Climatology_file_name)
Climatology_nbr_file = "%s/%s" % (Site_list_dir, Climatology_nbr_file_name)
Climo_grid_nbr_file = "%s/%s" % (Site_list_dir, Climo_grid_nbr_file_name)
Full_site_list_file = "%s/%s" % (Site_list_dir, Full_site_list_file_name)
Grid_site_list_file = "%s/%s" % (Site_list_dir, Grid_site_list_file_name)
Full_grid_site_list_file = "%s/%s" % (Site_list_dir, Full_grid_site_list_file_name)
Grid_climatology_file = "%s/%s" % (Climate_dir, Grid_climatology_file_name)
Monitor_file = "%s/%s" % (Config_dir, Monitor_config_file_name)
Mos_sites_file = "%s/%s" % (Site_list_dir, Mos_sites_file_name)
Post_proc_stage_file = "%s/%s" % (Post_proc_config_dir, Post_proc_stage_file_name)
Site_dictionary_file = "%s/%s" % (Site_list_dir, Site_dictionary_file_name)
Site_list_file = "%s/%s" % (Site_list_dir, Site_list_file_name)
Fcst_module_list_file = "%s/%s" % (Config_dir, Fcst_module_list_file_name)
#
Hour_list = [0,3,6,9,12,15,18,21]   # Default set of hours utilized by integrator empirics processing
#
Int_emp_def_file = "%s/%s_def.%s" % (Int_emp_def_dir, Int_emp_base, NC_SUFFIX)
#
#
# These should no longer be used -----------------------------------------
#
Hours = []                       # This will contain a list of all 24 hours
Int_emp_def_files = []           # The empirics default files for a full 24 hours
Num_hours = len(Hour_list)
for i in range(Num_hours):
    Hours.append(string.zfill(str(Hour_list[i]), 2))
    Int_emp_def_files.append("%s/%s_%s.%s" % (Int_emp_def_dir, Int_emp_base, Hours[i], NC_SUFFIX))
#
# ------------------------------------------------------------------------
#
