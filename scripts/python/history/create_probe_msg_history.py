#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import string
import time
import tim
import processed_file
import glob
import calendar
from optparse import OptionParser

PROC_SUCCESS = "success"


def create_probe_msg_history(cf, in_dir, out_dir, begin_time, end_time, logg):
    """
    Concatenates all probe message qc files in in_dir having their file name dates yyyymmddhhmm
       falling between begin_time and end_time and writes them to out_dir
    @return: 0 on success, -1 on failure
    """

    
    # Create the output file path
    delta_min = (end_time - begin_time) / 60
    begin_date = time.strftime("%Y%m%d", time.gmtime(begin_time))
    begin_hhmm = time.strftime("%H%M", time.gmtime(begin_time))
    # Output file name format:  history_probe_msg_qc.yyyymmdd.hhmm.delta_min.nc 
    output_file_name = "%s.%s.%s.%s.nc" % (cf.paths.probe_message_qc_history_base, begin_date, begin_hhmm, str(delta_min))
    output_file = os.path.join(out_dir, output_file_name)
    
    # Create a list of desired files
    file_list = []
    if begin_time != None:
        for update_time in xrange(begin_time, end_time, 300):
            date = time.strftime("%Y%m%d", time.gmtime(update_time))
            hhmm = time.strftime("%H%M", time.gmtime(update_time))

            dated_in_dir = os.path.join(in_dir, date)
            glob_path = os.path.join(dated_in_dir, "%s*%s*%s*nc" % (cf.paths.probe_message_qc_patterns, date, hhmm))
            file_list.extend(glob.glob(glob_path))
            #print file_list
            
    # Create the concatenation command using the ncrcat utility    
    command_string = "ncrcat "
    if file_list != []:
        for fname in file_list:
        
            if not os.path.exists(fname):
                logg.write_error("create_probe_msg_history: input file %s does not exist" % fname)
                logg.write_ending(1, "create_probe_msg_history.py")
                exit_value = 1
            elif not os.path.exists(out_dir):
                logg.write_error("create_probe_msg_history: output directory %s does not exist" % out_dir)
                logg.write_ending(1, "create_probe_msg_history.py")
                exit_value = 1

            statinfo = os.stat(fname)
            if (statinfo.st_size > 24600):
                command_string = command_string + fname + " "

        command_string = command_string + output_file
    else:
        logg.write_info("no input files were found")
        logg.write_ending(0, "create_probe_msg_history.py")
        exit(0)

    split_str = command_string.split()
    if len(split_str) > 2:

        # If the output file exists, remove it and create a new one
        if os.path.exists(output_file):
            rm_cmd = "rm %s" % output_file
            ret = log_msg.run_command(rm_cmd, logg)
            if ret != 0:
                logg.write_error("Error: The existing output file, %s, could not be removed" % output_file)
                logg.write_ending(1, "create_probe_msg_history.py")
                return 1

        ret = log_msg.run_command(command_string, logg)
        if ret != 0:
            logg.write_error("ncrcat error")
            logg.write_ending(1, "create_probe_msg_history.py")
            return 1
    else:
        logg.write_info("There are no non-empty files to concatenate at this time.")
         
    return 0       

def main():

    usage_str = "%prog config_file in_dir out_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-b", "--begin_time", dest="begin_time", help="start at particular time yyyymmddhhmm")
    parser.add_option("-e", "--end_time", dest="end_time", help="end at particular time yyyymmddhhmm (exclusive)")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    in_dir = args[1]
    out_dir = args[2]

    cf = __import__(config_file, globals(), locals(), [])

    pf = processed_file.ProcessedFile(cf.paths.PROCESSED_DIR, cf.paths.HISTORY_PROCESSED_BASE)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    begin_time = None
    end_time = None

    if options.begin_time:
        begin_time = tim.datetogmt(options.begin_time)
    if options.end_time:
        end_time = tim.datetogmt(options.end_time)

    if (begin_time != None and end_time == None) or (begin_time == None and end_time != None):
        print "Error: both begin_time and end_time must be set or neither should be set"
        sys.exit(2)

    if (begin_time > end_time):
        print "Error: end_time must be greater than begin_time"
        sys.exit(2)

    logg.set_suffix("%s" % cf.paths.LOG_PY_SUFFIX)
    logg.write_starting("create_probe_msg_history.py")
    
    create_probe_msg_history(cf, in_dir, out_dir, begin_time, end_time, logg)

    logg.write_ending(0, "create_probe_msg_history.py")

if __name__ == "__main__":

   main()
