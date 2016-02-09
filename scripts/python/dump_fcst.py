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

# Script to run dump_fcst_data.

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
    print "Usage: %s [-h] [-d date] [-l log_base] [-t] [--bc_params_file file_name] [--bc_input_dir input_dir] wx_input_dir rc_input_dir output_dir wx_params_file rc_params_file site_list" % prog_name
    print "Description: Runs dump_fcst_data on the most recent wx, road, and/or bridge forecats file."
    print "-h\t\tprint this help message"
    print "-d date\t\tuse date (YYYYMMDD.HHMM) for current time."
    print "-l log_base\tlog file base"
    print "-t\t\ttesting only, do not execute commands"
    print "--bc_params_file file_name\t a dump_fcst_data params file for dumping bridge (bc) forecast data"
    print "--bc_input_dir input_dir\tbridge forecast file input directory (e.g. /d2/dicast/rctm/bc_rec_tmt)"
    print "wx_input_dir\tweather forecast file input directory (e.g. /d2/dicast/st/post_proc/final_fcst)"
    print "rc_input_dir\troad forecast file input directory (e.g. /d2/dicast/rctm/rc_rec_tmt)"
    print "output_dir\tthe output directory where the output file will be written (e.g. /d2/dicast/rctm/dump_fcst)"
    print "wx_params_file\ta dump_fcst_data params file for dumping wx forecast data"
    print "rc_params_file\ta dump_fcst_data params file for dumping road (rc) forecast data"
    print "site_list\tdicast site list containing the list of sites to dump data for"


if __name__ == "__main__":
    testing = 0
    utime = 0
    log_base = 0
    bc_params_file = ""
    bc_input_dir = ""

    # Determine usage
    optlist = []
    args = []
    try:
	optlist, args = getopt.getopt(sys.argv[1:], "d:l:ht", ["bc_params_file=", "bc_input_dir="])
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
        elif optlist[i][0] == "--bc_params_file":
            bc_params_file = optlist[i][1]
        elif optlist[i][0] == "--bc_input_dir":
            bc_input_dir = optlist[i][1]

    if (len(args) != 6):
        print "Error: incorrect number of arguments"
        usage(sys.argv[0])
        sys.exit(2)

    wx_input_dir = args[0]
    rc_input_dir = args[1]
    out_dir = args[2]
    wx_params_file = args[3]
    rc_params_file = args[4]
    site_list = args[5]

    wx_params_str = "-params %s" % wx_params_file
    rc_params_str = "-params %s" % rc_params_file

    if(bc_params_file):
        bc_params_str = "-params %s" % bc_params_file
    else:
        bc_params_str = ""

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
    fcst_hr = curr_time[:2]
    
    cmd = "dump_fcst_data"
    
    # ********************** Dump Mesh Forecast ********************************
    
    # Get latest mesh input files
    index = index_file.Index_file(wx_input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    f, d = get_file.get_file(index, time_val - 86400, time_val, 1, 1)
    
    # If no input file, exit
    if (len(f) == 0):
        logf.write_time("Warning: No recent weather forecast file found.\n")
        wx_fcst_file = ""
    else:
        wx_fcst_file = f[0]

    if (wx_fcst_file != ""):

        # Make output path
        wx_out_dir = os.path.join(out_dir, "wx")
        dest_dir = os.path.join(wx_out_dir, curr_date)
        out_name = "%s.%s.%s.%s" % ("dump_fcst_wx", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
        wx_out_path = os.path.join(dest_dir, out_name)
        
        # Create command string
        command_str = "%s %s %s %s %s %s %s" % (cmd, wx_fcst_file, fcst_hr, site_list, wx_out_path, log_str, wx_params_str)
        #print command_str
        
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
            output_index = index_file.Index_file(wx_out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
            out_line = "%s %d" % (out_name, time_val)
            if (output_index != ""):
                output_index.write(out_line, time_val)
    
    else:
        wx_out_path = ""
    
    
    # ********************** Dump Road Cond Forecast ********************************

    # Get latest rc input files
    index = index_file.Index_file(rc_input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    f, d = get_file.get_file(index, time_val - 86400, time_val, 1, 1)
    
    # If no input file, exit
    if (len(f) == 0):
        logf.write_time("Warning: No recent road cond forecast file found.\n")
        rc_file = ""
    else:
        rc_file = f[0]
    
    if (rc_file != ""):
    
        # Make output path
        rc_out_dir = os.path.join(out_dir, "rc")
        dest_dir = os.path.join(rc_out_dir, curr_date)
        out_name = "%s.%s.%s.%s" % ("dump_fcst_rc", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
        rc_out_path = os.path.join(dest_dir, out_name)

        # Create command string
        command_str = "%s %s %s %s %s %s %s" % (cmd, rc_file, fcst_hr, site_list, rc_out_path, log_str, rc_params_str)
        #print command_str
    
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
            output_index = index_file.Index_file(rc_out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
            out_line = "%s %d" % (out_name, time_val)
            if (output_index != ""):
                output_index.write(out_line, time_val)

    else:
        rc_out_path = ""
    
    
    # ********************** Dump Bridge Cond Forecast ********************************

    bc_out_path = ""
    
    if ((bc_params_file != "") & (bc_input_dir != "")):

        # Get latest bc input files
        index = index_file.Index_file(bc_input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
        f, d = get_file.get_file(index, time_val - 86400, time_val, 1, 1)
    
        # If no input file, exit
        if (len(f) == 0):
            logf.write_time("Warning: No recent bridge cond forecast file found.\n")
            bc_file = ""
        else:
            bc_file = f[0]
    
        if (bc_file != ""):
    
            # Make output path
            bc_out_dir = os.path.join(out_dir, "bc")
            dest_dir = os.path.join(bc_out_dir, curr_date)
            out_name = "%s.%s.%s.%s" % ("dump_fcst_bc", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
            bc_out_path = os.path.join(dest_dir, out_name)

            # Create command string
            command_str = "%s %s %s %s %s %s %s" % (cmd, bc_file, fcst_hr, site_list, bc_out_path, log_str, bc_params_str)
            #print command_str
            
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
                output_index = index_file.Index_file(bc_out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
                out_line = "%s %d" % (out_name, time_val)
                if (output_index != ""):
                    output_index.write(out_line, time_val)
        
        else:
            bc_out_path = ""

    else:
        logf.write_time("Info: One or more bridge (bc) options not specified.\n")
    
    
    # ********************** Concatenate the mesh data file, road_cond data file and bridge_cond data file********************************

    tmp_dir = "tmp"
    tmp_file_base = "tmp_concat_fcst"
    tmp_path = "%s/%s" % (out_dir, tmp_dir)

    # Make tmp_path if needed
    if os.path.isdir(tmp_path) == 0:
        logf.write_time("Info: Making directory %s\n" % tmp_path)
        if not testing:
	    try:
                os.makedirs(tmp_path)
            except:
                logf.write_time("Error: Could not make directory %s\n" % tmp_path)
                sys.exit(1)
                
    tmp_file = "%s/%s.%s.%s.%s" % (tmp_path, tmp_file_base, curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
    #print tmp_file

    tmp_fo = open(tmp_file, "w")
    if (not tmp_fo):
        logf.write_time("Error: Unable to open tmp concatenated file %s.\n" % tmp_file)
        sys.exit(1)

    if(wx_out_path != ""):
        for line in fileinput.input(wx_out_path):
            #print line
            tmp_fo.write(line)

    if(rc_out_path != ""):
        for line in fileinput.input(rc_out_path):
            #print line
            tmp_fo.write(line)
    
    if(bc_out_path != ""):
        for line in fileinput.input(bc_out_path):
            #print line
            tmp_fo.write(line)
        
    tmp_fo.close()
    
    # Check and see if the tmp file is empty, if it is stop here
    ret = os.path.getsize(tmp_file)
    if ret == 0:
        logf.write_time("Warning: The forecast file is empty.\n")
        sys.exit(0)
    
    
    # ********************** Sort the concatenated file and output it ********************************

    dest_dir = os.path.join(out_dir, curr_date)
    out_name = "%s.%s.%s.%s" % ("dump_fcst", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
    out_path = os.path.join(dest_dir, out_name)
    sort_cmd = "sort -n"
    redirect_char = ">"
    command_str = "%s %s %s %s" % (sort_cmd, tmp_file, redirect_char, out_path)
    #print command_str

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
        

