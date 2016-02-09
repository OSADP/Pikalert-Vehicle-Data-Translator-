#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: michigan_data_status.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:52:21 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import umtri_reader
import data_statistics

# TEST
# ./michigan_data_status.py 20131021 /d2/vii/data/raw/umtri /d3/jpresto/test/michigan_data_status/output/

STATE = "Michigan"
MIN_LAT = 40.4
MAX_LAT = 48.7
#MIN_LON = -91.4
#MAX_LON = -80.7
MIN_LON = -80.7
MAX_LON = -91.4

GPS_lat_lon_error_count = "GPS_lat_lon_error_count"
GPS_obs_time_error_count = "GPS_lat_lon_error_count"

Canbus_lat_lon_error_count = "Canbus_lat_lon_error_count"
Canbus_obs_time_error_count = "Canbus_lat_lon_error_count"

Surface_patrol_lat_lon_error_count = "Surface_patrol_lat_lon_error_count"
Surface_patrol_obs_time_error_count = "Surface_patrol_lat_lon_error_count"


def get_trunc_hour_time(obstime):
    """Truncate obstime to nearest hour"""
    return((int(obstime)/3600) * 3600)

def get_trunc_minute_time(obstime):
    """Truncate obstime to nearest minute"""
    return((int(obstime)/60) * 60)

def parse_data(data_object, mapping_dict, data_dict, error_dict, logg):
    
    #for key in mapping_dict:
        #print key
        #for index, value in enumerate(data_object.get_field(key)):
        #    print index, value

    # Get the desired generic data from the Droid phone
    latitude = data_object.get_field('latitude')
    longitude = data_object.get_field('longitude')
    obs_time = data_object.get_field('obs_time')
    elevation = data_object.get_field("elevation")

    # Get the desired GPS data
    speed = data_object.get_field("speed")
    heading = data_object.get_field("heading")

    # Get the desired Canbus data
    hoz_accel_lat = data_object.get_field("hoz_accel_lat")
    hoz_accel_long = data_object.get_field("hoz_accel_long")
    air_temp = data_object.get_field("air_temp")
    anti_lock_brake_status  = data_object.get_field("anti_lock_brake_status")
    brake_status = data_object.get_field("brake_status")
    lights = data_object.get_field("lights")
    stab = data_object.get_field("stab")
    trac = data_object.get_field("trac")
    steering_angle = data_object.get_field("steering_angle")
    yaw_rate = data_object.get_field("yaw_rate")

    # Get the desired Surface patrol data
    surface_temp = data_object.get_field("surface_temp")
    dew_temp = data_object.get_field("dew_temp")

    for index, value in enumerate(latitude):

        # Set up data type dictionaries
        if not "GPS" in data_dict:
            data_dict["GPS"] = {}
        if not "Canbus" in data_dict:
            data_dict["Canbus"] = {}
        if not "Surface_patrol" in data_dict:
            data_dict["Surface_patrol"] = {}

        # Check for lat/lon bad values
        lat_lon_failure = False
        if ((value == -9999) or (longitude[index] == -9999)):
            #print "lat/lon == -9999: ", value, longitude[index]
            if GPS_lat_lon_error_count in error_dict:
                error_dict[GPS_lat_lon_error_count] += 1
            else:
                error_dict[GPS_lat_lon_error_count] = 1
            if Canbus_lat_lon_error_count in error_dict:
                error_dict[Canbus_lat_lon_error_count] += 1
            else:
                error_dict[Canbus_lat_lon_error_count] = 1
            if Surface_patrol_lat_lon_error_count in error_dict:
                error_dict[Surface_patrol_lat_lon_error_count] += 1
            else:
                error_dict[Surface_patrol_lat_lon_error_count] = 1
            lat_lon_failure = True
        else:
            if ((value == 0) or (longitude[index] == 0)):
                if GPS_lat_lon_error_count in error_dict:
                    error_dict[GPS_lat_lon_error_count] += 1
                else:
                    error_dict[GPS_lat_lon_error_count] = 1
                if Canbus_lat_lon_error_count in error_dict:
                    error_dict[Canbus_lat_lon_error_count] += 1
                else:
                    error_dict[Canbus_lat_lon_error_count] = 1
                if Surface_patrol_lat_lon_error_count in error_dict:
                    error_dict[Surface_patrol_lat_lon_error_count] += 1
                else:
                    error_dict[Surface_patrol_lat_lon_error_count] = 1
                lat_lon_failure = True
                
        # Check for obs_time bad values
        obs_time_failure = False
        if obs_time[index] == []:
            #print "obs_time_error: ", obs_time[index]
            if GPS_obs_time_error_count in error_dict:
                error_dict[GPS_obs_time_error_count] += 1
            else:
                error_dict[GPS_obs_time_error_count] = 1
            if Canbus_obs_time_error_count in error_dict:
                error_dict[Canbus_obs_time_error_count] += 1
            else:
                error_dict[Canbus_obs_time_error_count] = 1
            if Surface_patrol_obs_time_error_count in error_dict:
                error_dict[Surface_patrol_obs_time_error_count] += 1
            else:
                error_dict[Surface_patrol_obs_time_error_count] = 1
            obs_time_failure = True
            
        if lat_lon_failure or obs_time_failure:
            logg.write_info("Unable to process a line due to lat/lon (%s/%s) or obs_time (%s) bad values" % (value, longitude[index], obs_time[index]))
            continue

        # Set up the GPS data dict
        GPS_dict_struct = {}
        GPS_dict_struct["lat"] = latitude[index]
        GPS_dict_struct["lon"] = longitude[index]
        GPS_dict_struct["obs_time"] = obs_time[index]
        GPS_dict_struct["elevation"] = elevation[index]
        if speed != None:
            GPS_dict_struct["speed"] = speed[index]
        if heading != None:
            GPS_dict_struct["heading"] = heading[index]

        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["GPS"]:
            data_dict["GPS"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["GPS"][trunc_hour_time]:
            data_dict["GPS"][trunc_hour_time][trunc_minute_time] = []
        data_dict["GPS"][trunc_hour_time][trunc_minute_time].append(GPS_dict_struct)

        # Set up the Canbus data dict
        Canbus_dict_struct = {}
        Canbus_dict_struct["lat"] = latitude[index]
        Canbus_dict_struct["lon"] = longitude[index]
        Canbus_dict_struct["obs_time"] = obs_time[index]
        Canbus_dict_struct["elevation"] = elevation[index]
        if hoz_accel_lat != None:
            Canbus_dict_struct["hoz_accel_lat"] = hoz_accel_lat[index]
        if hoz_accel_long != None:
            Canbus_dict_struct["hoz_accel_long"] = hoz_accel_long[index]
        if air_temp != None:
            Canbus_dict_struct["air_temp"] = air_temp[index]
        if anti_lock_brake_status != None:
            Canbus_dict_struct["abs"] = anti_lock_brake_status[index]
        if brake_status != None:
            Canbus_dict_struct["brake_status"] = brake_status[index]
        if lights != None:
            Canbus_dict_struct["lights"] = lights[index]
        if stab != None:
            Canbus_dict_struct["stab"] = stab[index]
        if trac != None:
            Canbus_dict_struct["trac"] = trac[index]
        if steering_angle != None:
            Canbus_dict_struct["steering_angle"] = steering_angle[index]
        if yaw_rate != None:
            Canbus_dict_struct["yaw_rate"] = yaw_rate[index]
 
        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["Canbus"]:
            data_dict["Canbus"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["Canbus"][trunc_hour_time]:
            data_dict["Canbus"][trunc_hour_time][trunc_minute_time] = []
        data_dict["Canbus"][trunc_hour_time][trunc_minute_time].append(Canbus_dict_struct)

        # Set up the Surface patrol dict
        Surface_patrol_dict_struct = {}
        Surface_patrol_dict_struct["lat"] = latitude[index]
        Surface_patrol_dict_struct["lon"] = longitude[index]
        Surface_patrol_dict_struct["obs_time"] = obs_time[index]
        Surface_patrol_dict_struct["elevation"] = elevation[index]
        if surface_temp != None:
            Surface_patrol_dict_struct["surface_temp"] = surface_temp[index]
        if dew_temp != None:
            Surface_patrol_dict_struct["dew_temp"] = dew_temp[index]
           
        trunc_hour_time = get_trunc_hour_time(obs_time[index])
        trunc_minute_time = get_trunc_minute_time(obs_time[index])
        if not trunc_hour_time in data_dict["Surface_patrol"]:
            data_dict["Surface_patrol"][trunc_hour_time] = {}
        if not trunc_minute_time in data_dict["Surface_patrol"][trunc_hour_time]:
            data_dict["Surface_patrol"][trunc_hour_time][trunc_minute_time] = []
        data_dict["Surface_patrol"][trunc_hour_time][trunc_minute_time].append(Surface_patrol_dict_struct)
        

def get_data(dated_in_dir, input_file_list, data_dict, error_dict, date, logg):

    file_list = os.listdir(dated_in_dir)
    csv_file_list = []
    for myfile in file_list:
        if "csv" in myfile:
            csv_file_list.append(myfile)

    for myfile in csv_file_list:
        file_path = os.path.join(dated_in_dir, myfile)
        #print file_path
        umtri = umtri_reader.UmtriImo2(file_path, logg)
        if umtri.file_header_dict == {}:
            logg.write_error("Skipping file %s due to error" % (file_path))
        else:
            parse_data(umtri, umtri.Field_dict, data_dict, error_dict, logg)

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

    input_file_list = []
    data_dict = {}
    error_dict = {}
    stat_dict = {}
    lat_lon_cell_dict = {}

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting("michigan_data_status.py")
    
    get_data(dated_in_dir, input_file_list, data_dict, error_dict, date, logg)
    get_statistics(data_dict, stat_dict, lat_lon_cell_dict)
    write_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, STATE, logg)

    logg.write_ending(0, "michigan_data_status.py")

if __name__ == "__main__":

   main()
