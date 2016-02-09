#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: michigan_timing.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:10:49 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import math
import ordered_dict
from optparse import OptionParser
import umtri_reader

# ./michigan_timing.py 20131010 /d2/vii/data/raw/umtri /ftp/restricted/ftp_vdt/root/download/michigan/daily_statistics/
# Michigan_timing_<date>.txt

csv_file_list = []
BIN_BY_NUM = 5
NUM_MIN_IN_HOUR = 60
NUM_MIN_IN_SEC = 60

def michigan_timing(dated_input_dir, bin_dict, output_file_path, logg):

    if os.path.exists(dated_input_dir):
        all_file_list = os.listdir(dated_input_dir)
        for myfile in all_file_list:
            if "csv" in myfile:
                csv_file_list.append(myfile)
    else:
        logg.write_error("The dated input directory %s does not exist." % (dated_input_dir))
    
    for myfile in csv_file_list:
        # Get the file's received time
        full_file_path = os.path.join(dated_input_dir, myfile)
        file_received_time = os.path.getmtime(full_file_path)

        # Read the file and get the obs_time
        umtri = umtri_reader.UmtriImo2(full_file_path, logg)
        if umtri.file_header_dict == {}:
            logg.write_error("Skipping file %s due to error" % (full_file_path))
            continue
        obs_time = umtri.get_field('obs_time')

        # Determine # of minutes late and put into bin
        for mytime in obs_time:
            if file_received_time < mytime:
                logg.write_error("Error with date or time provided in file %s because the obs_time is greater than the received time." % (full_file_path))
                continue
            
            time_diff_minutes = (file_received_time - mytime) / NUM_MIN_IN_SEC
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
        #lower_min_late = (num_min_late - BIN_BY_NUM) % NUM_MIN_IN_HOUR
        #print key, upper_min_bin, num_hours_late, num_min_late, value, percent_total_msgs
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
        

    logg.write_starting("michigan_timing.py")

    bin_dict = {}
    dated_input_dir = os.path.join(in_dir, date)
    output_file_name = "Michigan_timing_%s.txt" % date
    output_file_path = os.path.join(out_dir, output_file_name)
    if os.path.exists(output_file_path):
        logg.write_info("%s exists already, removing" % output_file_path)
        cmd = "rm %s" % (output_file_path)
        os.system(cmd)
    
    michigan_timing(dated_input_dir, bin_dict, output_file_path, logg)

    logg.write_ending(0, "michigan_timing.py")

if __name__ == "__main__":

   main()
