#!/usr/bin/env python

"""Read Nevada csv file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: nevada_reader.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/22 21:50:09 $           #
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

# TEST: nevada_reader.py /d2/vii/data/raw/nevada/20130904/nevada_20130904.csv | more
# TEST: nevada_reader.py /d4/test/nevada/nevada_20130904.csv | more

def kPa_to_hPa(bar_pressure):

    hPa_value = bar_pressure * 10

    return hPa_value

class NevadaImo2(object):

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
        
        self.column_headers = lines[column_header_index].split(",")
        #print self.column_headers
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
                #print self.column_headers[ind]
                #if self.column_headers[ind] == "SpGPS(m/s)\r\n":
                #    print "spl[ind]: ", spl[ind]
                #    if spl[ind] == "\r\n":
                #        print "YES"
            
                if spl[ind] == "" or spl[ind] == "\r\n":
                    #if self.column_headers[ind] == "SpGPS(m/s)\r\n":
                    #    print "spl[ind] is empty string"
                    if not self.column_headers[ind] == 'VehicleID':
                        spl[ind] = NevadaImo2.In_missing_float

                self.data_dict[self.column_headers[ind]].append(spl[ind])

        #print self.data_dict["ATCanbus(\xbaC)"]

    def get_field(self, field):
        """Return appropriate values for given field"""
        if field in NevadaImo2.Field_dict.keys():
            return self.Field_dict[field](self)            


    def get_time(self):
        """Convert date and time to Unix seconds"""
        if not hasattr(self, "unix_time"):
            self.unix_time = None
            date = ""
            self.unix_time = []
            # date is in format yyyy-mm-dd        
            # time is in format hh:mm:ss
            for index, value in enumerate(self.data_dict["Date(UTC)"]):
                date = "%s%s%s" % (value[0:4], value[5:7], value[8:10])
                time_string = self.data_dict["Time(UTC)"][index]
                date_time = "%s%s%s%s" % (date, time_string[0:2], time_string[3:5], time_string[6:8])
                self.unix_time.append(tim.datetogmt(date_time))
        
        return self.unix_time
        
    def get_latitude(self):
        if not hasattr(self, "latitude"):
            self.latitude = None
            if "Latitude" in self.data_dict:
                self.latitude = np.array(self.data_dict["Latitude"], dtype=np.float64)
                self.latitude = ma.masked_values(self.latitude, NevadaImo2.In_missing_float)
                self.latitude = self.latitude.filled(NevadaImo2.Out_missing)
        return self.latitude

    def get_longitude(self):
        if not hasattr(self, "longitude"):
            self.longitude = None
            if "Longitude" in self.data_dict:
                self.longitude = np.array(self.data_dict["Longitude"], dtype=np.float64)
                self.longitude = ma.masked_values(self.longitude, NevadaImo2.In_missing_float)
                self.longitude = self.longitude.filled(NevadaImo2.Out_missing)
        return self.longitude

    def get_vehicle_id(self):
        if not hasattr(self, "vehicle_id"):
            tmp_list = []
            if "VehicleID" in self.data_dict:
                for index, value in enumerate(self.data_dict["VehicleID"]):
                    tmp_list.append(list(value.ljust(32,'\0')))                
                self.vehicle_id = np.array(tmp_list, dtype=object)    

        return self.vehicle_id

    def get_air_temperature(self):
        if not hasattr(self, "air_temperature"):
            self.air_temperature = None
            if "ATCanbus(\xbaC)" in self.data_dict:
                self.air_temperature = np.array(self.data_dict["ATCanbus(\xbaC)"], dtype=np.float64)
                self.air_temperature = ma.masked_values(self.air_temperature, NevadaImo2.In_missing_float)
                self.air_temperature = self.air_temperature.filled(NevadaImo2.Out_missing)

        #for index, value in enumerate(self.data_dict["ATCanbus(\xbaC)"]):
        #    print value, type(value)
            
        return self.air_temperature

    def get_air_temperature2(self):
        if not hasattr(self, "air_temperature2"):
            self.air_temperature2 = None
            if "ATVaisala(\xbaC)" in self.data_dict:
                self.air_temperature2 = np.array(self.data_dict["ATVaisala(\xbaC)"], dtype=np.float64)
                self.air_temperature2 = ma.masked_values(self.air_temperature2, NevadaImo2.In_missing_float)
                self.air_temperature2 = self.air_temperature2.filled(NevadaImo2.Out_missing)
                
        #for index, value in enumerate(self.data_dict["ATVaisala(\xbaC)"]):
        #    print value, type(value)
            
        return self.air_temperature2

    def get_humidity(self):
        if not hasattr(self, "humidity"):
            self.humidity = None
            if "RHVaisala(%)" in self.data_dict:
                self.humidity = np.array(self.data_dict["RHVaisala(%)"], dtype=np.float64)
                self.humidity = ma.masked_values(self.humidity, NevadaImo2.In_missing_float)
                self.humidity = self.humidity.filled(NevadaImo2.Out_missing)

        return self.humidity

    def get_bar_pressure(self):
        if not hasattr(self, "bar_pressure"):
            self.bar_pressure = None
            if "PRCanbus(kPa)" in self.data_dict:
                self.bar_pressure = np.array(self.data_dict["PRCanbus(kPa)"], dtype=np.float64)
                self.bar_pressure = ma.masked_values(self.bar_pressure, NevadaImo2.In_missing_float)
                self.bar_pressure = kPa_to_hPa(self.bar_pressure)
                self.bar_pressure = self.bar_pressure.filled(NevadaImo2.Out_missing)

        return self.bar_pressure

    def get_surface_temperature(self):
        if not hasattr(self, "surface_temp"):
            self.surface_temp = None
            if "RTVaisala(\xbaC)" in self.data_dict:
                self.surface_temp = np.array(self.data_dict["RTVaisala(\xbaC)"], dtype=np.float64)
                self.surface_temp = ma.masked_values(self.surface_temp, NevadaImo2.In_missing_float)
                self.surface_temp = self.surface_temp.filled(NevadaImo2.Out_missing)

        return self.surface_temp

    def get_speed(self):
        if not hasattr(self, "speed"):
            self.speed = None
            if "SpGPS(m/s)\r\n" in self.data_dict:
                #print self.data_dict["SpGPS(m/s)\r\n"]
                self.speed = np.array(self.data_dict["SpGPS(m/s)\r\n"], dtype=np.float64)
                self.speed = ma.masked_values(self.speed, NevadaImo2.In_missing_float)
                self.speed = self.speed.filled(NevadaImo2.Out_missing)

        return self.speed

    def get_rec_time(self):
        """Return received time"""
        # Get length of latitude to determine how long rec_time should be
        if not hasattr(self, "latitude"):
            latitude = None
            if "Latitude" in self.data_dict:
                latitude = np.array(self.data_dict["Latitude"], dtype=np.float64)
                latitude = ma.masked_values(latitude, NevadaImo2.In_missing_float)
                latitude = latitude.filled(NevadaImo2.Out_missing)
                
        current_time_value = time.time()
        # Truncate to the nearest minute and then subtract 1 second
        # Need rec_times to be within the five minute period for runs in
        # the VDT for persistence and step tests
        adjusted_time_value = ((int(current_time_value)/60) *60) - 1
        rec_time = [adjusted_time_value] * len(latitude)
        return rec_time

    # Maps the netcdf output variable name to the function to be called
    # to get the appropriate value
    Field_dict = {'rec_time': get_rec_time, 'vehicle_id': get_vehicle_id, 'obs_time': get_time,  'latitude': get_latitude,
                  'longitude': get_longitude, 'air_temp': get_air_temperature, 'air_temp2': get_air_temperature2,
                  'humidity': get_humidity, 'bar_pressure': get_bar_pressure, 'surface_temp': get_surface_temperature,
                  'speed': get_speed
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
    
    nevada = NevadaImo2(in_file, logg)
    #print nevada
    #print len(nevada.get_latitude())
    #print len(nevada.get_vehicle_id())
    #print "unix_time: ", nevada.get_time()
    #print "latitude: ", nevada.get_latitude()
    #print "longitude: ", nevada.get_longitude()
    #print "vehicle_id: ", nevada.get_vehicle_id()
    #print "air_temp: ", nevada.get_air_temperature()
    #print "air_temp2: ", nevada.get_air_temperature2()
    #print "humidity: ", nevada.get_humidity()
    #print "bar_pressure: ", nevada.get_bar_pressure()
    #print "surface_temp: ", nevada.get_surface_temperature()
    #print "speed: ", nevada.get_speed()
    
    """
    print ": ", nevada.get_()
    print ": ", nevada.get_()
    """
    logg.write_ending()

if __name__ == "__main__":

   main()
