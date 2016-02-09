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

# Script to run dump_meso_data.

import os
import getopt
import string
import sys
import re
import time
import fileinput
import tim
import log
import sys_path
import rctm_path
import index_file

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


def usage(prog_name):
    print "Usage: %s [-h] [-d date] [-l log_base] [-t] [-a] [-m site_map_file] meso_input_dir output_dir site_var_file site_list" % prog_name
    print "Description: Runs dump_meso_data on most recent mesonet file or from time specified."
    print "-h\t\t\tprint this help message"
    print "-d date\t\t\tuse date (YYYYMMDD.HHMM) for current time."
    print "-l log_base\t\tlog file base"
    print "-t\t\t\ttesting only, do not execute commands"
    print "-a\t\t\tarchived mode, allows for writing index files using historical dates" 
    print "-m site_map_file\ta file containg a list of dicast ids and a mapped icao-ids (from another site), obs will be output for the mapped icao-id"
    print "meso_input_dir\t\tdirectory where the decoded mesonet files are located (e.g. /d2/dicast/nt/dec_data/obs/mesonet)"
    print "output_dir\t\tdirectory where the dump_meso output files will be written (e.g. /d2/dicast/rctm/dump_meso)"
    print "site_var_file\t\tfile containing dicast_ids and their associated mesonet variables"
    print "site_list\t\tdicast site list"


if __name__ == "__main__":
    testing = 0
    utime = 0
    archived = 0
    log_base = 0
    site_map_file = 0
    
    # Determine usage
    optlist = []
    args = []
    try:
	optlist, args = getopt.getopt(sys.argv[1:], "d:l:m:hta")
    except:
	print "%s: error in parsing options, %s" % (sys.argv[0], sys.exc_value)
	usage(sys.argv[0])
	sys.exit(2)
	
    for i in xrange(0,len(optlist)):
	if optlist[i][0] == "-t":
            testing = 1
        elif optlist[i][0] == "-m":
            site_map_file = optlist[i][1]
        elif optlist[i][0] == "-a":
            archived = 1
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
    out_dir = args[1]
    site_var_file = args[2]
    site_list = args[3]
    

    # Set up site_map_file arg string
    if site_map_file:
        site_map_str = "-m %s" % site_map_file
    else:
        site_map_str = ""


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

    curr_hour = (int(time_val)/3600) * 3600
    meso_cmd = "dump_meso_data"
    
    # Find most recent input file to process, up to 1 hrs old
    in_file = ""
    for i in range(0,1):
        date = time.strftime("%Y%m%d", time.gmtime(time_val-i*3600))
        hhmm = time.strftime("%H00", time.gmtime(time_val-i*3600))
        file = "%s/%s/%s.%s.%s.%s" % (meso_input_dir, date, sys_path.Meso_base, date, hhmm, sys_path.NC_SUFFIX)

        if os.path.exists(file):
            in_file = file
	    break;

    if in_file == "":
        logf.write_time("Info: No recent mesonet files found for %s.%s.\n" % (date, hhmm))
        sys.exit(1)


    # Make output path
    dest_dir = os.path.join(out_dir, curr_date)
    out_name = "%s.%s.%s.%s" % ("dump_meso", curr_date, curr_time[:4], sys_path.ASC_SUFFIX)
    out_path = os.path.join(dest_dir, out_name)

    if not testing:
        make_dir(dest_dir, logf)
    
    # Create command string
    command = "%s %s %s %s %s %s %s" % (meso_cmd, in_file, site_var_file, site_list, out_path, site_map_str, log_str)
    
    # Execute the command
    logf.write_time("Executing: %s\n" % command)
    if not testing:
        ret = os.system(command)
        check_os_ret(ret, 1, logf)


    # Update the output index file
    output_index = index_file.Index_file(out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
    
    file_isvalid = 0
    hour_exists = 0
    
    if (output_index != ""):
        index_file_path = index_file.index_file_path(out_dir, sys_path.Index_base, curr_date)

        if os.path.isfile(index_file_path):
            file_isvalid = 1
            index_file_size = os.path.getsize(index_file_path)
                
        if (file_isvalid == 1 and index_file_size != 0):

            for line in fileinput.input(index_file_path):
                parts = line.split()
                file_utime = parts[1]
                file_hour = (int(file_utime)/3600) * 3600
                file_hour_str = time.strftime("%H%M", time.gmtime(file_hour))
                if(file_hour == curr_hour):
                    logf.write_time("Info: an entry for the current hour: %s already exists, not adding new entry to index file.\n" % file_hour_str)
                    hour_exists = 1
                    
            # Update index file for current hour file if an entry does not already exist
            if not testing:
                if hour_exists == 0 or archived == 1:
                    out_line = "%s %d" % (out_name, time_val)
                    output_index.write(out_line, time_val)
    
        else:
            # Create new index file 
            logf.write_time("Info: index_file_path: %s does not exist, creating new index file.\n" % index_file_path)
            if not testing:
                out_line = "%s %d" % (out_name, time_val)
                output_index.write(out_line, time_val)
                
    else:
        logf.write_time("Error: output_index object is empty.\n")
        sys.exit(1)
    
    
    sys.exit(0)
    
