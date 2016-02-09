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
import sys_path
import rctm_path
import obs
import time
import tim

proc_script = "run_proc.py"
exec_cmd = "fec"

Site_list_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/site_list")
Cdl_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/cdl")
Params_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "static_data/params")

input_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "rc_no_tmt")

site_list = "%s/%s" % (Site_list_dir, "road_cond_sites.asc")
cdl_file = "%s/%s" % (Cdl_dir, "road_cond.cdl")
nbr_file = "%s/%s" % (Site_list_dir, "pp_nbr.nc")

age = "86400 86400"

obs_dir = "None"
concat_meso_dir = "%s/%s" % (rctm_path.Rctm_root_dir, "concat_meso")

output_dir = "%s/%s" %  (rctm_path.Rctm_root_dir, "fec_rc_no_tmt")

log_base = "%s/%s" % (rctm_path.Log_dir, "fec_rc_no_tmt")

params_file = "%s/%s" % (Params_dir, "fec.rc.params")

try:
    date_time = os.environ['TEST_MODE_TIME']
    date_tup = time.strptime("%s" % (date_time),"%Y%m%d.%H%M%S")
    fcst_time = tim.mkgmtime(date_tup)
    date_str = "-d %s" % (date_time)
except:
    fcst_time = time.time()
    date_tup = time.gmtime(fcst_time)
    date_time = time.strftime("%Y%m%d.%H%M%S", date_tup)
    date_str = ''

fcst_time = (int(fcst_time)/3600) * 3600


static_files = "%s %s %s" % (site_list, nbr_file, cdl_file)

concat_meso_file = obs.get_concat_meso_file(concat_meso_dir, date_time)
if(os.path.exists(concat_meso_file)):
    concat_meso_str = "-f -c %s" % concat_meso_file
else:
    concat_meso_str = ""

command = "%s %s -e %s -a %s -u %s %s -s %s -i %s %s -o %s -p %s -l %s %s" % (proc_script, date_str, exec_cmd, age, fcst_time, fcst_time, static_files, obs_dir, input_dir, output_dir, params_file, log_base, concat_meso_str)

#print "command = ", command

ret = os.system(command)
#ret = 0
if (ret != 0):
    sys.exit(1)
else:
    sys.exit(0)

