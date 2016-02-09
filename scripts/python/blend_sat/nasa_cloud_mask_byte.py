#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2004
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Author Beth Weekley  3/05  @ UCAR
 
import os,sys
import log
import getopt
import ncvdefs
import index_file
import time

#------------------------------------------------------------------

def usage():
    print "usage: %s filenames_file output_dir cloud_mask_cdl date_str input_base output_base -L logfile [-m intermed_cdl]" % sys.argv[0]
    print "filenames_file     - the file with the list of files over the past 20 days, current file last"
    print "output_dir         - location of output files"
    print "cloud_mask_cdl     - cloud_mask_cdl file"
    print "config_file        - config file"
    print "date_str           - date string used to get the dated subdirectory for output and also the file name, YYYYMMDDHH"
    print "input_base         - g10 or g12"
    print "output_base        - eg g10, g12"
    print "-L logfile       - log file"
    print "[-m intermed_cdl]  - cdl file which corresponds to the intermediate matricies"
    
    sys.exit(2)
#------------------------------------------------------------------

if __name__ == "__main__":

    optlist = []
    args = []
    logfile_path   = ""
    filenames      = ""
    intermed_cdl   = ""
    output_path    = ""
    cloud_mask_cdl = ""
    config_file    = ""
    scan_type      = ""

    nasa_cloud_mask_command = "cloud_id_byte"

    # Process command args
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "L:m:")
    except:
        print "%s: Error in parsing options, %s" % (sys.argv[0], sys.exc_value)
        usage()
        sys.exit(2)

    for i in xrange(0,len(optlist)):
        if optlist[i][0] == "-L":    # logfile
            logfile_path = optlist[i][1]
        elif optlist[i][0] == "-m":  # intermediate cdl file
            intermed_cdl = optlist[i][1]
            
    if (len(args) != 8):
	print "%s: Error incorrect number of command line arguments" % sys.argv[0]
	usage()

    script = os.path.basename(sys.argv[0])
    filenames      = args[0]
    output_dir     = args[1]
    date_str       = args[2]
    input_base     = args[3]
    output_base    = args[4]
    cloud_mask_cdl = args[5]
    config_file    = args[6]
    scan_type      = args[7]

    logfile = log.Log(logfile_path)
    if (logfile_path == ""):
        logfile_arg = ""
    else:
	logfile_arg = "-l %s" % logfile_path
        logfile.set_suffix(ncvdefs.PYL_SUFFIX)

    logfile.write_time("Starting %s\n" % script)
    logfile.write_time("  Info: file type = %s\n" % input_base)

    out_index =index_file.Index_file(output_dir,ncvdefs.Index_base,ncvdefs.Index_period,"w",ncvdefs.Index_wait)
    file_suffix = ncvdefs.NETCDF_SUFFIX
    output_str = ncvdefs.Cloud_mask_base
    input_name = "%s.%s.%s" %(input_base, date_str, file_suffix)
    output_name = "%s_%s.%s.%s.%s" %(output_base, output_str, date_str[0:8], date_str[8:12], file_suffix)

    sat_file_time = date_str[0:8]

    # if output file already exists, exit
    if (out_index.file_processed(output_name, sat_file_time)):
	    logfile.write_time("  Info: the file %s, has already been processed.\n" %output_name)
	    logfile.write_time("Ending: exit status = 0\n")
	    sys.exit(0)

    output_dated_dir = os.path.join(output_dir, date_str[0:8])
    if (not os.path.exists(output_dated_dir)):
        os.makedirs(output_dated_dir)
        
    output_path = os.path.join(output_dated_dir, output_name)

    if intermed_cdl:
        intermed_output_str = "composite"
        #intermed_outputname =  "%s_%s.%s.%s%s.%s" %(input_base, intermed_output_str, date_str[0:8], date_str[8:10], "00", file_suffix)
        intermed_outputname =  "%s_%s.%s.%s.%s" %(output_base, intermed_output_str, date_str[0:8], date_str[8:12], file_suffix)
        output_dated_composite_dir = os.path.join(output_dated_dir,intermed_output_str)
        if (not os.path.exists(output_dated_composite_dir)):
            os.makedirs(output_dated_composite_dir)

        intermed_output_path = os.path.join(output_dated_composite_dir, intermed_outputname)
        command="%s -i %s -m %s -n %s -o %s -c %s -C %s %s" %(nasa_cloud_mask_command, filenames, intermed_cdl, intermed_output_path, output_path, cloud_mask_cdl, config_file, logfile_arg)

    else:
        command="%s -i %s -o %s -c %s -C %s -s %s %s" %(nasa_cloud_mask_command, filenames, output_path, cloud_mask_cdl, config_file, scan_type, logfile_arg)

    logfile.write_time("  Info: executing the command: %s\n" % command)
    ret = os.system(command)
    
    if (ret != 0):
	logfile.write_time("Error: Executing %s\n" % command)
	logfile.write_time("Ending. Exit status = 1\n")
	sys.exit(1)
 
    pres_time = time.time()
    index_line = "%s %d" % (output_name, int(pres_time))
    out_index.write(index_line, sat_file_time)

    logfile.write_time("Ending. Exit status = 0\n")
