#!/usr/bin/env python
"""
This function gets the input variable out of the netcdf
file and stores it in another netcdf file.
"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2004
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
#  Written by Beth Weekley  @ UCAR  July 2005

import os
import sys
from optparse import OptionParser
import ncvdefs
import log
import ncfile


#------------------------------------------------------------------
def get_time(in_file, time_name):

    """
    This function opens the in_file netcdf file and gets
    the requested time and returns it.


    Inputs:
    in_file    - netcdf input filename with path, which contains the time variable wanted
    time_name  - name of the time variable requested
    
    Outputs:
    time_out   - time in sec from the netcdf file

    """
    
    in_fp = ncfile.NcFile(in_file)
    time_out = in_fp.get_var_data(time_name)

    return time_out

    

#------------------------------------------------------------------

if __name__ == "__main__":
    """
    This function gets a variable out of one netcdf file and puts it
    into another netcdf file.  And adds the input variable, whos name
    and value are given on the command line, to the output file.  

    Inputs:
    in_filepath          - netcdf input filename with path, which contains the variable wanted
    cdl_file             - cdl file which corresponds to the output netcdf file
    out_filepath         - filename with path for the variable to be put into, this file is created in
                           this function with the cdl file
    var_name             - name of the variable which is wanted from the in_filepath
    field_name           - field number of files blended
    field_value          - integer value represents files

    """

    
    usage_str = "usage: %prog  [-l logfile_base] in_filepath cdl_file out_filepath var_name"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    (options, args) = parser.parse_args()

    if len(sys.argv) < 5:
        parser.print_help()
        sys.exit(2)
        
    script = os.path.basename(sys.argv[0])
    in_filepath = args[0] 
    cdl_file = args[1]
    out_filepath = args[2]
    var_name = args[3]
    field_name = args[4]
    field_value = args[5]

    logfile_path = ""
    
    if options.log:
        logfile = log.Log(options.log)
        logfile_arg = options.log
        logfile.set_suffix(ncvdefs.PYL_SUFFIX)
    else:
        logfile_arg = ""
        logfile = log.Log("")
        
    
    # create the output netcdf file using ncgen
    command = "ncgen -b %s -o %s" % (cdl_file, out_filepath)

    ret = os.system(command)
    if (ret != 0):
        logfile.write_time("Error failed to execute: ", command)
        raise ExecError("Failed to execute: ", command)
    
    # Open input NetCDF file for getting the cloud mask, "is_cloud"
    # and the times, reftime, valtime

    in_fp = ncfile.NcFile(in_filepath)
    in_var_array = in_fp.get_var_data(var_name)
    
    reftime = in_fp.get_var_data("reftime")
    valtime = in_fp.get_var_data("valtime")

    east_filename = in_fp.get_var_data("east_filename")
    east_reftime  = in_fp.get_var_data("east_reftime")
    east_valtime  = in_fp.get_var_data("east_valtime")

    west_filename = in_fp.get_var_data("west_filename")
    west_reftime  = in_fp.get_var_data("west_reftime")
    west_valtime  = in_fp.get_var_data("west_valtime")
    

    # Open output NetCDF file to add the variables to

    out_fp = ncfile.NcFile(out_filepath)
    out_fp.put_var_data(var_name, in_var_array)    
    
    out_fp.put_var_data("reftime", reftime)
    out_fp.put_var_data("valtime", valtime)

    out_fp.put_var_data("east_filename", east_filename)
    out_fp.put_var_data("east_reftime", east_reftime)
    out_fp.put_var_data("east_valtime", east_valtime)

    out_fp.put_var_data("west_filename", west_filename)   
    out_fp.put_var_data("west_reftime", west_reftime)
    out_fp.put_var_data("west_valtime", west_valtime)
        
    # write number of files in blend to output netcdf file
    field_val = int(field_value)
    out_fp.put_var_data(field_name, field_val)
