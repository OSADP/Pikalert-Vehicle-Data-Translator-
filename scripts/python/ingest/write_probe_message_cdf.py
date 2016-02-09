#!/usr/bin/env python

"""Create netcdf output probe message file using cdl and data_object container"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: write_probe_message_cdf.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/06/18 20:23:23 $           #
#                                                                                #
# ============================================================================== #

import os
import sys
from optparse import OptionParser
from netCDF4 import Dataset
import simulation_reader

def set_variable(array, var):
    if var != None:
        array[:] = var
    return

def write_probe_message(cdl, data_object, out_file, mapping_dict):

    if not os.path.exists(out_file):
        cmd = "ncgen -o %s %s" % (out_file, cdl)
        ret = os.system(cmd)
        if ret != 0:
            return -1
        
    nc = Dataset(out_file, "a")
    for key in mapping_dict:
        set_variable(nc.variables[key], data_object.get_field(key))

    nc.close()
    return 0

def main():

    usage_str = "%prog num_probe_messages out_file"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()
    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cdl_file = "/d2/vii/data/static/cdl/probe_message.cdl"
    num_messages = int(args[0])
    out_file = args[1]
    
    sim_reader = simulation_reader.SimulationReader(num_messages)

    mapping_dict = {}
    ret = write_probe_message(cdl_file, sim_reader, out_file, sim_reader.Field_dict)
    if ret != 0:
        return -1        
    

if __name__ == "__main__":

   main()
