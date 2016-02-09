#!/d1/local/src/Python-2.6.2/python
# used to be /usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: make_road_segments.py,v $                                           #
#       Version: $Revision: 1.4 $  Dated: $Date: 2013-11-11 23:30:10 $           #
#                                                                                #
# ============================================================================== #

import httplib
import xml.dom.minidom

import log_msg
import os
import sys
from optparse import OptionParser
import psycopg2 as pg
from osgeo import ogr
from osgeo import osr
from netCDF4 import Dataset
import numpy
import pyproj
import math
import datetime
import re

#cdl = "/d2/vii/data/static/cdl/road_segments.cdl"
GRC_EARTH_RADIUS = 6371
#NV_INTERSTATES = "Interstate 80"
#MN_INTERSTATES = ["Interstate 35", "Interstate 35e", "Interstate 35w", "Interstate 535", "Interstate 90", "Interstate 94", "Interstate 494",  "Interstate 694"]

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


def seg_length(seg, distances):
    t_dist = 0.0
    for a in range(len(seg)-1):
        pt1 = list(seg[a])
        pt2 = list(seg[a+1])

        # The next four lines should be removed
        if pt1[0] > 0:
            pt1[0] *= -1
        if pt2[0] > 0:
            pt2[0] *= -1

        d = dist(pt1[1],pt1[0],pt2[1],pt2[0])
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
    n = line.GetPointCount()
    #print "n: ", n
    seg = []
    # Iterate over the number of points in the polyline
    for i in range(n):
        distances = []
        total_distance = 0
        pt1 = line.GetPoint_2D(i)
        seg.append(pt1)
        d = seg_length(seg, distances)
        # Check to see if the total length of the points so far is greater than the specified segment length
        if d >= seg_len:                     # 1.6 km for 1 mile segments
            print "Total distance of segment in kilometers, in miles: %f, %f" % (d, d*0.6214)
            # If the desired segment length (or greater) has been reached, append the point to the list of segments
            total_distance = d
            #print "total_distance: ", total_distance
            segs.append(seg)
            #print "seg, total_distance: ", seg, total_distance
            set_mid_point(seg, total_distance, distances, mid_point_lats, mid_point_lons)
            seg = [pt1]
    return segs

def set_aux_id(seg_id, state):
    if (len(str(seg_id)) == 1):
        if state == "NV":
            aux_id = "N0000%d" % seg_id
        elif state == "MN":
            aux_id = "M0000%d" % seg_id
        elif state == "MI":
            aux_id = "I0000%d" % seg_id            
        elif state == "CO":
            aux_id = "C0000%d" % seg_id            
        return aux_id;
    elif (len(str(seg_id)) == 2):
        if state == "NV":
            aux_id = "N000%d" % seg_id
        elif state == "MN":
            aux_id = "M000%d" % seg_id
        elif state == "MI":
            aux_id = "I000%d" % seg_id
        elif state == "CO":
            aux_id = "C000%d" % seg_id
        return aux_id;
    elif (len(str(seg_id)) == 3):
        if state == "NV":
            aux_id = "N00%d" % seg_id
        elif state == "MN":
            aux_id = "M00%d" % seg_id
        elif state == "MI":
            aux_id = "I00%d" % seg_id
        elif state == "CO":
            aux_id = "C00%d" % seg_id
        return aux_id;
    elif (len(str(seg_id)) == 4):
        if state == "NV":
            aux_id = "N0%d" % seg_id
        elif state == "MN":
            aux_id = "M0%d" % seg_id
        elif state == "MI":
            aux_id = "I0%d" % seg_id
        elif state == "CO":
            aux_id = "C0%d" % seg_id
        return aux_id;
    elif (len(str(seg_id)) == 5):
        if state == "NV":
            aux_id = "N%d" % seg_id
        elif state == "MN":
            aux_id = "M%d" % seg_id
        elif state == "MI":
            aux_id = "I%d" % seg_id
        elif state == "CO":
            aux_id = "C%d" % seg_id
        return aux_id;
    elif (len(str(seg_id)) >= 6):
        logg.write_error("Too many segment ids. Unable to set aux_id.")
        exit(1);

def main():

    usage_str = " %prog shp_file prj_file seg_len state_abbrev cdl_file nc_out_file\n\tNote: nc_out_file will have the current date in YYYYMMDD format added (e.g. nv_roads.nc will become nv_roads.20130408.nc)"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 6:
        parser.print_help()
        sys.exit(2)

    shp_file = args[0]
    prj_file = args[1]
    seg_len = args[2]
    state = args[3]
    cdl_file = args[4]
    nc_out_file = args[5]

    try:
        # seg_len should be in kilometers
        seg_len = float(seg_len)
    except:
        print "couldn't parse %s as segment length" % seg_len
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    pt_names = []
    lats = []
    lons = []
    mid_point_lats = []
    mid_point_lons = []
    mid_point_latitudes = []
    mid_point_longitudes = []
    elevs = []
    ids = []
    aux_ids = []
    all_names = {}

    # Initialize ogr shape library
    ogr.RegisterAll()

    # Open shape file
    #ds = ogr.Open("/home/vii/mn_road_segments_from_jakin/Current_ALL_plowroutes.shp")
    logg.write_info("open %s" % shp_file)
    ds = ogr.Open(shp_file)

    # Read in the prj file
    #wkt = open("/home/vii/mn_road_segments_from_jakin/Current_ALL_plowroutes.prj").read()
    wkt = open(prj_file).read()

    # Shape file has a concept of layers. There can be multiple layers
    # such as roads, railroads, etc.  Most of the time its one layer
    # and here are the roads Get 1st layer
    root = ds.GetLayerByIndex(0)

    srs = osr.SpatialReference()
    srs.ImportFromESRI([wkt])
        
    proj = osr.SpatialReference()
    proj.ImportFromProj4(srs.ExportToProj4())

    # default projection
    def_proj = osr.SpatialReference()
    def_proj.ImportFromEPSG(4326)
    #seg_id = 1
    seg_id = 0

    logg.write_info("using %.5fkm as minimum segment length" % seg_len)
    logg.write_info("has %d features" % len(root))

    # Loop through all the lines in the layer
    lines = []
    road_names = []

    for i in range(len(root)):
        # Get the ith feature (road segment)
        f = root.GetFeature(i)

        # Just get the geometry of the feature (there can be
        # additional information for the feature such as name, etc.)
        geom = f.GetGeometryRef()

        # Sometimes features have no geometry
        if geom == None:
            continue

        # Assign projection
        geom.AssignSpatialReference(proj)

        # Transform to default projection
        geom.TransformTo(def_proj)

        # Get the name of the route (junction I35 with Chicago say)
        road_name = "unknown"
        for k in range(f.GetFieldCount()):
            field_name = f.GetFieldDefnRef(k).GetName()
            #print "field_name: ", field_name
            field_string = f.GetFieldAsString(k)
            #print "field_string: ", field_string

            # Only pull out Interstate 80 at this time
            if "lname" in field_name.lower():
                lname_string = f.GetFieldAsString(k)
                #print lname_string
                if state == "NV":
                    if re.search(r"^Interstate", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^US Hwy 395", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^US Hwy 395 F", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^US Hwy 50", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^US Hwy 50 A", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^US Hwy 95 A", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 431", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 447", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 427", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 341", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 28", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^State Hwy 88", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^Pyramid Wy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                    if re.search(r"^Pyramid Hwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                elif state == "MN":
                    if re.search(r"^Interstate", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print road_name
                elif state == "MI":
                    if re.search(r"^Interstate", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Chevrolet Buick Frwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Jeffries Frwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Edsel Ford Frwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Chrysler Frwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Fisher Frwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                elif state == "CO":
                    if re.search(r"^US Hwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^Interstate", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                    if re.search(r"^State Hwy", lname_string):
                        road_name = lname_string
                        road_names.append(road_name)
                        #print lname_string
                break
            #if "name" in name.lower():
            #    route = f.GetFieldAsString(k)
            #    break

        # Loop over the points in the geometry
        # A point is still a geometry but here a polyline consists of a number of line segments
        c = geom.GetGeometryCount()
        if c > 0:
            for j in range(c):
                line = geom.GetGeometryRef(j)
                 
                # Only pull out lat/lon for Interstate 80 at this time
                if (road_name != "unknown"):
                    #print "road_name: ", road_name
                    #print "geom: ", line
                    lines.append(line.Clone()) # use clone because GeometryRef will be garbage collected after this loop, causing a se               
                #lines.append(line.Clone()) # use clone because GeometryRef will be garbage collected after this loop, causing a segfault
        else:
            # Only pull out lat/lon for Interstate 80 at this time
            if (road_name != "unknown"):
                #print "road_name: ", road_name
                #print "geom: ", geom.Clone()
                lines.append(geom.Clone())
            
            #lines.append(geom.Clone())

    mid_lat_lon_index = 0
    for index, line in enumerate(lines):
        segs = segmentize(line, seg_len, mid_point_lats, mid_point_lons)
        #print "len(mid_point_lats): ", len(mid_point_lats)
        #print "len(mid_point_lons): ", len(mid_point_lons)
        #print "index: ", index
        #print "len(lines): ", len(lines)
        #print "len(road_names): ", len(road_names)
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
            tmp_aux_id = set_aux_id(seg_id, state)
            aux_id = list(tmp_aux_id.ljust(7,'\0'))
            
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
    
    nc.close()
    logg.write_ending()

if __name__ == "__main__":
   main()
