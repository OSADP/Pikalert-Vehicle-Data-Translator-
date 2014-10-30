#!/usr/bin/env python

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_cdf_to_csv_dicast.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2013/11/22 17:39:26 $           #
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

    parser = OptionParser(usage = "%prog [options] cdf_to_csv_config_file csv_output_dir")
    parser.add_option("-d","--date",dest="date",help="run cdf_to_csv_dicast application for specified date (YYYYMMDD.HHMM)")
    parser.add_option("-l","--log",dest="log",help="base name of log file")

    (options,args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    output_dir = args[1]
    
    ret = 0
    
    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("run_cdf_to_csv_dicast.py")
    
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

    # Make output dirs if necessary
    tmp_output_dir = "%s/tmp" % (output_dir)
    mkdir_path(output_dir)
    mkdir_path(tmp_output_dir)
    
    # Run cdf_to_cvs_dicast for all three states
    # then concatenate
    states = ["mi", "mn", "nv"]
    cat_str = ""
    for state in states:

        cap_state = ""
        if state == "mi":
            cap_state = "MI"
        elif state == "mn":
            cap_state = "MN"
        elif state == "nv":
            cap_state = "NV"
            
        fcst_site_list = "%s/%s_logicast_segment_sites_only.asc" % (vii_paths.SITE_LIST_DIR, cap_state)
        vdt_seg_file = "%s/%s_roads.20131111.nc" % (vii_paths.CDL_DIR, state)
        seg_stats_dir = "%s/%s_vdt_output" % (vii_paths.PROCESSED_DIR, state)

        #print "fcst_site_list: %s" % fcst_site_list
        #print "vdt_seg_file: %s" %  vdt_seg_file
        #print "seg_stats_dir: %s" % seg_stats_dir
        
        # Get the latest VDT segment-statistics file
        # Look back for the latest 5 minute file
        # Only look back 30 mintues to get the latest run
        # 
        nearest_5min_time = (int(time_val)/300) * 300
        max_lookback_time = nearest_5min_time - (30 * 60)
        loop_time = nearest_5min_time
        found = 0;

        while(loop_time >= max_lookback_time):
            file_date = time.strftime("%Y%m%d", time.gmtime(loop_time))
            file_hhmm = time.strftime("%H%M", time.gmtime(loop_time))

            file_base = "segment_statistics.%s.%s" % (file_date, file_hhmm)
            seg_stats_file = "%s/%s/%s.nc" % (seg_stats_dir, file_date, file_base)
            tmp_output_file = "%s/%s_%s.csv" % (tmp_output_dir, state, file_base)

            if os.path.exists(seg_stats_file):
                found = 1;
                break;
        
            loop_time = loop_time - 60;

        if(found):
            cmd_str = "cdf_to_csv_dicast %s %s %s %s %s" % (config_file, fcst_site_list, vdt_seg_file, seg_stats_file, tmp_output_file)
            # Run the command line
            ret = run_cmd(cmd_str, logg)
            if ret == 0:
                cat_str = cat_str + tmp_output_file + " "
            if ret != 0:
                logg.write_error("run_cmd() failed for cdf_to_csv_dicast cmd_str.")
        else:
            logg.write_warning("No recent segment-statistics file found in %s, not running cdf_to_csv_dicast for %s" % (seg_stats_dir, state))
        

    #print "cat_str = ", cat_str

    # Set up paths and command for concatenating the state csv files
    output_file = "all_segment_statistics.%s.%s.csv" % (curr_date, curr_hhmm)
    output_path = "%s/%s" % (output_dir, output_file)
    cmd_str = "cat %s > %s" % (cat_str, output_path)

    # Run the command line
    ret = run_cmd(cmd_str, logg)
    if ret !=0:
        logg.write_error("run_cmd() failed for cat cmd_str.")

    logg.write_ending(exit_status=ret, msg="run_cdf_to_csv_dicast.py")
    return ret

if __name__ == "__main__":
    main()
