#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: add_rwis_road_seg.py,v $                                 #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/07/09 16:24:52 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
from netCDF4 import Dataset
import numpy as np
import datetime

METERS_TO_FEET = 3.2808
ROAD_SEG_POINT_TYPE_ID = 0
RWIS_POINT_TYPE_ID = 1
SEG_NAME_LEN = 42
AUX_ID_LEN = 6

def meters_to_feet(meter_value):

    ft_value = float(meter_value) * METERS_TO_FEET
    
    return float(ft_value)


def main():

    usage_str = """%prog road_seg_file_path rwis_file_path out_cdl_file

    road_seg_file_path	path to a netCDF file
    rwis_file_path	path to a csv file
    out_cdl_file	path to a template cdl"""

    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    road_seg_file_path = args[0]
    rwis_file_path = args[1]
    out_cdl_file = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("add_rwis_road_seg.py")

    # Read the road segment file
    logg.write_info("Reading %s" % road_seg_file_path) 
    nc = Dataset(road_seg_file_path,"r")

    # Store variable data
    lat_var = nc.variables["latitude"]
    mid_pt_lat_var = nc.variables["mid_point_latitude"]
    lon_var = nc.variables["longitude"]
    mid_pt_lon_var = nc.variables["mid_point_longitude"]
    elev_var = nc.variables["elevation"]
    seg_name_var = nc.variables["seg_name"]
    seg_id_var = nc.variables["seg_id"]
    aux_id_var = nc.variables["aux_id"]
    rwfs_id_var = nc.variables["rwfs_id"]
    road_type_var = nc.variables["road_type"]
    route_class_var = nc.variables["route_class"]
    highway_type_var = nc.variables["highway_type"]
    speed_mph_var = nc.variables["speed_mph"]
    
    # Create the point_type value for the road segments
    point_type_list = []
    for i in range(len(lat_var[:])):
        point_type_list.append(ROAD_SEG_POINT_TYPE_ID)
    point_type_var = np.array(point_type_list)
        
    # Get the last value of the seg_id in the road segment file so
    # we can append the appropriate seg_id for the rwis site
    rwis_site_id =  seg_id_var[-1]

    # Read in RWIS site info
    #dicast-id ; wmo-id ; icao-id (aux-id) ; latitude ; longitude ; elevation (meters) ; region-number ; station-name ; state ; country
    rwis_seg_id = []
    rwis_aux_id = []
    rwis_lat = []
    rwis_mid_pt_lat = []
    rwis_lon = []
    rwis_mid_pt_lon = []
    rwis_elev = []
    rwis_point_type = []
    rwis_seg_name = []
    data = open(rwis_file_path, "r").readlines()
    for line in data:
        row = line.split(";");
        for index, value in enumerate(row):
            if (index ==  2):
                tmp_list = []
                for val in value:
                    tmp_list.append(val)
                for i in range(len(tmp_list), AUX_ID_LEN):
                    tmp_list.append('\0')
                rwis_aux_id.append(np.array(tmp_list))
            elif (index == 3):
                rwis_lat.append(float(value))
                rwis_mid_pt_lat.append(float(value))
            elif (index == 4):
                rwis_lon.append(float(value))
                rwis_mid_pt_lon.append(float(value))
            elif (index == 5):
                rwis_elev.append(meters_to_feet(value))
            elif (index == 7):
                tmp_list = []
                for val in value:
                    tmp_list.append(val)
                #Replace end of string with space and aux_id to ensure uiqueness
                append_cnt = 0
                while len(tmp_list) < SEG_NAME_LEN and append_cnt < (AUX_ID_LEN+1):
                    tmp_list.append(' ')
                    append_cnt += 1

                tmp_list[-(AUX_ID_LEN+1):] = [' '] + rwis_aux_id[-1].tolist()
                for i in range(len(tmp_list), SEG_NAME_LEN):
                    tmp_list.append('\0')
                
                rwis_seg_name.append(np.array(tmp_list))
        rwis_site_id = rwis_site_id + 1
        rwis_seg_id.append(rwis_site_id)
        rwis_point_type.append(RWIS_POINT_TYPE_ID)

    # Create numpy arrays of the rwis data
    np_rwis_seg_id = np.array(rwis_seg_id)
    np_rwis_aux_id = np.array(rwis_aux_id)
    np_rwis_lat = np.array(rwis_lat)
    np_rwis_mid_pt_lat = np.array(rwis_mid_pt_lat)
    np_rwis_lon = np.array(rwis_lon)
    np_rwis_mid_pt_lon = np.array(rwis_mid_pt_lon)
    np_rwis_elev = np.array(rwis_elev)
    np_rwis_point_type = np.array(rwis_point_type)
    np_rwis_seg_name = np.array(rwis_seg_name)

    # Combine road_seg and RWIS data
    new_array_size =  len(lat_var[:]) + len(np_rwis_lat)

    seg_id = np.zeros(new_array_size)
    aux_id = np.zeros(new_array_size)
    lat = np.zeros(new_array_size)
    mid_pt_lat = np.zeros(new_array_size)
    lon = np.zeros(new_array_size)
    mid_pt_lon = np.zeros(new_array_size)
    elev = np.zeros(new_array_size)
    point_type = np.zeros(new_array_size)
    seg_name = np.zeros(new_array_size)

    """
    # For debugging
    print "len(seg_id_var[:]):", len(seg_id_var[:])
    print type(seg_id_var[:])
    print seg_id_var[:].shape
    print "len(np_rwis_seg_id):", len(np_rwis_seg_id)
    print type(np_rwis_seg_id)
    print np_rwis_seg_id.shape
    print "len(aux_id_var[:]):", len(aux_id_var[:])
    print type(aux_id_var[:])
    print aux_id_var[:].shape
    print "len(np_rwis_aux_id):", len(np_rwis_aux_id)
    print type(np_rwis_aux_id)
    print np_rwis_aux_id.shape
    print "len(seg_name_var[:]):", len(seg_name_var[:])
    print type(seg_name_var[:])
    print seg_name_var[:].shape
    print "len(np_rwis_seg_name):", len(np_rwis_seg_name)
    print type(np_rwis_seg_name)
    print np_rwis_seg_name.shape
    """
    
    seg_id = np.concatenate([seg_id_var[:], np_rwis_seg_id])
    aux_id = np.concatenate([aux_id_var[:], np_rwis_aux_id])
    lat = np.concatenate([lat_var[:], np_rwis_lat])
    mid_lat = np.concatenate([mid_pt_lat_var[:], np_rwis_mid_pt_lat])
    lon = np.concatenate([lon_var[:], np_rwis_lon])
    mid_lon = np.concatenate([mid_pt_lon_var[:], np_rwis_mid_pt_lon])
    elev = np.concatenate([elev_var[:], np_rwis_elev])
    point_type = np.concatenate([point_type_var, np_rwis_point_type])
    print np_rwis_seg_name.shape
    print len(seg_name_var)
    seg_name = np.concatenate([seg_name_var[:], np_rwis_seg_name])

    # Write output file
    (output_file_path, basename) = os.path.split(road_seg_file_path)
    split_basename = basename.split(".")
    now = datetime.datetime.now()
    file_date = now.strftime("%Y%m%d")
    dated_nc_out_file = split_basename[0] + "." + file_date + "." + split_basename[-1]
    dated_nc_out_file_path = os.path.join(output_file_path, dated_nc_out_file)

    cmd = "ncgen -o %s %s" % (dated_nc_out_file_path, out_cdl_file)
    ret = os.system(cmd)
    if ret != 0:
        return -1

    nc_out = Dataset(dated_nc_out_file_path, "a")

    nc_out.variables["latitude"][:] = lat
    nc_out.variables["mid_point_latitude"][:] = mid_lat
    nc_out.variables["longitude"][:] = lon
    nc_out.variables["mid_point_longitude"][:] = mid_lon
    nc_out.variables["elevation"][:]= elev
    nc_out.variables["seg_name"][:] = seg_name
    nc_out.variables["seg_id"][:] = seg_id
    nc_out.variables["aux_id"][:] = aux_id
    nc_out.variables["point_type"][:] = point_type

    nc.close()
    nc_out.close()
    
    logg.write_ending(0, "add_rwis_road_seg.py")

if __name__ == "__main__":

   main()
