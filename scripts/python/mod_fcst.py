#!/usr/bin/env python

import os
import fileinput
import getopt
import string
import sys
import re
import time
import tim
import log
import get_file
import index_file
import sys_path


def usage(prog_name):
    print "Usage: %s [-h] [-d date] [-l log_base] [-p params_file] [-t] decoded_mdl_dir model_base, fcst_gen_hr site_list cdl_file modfcst_output_dir" % prog_name
    print "Description: Runs decoded2modfcst to convert ndfd decoded file to mod_fcst like file"
    print "-h\t\t\tprint this help message"
    print "-d date\t\t\tuse date (YYYYMMDD.HHMM) for current time."
    print "-l log_base\t\tlog file base"
    print "-t\t\t\ttesting only, do not execute commands"
    print "decoded_mdl_dir\tbase directory where the decoded model files are located (grib2site output files)"
    print "model_base\t\tbase name of model data"
    print "site_list\t\tdicast site list containing the list of sites ot output data for"
    print "cdl_file\t\tnetcdf configuration file used to setup the mod_fcst output file"
    print "output_dir\tbase directory where the output files will be written"


def check_os_ret(ret, exit_status, logf):
    if os.WIFSIGNALED(ret):
        logf.write("ERROR: Command failed due to signal %d.\n" % os.WTERMSIG(ret))
        if exit_status:
            sys.exit(1)
    elif os.WIFEXITED(ret):
        if os.WEXITSTATUS(ret) != 0:
            logf.write("ERROR: Command exited with status %d.\n" % os.WEXITSTATUS(ret))
            if exit_status:
                sys.exit(1)


def make_dir(dest_dir, logf):
    if os.path.isdir(dest_dir) == 0:
        logf.write_time("Info: Making directory %s\n" % dest_dir)
        try:
            os.makedirs(dest_dir)
        except:
            logf.write_time("Error: Could not make directory %s\n" % dest_dir)
            sys.exit(1)


if __name__ == "__main__":
    testing = 0
    utime = 0
    log_base = 0
    params_str = ""

    # Determine usage
    optlist = []
    args = []
    try:
	optlist, args = getopt.getopt(sys.argv[1:], "d:l:p:ht")
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
        elif optlist[i][0] == "-p":
            params_str = "-params %s" % optlist[i][1]    

    # Check number of command line args
    if (len(args) < 5):
        print "Error: incorrect number of arguments"
        usage(sys.argv[0])
        sys.exit(2)
    
    # Get the command line args
    input_dir = args[0]
    mdl_base = args[1]
    site_list = args[2]
    cdl_file = args[3]
    output_dir = args[4]
    exec_name = "decoded2modfcst"
    
    # Set up log file
    if log_base:
        logf = log.Log(log_base)
        logf.set_suffix(sys_path.LOG_PY_SUFFIX)
        log_str = "-l %s" % log_base
    else:
        logf = log.Log("")
        log_str = ""
   
    logf.write_time("Starting.\n")
    
    # Set up strings for user-supplied date and real-time
    if utime:
        curr_date = utime[:8]
        curr_time = utime[9:]
    else:
        curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
        curr_time = time.strftime("%H%M", time.gmtime(time.time()))

    # Create a Unix time from date/time info
    dt = "%s.%s" % (curr_date, curr_time)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)
                    

    output_index = index_file.Index_file(output_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)

    # loop over all possible hours
    for h in range(24):

        # Make the output path
        dest_dir = "%s/%s" % (output_dir, curr_date)
        out_name = "%s_fcst.%s.%02d00.nc" % (mdl_base, curr_date, h)
        output_file = "%s/%s" % (dest_dir, out_name)

        if output_index.file_processed(out_name, curr_date):
            #logf.write_time("Info: file processed: %s\n" % out_name)
            continue

        input_file = "%s/%s/%s/%s.%s.i%02d00.nc" % (input_dir, mdl_base, curr_date, mdl_base, curr_date, h)

        if os.path.exists(input_file):
            command = "%s %s %s %s %s %s %s" % (exec_name, input_file, site_list, cdl_file, output_file, params_str, log_str)

            # Make dest_dir if needed
            if not testing:
                make_dir(dest_dir, logf)
    
            logf.write_time("Executing: %s\n" % command)
            if not testing:
                ret = os.system(command)
                check_os_ret(ret, 1, logf)

            # Update index file
            if not testing:
                output_index = index_file.Index_file(output_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
                out_line = "%s %d" % (out_name, time_val)
                if (output_index != ""):
                    output_index.write(out_line, time_val)
    

    logf.write_time("Ending. Exit status = 0.\n")
    sys.exit(0)
    
