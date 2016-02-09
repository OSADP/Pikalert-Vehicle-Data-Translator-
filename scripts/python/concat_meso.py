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
    print "Usage: %s [-h] [-d date] [-l log_base] [-t] input_dir output_dir" % prog_name
    print "Description: Runs dump_meso_data on most recent mesonet file or from time specified."
    print "-h\t\tprint this help message"
    print "-d date\t\tuse date (YYYYMMDD.HHMM) for current time."
    print "-l log_base\tlog file base"
    print "-t\t\ttesting only, do not execute commands"
    print "input_dir\tinput directory containing dump_meso files (e.g. /d2/dicast/rctm/dump_meso)"
    print "output_dir\troot output directory (e.g. /d2/dicast/rctm/concat_meso)"


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


    if (len(args) != 2):
        print "Error: incorrect number of arguments"
        usage(sys.argv[0])
        sys.exit(2)


    input_dir = args[0]
    out_dir = args[1]
    

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

    
    # Get dump_meso files
    #
    dump_meso_ifile = index_file.Index_file(input_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    max_days = rctm_path.NUM_OUTPUT_DAYS
    max_files = max_days * 24
    max_time_back = time_val - (max_days * 86400)
    #print max_time_back
    dump_meso_list, date_list = get_file.get_file(dump_meso_ifile, max_time_back, time_val, get_file.LAST, max_files)
    
    
    # Concatenate the hourly files into one file
    #
    logf.write_time("Starting.\n")
    
    tmp_dir = os.path.join(out_dir, "tmp")
    tmp_file_base = "tmp_concat_meso"
    tmp_file = "%s/%s.%s.%s.%s" % (tmp_dir, tmp_file_base, curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
    #print tmp_file

    # Make tmp_dir if needed
    if os.path.isdir(tmp_dir) == 0:
        logf.write_time("Info: Making directory %s\n" % tmp_dir)
        if not testing:
            try:
                os.makedirs(tmp_dir)
            except:
                logf.write_time("Error: Could not make directory %s\n" % tmp_dir)
                sys.exit(1)

    tmp_fo = open(tmp_file, "w")
    if (not tmp_fo):
        logf.write_time("Error: Unable to open tmp concatenated file %s.\n" % tmp_file)
        sys.exit(1)
    
    for file in dump_meso_list:
        logf.write_time("Concatenating data from %s.\n" % file)
        #print file
        for line in fileinput.input(file):
                #print line
                tmp_fo.write(line)
                
    tmp_fo.close()
    
    
    # Sort the concatenated file and output it
    #
    dest_dir = os.path.join(out_dir, curr_date)
    out_name = "%s.%s.%s.%s" % ("concat_meso", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
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
    

    logf.write_time("Ending. Exit status = 0.\n")
    sys.exit(0)
        
