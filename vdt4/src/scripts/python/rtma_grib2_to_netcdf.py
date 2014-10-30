#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import log_msg
import time
from optparse import OptionParser
import sys
import subprocess
import os
import errno
import processed_file
import tim
import vii_paths

def mkdir_path(path):
    try:
        os.makedirs(path)
    except os.error, e:
        if e.errno != errno.EEXIST:
            raise

def run_cmd(cmd, logg):
    logg.write_info(cmd)
    p = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    (stdout_str, stderr_str) = p.communicate()
    ret = p.returncode
    if ret != 0:
        logg.write_error("failed executing: %s" % cmd)
        logg.write_ending(ret)
    return ret

def rtma_grib2_to_netcdf(yyyymmdd, in_file, in_path, out_base_dir, out_dir, logg):
    
    ret = 0
    period_pos = in_file.rfind('.')
    if period_pos == -1:
        out_file = in_file + ".nc"
    else:
        out_file = in_file[0:period_pos] + ".nc"
        
    processed_fname_base = "Processed" 
    proc_file = processed_file.ProcessedFile(out_base_dir, processed_fname_base)
    proc_stat_obj = proc_file.process_status(out_file, yyyymmdd)
    if proc_stat_obj != "" and proc_stat_obj[0] == processed_file.PROC_SUCCESS:
        return ret
    
    if os.path.exists(in_path):
        cmd = "ncl_convert2nc %s -o %s" % (in_path, out_dir)
        logg.write_info("Attempting: %s" % (cmd))
        ret = run_cmd(cmd, logg)
        if ret == 0:
            proc_file.write_processed(yyyymmdd, out_file, processed_file.PROC_SUCCESS)
            logg.write_info("Successfully processed: %s" % (in_path))
        else:
            logg.write_warning("Problem processing: %s" % (in_path))
            ret = 1
    else:
        logg.write_info("in path %s does not exist" % in_path)

    return ret

def run_rtma_grib2_to_netcdf(options, in_base_dir, out_base_dir, logg):

    total_ret = 0
    
    # rtma.t01z.2dvarges_ndfd.grb2
    date_list = []
    if not options.begin_date and not options.end_date:    
        ptime = time.mktime(time.gmtime())
        yyyymmdd = time.strftime("%Y%m%d", time.gmtime(ptime))
        date_list.append(yyyymmdd)
    else:
        date_list = tim.date_list(options.begin_date, options.end_date)

    logg.write_info("%d date(s) to process" % len(date_list))

    for date in date_list:
        in_dir = os.path.join(in_base_dir, date)
        if (not os.path.exists(in_dir)):
            logg.write_info("directory does no exist: %s" % in_dir)
            continue

        file_list = os.listdir(in_dir)
        logg.write_info("input file list has %d files" % len(file_list))

        for in_file in file_list:
            if "2dvarges_ndfd" in in_file:
                in_path = os.path.join(in_dir, in_file)
                out_dir = os.path.join(out_base_dir, date)
                mkdir_path(out_dir)
                ret = rtma_grib2_to_netcdf(date, in_file, in_path, out_base_dir, out_dir, logg)
                if ret != 0:
                    total_ret = ret
    return total_ret    

def main():

    parser = OptionParser(usage = "%prog")
    parser.add_option("-l","--log",dest="log",help="base name of log file")
    parser.add_option("-b", "--begin_date", dest="begin_date", help="start at particular date yyyymmdd")
    parser.add_option("-e", "--end_date", dest="end_date", help="end at particular date yyyymmdd")
    parser.add_option("-i", "--in_dir", dest="in_dir", help="provide input directory other than the one specified in vii_paths.py")
    parser.add_option("-o", "--out_dir", dest="out_dir", help="provide output directory other than the one specified in vii_paths.py")
    (options,args) = parser.parse_args()

    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    if  (options.begin_date != None and options.end_date == None) or (options.begin_date == None and options.end_date != None):
        print "Error: both begin_date and end_date must be set or neither should be set"
        sys.exit(2)

    in_base_dir = vii_paths.RTMA_DIR
    out_base_dir = vii_paths.RTMA_NC_DIR
    if options.in_dir:
        in_base_dir = options.in_dir
    if options.out_dir:
        out_base_dir = options.out_dir
        
    logg.write_starting("rtma_grib2_to_netcdf.py")
    ret = run_rtma_grib2_to_netcdf(options, in_base_dir, out_base_dir, logg)
    logg.write_ending(msg="rtma_grib2_to_netcdf.py", exit_status=ret)
    

if __name__ == "__main__":
    main()
