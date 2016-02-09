#!/usr/bin/env python

import os
import sys
from optparse import OptionParser
from netCDF4 import Dataset       
import json


class RoadSegment(object):

    def __init__(self, segment_id, aux_id, dicast_id, center_lat, center_lon, lat, lon):
        self.segment_id =  segment_id
        self.aux_id = aux_id
        self.dicast_id = dicast_id
        self.center_lat = center_lat
        self.center_lon = center_lon
        self.lat_lon_point_list = [(lat, lon)]
        return

    def add_lat_lon(self, lat, lon):
        lat_lon_point = (lat, lon)
        self.lat_lon_point_list.append(lat_lon_point)
        return

class RoadSegmentReader(object):
        
    def __init__(self, netcdf_file):
        
        netcdf_file = Dataset(netcdf_file,"r")
        self.road_segment_list = []
        
        self.road_segment_id_map = {}
        
        length_dataset = len(netcdf_file.variables["longitude"])
        
        for x in xrange(0, length_dataset):
             
            seg_id = str(netcdf_file.variables["seg_id"][x])
            aux_id = netcdf_file.variables["aux_id"][x].tostring()
            dicast_id = netcdf_file.variables["dicast_id"][x].tostring()

            center_lat =  netcdf_file.variables["mid_point_latitude"][x]
            center_lon =  netcdf_file.variables["mid_point_longitude"][x]
            lat = netcdf_file.variables["latitude"][x]
            lon = netcdf_file.variables["longitude"][x]
            road_segment_value = self.road_segment_id_map.get(seg_id, None)
            if road_segment_value == None:
                road_segment = RoadSegment(seg_id, aux_id, dicast_id, center_lat, center_lon, lat, lon)
                self.road_segment_id_map[seg_id] = road_segment
            else:
                road_segment_value.add_lat_lon(lat, lon)

        self.road_segment_list = self.road_segment_id_map.values()
        return

    def store_road_segment_dict(self, output_dict):

        segments = []
        for road_segment in self.road_segment_list:

            # format road segment dict output for json recepient
            coordinates = []
            for point in road_segment.lat_lon_point_list:
                coordinates.append({"lat" : point[0], "lon" : point[1]})

            road_segment_dict = {"segment_id" : road_segment.segment_id, "dicast_id" : road_segment.dicast_id, "center_lat" : road_segment.center_lat, "center_lon" : road_segment.center_lon, "coordinates" : coordinates}
            segments.append(road_segment_dict)

        output_dict["segments"] = segments 
        return
    
    def write_road_segment_json(self, output_dict, output_file):

        output_fp = open(output_file, "w")
        output_string = json.dumps(output_dict, indent=4)
        output_fp.write(output_string)
        output_fp.close()
        

def main():

    usage_str = "%prog in_file out_file"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    in_file = args[0]
    out_file = args[1]

    road_segment_reader = RoadSegmentReader(in_file)
    output_dict = {}
    road_segment_reader.store_road_segment_dict(output_dict)
    road_segment_reader.write_road_segment_json(output_dict, out_file)


if __name__ == "__main__":

   main()
