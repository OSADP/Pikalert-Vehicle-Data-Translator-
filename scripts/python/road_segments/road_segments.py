#!/usr/bin/env python

"""A revision of make_road_segments.py using shapefile"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: road_segments.py,v $                                     #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/07/09 16:24:52 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import math
from netCDF4 import Dataset
import datetime
import re
import shapefile

"""
road_segments.py:175: No global (logg) found
road_segments.py:189: No global (m) found
road_segments.py:191: Function (main) has too many local variables (51)
road_segments.py:204: Local variable (prj_file) not used
road_segments.py:231: Local variable (elevs) not used
road_segments.py:243: No global (shapefile) found
road_segments.py:254: Invalid arguments to (write_error), got 3, expected 1
road_segments.py:379: Variable (seg_id) used before being set
"""


GRC_EARTH_RADIUS = 6371
AUX_ID_LEN = 6

def dist(lat1,lon1,lat2,lon2):

    """Great circle distance"""
    
    rlat1 = math.radians(lat1)
    rlat2 = math.radians(lat2)
    rlon1 = math.radians(lon1)
    rlon2 = math.radians(lon2)
    delta_lat = rlat2 - rlat1;
    delta_lon = rlon2 - rlon1;

    sin_delta_lat2 = math.sin(delta_lat / 2.0)
    sin_delta_lat2_sq = sin_delta_lat2 * sin_delta_lat2
    sin_delta_lon2 = math.sin(delta_lon / 2.0)
    sin_delta_lon2_sq = sin_delta_lon2 * sin_delta_lon2    
    
    temp = sin_delta_lat2_sq + math.cos(rlat1) * math.cos(rlat2) * sin_delta_lon2_sq
    distance = GRC_EARTH_RADIUS * 2 * math.atan2(math.sqrt(temp), math.sqrt(1 - temp))

    return distance


def seg_length(lon_lat_list, distances):
    t_dist = 0.0
    for ind in range(len(lon_lat_list)-1):
        lon1, lat1 = lon_lat_list[ind]
        lon2, lat2 = lon_lat_list[ind+1]

        d = dist(lat1, lon1, lat2, lon2)
        distances.append(d)
        t_dist += d
    return t_dist

def set_mid_point(seg, total_distance, distances, mid_point_lats, mid_point_lons):
    half_total_distance = total_distance / 2.0
    #print "half_total_distance: ", half_total_distance
    partial_distance = 0
    previous_partial_distance = 0;
    mid_index = 0
    #print "distances: ", distances
    #print "seg: ", seg
    for b in range(len(seg)-1):
        #print "b: ", b
        previous_partial_distance = partial_distance
        partial_distance += distances[b]
        #print "partial_distance, half_total_distance: %f, %f" % (partial_distance, half_total_distance)
        #print "partial_distance: ", partial_distance
        #print "previous_partial_distance: ", previous_partial_distance
        if (partial_distance >= half_total_distance):
            #print "partial_distance: ", partial_distance
            if (half_total_distance - previous_partial_distance) < (partial_distance - half_total_distance):
                mid_index = b
            else:
                mid_index = b+1
            break;
    #print "mid_point_index: ", mid_index
    #print "mid_point_lat: ", seg[mid_index][1]
    #print
    mid_point_lats.append(seg[mid_index][1])    
    mid_point_lons.append(seg[mid_index][0])    
    #print "mid_point_lat: ", seg[mid_index][1]
    #print "mid_point_lon: ", seg[mid_index][0]
    
            

def segmentize(line, seg_len, mid_point_lats, mid_point_lons):
    """Pull out the points in the polyline. Note that in the GIS world a polyline is defined as a set of lines that may not be connected"""
    segs = []
    if len(line) < 2:
        return segs

    
    # Iterate over the number of points in the polyline
    seg = []
    for i in range(0, len(line)):
        distances = []
        seg.append(line[i])
        
        total_distance = seg_length(seg, distances)
        # Check to see if the total length of the points so far is greater than the specified segment length
        if total_distance >= seg_len:                     
            print "Total distance of segment in kilometers, in miles: %f, %f" % (total_distance , total_distance*0.6214)
            # If the desired segment length (or greater) has been reached, append the point to the list of segments
            segs.append(seg)
            #print "seg, total_distance: ", seg, total_distance
            set_mid_point(seg, total_distance, distances, mid_point_lats, mid_point_lons)
            seg = [line[i]]

    #Handle case when the sum distance of all lines in seg are < seg_len
    if (len(segs) == 0 or segs[-1] != seg) and total_distance >= 0.25*seg_len:
        segs.append(seg)
        set_mid_point(seg, total_distance, distances, mid_point_lats, mid_point_lons)
        
    return segs


def set_aux_id(seg_id, state, logg):
    """
    >>> set_aux_id(1, "MI", log_msg.LogMessage(""))
    'MI00000000000001'
    >>> set_aux_id(99999999999999, "NV", log_msg.LogMessage(""))
    'NV99999999999999'
    >>> set_aux_id(999999999999999, "MN", log_msg.LogMessage(""))
    Traceback (most recent call last):
      ...
      File "/usr/local/python/lib/python2.7/site.py", line 372, in __call__
        raise SystemExit(code)
    SystemExit: 1
    >>> set_aux_id(256, "ALASKA", log_msg.LogMessage(""))
    'ALASKA0000000256'
    """
    seglen = len(str(seg_id))

    if seglen > (AUX_ID_LEN-1):
        logg.write_error("Too many segment ids. Unable to set aux_id.")
        exit(1);
    
    zeros = "".join(map(str, [0]*(AUX_ID_LEN-seglen-1)))
    aux_id = "%s%s%d" % (str(state[0]), zeros, seg_id)
    return aux_id


def find_segment_name(state, record):
    # Get the name of the route (junction I35 with Chicago say)
    road_name = "unknown"

    for item in record:
        lname_string = item
        if type(lname_string) == type(""):
            if state == "NV":
                if re.search(r"^Interstate", lname_string):
                    road_name = lname_string
                if re.search(r"^US Hwy 395", lname_string):
                    road_name = lname_string
                if re.search(r"^US Hwy 395 F", lname_string):
                    road_name = lname_string
                if re.search(r"^US Hwy 50", lname_string):
                    road_name = lname_string
                if re.search(r"^US Hwy 50 A", lname_string):
                    road_name = lname_string
                if re.search(r"^US Hwy 95 A", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 431", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 447", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 427", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 341", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 28", lname_string):
                    road_name = lname_string
                if re.search(r"^State Hwy 88", lname_string):
                    road_name = lname_string
                if re.search(r"^Pyramid Wy", lname_string):
                    road_name = lname_string
                if re.search(r"^Pyramid Hwy", lname_string):
                    road_name = lname_string
            elif state == "MN":
                if re.search(r"^Interstate", lname_string):
                    road_name = lname_string
                    #print road_name
            elif state == "MI":
                if re.search(r"^Interstate", lname_string):
                    road_name = lname_string
                    #print lname_string
                if re.search(r"^Chevrolet Buick Frwy", lname_string):
                    road_name = lname_string
                    #print lname_string
                if re.search(r"^Jeffries Frwy", lname_string):
                    road_name = lname_string
                    #print lname_string
                if re.search(r"^Edsel Ford Frwy", lname_string):
                    road_name = lname_string
                    #print lname_string
                if re.search(r"^Chrysler Frwy", lname_string):
                    road_name = lname_string
                    #print lname_string
                if re.search(r"^Fisher Frwy", lname_string):
                    road_name = lname_string
                    #print lname_string
            elif state == "C":
		#Allow all the roads
                road_name = lname_string
                road_name = road_name.replace('\r\n',' ')
                #print lname_string

            if road_name != "unknown":
                break

    return road_name

            
def main():

    usage_str = " %prog shp_file seg_len state_abbrev cdl_file nc_out_file\n\tNote: nc_out_file will have the current date in YYYYMMDD format added (e.g. nv_roads.nc will become nv_roads.20130408.nc)"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-n", "--nameid", dest="nameid", help="Specifies a field ordinal to extract as the segment name. " \
                          "The file should be pre-filtered and all records in the file will be utilized.") 
    parser.add_option("-t", "--test", dest="test", help="Executes tests.", action="store_true")

    (options, args) = parser.parse_args()

    if options.test:
        import doctest
        doctest.testmod()
        sys.exit(1)

    if len(args) < 5:
        parser.print_help()
        sys.exit(2)

    shp_file = args[0]
    seg_len = args[1]
    state = args[2]
    cdl_file = args[3]
    nc_out_file = args[4]

    try:
        # seg_len should be in kilometers
        seg_len = float(seg_len)
    except:
        print "couldn't parse %s as segment length" % seg_len
        sys.exit(2)

    logg = None
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    nameid = -1
    if options.nameid:
        nameid = int(options.nameid)
        
    logg.write_starting()

    pt_names = []
    lats = []
    lons = []
    mid_point_lats = []
    mid_point_lons = []
    mid_point_latitudes = []
    mid_point_longitudes = []
    ids = []
    aux_ids = []
    all_names = {}

    lines = []
    road_names = []
    seg_id = 0

    # Open shape file
    logg.write_info("opening shape file %s" % shp_file)
    sf = shapefile.Reader(shp_file)

    shapes = sf.shapes()
    for shape in shapes:
        if shape.shapeType == shapefile.POLYLINE:
            #print dir(shape)
            #print shape.points
            pass
    records = sf.records()
    #for record in records:
    #    print record
    if len(shapes) != len(records):
        logg.write_error("length shapes != length records %d != %d\n", len(shapes), len(records))
        logg.write_ending(1)
        sys.exit(1)


    logg.write_info("iterating through shapes and records")
    for ind in range(len(shapes)):

        shape = shapes[ind]
        if shape.shapeType in (shapefile.POLYLINE, shapefile.POLYLINEZ):

            # Get the name of the route (junction I35 with Chicago say)
            road_name = records[ind][nameid] if nameid >= 0 else find_segment_name(state, records[ind])

            if road_name != "unknown":
                print "appending line for road: ", road_name
                lines.append(shape.points) # use clone because GeometryRef will be garbage collected after this loop, causing a se               
                road_names.append(road_name)
                
                
    mid_lat_lon_index = 0

    for index, line in enumerate(lines):
        print line
        segs = segmentize(line, seg_len, mid_point_lats, mid_point_lons)
        print "len(mid_point_lats): ", len(mid_point_lats)
        print "len(mid_point_lons): ", len(mid_point_lons)
        print "index: ", index
        print "len(lines): ", len(lines)
        print "len(road_names): ", len(road_names)
        tmp_aux_id = ""
        for ind, seg in enumerate(segs):
            #print "seg: ", seg
            #print "len(seg): ", len(seg)
            #print "ind: ", ind
            if not all_names.has_key(road_names[index]):
                name = road_names[index]
                all_names[road_names[index]] = 1
                name = "%s %d" % (road_names[index], all_names[road_names[index]])
                seg_id += 1
            else:
                name = "%s %d" % (road_names[index], all_names[road_names[index]]+1)
                all_names[road_names[index]] += 1
                seg_id += 1
            
            name = list(name.ljust(42,'\0'))
            if len(name) > 42: name = name[0:42]
            tmp_aux_id = set_aux_id(seg_id, state, logg)
            aux_id = list(tmp_aux_id.ljust(AUX_ID_LEN,'\0'))
            
            for (lon,lat) in seg:
                lats.append(lat)
                lons.append(lon)
                pt_names.append(name)
                #print "name: ", name
                ids.append(seg_id)
                #print "mid_lat_lon_index: ", mid_lat_lon_index
                mid_point_latitudes.append(mid_point_lats[mid_lat_lon_index])
                #print "mid_point_lats[%d]: %f" % (mid_lat_lon_index, mid_point_lats[mid_lat_lon_index])
                mid_point_longitudes.append(mid_point_lons[mid_lat_lon_index])
                #print "mid_point_lons[%d]: %f" % (mid_lat_lon_index, mid_point_lons[mid_lat_lon_index])
                aux_ids.append(aux_id)
            mid_lat_lon_index += 1


    print aux_ids
    # Now have a list of different information for the line in
    # lats, lons, pt_names, ids

    # Generate output file with lats, lons and segment ids
    out_file_split = nc_out_file.split(".")
    tmp_out_file_split = out_file_split[:-1]
    now = datetime.datetime.now()
    file_date = now.strftime("%Y%m%d")
    dated_nc_out_file = '.'.join(tmp_out_file_split)
    dated_nc_out_file = dated_nc_out_file + "." + str(file_date) + "." + out_file_split[-1]
    cmd = "/usr/local/netcdf/bin/ncgen -k 3 -o %s %s" % (dated_nc_out_file, cdl_file)
    logg.write_info(cmd)
    os.system(cmd)
    nc = Dataset(dated_nc_out_file,"a")
    nc.variables["latitude"][:] = lats
    nc.variables["longitude"][:] = lons
    nc.variables["mid_point_latitude"][:] = mid_point_latitudes
    nc.variables["mid_point_longitude"][:] = mid_point_longitudes
    nc.variables["seg_name"][:] = pt_names
    nc.variables["aux_id"][:] = aux_ids
    nc.variables["seg_id"][:] = ids
    nc.variables["point_type"][:] = [0]*len(ids)
    
    nc.close()
    logg.write_ending()
    
if __name__ == "__main__":

   main()
