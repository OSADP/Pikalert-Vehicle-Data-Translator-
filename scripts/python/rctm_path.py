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

# Set the RCTM root dir

try:
   Rctm_root_dir = os.environ["RCTM_ROOT_DIR"]
except:
   Rctm_root_dir = "/d2/dicast/rctm"

#
# System-specific items
#
NUM_OUTPUT_DAYS = 6

#
# Some base names
#
Wx_fcst_base = "wx_fcst"
Rdwx_fcst_base = "rdwx_fcst"
Mesh_fcst_base = "mesh_fcst"
Dump_meso_base = "dump_meso"
Concat_meso_base = "concat_meso"
Dump_fcst_base = "dump_fcst"
Obs_hist_base = "obs_hist"

Road_cond_init_base = "rc_init"
Road_cond_cur_tmt_base = "rc_cur_tmt"
Road_cond_no_tmt_base = "rc_no_tmt"
Road_cond_rec_tmt_base = "rc_rec_tmt"
Road_cond_suff_tmt_base = "rc_suff_tmt"

Fec_road_cond_cur_tmt_base = "fec_rc_cur_tmt"
Fec_road_cond_no_tmt_base = "fec_rc_no_tmt"
Fec_road_cond_rec_tmt_base = "fec_rc_rec_tmt"
Fec_road_cond_suff_tmt_base = "fec_rc_suff_tmt"

Bridge_cond_init_base = "bc_init"
Bridge_cond_cur_tmt_base = "bc_cur_tmt"
Bridge_cond_no_tmt_base = "bc_no_tmt"
Bridge_cond_rec_tmt_base = "bc_rec_tmt"
Bridge_cond_suff_tmt_base = "bc_suff_tmt"

Fec_bridge_cond_cur_tmt_base = "fec_bc_cur_tmt"
Fec_bridge_cond_no_tmt_base = "fec_bc_no_tmt"
Fec_bridge_cond_rec_tmt_base = "fec_bc_rec_tmt"
Fec_bridge_cond_suff_tmt_base = "fec_bc_suff_tmt"

Bridge_cond_frost_no_tmt_base = "bc_frost_no_tmt"
Bridge_cond_frost_rec_tmt_base = "bc_frost_rec_tmt"
Bridge_cond_frost_suff_tmt_base = "bc_frost_suff_tmt"

Merge_cur_tmt_base = "merge_cur_tmt"
Merge_no_tmt_base = "merge_no_tmt"
Merge_rec_tmt_base = "merge_rec_tmt"
Merge_suff_tmt_base = "merge_suff_tmt"
Merge_tmt_base = "merge_tmt"

#
# Some top directory names
#
Wx_fcst_dir = "%s/%s" % (Rctm_root_dir, Wx_fcst_base)
Rdwx_fcst_dir = "%s/%s" % (Rctm_root_dir, Rdwx_fcst_base)
Mesh_fcst_dir = "%s/%s" % (Rctm_root_dir, Mesh_fcst_base)
Dump_meso_dir = "%s/%s" % (Rctm_root_dir, Dump_meso_base)
Concat_meso_dir = "%s/%s" % (Rctm_root_dir, Concat_meso_base)
Dump_fcst_dir = "%s/%s" % (Rctm_root_dir, Dump_fcst_base)
Obs_hist_dir = "%s/%s" % (Rctm_root_dir, Obs_hist_base)

Road_cond_init_dir = "%s/%s" % (Rctm_root_dir, Road_cond_init_base)
Road_cond_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Road_cond_cur_tmt_base)
Road_cond_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Road_cond_no_tmt_base)
Road_cond_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Road_cond_rec_tmt_base)
Road_cond_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Road_cond_suff_tmt_base)

Fec_road_cond_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_road_cond_cur_tmt_base)
Fec_road_cond_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_road_cond_no_tmt_base)
Fec_road_cond_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_road_cond_rec_tmt_base)
Fec_road_cond_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_road_cond_suff_tmt_base)

Bridge_cond_init_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_init_base)
Bridge_cond_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_cur_tmt_base)
Bridge_cond_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_no_tmt_base)
Bridge_cond_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_rec_tmt_base)
Bridge_cond_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_suff_tmt_base)

Fec_bridge_cond_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_bridge_cond_cur_tmt_base)
Fec_bridge_cond_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_bridge_cond_no_tmt_base)
Fec_bridge_cond_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_bridge_cond_rec_tmt_base)
Fec_bridge_cond_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Fec_bridge_cond_suff_tmt_base)

Bridge_cond_frost_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_frost_no_tmt_base)
Bridge_cond_frost_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_frost_rec_tmt_base)
Bridge_cond_frost_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Bridge_cond_frost_suff_tmt_base)

Merge_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Merge_cur_tmt_base)
Merge_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Merge_no_tmt_base)
Merge_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Merge_rec_tmt_base)
Merge_suff_tmt_dir = "%s/%s" % (Rctm_root_dir, Merge_suff_tmt_base)
Merge_tmt_dir = "%s/%s" % (Rctm_root_dir, Merge_tmt_base)

Static_dir = "%s/%s" % (Rctm_root_dir, "static_data")
Tmp_dir = "%s/%s" % (Rctm_root_dir, "tmp")
Log_dir = "%s/%s" % (Rctm_root_dir, "log")
Lock_dir = "%s/%s" % (Rctm_root_dir, "lock")

#
# Secondary directory layers
#
Cdl_dir = "%s/%s" % (Static_dir, "cdl")
Config_dir = "%s/%s" % (Static_dir, "config")
Params_dir = "%s/%s" % (Static_dir, "params")
Site_list_dir = "%s/%s" % (Static_dir, "site_list")


#
# Client base names
#
Fcst2db_base = "fcst2db"

#
# Client top directory names
#
Fcst2db_dir = "%s/%s" % (Rctm_root_dir, Fcst2db_base)


#
# MDSS base names
#
Alert_base = "alert"
Avl_base = "avl"
Webcam_base = "webcam"
NwsWW_base = "nwsww"
Conv_wx_base = "conv_wx"
Conv_meso_base = "conv_meso"
Conv_cur_tmt_base = "conv_cur_tmt"
Conv_no_tmt_base = "conv_no_tmt"
Conv_rec_tmt_base = "conv_rec_tmt"
Current_plan_base = "current_plan"
Site_plans_base = "site_plans"
Reset_rc_init_base = "reset_rc_init"

User_treatment_base = "user_treatment"
Road_cond_usr_tmt_base = "rc_usr_tmt"
Fec_road_cond_usr_tmt_base = "fec_rc_usr_tmt"
Bridge_cond_usr_tmt_base = "bc_usr_tmt"
Fec_bridge_cond_usr_tmt_base = "fec_bc_usr_tmt"
Merge_usr_tmt_base = "merge_usr_tmt"
Conv_usr_tmt_base = "conv_usr_tmt"

Conv_fcst_hist_base = "conv_fcst_hist"
  
#
# MDSS top directory names
#
Alert_dir = "%s/%s" % (Rctm_root_dir, Alert_base)
Avl_dir = "%s/%s" % (Rctm_root_dir, Avl_base)
Webcam_dir = "%s/%s" % (Rctm_root_dir, Webcam_base)
NwsWW_dir = "%s/%s" % (Rctm_root_dir, NwsWW_base)
Conv_wx_dir = "%s/%s" % (Rctm_root_dir, Conv_wx_base)
Conv_meso_dir = "%s/%s" % (Rctm_root_dir, Conv_meso_base)
Conv_cur_tmt_dir = "%s/%s" % (Rctm_root_dir, Conv_cur_tmt_base)
Conv_no_tmt_dir = "%s/%s" % (Rctm_root_dir, Conv_no_tmt_base)
Conv_rec_tmt_dir = "%s/%s" % (Rctm_root_dir, Conv_rec_tmt_base)
Current_plan_dir = "%s/%s" % (Rctm_root_dir, Current_plan_base)
Site_plans_dir = "%s/%s" % (Rctm_root_dir, Site_plans_base)
Reset_rc_init_dir = "%s/%s" % (Rctm_root_dir, Reset_rc_init_base)

User_treatment_dir = "%s/%s" % (Rctm_root_dir, User_treatment_base)

Fcst_hist_root_dir =  "%s/fcst_hist" % Rctm_root_dir
Fcst_hist_dump_meso_dir = "%s/%s" % (Fcst_hist_root_dir, Dump_meso_base)
Fcst_hist_concat_meso_dir = "%s/%s" % (Fcst_hist_root_dir, Concat_meso_base)
Fcst_hist_dump_fcst_dir = "%s/%s" % (Fcst_hist_root_dir, Dump_fcst_base)
Conv_fcst_hist_dir = "%s/%s" % (Fcst_hist_root_dir, Conv_fcst_hist_base)

#
# MDSS secondary directory layers
#
Web_perm_dir = "%s/%s" % (Static_dir, "web_perm")

#
# MDSS lookup table for location of files
#
dataLocation = {
                "wx" :  Conv_wx_dir,
                "nt" :  Conv_no_tmt_dir,
                "st" :  Conv_rec_tmt_dir,
                "ct" :  Conv_cur_tmt_dir,
                "rwis": Conv_meso_dir,
                "oh" :  Conv_fcst_hist_dir,
                "avl":  Avl_dir,
                "nwsww": NwsWW_dir,
                "alert": Alert_dir,
                "cam" : Webcam_dir
                }

#
# MDSS cgi script lookup table for location of files with id option
#
def get_location(location, id=None):

   if location == "cam":
      if id:
         return "%s/%s" % (dataLocation["cam"], id)
      else:
         return "%s/%s" % (dataLocation["cam"], "meta")         
   else:
      return dataLocation[location]


# MDSS tx plan dir. Used by cgi scripts.
#
planDir = Site_plans_dir 

#
# MDSS user defined tx dir. Used by cgi scripts.
#
userDefinedDir = User_treatment_dir

#
# MDSS file containing host and site id information for sheds.
# Used by cgi scripts.
#
Sheds = "%s/sheds_users.asc" % Web_perm_dir
Sheds_view = "%s/sheds_users_view.asc" % Web_perm_dir
Sheds_select = "%s/sheds_users_select.asc" % Web_perm_dir

#
# MDSS file containing list of special users( users possibly
# granted permission to run a user defined tx plan)
# Used by cgi scripts.
#
Special_users = "%s/special_users.asc" % Web_perm_dir

#
# MDSS file containing user defined plan access level.
# Note: File contains just one string: "ALL", "SHEDS_ONLY",
# or "SPECIAL_USERS". Used by cgi scripts.
#
User_plan_access = "%s/user_plan_access_level.asc" % Web_perm_dir

#
# MDSS file which will act as a flag to run current tx. 
# (Created and removed as necessary.
#
Run_current_flag = "%s/run_current_flag" % Site_plans_dir 

#
# MDSS file which contains site IDs which need to be reset 
# before next model run.
#
resetSitesFile = "%s/tmp/resetSites.asc" % Rctm_root_dir
