#!/usr/bin/env python

"""Read umtri csv file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: umtri_reader.py,v $                                           #
#       Version: $Revision: 1.13 $  Dated: $Date: 2014/06/18 20:12:39 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import numpy as np
import numpy.ma as ma
import tim
import time
import datetime
from pytz import timezone

VEHICLE_ID_LEN = 32

def no_op(value):
    return value

def fahrenheit_to_celsius(fahrenheit_value):
    
    celsius_value = (fahrenheit_value - 32) * 5.0/9.0

    return celsius_value

def kpatohpa(kpa_value):
    
    hpa_value = 10 * kpa_value

    return hpa_value

def feet_to_meters(feet_value):

    meters_value = feet_value * .3048

    return meters_value

def miles_per_hour_to_meters_per_second(miles_per_hour_value):

    meters_per_second_value = miles_per_hour_value * .44704

    return meters_per_second_value

class UmtriImo2(object):

    Token_list = ["Size:", "File Name:", "VIN:", "Date:", "Air Temp:", "Barometer:", "Coolant Temp:", "Odometer:"]
    Vid_token_pos = 2
    Air_temp_token_pos = 4
    Pressure_token_pos = 5
    In_missing = 10001
    In_missing_string = '10001'
    Out_missing = -9999
    Out_missing_string = '-9999'
   
    def get_token_value(self, token, line):

        spl = line[:-1].split(",")
        if spl[0] == token:
            return spl[3].rstrip()
        else:
            return ""
        
    def __init__(self, file_name, logg):

        self.file_header_dict = {}

        # 1FDWX305X8ED93082_04222014_152018_v3.7.1.csv
        # split the file name on the underscore and use the first item for vid
        self.file_name = file_name

        lines = file(file_name).readlines()

        if len(lines) < len(UmtriImo2.Token_list):
            logg.write_error("file %s: header has %d lines -- too short" % (file_name, len(lines)))
            self.error = "file %s: header has %d lines -- too short" % (file_name, len(lines))
            return

        # Get header
        for ind, line in enumerate(lines[0:len(UmtriImo2.Token_list)]):
            token_value = UmtriImo2.get_token_value(self, UmtriImo2.Token_list[ind], line)            
            self.file_header_dict[UmtriImo2.Token_list[ind]] = token_value
            
        #print self.file_header_dict
        
        # Find column headers line
        column_header_index = len(UmtriImo2.Token_list)
        for ind in range(len(UmtriImo2.Token_list), len(lines)):
            spl = lines[ind].split(",")
            if len(spl[0].strip()) > 0:
                column_header_index = ind
                break

        self.column_headers = lines[column_header_index].split(",")
        self.data_dict = {}

        for column in self.column_headers:
            self.data_dict[column] = []
                    
        # Collect data for each column header and store in lists
        for line_ind in range(column_header_index+1, len(lines)):
            spl = lines[line_ind].split(",")

            if len(spl) > len(self.column_headers):
                self.error = "The number of columns of data %d exceeds the number of columns in the header %d." % (len(spl), len(self.column_headers))
                #logg.write_error("The number of columns of data %d exceeds the number of columns in the header %d." % (len(spl), len(self.column_headers)))
                logg.write_error("The number of columns of data %d exceeds the number of columns in the header %d. Trimming line." % (len(spl), len(self.column_headers)))
                spl = spl[0:len(self.column_headers)]
            
            for ind in range(len(spl)):
                self.data_dict[self.column_headers[ind]].append(spl[ind])

    def get_field(self, field):
        """Return appropriate values for given field"""
        if field in UmtriImo2.Field_dict.keys():
            return self.Field_dict[field](self)            
            
    def get_heading(self):
        if not hasattr(self, "heading"):
            # do string conversion and missing value detection on first call
            self.heading = None
            if "HEAD_G" in self.data_dict:
                self.heading = np.array(self.data_dict["HEAD_G"], dtype=np.float32)
                self.heading = ma.masked_values(self.heading, UmtriImo2.In_missing)
                self.heading = self.heading.filled(UmtriImo2.Out_missing)
        return self.heading
    
    def get_abs(self):
        """Return automatic brake system status"""
        if not hasattr(self, "abs"):
            self.abs = None
            if "ABS" in self.data_dict:
                self.abs = np.array(self.data_dict["ABS"], dtype=np.short)
                self.abs = ma.masked_values(self.abs, UmtriImo2.In_missing)
                self.abs = self.abs + 1
                self.abs = self.abs.filled(UmtriImo2.Out_missing)
        return self.abs

    def get_air_temperature(self):
        """Return air temperature in Celsius converting from Fahrenheit"""

        air_temperature_string = self.file_header_dict[UmtriImo2.Token_list[UmtriImo2.Air_temp_token_pos]]
        if air_temperature_string != UmtriImo2.In_missing_string:
            temperature_value = round(float(air_temperature_string))
            final_list = [temperature_value] * len(self.get_latitude())
            self.air_temperature = np.array(final_list, dtype=np.int16)
            self.air_temperature = fahrenheit_to_celsius(self.air_temperature)
        else:
            final_list = [UmtriImo2.Out_missing] * len(self.get_latitude())
            self.air_temperature = np.array(final_list, dtype=np.int16)

        return self.air_temperature

    def get_pressure(self):
        """Return pressure in hPa"""

        pressure_string = self.file_header_dict[UmtriImo2.Token_list[UmtriImo2.Pressure_token_pos]]
        if pressure_string != UmtriImo2.In_missing_string:
            pressure_value = round(float(pressure_string))
            final_list = [pressure_value] * len(self.get_latitude())
            self.pressure = np.array(final_list, dtype=np.int16)
            self.pressure = kpatohpa(self.pressure)
        else:
            final_list = [UmtriImo2.Out_missing] * len(self.get_latitude())
            self.pressure = np.array(final_list, dtype=np.int16)

        return self.pressure

    def get_air_temperature2(self):
        """Return air temperature in Celsius converting from Fahrenheit"""
        if not hasattr(self, "air_temperature2"):
            self.air_temperature2 = None
            if "TMP-SP" in self.data_dict:
                self.air_temperature2 = np.array(self.data_dict["TMP-SP"], dtype=np.float32)
                self.air_temperature2 = ma.masked_values(self.air_temperature2, UmtriImo2.In_missing)
                #self.air_temperature2 = su.convert(self.air_temperature2, su.temperature.fahrenheit, su.temperature.celsius)
                self.air_temperature2 = fahrenheit_to_celsius(self.air_temperature2)
                self.air_temperature2 = self.air_temperature2.filled(UmtriImo2.Out_missing)
        return self.air_temperature2

    def get_brake(self):
        """Return brake status"""
        if not hasattr(self, "brake"):
            self.brake = None
            if "BR" in self.data_dict:
                self.brake = np.array(self.data_dict["BR"], dtype=np.short)
                self.brake = ma.masked_values(self.brake, UmtriImo2.In_missing)
                self.brake = self.brake + 1
                self.brake = self.brake.filled(UmtriImo2.Out_missing)
            elif "BRK" in self.data_dict:
                self.brake = np.array(self.data_dict["BRK"], dtype=np.short)
                self.brake = ma.masked_values(self.brake, UmtriImo2.In_missing)
                self.brake = self.brake + 1
                self.brake = self.brake.filled(UmtriImo2.Out_missing)
        return self.brake

    def get_latitude(self):
        if not hasattr(self, "latitude"):
            self.latitude = None
            if "LAT" in self.data_dict:
                self.latitude = np.array(self.data_dict["LAT"], dtype=np.float64)
                self.latitude = ma.masked_values(self.latitude, UmtriImo2.In_missing)
                self.latitude = self.latitude.filled(UmtriImo2.Out_missing)
        return self.latitude

    def get_longitude(self):
        if not hasattr(self, "longitude"):
            self.longitude = None
            if "LONG" in self.data_dict:
                self.longitude = np.array(self.data_dict["LONG"], dtype=np.float64)
                self.longitude = ma.masked_values(self.longitude, UmtriImo2.In_missing)
                self.longitude = self.longitude.filled(UmtriImo2.Out_missing)
        return self.longitude

    def get_elevation(self):
        """Return elevation in meters converting from feet"""
        if not hasattr(self, "elevation"):
            self.elevation = None
            if "ALT" in self.data_dict:
                self.elevation = np.array(self.data_dict["ALT"], dtype=np.float32)
                self.elevation = ma.masked_values(self.elevation, UmtriImo2.In_missing)
                #self.elevation = su.convert(self.elevation, su.length.feet, su.length.meters)
                self.elevation = feet_to_meters(self.elevation)
                self.elevation = self.elevation.filled(UmtriImo2.Out_missing)
        return self.elevation

    def get_dewpoint(self):
        """Return dewpoint in Celsius converting from Fahrenheit"""
        if not hasattr(self, "dewpoint"):
            self.dewpoint = None
            if "DPNT" in self.data_dict:
                self.dewpoint = np.array(self.data_dict["DPNT"], dtype=np.float32)
                self.dewpoint = ma.masked_values(self.dewpoint, UmtriImo2.In_missing)
                #self.dewpoint = su.convert(self.dewpoint, su.temperature.fahrenheit, su.temperature.celsius)
                self.dewpoint = fahrenheit_to_celsius(self.dewpoint)
                self.dewpoint = self.dewpoint.filled(UmtriImo2.Out_missing)
        return self.dewpoint

    def get_humidity(self):
        """Return humidity"""
        if not hasattr(self, "humidity"):
            self.humidity = None
            if "HUMD" in self.data_dict:
                self.humidity = np.array(self.data_dict["HUMD"], dtype=np.float32)
                self.humidity = ma.masked_values(self.humidity, UmtriImo2.In_missing)
                self.humidity = self.humidity.filled(UmtriImo2.Out_missing)
        return self.humidity

    def get_lights(self):
        """Return lights"""
        if not hasattr(self, "lights"):
            self.lights = None
            if "HDL" in self.data_dict:
                self.lights = np.array(self.data_dict["HDL"], dtype=np.short)
                self.lights = ma.masked_values(self.lights, UmtriImo2.In_missing)
                self.lights = self.lights.filled(UmtriImo2.Out_missing)
        return self.lights
        
    def get_speed_meters_per_second(self):
        """Return speed in meters per second converting from miles per hour"""
        if not hasattr(self, "speed_mps"):
            self.speed_mps = None
            if "SPD-C" in self.data_dict:
                self.speed_mps = np.array(self.data_dict["SPD-C"], dtype=np.float32)
                self.speed_mps = ma.masked_values(self.speed_mps, UmtriImo2.In_missing)
                #self.speed_mps = su.convert(self.speed_mps, su.speed.miles_per_hour, su.speed.meters_per_second)
                self.speed_mps = miles_per_hour_to_meters_per_second(self.speed_mps)
                self.speed_mps = self.speed_mps.filled(UmtriImo2.Out_missing)
        return self.speed_mps

    def get_accel_lat(self):
        """Return latitudinal acceleration"""
        if not hasattr(self, "accel_lat"):
            self.accel_lat = None
            if "LAT-A" in self.data_dict:
                self.accel_lat = np.array(self.data_dict["LAT-A"], dtype=np.float32)
                self.accel_lat = ma.masked_values(self.accel_lat, UmtriImo2.In_missing)
                #self.accel_lat = su.convert(self.accel_lat, su.speed.miles_per_hour, su.speed.meters_per_second)
                #self.accel_lat = miles_per_hour_to_meters_per_second(self.accel_lat)
                self.accel_lat = self.accel_lat.filled(UmtriImo2.Out_missing)
        return self.accel_lat

    def get_accel_long(self):
        """Return longitudinal acceleration"""
        if not hasattr(self, "accel_long"):
            self.accel_long = None
            if "LONG-A" in self.data_dict:
                self.accel_long = np.array(self.data_dict["LONG-A"], dtype=np.float32)
                self.accel_long = ma.masked_values(self.accel_long, UmtriImo2.In_missing)
                #self.accel_long = su.convert(self.accel_long, su.speed.miles_per_hour, su.speed.meters_per_second)
                #self.accel_long = miles_per_hour_to_meters_per_second(self.accel_long)
                self.accel_long = self.accel_long.filled(UmtriImo2.Out_missing)
        return self.accel_long

    def get_stab(self):
        """Return stability"""
        if not hasattr(self, "stab"):
            self.stab = None
            if "ESP" in self.data_dict:
                self.stab = np.array(self.data_dict["ESP"], dtype=np.short)
                self.stab = ma.masked_values(self.stab, UmtriImo2.In_missing)
                self.stab = self.stab + 1
                self.stab = self.stab.filled(UmtriImo2.Out_missing)
            elif "ESC" in self.data_dict:
                self.brake = np.array(self.data_dict["ESC"], dtype=np.short)
                self.brake = ma.masked_values(self.brake, UmtriImo2.In_missing)
                self.brake = self.brake + 1
                self.brake = self.brake.filled(UmtriImo2.Out_missing)
        return self.stab

    def get_surface_temperature(self):
        """Return surface temperature in Celsius converting from Fahrenheit"""
        if not hasattr(self, "surface_temperature"):
            self.surface_temperature = None
            if "STMP" in self.data_dict:
                self.surface_temperature = np.array(self.data_dict["STMP"], dtype=np.float32)
                self.surface_temperature = ma.masked_values(self.surface_temperature, UmtriImo2.In_missing)
                #self.surface_temperature = su.convert(self.surface_temperature, su.temperature.fahrenheit, su.temperature.celsius)
                self.surface_temperature = fahrenheit_to_celsius(self.surface_temperature)
                self.surface_temperature = self.surface_temperature.filled(UmtriImo2.Out_missing)
        return self.surface_temperature

    def get_time(self):
        """Convert time to Unix seconds"""
        if not hasattr(self, "unix_time"):
            self.unix_time = None
            date = ""
            if "TIME" in self.data_dict:
                # date is in format mm/dd/yyyy
                mdy_format = self.file_header_dict["Date:"]
                date = "%s%s%s" % (mdy_format[6:10], mdy_format[0:2], mdy_format[3:5])
            else:
                return self.unix_time

            self.unix_time = []
            # time is in format hh:mm:ss
            for time_string in self.data_dict["TIME"]:
                date_time = "%s%s%s%s" % (date, time_string[0:2], time_string[3:5], time_string[6:8])
                #print date_time
                #self.unix_time.append(tim.datetogmt(date_time))
                #print tim.datetogmt(date_time)

                # Convert to UTC time since times are local
                obs_time = datetime.datetime.strptime(date_time, "%Y%m%d%H%M%S")
                eastern = timezone("US/Eastern")
                utc = timezone("UTC")
                local_date_time = eastern.localize(obs_time)
                utc_date_time = local_date_time.astimezone(utc)
                obs_date_time = time.mktime(utc_date_time.timetuple())
                self.unix_time.append(obs_date_time)

        return self.unix_time

    def get_vehicle_id(self):
        """Return vehicle id"""
        if not hasattr(self, "vehicle_id"):
            tmp_list = []
            vin_list = []
            final_list = []
            self.vehicle_id = None
            if not self.file_header_dict["VIN:"] == "10001":
                vin_list.append(self.file_header_dict["VIN:"])
                for index, value in enumerate(vin_list):
                    tmp_list.append(list(value.ljust(32,'\0')))
                final_list = [tmp_list[0]] * len(self.get_latitude())
                self.vehicle_id = np.array(final_list, dtype=object)
                
        return self.vehicle_id
                

    def get_trac(self):
        """Return traction control"""
        if not hasattr(self, "trac"):
            self.trac = None
            if "TCSB" in self.data_dict:
                self.trac = np.array(self.data_dict["TCSB"], dtype=np.short)
                self.trac = ma.masked_values(self.trac, UmtriImo2.In_missing)
                self.trac = self.trac + 1
                self.trac = self.trac.filled(UmtriImo2.Out_missing)
            elif "TCS" in self.data_dict:
                self.trac = np.array(self.data_dict["TCS"], dtype=np.short)
                self.trac = ma.masked_values(self.trac, UmtriImo2.In_missing)
                self.trac = self.trac + 1
                self.trac = self.trac.filled(UmtriImo2.Out_missing)
        return self.trac

    def get_wiper_status(self):
        """Return wiper status"""
        if not hasattr(self, "wiper_status"):
            self.wiper_status = None
            if "WIP" in self.data_dict:
                self.wiper_status = np.array(self.data_dict["WIP"], dtype=np.short)
                self.wiper_status = ma.masked_values(self.wiper_status, UmtriImo2.In_missing)
                self.wiper_status = self.wiper_status + 1
                self.wiper_status = self.wiper_status.filled(UmtriImo2.Out_missing)
        return self.wiper_status

    def get_steering_angle(self):
        """Return steering angle"""
        if not hasattr(self, "steering_angle"):
            self.steering_angle = None
            if "SWA" in self.data_dict:
                self.steering_angle = np.array(self.data_dict["SWA"], dtype=np.float32)
                self.steering_angle = ma.masked_values(self.steering_angle, UmtriImo2.In_missing)
                self.steering_angle = self.steering_angle.filled(UmtriImo2.Out_missing)
        return self.steering_angle

    def get_yaw_rate(self):
        """Return yaw rate"""
        if not hasattr(self, "yaw_rate"):
            self.yaw_rate = None
            if "YAW" in self.data_dict:
                self.yaw_rate = np.array(self.data_dict["YAW"], dtype=np.float32)
                self.yaw_rate = ma.masked_values(self.yaw_rate, UmtriImo2.In_missing)
                self.yaw_rate = self.yaw_rate.filled(UmtriImo2.Out_missing)
        return self.yaw_rate

    def get_rec_time(self):
        """Return received time"""
        # Get length of latitude to determine how long rec_time should be
        latitude = None
        if not hasattr(self, "latitude"):
            if "LAT" in self.data_dict:
                latitude = np.array(self.data_dict["LAT"], dtype=np.float64)
                latitude = ma.masked_values(latitude, UmtriImo2.In_missing)
                latitude = latitude.filled(UmtriImo2.Out_missing)
        else:
            latitude = self.latitude
        current_time_value = time.time()
        # Truncate to the nearest minute and then subtract 1 second
        # Need rec_times to be within the five minute period for runs in
        # the VDT for persistence and step tests
        adjusted_time_value = ((int(current_time_value)/60) *60) - 1
        try:
            rec_time = [adjusted_time_value] * len(latitude)
            return rec_time
        except:
            print "Latitude has no length. Exiting."
            sys.exit(2)
            

    def __repr__(self):

        ret_string = "<UmtriImo2 "
        for token in sorted(self.file_header_dict.keys()):
            ret_string += " %s %s," % (token, self.file_header_dict[token])
        ret_string += ">"
            
        return ret_string

    # Maps the netcdf output variable name to the function to be called
    # to get the appropriate value
    Field_dict = {'rec_time': get_rec_time, 'obs_time': get_time, 'latitude': get_latitude, 'longitude': get_longitude, 'elevation': get_elevation,
                  'speed': get_speed_meters_per_second, 'hoz_accel_lat': get_accel_lat, 'hoz_accel_long': get_accel_long, 'humidity':get_humidity,
                  'surface_temp': get_surface_temperature, 'air_temp2': get_air_temperature2, 'dew_temp': get_dewpoint, 'heading': get_heading,
                  'abs': get_abs, 'brake_status': get_brake, 'lights': get_lights, 'stab': get_stab, 'wiper_status': get_wiper_status, 'trac': get_trac,
                  'steering_angle': get_steering_angle, 'yaw_rate': get_yaw_rate, 'vehicle_id': get_vehicle_id, 'air_temp' : get_air_temperature, 'bar_pressure' : get_pressure}

def main():

    usage_str = "%prog in_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    in_file = args[0]
    umtri = UmtriImo2(in_file, logg)
    print umtri
    #print "airtemp: ", umtri.get_air_temperature2()
    #print "surfacetemp: ", umtri.get_surface_temperature()
    #print "latitude: ", umtri.get_latitude()
    #print "longitude: ", umtri.get_longitude()
    #print "speed: ", umtri.get_speed_meters_per_second()
    #print "elevation: ", umtri.get_elevation()
    #print "abs: ", umtri.get_abs()
    #print "stab: ", umtri.get_stab()
    #print "heading: ", umtri.get_heading()
    #print "yaw: ", umtri.get_yaw_rate()
    #print "trac: ", umtri.get_trac()
    #print "steering_angle: ", umtri.get_steering_angle()
    #print "dewpoint: ", umtri.get_dewpoint()
    #print "brake: ", umtri.get_brake()
    #print "lights: ", umtri.get_lights()
    #print "accel_lat: ", umtri.get_accel_lat()
    #print "accel_long: ", umtri.get_accel_long()
    print "vehicle_id: ", umtri.get_vehicle_id()
    print "pressure", umtri.get_pressure()
    print "air temperature", umtri.get_air_temperature()

if __name__ == "__main__":

   main()
