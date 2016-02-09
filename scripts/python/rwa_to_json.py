#!/usr/bin/env python

import data_time_index
import log_msg
import os
import time
from optparse import OptionParser
import sys
import subprocess
import calendar
import errno
import processed_file

error_ret_flag = 0

#LOOKBACK_INTERVAL_SEC = 1800
LOOKBACK_INTERVAL_SEC = 600

LOOKBACK_STATES = {"mi", "mn"}

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

def run_rwa_to_json(yyyymmdd, hhmm, state_object, cf, logg):
    
    file_time = yyyymmdd + "." + hhmm
    logg.write_info("file_time: %s %s" % (yyyymmdd, hhmm))

    state = cf.state_full_to_abbrev[state_object.state]
    nc_fname = "%s_rwh.%s.%s.nc" % (state, yyyymmdd, hhmm)

    district_alerts_json_fname = "%s.%s.%s.json" % (cf.district_alerts_base_name, yyyymmdd, hhmm)
    plots_json_fname = "%s.%s.%s" % (cf.plots_base_name, yyyymmdd, hhmm)

    full_input_dir = os.path.join(state_object.rwh_dir, yyyymmdd)
    input_path = os.path.join(full_input_dir, nc_fname)
    
    # segment_statistics.20140115.2045.nc
    obs_stats_input_fname = "segment_statistics.%s.%s.nc" % (yyyymmdd, hhmm)
    obs_stats_input_dir = os.path.join(state_object.obs_stats_dir, yyyymmdd)
    obs_stats_input_path = os.path.join(obs_stats_input_dir, obs_stats_input_fname)

    # obs_stats_data.20140114.2045    
    obs_stats_output_fname = "obs_stats_data.%s.%s.json" % (yyyymmdd, hhmm)
    obs_stats_output_dir = os.path.join(state_object.obs_stats_json_dir)
    obs_stats_output_subdir = os.path.join(obs_stats_output_dir, yyyymmdd)
    mkdir_path(obs_stats_output_subdir)
    obs_stats_output_path = os.path.join(obs_stats_output_subdir, obs_stats_output_fname)

    # mwa.20140114.2045.json
    maw_json_output_fname = "maw.%s.%s.json" % (yyyymmdd, hhmm)
    maw_json_output_dir = os.path.join(state_object.maw_json_dir)
    maw_json_output_subdir = os.path.join(maw_json_output_dir, yyyymmdd)
    mkdir_path(maw_json_output_subdir)
    maw_json_output_path = os.path.join(maw_json_output_subdir, maw_json_output_fname)

    district_alerts_dir = ""
    district_alerts_subdir = ""
    site_list_file = ""
    district_alerts_dir = state_object.district_alerts_dir
    district_alerts_subdir = os.path.join(district_alerts_dir, yyyymmdd)
    site_list_file = state_object.all_sites_file
    mkdir_path(district_alerts_subdir)
    district_alerts_path = os.path.join(district_alerts_subdir, district_alerts_json_fname)

    plots_dir = ""
    plots_subdir = ""
    site_list_file = ""
    plots_dir = state_object.plots_dir
    plots_subdir = os.path.join(plots_dir, yyyymmdd)
    site_list_file = state_object.all_sites_file
    mkdir_path(plots_subdir)
    plots_path = os.path.join(plots_subdir, plots_json_fname)

    config_file = "%s_rwa.cfg" % state
    config_path = os.path.join(cf.config_dir, config_file)

    processed_fname_base = "Processed" 
    
    proc_file = processed_file.ProcessedFile(district_alerts_dir, processed_fname_base)
    plots_proc_file = processed_file.ProcessedFile(plots_dir, processed_fname_base)
    obs_stats_proc_file = processed_file.ProcessedFile(obs_stats_output_dir, processed_fname_base)
    maw_json_proc_file = processed_file.ProcessedFile(maw_json_output_dir, processed_fname_base)

    proc_stat_obj = proc_file.process_status(district_alerts_json_fname, yyyymmdd)
    if proc_stat_obj != "" and proc_stat_obj[0] == processed_file.PROC_SUCCESS:
        return

    # get latest treatment file
    (latest_treatment_file, latest_treatment_file_date) = data_time_index.get_latest_file(state_object.tmt_dir)
    treatment_option = ""
    if latest_treatment_file != "":
        latest_treatment_path = os.path.join(state_object.tmt_dir, latest_treatment_file_date[0:8], latest_treatment_file)
        treatment_option = "-t %s" % latest_treatment_path
        
    if os.path.exists(input_path):
        cmd = "rwa -s %s -l %s/rwa_%s %s %s %s %s %s %s %s %s %s" % (obs_stats_input_path, cf.log_dir, state, treatment_option, file_time, config_path, site_list_file, input_path, district_alerts_path, plots_path, obs_stats_output_path, maw_json_output_path)
        logg.write_info("Attempting: %s" % (cmd))
        ret = run_cmd(cmd, logg)
        if ret == 0:
            proc_file.write_processed(yyyymmdd, district_alerts_json_fname, processed_file.PROC_SUCCESS)
            plots_proc_file.write_processed(yyyymmdd, plots_json_fname, processed_file.PROC_SUCCESS)
            obs_stats_proc_file.write_processed(yyyymmdd, obs_stats_output_fname, processed_file.PROC_SUCCESS)
            maw_json_proc_file.write_processed(yyyymmdd, maw_json_output_fname, processed_file.PROC_SUCCESS)
            logg.write_info("successfully processed: %s" % (input_path))
        else:
            logg.write_error("failed processing: %s" % (input_path))
            return 1
    else:
        logg.write_info("input path %s does not exist" % input_path)

    return 0

def rwa_to_json(state_object, cf, interval_time_sec, logg, options):

    
    if not options.begin_date and not options.end_date:    
        ptime = time.mktime(time.gmtime())
        time_value= ptime - (ptime % interval_time_sec) - interval_time_sec # use begin time interval time
        file_time = time.strftime("%Y%m%d%H%M", time.gmtime(time_value))
        yyyymmdd = file_time[0:8]
        hhmm = file_time[8:12]

        state = cf.state_full_to_abbrev[state_object.state]
        if state in LOOKBACK_STATES:
            lookback_time_value = time_value + interval_time_sec - LOOKBACK_INTERVAL_SEC
            #lookback_time_value = time_value - LOOKBACK_INTERVAL_SEC
            lookback_file_time = time.strftime("%Y%m%d%H%M", time.gmtime(lookback_time_value))
            lookback_yyyymmdd = lookback_file_time[0:8]
            lookback_hhmm = lookback_file_time[8:12]
            run_rwa_to_json(lookback_yyyymmdd, lookback_hhmm, state_object, cf, logg)
        else:
            run_rwa_to_json(yyyymmdd, hhmm, state_object, cf, logg)
        
    else:
        date_list = gen_date_list(options.begin_date, options.end_date)        
        if len(date_list) > 0:
            logg.write_info("last date in date list for processing: %s" % date_list[-1])
        else:
            logg.write_info("no dates in date list for processing")

        for mydate in date_list:
            full_input_dir = os.path.join(state_object.rwh_dir, mydate)
            if (not os.path.exists(full_input_dir)):
                logg.write_info("directory does no exist: %s" % full_input_dir)
                continue
                
            file_list = os.listdir(full_input_dir)
            for myfile in file_list:
                if "rwh" in myfile:
                    split_fname = myfile.split(".")
                    yyyymmdd = split_fname[1]
                    hhmm = split_fname[2]
                    run_rwa_to_json(yyyymmdd, hhmm, state_object, cf, logg)

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

    # Get state object
    full_state = cf.state_abbrev_to_full[state]
    
    state_object = cf.State_dictionary[full_state + "_vdt"]
    logg.write_starting("rwa_to_json.py")

    rwa_to_json(state_object, cf, interval_time_sec, logg, options)
    
    logg.write_ending(error_ret_flag, "rwa_to_json.py")
    

if __name__ == "__main__":
    main()
