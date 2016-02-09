#!/usr/bin/env python

#*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
#* Copyright (c) 2006 UCAR
#* University Corporation for Atmospheric Research(UCAR)
#* National Center for Atmospheric Research(NCAR)
#* Research Applications Program(RAP)
#* P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
#* All rights reserved.
#*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import os
import sys
import ncvdefs
import log
import index_file
import time
import name_schema
import get_file
import index_utils
import datetime
import filename_io
#from sets import Set
import sys_functions

# Author Beth Weekley @ UCAR/RAL  6/06
"""
This code takes AWC input GINI file names and converts
them to our expected naming convention and creates links
to the original files.  Also it creates initial index
files at the same time.  This function can be used in
place of run_index_gini.py  and index_gini.py for AWC
GINI files.  
"""

def usage():
    print "usage: %s input_dir input_base output_dir output_base [-L logfile_base]" % sys.argv[0]
    sys.exit(2)

#------------------------------------------------------------------
def main():
    arg_len = (len(sys.argv))
    if (arg_len < 5):
        print "Not enough command line arguments"
        usage()

    script = os.path.basename(sys.argv[0])
    input_dir = sys.argv[1]
    input_base = sys.argv[2]
    output_dir = sys.argv[3]
    output_base = sys.argv[4]
    output_suffix = sys.argv[5]
    logfile_path = ""

    i = 5
    while (i<arg_len):
        if (sys.argv[i] == "-l"):
            i = i + 1
            logfile_path = sys.argv[i]
        i = i+1

    logfile = log.Log(logfile_path)
    if (logfile_path == ""):
        logfile_arg = ""
    else:
        logfile_arg = "-l %s" % logfile_path
        logfile.set_suffix(ncvdefs.PYL_SUFFIX)
        logfile.write_time("Starting %s\n" % script)

    num_days = ncvdefs.Index_prior_days
    # Generate day list
    date = datetime.datetime.utcnow()
    day_list = [0] * (num_days+1)
    for n in xrange(num_days+1):
        prev_date = date - datetime.timedelta(days=n)
        day_list[n] = prev_date.strftime("%Y%m%d")


    pres_time = time.time()
    # make sure the input directory exists if not exit with an error
    if (not os.path.exists(input_dir)):
        logfile.write_time("Error. in %s.  The input directory %s does not exist. \n" %(script,input_dir))
        logfile.write_time("Ending %s, status = 1\n\n" % script)
        return 1
    
    # get the list all the files in the input directory
    # note the files are not in dated subdirectories
    # so we need to go through the list. Filter out any hidden ('.') files.
    #dir_list = os.listdir(input_dir)
    dir_list = [fn for fn in os.listdir(input_dir) if fn[0] != '.']
    dir_list.sort()
    
    AWC_in_ns_pattern = ["YYYYMMDD","HHMM","",input_base,""]
    AWC_in_ns_format = "%D_%I.%F%B%S"
    AWC_ns = name_schema.Name_schema(AWC_in_ns_format,AWC_in_ns_pattern)

    out_ns_pattern = [output_base,"YYYYMMDD","HHMM","",output_suffix]
    out_ns_format = "%B.%D.%I.%F%S"
    out_ns = name_schema.Name_schema(out_ns_format,out_ns_pattern)

    # -------------
    # create an instance of the MapFileNameGeneral class
    # -------------
    out_fno = name_schema.Data_fname(output_base, output_suffix)
    map_file_name = filename_io.MapFileNameGeneral(AWC_ns,out_ns)

    output_index = index_file.Index_file(output_dir, ncvdefs.Index_base, ncvdefs.Index_period, "w", ncvdefs.Index_wait)
            
    # put the file names in sets corresponding to the dates
    if day_list != []:
        # get a list of dates corresponding to the file names
        date_list = []
        for file_ind in xrange(len(dir_list)):
            curr_file = dir_list[file_ind]
            # parse the file
            curr_date = AWC_ns.get_date(curr_file)
            ut = AWC_ns.get_utime(curr_file)
            date_list.append(curr_date)

        # loop over the day list to check for files
        for yyyymmdd in day_list:
            # put the above file names in a set
            input_set = set([])
            for date_ind in xrange(len(date_list)):
                if date_list[date_ind] == yyyymmdd:
                     input_set.add(dir_list[date_ind])

            # -------------
            # create an instance of the UnlinkedFiles
            # -------------
            get_unlinked_files = filename_io.UnlinkedFiles(map_file_name,input_set,output_index)
           
            # ---------
            # call the method get_files in the UnlinkedFiles
            # class, pass in the date
            # ---------
            unlinked_files = get_unlinked_files.get_files(yyyymmdd)
            if (len(unlinked_files) == 0 ):
                continue
            else:
                # check to see if the output dated dir exists if not create it. 
                output_dated_dir = os.path.join(output_dir, yyyymmdd)
                if (not os.path.exists(output_dated_dir)):
                    os.makedirs(output_dated_dir)

                # ---------
                # link the unlinked files & write the file
                # names in the output file
                # ---------
                for ind in unlinked_files:
                    
                    (file_time,f_time) = AWC_ns.get_utime(ind)
                    output_name = out_fno.make_name_utime(file_time)
                    # link the files
                    input_file = os.path.join(input_dir,ind)
                    output_file = os.path.join(output_dated_dir,output_name)
                    logfile.write_time("  Info: ln -s %s %s\n" % (input_file, output_file))
                    ret = index_utils.link_dirs(input_file, output_file)
 
                    if ret == 0:
                        # create an instance of the index_file_io class
                        index_file_io = sys_functions.IndexFileIo()
                        line = index_file_io.create_index_line(output_name, pres_time)
                        output_index.write(line, yyyymmdd)
                    else:
                        logfile.write_time("  Error: Creating symlink.\n")
                        logfile.write_time("Ending %s, status = 1\n\n" %script)
                                           
                                           
    logfile.write_time("Ending %s, status = 0\n\n" %script)
    return 0

# -------------------------
if __name__ == "__main__":
    main()    


