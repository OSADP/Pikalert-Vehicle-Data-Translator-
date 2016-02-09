#!/usr/bin/env python

""" Get Michigan UMTRI data from the ftp site and move it to the VDT area """

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2007
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import log_msg
import os
import sys
import string
from optparse import OptionParser
import name_schema
import tim
import time
import glob
from datetime import datetime
import shutil

def process_file(fname, yyyymmdd, output_dir, logg):

        
    #
    # Create the dated output directory (received date/time),
    # if necessary
    #
    dated_output_dir = os.path.join(output_dir, yyyymmdd)
    if not os.path.exists(dated_output_dir):
        mkdir_command = "csh -c 'mkdir -p %s'" % dated_output_dir
        ret = log_msg.run_command(mkdir_command, logg)
            
        if ret != 0:
            logg.write_error("mkdir error")
            logg.write_ending(1, "get_michigan_umtri_data.py")
            return 1        

    #
    # Move the file from the input directory to the dated output directory
    # If the input file already exists, replace it with the new one as usually
    # we see 0 files files first, followed by files with data in them.
    # 
    logg.write_info("Moving %s %s" % (fname, dated_output_dir))

    try:
        basename = os.path.basename(fname)
        full_output_path = os.path.join(dated_output_dir, basename)
        if basename[0] == " ":
            basename = basename[1:]
            full_output_path = os.path.join(dated_output_dir, basename)
        #logg.write_info("%s basename" % basename)
        #logg.write_info("%s dated output_dir" % dated_output_dir)
        #logg.write_info("%s full_output_path" % full_output_path)
    
        if os.path.exists(full_output_path):
            os.remove(full_output_path)
            logg.write_info("%s already existed.  Removing and replacing with new file." % full_output_path)
        #shutil.move(fname, dated_output_dir)
        shutil.move(fname, full_output_path)
        
    except OSError, e:
        logg.write_error("error %s moving %s to %s " % (e, fname, dated_output_dir))
        logg.write_ending(1, "get_michigan_umtri_data.py")
        return 1
        
    

def main():

    usage_str = "%prog config input_dir output_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    (options, args) = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    input_dir = args[1]
    output_dir = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    logg.set_suffix(".pyl")        
    logg.write_starting()

    cf = __import__(config_file, globals(), locals(), [])



    #
    # Create file list
    #
    file_list = []
    last_file = ""
    glob_path = os.path.join(input_dir, "*")
    file_list = glob.glob(glob_path)

    final_ret = 0

    #
    # Sort files by modification time and at first exclude the most recent file
    # as it may not be finished coming in yet, but check to see if it is at least
    # one minute old at the end of the processing of the other files
    #

    file_list.sort(key=lambda x: os.path.getmtime(x))
    if (file_list != []):
        last_file = file_list[-1]
        file_list = file_list[:-1]
    
    for fname in file_list:

        unix_time = os.path.getmtime(fname)
        date_time = datetime.fromtimestamp(int(unix_time)).strftime('%Y%m%d %H:%M:%S')

        yyyymmdd = date_time[0:8]

        process_file(fname, yyyymmdd, output_dir, logg)
    
    #
    # Process the last file now
    #
    
    #
    # Get the modification time of the file and check to see if it is more than one minute old
    # If it is process, if not, wait for the next run
    #

    if (last_file != ""):
        mod_time = os.stat(last_file).st_mtime
        now_time =  time.mktime(time.gmtime())

        if (now_time - mod_time) >= 60:
            unix_time = os.path.getmtime(last_file)
            date_time = datetime.fromtimestamp(int(unix_time)).strftime('%Y%m%d %H:%M:%S')

            yyyymmdd = date_time[0:8]

            process_file(last_file, yyyymmdd, output_dir, logg)
        
    logg.write_ending(final_ret, "get_michigan_umtri_data.py")
    sys.exit(final_ret)


if __name__ == "__main__":
    
    main()

