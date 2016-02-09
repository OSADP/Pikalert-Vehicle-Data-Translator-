#!/usr/bin/env python
"""
This script calls the Gini2nc C++ executable to convert
raw GINI files to netCDF files. 
"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2006
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Author Beth Weekley @ UCAR/RAL  4/06

import sys
import os
import getopt
from optparse import OptionParser
import index_file
import ncvdefs
import log
import filename_io
import name_schema
import time
import tim
import string
import sys_functions

index_base = ncvdefs.Index_base
index_file_hours = ncvdefs.Index_period
index_file_wait = ncvdefs.Index_wait
index_file_prior_days = ncvdefs.Index_prior_days
input_base = ncvdefs.Gini_base
input_gini_suffix = ncvdefs.GINI_SUFFIX
output_base =  ncvdefs.Gini2nc_out_base
output_suffix = ncvdefs.NETCDF_SUFFIX


# -------------------------
def main():

    # Parse file name option
    args = []
    input_dir = ""
    output_dir = ""
    cdl_file = ""
    params_dir = ""
    logfile = ""

    # -------------
    usage_str = "%prog input_dir output_dir cdl_file params_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--logfile_path", dest="logfile_path", help="log file")

    (options, args) = parser.parse_args()

    if len(sys.argv) < 6:
        parser.print_help()
        print "incorrect command line arguements"
        sys.exit(2)

    script = os.path.basename(sys.argv[0])
    input_dir = args[0]
    output_dir = args[1]
    cdl_file = args[2]
    params_dir = args[3]
    channel = args[4]
    satellite = args[5]

    logfile_path = options.logfile_path
    
    # -------------
    # get the base and suffix info from the ncvdefs.py script
    # -------------
    input_suffix = input_gini_suffix
    input_base = satellite + "_" + channel

    # -------------
    # the output index filename should have the name of the channel in it.
    # -------------
    output_file_base = satellite + "_" + output_base

    # -------------
    pres_time = time.time()
    exec_cmd = "Gini2nc"

    # -------------
    # set up the log file
    # -------------
    logfile = log.Log(logfile_path, ncvdefs.PYL_SUFFIX)
    logfile.write_time("Starting %s\n" % script)

    logfile_p = logfile.get_log_path()
    logfile_arg = ""
    if (logfile_p != ""):
        logfile_arg = "-l %s" % logfile_p

    # -------------
    # create an instance of the name_schema
    # -------------
    in_fno = name_schema.Data_fname(input_base, input_suffix)
    out_fno = name_schema.Data_fname(output_file_base, output_suffix)

    # -------------
    # set up the index files
    # -------------
    input_index = index_file.Index_file(input_dir,index_base,index_file_hours,"r",index_file_wait)    
    output_index_base = index_base + "_" + channel
    output_index=index_file.Index_file(output_dir,output_index_base,index_file_hours,"w",index_file_wait)

    # -------------
    # create an instance of the MapFileName class
    # -------------
    map_file_name = filename_io.MapFileName(in_fno, out_fno)

    # -------------
    # create an instance of the UnprocessedFiles
    # -------------
    get_unproc_files = filename_io.UnprocessedFiles(map_file_name, input_index, output_index)
    
    # ---------
    # get the date from the input index name
    # ---------
    input_str = os.path.basename(input_index.curr_index_file)
    input_split = string.split(input_str, ".")
    init_input_date = input_split[1]
    input_sec = tim.date2sec(init_input_date)

    # ---------
    # loop over the number of days to process
    # ---------
    np = 0
    for ndays in xrange(0,index_file_prior_days+1):
        prior_days_sec = input_sec - ndays*86400
        (input_date, hr_str) = tim.sec2date(prior_days_sec)
        # -------------
        # call the method get_files in the UnprocessedFiles class, and pass in the date
        # -------------
        unproc_files = get_unproc_files.get_files(input_date)
        if (len(unproc_files) == 0 ):
            continue
        
        # ---------
        # loop over all the unprocessed files and process them
        # ---------
        else: 
            for ind in unproc_files:
                file_path = os.path.join(input_dir,input_date)
                in_path = os.path.join(file_path, ind)
                file_bname = os.path.basename(in_path)

                file_time = in_fno.get_utime(file_bname)
            
                output_name = out_fno.make_name_utime(file_time)
                # remove the initial '.' from the file name
                if output_name[0] == ".":
                    output_name = output_name[1:]

                date_str = output_index.get_date_string(file_time)
                
                output_dated_dir = os.path.join(output_dir, date_str)
                if (not os.path.exists(output_dated_dir)):
		    os.makedirs(output_dated_dir)

                output_path = os.path.join(output_dated_dir, output_name)
                
                command = "%s %s %s %s %s %s" %(exec_cmd, logfile_arg, in_path, params_dir, output_path, cdl_file)
                logfile.write_time("  Executing: %s\n" % command)
                ret = os.system(command)
                if (ret != 0):
                    logfile.write_time("  Error: Executing %s\n" % command)
                    logfile.write_time("Ending %s, status = 1\n\n" % script)
                    return 1
                else:	    
 	            index_file_io = sys_functions.IndexFileIo()
                    index_line = index_file_io.create_index_line(output_name, pres_time)
                    output_index.write(index_line, file_time)
                    np = np + 1

                if np == ncvdefs.Max_sat_files_to_process:
                    logfile.write_time("  Info. Reached maximum file process limit (%d).\n" % np)
                    logfile.write_time("Ending %s, status = 0\n\n" % script)
                    return 0
    
    logfile.write_time("Ending %s, status = 0\n\n" % script)
    
    return 0
# -------------------------
if __name__ == "__main__":
    main()    



