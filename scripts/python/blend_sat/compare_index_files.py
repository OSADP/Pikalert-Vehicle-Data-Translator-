#!/usr/bin/env python
"""
This script compares two index files to make
sure the same file times have been processed
and then adds the output times which have been
processed in both input files to an output file.  
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
# Author Beth Weekley @ UCAR/RAL  5/06

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
output_base =  ncvdefs.Gini2nc_out_base
output_suffix = ncvdefs.NETCDF_SUFFIX

# -------------------------
def main():

    # Parse file name option
    args = []
    input_dir = ""
    output_dir = ""
    logfile = ""

    # -------------
    usage_str = "%prog input_dir_a input_dir_b output_dir satellite"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--logfile_path", dest="logfile_path", help="log file")

    (options, args) = parser.parse_args()

    if len(sys.argv) < 3:
        parser.print_help()
        print "incorrect command line arguements"
        sys.exit(2)

    script = os.path.basename(sys.argv[0])
    input_dir = args[0]
    output_dir = args[1]
    satellite = args[2]

    logfile_path = options.logfile_path

    channel = ncvdefs.sat_channels
    
    # -------------
    # get the base and suffix info from the ncvdefs.py script
    # -------------
    input_suffix = output_suffix
    input_base = satellite + "_" + output_base
    
    # -------------
    # the output index filename should have the name of the channel in it.
    # -------------
    output_file_base = satellite + "_" + output_base

    # -------------
    pres_time = time.time()

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
    # set up the index files, the number of input_index files changes depending on the number of channels
    # -------------
    input_index_base_list = []
    input_index_list = []
    for channel_ind in xrange(len(channel)):
        input_index_base_list.append(index_base + "_" + channel[channel_ind])
        input_index_list.append(index_file.Index_file(input_dir,input_index_base_list[channel_ind],index_file_hours,"r",index_file_wait))
        input_index = input_index_list[channel_ind]
        
    output_index_base = index_base
    output_index=index_file.Index_file(output_dir,output_index_base,index_file_hours,"w",index_file_wait)

    # -------------
    # create an instance of the MapFileName class
    # -------------
    map_file_name = filename_io.MapFileName(out_fno,in_fno)

    # -------------
    # create an instance of the UnprocessedFiles
    # -------------
    get_proc_files = filename_io.ProcessedFiles(map_file_name, input_index_list, output_index)
    
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
    for ndays in xrange(0,index_file_prior_days+1): 
        prior_days_sec = input_sec - ndays*86400
        (input_date, hr_str) = tim.sec2date(prior_days_sec)

        # ---------
        # call the method get_files in the UnprocessedFiles class, and pass in the date
        # ---------
        proc_files = get_proc_files.get_files(input_date)
        if (len(proc_files) == 0 ):
            continue
        else:
            # ---------
            # write the file names to the output file
            # ---------
            for ind in proc_files:
                file_time = in_fno.get_utime(ind)
                output_name = out_fno.make_name_utime(file_time)

                # create an instance of the index_file_io class
                index_file_io = sys_functions.IndexFileIo()
                line = index_file_io.create_index_line(output_name, pres_time)
                output_index.write(line, file_time)


    logfile.write_time("Ending %s, status = 0\n\n" % script)
    return 0
    
# -------------------------
if __name__ == "__main__":
    main()    



