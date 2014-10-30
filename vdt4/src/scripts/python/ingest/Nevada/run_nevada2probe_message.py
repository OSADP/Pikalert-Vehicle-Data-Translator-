#!/usr/bin/env python

"""Convert nevada csv files to netcdf probe message format"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_nevada2probe_message.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/09/11 16:21:21 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import vii_paths
import glob

#sys.path.append(os.path.expanduser('/home/vii/scripts/python/'))


# run_nevada2probe_message.py -l /d2/vii/data/log/run_nevada2probe_message vii_paths.py 20130904 /d2/vii/data/raw/nevada/ /d2/vii/data/processed/nv_csv2nc /d2/vii/data/static/cdl/probe_message.cdl

# Test
# run_nevada2probe_message.py vii_paths.py 20130904 /d4/test/nevada/input/ /d4/test/nevada/output/ /d2/vii/data/static/cdl/probe_message.cdl

def run_nevada2probe_message(cf, date, input_dir, out_dir, cdl, logg):

    final_ret = 0

    # Prepare input and output directories
    dated_input_dir = input_dir + "/" + date

    dated_output_dir = out_dir + "/" + date
    if not os.path.exists(dated_output_dir):
        mkdir_command = "mkdir -p %s" % (dated_output_dir)
        ret = log_msg.run_command(mkdir_command, logg)
        if ret != 0:
            logg.write_error("mkdir dated output dir error")
            logg.write_ending(1, "run_nevada2probe_message.py")
            return 1    
    
    file_list = []
    glob_path = os.path.join(dated_input_dir, "*.csv")
    file_list = glob.glob(glob_path)

    for fname in file_list:

        # Check to see if file size is 0
        # If so, skip
        stat_info = os.stat(fname)
        if (stat_info.st_size == 0):
            logg.write_info("This file %s is empty and will not be processed. Continuing." % (fname))
            continue

        # Create output file path
        filename = fname.split('/')
        filename = filename[-1]
        split_filename = filename.split('.')
        split_filename[-1] = "nc"
        output_filename = '.'.join(split_filename)
        output_file_path = dated_output_dir + "/" + output_filename

        log_file = cf.LOG_DIR + "/" + "nevada2probe_message"

        nevada_ret = 0
        
        #cmd = "%s nevada2probe_message.py -l %s %s %s %s %d" % (cf.ENTHOUGHT_PYTHON, log_file, cdl, fname, output_file_path, nevada_ret)
        cmd = "nevada2probe_message.py -l %s %s %s %s %d" % (log_file, cdl, fname, output_file_path, nevada_ret)
        ret = log_msg.run_command(cmd, logg)
        
        if nevada_ret != 0:
            logg.write_error("error calling nevada2probe_message.py")
            logg.write_ending(1, "run_nevada2probe_message.py")
            return 1
        else:
            logg.write_info("nevada2probe_message ran successfully")

    logg.write_ending(final_ret, "run_nevada2probe_message.py")
    sys.exit(final_ret)


def main():

    usage_str = "%prog config_file date in_dir out_dir cdl \n\tNote: data is expected to be in dated directories in in_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 5:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    date = args[1]
    in_dir = args[2]
    out_dir = args[3]
    cdl = args[4]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("run_nevada2probe_message.py")
        
    logg.write_starting()

    if (len(date) != 8):
        logg.write_info("Date must be in the format YYYYMMDD")
        logg.write_time("Ending: exit status = 1.")
        sys.exit(1)

    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])

    run_nevada2probe_message(cf, date, in_dir, out_dir, cdl, logg)
    
    logg.write_ending()

if __name__ == "__main__":

   main()
