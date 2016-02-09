#!/usr/bin/env python

import sys,os
import sys_path
import time
import tim

def usage():
    print "Usage: %s model" % sys.argv[0]
    sys.exit(2)

if len(sys.argv) < 2:
    usage()

model = sys.argv[1]

proc_script = "mod_fcst.py"

input_dir = sys_path.Model_input_dir
site_list = sys_path.Full_site_list_file
cdl_file = sys_path.Mod_fcst_cdl

output_dir = "%s/%s_%s" % (sys_path.Mod_fcst_dir, model, "fcst")
log_base = "%s/%s_%s" % (sys_path.Log_dir, model, "mod_fcst")
params_file = "%s/%s" % (sys_path.Params_dir, "mod_fcst.params")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time[:13])
except:
    fcst_time = time.time()
    date_str = ''


command = "%s -l %s -p %s %s %s %s %s %s %s" % (proc_script, log_base, params_file, date_str, input_dir, model, site_list, cdl_file, output_dir)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)
