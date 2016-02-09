#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: wxt_parse.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/09/29 18:35:29 $           #
#                                                                                #
# ============================================================================== #

import fields
import numpy
import log_msg
import os
import sys
from optparse import OptionParser
from lxml import etree
from netCDF4 import chartostring, stringtoarr
import wxt_cdf

# Equipment List
# Friendly name     Equipment type     Field id
# Road Temperature     Passive InfraRed     208
# Air Temperature     Semiconductor     209
# Rain Intensity         210
# Relative Humidity     Absorption     211
# Light Level         10100
# Atmospheric Pressure         10101
# Ozone Level         10102
# GPS Elevation     GPS Elevation     10104

Dates = [
("2013-01-31T00:00:00Z", "2013-01-31T01:00:00Z"),
("2013-01-31T01:00:00Z", "2013-01-31T02:00:00Z"),
("2013-01-31T02:00:00Z", "2013-01-31T03:00:00Z"),
("2013-01-31T03:00:00Z", "2013-01-31T04:00:00Z"),
("2013-01-31T04:00:00Z", "2013-01-31T05:00:00Z"),
("2013-01-31T05:00:00Z", "2013-01-31T06:00:00Z"),
("2013-01-31T06:00:00Z", "2013-01-31T07:00:00Z"),
("2013-01-31T07:00:00Z", "2013-01-31T08:00:00Z"),
("2013-01-31T08:00:00Z", "2013-01-31T09:00:00Z"),
("2013-01-31T09:00:00Z", "2013-01-31T10:00:00Z"),
("2013-01-31T10:00:00Z", "2013-01-31T11:00:00Z"),
("2013-01-31T11:00:00Z", "2013-01-31T12:00:00Z"),
("2013-01-31T12:00:00Z", "2013-01-31T13:00:00Z"),
("2013-01-31T13:00:00Z", "2013-01-31T14:00:00Z"),
("2013-01-31T14:00:00Z", "2013-01-31T15:00:00Z"),
("2013-01-31T15:00:00Z", "2013-01-31T16:00:00Z"),
("2013-01-31T16:00:00Z", "2013-01-31T17:00:00Z"),
("2013-01-31T17:00:00Z", "2013-01-31T18:00:00Z"),
("2013-01-31T18:00:00Z", "2013-01-31T19:00:00Z"),
("2013-01-31T19:00:00Z", "2013-01-31T20:00:00Z"),
("2013-01-31T20:00:00Z", "2013-01-31T21:00:00Z"),
("2013-01-31T21:00:00Z", "2013-01-31T22:00:00Z"),
("2013-01-31T22:00:00Z", "2013-01-31T23:00:00Z"),
("2013-01-31T23:00:00Z", "2013-02-01T00:00:00Z")
]

Dates30 = [
("2013-01-30T00:00:00Z", "2013-01-30T01:00:00Z"),
("2013-01-30T01:00:00Z", "2013-01-30T02:00:00Z"),
("2013-01-30T02:00:00Z", "2013-01-30T03:00:00Z"),
("2013-01-30T03:00:00Z", "2013-01-30T04:00:00Z"),
("2013-01-30T04:00:00Z", "2013-01-30T05:00:00Z"),
("2013-01-30T05:00:00Z", "2013-01-30T06:00:00Z"),
("2013-01-30T06:00:00Z", "2013-01-30T07:00:00Z"),
("2013-01-30T07:00:00Z", "2013-01-30T08:00:00Z"),
("2013-01-30T08:00:00Z", "2013-01-30T09:00:00Z"),
("2013-01-30T09:00:00Z", "2013-01-30T10:00:00Z"),
("2013-01-30T10:00:00Z", "2013-01-30T11:00:00Z"),
("2013-01-30T11:00:00Z", "2013-01-30T12:00:00Z"),
("2013-01-30T12:00:00Z", "2013-01-30T13:00:00Z"),
("2013-01-30T13:00:00Z", "2013-01-30T14:00:00Z"),
("2013-01-30T14:00:00Z", "2013-01-30T15:00:00Z"),
("2013-01-30T15:00:00Z", "2013-01-30T16:00:00Z"),
("2013-01-30T16:00:00Z", "2013-01-30T17:00:00Z"),
("2013-01-30T17:00:00Z", "2013-01-30T18:00:00Z"),
("2013-01-30T18:00:00Z", "2013-01-30T19:00:00Z"),
("2013-01-30T19:00:00Z", "2013-01-30T20:00:00Z"),
("2013-01-30T20:00:00Z", "2013-01-30T21:00:00Z"),
("2013-01-30T21:00:00Z", "2013-01-30T22:00:00Z"),
("2013-01-30T22:00:00Z", "2013-01-30T23:00:00Z"),
("2013-01-30T23:00:00Z", "2013-01-31T00:00:00Z")
]

Dates21 = [
("2012-12-21T00:00:00Z", "2012-12-21T01:00:00Z"),
("2012-12-21T01:00:00Z", "2012-12-21T02:00:00Z"),
("2012-12-21T02:00:00Z", "2012-12-21T03:00:00Z"),
("2012-12-21T03:00:00Z", "2012-12-21T04:00:00Z"),
("2012-12-21T04:00:00Z", "2012-12-21T05:00:00Z"),
("2012-12-21T05:00:00Z", "2012-12-21T06:00:00Z"),
("2012-12-21T06:00:00Z", "2012-12-21T07:00:00Z"),
("2012-12-21T07:00:00Z", "2012-12-21T08:00:00Z"),
("2012-12-21T08:00:00Z", "2012-12-21T09:00:00Z"),
("2012-12-21T09:00:00Z", "2012-12-21T10:00:00Z"),
("2012-12-21T10:00:00Z", "2012-12-21T11:00:00Z"),
("2012-12-21T11:00:00Z", "2012-12-21T12:00:00Z"),
("2012-12-21T12:00:00Z", "2012-12-21T13:00:00Z"),
("2012-12-21T13:00:00Z", "2012-12-21T14:00:00Z"),
("2012-12-21T14:00:00Z", "2012-12-21T15:00:00Z"),
("2012-12-21T15:00:00Z", "2012-12-21T16:00:00Z"),
("2012-12-21T16:00:00Z", "2012-12-21T17:00:00Z"),
("2012-12-21T17:00:00Z", "2012-12-21T18:00:00Z"),
("2012-12-21T18:00:00Z", "2012-12-21T19:00:00Z"),
("2012-12-21T19:00:00Z", "2012-12-21T20:00:00Z"),
("2012-12-21T20:00:00Z", "2012-12-21T21:00:00Z"),
("2012-12-21T21:00:00Z", "2012-12-21T22:00:00Z"),
("2012-12-21T22:00:00Z", "2012-12-21T23:00:00Z"),
("2012-12-21T23:00:00Z", "2012-12-22T00:00:00Z")
]
    
Dates17 = [
("2012-12-17T00:00:00Z", "2012-12-17T01:00:00Z"),
("2012-12-17T01:00:00Z", "2012-12-17T02:00:00Z"),
("2012-12-17T02:00:00Z", "2012-12-17T03:00:00Z"),
("2012-12-17T03:00:00Z", "2012-12-17T04:00:00Z"),
("2012-12-17T04:00:00Z", "2012-12-17T05:00:00Z"),
("2012-12-17T05:00:00Z", "2012-12-17T06:00:00Z"),
("2012-12-17T06:00:00Z", "2012-12-17T07:00:00Z"),
("2012-12-17T07:00:00Z", "2012-12-17T08:00:00Z"),
("2012-12-17T08:00:00Z", "2012-12-17T09:00:00Z"),
("2012-12-17T09:00:00Z", "2012-12-17T10:00:00Z"),
("2012-12-17T10:00:00Z", "2012-12-17T11:00:00Z"),
("2012-12-17T11:00:00Z", "2012-12-17T12:00:00Z"),
("2012-12-17T12:00:00Z", "2012-12-17T13:00:00Z"),
("2012-12-17T13:00:00Z", "2012-12-17T14:00:00Z"),
("2012-12-17T14:00:00Z", "2012-12-17T15:00:00Z"),
("2012-12-17T15:00:00Z", "2012-12-17T16:00:00Z"),
("2012-12-17T16:00:00Z", "2012-12-17T17:00:00Z"),
("2012-12-17T17:00:00Z", "2012-12-17T18:00:00Z"),
("2012-12-17T18:00:00Z", "2012-12-17T19:00:00Z"),
("2012-12-17T19:00:00Z", "2012-12-17T20:00:00Z"),
("2012-12-17T20:00:00Z", "2012-12-17T21:00:00Z"),
("2012-12-17T21:00:00Z", "2012-12-17T22:00:00Z"),
("2012-12-17T22:00:00Z", "2012-12-17T23:00:00Z"),
("2012-12-17T23:00:00Z", "2012-12-18T00:00:00Z")]

Equipment_dict = {"208": "road_temp", "209" : "air_temp", "210" : "rain_intensity", "211" : "relative_humidity", "10100" : "light_level", "10101" : "atmospheric_pressure", "10102" : "ozone_level", "10104" : "elevation"}

Observation_fields = ["id", "unit-id", "insertion-time", "observation-time", "latitude", "longitude", "road temp", "air temp", "rain intensity", "relative humidity", "light level", "atmospheric pressure", "ozone level", "gps elevation"]

Tag_translation = {"id" : "id", "latitude" : "latitude", "longitude" : "longitude", "unit-id" : "unit_id", "insertion-time" : "insertion_time", "observation-time" : "observation_time"} 

def unpack(fields, observation_dict):

    missing = fields.missing
    names = fields.names
    types = fields.types
    sizes = fields.sizes
    
    field_list = []
    for ind in range(len(names)):
        name = names[ind]
        type_value = types[ind]
        if name in observation_dict:
            if type_value == "f4":
                field_list.append(float(observation_dict[name]))
            elif type_value == "a32":
                field_list.append(observation_dict[name])
        else:
            field_list.append(missing[ind])

    return tuple(field_list)

class ObservationSets(object):

    def __init__(self, obs_time_string, field_object, observation_set_dict):
        self._obs_time_string = obs_time_string
        self._field_object = field_object
        self._observation_set_dict = observation_set_dict

    @property
    def field_object(self):
        return self._field_object

    @property
    def observation_set_dict(self):
        return self._observation_set_dict

    def get_field_data(self):
        """Extracts fields the observation sets and forms numpy record array"""
        type_info = zip(self._field_object.names, self._field_object.types, self._field_object.sizes)
        for ind in range(len(type_info)):
            if type_info[ind][2] == None:
                type_info[ind] = (type_info[ind][0], type_info[ind][1])
                         
        observation_list = []
        for time_string in sorted(self._observation_set_dict.keys()):
            for observation_dict in self._observation_set_dict[time_string]:
                observation_list.append(unpack(self._field_object, observation_dict))
        dtype = numpy.dtype(type_info)
        array = numpy.array(observation_list, dtype)
        return array
    
    def add_observation_dict(self, observation_dict):
        time_string = observation_dict[self._obs_time_string]
        if time_string in self._observation_set_dict:
            self._observation_set_dict[time_string].append(observation_dict)
        else:
            self._observation_set_dict[time_string] = [observation_dict]
        
    def merge(self, observation_sets):

        if self.field_object == observation_sets.field_object:
            observation_set_dict = observation_sets.observation_set_dict
            for time_string in observation_set_dict.keys():
                for observation_dict in observation_set_dict[time_string]:
                    self.add_observation_dict(observation_dict)
            return 0
        else:
            return -1
        
    def get_date_observation_dict(self, time_string):
        return self._observation_set_dict.get(time_string)
    
    def write_ascii(self, fp_out):
        for time_string in sorted(self._observation_set_dict.keys()):
            for observation_dict in self._observation_set_dict[time_string]:
                print_observation(fp_out, observation_dict)

def print_observation(fp_out, observation):

    obs_id = observation.get("id")
    fp_out.write("id: %s\n" % obs_id);
    unit_id = observation.get("unit_id")
    fp_out.write("unit id: %s\n" % unit_id);
    insertion_time = observation.get("insertion_time")
    fp_out.write("insertion time: %s\n" % insertion_time);
    observation_time = observation.get("observation_time")
    fp_out.write("observation time: %s\n" % observation_time);
    latitude = observation.get("latitude")
    fp_out.write("latitude: %s\n" % latitude);
    longitude = observation.get("longitude")
    fp_out.write("longitude: %s\n" % longitude);
    elevation = observation.get("elevation")
    fp_out.write("gps elevation: %s\n" % elevation);
    air_temp = observation.get("air_temp")
    fp_out.write("air temp: %s\n" % air_temp);
    atmospheric_pressure = observation.get("atmospheric_pressure")
    fp_out.write("atmospheric pressure: %s\n" % atmospheric_pressure);
    light_level = observation.get("light_level")
    fp_out.write("light level: %s\n" % light_level);
    rain_intensity = observation.get("rain_intensity")
    fp_out.write("rain intensity: %s\n" % rain_intensity);
    relative_humidity = observation.get("relative_humidity")
    fp_out.write("relative humidity: %s\n" % relative_humidity);
    road_temp = observation.get("road_temp")
    fp_out.write("road temp: %s\n" % road_temp);
    fp_out.write("\n")
    
class wxtParse(object):

    def __init__(self, in_file):
        try:
            self.tree = etree.parse(in_file)
        except:
            pass
        print "parse2"
        self.root = self.tree.getroot()
        print "parse3"

        self._observation_set_dict = {}
        for element in self.root:
            if element.tag == "observation-sets":
                for _observation_set in element:
                    observation_dict = {}
                    for item in _observation_set:
                        if item.tag != "observations":
                            observation_dict[Tag_translation[item.tag]] = item.text
                        else:
                            for observation in item:
                                field_type = "NA"
                                field_value = "NA"
                                for field in observation:
                                    if field.tag == "equipment-id":
                                        if field.text in Equipment_dict:
                                            field_type = Equipment_dict[field.text]
                                    elif field.tag == "value":
                                        field_value = field.text
                                if field_type != "NA" and field_value != "NA":
                                    observation_dict[field_type] = field_value
                    time_string = observation_dict["observation_time"]
                    if time_string in self._observation_set_dict:
                        self._observation_set_dict[time_string].append(observation_dict)
                    else:
                        self._observation_set_dict[time_string] = [observation_dict]
          
    @property
    def observation_set_dict(self):
        return self._observation_set_dict
    

               
def main():

    usage_str = "%prog in_file[s] out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-d", "--directory", dest="directory", help="use all files in specified directory as input")
    parser.add_option("-n", "--netcdf_file", dest="cdf", action="store_true", help="write output netcdf file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if (not options.directory and len(args) < 2) or (options.directory and len(args) < 1):
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting()
    in_files = []
    if options.directory:
        # create a list of file paths for all files in directory
        dirlist = os.listdir(options.directory)
        for fname in dirlist:
            path = os.path.join(options.directory, fname)
            in_files.append(path)
    else:            
        in_files = args[0:-1]

    out_file = args[-1]
    field_names = ["id", "unit_id", "insertion_time", "observation_time", "latitude", "longitude", "elevation", "air_temp", "atmospheric_pressure", "light_level", "rain_intensity", "relative_humidity", "road_temp"]
    field_types = ["a32", "a32", "a32", "a32", "f4", "f4", "f4", "f4", "f4", "f4", "f4", "f4", "f4"]
    field_units = [None, None, "seconds since 1970-01-01 00 UTC", "seconds since 1970-01-01 00 UTC", "degrees north", "degrees east", "meters", "celsius", "hPa", "lux", None, "percent", "celsius"]
    data = []
    missing = ["", "", "", "", -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]
    sizes = [None, None, None, None, None, None, None, None, None, None, None, None, None]
    field_object = fields.Fields(data, field_names, field_types, field_units, missing, sizes)
    logg.write_info("parsing first input file %s" % in_files[0])
    wxt_parse = wxtParse(in_files[0])
    observation_sets = ObservationSets("observation_time", field_object, wxt_parse.observation_set_dict)

    logg.write_info("parsing input files")
    for in_file in in_files[1:]:
        logg.write_info("parsing %s" % in_file)
        wxt_parse = wxtParse(in_file)
        new_observation_sets = ObservationSets("observation_time", field_object, wxt_parse.observation_set_dict)
        observation_sets.merge(new_observation_sets)
        
    if options.cdf:
        array = observation_sets.get_field_data()
        for date_pairs in Dates:
            choose = numpy.logical_and(numpy.char.greater_equal(array["observation_time"], date_pairs[0]), numpy.char.less(array["observation_time"], date_pairs[1]))
            array_selection = array[choose]
            if array_selection.shape[0] == 0:
                continue
            
            data_fields = []
            for field_name in field_names:
                data_fields.append(array_selection[field_name])
            field_object.data = data_fields
            date_string = date_pairs[0][0:4] + date_pairs[0][5:7] + date_pairs[0][8:10] +  date_pairs[0][11:13]
            out_file_name = "%s.%s.nc" % (out_file, date_string)
            logg.write_info("writing netcdf file %s" % out_file_name)
            wxt_cdf.WxtCdf(out_file_name, field_object)
    else:
        fp_out = file(out_file, "w")
        observation_sets.write_ascii(fp_out)
        fp_out.close()

    logg.write_ending()    

if __name__ == "__main__":

    main()
   
