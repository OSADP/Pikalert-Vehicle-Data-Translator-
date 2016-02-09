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

proc_script = "dump_fcst.py"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

log_base = "%s/%s_rc" % (rctm_path.Log_dir, rctm_path.Dump_fcst_base)

wx_input_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "rdwx_fcst")
bc_input_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "bc_init")
rc_input_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "rc_init")

output_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "dump_fcst")

bc_params_file = "%s/%s" % (Params_dir, "dump_fcst_data.bc.params")
wx_params_file = "%s/%s" % (Params_dir, "dump_fcst_data.wx.params")
rc_params_file = "%s/%s" % (Params_dir, "dump_fcst_data.rc.params")

site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time[:13])
except:
    fcst_time = time.time()
    date_str = ''


command = "%s %s -l %s --bc_params_file %s --bc_input_dir %s %s %s %s %s %s %s" % (proc_script, date_str, log_base, bc_params_file, bc_input_dir, wx_input_dir, rc_input_dir, output_dir, wx_params_file, rc_params_file, site_list)

#print "command = ", command

ret = os.system(command)
#ret = 0
if (ret != 0):
    sys.exit(1)

