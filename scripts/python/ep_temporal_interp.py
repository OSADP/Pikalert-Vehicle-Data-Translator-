#!/usr/bin/env python

import sys,os
import sys_path
import time
import tim


proc_script = "run_proc.py"
exec_cmd = "temp_interp"

site_list = sys_path.Full_site_list_file
cdl_file  = "%s/%s" % (sys_path.Cdl_dir, "derive_rdwx.cdl")
model = "nam_fcst"

static_files = "%s %s" % (site_list, cdl_file)

age = "86400"

input_dir = "%s/%s" % (sys_path.Mod_fcst_dir, model)
output_dir = "%s/%s" % (sys_path.System_root_dir, "temporal_interp")

log_base = "%s/%s" % (sys_path.Log_dir, "temporal_interp")

params_file = "%s/%s" % (sys_path.Params_dir, "temporal_interp.params")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % date_time
except:
    fcst_time = (int(time.time())/3600) * 3600
    date_str = ''


command = "%s -e %s -a %s %s -u %s -s %s -i %s -o %s -p %s -l %s" % (proc_script, exec_cmd, age, date_str, fcst_time, static_files, input_dir, output_dir, params_file, log_base)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

