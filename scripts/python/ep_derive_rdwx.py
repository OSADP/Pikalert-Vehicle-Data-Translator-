#!/usr/bin/env python

import sys,os
import sys_path
import obs
import time
import tim


proc_script = "run_proc.py"
exec_cmd = "derive_vars"

site_list = sys_path.Full_site_list_file
cdl_file =  "%s/%s" % (sys_path.Cdl_dir, "derive_rdwx.cdl")

static_files = "%s %s" % (site_list, cdl_file)

age = "86400"

input_dir = "%s/%s" % (sys_path.System_root_dir, "temporal_interp")
output_dir = "%s/%s" % (sys_path.System_root_dir, "derive_rdwx")

log_base = "%s/%s" % (sys_path.Log_dir, "derive_rdwx")

params_file = "%s/%s" % (sys_path.Params_dir, "derive_rdwx.params")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % date_time
except:
    fcst_time = (int(time.time())/3600) * 3600
    date_str = ''


command = "%s -e %s -a %s %s -u %s -s %s -i %s -o %s -l %s -p %s" % (proc_script, exec_cmd, age, date_str, fcst_time, static_files, input_dir, output_dir, log_base, params_file)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

