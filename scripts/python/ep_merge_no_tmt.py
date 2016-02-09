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

# Runs merge to combine rc_no_tmt and bc_no_tmt

import sys,os
import rctm_path
import time
import tim

proc_script = "run_proc.py"
exec_cmd = "merge_var"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")
cdl_file = "%s/%s" %(Cdl_dir, "merge_rc_bc.cdl")

static_files = "%s %s" % (site_list, cdl_file)

Road_cond_no_tmt_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "fec_rc_no_tmt")
Bridge_cond_no_tmt_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "bc_no_tmt")

input_dirs = "%s %s" % (Road_cond_no_tmt_dir, Bridge_cond_no_tmt_dir)

age = "86400 86400"

output_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "merge_no_tmt")

log_base = "%s/%s" % (rctm_path.Log_dir, "merge_no_tmt")

params_file = "%s/%s" % (Params_dir, "merge_no_tmt.params")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time)
except:
    fcst_time = time.time()
    date_str = ''

fcst_time = (int(fcst_time)/3600) * 3600


command = "%s %s -e %s -a %s -u %s -s %s -i %s -o %s -l %s -p %s" % (proc_script, date_str, exec_cmd, age, fcst_time, static_files, input_dirs, output_dir, log_base, params_file)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)
