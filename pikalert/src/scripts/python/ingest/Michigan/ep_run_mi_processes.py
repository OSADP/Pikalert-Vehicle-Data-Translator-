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
import time
import vii_paths


# Move raw Michigan data from ftp area to VDT area

proc_script = "get_michigan_umtri_data.py"
log_base = "%s/%s" % (vii_paths.LOG_DIR, "get_michigan_umtri_data")
config = "vii_config"
ftp_dir = "/var/autofs/mnt/ftp/restricted/ftp_vdt/root/upload/michigan/"
output_dir = vii_paths.UMTRI_RAW_DIR

command = "%s -l %s %s %s %s" % (proc_script, log_base, config, ftp_dir, output_dir)

#print command

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################

# Create Michigan netcdf file for each csv file

proc_script = "run_umtri2probe_message.py"
log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_umtri2probe_message")
config = "vii_paths.py"
date = time.strftime("%Y%m%d")
input_dir = vii_paths.UMTRI_RAW_DIR
output_dir = vii_paths.MI_CSV2NC_DIR
cdl_file = "%s/%s" % (vii_paths.CDL_DIR, "probe_message.cdl") 

command = "%s -l %s %s %s %s %s %s" % (proc_script, log_base, config, date, input_dir, output_dir, cdl_file)

#print command

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################


# Group Michigan files for VDT

proc_script = "run_group_umtri_probe_message_files.py"
log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_group_umtri_probe_message_files")

command = "%s -l %s" % (proc_script, log_base)

#print command

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################


# Run vdt4 for MI

control_script = "vii_ep.py"
config = "vii_config"
proc_script1 = "run_vdt4_mi.py"
proc_script2 = "ep_run_rwh_mi.py"
num_sec = 60

command = "%s %s %s %s %s" % (control_script, config, proc_script1, proc_script2, num_sec)

#print command

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################


# Run probe to json

proc_script = "probe_to_json.py"
log_base = "%s/%s" % (vii_paths.LOG_DIR, "probe_to_json_mi")
config = "backend_sys_path"
state_abbrev = "mi"
num_sec = "60"

command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################

# Run rwa to json

proc_script = "rwa_to_json.py"
log_base = "%s/%s" % (vii_paths.LOG_DIR, "rwa_to_json_mi")
config = "backend_sys_path"
state_abbrev = "mi"
num_sec = "60"

command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

ret = os.system(command)
if (ret != 0):
    sys.exit(1)

################################################################

