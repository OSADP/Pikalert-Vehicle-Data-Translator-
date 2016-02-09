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

import os,sys
import string
import get_cloud_mask_files
import getopt
import log
import ncvdefs

#------------------------------------------------------------------
def usage():
    print "usage: %s input_unproc_dir, [-d day_list] input_store_dir, output_dir, tmp_dir, config_file, input_base, output_base, scan_type, logfile_path, intermed_cdl, cloud_mask_cdl, output_cloud_id_file, config_cloud_mask" % sys.argv[0]
    print "day_list:         a list of directories to process in the form  YYYYMMDD, 20050502"

    sys.exit(2)
#------------------------------------------------------------------
# -------------------------
def main():

    optlist      = []
    args         = []
    day_list     = []
    logfile_path = ""

    # Process command args
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "d:")
    except:
        print "%s: Error in parsing options, %s" % (sys.argv[0], sys.exc_value)
        usage()
        sys.exit(2)

    for i in xrange(0,len(optlist)):
        if optlist[i][0] == "-d":    # day list, list of YYYYMMDDHHMM
            day_list = optlist[i][1]

    if (len(args) != 13):
	print "%s: Error incorrect number of command line arguments" % sys.argv[0]
	usage()

    script = os.path.basename(sys.argv[0])
    input_unproc_dir      = args[0]
    input_store_dir       = args[1]
    output_dir_files      = args[2]
    tmp_dir               = args[3]
    config_file           = args[4]
    input_base            = args[5]
    output_base           = args[6]
    scan_type             = args[7]
    log_dir               = args[8]
    intermed_cdl          = args[9]
    cloud_mask_cdl        = args[10]
    output_cloud_id_file  = args[11]
    config_cloud_mask     = args[12]


    logfile_path = "%s/%s" % (log_dir, "cloud_mask_byte")
    logfile = log.Log(logfile_path)
    logfile.set_suffix(ncvdefs.PYL_SUFFIX)
    logfile.write_time("Starting. %s\n" % script)
    logfile.write_time("  Info: file type = %s\n" % input_base)

    # --------------
    # get all the satellite files in the all the dated subdirectories
    # files of the input directory and process all the
    # unprocessed files in these directories.  
    # --------------
    if (day_list == []):
	if os.path.isdir(input_unproc_dir):
	    dir_list = os.listdir(input_unproc_dir)
	    for ind in xrange(len(dir_list)):
                str = string.split(dir_list[ind],'.') 
                if (str[0] != "index"):
                    day_list.append(dir_list[ind])


    # --------------
    # open the config file
    # --------------
    config_data = get_cloud_mask_files.get_config_data(config_file)

    # --------------
    # loop over all the files in the day_list
    # and process one directory at a time
    # --------------
    day_list.sort()
    for dir_ind in xrange(len(day_list)):
        unprocessed_path = input_unproc_dir + "/" + day_list[dir_ind]
        file_list = os.listdir(unprocessed_path)
        if file_list == []:
            #logfile.write_time("  Info: No unprocessed files in the directory %s .\n" %unprocessed_path)
            #logfile.write_time("  Info: Continue to the next file,  status = 0\n")
            continue

        # --------------
        # get the dates and times out of the file names
        # --------------
        file_dates = []
        for ind in xrange(len(file_list)):
            name_split = string.split(file_list[ind], ".")
            file_dates.append(name_split[1]+name_split[2])

        # --------------
        # loop over these dates and then move the
        # processed files to a processed directory
        # --------------
        for date_ind in xrange(len(file_dates)):
            date_str = file_dates[date_ind]
        
            # --------------
            # get the run date
            # --------------
            run_str = date_str[8:12]
            run_HHMM = get_cloud_mask_files.get_minute(config_data, run_str)
            date_str_closest = date_str[0:8] + run_HHMM
            
            # --------------
            # create the directory strings
            # --------------
            full_file_name = input_base + "." + date_str[0:8] + "." + date_str[8:12] + "."  + ncvdefs.NETCDF_SUFFIX+ "." + ncvdefs.GZIP_SUFFIX

            unprocessed_file = "%s/%s/%s" %(input_unproc_dir,date_str[0:8], full_file_name)
            processed_path = "%s/%s" %(input_store_dir,date_str[0:8])

            # --------------
            # make sure the date_str_closest is within delta_min of the
            # original time, if not it may not be a desired file
            # --------------
            file_time_diff = abs(get_cloud_mask_files.date2sec(date_str) - get_cloud_mask_files.date2sec(date_str_closest))
            if (file_time_diff > (config_data.delta_min*60) ):
                #logfile.write_time("  Info: The run file %s is %f seconds from the closest time in the config file,  status = 0\n" %(full_file_name,file_time_diff))
                #logfile.write_time("  Info: continue to the next file,  status = 0\n")
                continue

            if (not os.path.exists(tmp_dir)):
                os.makedirs(tmp_dir)

            tmp_path = "%s/%s" %(tmp_dir,date_str)
            if (not os.path.exists(output_dir_files)):
                os.makedirs(output_dir_files)
            output_file = "%s%s.%s.txt" %(output_dir_files,input_base,date_str)

            # --------------
            # check to see that the processed directory exists
            # move the processed files to the processed directory
            # --------------
            if (not os.path.exists(processed_path)):
                os.makedirs(processed_path)

            # --------------
            # copy the unprocessed file to the processed directory
            # with a "." and then move the file in the same 
            # directory without the "."
            # --------------
            if (os.path.exists(processed_path)):
                processed_file_tmp = "%s/%s%s" %(processed_path, ".", full_file_name )   
                command = "cp %s %s" %(unprocessed_file, processed_file_tmp)
                logfile.write_time("  Info: executing the command: %s\n" % command)
                ret = os.system(command)
                if (ret != 0):
                    logfile.write_time("Error: Unable to %s \n" % command)
                    logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                    continue
                
                processed_file =  "%s/%s" %(processed_path, full_file_name )   
                command = "mv %s %s" %(processed_file_tmp, processed_file)
                logfile.write_time("  Info: executing the command: %s\n" % command)
                ret = os.system(command)
                if (ret != 0):
                    logfile.write_time("Error: Unable to %s \n" % command)
                    logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                    continue
                
            # --------------
            # get the list of satellite files
            # --------------
            proc_script_get_files = "get_cloud_mask_files.py"
            proc_logfile = "%s/cloud_mask_history" % (log_dir)

            command = "%s -L %s %s %s %s %s %s" %(proc_script_get_files, proc_logfile, input_store_dir, output_file, input_base, date_str, tmp_path)
            logfile.write_time("  Info: executing the command: %s\n" % command)
            ret = os.system(command)
            if (ret !=0 ):
                logfile.write_time("Error: Unable to %s\n" % command)
                logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                continue

            # --------------
            # run cloud id code
            # --------------
            proc_script = "nasa_cloud_mask_byte.py"
            proc_logfile = "%s/nasa_cloud_mask" % (log_dir)
            filenames = output_file    

            command = "%s -L %s %s %s %s %s %s %s %s %s" %(proc_script, proc_logfile, filenames, output_cloud_id_file, date_str, input_base, output_base, cloud_mask_cdl, config_cloud_mask, scan_type ) 

            logfile.write_time("  Info: executing the command: %s\n" % command)
            ret = os.system(command)
            if (ret != 0):
                logfile.write_time("Error: Unable to %s\n" % command)
                logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                continue

            # --------------
            # remove the temporary files 
            # --------------
            command = "rm -rf %s" %tmp_path
            logfile.write_time("  Info: executing the command: %s\n" % command)
            # temporarily suspend this step
            #ret = 0
            ret = os.system(command)
            if (ret != 0):
                logfile.write_time("Error: Unable to %s\n" % command)
                logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                continue
    
            # --------------
            # remove the processed file
            # --------------
            if (os.path.exists(processed_path)):
                command = "rm %s" %(unprocessed_file)
                logfile.write_time("  Info: executing the command: %s\n" % command)
                ret = os.system(command)
                if (ret != 0):
                    logfile.write_time("Error: Unable to %s\n" % command)
                    logfile.write_time("  Info: Continue to the next file,  status = 1\n")
                    continue


    logfile.write_time("Ending. %s. Exit status = 0 \n" %script)
    return 0

# -------------------------
if __name__ == "__main__":
    main()    
