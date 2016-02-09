#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: nevada_timing.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:10:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import csv
import tim
import math
import ordered_dict
from optparse import OptionParser

# ./nevada_timing.py 20131022 /d2/vii/data/raw/nevada_timing/ /ftp/restricted/ftp_vdt/root/download/nevada/daily_statistics/
# TEST
# ./nevada_timing.py 20131017 /d3/jpresto/test/nevada_timing/raw/nevada_timing/ /d3/jpresto/test/nevada_timing/raw/nevada_timing/

UTC_date_position = 1
UTC_obs_time_position = 2

BIN_BY_NUM = 5
NUM_MIN_IN_HOUR = 60
NUM_MIN_IN_SEC = 60

def nevada_timing(dated_input_path, bin_dict, output_file_path, logg):

    if not os.path.exists(dated_input_path):
        logg.write_error("The dated input file %s does not exist." % (dated_input_path))
    else:
        reader = csv.reader(open(dated_input_path, "rb"))
        for index, row in enumerate(reader):
            # Skip the header line
            if (index != 0):    
                # Get the obs time and convert to unix time
                UTC_obs_date = row[1]
                UTC_obs_time = row[2]
                date = "%s%s%s" % (UTC_obs_date[0:4], UTC_obs_date[5:7], UTC_obs_date[8:10])
                date_time = "%s%s%s%s" % (date, UTC_obs_time[0:2], UTC_obs_time[3:5], UTC_obs_time[6:8])
                obs_time = tim.datetogmt(date_time)

                # Get the noticed time and convert to unix time
                date_time = "%s%s%s%s" % (date, row[-1][0:2], row[-1][3:5], row[-1][6:8])
                noticed_time = tim.datetogmt(date_time)

                if noticed_time < obs_time:
                    logg.write_error("Error with date or time provided in file %s because the obs_time is greater than the noticed time." % (dated_input_path))
                    continue

                time_diff_minutes = (noticed_time - obs_time) / NUM_MIN_IN_SEC
                time_bin = math.ceil(time_diff_minutes / BIN_BY_NUM)

                if time_bin in bin_dict:
                    bin_dict[time_bin] += 1
                else:
                    bin_dict[time_bin] = 1

        # Get the total number of messages
        total_num_msgs = 0
        ordered_bin_dict = ordered_dict.OrderedDict(sorted(bin_dict.items()))
        for key, value in ordered_bin_dict.iteritems():
            total_num_msgs = value + total_num_msgs

        # Open output file and print information
        f = open(output_file_path, 'a')
        for key, value in ordered_bin_dict.iteritems():
            upper_min_bin = key * 5
            num_hours_late = (upper_min_bin - (upper_min_bin % NUM_MIN_IN_HOUR)) / NUM_MIN_IN_HOUR
            num_min_late = upper_min_bin % NUM_MIN_IN_HOUR
            percent_total_msgs = (float(value) / total_num_msgs) * 100
            msg = "%s msgs (%.2f%%): %s hr and %s min late\n" % (value, percent_total_msgs, int(num_hours_late), int(num_min_late))
            f.write(msg)
        f.close()
        
                
def main():

    usage_str = "%prog date in_dir out_dir \n\tNote: date should be in the format YYYYMMDD\n\t      in_dir should contain a list of dated directories"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    date = args[0]
    in_dir = args[1]
    out_dir = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        

    logg.write_starting("nevada_timing.py")

    bin_dict = {}
    input_file_name = "nevada_%s.csv" % (date)
    dated_input_path = os.path.join(in_dir, date, input_file_name)
    output_file_name = "Nevada_timing_%s.txt" % date
    output_file_path = os.path.join(out_dir, output_file_name)
    if os.path.exists(output_file_path):
        logg.write_info("%s exists already, removing" % output_file_path)
        cmd = "rm %s" % (output_file_path)
        os.system(cmd) 
    
    nevada_timing(dated_input_path, bin_dict, output_file_path, logg)

    logg.write_ending(0, "nevada_timing.py")

if __name__ == "__main__":

   main()
