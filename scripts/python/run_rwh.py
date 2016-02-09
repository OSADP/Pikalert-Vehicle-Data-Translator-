#!/usr/bin/env python

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_rwh.py,v $                                           #
#       Version: $Revision: 1.5 $  Dated: $Date: 2014/07/08 19:49:26 $           #
#                                                                                #
# ============================================================================== #

import log, log_msg
import os
import time
from optparse import OptionParser
import sys
import vii_paths
import subprocess
import os
import errno
import processed_file
import tim
import index_file
import get_file

def mkdir_path(path):
    try:
        os.makedirs(path)
    except os.error, e:
        if e.errno != errno.EEXIST:
            raise

def run_cmd(cmd, logg):
    logg.write_info(cmd)
    p = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    (stdout_str, stderr_str) = p.communicate()
    ret = p.returncode
    if ret != 0:
        logg.write_error("failed executing: %s" % cmd)
        logg.write_ending(ret)
        sys.exit(1)
    return ret

def main():

    parser = OptionParser(usage = "%prog [options] rwh_config_file fcst_road_seg_file vdt_road_seg_file state forecast_hours_out")
    parser.add_option("-d","--date",dest="date",help="run rwh application for specified date (YYYYMMDD.HHMM)")
    parser.add_option("-l","--log",dest="log",help="base name of log file")

    (options,args) = parser.parse_args()

    if len(args) < 5:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    fcst_road_seg_file = args[1]
    vdt_road_seg_file = args[2]
    state = args[3]
    fcst_hours_out = int(args[4])

    ret = 0
    
    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("run_rwh.py")

    
    # Resolve date and time
    # Get optional date and time if we have it
    if options.date:
        date_time = options.date
        curr_date = date_time[:8]
        curr_hhmmss = date_time[9:] + "00"
        curr_hhmm = date_time[9:]
    else:
        curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
        curr_hhmmss = time.strftime("%H%M%S", time.gmtime(time.time()))
        curr_hhmm = time.strftime("%H%M", time.gmtime(time.time()))
    
    dt = "%s.%s" % (curr_date, curr_hhmmss)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)


    # Get the latest road-weather forecast file
    rdwx_dir = "%s/%s" % (vii_paths.FORECAST_DIR, "rdwx_fcst")
    index = index_file.Index_file(rdwx_dir, "index", 24, "r", 0)
    f, d = get_file.get_file(index, time_val-7200, time_val, 1, 1)
    road_wx_opt_str = ""
    if (len(f) == 0):
        logg.write_warning("No recent road-weather forecast file found in %s." % rdwx_dir)
    else:
        rdwx_file = f[0]
        road_wx_opt_str = "-w %s" % rdwx_file


    # Get the latest road-conditions forecast file
    road_cond_dir = "%s/%s" % (vii_paths.FORECAST_DIR, "merge_rec_tmt")
    index = index_file.Index_file(road_cond_dir, "index", 24, "r", 0)
    f, d = get_file.get_file(index, time_val-7200, time_val, 1, 1)
    road_cond_opt_str = ""
    if (len(f) == 0):
        logg.write_warning("No recent road-cond forecast file found in %s." % road_cond_dir)
    else:
        road_cond_file = f[0]
        road_cond_opt_str = "-r %s" % road_cond_file

    
    # Get the latest VDT segment-statistics file
    # Look back for the latest 5 minute file
    # Only look back 16 mintues to get the latest run
    # otherwise we do not run rwh
    #
    nearest_1min_time = (int(time_val)/60) * 60
    max_lookback_time = nearest_1min_time - 1860;
    loop_time = nearest_1min_time
    found = 0;
    
    seg_stats_state_dir = "%s/%s_vdt_output" % (vii_paths.PROCESSED_DIR, state)

    output_date = ""
    output_hhmm = ""
    while(loop_time >= max_lookback_time):
        file_date = time.strftime("%Y%m%d", time.gmtime(loop_time))
        file_hhmm = time.strftime("%H%M", time.gmtime(loop_time))
        
        seg_stats_file = "%s/%s/segment_statistics.%s.%s.nc" % (seg_stats_state_dir, file_date, file_date, file_hhmm)

        if os.path.exists(seg_stats_file):
            found = 1;
            output_date = file_date
            output_hhmm = file_hhmm
            break;
        
        loop_time = loop_time - 60;

    seg_stats_opt_str = ""
    if(not found):
        logg.write_warning("No recent segment-statistics file found in %s, running with just forecast files." % seg_stats_state_dir)
        #Fix to allow a default date otherwise output files will look like //co_rwh...nc
        output_date = time.strftime("%Y%m%d", time.gmtime())
        output_hhmm = time.strftime("%H%M", time.gmtime())
    else:
        seg_stats_opt_str = "-s %s" % seg_stats_file
        

    # If we don't have any of the input files, exit.
    if(road_wx_opt_str == "" and road_cond_opt_str == "" and seg_stats_opt_str == ""):
        logg.write_error("No recent road-wx, road-cond or seg-stats file found, can't run rwh.")
        logg.write_ending(exit_status=1, msg="run_rwh.py")
        sys.exit(1)


    # Get a list of previoius RWH files over the last 3 hours
    #
    prev_rwh_files = []
    output_state_dir = "%s/%s_rwh_output" % (vii_paths.PROCESSED_DIR, state)
    max_lookback_time = nearest_1min_time - (3 * 3600)
    loop_time = nearest_1min_time
    while(loop_time >= max_lookback_time):
        file_date = time.strftime("%Y%m%d", time.gmtime(loop_time))
        file_hhmm = time.strftime("%H%M", time.gmtime(loop_time))

        date_dir = "%s/%s" % (output_state_dir, file_date)
        rwh_file = "%s_rwh.%s.%s.nc" % (state, file_date, file_hhmm)
        rwh_path = "%s/%s" % (date_dir, rwh_file)
        
        if os.path.exists(rwh_path):
            #print "found rwh_path: %s" % rwh_path
            prev_rwh_files.append(rwh_path)
            
        loop_time = loop_time - 60;

    prev_rwh_files.sort()
    num_prev_rwh_files = len(prev_rwh_files)
    #print "num_prev_rwh_files = ", num_prev_rwh_files
    
    # Creat tmp directory and file for previous RWH file list
    tmp_dir = "%s/%s" % (output_state_dir, "tmp")
    prev_rwh_config_file = "%s/previous_rwh_conf.%s.%s.asc" % (tmp_dir, output_date, output_hhmm)

    # Make dated output dir if necessary
    mkdir_path(tmp_dir)

    prev_rwh_opt_str = ""
    if(num_prev_rwh_files > 0):
        prev_rwh_opt_str = "-p %s" % prev_rwh_config_file

        # Create previous rwh file list
        tmp_fh = open(prev_rwh_config_file, "w")
        if (not tmp_fh):
            logf.write_time("Error: Unable to open temporary file %s.\n" % prev_rwh_config_file)
            sys.exit(1)

        print "writing previous rwh files to %s" % prev_rwh_config_file
        for file in prev_rwh_files:
            file_str = "%s\n" % file
            tmp_fh.write(file_str)
        
        tmp_fh.close()
    else:
        logf.write_time("Info: no previous RWH files found.\n")


    #print "prev_rwh_opt_str: %s" % prev_rwh_opt_str

    
    # Setup command to run rwh
    #
    begin_time_val = time_val
    end_time_val_init = time_val + (fcst_hours_out * 3600)
    end_time_val = (int(end_time_val_init)/3600) * 3600
    
    rwh_begin_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(begin_time_val))
    rwh_end_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(end_time_val))
    
    output_dir = "%s/%s" % (output_state_dir, output_date)
    output_file = "%s_rwh.%s.%s.nc" % (state, output_date, output_hhmm)
    output_path = "%s/%s" % (output_dir, output_file)
    
    # Make dated output dir if necessary
    mkdir_path(output_dir)
    
    cmd_str = "rwh %s %s %s %s %s %s -l %s %s %s %s %s" % (config_file, rwh_begin_time_string, rwh_end_time_string, fcst_road_seg_file, vdt_road_seg_file, output_path, options.log, road_wx_opt_str, road_cond_opt_str, seg_stats_opt_str, prev_rwh_opt_str)
    
    # Setup Processed file object
    proc_file = processed_file.ProcessedFile(output_state_dir, "Processed")
    
    # Run the command line
    ret = run_cmd(cmd_str, logg)
    if ret == 0:
        proc_file.write_processed(curr_date, output_file, processed_file.PROC_SUCCESS)
    else:
        logg.write_error("run_cmd() failed for rwh command_str.")

    logg.write_ending(exit_status=ret, msg="run_rwh.py")
    return ret

if __name__ == "__main__":
    main()
