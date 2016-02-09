#!/usr/bin/env python

"""
This  script to convert model data from one projection to another. 
"""
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2006
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licensed use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Author Beth Weekley @ UCAR/RAL  5/06

import os,sys
import getopt
import get_file
import index_file
import log
import tim
import time
import string
import ncvdefs
import name_schema
from grid_convert import *
import filename_io
import sys_functions
from optparse import OptionParser


#######################################################################
def main():

    optlist = []
    args = []
    testing = 0
    logfile_path = ""
    
    # -------------
    usage_str = "\n%prog \n\tinput_base\tbase name of model data\n\tinput_dir\tpath to input files \n\tvar_names_file\tfile containing the variables to extract\n\tcdl_file\tpath to cdl file used for creating output file \n\toutput_dir\tlocation to write output data\n\ttmp_dir \tlocation to write the gzip data file & them mv to the output_dir \n"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--logfile_path", dest="logfile_path", help="log file")
    parser.add_option("-t", "--testing", dest="testing", help="testing")

    (options, args) = parser.parse_args()

    if len(args) < 7:
        print "%s: Error - incorrect number of arguments" % sys.argv[0]
        parser.print_help()
        sys.exit(2)

    script = os.path.basename(sys.argv[0])
    input_base = args[0]
    input_dir = args[1]
    var_names_file = args[2]
    cdl_file = args[3]
    output_base = args[4]
    output_dir = args[5]
    tmp_dir = args[6]

    logfile_path = options.logfile_path
    logfile = log.Log(logfile_path, ncvdefs.PYL_SUFFIX)
    logfile.write_time("Starting %s\n" % script)


    logfile_arg = ""
    if (logfile_path != ""):
        logfile_arg = "-l %s" % logfile_path
        
    index_file_base = ncvdefs.Index_base
    index_file_hours = ncvdefs.Index_period
    index_file_wait = ncvdefs.Index_wait
    index_file_prior_days = ncvdefs.Index_prior_days

    # -------------
    # get the base and suffix info from the ncvdefs.py script
    # -------------
    input_suffix = ncvdefs.NETCDF_SUFFIX

    # -------------
    # the output index filename should have the name of the channel in it.
    # -------------
    output_suffix = ncvdefs.NETCDF_SUFFIX

    # -------------
    # Create a Unix time from date/time info
    # -------------
    pres_time = time.time()
    exec_cmd = "grid_convert"

    # -------------
    # create an instance of the name_schema
    # -------------
    in_fno = name_schema.Data_fname(input_base, input_suffix)
    out_fno = name_schema.Data_fname(output_base, output_suffix)

    # -------------
    # set up the index files
    # -------------
    input_index = index_file.Index_file(input_dir, index_file_base, index_file_hours, "r", index_file_wait)
    output_index = index_file.Index_file(output_dir, index_file_base, index_file_hours, "w", index_file_wait)
    
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
                if (not os.path.exists(tmp_dir)):
		    os.makedirs(tmp_dir)

                outfile_path = os.path.join(output_dated_dir, output_name)
                tmpfile_path =  os.path.join(tmp_dir, output_name)

                command = "%s %s %s %s %s %s" %(exec_cmd, in_path, var_names_file, cdl_file, tmpfile_path, logfile_arg)
                logfile.write_time("  Executing: %s\n" % command)
                ret = os.system(command)
                if (ret != 0):
                    logfile.write_time("  Error: Executing %s\n" % command)
                    logfile.write_time("Ending %s, status = 1\n\n" % script)
                    return 1
                else:
                    # gzip the file
                    command = "gzip %s" %(tmpfile_path)
                    logfile.write_time("  Executing: %s\n" % command)
                    ret = os.system(command)       
                    if (ret != 0):
                        logfile.write_time("  Error: Executing %s\n" % command)
                        logfile.write_time("Ending %s, status = 1\n\n" % script)
                        return 1
            
                    command = "mv -f %s.%s %s.%s" %(tmpfile_path, ncvdefs.GZIP_SUFFIX, outfile_path, ncvdefs.GZIP_SUFFIX)
                    logfile.write_time("  Executing: %s\n" % command)
                    ret = os.system(command)
                    if (ret != 0):
                        logfile.write_time("  Error: Executing %s\n" % command)
                        logfile.write_time("Ending %s, status = 1\n\n" % script)
                        return 1


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
