#!/usr/bin/env python

"""Creates monthly statistics files based on daily statistics files"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: create_monthly_stats.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/10 16:03:15 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import glob
import csv
from operator import itemgetter

# create_monthly_stats.py 2013 06 Minnesota /ftp/restricted/ftp_vdt/root/download/minnesota/daily_statistics /ftp/restricted/ftp_vdt/root/download/minnesota/monthly_statistics

# Test
# create_monthly_stats.py 2013 07 Minnesota /d4/test/stats/mn_daily_stats /d4/test/stats/mn_monthly_stats

def get_hourly_stats_file_list(year, month, state, input_dir):

    glob_path = os.path.join(input_dir, "%s_%s%s*_hourly_stats.csv" % (state, year, month))
    file_list = glob.glob(glob_path)
    
    return file_list

def get_lat_lon_obstime_error_file_list(year, month, state, input_dir):

    glob_path = os.path.join(input_dir, "%s_%s%s*_lat_lon_obstime_error.csv" % (state, year, month))
    file_list = glob.glob(glob_path)
    
    return file_list

def get_lat_long_stats_file_list(year, month, state, input_dir):

    glob_path = os.path.join(input_dir, "%s_%s%s*_lat_lon_stats.csv" % (state, year, month))
    file_list = glob.glob(glob_path)
    
    return file_list

def create_monthly_lat_lon_stats(lat_lon_stats_file_list, year, month, state, input_dir, output_dir, logg):

    # For each lat/lon pair, add up total probe messages for the month
    lat_lon_dict = {}
    for filename in lat_lon_stats_file_list:
        reader = csv.reader(open(filename, "rb"))
        for index, row in enumerate(reader):
            if index != 0:
                if (row[0], row[1]) in lat_lon_dict:
                    lat_lon_dict[(row[0], row[1])] = lat_lon_dict[(row[0], row[1])] + int(row[2])
                else:
                    lat_lon_dict[(row[0], row[1])] = int(row[2])

    # Convert dictionary into a list
    tmp_lat_lon_output_list = []
    for key, value in lat_lon_dict.items():
        line = []
        line.append(key[0])
        line.append(key[1])
        line.append(value)
        tmp_lat_lon_output_list.append(line)

    # Sort and prepare for writing
    lat_lon_output_list = []
    sorted_tmp_lat_lon_list = sorted(tmp_lat_lon_output_list, key=itemgetter(0, 1))
    for item in sorted_tmp_lat_lon_list:
        line = ""
        for value in item:
            line = line + str(value) + ","
        line = line[:-1]
        lat_lon_output_list.append(line)        

    # Create output path and filename
    out_filename = "%s_%s%s_lat_lon_stats.csv" % (state, year, month)
    output_path = output_dir + "/" + out_filename
    logg.write_info("Writing lat_lon_stats monthly file for %s for %s%s to %s" % (state, year, month, output_path))
    f = open(output_path, "w")
    f.write("lat,lon,num_messages_in_lat_lon_pair\n")
    for line in lat_lon_output_list:
        f.write("%s\n" % (line))
    f.close()

def create_monthly_lat_lon_obstime_error_stats(lat_lon_obstime_error_file_list, year, month, state, input_dir, output_dir, logg):

    # Read the file and store the num_error_messages and total_num_messages for each probe message type
    lat_lon_obstime_error_dict = {}
    for filename in lat_lon_obstime_error_file_list:
        reader = csv.reader(open(filename, "rb"))
        for index, row in enumerate(reader):
            if index != 0:
                if (row[0]) in lat_lon_obstime_error_dict:
                    lat_lon_obstime_error_dict[row[0]] = [lat_lon_obstime_error_dict[row[0]][0] + int(row[1]), lat_lon_obstime_error_dict[row[0]][1] + int(row[2])]
                else:
                    lat_lon_obstime_error_dict[row[0]] = [int(row[1]), int(row[2])]

    # Calculate the percent error for each probe message type
    for key, value in lat_lon_obstime_error_dict.items():
        percent_error = float(value[0]) / value[1]
        value.append("%.3f" % percent_error)

    # Convert dictionary into a list
    lat_lon_obstime_error_list = []
    for key, value in lat_lon_obstime_error_dict.items():
        line = "%s, %d, %d, %.3f" % (key, int(value[0]), int(value[1]), float(value[2]))
        lat_lon_obstime_error_list.append(line)

    # Create output path and filename
    out_filename = "%s_%s%s_lat_lon_obstime_error.csv" % (state, year, month)
    output_path = output_dir + "/" + out_filename
    logg.write_info("Writing lat_lon_obstime_error monthly file for %s for %s%s to %s" % (state, year, month, output_path))
    f = open(output_path, "w")
    f.write("data_type,num_error_messages,total_num_messages,error\n")
    for line in lat_lon_obstime_error_list:
        f.write("%s\n" % (line))
    f.close()

def create_monthly_hourly_stats(hourly_stats_file_list, year, month, state, input_dir, output_dir, logg):

    # Read the file and store the min, max and average of the num_messages
    # and store the min, max, and average of the amount of hour_covered
    min_num_msgs_hourly_stats_dict = {}
    max_num_msgs_hourly_stats_dict = {}
    total_num_msgs_hourly_stats_dict = {}
    count_num_msgs_hourly_stats_dict = {}
    avg_num_msgs_hourly_stats_dict = {}

    min_hour_covered_hourly_stats_dict = {}
    max_hour_covered_hourly_stats_dict = {}
    total_hour_covered_hourly_stats_dict = {}
    count_hour_covered_hourly_stats_dict = {}
    avg_hour_covered_hourly_stats_dict = {}
    
    for filename in hourly_stats_file_list:
        reader = csv.reader(open(filename, "rb"))
        for index, row in enumerate(reader):
            if index != 0:
                # Store min, max, total, and counts for the number of messages
                if (row[0], row[2]) in min_num_msgs_hourly_stats_dict:
                    if min_num_msgs_hourly_stats_dict[(row[0], row[2])] > int(row[3]):
                        min_num_msgs_hourly_stats_dict[(row[0], row[2])] = int(row[3])
                else:
                    min_num_msgs_hourly_stats_dict[(row[0], row[2])] = int(row[3])
                if (row[0], row[2]) in max_num_msgs_hourly_stats_dict:
                    if max_num_msgs_hourly_stats_dict[(row[0], row[2])] < int(row[3]):
                        max_num_msgs_hourly_stats_dict[(row[0], row[2])] = int(row[3])
                else:
                    max_num_msgs_hourly_stats_dict[(row[0], row[2])] = int(row[3])
                if (row[0], row[2]) in total_num_msgs_hourly_stats_dict:
                    total_num_msgs_hourly_stats_dict[(row[0], row[2])] = total_num_msgs_hourly_stats_dict[(row[0], row[2])] + int(row[3])
                else:
                    total_num_msgs_hourly_stats_dict[(row[0], row[2])] = int(row[3])
                if (row[0], row[2]) in count_num_msgs_hourly_stats_dict:
                    count_num_msgs_hourly_stats_dict[(row[0], row[2])] += 1
                else:
                    count_num_msgs_hourly_stats_dict[(row[0], row[2])] = 1

                # Store min, max, total, and counts for the amount of hour covered
                if (row[0], row[2]) in min_hour_covered_hourly_stats_dict:
                    if min_hour_covered_hourly_stats_dict[(row[0], row[2])] > float(row[4]):
                        min_hour_covered_hourly_stats_dict[(row[0], row[2])] = float(row[4])
                else:
                    min_hour_covered_hourly_stats_dict[(row[0], row[2])] = float(row[4])
                if (row[0], row[2]) in max_hour_covered_hourly_stats_dict:
                    if max_hour_covered_hourly_stats_dict[(row[0], row[2])] < float(row[4]):
                        max_hour_covered_hourly_stats_dict[(row[0], row[2])] = float(row[4])
                else:
                    max_hour_covered_hourly_stats_dict[(row[0], row[2])] = float(row[4])
                if (row[0], row[2]) in total_hour_covered_hourly_stats_dict:
                    total_hour_covered_hourly_stats_dict[(row[0], row[2])] = total_hour_covered_hourly_stats_dict[(row[0], row[2])] + float(row[4])
                else:
                    total_hour_covered_hourly_stats_dict[(row[0], row[2])] = float(row[4])
                if (row[0], row[2]) in count_hour_covered_hourly_stats_dict:
                    count_hour_covered_hourly_stats_dict[(row[0], row[2])] += 1
                else:
                    count_hour_covered_hourly_stats_dict[(row[0], row[2])] = 1
                    
    # Get the average number of messages
    for key, value in total_num_msgs_hourly_stats_dict.items():
        if key in count_num_msgs_hourly_stats_dict:
            avg_num_msgs_hourly_stats_dict[key] = float(value) / count_num_msgs_hourly_stats_dict[key]

    # Get the average amount of hour covered
    for key, value in total_hour_covered_hourly_stats_dict.items():
        if key in count_hour_covered_hourly_stats_dict:
            avg_hour_covered_hourly_stats_dict[key] = float(value) / count_hour_covered_hourly_stats_dict[key]

    
    # Convert the dictionaries into a list:
    tmp_hourly_stats_list = []
    for key, value in min_num_msgs_hourly_stats_dict.items():
        if key in max_num_msgs_hourly_stats_dict:
            if key in avg_num_msgs_hourly_stats_dict:
                if key in min_hour_covered_hourly_stats_dict:
                    if key in max_hour_covered_hourly_stats_dict:
                        if key in avg_hour_covered_hourly_stats_dict:
                            line = []
                            line.append(key[0])
                            line.append(key[1])
                            line.append(value)
                            line.append(max_num_msgs_hourly_stats_dict[key])
                            line.append(avg_num_msgs_hourly_stats_dict[key])
                            line.append(min_hour_covered_hourly_stats_dict[key])
                            line.append(max_hour_covered_hourly_stats_dict[key])
                            line.append(avg_hour_covered_hourly_stats_dict[key])
                            tmp_hourly_stats_list.append(line)

    # Sort the list
    hourly_stats_list = []
    sorted_tmp_hourly_stats_list = sorted(tmp_hourly_stats_list, key=itemgetter(0,1))
    for item in sorted_tmp_hourly_stats_list:
        line = ""
        for value in item:
            line = line + str(value) + ","
        line = line[:-1]
        hourly_stats_list.append(line)

    # Create output path and filename
    out_filename = "%s_%s%s_hourly_stats.csv" % (state, year, month)
    output_path = output_dir + "/" + out_filename
    logg.write_info("Writing hourly stats monthly file for %s for %s%s to %s" % (state, year, month, output_path))
    f = open(output_path, "w")
    f.write("data_type,hour,min_num_messages,max_num_messages,avg_num_messages,min_hour_covered,max_hour_covered,avg_hour_covered\n")
    for line in hourly_stats_list:
        f.write("%s\n" % (line))
    f.close()

def create_monthly_stats(year, month, state, input_dir, output_dir, logg):
    """
    @type year: string
    @param year: year
    @type month: string
    @param month: month
    @type state: string
    @param state: state name
    @type input_dir: string
    @param input_dir: input directory
    @type output_dir: string
    @param output_dir: output directory 
    @rtype: number
    @return: 0 on success, -1 on failure
    """

    hourly_stats_file_list = get_hourly_stats_file_list(year, month, state, input_dir)
    lat_lon_obstime_error_file_list = get_lat_lon_obstime_error_file_list(year, month, state, input_dir)
    lat_lon_stats_file_list = get_lat_long_stats_file_list(year, month, state, input_dir)

    create_monthly_lat_lon_stats(lat_lon_stats_file_list, year, month, state, input_dir, output_dir, logg)
    create_monthly_lat_lon_obstime_error_stats(lat_lon_obstime_error_file_list, year, month, state, input_dir, output_dir, logg)
    create_monthly_hourly_stats(hourly_stats_file_list, year, month, state, input_dir, output_dir, logg)
    
    return 0       

def main():

    usage_str = "%prog year month state input_dir output_dir \n\tNote: year should be in the format YYYY; month should be in the format MM; state should be in the same format as the basename in the daily files (e.g. Minnesota)"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    year = args[0]
    month = args[1]
    state = args[2]
    input_dir = args[3]
    output_dir = args[4]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.set_suffix(".pyl")
    logg.write_starting("create_monthly_stats.py")

    create_monthly_stats(year, month, state, input_dir, output_dir, logg)

    logg.write_ending(0, "create_montly_stats.py")

if __name__ == "__main__":

   main()
