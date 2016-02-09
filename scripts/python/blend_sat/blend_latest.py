#!/usr/bin/env python

import os
import sys
import ncvdefs
import log
import index_file
import time
import name_schema
import get_file
import get_nc_var
import tim

# Author Beth Weekley @ UCAR/RAL  6/06

def usage():
    print "usage: %s west_input_dir east_input_dir cdl_file  output_dir [-L logfile_base]" % sys.argv[0]
    sys.exit(2)

#------------------------------------------------------------------
def check_latest_file(west_last_file, east_last_file, pres_time, time_window):
    """
    This function checks the latest west and east file to see if they are
    within the time window.
    Inputs:

    Outputs:

    """

    blend_opts_names = ("west", "east", "both")
    num_blend_opts = len(blend_opts_names)
    (OUT_g10, OUT_g12, OUT_both) = xrange(num_blend_opts)

    # check to see if you have only one file and if it is within the time_window
    if (west_last_file != "" ):
        west_tm = get_nc_var.get_time(west_last_file,"west_reftime")
        west_cloud_time = west_tm
        if ((pres_time - west_cloud_time) < time_window):
            new_cloud_time = west_tm
            old_cloud_time = west_tm
            files_in_blend = OUT_g10
            last_file = west_last_file
        else:
            # if the time criteria is not met then set the file to an empty string
            west_last_file = ""
            
    if (east_last_file != "" ):  
        east_tm = get_nc_var.get_time(east_last_file,"east_reftime")
        east_cloud_time = east_tm
        if ((pres_time - east_cloud_time) < time_window):
            new_cloud_time = east_tm
            old_cloud_time = east_tm
            files_in_blend = OUT_g12
            last_file = east_last_file            
        else:
            east_last_file = ""

    
    if (west_last_file != "" and east_last_file != ""):
        files_in_blend = 2
        if (west_cloud_time < east_cloud_time):
            new_cloud_time = east_cloud_time
            old_cloud_time = west_cloud_time
        else:
            new_cloud_time = west_cloud_time
            old_cloud_time = east_cloud_time

    if (west_last_file == "" and east_last_file == ""):
        files_in_blend = 0
        new_cloud_time = ""
        old_cloud_time = ""
        last_file = ""
            

    return(new_cloud_time, old_cloud_time, files_in_blend, west_last_file, east_last_file, last_file)

#------------------------------------------------------------------
def main():
    arg_len = (len(sys.argv))
    if (arg_len < 9):
	print "Not enough command line arguments"
	usage()

    script = os.path.basename(sys.argv[0])
    west_input_dir = sys.argv[1]
    west_input_base = sys.argv[2]
    east_input_dir = sys.argv[3]
    east_input_base = sys.argv[4]
    cdl_file = sys.argv[5]
    output_dir = sys.argv[6]
    logfile_path = ""

    i = 5
    while (i<arg_len):
	if (sys.argv[i] == "-L"):
	    i = i + 1
	    logfile_path = sys.argv[i]

	i = i+1

    logfile = log.Log(logfile_path)
    if (logfile_path == ""):
        logfile_arg = ""
    else:
	logfile_arg = "-l %s" % logfile_path
        logfile.set_suffix(ncvdefs.PYL_SUFFIX)
        logfile.write_time("Starting. %s\n" % script)
        

    west_input_index = index_file.Index_file(west_input_dir, ncvdefs.Index_base, ncvdefs.Index_period, "r", ncvdefs.Index_wait)
    east_input_index = index_file.Index_file(east_input_dir, ncvdefs.Index_base, ncvdefs.Index_period, "r", ncvdefs.Index_wait)
    output_index =  index_file.Index_file(output_dir, ncvdefs.Index_base, ncvdefs.Index_period, "w", ncvdefs.Index_wait)

    pres_time = time.time()
    # minutes to look back at previous file
    num_min = ncvdefs.blend_time_window + 10 
    start_time = pres_time - num_min*60
    

    nc2conus_command = "blend_sat"
    get_nc_var_command = "get_nc_var.py"
    output_base = ncvdefs.Cloud_mask_base
    suffix = ncvdefs.NETCDF_SUFFIX

    west_last_file = get_file.get_last_file(west_input_index, start_time, pres_time)
    east_last_file = get_file.get_last_file(east_input_index, start_time, pres_time)

    if (west_last_file == "" and east_last_file == ""):
	logfile.write_time(" Info: No GOES west or GOES east files within last %d minutes.\n" %num_min)
	logfile.write_time("Ending. Exit status = 0.\n")
	sys.exit(1)


    # check to see if the last files are within the the time delta
    (new_cloud_time, old_cloud_time, files_in_blend, west_last_file, east_last_file, last_file)=check_latest_file(west_last_file, east_last_file, pres_time, ncvdefs.blend_time_window)


    if (east_last_file == "" and west_last_file == "") :
	logfile.write_time(" No files within the blend_time_window = %d sec to process.\n" %ncvdefs.blend_time_window)
	logfile.write_time("Ending. Exit status = 0.\n")
        sys.exit(0)

    out_fno = name_schema.Data_fname(output_base, suffix)
    # create output name, which corresponds to the time the file was processed
    output_name =  out_fno.make_name_utime(int(pres_time))
    
    #date_str = output_index.get_date_string(old_cloud_time)
    #date_str = output_inde.get_date_string(new_cloud_time)
    (pres_date, pres_hhmm) = tim.sec2date(pres_time)
    date_str = pres_date
    output_dated_dir = os.path.join(output_dir, date_str)
    if (not os.path.exists(output_dated_dir)):
        os.makedirs(output_dated_dir)

    output_path = os.path.join(output_dated_dir, output_name)

    
    if (west_last_file != "" and east_last_file != ""):
        command = "%s -w %s -e %s -c %s -o %s %s" % (nc2conus_command, west_last_file, east_last_file, cdl_file, output_path, logfile_arg)
    else:
        command = "%s %s %s %s %s %s %d" %(get_nc_var_command, last_file, cdl_file, output_path, "is_cloud", "files_in_blend", files_in_blend)


    logfile.write_time(" Info: Executing %s.\n" % command)
    ret = os.system(command)

    if (ret != 0):
	logfile.write_time("Error: Executing %s.\n" % command)
	logfile.write_time("Ending. Exit status = 1.\n")
	sys.exit(1)
    else:
        write_line = "%s %s" % (output_name, int(pres_time))
        output_index.write(write_line, pres_time)


    logfile.write_time("Ending. %s. Exit status = 0 \n" %script)
    return 0

# -------------------------
if __name__ == "__main__":
    main()    


