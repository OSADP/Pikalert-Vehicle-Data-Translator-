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

import sys,os
import rctm_path
import time
import tim

proc_script = "metro_input_obs_hist.py"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

concat_meso_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "concat_meso")
dump_fcst_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "dump_fcst")
site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")
output_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "obs_hist")

log_base = "%s/%s" % (rctm_path.Log_dir, rctm_path.Obs_hist_base)

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time[:13])
except:
    fcst_time = time.time()
    date_str = ''


command = "%s %s -l %s %s %s %s %s" % (proc_script, date_str, log_base, concat_meso_dir, dump_fcst_dir, site_list, output_dir)

ret = os.system(command)

if (ret != 0):
    sys.exit(1)

