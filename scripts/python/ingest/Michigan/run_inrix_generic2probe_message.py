#!/usr/bin/env python

"""Convert INRIX csv files to netcdf probe message format"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_inrix_generic2probe_message.py,v $                   #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013-07-09 17:55:40 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import vii_paths
import processed_file
import glob

#sys.path.append(os.path.expanduser('/home/vii/scripts/python/'))

PROC_ATTEMPT = "attempt"
PROC_SUCCESS = "success"
(NO_REPROCESS, REPROCESS, ALWAYS_REPROCESS) = range(3)

#TEST
# run_inrix_generic2probe_message.py -l /d2/vii/data/log/run_inrix_generic2probe_message vii_paths.py 20140715 /d2/vii/data/raw/test/inrix/in /d2/vii/data/raw/test/inrix/out /d2/vii/data/static/cdl/probe_message.cdl

# run_inrix_generic2probe_message.py -l /d2/vii/data/log/run_inrix_generic2probe_message vii_paths.py 20140715 /d2/vii/data/raw/inrix /d2/vii/data/processed/mi_csv2nc /d2/vii/data/static/cdl/probe_message.cdl

# Test (old - umtri)
# run_umtri2probe_message.py vii_paths.py 20130701 /d4/test/umtri/input/ /d4/test/umtri/output/ /d2/vii/data/static/cdl/probe_message.cdl

def run_inrix_generic2probe_message(cf, date, input_dir, out_dir, cdl, logg):
    """
    @type cf: config file import
    @param cf:
    @type date: string
    @param date: date to process
    @type in_dir: string
    @param in_dir: input dir
    @type out_dir: string
    @param out_dir: output dir
    @type: string
    @param: cdl file path
    @type: string
    @param: log
    @return: 0 on success, -1 on failure
    """

    pf = processed_file.ProcessedFile(cf.PROCESSED_LOG_DIR, "processed_inrix_generic2nc")
    final_ret = 0

    # Prepare input and output directories
    dated_input_dir = os.path.join(input_dir, date)

    dated_output_dir = os.path.join(out_dir, date)
    if not os.path.exists(dated_output_dir):
        #if not os.path.exists(out_dir):
        mkdir_command = "mkdir -p %s" % (dated_output_dir)
        ret = log_msg.run_command(mkdir_command, logg)
        if ret != 0:
            logg.write_error("mkdir dated output dir error")
            logg.write_ending(1, "run_inrix_generic2probe_message.py")
            return 1    
    
    file_list = []
    glob_path = os.path.join(dated_input_dir, "*.csv")
    file_list = glob.glob(glob_path)

    reprocess = REPROCESS
    already_processed_ct = 0

    for fname in file_list:
        #print fname

        # Assess whether the file is to be processed using stored processing information
        
        if reprocess != ALWAYS_REPROCESS and pf.process_status(fname, date) != (processed_file.PROC_NONE, 0):
            # file has already been processed so continue
            #logg.write_info("%s has already been processed, skipping" % fname)
            already_processed_ct += 1
            continue

        logg.write_info("Writing attempt in processed file")
        #print "date, fname, PROC_ATTEMPT: ", date, fname, PROC_ATTEMPT
        pf.write_processed(date, fname, PROC_ATTEMPT)

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

        log_file = cf.LOG_DIR + "/" + "inrix_generic2probe_message"

        umtri_ret = 0
        
        #cmd = "%s generic2probe_message.py -l %s %s %s %s %d" % (cf.ENTHOUGHT_PYTHON, log_file, cdl, fname, output_file_path, umtri_ret)
        cmd = "generic2probe_message.py -l %s %s %s %s %d" % (log_file, cdl, fname, output_file_path, umtri_ret)
        ret = log_msg.run_command(cmd, logg)
        
        if umtri_ret != 0:
            logg.write_error("error calling generic2probe_message.py")
            logg.write_ending(1, "run_inrix_generic2probe_message.py")
            return 1
        else:
            logg.write_info("Writing success in processed file")
            pf.write_processed(date, fname, PROC_SUCCESS)

    logg.write_ending(final_ret, "run_inrix_generic2probe_message.py")
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
        logg = log_msg.LogMessage("run_inrix_generic2probe_message.py")
        
    logg.write_starting()

    if (len(date) != 8):
        logg.write_info("Date must be in the format YYYYMMDD")
        logg.write_time("Ending: exit status = 1.")
        sys.exit(1)

    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])

    run_inrix_generic2probe_message(cf, date, in_dir, out_dir, cdl, logg)
    
    logg.write_ending()

if __name__ == "__main__":

   main()
