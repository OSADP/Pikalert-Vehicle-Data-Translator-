#!/usr/bin/env python

import sys
import os
import time
import vii_paths
from datetime import datetime
import log_msg

"""
Ingests Nevada data, converts to probe, groups, and then runs through the VDT and RWH modules.
"""

#This should be a multiple of the number of minutes this script is set to run in the crontab
group_interval = 15

def exec_command(command, logg):
   logg.write_info(command)
   ret = os.system(command)
   if (ret != 0):
       logg.write_error("Error running command: %s\n" % command)
       sys.exit(1)

def today():
   return datetime.utcnow().strftime('%Y%m%d')

################################################################
       
# Move raw Nevada data from ftp area to VDT area
def ingest_data(logg):
    proc_script = "get_raw_nevada_data.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "get_raw_nevada_data")
    config = "vii_config"
    
    command = "%s -l %s %s %s" % (proc_script, log_base, config, today())

    exec_command(command, logg)

################################################################

# Create Nevada netcdf file for each csv file
def raw_2_probe(logg):
    proc_script = "run_nevada2probe_message.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_nevada2probe_message")
    config = "vii_paths.py"
    indir = "/d2/vii/data/raw/nevada/"
    outdir = "/d2/vii/data/processed/nv_csv2nc"
    cdl = "/d2/vii/data/static/cdl/probe_message.cdl"

    command = "%s -l %s %s %s %s %s %s" % (proc_script, log_base, config, today(), indir, outdir, cdl)

    exec_command(command, logg)

################################################################
    
# Create fifteen minute Nevada files for VDT
def group(logg):
    if datetime.utcnow().minute % group_interval != 0:
      logg.write_info("Skipping group()")
      return
   
    proc_script = "group_nevada_probe_message_files.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "group_nevada_probe_message_files")
    days_ago = 2
    config = "vii_paths.py"
    cdl = "/d2/vii/data/static/cdl/probe_message.cdl"
    work_dir = "/d2/vii/data/tmp"
    indir = "/d2/vii/data/processed/nv_csv2nc"
    outdir = "/d2/vii/data/processed/nv_probe_message"

    command = "%s -l %s -d %s %s %s %s %s %s" % (proc_script, log_base, days_ago, config, cdl, work_dir, indir, outdir)

    exec_command(command, logg)
    
################################################################

def run_vdt4(logg):
    if datetime.utcnow().minute % group_interval != 0:
      logg.write_info("Skipping run_vdt4()")
      return
   
    proc_script = "run_vdt4_nv.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "run_vdt4_nv")

    command = "%s -l %s" % (proc_script, log_base)

    exec_command(command, logg)
    
################################################################

def run_rwh(logg):
    if datetime.utcnow().minute % group_interval != 0:
      logg.write_info("Skipping run_rwh()")
      return
   
    proc_script = "ep_run_rwh_nv.py"

    command = "%s" % (proc_script)

    exec_command(command, logg)

################################################################

def run_probe_to_json(logg):
    if datetime.utcnow().minute % group_interval != 0:
        logg.write_info("Skipping run_probe_to_json()")
        return
   
    proc_script = "probe_to_json.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "probe_to_json_nv")
    config = "backend_sys_path"
    state_abbrev = "nv"
    num_sec = "900"

    command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

    exec_command(command, logg)

################################################################

def run_rwa_to_json(logg):
    if datetime.utcnow().minute % group_interval != 0:
        logg.write_info("Skipping run_rwa_to_json()")
        return

    proc_script = "rwa_to_json.py"
    log_base = "%s/%s" % (vii_paths.LOG_DIR, "rwa_to_json_nv")
    config = "backend_sys_path"
    state_abbrev = "nv"
    num_sec = "900"

    command = "%s -l %s %s %s %s" % (proc_script, log_base, config, state_abbrev, num_sec)

    exec_command(command, logg)
    
################################################################

def main():
    logg = log_msg.LogMessage('/d2/vii/data/log/ep_run_nv_processes')
    ingest_data(logg)
    raw_2_probe(logg)
    group(logg)
    run_vdt4(logg)
    run_rwh(logg)
    run_probe_to_json(logg)
    run_rwa_to_json(logg)
    
if __name__ == '__main__':
    main()
