#!/usr/bin/env python

"""Read information from road data files"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import pandas as pd
import os
import sys
import time
import tim
import math
from optparse import OptionParser

import little_r

# case, number_probe, lat (degrees), lon (degrees), radar_x (grid), radar_y (grid), radar_ref (dbZ), wiper_status (none), model_x (grid), model_y (grid), model_dewpoint (celsius), model_air_temp (celsius), model_bar_press (pascals), nss_air_temp_mean (celsius), nss_prevail_vis_mean (pascals), nss_bar_press_mean (km), nss_dew_temp_mean (celsius), nss_wind_speed_mean (m/s), nss_wind_dir_mean (degrees), nss_hourly_precip_mean (in)

MISSING = -9999
LITTLE_R_MISSING = -888888

def speed_dir_to_uv(speed, direction):
    radians = (direction / 360.) * (2 * math.pi)
    u = -1 * speed * math.sin(radians)
    v = -1 * speed * math.cos(radians)
    return (u, v)


class RoadData(object):

    def __init__(self, in_file):

        self.in_file = in_file
        self.table = pd.read_csv(self.in_file, engine='c') 
        print self.table.keys()
        self.table.rename(columns={' lat (degrees)': 'lat',  ' lon (degrees)': 'lon', ' radar_x (grid)' : 'radar_x', ' radar_y (grid)' : 'radar_y', ' radar_ref (dbZ)' : 'radar_ref', ' wiper_status (none)' : 'wiper_status',
                              ' model_x (grid)' : 'model_x', ' model_y (grid)' : 'model_y', ' model_air_temp (celsius)' : 'model_air_temp', ' model_bar_press (hPa)' : 'model_bar_press', ' model_dewpoint (celsius)' : 'model_dewpoint', ' model_geopotential_height (m)' : 'model_geopotential_height', ' model_wind_dir (degrees)' : 'model_wind_dir', ' model_wind_speed (m/s)' : 'model_wind_speed', ' nss_air_temp_mean (celsius)' : 'nss_air_temp_mean', ' nss_bar_press_mean (hPa)' : 'nss_bar_press_mean', ' nss_dew_temp_mean (celsius)' : 'nss_dew_temp_mean', ' nss_hourly_precip_mean (in)' : 'nss_hourly_precip_mean', ' nss_prevail_vis_mean (pascals)' : 'nss_prevail_vis_mean', ' nss_wind_dir_mean (degrees)' : 'nss_wind_dir_mean', ' nss_wind_speed_mean (m/s)' : 'nss_wind_speed_mean'}, inplace=True)
        print "table: ", self.table

def celsius_to_kelvin(missing, celsius):

    if celsius != missing:
        return celsius + 273.15
    else:
        return celsius

def hPa_to_Pa(missing, hPa):

    if hPa == missing:
        return missing
    else:
        return 100 * hPa
    
def create_little_r(omit_wind_speed_dir, road_data_in_file, little_r_out_file):
    """
    Read in road_data_in_file and output little_r_out_file
    """

    print "before road_data: reading %s" % road_data_in_file
    road_data = RoadData(road_data_in_file)

    # Sample road data file name road_data.20140331.0135.csv
    yyyymmddhhmm = road_data_in_file[-17:-9] + road_data_in_file[-8:-4] 

    identifier = "road segment"
    name = "simulation"
    platform = 'vehicle'
    source = 'data simulator'
    num_valid_field = 5 # wait on this for Jared
    num_error = 0
    num_warning = 0
    num_duplicates = 0
    is_sounding = "F"
    bogus = "F"
    discard = "F"
    utc_seconds = tim.datetogmt(yyyymmddhhmm)
    tms = time.gmtime(utc_seconds)
    julian = tms.tm_yday
    date = time.strftime("%Y%m%d%H%M%S", tms)

    sea_level_pressure = LITTLE_R_MISSING
    sea_level_pressure_qc = LITTLE_R_MISSING
    ref_pressure = LITTLE_R_MISSING
    ref_pressure_qc = LITTLE_R_MISSING
    ground_temp = LITTLE_R_MISSING
    ground_temp_qc = LITTLE_R_MISSING
    sea_surface_temp = LITTLE_R_MISSING
    sea_surface_temp_qc = LITTLE_R_MISSING
    surface_pressure = LITTLE_R_MISSING
    surface_pressure_qc = LITTLE_R_MISSING
    precip = LITTLE_R_MISSING
    precip_qc = LITTLE_R_MISSING
    max_temp = LITTLE_R_MISSING
    max_temp_qc = LITTLE_R_MISSING
    min_temp = LITTLE_R_MISSING
    min_temp_qc = LITTLE_R_MISSING
    night_min_temp = LITTLE_R_MISSING
    night_min_temp_qc = LITTLE_R_MISSING
    pressure_change_3h = LITTLE_R_MISSING
    pressure_change_3h_qc = LITTLE_R_MISSING
    pressure_change_24h = LITTLE_R_MISSING
    pressure_change_24h_qc = LITTLE_R_MISSING
    cloud_cover = LITTLE_R_MISSING
    cloud_cover_qc = LITTLE_R_MISSING
    ceiling = LITTLE_R_MISSING
    ceiling_qc = LITTLE_R_MISSING
    
    sequence_num = 1
    fp_out = open(little_r_out_file, "w")

    """
    for x in [latitude, longitude, identifier, name, platform, source, elevation, num_valid_field, num_error, num_warning, sequence_num, num_duplicates, bogus, discard, utc_seconds, julian, date, sea_level_pressure, sea_level_pressure_qc, ref_pressure, ref_pressure_qc, ground_temp, ground_temp_qc, sea_surface_temp, sea_surface_temp_qc, surface_pressure, surface_pressure_qc, precip, precip_qc, max_temp, max_temp_qc, min_temp, min_temp_qc, night_min_temp, night_min_temp_qc, pressure_change_3h, pressure_change_3h_qc, pressure_change_24h, pressure_change_24h_qc, cloud_cover, cloud_cover_qc, ceiling, ceiling_qc]:
        print(type(x))
    """
    
    for index in range(len(road_data.table["case"])):
        latitude = float(road_data.table['lat'][index])
        longitude = float(road_data.table['lon'][index])
        elevation = float(road_data.table['model_geopotential_height'][index])         # getting height from RTMA surface geopotential height field

        little_r.write_little_r_header(latitude, longitude, identifier, name, platform, source, elevation, num_valid_field, num_error, num_warning, sequence_num, num_duplicates, is_sounding, bogus, discard, utc_seconds, julian, date, sea_level_pressure, sea_level_pressure_qc, ref_pressure, ref_pressure_qc, ground_temp, ground_temp_qc, sea_surface_temp, sea_surface_temp_qc, surface_pressure, surface_pressure_qc, precip, precip_qc, max_temp, max_temp_qc, min_temp, min_temp_qc, night_min_temp, night_min_temp_qc, pressure_change_3h, pressure_change_3h_qc, pressure_change_24h, pressure_change_24h_qc, cloud_cover, cloud_cover_qc, ceiling, ceiling_qc, fp_out)
        pressure = road_data.table["nss_bar_press_mean"][index]
        if pressure == MISSING:
            pressure = road_data.table["model_bar_press"][index]
        pressure = hPa_to_Pa(MISSING, pressure)
        pressure_qc = 0
        height = road_data.table["model_geopotential_height"][index]
        height_qc = 0
        if height == MISSING:
            height = LITTLE_R_MISSING
            height_qc = LITTLE_R_MISSING
        air_temperature = road_data.table["nss_air_temp_mean"][index]
        if air_temperature == MISSING:
            air_temperature = road_data.table["model_air_temp"][index]
        air_temperature = celsius_to_kelvin(MISSING, air_temperature)
        air_temperature_qc = 0

        dew_point = road_data.table["nss_dew_temp_mean"][index]
        if dew_point == MISSING:
            dew_point = road_data.table["model_dewpoint"][index]
        dew_point = celsius_to_kelvin(MISSING, dew_point)
        dew_point_qc = 0

        if omit_wind_speed_dir:
            wind_speed = MISSING
            wind_dir = MISSING
            wind_speed_qc = 0
            wind_dir_qc = 0
            u = MISSING
            v = MISSING
            u_qc = 0
            v_qc = 0
        else:
            wind_speed_qc = 0
            wind_speed = road_data.table["nss_wind_speed_mean"][index]
            if wind_speed == MISSING:
                wind_speed = road_data.table["model_wind_speed"][index]
                if wind_speed == MISSING:
                    wind_speed = LITTLE_R_MISSING
                    wind_speed_qc = LITTLE_R_MISSING
                    
            wind_dir = road_data.table["nss_wind_dir_mean"][index]
            wind_dir_qc = 0
            if wind_dir == MISSING:
                wind_dir = road_data.table["model_wind_dir"][index]
                if wind_dir == MISSING:
                    wind_dir = LITTLE_R_MISSING
                    wind_dir_qc = LITTLE_R_MISSING

            if wind_dir != LITTLE_R_MISSING and wind_speed != LITTLE_R_MISSING:
                (u, v) = speed_dir_to_uv(wind_speed, wind_dir)
                u_qc = 0
                v_qc = 0
            else:
                u = LITTLE_R_MISSING
                v = LITTLE_R_MISSING
                u_qc = LITTLE_R_MISSING
                v_qc = LITTLE_R_MISSING
        
            
        rh = LITTLE_R_MISSING
        rh_qc = LITTLE_R_MISSING

        thickness = LITTLE_R_MISSING
        thickness_qc = LITTLE_R_MISSING
        
        little_r.write_little_r_obs(pressure, pressure_qc, height, height_qc, air_temperature, air_temperature_qc, dew_point, dew_point_qc, wind_speed, wind_speed_qc, wind_dir, wind_dir_qc, u, u_qc, v, v_qc, rh, rh_qc, thickness, thickness_qc, fp_out)

        # write end data record
        pressure = -777777
        height = -777777

        # write end report record
        little_r.write_little_r_obs(pressure, pressure_qc, height, height_qc, air_temperature, air_temperature_qc, dew_point, dew_point_qc, wind_speed, wind_speed_qc, wind_dir, wind_dir_qc, u, u_qc, v, v_qc, rh, rh_qc, thickness, thickness_qc, fp_out)

        fp_out.write("-777777-777777-777777\n")
        sequence_num += 1

    fp_out.close()

def main():

    usage_str = "%prog in_file[s]"
    parser = OptionParser(usage = usage_str)
    
    parser.add_option("-o", "--omit_wind_speed_dir", dest="omit_wind_speed_dir", action="store_true", help="omit wind speed and direction from output")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)


    in_files = args

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    if options.omit_wind_speed_dir:
        logg.write_info("using no wind option")
    else:
        logg.write_info("with winds")

    for in_file in in_files:
        if in_file[-4:] == ".csv" or in_file[-4:] == ".txt":
            out_file = in_file[:-4] + ".lr"
        else:
            out_file = in_file + ".lr"
            
        create_little_r(options.omit_wind_speed_dir, in_file, out_file)
    logg.write_ending()

if __name__ == "__main__":

   main()
