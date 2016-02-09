#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: wxt_parse_json.py,v $                                           #
#       Version: $Revision: 1.4 $  Dated: $Date: 2014/10/08 19:56:12 $           #
#                                                                                #
# ============================================================================== #

import json
import fields
import numpy
import log_msg
import os
import sys
import tim
from optparse import OptionParser
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

Equipment_dict = {208: "road_temp", 209 : "air_temp", 210 : "rain_intensity", 211 : "relative_humidity", 10100 : "light_level", 10101: "atmospheric_pressure", 10102 : "ozone_level", 10104 : "elevation"}

Observation_fields = ["id", "unit_id", "insertion_time", "observation_time", "latitude", "longitude", "road temp", "air temp", "rain intensity", "relative humidity", "light level", "atmospheric pressure", "ozone level", "gps elevation"]

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

        content = file(in_file).read()
        diction = json.loads(content)
        self._observation_set_dict = {}
        observation_list = diction["observation_sets"]
        for input_observation_dict in observation_list:
            observations = input_observation_dict["observations"]
            observation_dict = {}
            for observation in observations:
                field_type = "NA"
                field_value = "NA"
                equipment_id = observation["equipment_id"]
                field_value = observation["value"]
                if equipment_id in Equipment_dict:
                    field_type = Equipment_dict[equipment_id]
                if field_type != "NA" and field_value != "NA":
                    observation_dict[field_type] = field_value

            for key in input_observation_dict.keys():
                if key != "observations":
                    observation_dict[key] = input_observation_dict[key]

            time_string = input_observation_dict["observation_time"]
            if time_string in self._observation_set_dict:
                self._observation_set_dict[time_string].append(observation_dict)
            else:
                self._observation_set_dict[time_string] = [observation_dict]
          
    @property
    def observation_set_dict(self):
        return self._observation_set_dict


def parse(in_file, out_file, cdfout, logg):
    """
    Takes a single json input file, converts it to netcdf or ASCII, and writes it out 
    """
    files_written = []

    field_names = ["id", "unit_id", "insertion_time", "observation_time", "latitude", "longitude", "elevation", "air_temp", "atmospheric_pressure", "light_level", "rain_intensity", "relative_humidity", "road_temp"]
    field_types = ["a32", "a32", "a32", "a32", "f4", "f4", "f4", "f4", "f4", "f4", "f4", "f4", "f4"]
    field_units = [None, None, "seconds since 1970-01-01 00 UTC", "seconds since 1970-01-01 00 UTC", "degrees north", "degrees east", "meters", "celsius", "hPa", "lux", None, "percent", "celsius"]
    data = []
    missing = ["", "", "", "", -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999., -9999.]
    sizes = [None, None, None, None, None, None, None, None, None, None, None, None, None]
    field_object = fields.Fields(data, field_names, field_types, field_units, missing, sizes)
    logg.write_info("parsing input file %s" % in_file)
    wxt_parse = wxtParse(in_file)
    observation_sets = ObservationSets("observation_time", field_object, wxt_parse.observation_set_dict)
        
    if cdfout:
        array = observation_sets.get_field_data()  
        data_fields = []
        for field_name in field_names:
            data_fields.append(array[field_name])
        field_object.data = data_fields
        logg.write_info("writing netcdf file %s" % out_file)
        files_written.append(out_file)
        wxt_cdf.WxtCdf(out_file, field_object)
    else:
        fp_out = file(out_file, "w")
        files_written.append(out_file)
        observation_sets.write_ascii(fp_out)
        fp_out.close()

    return files_written

    
def parse_hourly(begin_date, end_date, in_files, out_file, cdfout, logg):
    files_written = []
    
    # Formulate date list
    delta_seconds = 3600
    single_dates = tim.date_list_time_delta_iso(begin_date, end_date, delta_seconds)
    if len(single_dates) <= 1:
        logg.write_error("date list too small")
        return files_written
        
    date_pairs = []
    for ind in range(len(single_dates)-1):
        date_pairs.append((single_dates[ind], single_dates[ind+1]))

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
    for ind, in_file in enumerate(in_files[1:]):
        if ind > 0 and (ind % 1000) == 0:
            logg.write_info("finished parsing %d out of %d files" % (ind, len(in_files)))
                            
        wxt_parse = wxtParse(in_file)
        new_observation_sets = ObservationSets("observation_time", field_object, wxt_parse.observation_set_dict)
        observation_sets.merge(new_observation_sets)
        
    if cdfout:
        array = observation_sets.get_field_data()
        for date_pair in date_pairs:
            choose = numpy.logical_and(numpy.char.greater_equal(array["observation_time"], date_pair[0]), numpy.char.less(array["observation_time"], date_pair[1]))
            array_selection = array[choose]
            if array_selection.shape[0] == 0:
                continue
            
            data_fields = []
            for field_name in field_names:
                data_fields.append(array_selection[field_name])
            field_object.data = data_fields
            date_string = date_pair[0][0:4] + date_pair[0][5:7] + date_pair[0][8:10] +  date_pair[0][11:13]
            out_file_name = "%s.%s.nc" % (out_file, date_string)
            logg.write_info("writing netcdf file %s" % out_file_name)
            files_written.append(out_file_name)
            wxt_cdf.WxtCdf(out_file_name, field_object)
    else:
        fp_out = file(out_file, "w")
        files_written.append(out_file)
        observation_sets.write_ascii(fp_out)
        fp_out.close()

    return files_written
    

    
def main():

    usage_str = "%prog begin_date end_date in_file[s] out_file_base_name\nBegin and end dates should be provided in the form yyyymmdd.\nEnd date is one day beyond the days of interest and its hour should be set to the 0th hour.\nInput files are converted from json format into hourly netcdf files"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-d", "--directory", dest="directory", help="use all files in specified directory as input")
    parser.add_option("-n", "--netcdf_file", dest="cdf", action="store_true", help="write output netcdf file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if (not options.directory and len(args) < 4) or (options.directory and len(args) < 3):
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting()
    begin_date = args[0]
    end_date = args[1]

    in_files = []
    if options.directory:
        # create a list of file paths for all files in directory
        dirlist = os.listdir(options.directory)
        for fname in dirlist:
            path = os.path.join(options.directory, fname)
            in_files.append(path)
    else:            
        in_files = args[2:-1]

    out_file = args[-1]
    
    parse_hourly(begin_date, end_date, in_files, out_file, options.cdf, logg)

    logg.write_ending()    

if __name__ == "__main__":

    main()
   
