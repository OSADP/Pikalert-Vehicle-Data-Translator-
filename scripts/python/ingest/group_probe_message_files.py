#!/usr/bin/env python

"""A generic script to create probe message files grouped in a specific time period"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: group_probe_message_files.py,v $                                           #
#       Version: $Revision: 1.5 $  Dated: $Date: 2014/10/22 17:25:31 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import glob
import tim
import time
import name_schema
import shutil
from netCDF4 import Dataset
import datetime

# Test WXT Data
#group_probe_message_files.py vii_paths.py /d2/vii/data/static/cdl/probe_message.cdl $VII_DATA/tmp $VII_DATA/processed/mi_group_netcdf $VII_DATA/processed/temp wxt

def put_scalar_var(vname, in_var, time_interval_dict):
    """Put scalar variable value in appropriate output files"""

    for work_tint_tval in time_interval_dict:
        # Get appropriate file, rec_num for this index group
        (work_nc, work_rec_num) = time_interval_dict[work_tint_tval]
        work_var = work_nc.variables[vname]
        work_var.assignValue(in_var)
    return    


def put_array_var(vname, in_var, time_interval_dict, index_groups, logg):
    """Put array variable in_var in appropriate output files"""

    
    for work_tint_tval in time_interval_dict:
        
        work_list = index_groups[work_tint_tval]
        # Get appropriate file, rec_num for this index group
        (work_nc, work_rec_num) = time_interval_dict[work_tint_tval]

        # Create output matrix for the variable
        work_var = work_nc.variables[vname]
        if len(work_var.dimensions) == 2:
            #logg.write_debug("work_var.shape before append: %s" % str(work_var.shape))
            work_mat = in_var[work_list, :]
            work_var[work_rec_num:,:] = work_mat
            #logg.write_debug("work_var.shape after append: %s" % str(work_var.shape))
            
        else:
            work_mat = in_var[work_list]
            #logg.write_debug("work_var.shape before append: %s" % str(work_var.shape))
            work_var[work_rec_num:] = work_mat
            #logg.write_debug("work_var.shape after append: %s" % str(work_var.shape))

    return    

def get_index_groups(time_list):
    """Get index groups corresponding to the different time interval 
    tvals.  An index group is a pair consisting of a time interval
    time value and a list of indices corresponding to that time value. A
    dictionary will map the time interval tvals to their corresponding
    index lists.

    Inputs:
      time_list -- list of triples (tval, time_interval_time_val, ind)

    Returns:
      dictionary mapping time_interval_time_val to its index list
    """
    index_groups = {}
    for (tval, time_interval_time_val, ind) in time_list:
        if time_interval_time_val in index_groups:
            index_groups[time_interval_time_val].append(ind)
        else:
            index_groups[time_interval_time_val] = [ind]

    return index_groups        

    
def get_work_file(time_interval_time_val, cdl_file, storage_dir, storage_name_schema, work_dir):
    """Produce a copy of a time interval file in work_dir. If there is
    a copy in storage_dir, copy it to work_dir, otherwise generate a
    new file in work_dir.
    
    Inputs:
      time_interval_time_val -- time truncated to nearest time interval (for example, time truncated to the nearest hour)
      cdl_file -- name of cdl file for output file
      storage_dir -- storage directory name string
      storage_name_schema -- name schema object used to produce storage file name
      work_dir -- working directory name string

    Returns:
      file path string for file in work_dir
    """
    
    # Form dated output directory and create directory, if necessary
    storage_name = storage_name_schema.make_name_utime(time_interval_time_val, None)
    dir_date = storage_name_schema.get_date(storage_name)
    dated_storage_dir = os.path.join(storage_dir, dir_date)
    if not os.path.exists(dated_storage_dir):
        mkdir_command = "csh -c 'mkdir %s'" % (dated_storage_dir)
        ret = os.system(mkdir_command)
        if ret != 0:
            raise IOError, "Could not make dated output directory %s" % dated_storage_dir

    # Copy the output file to the working directory if it already exists
    storage_path = os.path.join(dated_storage_dir, storage_name)
    work_path = os.path.join(work_dir, storage_name)
    if os.path.exists(storage_path):
        shutil.copyfile(storage_path, work_path)
    else:
        ret = os.system("csh -c 'ncgen %s -o %s'" % (cdl_file, work_path))
        if ret != 0:
            print "Problem making output file"
            raise IOError, "Could not make output file %s" % work_path

    return (work_path, dated_storage_dir)


def order_files(fname, time_interval, cdl_file, work_dir, out_dir, out_name_schema, logg, filter_gte=None):
    """Given a single netcdf input file, extract each record and place it in the appropriate netCDF time interval file. For example, if time_interval = 3600, place records in hourly files.

    Inputs:
      fname -- input file name string
      time_interval -- time interval in seconds. For example, 3600 to create hourly summary files
      cdl_file -- template for output netCDF file
      work_dir -- working directory (must be different from out_dir but should be on the same file system)
      out_dir -- output directory name string
      out_name_schema -- output name schema information
      logg -- log file object
      filter_gte -- filters all data where obs_time >= filter_gte (UNIX time)
      
    Returns:
      list of output file names
      
    Algorithm:

    For each observation in the file:
      Get the observation time.

      Check if the appropriate time interval output file is open. If not, if
      there is a copy of the file in out_dir, copy it to work_dir and
      then open it. if no copy exists in out_dir, create a new file in work_dir.

      Store all the information for the record in the appropriate opened file. Update minDate or maxDate as necessary.

    When finished, close all working files and move them to out_dir.
    """
    try:
        in_nc = Dataset(fname, "r")
    except:
        logg.write_error("unable to open file %s" % fname)
        return
    
    var_table = in_nc.variables

    if len(in_nc.variables["obs_time"]) <= 0:
        logg.write_info("Probe file %s is empty. Skipping it." % fname)
        return
    
    # In particular, get obs_time variable
    obs_time = in_nc.variables["obs_time"][:]
    #logg.write_debug("obs_time count: %d" % len(obs_time))
    if filter_gte:
        time_mask = obs_time >= filter_gte
        obs_time = obs_time[time_mask]
        #logg.write_debug("obs_time count after filter: %d" %len(obs_time))
        
    # Check that the input file is not empty before proceeding
    rec_num = obs_time.shape[0]
    if rec_num == 0:
        logg.write_info("The file %s is empty or filter_gte removed all data" % fname)
        in_nc.close()
        return 

    # Open appropriate work files and store their file pointers and
    # number of records in dictionary
    time_interval_dict = {}
    tim_list = []
    work_file_list = []
    
    for ind, tval in enumerate(obs_time):
        time_interval_time_val = tim.truncate_secs(tval, time_interval)
        # Time value, time interval time value (e.g. hour 00, 01, 02 (in unix time) if time_interval is 3600), index
        tim_list.append((tval, time_interval_time_val, ind))

        if not time_interval_time_val in time_interval_dict:
            dated_out_dir = ""
            (work_file, dated_out_dir) = get_work_file(time_interval_time_val, cdl_file, out_dir, out_name_schema, work_dir)
            work_file_list.append(work_file)
            work_nc = Dataset(work_file, "a")
            work_rec_num = len(work_nc.dimensions["rec_num"])
            time_interval_dict[time_interval_time_val] = (work_nc, work_rec_num)

    # Sort the time list so that the index groups are in sort time
    # order for minimum and maximum seconds calculation
    tim_list.sort()
    
    # Get index groups corresponding to the different time interval files
    index_groups = get_index_groups(tim_list)
    #logg.write_debug("index_groups: %s" % index_groups)
    
    # Now copy each variable into the output files

    # Copy observation data to output files
    var_names = var_table.keys()
    in_vars = in_nc.variables

    for vname in var_names:
        if vname != "obs_time":
            in_var = in_nc.variables[vname][:]
        else:
            in_var = obs_time

        # Get dimension information about the variable
        # Note that in_var_info contains a tuple of dimension
        # name strings
        in_var_info = in_vars[vname].dimensions
        
        if in_var_info == () or in_var_info[0] != "rec_num":
            # The variable is a scalar variable or an array that does
            # not correspond to the unlimited dimension so simply copy
            # source to destination and continue
            put_scalar_var(vname, in_var, time_interval_dict)
        else:
            if filter_gte and vname != "obs_time":
                in_var = in_var[time_mask]
            #logg.write_debug(vname)
            put_array_var(vname, in_var, time_interval_dict, index_groups, logg)
            
    # Close all work files
    for (nc_fp, nc_rec_num) in time_interval_dict.values():
        nc_fp.close()

    # Move all work files back to output directory
    out_file_list = []
    for work_file in work_file_list:
        out_file = os.path.basename(work_file)
        dir_date = out_name_schema.get_date(out_file)
        dated_out_dir = os.path.join(out_dir, dir_date)
        if not os.path.exists(dated_out_dir):
            mkdir_command = "csh -c 'mkdir %s'" % (dated_out_dir)
            ret = os.system(mkdir_command)
            if ret != 0:
                raise IOError, "Could not make dated output directory %s" % dated_out_dir
        out_file_list.append(out_file)
        os.rename(work_file, os.path.join(dated_out_dir, out_file))
                  
    return out_file_list

def group_probe_message_files(fname, time_interval, cdl_file, work_dir, out_dir, out_name_schema, logg, filter_gte=None):

    #print fname

    if not os.path.exists(work_dir):
        logg.write_error("working directory %s does not exist" % work_dir)
        exit_value = 1
    elif not os.path.exists(out_dir):
        logg.write_error("output directory %s does not exist" % out_dir)
        exit_value = 1
    elif not os.path.exists(cdl_file):
        logg.write_error("cdl file %s does not exist" % cdl_file)
        exit_value = 1
    else:
        logg.write_info("beginning to order file %s by obs time" % fname)
        out_file_list = order_files(fname, time_interval, cdl_file, work_dir, out_dir, out_name_schema, logg, filter_gte)

        if out_file_list != None:
            for out_file in out_file_list:
                logg.write_info("created and/or modified %s" % os.path.join(out_dir, datetime.datetime.now().strftime("%Y%m%d"), out_file))
        exit_value = 0
    
    return exit_value      

def main():

    usage_str = "%prog config cdl_file work_dir in_dir(s) out_dir time_interval name_schema\n\n-time_interval is in seconds\n-name_schema should be a name found in the config file (i.e. umtri, nevada, wxt)"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-d", "--days_lookback", dest="days_lookback", help="lookback specified number of days")
    parser.add_option("-b", "--begin_date", dest="begin_date", help="start at particular date yyyymmddhh")
    parser.add_option("-e", "--end_date", dest="end_date", help="end at particular date yyyymmddhh")
    parser.add_option("-D", "--date", dest="user_date", help="replace current time with a particular date in the form yyyymmddhh")
    parser.add_option("-f", "--filter_gte", dest="filter_gte", help="filters all data records with obs_time >= yyyymmdd[hh[mm[ss]]]")

    (options, args) = parser.parse_args()

    if len(args) < 7:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    cdl_file = args[1]
    work_dir = args[2]
    in_dir_list = args[3:-3]
    out_dir = args[-3]
    time_interval = int(args[-2])
    name_schema_base = args[-1]
    
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])

    days_lookback = 0
    if options.days_lookback:
        days_lookback = int(options.days_lookback)
        
    user_date = -1
    if options.user_date:
        user_date = tim.datetogmt(options.user_date)

    filter_gte = None
    if options.filter_gte:
        filter_gte = tim.datetogmt(options.filter_gte)
        
    begin_date = None
    end_date = None
    
    if options.begin_date:
        begin_date = tim.datetogmt(options.begin_date)
    if options.end_date:
        end_date = tim.datetogmt(options.end_date)

    if (begin_date != None and end_date == None) or (begin_date == None and end_date != None):
        print "Error: both begin_date and end_date must be set or neither should be set"
        sys.exit(2)

    logg.set_suffix("%s" % cf.LOG_PY_SUFFIX)
    logg.write_starting()

    name_format = "%s_format" % name_schema_base
    name_patterns = "%s_patterns" % name_schema_base
    out_name_schema = name_schema.Name_schema(getattr(cf, name_format), getattr(cf, name_patterns))

    file_list = []

    for index, in_dir in enumerate(in_dir_list):
    
        if begin_date != None:
            for update_time in xrange(begin_date, end_date, 86400):
                date = time.strftime("%Y%m%d", time.gmtime(update_time))
                glob_path = os.path.join(in_dir, "%s/*nc" % (date))
                file_list.extend(glob.glob(glob_path))
        elif days_lookback == 0:
            curr_time = time.time()
            if user_date != -1:
                curr_time = user_date
            date = time.strftime("%Y%m%d", time.gmtime(curr_time))
            glob_path = os.path.join(in_dir, "%s/*.nc" % date)
            file_list = glob.glob(glob_path)
        else:
            curr_time = time.time()
            if user_date != -1:
                curr_time = user_date
            
            for day in range(days_lookback):
                date = time.strftime("%Y%m%d", time.gmtime(curr_time - day * 86400))
                glob_path = os.path.join(in_dir, "%s/*nc" % (date))
                file_list.extend(glob.glob(glob_path))    

    final_ret = 0

    for fname in file_list:

        ret_value = group_probe_message_files(fname, time_interval, cdl_file, work_dir, out_dir, out_name_schema, logg, filter_gte)
        if ret_value != 0:
            logg.write_warning("problem occurred while processing %s" % fname)
            final_ret = ret_value
        else:
            logg.write_info("successfully finished processing file %s" % fname)
            
    logg.write_ending(final_ret, "group_probe_message_files.py")
    sys.exit(final_ret)
    
if __name__ == "__main__":

   main()
