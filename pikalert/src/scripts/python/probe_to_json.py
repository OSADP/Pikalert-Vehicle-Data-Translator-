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

import log, log_msg
import os
import time
from optparse import OptionParser
import sys
#import sys_path
import subprocess
#import index_file
import calendar
import os
import errno
import processed_file
import tim

error_ret_flag = 0
LOOKBACK_INTERVAL_SEC = 600

def gen_date_list(begin_date, end_date):
    """Generates a list of dates of the form yyyymmdd from a being date to end date
    Inputs:
      begin_date -- such as "20070101"
      end_date -- such as "20070103"
    Returns:
      date_list -- such as ["20070101","20070102","20070103"]
    """
    begin_tm = time.strptime(begin_date, "%Y%m%d")
    end_tm = time.strptime(end_date, "%Y%m%d")
    begin_tv = calendar.timegm(begin_tm)
    end_tv = calendar.timegm(end_tm)
    date_list = []
    for tv in xrange(begin_tv, end_tv+86400, 86400):
        date_list.append(time.strftime("%Y%m%d", time.gmtime(tv)))
    return date_list                         

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
        sys.exit(1)
    return ret

def run_probe_to_json(yyyymmdd, hhmm, state, cf, logg):
    
    file_time = yyyymmdd + hhmm
    logg.write_info("file_time: %s %s" % (yyyymmdd, hhmm))

    nc_fname = "vdt.%s.%s.nc" % (yyyymmdd, hhmm)
    json_fname = "vdt.%s.%s.json" % (yyyymmdd, hhmm)
    
    full_input_dir = ""
    if state == "mn":
        full_input_dir = os.path.join(cf.Minnesota_vdt_output_dir, yyyymmdd)
    elif state == "mi":
        full_input_dir = os.path.join(cf.Michigan_vdt_output_dir, yyyymmdd)
    elif state == "nv":
        full_input_dir = os.path.join(cf.Nevada_vdt_output_dir, yyyymmdd)
    input_path = os.path.join(full_input_dir, nc_fname)
    
    parent_output_dir = ""
    full_output_dir = ""
    if state == "mn":
        parent_output_dir = cf.Minnesota_latest_vehicles_dir
        full_output_dir = os.path.join(cf.Minnesota_latest_vehicles_dir, yyyymmdd)
    elif state == "mi":
        parent_output_dir = cf.Michigan_latest_vehicles_dir
        full_output_dir = os.path.join(cf.Michigan_latest_vehicles_dir, yyyymmdd)
    elif state == "nv":
        parent_output_dir = cf.Nevada_latest_vehicles_dir
        full_output_dir = os.path.join(cf.Nevada_latest_vehicles_dir, yyyymmdd)
    mkdir_path(full_output_dir)
    output_path = os.path.join(full_output_dir, json_fname)
    
    config_file = "%s_cdf_config.cfg" % state
    config_path = os.path.join(cf.config_dir, config_file)
    
    processed_fname_base = "Processed" 
    
    proc_file = processed_file.ProcessedFile(parent_output_dir, processed_fname_base)

    proc_stat_obj = proc_file.process_status(json_fname, yyyymmdd)
    if proc_stat_obj != "" and proc_stat_obj[0] == processed_file.PROC_SUCCESS:
        return
    
    if os.path.exists(input_path):
        cmd = "probe_to_json -l %s/probe_to_json_%s %s %s %s %s" % (cf.log_dir, state, file_time, config_path, input_path, output_path)
        logg.write_info("Attempting: %s" % (cmd))
        ret = run_cmd(cmd, logg)
        if ret == 0:
            proc_file.write_processed(yyyymmdd, json_fname, processed_file.PROC_SUCCESS)
            logg.write_info("Successfully processed: %s" % (input_path))
        else:
            error_ret_flag = 1
            logg.write_error("Problem processing: %s" % (input_path))
    else:
        logg.write_info("input path %s does not exist" % input_path)

def probe_to_json(state, cf, interval_time_sec, logg, options):

    if not options.begin_date and not options.end_date:    
        ptime = time.mktime(time.gmtime())
        if state == "mi":
            ptime = ptime - LOOKBACK_INTERVAL_SEC + interval_time_sec
        time_value= ptime - (ptime % interval_time_sec) - interval_time_sec
        
        file_time = time.strftime("%Y%m%d%H%M", time.gmtime(time_value))
        yyyymmdd = file_time[0:8]
        hhmm = file_time[8:12]
        run_probe_to_json(yyyymmdd, hhmm, state, cf, logg)
        
    else:
        date_list = gen_date_list(options.begin_date, options.end_date)        
        if len(date_list) > 0:
            logg.write_info("last date in date list for processing: %s" % date_list[-1])
        else:
            logg.write_info("no dates in date list for processing")

        for mydate in date_list:
            full_input_dir = ""
            if state == "mn":
                full_input_dir = os.path.join(cf.Minnesota_vdt_output_dir, mydate)
            elif state == "mi":
                full_input_dir = os.path.join(cf.Michigan_vdt_output_dir, mydate)
            elif state == "nv":
                full_input_dir = os.path.join(cf.Nevada_vdt_output_dir, mydate)
            if (not os.path.exists(full_input_dir)):
                logg.write_info("directory does no exist: %s" % full_input_dir)
                continue
                
            file_list = os.listdir(full_input_dir)
            for myfile in file_list:
                if "vdt" in myfile:
                    split_fname = myfile.split(".")
                    yyyymmdd = split_fname[1]
                    hhmm = split_fname[2]
                    run_probe_to_json(yyyymmdd, hhmm, state, cf, logg)                                       
    
def main():

    parser = OptionParser(usage = "%prog [options] config state_abbreviation interval_time_sec\n\tNote: state_abbreviation should be in lower case e.g. mn, mi, nv")
    parser.add_option("-l","--log",dest="log",help="base name of log file")
    parser.add_option("-b", "--begin_date", dest="begin_date", help="start at particular date yyyymmdd")
    parser.add_option("-e", "--end_date", dest="end_date", help="end at particular date yyyymmdd")
    (options,args) = parser.parse_args()

    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    config = args[0]
    state = args[1]
    interval_time_sec = args[2]
    interval_time_sec = int(interval_time_sec)

    if  (options.begin_date != None and options.end_date == None) or (options.begin_date == None and options.end_date != None):
        print "Error: both begin_date and end_date must be set or neither should be set"
        sys.exit(2)

    cf = __import__(config, globals(), locals(), [])

    logg.write_starting("probe_to_json.py")
    probe_to_json(state, cf, interval_time_sec, logg, options)
    logg.write_ending(error_ret_flag, "probe_to_json.py")
    

if __name__ == "__main__":
    main()
