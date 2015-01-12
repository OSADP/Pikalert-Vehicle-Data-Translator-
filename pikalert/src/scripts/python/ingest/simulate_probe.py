#!/usr/bin/env python

"""Get latest file in probe message dated directory and update if not already processed"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: simulate_probe.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/06/18 20:19:50 $           #
#                                                                                #
# ============================================================================== #

import create_generic
import generic2probe_message
import log_msg
import os
import sys
from optparse import OptionParser
import time
import vii_paths

def simulate_probe(location, cdl_file, csv_dir, destination_base_dir, logg):
    """
    """

    last_processed = ""
    while 1:
        # Get current time
        current_time = time.time()
        if current_time % 60 > 10:
            current_time_string = time.strftime("%Y%m%d%H%M%s", time.gmtime(current_time))
            previous_time_string = time.strftime("%Y%m%d%H%M%s", time.gmtime(current_time-600))
            dated_subdir = os.path.join(destination_base_dir, current_time_string[0:8])

            # example file: umtri_probe_message.20140518.1955.nc
            latest_file = os.path.join(dated_subdir, "umtri_probe_message.%s.%s.nc" % (previous_time_string[0:8], previous_time_string[8:12]))
            csv_dated_subdir = os.path.join(csv_dir, current_time_string[0:8])
            if not os.path.exists(csv_dated_subdir):
                os.mkdir(csv_dated_subdir)
            csv_file = os.path.join(csv_dir, "umtri_probe_message.%s.%s.csv" % (current_time_string[0:8], current_time_string[8:12]))
            if csv_file == last_processed:
                time.sleep(5)
                continue
            logg.write_info("creating generic file: %s" % csv_file)
            if location == "belle_isle":
                create_generic.create_generic_belle_isle(current_time-60, csv_file)
            elif location == "belle_isle_clear":
                create_generic.create_generic_belle_isle_clear(current_time-60, csv_file)
            elif location == "belle_isle_low":
                create_generic.create_generic_belle_isle_low(current_time-60, csv_file)
            elif location == "belle_isle_high":
                create_generic.create_generic_belle_isle_high(current_time-60, csv_file)
            elif location == "mdot":
                create_generic.create_generic_mdot(current_time-60, csv_file)
            else:
                print "Error: selection %s is not available" % location
                return -1
            last_processed = csv_file
            logg.write_info("calling generic2probe using cdl_file, csv_file, latest_file: %s, %s, %s" % (cdl_file, csv_file, latest_file))
            generic2probe_message.generic2probe_message(cdl_file, csv_file, latest_file, logg)
            time.sleep(5)
        else:
            time.sleep(5)

def main():

    usage_str = "%prog location where location is belle_isle, belle_isle_low, belle_isle_high, mdot"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    location = args[0]
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    cdl_file = vii_paths.PROBE_MESSAGE_CDL
    logg.write_starting()
    simulate_probe(location, cdl_file, vii_paths.MI_SIMULATED_DIR, vii_paths.MI_PROBE_MESSAGE_DIR, logg)
    logg.write_ending()

if __name__ == "__main__":

   main()
