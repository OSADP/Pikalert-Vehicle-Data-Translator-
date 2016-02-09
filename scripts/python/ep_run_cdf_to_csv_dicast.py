#!/usr/bin/env python

import sys
import os
import vii_paths
import time

proc_script = "run_cdf_to_csv_dicast.py"

log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_cdf_to_csv_dicast")

config_file = "%s/%s" % (vii_paths.CONFIG_DIR, "cdf_to_csv_dicast.cfg")
output_dir = "%s/%s" % (vii_paths.PROCESSED_DIR, "cdf_to_csv_dicast") 

command = "%s -l %s %s %s" % (proc_script, log_base, config_file, output_dir)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

