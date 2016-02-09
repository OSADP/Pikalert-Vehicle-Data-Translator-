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

# Script that run obs_history to create input data for METRo.

import os
import fileinput
import getopt
import string
import sys
import re
import time
import tim
import log
import sys_path
import rctm_path
import index_file
import get_file


def usage(prog_name):
    print "Usage: %s [-h] [-d date] [-l log_base] [-t] concat_meso_dir dump_fcst_dir site_list output_dir" % prog_name
    print "Description: Runs obs_history on most recent dump_fcst and concat_meso files or from time specified."
    print "-h\t\tprint this help message"
    print "-d date\t\tuse date (YYYYMMDD.HHMM) for current time."
    print "-l log_base\tlog file base"
    print "-t\t\ttesting only, do not execute commands"
    print "concat_meso_dir\tthe directory where the concat_meso files are located (e.g. /d2/dicast/rctm/metro_input/metro_input_concat_meso)"
    print "dump_fcst_dir\tthe directory where the dump_fcst files are located (e.g. /d2/dicast/rctm/metro_input/metro_input_dump_fcst)"
    print "site_list\tthe dicast site list containing the obs history sites (e.g. /d2/dicast/rctm/static_data/site_list/road_cond_sites.asc)";
    print "output_dir\tthe directory where the output files will be written (e.g./d2/dicast/rctm/metro_input/metro_input_obs_hist)"
    

if __name__ == "__main__":
    testing = 0
    utime = 0
    log_base = 0

    # Determine usage
    optlist = []
    args = []
    try:
	optlist, args = getopt.getopt(sys.argv[1:], "d:l:ht")
    except:
	print "%s: error in parsing options, %s" % (sys.argv[0], sys.exc_value)
	usage(sys.argv[0])
	sys.exit(2)
	
    for i in xrange(0,len(optlist)):
	if optlist[i][0] == "-t":
            testing = 1
        elif optlist[i][0] == "-h":
            usage(sys.argv[0])
            sys.exit(2)
        elif optlist[i][0] == "-d":
            utime = optlist[i][1]
            if (len(utime) != 13):
		usage(sys.argv[0])
		sys.exit(2)
        elif optlist[i][0] == "-l":
            log_base = optlist[i][1]
    
    if (len(args) != 4):
        print "Error: incorrect number of arguments"
        usage(sys.argv[0])
        sys.exit(2)

    meso_input_dir = args[0]
    fcst_input_dir = args[1]
    site_list = args[2]
    out_dir = args[3]
    
    # Set up log file
    if log_base:
        logf = log.Log(log_base)
        logf.set_suffix(sys_path.LOG_PY_SUFFIX)
        log_str = "-l %s" % log_base
    else:
        logf = log.Log("")
        log_str = ""
    
    
    # Set up strings for user-supplied date and real-time
    if utime:
        curr_date = utime[:8]
        curr_time = utime[9:] + "00"
    else:
        curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
        curr_time = time.strftime("%H%M%S", time.gmtime(time.time()))


    # Create a Unix time from date/time info
    dt = "%s.%s" % (curr_date, curr_time)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)
    
    
    # Get latest concatenated obs files
    index = index_file.Index_file(meso_input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    f, d = get_file.get_file(index, time_val-(12*3600), time_val, 1, 1)
    # If no input file, set obs_file to "none"
    if (len(f) == 0):
        logf.write_time("Warning: No recent concat_meso file found.\n")
        obs_file = "none"
    else:
        obs_file = f[0]

    #print "obs_file = ", obs_file

    
    # Get the latest dump_fcst file
    index = index_file.Index_file(fcst_input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    f, d = get_file.get_file(index, time_val-(45*3600), time_val, 1, 1)
    # If no input file, exit
    if (len(f) == 0):
        logf.write_time("Error: No recent dump_fcst file found.\n")
        sys.exit(1)
    else:
        fcst_file = f[0]

    #print "fcst_file = ", fcst_file
    

    # Run obs_history
    
    exec_cmd = "obs_history"
    cdl_file = "%s/%s" %(rctm_path.Cdl_dir, "obs_history.cdl")
    dest_dir = os.path.join(out_dir, curr_date)
    out_name = "%s.%s.%s.%s" % (rctm_path.Obs_hist_base, curr_date, curr_time[:4], sys_path.NC_SUFFIX)
    out_path = os.path.join(dest_dir, out_name)

    # Create command string
    command_str = "%s %s %s %s %s %s %s %s" % (exec_cmd, time_val, obs_file, fcst_file, site_list, cdl_file, out_path, log_str)

    # Make dest_dir if needed
    if os.path.isdir(dest_dir) == 0:
        logf.write_time("Info: Making directory %s\n" % dest_dir)
        if not testing:
            try:
                os.makedirs(dest_dir)
            except:
                logf.write_time("Error: Could not make directory %s\n" % dest_dir)
                sys.exit(1)

    # Execute the command
    logf.write_time("Executing: %s\n" % command_str)
    if not testing:
        ret = os.system(command_str)
        if os.WIFSIGNALED(ret):
            logf.write_time("Error: Command failed due to signal %d.\n" % os.WTERMSIG(ret))
            sys.exit(1)
        elif os.WIFEXITED(ret):
            if os.WEXITSTATUS(ret) != 0:
                logf.write_time("Error: Command exited with status %d.\n" % os.WEXITSTATUS(ret))
                sys.exit(1)
    
    # Update index file
    if not testing:
        output_index = index_file.Index_file(out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
        out_line = "%s %d" % (out_name, time_val)
        if (output_index != ""):
            output_index.write(out_line, time_val)
    
                
    sys.exit(0)

