#! /usr/bin/env python

import os,sys
import time

import name_schema
import index_file
import log
import tim

convert_command = "grid_convert"

def grid_convert(pres_time, infile_path, var_names_file, cdl_file, output_dir, out_index_file, logfile, input_base, output_base, testing):

    success = 0
    logfile_path = logfile.get_log_path()
    

    # Construct output path names.
    ifo = name_schema.Fcst_fname(input_base, "nc")
    
    infile = os.path.basename(infile_path)
        
    file_date_str = ifo.get_date(infile)
    file_date_tup = tim.datetotuple(file_date_str)
    file_date = tim.mkgmtime(file_date_tup)
    
    output_path = os.path.join(output_dir, file_date_str)    
    if (not os.path.exists(output_path)):
	logfile.write_time("Info: Executing mkdir -p %s\n" % output_path)
	if not testing:
	    ret = os.system("mkdir -p %s 2> /dev/null" % output_path)
	    if (ret != 0):
		logfile.write_time("Error: Unable to make directory.\n")
		return (0)
        
    ofo = name_schema.Fcst_fname(output_base, "nc")    
    outfile = ofo.make_name(file_date_str, ifo.get_it(infile), ifo.get_ft(infile))
    outfile_path = os.path.join(output_path, outfile)

    if (out_index_file.file_processed(outfile, file_date)):
        logfile.write_time("Info: File %s already exists.\n" % outfile)
        return (1)


    logfile_arg = ""
    if (logfile_path != ""):
        logfile_arg = "-l %s" % logfile_path
    command = "%s %s %s %s %s %s" % (convert_command, infile_path, var_names_file, cdl_file, outfile_path, logfile_arg)

    logfile.write_time("Info: Executing %s\n" % command)
    if not testing:
        ret = os.system(command)

        if (ret == 0):
            write_str = "%s %d" % (outfile, int(pres_time))
            out_index_file.write(write_str, file_date)            
            success = 1
        else:
            logfile.write_time("Error: Unable to convert to file %s. \n" % outfile)

    return (success)
        


