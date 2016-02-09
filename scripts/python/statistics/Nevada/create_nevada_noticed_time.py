#!/usr/bin/env python

"""Adds a time stamp in the nevada csv file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: create_nevada_noticed_time.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:09:26 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import csv
import time
from optparse import OptionParser

# Test: ./create_nevada_noticed_time.py 20131017 /d3/jpresto/test/nevada_timing/raw/nevada/ /d3/jpresto/test/nevada_timing/raw/nevada_timing/

complete_raw_list = []
current_timing_dict = {}
header_dict = {}

def get_noticed_time_data(date, full_raw_input_path, full_timing_input_path, logg):

    if not os.path.exists(full_raw_input_path):
        logg.write_info("No raw data exists for %s in %s." % (date, raw_input_dir))
        return -1
    else:
        #print "Timing file: "
        if os.path.exists(full_timing_input_path):
            reader = csv.reader(open(full_timing_input_path, "rb"))
            for index,row in enumerate(reader):
                if (index != 0):
                    # Make the key be the row as a string minus the FirstNoticed value
                    key_string = ",".join(row[:-1])
                    if not key_string in current_timing_dict:
                        # Make the value be the row including the FirstNoticed value
                        current_timing_dict[key_string] = row
                    else:
                        logg.write_info("Duplicate data. Skipping.")
                       #print "not sure what to do here... this shouldn't happen"
        else:
            (timing_dir, timing_file) = os.path.split(full_timing_input_path)
            if not os.path.exists(timing_dir):
                os.mkdir(timing_dir)
            return 
        
def get_raw_data(date, full_raw_input_path, logg):

    file_mod_time = os.path.getmtime(full_raw_input_path)

    reader = csv.reader(open(full_raw_input_path, "rb"))
    for index, row in enumerate(reader):
        #print row
        row_with_noticed_time = []
        key_string = ",".join(row)
        if (index != 0):
            if not key_string in current_timing_dict:
                #print row
                HHMMSS_mod_time = time.strftime("%H:%M:%S", time.gmtime(file_mod_time))
                extended_row = row
                extended_row.append(HHMMSS_mod_time)
                #print "Need to process"
                current_timing_dict[key_string] = extended_row
        else:
            row_header = row
            row_header.append("FirstNoticed")
            #print row_header
            header_dict[key_string] = row_header
        #else:
        #    print "Already processed"



def main():

    usage_str = "%prog date raw_input_dir timing_input_dir\n\tNote: date must be in the format YYYYMMDD"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    date = args[0]
    raw_input_dir = args[1]
    timing_input_dir = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting("create_nevada_noticed_time.py")

    file_name = "nevada_%s.csv"  % (date)
    tmp_file_name = "nevada_%s.csv.tmp"  % (date)
    full_raw_input_path = os.path.join(raw_input_dir, date, file_name)
    full_timing_input_path = os.path.join(timing_input_dir, date, file_name)
    tmp_full_timing_input_path = os.path.join(timing_input_dir, date, tmp_file_name)

    ret = get_noticed_time_data(date, full_raw_input_path, full_timing_input_path, logg)
    if (ret == -1):
        return
    
    get_raw_data(date, full_raw_input_path, logg)

    # Write timing data

    writer = csv.writer(open(tmp_full_timing_input_path, "wb"))
    for key in header_dict.iterkeys():
        #print header_dict[key]
        writer.writerow(header_dict[key])
    for key in current_timing_dict.iterkeys():
        writer.writerow(current_timing_dict[key])

    cmd = "mv %s %s" % (tmp_full_timing_input_path, full_timing_input_path)
    ret = os.system(cmd)
    if (ret != 0):
        logg.write_error("Unable to execute: %s" % cmd)
    
    logg.write_ending(0, "create_nevada_noticed_time.py")

if __name__ == "__main__":

   main()
