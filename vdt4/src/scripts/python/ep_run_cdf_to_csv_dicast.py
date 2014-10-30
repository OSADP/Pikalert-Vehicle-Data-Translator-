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

proc_script = "run_cdf_to_csv_dicast.py"

log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_cdf_to_csv_dicast")

config_file = "%s/%s" % (vii_paths.CONFIG_DIR, "cdf_to_csv_dicast.cfg")
output_dir = "%s/%s" % (vii_paths.PROCESSED_DIR, "cdf_to_csv_dicast") 

command = "%s -l %s %s %s" % (proc_script, log_base, config_file, output_dir)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

