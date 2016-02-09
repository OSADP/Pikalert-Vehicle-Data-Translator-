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

proc_script = "concat_meso.py"

log_base = "%s/%s_rc" % (rctm_path.Log_dir, rctm_path.Concat_meso_base)

input_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "dump_meso")
output_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "concat_meso")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time[:13])
except:
    fcst_time = time.time()
    date_str = ''


command = "%s %s -l %s %s %s" % (proc_script, date_str, log_base, input_dir, output_dir)

ret = os.system(command)

if (ret != 0):
    sys.exit(1)

