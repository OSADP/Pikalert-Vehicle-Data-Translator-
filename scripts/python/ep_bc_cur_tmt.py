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

# Runs roadcond_mdss for bridges with the current-treatment option.

import sys,os
import rctm_path
import plan
import time
import tim

proc_script = "run_proc.py"
exec_cmd = "roadcond_mdss"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Config_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/config")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")
bridge_list = "%s/%s" % (Site_list_dir, "bridge_cond_sites.asc")
cdl_file = "%s/%s" % (Cdl_dir, "road_cond.cdl")
site_config_file = "%s/%s" % (Config_dir, "bridge_config.conf")
treatment_option = "2"

static_files = "%s %s %s %s %s" % (site_list, bridge_list, cdl_file, site_config_file, treatment_option)

Current_plan_dir  = "%s/%s" % (rctm_path.Rctm_root_dir, "current_plan")
Bridge_cond_init_dir  = "%s/%s" %  (rctm_path.Rctm_root_dir, "bc_init")
Rdwx_fcst_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "rdwx_fcst")
Obs_hist_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "obs_hist")

input_dirs = "%s %s %s %s" % (Current_plan_dir, Bridge_cond_init_dir, Obs_hist_dir, Rdwx_fcst_dir)

age = "86400 86400 86400 86400"

output_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "bc_cur_tmt")

log_base = "%s/%s" % (rctm_path.Log_dir, "bc_cur_tmt")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time)
except:
    fcst_time = time.time()
    date_str = ''

fcst_time = (int(fcst_time)/3600) * 3600


# The rc_* chain will do this.
#
# Make default plans as needed
#ret = plan.mk_def_plan(rctm_path.Rctm_root_dir, site_list)
# Create concatenated current plan file
#ret = plan.mk_current_plan(rctm_path.Rctm_root_dir)

use_def_mat_str = "-mat_default CONCRETE"

command = "%s %s -e %s -a %s -u %s -s %s -i %s -o %s -l %s -f %s" % (proc_script, date_str, exec_cmd, age, fcst_time, static_files, input_dirs, output_dir, log_base, use_def_mat_str)

tmp_dir = "%s/tmp" % output_dir
if os.path.isdir(tmp_dir) == 0:
        os.makedirs(tmp_dir)

os.chdir(tmp_dir)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)
