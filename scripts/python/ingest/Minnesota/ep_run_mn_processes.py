#!/usr/bin/env python

import sys
import os
import time
import vii_paths
import log_msg

"""
Ingest Minnesota data, converts to probe, and then runs through the VDT and RWH modules
"""

def exec_command(command, logg):
   logg.write_info(command)
   ret = os.system(command)
   if (ret != 0):
       logg.write_error("Error running command: %s\n" % command)
       sys.exit(1)

################################################################
       
# Move raw Minnesota data from ftp area to VDT area
def ingest_data(logg):
    proc_script = "run_ameritrak_ingest.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_ameritrak_ingest")

    command = "%s -l %s" % (proc_script, log_base)

    exec_command(command, logg)

################################################################

# Create Minnesota netcdf file for each csv file
def raw_2_probe(logg):
    proc_script = "run_ameritrak2probe_message.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_ameritrak2probe_message")

    command = "%s -l %s" % (proc_script, log_base)

    exec_command(command, logg)

################################################################

# Combine the data from the WXT and probe ingests
def combine_wxt(logg):
   proc_script = "combine_mn_wxt.sh"
   
   command = "%s" % (proc_script,)
   
   exec_command(command, logg)
    
################################################################

def run_vdt4(logg):
    proc_script = "run_vdt4_mn.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_vdt4_mn")

    command = "%s -l %s" % (proc_script, log_base)

    exec_command(command, logg)
    
################################################################

def run_rwh(logg):
    proc_script = "ep_run_rwh_mn.py"

    command = "%s" % (proc_script)

    exec_command(command, logg)

################################################################

def run_probe_to_json(logg):
    proc_script = "probe_to_json.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "probe_to_json_mn")
    config = "backend_sys_path"
    state_abbrev = "mn"
    num_sec = "60"

    command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

    exec_command(command, logg)

################################################################

def run_rwa_to_json(logg):
    proc_script = "rwa_to_json.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "rwa_to_json_mn")
    config = "backend_sys_path"
    state_abbrev = "mn"
    num_sec = "60"

    command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

    exec_command(command, logg)
    
################################################################

def main():
    logg = log_msg.LogMessage('/d2/vii/data/log/ep_run_mn_processes')
    ingest_data(logg)
    raw_2_probe(logg)
#    combine_wxt(logg)
    run_vdt4(logg)
    run_rwh(logg)
    run_probe_to_json(logg)
    run_rwa_to_json(logg)
    
if __name__ == '__main__':
    main()
