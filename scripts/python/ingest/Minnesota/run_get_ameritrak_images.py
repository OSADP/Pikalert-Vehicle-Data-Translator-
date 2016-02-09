#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_get_ameritrak_images.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014-02-13 22:46:43 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import tim
import datetime

# Set end time five minutes earlier than current time.
SEC_DELAY = 300  

def run_get_ameritrak_images(yyyymmddhhmm, out_dir, time_interval, logg):

    # Make dated output directory
    yyyymmdd = yyyymmddhhmm[0:8]
    dated_out_dir = os.path.join(out_dir, yyyymmdd)
    if not os.path.exists(dated_out_dir):
        cmd = "mkdir %s" % (dated_out_dir)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("Unable to create directory %s" % (dated_out_dir))
            logg.write_ending(1, "run_get_ameritrak_images.py")

    os.chdir(dated_out_dir)

    # Form begin time and end time
    end_time_unix = tim.date2sec(yyyymmddhhmm)
    end_time_unix = end_time_unix - SEC_DELAY
    start_time_unix = end_time_unix - int(time_interval) # one minute
    value = datetime.datetime.fromtimestamp(start_time_unix)
    start_time = value.strftime('%Y%m%d%H%M')
    end_value = datetime.datetime.fromtimestamp(end_time_unix)
    end_time = end_value.strftime('%Y%m%d%H%M')

    # Call get_ameritrak_images
    #cmd = "get_ameritrak_images.py -l /d2/vii/data/log/get_ameritrak_images %s %s" % (start_time, yyyymmddhhmm)
    cmd = "get_ameritrak_images.py -l /d2/vii/data/log/get_ameritrak_images %s %s" % (start_time, end_time)
    logg.write_info("Executing: %s" % (cmd))
    ret = os.system(cmd)
    if ret != 0:
        logg.write_error("Unable to execute %s" % (cmd))
        logg.write_ending(1, "run_get_ameritrak_images.py")
    

def main():

    usage_str = "%prog YYYYMMDDHHMM output_directory time_interval_secs"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    yyyymmddhhmm = args[0]
    output_directory = args[1]
    time_interval = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    run_get_ameritrak_images(yyyymmddhhmm, output_directory, time_interval, logg)
    
    logg.write_ending()

if __name__ == "__main__":

   main()
