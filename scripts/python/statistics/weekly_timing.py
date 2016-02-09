#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: weekly_timing.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:08:33 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import time
import calendar
import glob
import ordered_dict
from optparse import OptionParser

# weekly_timing.py 20131006 20131012 /ftp/restricted/ftp_vdt/root/download/michigan/daily_statistics/ /ftp/restricted/ftp_vdt/root/download/michigan/weekly_statistics

def gen_date_list(begin_date, end_date):
    """Generates a list of dates of the form yyyymmdd from a being date to end date
    Inputs:
      begin_date -- such as "20070101"
      end_date -- such as "20070103"
    Returns:
      date_list -- such as ["20070101","20070102","20070103"]
    """
    begin_tm = time.strptime(begin_date, "%Y%m%d")
    end_tm = time.strptime(end_date, "%Y%m%d")
    begin_tv = calendar.timegm(begin_tm)
    end_tv = calendar.timegm(end_tm)
    date_list = []
    for tv in xrange(begin_tv, end_tv+86400, 86400):
        date_list.append(time.strftime("%Y%m%d", time.gmtime(tv)))
    return date_list                         

def get_timing_file_list(mydatelist, input_dir):

    timing_file_list = []
    file_list = os.listdir(input_dir)
    for mydate in mydatelist:
        file_name_string = "*timing_%s.txt" % mydate
        glob_path = os.path.join(input_dir, file_name_string)
        glob_list = glob.glob(glob_path)
        if glob_list != []:
            timing_file_list.append(glob_list[0])

    return timing_file_list

def weekly_michigan_timing(mydatelist, input_dir, output_file_path, logg):

    timing_file_dict = {}
    timing_file_list = get_timing_file_list(mydatelist, input_dir)
    for timing_file in timing_file_list:
        #print timing_file
        f = open(timing_file, "r")
        lines = f.read().splitlines()
        for line in lines:
            #print line
            line_split = line.split(":")
            late_key = line_split[1][1:]
            #print "late_key: ", late_key
            split_late_key = late_key.split(" ")
            #print split_late_key
            if split_late_key[3] == '5':
                split_late_key[3] = '05'
                late_key = " ".join(split_late_key)
            #print late_key
            msg_split = line_split[0].split(" ")
            num_msgs = int(msg_split[0])
            #print "num_msgs: ", num_msgs
            if late_key in timing_file_dict:
                timing_file_dict[late_key] += num_msgs
                #print "latter timing_file_dict[%s] = %s" % (late_key, timing_file_dict[late_key])
            else:
                timing_file_dict[late_key] = num_msgs
                #print "first timing_file_dict[%s] = %s" % (late_key, timing_file_dict[late_key])

            # Get total num msgs for week
            # Set up dictionary
    total_num_msgs = 0
    #print timing_file_dict.items()
    #sorted(timing_file_dict.keys(), key=lambda a:map(int,a.split(' ')))
    #sorted(timing_file_dict.keys(), key=lambda a: )
    #sorted(timing_file_dict.keys(), key=lambda a: [int(i) for i in a.split(' ')])
    #print timing_file_dict
    
    
    ordered_timing_file_dict = ordered_dict.OrderedDict(sorted(timing_file_dict.items()))
    for key, value, in ordered_timing_file_dict.iteritems():
        total_num_msgs = value + total_num_msgs
        
    f = open(output_file_path, 'a')
    for key, value in ordered_timing_file_dict.iteritems():
        percent_total_msgs = (float(value) / total_num_msgs) * 100
        msg = "%s msgs (%.2f%%): %s\n" % (value, percent_total_msgs, key)
        f.write(msg)
    f.close()
    
           
def main():

    usage_str = "%prog begin_date end_date input_dir output_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    begin_date = args[0]
    end_date = args[1]
    input_dir = args[2]
    output_dir = args[3]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting("weekly_timing.py")

    mydatelist = gen_date_list(begin_date, end_date)
    output_file_name  = "timing_%s_%s.txt" % (begin_date, end_date)
    output_file_path = os.path.join(output_dir, output_file_name)
    if os.path.exists(output_file_path):
        logg.write_info("%s exists already, removing" % output_file_path)
        cmd = "rm %s" % (output_file_path)
        os.system(cmd)
    weekly_michigan_timing(mydatelist, input_dir, output_file_path, logg)

    logg.write_ending(0, "weekly_timing.py")

if __name__ == "__main__":

   main()
