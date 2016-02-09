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
import sys_path
import rctm_path
import time
import tim

proc_script = "dump_meso.py"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

log_base = "%s/%s_rc" % (rctm_path.Log_dir, rctm_path.Dump_meso_base)

meso_input_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "dec_data/obs/mesonet")
output_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "dump_meso")

site_var_file = "%s/%s" % (Site_list_dir, "dump_meso_site_var_file.asc")
site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")


try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    cur_time = tim.mkgmtime(date_tup)
except:
    cur_time = time.time()

cur_date = time.strftime("%Y%m%d.%H%M", time.gmtime(cur_time))

one_hour_time = cur_time - 3600
one_hour_date = time.strftime("%Y%m%d.%H%M", time.gmtime(one_hour_time))

# Run for two hours back
#command = "%s -d %s -l %s %s %s %s %s" % (proc_script, two_hour_date, log_base, meso_input_dir, output_dir, site_var_file, site_list)
#ret = os.system(command)

# Run for one hour back
command = "%s -d %s -l %s %s %s %s %s" % (proc_script, one_hour_date, log_base, meso_input_dir, output_dir, site_var_file, site_list)
ret = os.system(command)

# Run for current hour
command = "%s -d %s -l %s %s %s %s %s" % (proc_script, cur_date, log_base, meso_input_dir, output_dir, site_var_file, site_list)
ret = os.system(command)
