#!/usr/bin/env python

""""""

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
import os
import sys
from optparse import OptionParser
from netCDF4 import Dataset
import json
from matplotlib import path
from road_segment_reader import RoadSegmentReader, RoadSegment
import processed_file
import shapefile
import FileDownloader
import tim
import time

NOAA_URL = "http://www.nws.noaa.gov/regsci/gis/shapefiles/current_warnings.zip"

class WatchWarningFileReader(object):

    def __init__(self, shape_file_directory):

        self.watch_warning_list = []
        time_string = time.strftime("%Y%m%d", time.gmtime(time.time()))
        for subdir, dirs, files in os.walk(shape_file_directory):
            
            for sf in files:
                
                if not ".shp" in sf:  # only give the .shp files to the reader, it will find the other types
                    continue
                
                elif not all(x in files for x in ["%s.shx" % sf[:3], "%s.dbf" % sf[:3]]):   # shapefile.Reader requires .dbf and .shx files to exist
                    continue

                elif os.path.getsize("%s/%s" % (shape_file_directory,sf)) <= 100:

                    continue
                    
                shape_file = shapefile.Reader(os.path.join(shape_file_directory, sf))
                
                polygon_list = shape_file.shapes() # has bounding box, polygon_record_list doesn't 
             
                polygon_record_list = shape_file.shapeRecords() # has watch/warning information, polygon_list doesn't
                
                for p in xrange(0, len(polygon_list)):
                    point_collection = make_point_collection(polygon_record_list[p].shape.points)
                    begin_time = polygon_record_list[p].record[0].strip()
                    end_time = polygon_record_list[p].record[1].strip()
                    begin_time_month = int(begin_time[0:2])
                    end_time_month = int(end_time[0:2])
                    time_string_month = int(time_string[4:6])

                    if begin_time_month == time_string_month or begin_time_month == (time_string_month - 1):
                        begin_time = time_string[0:4] + begin_time
                    elif begin_time_month == 12 and time_string_month == 1:
                        year = int(time_string[0:4] - 1)
                        begin_time = str(year) + begin_time
                    else:
                        continue

                    if end_time_month == time_string_month or end_time_month == (time_string_month - 1):
                        end_time = time_string[0:4] + end_time
                    elif end_time_month == 12 and time_string_month == 1:
                        year = int(time_string[0:4] - 1)
                        end_time = str(year) + end_time
                    else:
                        continue
                    
                    self.watch_warning_list.append( WatchWarning( point_collection, polygon_record_list[p].record[5], polygon_list[p].bbox, sf[:3], begin_time, end_time))
                    


class WatchWarning(object):

    def __init__(self, point_collection, watch_warning_info, bounding_box, file_type, begin_time, end_time):

        self.polygon = point_collection
        self.watch_warning_info = watch_warning_info
        self.type = file_type
        self.polygon_bounding_box = BoundingBox(bounding_box[1],bounding_box[3],bounding_box[0],bounding_box[2])
        self.begin_time = begin_time
        self.end_time = end_time

        
        
    def road_segment_impact(self, road_segment_box, road_segment_list):
        
        impacted_road_segment_ids = []
        
        """
        
        Parameters
        ----------
        road_segment_bounding_box : bounding box for road segment
        road_segment_list : list of road segment objects        

        Returns
        -------

        list of road segment ids impacted by polygon and the  watch_warning_info
        
        """
        
        # a road segment is an object consisting of a  point collection and an id
        
        if self.polygon_bounding_box.lat_min < road_segment_box.lat_max and self.polygon_bounding_box.lon_max > road_segment_box.lon_min:
            
                # then the boundings boxes overlap, they might contain the same road segments
                for seg in road_segment_list:
                        
                        for road_segment_point in seg.lat_lon_point_list:
                            polygon_vertices = path.Path(reshape_polygon(self.polygon))
                            
                            if polygon_vertices.contains_point(road_segment_point):
                                impacted_road_segment_ids.append(seg.segment_id)                                   
        
                                
                impacted_road_seg_ids = list(set(impacted_road_segment_ids))
                
                if len(impacted_road_seg_ids) == 0:
                        
                        return (None, None)  # if the bounding boxes intersected, but no road segments intersected the polygon 
                else:
                        return (impacted_road_seg_ids, self.watch_warning_info)
        else:
            return (None, None)


class WatchWarningImpact(object):
    
    def __init__(self, watch_warning_list, road_segment_reader):
        
        self.watch_warning_list = watch_warning_list
        self.road_segment_reader = road_segment_reader
        self.road_segment_list = road_segment_reader.road_segment_list
        self.road_segment_box = get_road_segments_bounding_box(self.road_segment_list)
        
    def store_polygons(self, output_dict):
        
        hazard_array = []
        for watch_warning in self.watch_warning_list:

            polygon_points = undo_make_point_collection(watch_warning.polygon) #### re-formats the polygon to look nicer
            
            hazard = {}
            hazard['Type'] = watch_warning.type
            hazard['Weather Watch/Warning Text'] = watch_warning.watch_warning_info
            hazard['Begin Time'] = watch_warning.begin_time
            hazard['End Time'] = watch_warning.end_time
            hazard['coordinates'] = polygon_points

            hazard_array.append(hazard)
           
        output_dict["hazards"] = hazard_array

        
    def store_road_segment_impact(self, output_dict):
        
        segments = []
        for watch_warning in self.watch_warning_list:
            impacted_road_segment_ids , watch_warning_statement = watch_warning.road_segment_impact(self.road_segment_box, self.road_segment_list)
            if impacted_road_segment_ids != None and watch_warning_statement != None:
                for seg_id in impacted_road_segment_ids:
                    # get road segment object
                    road_segment = self.road_segment_reader.road_segment_id_map[seg_id]

                    # format road segment dict output for json recepient
                    coordinates = []
                    for point in road_segment.lat_lon_point_list:
                        coordinates.append({"lat" : point[0], "lon" : point[1]})
                    road_segment_dict = {"segment_id" : road_segment.segment_id, "dicast_id" : road_segment.dicast_id, "center_lat" : road_segment.center_lat, "center_lon" : road_segment.center_lon, "coordinates" : coordinates}
                    road_segment_dict["Type"] = watch_warning.type
                    road_segment_dict["Begin Time"] = watch_warning.begin_time
                    road_segment_dict["End Time"] = watch_warning.end_time
                    segments.append(road_segment_dict)

        output_dict["segments"] = segments

class BoundingBox(object):

    def __init__(self, lat_min, lat_max, lon_min, lon_max):

        self.lat_min = lat_min
        self.lat_max = lat_max
        self.lon_min = lon_min
        self.lon_max = lon_max


class PointCollection(object):

    def __init__(self, latitudes, longitudes):
        
        self.latitudes = latitudes
        self.longitudes = longitudes

    
def undo_make_point_collection(point_collection):

    """ re-formats the point collection so it will look better in the json file"""
    
    lon_lat_pairs = []
    
    for ind in xrange(0, len(point_collection.latitudes)):
        lon_lat_dict = {"lon" : point_collection.longitudes[ind], "lat" : point_collection.latitudes[ind]}
        lon_lat_pairs.append(lon_lat_dict)
    
    return tuple(lon_lat_pairs)

        
def make_point_collection(shapefile_points):

    """ re-organizes the point collection given by the shapefile to a format that the PointCollection class can read"""
    
    lat_lon_list = [[],[]]
    
    for x in xrange(0, len(shapefile_points)):
        lat_lon_list[0].append(shapefile_points[x][1])
        lat_lon_list[1].append(shapefile_points[x][0])

    return PointCollection(lat_lon_list[0],lat_lon_list[1])
    

def get_road_segments_bounding_box(road_segment_list):

    latitudes = []
    longitudes = []

    for road_segment in road_segment_list:
        for road_segment_point in road_segment.lat_lon_point_list:
            latitudes.append(road_segment_point[0])
            longitudes.append(road_segment_point[1])

    return BoundingBox(min(latitudes),max(latitudes),min(longitudes),max(latitudes))


def reshape_polygon(polygon):

    """ the matplotlib path module requires the polygon to be in a certain format to use the .contains_point method, this function
    puts the polygon in that format"""
    
    latitudes = polygon.latitudes
    longitudes = polygon.longitudes
    polygon_vertices = []

    for x in xrange(0,len(latitudes)):
        polygon_vertices.append([latitudes[x], longitudes[x]])

    return polygon_vertices


def download_current_warnings(shapefile_output_directory, logg):

    logg.write_info("calling FileDownloader")
    url = NOAA_URL
    file_downloader = FileDownloader.FileDownloader(url, shapefile_output_directory, "current_warnings.zip", True)
    if file_downloader.file_downloaded():
        logg.write_info("downloaded successfully: %s" % os.path.join(shapefile_output_directory, "current_warnings.zip"))
    else:
        logg.write_error("current_warnings.zip not downloaded successfully")
        return None
        
    return file_downloader

def gen_watch_warning(time_string, road_file, shapefile_output_directory, output_directory, logg):

    if not os.path.exists(output_directory):
        os.mkdir(output_directory)
    
    day_string = time_string[0:-6]
    hhmm = time_string[-6:-2]
    
    logg.write_info("calling WatchWarningFileReader")        
    impacted_polygon_list = WatchWarningFileReader(shapefile_output_directory).watch_warning_list

    logg.write_info("calling RoadSegmentReader on file %s" % road_file)
    road_segment_reader = RoadSegmentReader(road_file)
    road_segment_array = road_segment_reader.road_segment_list

    logg.write_info("calling WatchWarningImpact")            
    watch_warning_impact = WatchWarningImpact(impacted_polygon_list, road_segment_reader)

    json_output_directory = os.path.join(output_directory, day_string)
    if not os.path.exists(json_output_directory):
        os.mkdir(json_output_directory)
    
    impacted_polygon_file = os.path.join(json_output_directory, "impacted_polygons.%s.%s.json"  %  (day_string, hhmm))

    logg.write_info("writing json impact file: %s" % impacted_polygon_file)
    output_dict = {}
    watch_warning_impact.store_polygons(output_dict)
    watch_warning_impact.store_road_segment_impact(output_dict)

    output_file = open(impacted_polygon_file, "w")
    output_string = json.dumps(output_dict, indent=4)
    output_file.write(output_string)
    output_file.close()

    return impacted_polygon_file
    
def main():

    usage_str = "%prog config_file state_list consisting of at least one of [co, mi, mn, nv]. Note that backend_sys_path.py contains state road segment file information and shapefile location information"

    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-r", "--ceil_seconds", dest="ceil_seconds", help="round the time value upward to the nearest integral multiple of ceil_seconds")
    parser.add_option("-t", "--specify_time", dest="time_string", help="specify the time string instead of using current time")
    
    (options, args) = parser.parse_args()
     
    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    states = args[1:]

    ceil_secs = 300
    if options.ceil_seconds:
        ceil_secs = int(options.ceil_seconds)
    
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])
    shape_file_directory = cf.Shape_file_directory
                
    if not os.path.exists(shape_file_directory):
        os.mkdir(shape_file_directory)
   
    logfile = ""
    if options.log:
        logfile = options.log
        
    logg = log_msg.LogMessage(logfile)
    logg.set_suffix(".pyl")

    logg.write_starting("weather_watch_warning.py")
        
    if options.time_string:
        ceiling_time = options.time_string
    else:    
        current_time = time.time()
        ceiling_time = tim.ceil_seconds(current_time, ceil_secs)

    time_string = time.strftime("%Y%m%d%H%M%S", time.gmtime(ceiling_time))
    logg.write_info("current_time %ld, ceiling_time %ld, time_string %s" % (current_time, ceiling_time, time_string))
    minute_string = time_string[0:-2]
    shapefile_output_directory = os.path.join(shape_file_directory, minute_string)
    download_current_warnings(shapefile_output_directory, logg)
    logg.write_info("using shapefile_output_directory %s" % shapefile_output_directory)
    
    for state in states:
        if state in cf.State_weather_watch_directory:
            state_output_directory = cf.State_weather_watch_directory[state]
            state_road_file = cf.State_road_files[state]
            impacted_polygon_file = gen_watch_warning(time_string, state_road_file, shapefile_output_directory, state_output_directory, logg)
            processed_fname_base = "Processed" 
            proc_file = processed_file.ProcessedFile(state_output_directory, processed_fname_base)
            proc_file.write_processed(time_string[0:8], os.path.basename(impacted_polygon_file), processed_file.PROC_SUCCESS)
        else:
            logg.write_error("state %s is not in configuration file %s" % (state, config_file))

    logg.write_ending()


if __name__ == "__main__":

    main()
