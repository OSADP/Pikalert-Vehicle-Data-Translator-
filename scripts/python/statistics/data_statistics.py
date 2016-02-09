#!/usr/bin/env python

"""Gather statistics on state's data"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: data_statistics.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/10 16:02:38 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import datetime
from optparse import OptionParser
from operator import itemgetter

MINUTES_PER_HOUR = 60

class LatLonBox(object):

    def __init__(self, min_lat, max_lat, min_lon, max_lon):
        """Initialize variables"""
        self.min_lat = float(min_lat)
        self.max_lat = float(max_lat)
        self.min_lon = float(min_lon)
        self.max_lon = float(max_lon)

    def in_box(self, lat_lon_pair):
        """Check to see if a given lat/lon is in the specified lat/lon box"""
        lat_in_box = False
        lon_in_box = False

        # Check latitude
        if (float(lat_lon_pair[0]) > self.min_lat) and (float(lat_lon_pair[0]) < self.max_lat):
            lat_in_box = True
        # Check longitude
        if (abs(float(lat_lon_pair[1])) > abs(self.min_lon)) and (abs(float(lat_lon_pair[1])) < (abs(self.max_lon))):
            lon_in_box = True

        if lat_in_box and lon_in_box:
            return True
        else:
            return False


def trunc(f, n):
    '''Truncates a float f to n decimal places without rounding'''
    slen = len('%.*f' % (n, f))
    return str(f)[:slen]

def create_stat_dict(data_dict, stat_dict):
    """Create a statistics dictionary"""
    
    for data_type in data_dict:
        #print "DATA_TYPE: ", data_type
        if not data_type in stat_dict:
            stat_dict[data_type] = {}          
        for hour in data_dict[data_type]:
            readable_hour = datetime.datetime.fromtimestamp(int(hour)).strftime('%H')
            #print "HOUR: ", readable_hour
            if not readable_hour in stat_dict[data_type]:
                stat_dict[data_type][readable_hour] = {}
            for minute in data_dict[data_type][hour]:
                readable_minute = datetime.datetime.fromtimestamp(int(minute)).strftime('%M')
                #print "MINUTE: ", readable_minute
                minute_msg_counter = 0
                if not readable_minute in stat_dict[data_type][readable_hour]:
                    stat_dict[data_type][readable_hour][readable_minute] = 0
                for dict_struct in data_dict[data_type][hour][minute]:
                    for key in dict_struct:
                        if key == "lat":
                            minute_msg_counter += 1
                            #print "minute_msg_counter += 1: ", minute_msg_counter
                            #print "KEY: ", key

                stat_dict[data_type][readable_hour][readable_minute] = minute_msg_counter
                #print "Setting data_type %s, hour %s, minute %s, to %d" % (data_type, readable_hour, readable_minute, minute_msg_counter)

def create_lat_lon_cell_dict(data_dict, min_lat, max_lat, min_lon, max_lon, lat_lon_cell_dict):
    """Create a lat/lon cell dictionary"""
    ll_obj = LatLonBox(min_lat, max_lat, min_lon, max_lon)
    lat_lon_pair = []                

    for data_type in data_dict:
        for hour in data_dict[data_type]:
            for minute in data_dict[data_type][hour]:
                for index, value in enumerate(data_dict[data_type][hour][minute]):
                    lat = data_dict[data_type][hour][minute][index]['lat']
                    lon = data_dict[data_type][hour][minute][index]['lon']
                    #print lat, lon
                    lat_tenth = trunc(lat, 1)
                    lon_tenth = trunc(lon, 1)
                    #print lat_tenth, lon_tenth
                    lat_lon_pair = (lat_tenth, lon_tenth)
                    #print ll_obj.in_box(lat_lon_pair)

                    if ll_obj.in_box(lat_lon_pair) and not lat_lon_pair in lat_lon_cell_dict:
                        lat_lon_cell_dict[lat_lon_pair] = 1
                    elif ll_obj.in_box(lat_lon_pair):
                        lat_lon_cell_dict[lat_lon_pair] += 1

    #print lat_lon_cell_dict

def get_data(stat_dict, error_dict, lat_lon_cell_dict, date, hourly_stats_output_list, lat_lon_obstime_error_output_list, lat_lon_cell_output_list):



    # Get hourly stats data and get daily totals for each data type

    tmp_hourly_stats_output_list = []
    daily_total_dict = {}

    for data_type in stat_dict:
        #print "data_type: ", data_type
        if not data_type in daily_total_dict:
            daily_total_dict[data_type] = 0
        msg_per_hour = 0
        for hour in stat_dict[data_type]:
            hour_counter = 0
            #print "hour: ", hour
            line = []
            minute_counter = 0
            for minute in stat_dict[data_type][hour]:
                minute_counter += 1
                msg_per_hour += stat_dict[data_type][hour][minute]
                hour_counter += stat_dict[data_type][hour][minute]
                #print "minute: ", minute
                #print "minute_counter: ", minute_counter
                #print "hour_counter: ", hour_counter
                #print "msg_per_hour: ", msg_per_hour
                #print "stat_dict[%s][%s][%s]: %d" % (data_type, hour, minute, stat_dict[data_type][hour][minute])
            hour_percent_covered = float(minute_counter) / MINUTES_PER_HOUR
            #print "percent_covered: ", hour_percent_covered
            #line = ["%s, %s, %s, %d, %.2f" % (data_type, date, hour, minute_counter, hour_percent_covered)]
            line.append(data_type)
            line.append(date)
            line.append(hour)
            #line.append(minute_counter)
            line.append(hour_counter)
            line.append("%.2f" % hour_percent_covered)
            #print line
            tmp_hourly_stats_output_list.append(line)
        daily_total_dict[data_type] = msg_per_hour
        #print "daily_total_dict[%s]: %d" % (data_type, daily_total_dict[data_type])
    #print tmp_hourly_stats_output_list
    #print daily_total_dict

    sorted_tmp_hourly_stats_output_list = sorted(tmp_hourly_stats_output_list, key=itemgetter(0, 2))
    for item in sorted_tmp_hourly_stats_output_list:
        line = ""
        for value in item:
            line = line + str(value) + ","
        line = line[:-1]
        hourly_stats_output_list.append(line)

    #print hourly_stats_output_list

    # Get lat/lon obstime error

    
    for key in error_dict:
        #print key
        split_key = (str(key)).split("_")
        data_type = split_key[0]
        #print "daily_total_dict: ", daily_total_dict
        if data_type in daily_total_dict:
            #print "data_type: ", data_type
            #print data_type, error_dict[key], daily_total_dict[data_type], error_dict[key] + daily_total_dict[data_type]
            total_num_messages = error_dict[key] + daily_total_dict[data_type]
            percent_error = float(error_dict[key]) / total_num_messages
            line = "%s, %d, %d, %.2f" % (key, error_dict[key], total_num_messages, percent_error)
            lat_lon_obstime_error_output_list.append(line)

    """
    for key in error_dict:
        line = "%s, %d" % (key, error_dict[key])
        lat_lon_obstime_error_output_list.append(line)
    """

    #print lat_lon_obstime_error_output_list

    # Get lat/lon stats

    tmp_lat_lon_cell_output_list = []
    for key in lat_lon_cell_dict:
        line = []
        #line = "%s, %d" % (key, lat_lon_cell_dict[key])
        line.append(key[0]) #lat
        line.append(key[1]) #lon
        line.append(lat_lon_cell_dict[key]) #count
        tmp_lat_lon_cell_output_list.append(line)
        #print line
        #lat_lon_cell_output_list.append(line)
    #print tmp_lat_lon_cell_output_list

    sorted_tmplat_lon_cell_output_list = sorted(tmp_lat_lon_cell_output_list, key=itemgetter(0, 1))
    for item in sorted_tmplat_lon_cell_output_list:
        line = ""
        for value in item:
            line = line + str(value) + ","
        line = line[:-1]
        lat_lon_cell_output_list.append(line)    

    #print lat_lon_cell_output_list
    

def write_data(hourly_stats_output_list, lat_lon_obstime_error_output_list, lat_lon_cell_output_list, date, state, out_dir, logg):

    hourly_stats_out_path_file = out_dir + "/" + state + "_" + date + "_hourly_stats.csv"
    lat_lon_obstime_error_out_path_file =  out_dir + "/" + state + "_" + date + "_lat_lon_obstime_error.csv"
    lat_lon_stats_out_path_file =  out_dir + "/" + state + "_" + date + "_lat_lon_stats.csv"

    if not os.path.exists(out_dir):
        cmd = "mkdir %s" % out_dir
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("There was a problem in creating the output directory: %s" % out_dir)    
            logfile.write_ending(1, "data_statistics.py")
            return 1

    # Write the hourly statistics file

    if os.path.exists(hourly_stats_out_path_file):
        cmd = "rm %s" % hourly_stats_out_path_file
        logg.write_info("The output file, %s, already exists.  Deleting and creating a new one." % hourly_stats_out_path_file)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("There was a problme in removing the exisitng output file: %s" % hourly_stats_out_path_file)    
            logfile.write_ending(1, "data_statistics.py")
            return 1

    fp_out = file(hourly_stats_out_path_file,"w")
    logg.write_info("Writing the hourly statistics output file %s" % (hourly_stats_out_path_file))
    fp_out.write("data_type,date,hour,num_messages,hour_covered\n")
    for line in hourly_stats_output_list:
        fp_out.write("%s\n" % line)
    fp_out.close()
    
    # Write the lat/lon obstime error file

    if os.path.exists(lat_lon_obstime_error_out_path_file):
        cmd = "rm %s" % lat_lon_obstime_error_out_path_file
        logg.write_info("The output file, %s, already exists.  Deleting and creating a new one." % lat_lon_obstime_error_out_path_file)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("There was a problme in removing the exisitng output file: %s" % lat_lon_obstime_error_out_path_file)    
            logfile.write_ending(1, "data_statistics.py")
            return 1

    fp_out = file(lat_lon_obstime_error_out_path_file,"w")
    logg.write_info("Writing the lat/lon obstime error output file %s" % (lat_lon_obstime_error_out_path_file))
    fp_out.write("data_type,num_error_messages,total_num_messages,error\n")
    for line in lat_lon_obstime_error_output_list:
        fp_out.write("%s\n" % line)
    fp_out.close()

    # Write the lat/lon stats file
    
    if os.path.exists(lat_lon_stats_out_path_file):
        cmd = "rm %s" % lat_lon_stats_out_path_file
        logg.write_info("The output file, %s, already exists.  Deleting and creating a new one." % lat_lon_stats_out_path_file)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("There was a problme in removing the exisitng output file: %s" % lat_lon_stats_out_path_file)    
            logfile.write_ending(1, "data_statistics.py")
            return 1

    fp_out = file(lat_lon_stats_out_path_file,"w")
    logg.write_info("Writing the lat/lon stats output file %s" % (lat_lon_stats_out_path_file))
    fp_out.write("lat,lon,num_messages_in_lat_lon_pair\n")
    for line in lat_lon_cell_output_list:
        fp_out.write("%s\n" % line)
    fp_out.close()

    


def output_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, state, logg):

    hourly_stats_output_list = []
    lat_lon_obstime_error_output_list = []
    lat_lon_cell_output_list = []

    get_data(stat_dict, error_dict, lat_lon_cell_dict, date, hourly_stats_output_list, lat_lon_obstime_error_output_list, lat_lon_cell_output_list)

    write_data(hourly_stats_output_list, lat_lon_obstime_error_output_list, lat_lon_cell_output_list, date, state, out_dir, logg)


def in_box_test():
    """Test the functionality of in_box(); Results should be False, False, False, True"""
    min_lat = 43.4
    max_lat = 49.7
    min_lon = -88.5
    max_lon = -97.7

    ll_obj = LatLonBox(min_lat, max_lat, min_lon, max_lon)

    pair = (23.1, -80.3)
    print ll_obj.in_box(pair)

    pair = (23.1, -90.3)
    print ll_obj.in_box(pair)

    pair = (45.1, -80.3)
    print ll_obj.in_box(pair)

    pair = (45.1, 90.3)
    print ll_obj.in_box(pair)


def main():
    in_box_test()

if __name__ == "__main__":

   main()
