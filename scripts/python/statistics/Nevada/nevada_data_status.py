#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: nevada_data_status.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2013/10/22 22:51:49 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import nevada_reader
import data_statistics

STATE = "Nevada"
MIN_LAT = 34.5
MAX_LAT = 42.8
#MIN_LON = -120.8
#MAX_LON = -113.1
MIN_LON = -113.1
MAX_LON = -120.8

Canbus_lat_lon_error_count = "Canbus_lat_lon_error_count"
Canbus_obs_time_error_count = "Canbus_obs_time_error_count"

Vaisala_lat_lon_error_count = "Vaisala_lat_lon_error_count"
Vaisala_obs_time_error_count = "Vaisala_obs_time_error_count"

GPS_lat_lon_error_count = "GPS_lat_lon_error_count"
GPS_obs_time_error_count = "GPS_obs_time_error_count"

def get_trunc_hour_time(obstime):
    """Truncate obstime to nearest hour"""
    return((int(obstime)/3600) * 3600)

def get_trunc_minute_time(obstime):
    """Truncate obstime to nearest minute"""
    return((int(obstime)/60) * 60)
    
def parse_data(data_object, mapping_dict, data_dict, error_dict, logg):
    # Canbus: air_temp, bar_pressure
    # Vaisala: air_temp2, humidity, surface_temp
    # GPS: speed
    # obs_time, latitude, longitude, vehicle_id
    # all fields set to Out_missing = -9999 except obs_time which would equal []

    #for key in mapping_dict:
    #    print key
    #    for index, value in enumerate(data_object.get_field(key)):
    #        print index, value

    # Get the desired generic data
    latitude = data_object.get_field('latitude')
    longitude = data_object.get_field('longitude')
    obs_time = data_object.get_field('obs_time')
    vehicle_id = data_object.get_field('vehicle_id')

    # Get the desired canbus data
    air_temp = data_object.get_field("air_temp")
    bar_pressure = data_object.get_field("bar_pressure")

    # Get the desired vaisala data
    air_temp2 = data_object.get_field("air_temp2")
    humidity = data_object.get_field("humidity")
    surface_temp = data_object.get_field("surface_temp")

    # Get the desired GPS data
    speed = data_object.get_field("speed")

    for index, value in enumerate(latitude):

        # Set up data type dictionaries
        if not "Canbus" in data_dict:
            data_dict["Canbus"] = {}
        if not "Vaisala" in data_dict:
            data_dict["Vaisala"] = {}
        if not "GPS" in data_dict:
            data_dict["GPS"] = {}

        # Check for lat/lon bad values
        lat_lon_failure = False
        if ((value == -9999) or (longitude[index] == -9999)):
            #print "lat/lon == -9999: ", value, longitude[index]
            if Canbus_lat_lon_error_count in error_dict:
                error_dict[Canbus_lat_lon_error_count] += 1
            else:
                error_dict[Canbus_lat_lon_error_count] = 1
            if Vaisala_lat_lon_error_count in error_dict:
                error_dict[Vaisala_lat_lon_error_count] += 1
            else:
                error_dict[Vaisala_lat_lon_error_count] = 1
            if GPS_lat_lon_error_count in error_dict:
                error_dict[GPS_lat_lon_error_count] += 1
            else:
                error_dict[GPS_lat_lon_error_count] = 1
            lat_lon_failure = True
        else:
            if ((value == 0) or (longitude[index] == 0)):
                #print "lat/lon == 0: ", value, longitude[index]
                if Canbus_lat_lon_error_count in error_dict:
                    error_dict[Canbus_lat_lon_error_count] += 1
                else:
                    error_dict[Canbus_lat_lon_error_count] = 1
                if Vaisala_lat_lon_error_count in error_dict:
                    error_dict[Vaisala_lat_lon_error_count] += 1
                else:
                    error_dict[Vaisala_lat_lon_error_count] = 1
                if GPS_lat_lon_error_count in error_dict:
                    error_dict[GPS_lat_lon_error_count] += 1
                else:
                    error_dict[GPS_lat_lon_error_count] = 1
                lat_lon_failure = True

        # Check for obs_time bad values
        obs_time_failure = False
        if obs_time[index] == []:
            #print "obs_time_error: ", obs_time[index]
            if Canbus_obs_time_error_count in error_dict:
                error_dict[Canbus_obs_time_error_count] += 1
            else:
                error_dict[Canbus_obs_time_error_count] = 1
            if Vaisala_obs_time_error_count in error_dict:
                error_dict[Vaisala_obs_time_error_count] += 1
            else:
                error_dict[Vaisala_obs_time_error_count] = 1
            if GPS_obs_time_error_count in error_dict:
                error_dict[GPS_obs_time_error_count] += 1
            else:
                error_dict[GPS_obs_time_error_count] = 1
            obs_time_failure = True

        if lat_lon_failure or obs_time_failure:
            logg.write_info("Unable to process a line due to lat/lon (%s/%s) or obs_time (%s) bad values" % (value, longitude[index], obs_time[index]))
            continue


        # Set up canbus data dict
        canbus_dict_struct = {}
        canbus_dict_struct["vid"] = vehicle_id[index]
        canbus_dict_struct["obs_time"] = obs_time[index]
        canbus_dict_struct["lat"] = value
        canbus_dict_struct["lon"] = longitude[index]
        canbus_dict_struct["air_temp"] = air_temp[index]
        canbus_dict_struct["bar_pressure"] = bar_pressure[index]
        
        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["Canbus"]:
            data_dict["Canbus"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["Canbus"][trunc_hour_time]:
            data_dict["Canbus"][trunc_hour_time][trunc_minute_time] = []
        data_dict["Canbus"][trunc_hour_time][trunc_minute_time].append(canbus_dict_struct)

        # Set up vaisala data dict
        vaisala_dict_struct = {}
        vaisala_dict_struct["vid"] = vehicle_id[index]
        vaisala_dict_struct["obs_time"] = obs_time[index]
        vaisala_dict_struct["lat"] = value
        vaisala_dict_struct["lon"] = longitude[index]
        vaisala_dict_struct["air_temp2"] = air_temp2[index]
        vaisala_dict_struct["humidity"] = humidity[index]
        vaisala_dict_struct["surface_temp"] = surface_temp[index]

        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["Vaisala"]:
            data_dict["Vaisala"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["Vaisala"][trunc_hour_time]:
            data_dict["Vaisala"][trunc_hour_time][trunc_minute_time] = []
        data_dict["Vaisala"][trunc_hour_time][trunc_minute_time].append(vaisala_dict_struct)

        # Set up gps data dict
        gps_dict_struct = {}
        gps_dict_struct["vid"] = vehicle_id[index]
        gps_dict_struct["obs_time"] = obs_time[index]
        gps_dict_struct["lat"] = value
        gps_dict_struct["lon"] = longitude[index]
        gps_dict_struct["speed"] = speed[index]

        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["GPS"]:
            data_dict["GPS"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["GPS"][trunc_hour_time]:
            data_dict["GPS"][trunc_hour_time][trunc_minute_time] = []
        data_dict["GPS"][trunc_hour_time][trunc_minute_time].append(gps_dict_struct)

        
        
        
def get_data(dated_in_dir, input_file_list, data_dict, error_dict, date, logg):

    file_list = os.listdir(dated_in_dir)
    for myfile in file_list:
        #print myfile
        file_path = os.path.join(dated_in_dir, myfile)
        nevada = nevada_reader.NevadaImo2(file_path, logg)
        parse_data(nevada, nevada.Field_dict, data_dict, error_dict, logg)
    
def get_statistics(data_dict, stat_dict, lat_lon_cell_dict):
    
    data_statistics.create_stat_dict(data_dict, stat_dict)
    data_statistics.create_lat_lon_cell_dict(data_dict, MIN_LAT, MAX_LAT, MIN_LON, MAX_LON, lat_lon_cell_dict)

def write_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, state, logg):
    
    data_statistics.output_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, state, logg)

def main():

    usage_str = "%prog date in_dir out_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    date = args[0]
    in_dir = args[1]
    out_dir = args[2]

    dated_in_dir = os.path.join(in_dir, date)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    input_file_list = []
    data_dict = {}
    error_dict = {}
    stat_dict = {}
    lat_lon_cell_dict = {}

    logg.write_starting("nevada_data_status.py")

    get_data(dated_in_dir, input_file_list, data_dict, error_dict, date, logg)
    get_statistics(data_dict, stat_dict, lat_lon_cell_dict)
    #print error_dict
    write_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, STATE, logg)

    logg.write_ending(0, "nevada_data_status.py")

if __name__ == "__main__":

   main()
