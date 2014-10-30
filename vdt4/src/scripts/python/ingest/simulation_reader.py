#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: simulation_reader.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/06/18 20:19:50 $           #
#                                                                                #
# ============================================================================== #

import numpy
import os
import sys
import time
from optparse import OptionParser

class SimulationReader(object):

    Token_list = ["Size:", "File Name:", "VIN:", "Date:", "Air Temp:", "Barometer:", "Coolant Temp:", "Odometer:"]
    In_missing = 10001
    In_missing_string = '10001'
    Out_missing = -9999
    Out_missing_string = '-9999'
   
    def __init__(self, size):

        self.size = size
        self.heading = numpy.zeros(size, dtype=numpy.float32) + 180
        self.abs = numpy.zeros(size, dtype=numpy.int8) + 2
        self.air_temperature2 = numpy.zeros(size, dtype=numpy.float32) + 20
        self.brake = numpy.zeros(size, dtype=numpy.int8) + 15
        self.latitude = numpy.zeros(size, dtype=numpy.float32) + 35
        self.longitude = numpy.zeros(size, dtype=numpy.float32) + 190
        self.dewpoint = numpy.zeros(size, dtype=numpy.float32) + 20
        self.elevation = numpy.zeros(size, dtype=numpy.float32) + 1000
        self.humidity = numpy.zeros(size, dtype=numpy.float32) + 60
        self.lights = numpy.zeros(size, dtype=numpy.int8) + 64
        self.speed_mps = numpy.zeros(size, dtype=numpy.float32) + 20
        self.accel_lat = numpy.zeros(size, dtype=numpy.float32) + 5
        self.accel_long = numpy.zeros(size, dtype=numpy.float32) + 5
        self.surface_temperature = numpy.zeros(size, dtype=numpy.float32) + 20
        self.stab = numpy.zeros(size, dtype=numpy.int16) + 3
        self.trac = numpy.zeros(size, dtype=numpy.int16) + 3
        self.unix_time = numpy.zeros(size, dtype=numpy.float64) + time.time()
        self.steering_angle = numpy.zeros(size, dtype=numpy.float32) + 180
        self.yaw_rate = numpy.zeros(size, dtype=numpy.float32) + 30
        self.rec_time = numpy.zeros(size, dtype=numpy.float64) + time.time()
        self.vehicle_id = None
        

    def get_field(self, field):
        """Return appropriate values for given field"""
        if field in SimulationReader.Field_dict.keys():
            return self.Field_dict[field](self)            
            
    def get_heading(self):
        """Return heading"""
        return self.heading
    
    def get_abs(self):
        """Return automatic brake system status"""
        return self.abs

    def get_air_temperature2(self):
        """Return air temperature in Celsius converting from Fahrenheit"""
        return self.air_temperature2

    def get_brake(self):
        """Return brake status"""
        return self.brake

    def get_latitude(self):
        return self.latitude

    def get_longitude(self):
        return self.longitude

    def get_elevation(self):
        """Return elevation in meters"""
        return self.elevation

    def get_dewpoint(self):
        """Return dewpoint in Celsius"""
        return self.dewpoint

    def get_humidity(self):
        """Return humidity"""
        return self.humidity

    def get_lights(self):
        """Return lights"""
        return self.lights
        
    def get_speed_meters_per_second(self):
        """Return speed in meters per second"""
        return self.speed_mps

    def get_accel_lat(self):
        """Return latitudinal acceleration"""
        return self.accel_lat

    def get_accel_long(self):
        """Return longitudinal acceleration"""
        return self.accel_long

    def get_stab(self):
        """Return stability"""
        return self.stab

    def get_surface_temperature(self):
        """Return surface temperature in Celsius converting from Fahrenheit"""
        return self.surface_temperature

    def get_time(self):
        """Convert time to Unix seconds"""
        return self.unix_time

    def get_vehicle_id(self):
        """Return vehicle id"""
        return self.vehicle_id
                
    def get_trac(self):
        """Return traction control"""
        return self.trac

    def get_steering_angle(self):
        """Return steering angle"""
        return self.steering_angle

    def get_yaw_rate(self):
        """Return yaw rate"""
        return self.yaw_rate

    def get_rec_time(self):
        """Return received time"""
        return self.rec_time

    # Maps the netcdf output variable name to the function to be called
    # to get the appropriate value
    Field_dict = {'rec_time': get_rec_time, 'obs_time': get_time, 'latitude': get_latitude, 'longitude': get_longitude, 'elevation': get_elevation,
                  'speed': get_speed_meters_per_second, 'hoz_accel_lat': get_accel_lat, 'hoz_accel_long': get_accel_long, 'humidity':get_humidity,
                  'surface_temp': get_surface_temperature, 'air_temp2': get_air_temperature2, 'dew_temp': get_dewpoint,
                  'heading': get_heading, 'abs': get_abs, 'brake_status': get_brake, 'lights': get_lights, 'stab': get_stab,
                  'trac': get_trac, 'steering_angle': get_steering_angle, 'yaw_rate': get_yaw_rate}

def main():

    usage_str = "%prog"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    SimulationReader(10)

if __name__ == "__main__":

   main()
