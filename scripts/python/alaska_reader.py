#!/usr/bin/env python

"""Read Alaska csv file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: alaska_reader.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/04/21 22:03:36 $           #
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

# TEST: alaska_reader.py /d2/vii/data/raw/alaska/20131216/alaska_20131216.csv | more
# TEST: alaska_reader.py test_alaska.20131217.csv | more

def kPa_to_hPa(bar_pressure):

    hPa_value = bar_pressure * 10

    return hPa_value

class AlaskaImo2(object):

    In_missing = ""
    In_missing_string = ""
    In_missing_float = -9999
    In_missing_list = []
    Out_missing = -9999
    Out_missing_fill = " "
    

    def __init__(self, file_name, logg):
        
        lines = file(file_name).readlines()

        # Get column headers line
        column_header_index = 0
        for ind in range(0, len(lines)):
            spl = lines[ind].split(",")
            if len(spl[0].strip()) > 0:
                column_header_index = ind
                break

        # Added this rstript to remove '\n' and '\r' from last column of header (for DewpointTemp)
        lines[column_header_index] = lines[column_header_index].rstrip('\n' +'\r')
        self.column_headers = lines[column_header_index].split(",")
        print self.column_headers
        self.data_dict = {}
        
        for column in self.column_headers:
            self.data_dict[column] = []

        # Collect data for each column header and store in lists
        for line_ind in range(1, len(lines)):
            spl = lines[line_ind].split(",")

            if len(spl) > len(self.column_headers):
                self.error = "The number of columns of data %d exceeds the number of columns in the header %d." % (len(spl), len(self.column_headers))
                logg.write_error("The number of columns of data %d exceeds the number of columns in the header %d. Trimming line." % (len(spl), len(self.column_headers)))
                spl = spl[0:len(self.column_headers)]
            
            for ind in range(len(spl)):
                # Added this rstript to remove '\n' and '\r' from each line
                # This fixes bug in reading in the values from the last column (DewpointTemp)
                spl[ind] = spl[ind].rstrip('\n' +'\r')
                if spl[ind] == "" or spl[ind] == "\r\n":
                    if not self.column_headers[ind] == 'VehicleID':
                        spl[ind] = AlaskaImo2.In_missing_float

                #print "self.column_headers[ind] = ", self.column_headers[ind]
                self.data_dict[self.column_headers[ind]].append(spl[ind])

        #print self.data_dict["AirTemp"]
        #print self.data_dict["DewpointTemp"]

    def get_field(self, field):
        """Return appropriate values for given field"""
        if field in AlaskaImo2.Field_dict.keys():
            #print " self.Field_dict[%s]: %s" % (field, self.Field_dict[field])
            return self.Field_dict[field](self)            


    #def get_time(self):
    #    """Convert date and time to Unix seconds"""
    #    if not hasattr(self, "unix_time"):
    #        self.unix_time = None
    #        date = ""
    #        self.unix_time = []
    #        # date is in format yyyy-mm-dd        
    #        # time is in format hh:mm:ss
    #        for index, value in enumerate(self.data_dict["Date(UTC)"]):
    #            date = "%s%s%s" % (value[0:4], value[5:7], value[8:10])
    #            time_string = self.data_dict["Time(UTC)"][index]
    #            date_time = "%s%s%s%s" % (date, time_string[0:2], time_string[3:5], time_string[6:8])
    #            self.unix_time.append(tim.datetogmt(date_time))
    #    
    #    return self.unix_time
        
    def get_time(self):
        if not hasattr(self, "unix_time"):
            self.unix_time = None
            if "Time(UTC)" in self.data_dict:
                self.unix_time = np.array(self.data_dict["Time(UTC)"], dtype=np.float64)
                self.unix_time = ma.masked_values(self.unix_time, AlaskaImo2.In_missing_float)
                self.unix_time = self.unix_time.filled(AlaskaImo2.Out_missing)

        #for index, value in enumerate(self.data_dict["Time(UTC)"]):
        #    print value, type(value)
            
        return self.unix_time
        
    def get_latitude(self):
        if not hasattr(self, "latitude"):
            self.latitude = None
            if "Latitude" in self.data_dict:
                self.latitude = np.array(self.data_dict["Latitude"], dtype=np.float64)
                self.latitude = ma.masked_values(self.latitude, AlaskaImo2.In_missing_float)
                self.latitude = self.latitude.filled(AlaskaImo2.Out_missing)
        return self.latitude

    def get_longitude(self):
        if not hasattr(self, "longitude"):
            self.longitude = None
            if "Longitude" in self.data_dict:
                self.longitude = np.array(self.data_dict["Longitude"], dtype=np.float64)
                self.longitude = ma.masked_values(self.longitude, AlaskaImo2.In_missing_float)
                self.longitude = self.longitude.filled(AlaskaImo2.Out_missing)
        return self.longitude

    def get_speed(self):
        if not hasattr(self, "speed"):
            self.speed = None
            if "Speed" in self.data_dict:
                self.speed = np.array(self.data_dict["Speed"], dtype=np.float64)
                self.speed = ma.masked_values(self.speed, AlaskaImo2.In_missing_float)
                self.speed = self.speed.filled(AlaskaImo2.Out_missing)
        return self.speed

    def get_heading(self):
        if not hasattr(self, "heading"):
            self.heading = None
            if "Heading" in self.data_dict:
                self.heading = np.array(self.data_dict["Heading"], dtype=np.float64)
                self.heading = ma.masked_values(self.heading, AlaskaImo2.In_missing_float)
                self.heading = self.heading.filled(AlaskaImo2.Out_missing)
        return self.heading

    def get_vehicle_id(self):
        if not hasattr(self, "vehicle_id"):
            tmp_list = []
            if "VehicleID" in self.data_dict:
                #print self.data_dict["VehicleID"]
                for index, value in enumerate(self.data_dict["VehicleID"]):
                    #print value
                    tmp_list.append(list(value.ljust(32,'\0')))                
                self.vehicle_id = np.array(tmp_list, dtype=object)
                
        return self.vehicle_id

    def get_air_temperature(self):
        if not hasattr(self, "air_temperature"):
            self.air_temperature = None
            if "AirTemp" in self.data_dict:
                self.air_temperature = np.array(self.data_dict["AirTemp"], dtype=np.float64)
                self.air_temperature = ma.masked_values(self.air_temperature, AlaskaImo2.In_missing_float)
                self.air_temperature = self.air_temperature.filled(AlaskaImo2.Out_missing)

        #for index, value in enumerate(self.data_dict["AirTemp"]):
        #    print value, type(value)
            
        return self.air_temperature

    def get_humidity(self):
        if not hasattr(self, "humidity"):
            self.humidity = None
            if "RelativeHumidity" in self.data_dict:
                self.humidity = np.array(self.data_dict["RelativeHumidity"], dtype=np.float64)
                self.humidity = ma.masked_values(self.humidity, AlaskaImo2.In_missing_float)
                self.humidity = self.humidity.filled(AlaskaImo2.Out_missing)

        return self.humidity

    def get_surface_temperature(self):
        if not hasattr(self, "surface_temp"):
            self.surface_temp = None
            if "RoadTemp" in self.data_dict:
                self.surface_temp = np.array(self.data_dict["RoadTemp"], dtype=np.float64)
                self.surface_temp = ma.masked_values(self.surface_temp, AlaskaImo2.In_missing_float)
                self.surface_temp = self.surface_temp.filled(AlaskaImo2.Out_missing)

        return self.surface_temp

    def get_dewpt_temperature(self):
        if not hasattr(self, "dewpt_temp"):
            self.dewpt_temp = None
            if "DewpointTemp" in self.data_dict:
                self.dewpt_temp = np.array(self.data_dict["DewpointTemp"], dtype=np.float64)
                self.dewpt_temp = ma.masked_values(self.dewpt_temp, AlaskaImo2.In_missing_float)
                self.dewpt_temp = self.dewpt_temp.filled(AlaskaImo2.Out_missing)

        #for index, value in enumerate(self.data_dict["DewpointTemp"]):
        #    print value, type(value)
        
        return self.dewpt_temp

    # Maps the netcdf output variable name to the function to be called
    # to get the appropriate value
    Field_dict = {'vehicle_id': get_vehicle_id, 'obs_time': get_time,  'latitude': get_latitude,
                  'longitude': get_longitude, 'speed': get_speed, 'heading': get_heading,
                  'air_temp': get_air_temperature, 'humidity': get_humidity,
                  'surface_temp': get_surface_temperature, 'dew_temp': get_dewpt_temperature
                  }
    # For debugging:
    # np.set_printoptions(threshold='nan')


def main():

    usage_str = "%prog in_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    in_file = args[0]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    
    alaska = AlaskaImo2(in_file, logg)
    #print alaska
    #print "lenght of latitude array: ", len(alaska.get_latitude())
    #print "length of vehicle_id array: ", len(alaska.get_vehicle_id())
    #print "unix_time: ", alaska.get_time()
    #print "latitude: ", alaska.get_latitude()
    #print "longitude: ", alaska.get_longitude()
    #print "speed: ", alaska.get_speed()
    #print "heading: ", alaska.get_heading()
    #print "vehicle_id: ", alaska.get_vehicle_id()
    #print "air_temp: ", alaska.get_air_temperature()
    #print "humidity: ", alaska.get_humidity()
    #print "surface_temp: ", alaska.get_surface_temperature()
    #print "dew_temp: ", alaska.get_dewpt_temperature()
    
    logg.write_ending()

if __name__ == "__main__":

   main()
