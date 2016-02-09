#!/usr/bin/env python

"""Add dicast site ids to road segment file"""

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
import shutil
import sys
from optparse import OptionParser
from netCDF4 import Dataset

def add_dicast_ids(road_segment_nc_file, dicast_site_file, out_file):
    r"""Add dicast ids to road segment nc file

    Parameters
    ----------
    road_segment_nc_file : string
    dicast_site_file : string
    out_file : string
    
    Returns
    -------
    0 : success
    1 : error
    """

    # copy road_segment_nc_file to out_file
    try:
        shutil.copyfile(road_segment_nc_file, out_file)
    except:
        return 1
    
    # open out_file for updating
    netcdf_file = Dataset(out_file,"a")
    
    # extract aux_id values
    aux_id = netcdf_file.variables["aux_id"]
    aux_id_string = []
    for ind in range(aux_id.shape[0]):
        aux_id_string.append(aux_id[ind].tostring().rstrip('\x00'))


    # create map from aux_id to dicast numbers
    dicast_dict = {}
    for line in open(dicast_site_file):
        spl = line.strip().split(";")
        if spl[0] == "site_num":
            # skip csv header
            continue
        (site_num, n, site_id, lat, lon, elev, n, desc, state, co, site_type) = spl
        dicast_dict[site_id] = site_num

    # find dicast ids for aux_id values
    dicast_ids = []
    for aux_string in aux_id_string:
        dicast_ids.append(dicast_dict[aux_string])
        
    netcdf_file.createDimension('dicast_id_len', size=8)
    dicast_ids_var = netcdf_file.createVariable('dicast_id', 'c', ('point_num', 'dicast_id_len'))
    dicast_ids_var[:] = dicast_ids
    netcdf_file.close()
    
    # open out_file and append dicast id values
    
    return 0

def main():

    #/d2/vii/data/static/config/CO_logicast_road_sites_extended.asc
    usage_str = "%prog road_segment_file dicast_file out_file"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    road_segment_file = args[0]
    dicast_file = args[1]
    out_file = args[2]

    add_dicast_ids(road_segment_file, dicast_file, out_file)

if __name__ == "__main__":

   main()
