#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import sys
import os
import vii_paths
import time

proc_script = "run_rwh.py"

log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_rwh_mn")

config_file = "%s/%s" % (vii_paths.CONFIG_DIR, "rwh_config.cfg")
fcst_road_seg_file = "%s/%s" % (vii_paths.SITE_LIST_DIR, "MN_logicast_road_sites.asc")
vdt_road_seg_file = "%s/%s" % (vii_paths.CDL_DIR, "mn_roads.20131111.nc")
state = "mn"
fcst_hours_out = 72

command = "%s -l %s %s %s %s %s %s" % (proc_script, log_base, config_file, fcst_road_seg_file, vdt_road_seg_file, state, fcst_hours_out)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

