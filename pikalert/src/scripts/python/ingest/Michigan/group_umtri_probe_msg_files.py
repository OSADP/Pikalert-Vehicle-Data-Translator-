#!/usr/bin/env python

#!/usr/bin/env python

"""Create umtri probe message files grouped in a specific time period"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: group_umtri_probe_msg_files.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/06/26 19:57:29 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import glob
import processed_file
import tim
import time
import name_schema
import shutil
from netCDF4 import Dataset
import datetime
from pytz import timezone

#LOOKBACK_FILE_RECEIVED_INTERVAL_SEC = 1800  # 30 minutes
LOOKBACK_FILE_RECEIVED_INTERVAL_SEC = 900  # 15 minutes


def main():

    usage_str = "%prog config begin_time end_time cdl_file work_dir in_dir(s) out_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    begin_time = args[1]
    end_time = args[2]
    cdl_file = args[3]
    work_dir = args[4]
    in_dir_list = args[5:-1]
    out_dir = args[-1]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])
        
    logg.set_suffix("%s" % cf.LOG_PY_SUFFIX)
    logg.write_starting()

    out_name_schema = name_schema.Name_schema(cf.umtri_format, cf.umtri_patterns)

    file_list = []
    lookback_file_list = []

    print "begin_time, end_time: ", begin_time, end_time
    begin_unix_time = tim.datetogmt(begin_time)
    end_unix_time = tim.datetogmt(end_time)

    dated_out_dir = os.path.join(out_dir, begin_time[0:8])
    out_file_name = out_name_schema.make_name_utime(begin_unix_time, None)
    out_file_path = os.path.join(dated_out_dir, out_file_name)
    begin_unix_time -= 300 # need to be more lenient

    if not os.path.exists(dated_out_dir):
        cmd = "mkdir %s" % (dated_out_dir)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("mkdir error")
            logg.write_ending(1, "group_umtri_probe_msg_files.py")
                                 
    
    for index, in_dir in enumerate(in_dir_list):

        # Collect files to check
        for update_time in xrange(begin_unix_time, end_unix_time, 86400):
            date = time.strftime("%Y%m%d", time.gmtime(update_time))
            #print update_time, date, os.path.join(in_dir, "%s/*nc" % (date))
            glob_path = os.path.join(in_dir, "%s/*nc" % (date))
            file_list.extend(glob.glob(glob_path))

        file_list = sorted(file_list)
        
        # Check each file to determine if it was modified within LOOKBACK_FILE_RECEIVED_INTERVAL_SEC
        for fpath in file_list:
            #print fpath
            fname = fpath.split("/")
            fname = fname[-1]
            fname_split = fname.split("_")
            #print "fname_split: ", fname_split
            mmddyyyy = fname_split[1]
            hhmmss = fname_split[2]
            date = mmddyyyy[4:8] + mmddyyyy[0:2] + mmddyyyy[2:4]
            date_time = date + hhmmss
            #print date_time
            #unix_date_time = tim.datetogmt(date_time)
            # Need to convert MI local time to UTC unix time
            obs_time = datetime.datetime.strptime(date_time, "%Y%m%d%H%M%S")
            eastern = timezone("US/Eastern")
            utc = timezone("UTC")
            local_date_time = eastern.localize(obs_time)
            utc_date_time = local_date_time.astimezone(utc)
            unix_date_time = time.mktime(utc_date_time.timetuple())
            print "fpath time: ", unix_date_time
                        
            print end_unix_time, unix_date_time, (end_unix_time - unix_date_time)
            if ((end_unix_time - unix_date_time) <= LOOKBACK_FILE_RECEIVED_INTERVAL_SEC) and ((end_unix_time - unix_date_time) >= 0):
                print "appending: ", fpath
                lookback_file_list.append(fpath)
            else:
                print "NOT app: ", fpath
                
    file_list = lookback_file_list

    command_string = "ncrcat "
    if file_list != []:
    
        for fname in file_list:
                    
            if not os.path.exists(fname):
                logg.write_error("group_umtri_probe_msg_files: input file %s does not exist" % fname)
                logg.write_ending(1, "group_umtri_probe_msg_files.py")
                exit_value = 1

            command_string = command_string + fname + " "

        command_string = command_string + out_file_path

    else:
        logg.write_info("no input files were found")
        logg.write_ending(0, "group_umtri_probe_msg_files.py")
        exit(0)

    split_str = command_string.split()
    if len(split_str) > 2:

        # If the output file exists, remove it and create a new one
        if os.path.exists(out_file_path):
            rm_cmd = "rm %s" % out_file_path
            ret = log_msg.run_command(rm_cmd, logg)
            if ret != 0:
                logg.write_error("Error: The existing output file, %s, could not be removed" % out_file_path)
                logg.write_ending(1, "group_umtri_probe_msg_files.py")
                return 1

        ret = log_msg.run_command(command_string, logg)
        if ret != 0:
            logg.write_error("ncrcat error")
            logg.write_ending(1, "group_umtri_probe_msg_files.py")
            return 1
    else:
        logg.write_info("There are no files to concatenate at this time.")
         

    logg.write_ending(0, "group_umtri_probe_msg_files.py")

if __name__ == "__main__":

   main()
