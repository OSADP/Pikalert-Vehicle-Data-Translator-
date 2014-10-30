#!/usr/bin/env python

"""Get dataset"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: dataset.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/02/28 21:00:31 $           #
#                                                                                #
# ============================================================================== #

import backend_sys_path
import log_msg
import os
import processed_file
import sys
from optparse import OptionParser

def get_dataset(input_dir, base_name, yyyymmddhhmm, suffix):
    """
    Parameters
    ----------
    cf : state dictionary such as sys_path.State_dictionary[state]
    yyyymmddhhmm : data time string in form yyyymmddhhmm

    Returns
    -------
    file_path : string
        Empty string if no reasonable path available
    """

    print input_dir
    proc_file = processed_file.ProcessedFile(input_dir, "Processed")
    yyyymmdd = yyyymmddhhmm[0:8]
    hhmm = yyyymmddhhmm[8:12]
    output_file = "%s.%s.%s.%s" % (base_name, yyyymmdd, hhmm, suffix)
    print "gd: output_file: ", output_file
    proc_stat_tuple = proc_file.process_status(output_file, yyyymmdd)
    print "proc_stat_tuple[0]: ", proc_stat_tuple[0]
    if proc_stat_tuple[0] == processed_file.PROC_SUCCESS:
        dated_subdir = os.path.join(input_dir, yyyymmdd)
        output_path = os.path.join(dated_subdir, output_file)
        return output_path
    return ""

def main():

    usage_str = "%prog yyyymmddhhmm state"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    yyyymmddhhmm = args[0]
    state = args[1]
    
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        

    logg.write_starting()
    cf = backend_sys_path.State_dictionary[state]
    print get_dataset(cf.district_alerts_dir, backend_sys_path.district_alerts_base_name, yyyymmddhhmm)
    logg.write_ending()

if __name__ == "__main__":

   main()
