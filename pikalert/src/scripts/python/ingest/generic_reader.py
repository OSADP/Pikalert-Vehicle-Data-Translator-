#!/usr/bin/env python

"""Read umtri csv file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: generic_reader.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/07/09 22:57:47 $           #
#                                                                                #
# ============================================================================== #

import calendar
import log_msg
import os
import sys
from optparse import OptionParser
import numpy as np
import numpy.ma as ma
import tim
import time
import datetime
import pytz

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

class GenericProbeReader(object):

    In_missing = -9999
    In_missing_string = '-9999'
    Out_missing = -9999
    Out_missing_string = '-9999'
    Field_names = ["abs", "accel_lat", "accel_long", "air_temperature", "air_temperature2", "brake", "dewpoint", "elevation", "heading", "humidity", "lights", "latitude", "longitude", "pressure", "received_time", "speed_mps", "stab", "steering_angle", "surface_temperature", "time", "trac", "vehicle_id", "wiper_status", "yaw_rate"]

   
    
    def get_token_value(self, token, line):

        spl = line[:-1].split(",")
        if spl[0] == token:
            return spl[3].rstrip()
        else:
            return ""
        
    def __init__(self, file_name, logg):

        self.file_header_dict = {}
        self.file_name = file_name

        lines = file(file_name).readlines()

        # Split header line
        column_header_index = 0
        self.column_headers = lines[column_header_index].strip().split(',')
        self.data_dict = {}

        for column in self.column_headers:
            self.data_dict[column] = []
                    
        # Collect data for each column header and store in lists
        for line_ind in range(column_header_index+1, len(lines)):
            spl = lines[line_ind].strip().split(",")

            if len(spl) > len(self.column_headers):
                self.error = "The number of columns of data %d exceeds the number of columns in the header %d." % (len(spl), len(self.column_headers))
                logg.write_error("The number of columns of data %d exceeds the number of columns in the header %d. Trimming line." % (len(spl), len(self.column_headers)))
                spl = spl[0:len(self.column_headers)]
            
            for ind in range(len(spl)):
                self.data_dict[self.column_headers[ind]].append(spl[ind])

    def get_field(self, field):
        """Return appropriate values for given field"""
        if field in GenericProbeReader.Field_dict.keys():
            return self.Field_dict[field](self)
            
    def get_heading(self):
        if not hasattr(self, "heading"):
            # do string conversion and missing value detection on first call
            self.heading = None
            if "heading" in self.data_dict:
                self.heading = np.array(self.data_dict["heading"], dtype=np.float32)
                self.heading = ma.masked_values(self.heading, GenericProbeReader.In_missing)
                self.heading = self.heading.filled(GenericProbeReader.Out_missing)
        return self.heading
    
    def get_abs(self):
        """Return automatic brake system status"""
        if not hasattr(self, "abs"):
            self.abs = None
            if "abs" in self.data_dict:
                self.abs = np.array(self.data_dict["abs"], dtype=np.short)
                self.abs = ma.masked_values(self.abs, GenericProbeReader.In_missing)
                self.abs = self.abs + 1
                self.abs = self.abs.filled(GenericProbeReader.Out_missing)
        return self.abs

    def get_air_temperature(self):
        """Return air temperature in Celsius converting from Fahrenheit"""
        if not hasattr(self, "air_temperature"):
            self.air_temperature = None
            if "air_temperature" in self.data_dict:
                self.air_temperature = np.array(self.data_dict["air_temperature"], dtype=np.float32)
                self.air_temperature = ma.masked_values(self.air_temperature, GenericProbeReader.In_missing)
                #self.air_temperature = su.convert(self.air_temperature, su.temperature.fahrenheit, su.temperature.celsius)
                self.air_temperature = fahrenheit_to_celsius(self.air_temperature)
                self.air_temperature = self.air_temperature.filled(GenericProbeReader.Out_missing)
        return self.air_temperature

    def get_pressure(self):
        """Return pressure in hPa"""

        if not hasattr(self, "pressure"):
            self.pressure = None
            if "pressure" in self.data_dict:
                self.pressure = np.array(self.data_dict["pressure"], dtype=np.float32)
                self.pressure = ma.masked_values(self.pressure, GenericProbeReader.In_missing)
                self.pressure = kpatohpa(self.pressure)
                self.pressure = self.pressure.filled(GenericProbeReader.Out_missing)
        return self.pressure


    def get_air_temperature2(self):
        """Return air temperature in Celsius converting from Fahrenheit"""
        if not hasattr(self, "air_temperature2"):
            self.air_temperature2 = None
            if "air_temperature2" in self.data_dict:
                self.air_temperature2 = np.array(self.data_dict["air_temperature2"], dtype=np.float32)
                self.air_temperature2 = ma.masked_values(self.air_temperature2, GenericProbeReader.In_missing)
                self.air_temperature2 = fahrenheit_to_celsius(self.air_temperature2)
                self.air_temperature2 = self.air_temperature2.filled(GenericProbeReader.Out_missing)
        return self.air_temperature2

    def get_brake(self):
        """Return brake status"""
        if not hasattr(self, "brake"):
            self.brake = None
            if "brake" in self.data_dict:
                self.brake = np.array(self.data_dict["brake"], dtype=np.short)
                self.brake = ma.masked_values(self.brake, GenericProbeReader.In_missing)
                self.brake = self.brake + 1
                self.brake = self.brake.filled(GenericProbeReader.Out_missing)
        return self.brake

    def get_latitude(self):
        if not hasattr(self, "latitude"):
            self.latitude = None
            if "latitude" in self.data_dict:
                self.latitude = np.array(self.data_dict["latitude"], dtype=np.float64)
                self.latitude = ma.masked_values(self.latitude, GenericProbeReader.In_missing)
                self.latitude = self.latitude.filled(GenericProbeReader.Out_missing)
        return self.latitude

    def get_longitude(self):
        if not hasattr(self, "longitude"):
            self.longitude = None
            if "longitude" in self.data_dict:
                self.longitude = np.array(self.data_dict["longitude"], dtype=np.float64)
                self.longitude = ma.masked_values(self.longitude, GenericProbeReader.In_missing)
                self.longitude = self.longitude.filled(GenericProbeReader.Out_missing)
        return self.longitude


    def get_elevation(self):
        """Return elevation in meters converting from feet"""
        if not hasattr(self, "elevation"):
            self.elevation = None
            if "elevation" in self.data_dict:
                self.elevation = np.array(self.data_dict["elevation"], dtype=np.float32)
                self.elevation = ma.masked_values(self.elevation, GenericProbeReader.In_missing)
                #self.elevation = su.convert(self.elevation, su.length.feet, su.length.meters)
                self.elevation = feet_to_meters(self.elevation)
                self.elevation = self.elevation.filled(GenericProbeReader.Out_missing)
        return self.elevation

    def get_dewpoint(self):
        """Return dewpoint in Celsius converting from Fahrenheit"""
        if not hasattr(self, "dewpoint"):
            self.dewpoint = None
            if "dewpoint" in self.data_dict:
                self.dewpoint = np.array(self.data_dict["dewpoint"], dtype=np.float32)
                self.dewpoint = ma.masked_values(self.dewpoint, GenericProbeReader.In_missing)
                self.dewpoint = fahrenheit_to_celsius(self.dewpoint)
                self.dewpoint = self.dewpoint.filled(GenericProbeReader.Out_missing)
        return self.dewpoint

    def get_humidity(self):
        """Return humidity"""
        if not hasattr(self, "humidity"):
            self.humidity = None
            if "humidity" in self.data_dict:
                self.humidity = np.array(self.data_dict["humidity"], dtype=np.float32)
                self.humidity = ma.masked_values(self.humidity, GenericProbeReader.In_missing)
                self.humidity = self.humidity.filled(GenericProbeReader.Out_missing)
        return self.humidity


    def get_lights(self):
        """Return lights"""
        if not hasattr(self, "lights"):
            self.lights = None
            if "lights" in self.data_dict:
                self.lights = np.array(self.data_dict["lights"], dtype=np.short)
                self.lights = ma.masked_values(self.lights, GenericProbeReader.In_missing)
                self.lights = self.lights.filled(GenericProbeReader.Out_missing)
        return self.lights
        
    def get_speed_meters_per_second(self):
        """Return speed in meters per second converting from miles per hour"""
        if not hasattr(self, "speed_mps"):
            self.speed_mps = None
            if "speed_mph" in self.data_dict:
                self.speed_mps = np.array(self.data_dict["speed_mph"], dtype=np.float32)
                self.speed_mps = ma.masked_values(self.speed_mps, GenericProbeReader.In_missing)
                self.speed_mps = miles_per_hour_to_meters_per_second(self.speed_mps)
                self.speed_mps = self.speed_mps.filled(GenericProbeReader.Out_missing)
        return self.speed_mps

    def get_accel_lat(self):
        """Return latitudinal acceleration"""
        if not hasattr(self, "accel_lat"):
            self.accel_lat = None
            if "accel_lat" in self.data_dict:
                self.accel_lat = np.array(self.data_dict["accel_lat"], dtype=np.float32)
                self.accel_lat = ma.masked_values(self.accel_lat, GenericProbeReader.In_missing)
                self.accel_lat = self.accel_lat.filled(GenericProbeReader.Out_missing)
        return self.accel_lat

    def get_accel_long(self):
        """Return longitudinal acceleration"""
        if not hasattr(self, "accel_long"):
            self.accel_long = None
            if "accel_long" in self.data_dict:
                self.accel_long = np.array(self.data_dict["accel_long"], dtype=np.float32)
                self.accel_long = ma.masked_values(self.accel_long, GenericProbeReader.In_missing)
                self.accel_long = self.accel_long.filled(GenericProbeReader.Out_missing)
        return self.accel_long


    def get_stab(self):
        """Return stability"""
        if not hasattr(self, "stab"):
            self.stab = None
            if "stab" in self.data_dict:
                self.stab = np.array(self.data_dict["stab"], dtype=np.short)
                self.stab = ma.masked_values(self.stab, GenericProbeReader.In_missing)
                self.stab = self.stab + 1
                self.stab = self.stab.filled(GenericProbeReader.Out_missing)
        return self.stab

    def get_surface_temperature(self):
        """Return surface temperature in Celsius converting from Fahrenheit"""
        if not hasattr(self, "surface_temperature"):
            self.surface_temperature = None
            if "surface_temp" in self.data_dict:
                self.surface_temperature = np.array(self.data_dict["surface_temp"], dtype=np.float32)
                self.surface_temperature = ma.masked_values(self.surface_temperature, GenericProbeReader.In_missing)
                self.surface_temperature = fahrenheit_to_celsius(self.surface_temperature)
                self.surface_temperature = self.surface_temperature.filled(GenericProbeReader.Out_missing)
        return self.surface_temperature

    def get_time(self):
        """Convert time to Unix seconds"""
        if not hasattr(self, "time"):
            self.unix_time = []
            # time is in format hh:mm:ss
            for time_string in self.data_dict["time"]:
                #print date_time
                #self.unix_time.append(tim.datetogmt(date_time))
                #print tim.datetogmt(date_time)

                # Convert to UTC time since times are local
                obs_time = calendar.timegm(time.strptime(time_string, "%Y%m%d%H%M%S"))
                self.unix_time.append(obs_time)

        return self.unix_time


    def get_vehicle_id(self):
        """Return vehicle id"""
        if not hasattr(self, "vehicle_id"):
            self.vehicle_id = None
            tmp_list = []
            vid_list = []

            if "vehicle_id" in self.data_dict:

                vid_list = self.data_dict["vehicle_id"]
                for index, value in enumerate(vid_list):
                    tmp_list.append(list(value.ljust(32,'\0')))
                self.vehicle_id = np.array(tmp_list, dtype=object)

        return self.vehicle_id
                

    def get_trac(self):
        """Return traction control"""
        if not hasattr(self, "trac"):
            self.trac = None
            if "trac" in self.data_dict:
                self.trac = np.array(self.data_dict["trac"], dtype=np.short)
                self.trac = ma.masked_values(self.trac, GenericProbeReader.In_missing)
                self.trac = self.trac + 1
                self.trac = self.trac.filled(GenericProbeReader.Out_missing)
        return self.trac

    def get_wiper_status(self):
        """Return wiper status"""
        if not hasattr(self, "wiper_status"):
            self.wiper_status = None
            if "wiper_status" in self.data_dict:
                self.wiper_status = np.array(self.data_dict["wiper_status"], dtype=np.short)
                self.wiper_status = ma.masked_values(self.wiper_status, GenericProbeReader.In_missing)
                self.wiper_status = self.wiper_status + 1
                self.wiper_status = self.wiper_status.filled(GenericProbeReader.Out_missing)
        return self.wiper_status

    def get_steering_angle(self):
        """Return steering angle"""
        if not hasattr(self, "steering_angle"):
            self.steering_angle = None
            if "steering_angle" in self.data_dict:
                self.steering_angle = np.array(self.data_dict["steering_angle"], dtype=np.float32)
                self.steering_angle = ma.masked_values(self.steering_angle, GenericProbeReader.In_missing)
                self.steering_angle = self.steering_angle.filled(GenericProbeReader.Out_missing)
        return self.steering_angle

    def get_yaw_rate(self):
        """Return yaw rate"""
        if not hasattr(self, "yaw_rate"):
            self.yaw_rate = None
            if "yaw_rate" in self.data_dict:
                self.yaw_rate = np.array(self.data_dict["yaw_rate"], dtype=np.float32)
                self.yaw_rate = ma.masked_values(self.yaw_rate, GenericProbeReader.In_missing)
                self.yaw_rate = self.yaw_rate.filled(GenericProbeReader.Out_missing)
        return self.yaw_rate

    def get_rec_time(self):
        """Return received time"""
        # Get length of latitude to determine how long rec_time should be
        latitude = None
        if not hasattr(self, "received_time"):
            if "latitude" in self.data_dict:
                latitude = np.array(self.data_dict["latitude"], dtype=np.float64)
                latitude = ma.masked_values(latitude, GenericProbeReader.In_missing)
                latitude = latitude.filled(GenericProbeReader.Out_missing)
        else:
            latitude = self.latitude
        current_time_value = time.time()
        # Truncate to the nearest minute and then subtract 1 second
        # Need rec_times to be within the five minute period for runs in
        # the VDT for persistence and step tests
        adjusted_time_value = ((int(current_time_value)/60) *60) - 1
        rec_time = [adjusted_time_value] * len(latitude)
        return rec_time

    def __repr__(self):

        ret_string = "<GenericProbeReader "
        for token in sorted(self.file_header_dict.keys()):
            ret_string += " %s %s," % (token, self.file_header_dict[token])
        ret_string += ">"
            
        return ret_string

    # Maps the netcdf output variable name to the function to be called
    # to get the appropriate value
    Field_dict = {'rec_time': get_rec_time, 'obs_time': get_time, 'latitude': get_latitude, 'longitude': get_longitude, 'elevation': get_elevation,
                  'speed': get_speed_meters_per_second, 'hoz_accel_lat': get_accel_lat, 'hoz_accel_long': get_accel_long, 'humidity': get_humidity,
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
    generic = GenericProbeReader(in_file, logg)
    print generic
    #print "airtemp: ", generic.get_air_temperature2()
    #print "surfacetemp: ", generic.get_surface_temperature()
    #print "latitude: ", generic.get_latitude()
    #print "longitude: ", generic.get_longitude()
    #print "speed: ", generic.get_speed_meters_per_second()
    #print "elevation: ", generic.get_elevation()
    #print "abs: ", generic.get_abs()
    #print "stab: ", generic.get_stab()
    #print "heading: ", generic.get_heading()
    #print "yaw: ", generic.get_yaw_rate()
    #print "trac: ", generic.get_trac()
    #print "steering_angle: ", generic.get_steering_angle()
    #print "dewpoint: ", generic.get_dewpoint()
    #print "brake: ", generic.get_brake()
    #print "lights: ", generic.get_lights()
    #print "accel_lat: ", generic.get_accel_lat()
    #print "accel_long: ", generic.get_accel_long()
    print "vehicle_id: ", generic.get_vehicle_id()
    print "speed", generic.get_speed_meters_per_second()
    print "air temperature", generic.get_air_temperature()
    print "latitude", generic.get_latitude()
    print "longitude", generic.get_longitude()

if __name__ == "__main__":

   main()
