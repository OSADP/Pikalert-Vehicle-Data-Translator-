#!/usr/bin/env python

import sys
import os
import vii_paths
import time

proc_script = "run_rwh.py"

log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_rwh_nv")

config_file = "%s/%s" % (vii_paths.CONFIG_DIR, "rwh_config_new.cfg")
fcst_road_seg_file = "%s/%s" % (vii_paths.SITE_LIST_DIR, "NV_logicast_road_sites.asc")
vdt_road_seg_file = "%s/%s" % (vii_paths.CDL_DIR, "nv_roads.20131111.nc")
state = "nv"
fcst_hours_out = 72

command = "%s -l %s %s %s %s %s %s" % (proc_script, log_base, config_file, fcst_road_seg_file, vdt_road_seg_file, state, fcst_hours_out)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

