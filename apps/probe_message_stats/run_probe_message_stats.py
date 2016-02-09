#!/usr/bin/env python

"""Run probe message stats creating hourly files"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_probe_message_stats.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 18:32:17 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import glob

def run_probe_message_stats(base_dir, yyyymmdd, out_file):
    """
    Create hourly files
    """

    # probe_message.20130120.1540.nc
    for hour in range(24):
        path = os.path.join(base_dir, yyyymmdd, "probe_message.%s.%02d*.nc" % (yyyymmdd, hour))
        file_string = " ".join(glob.glob(path))
        #for fname in glob.glob(path):
        #    print fname
        command = "probe_message_stats probe_stats_config.cfg %s %s.%d.txt" % (file_string, out_file, hour)
        print command
        os.system(command)
    return 

def main():

    usage_str = "%prog base_dir yyyymmdd out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    #config_file = args[0]
    base_dir = args[0]
    yyyymmdd = args[1]
    out_file = args[2]
    
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    """
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])
    """

    run_probe_message_stats(base_dir, yyyymmdd, out_file)

if __name__ == "__main__":

   main()
