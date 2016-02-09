#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_vdt4_mn.py,v $                                          #
#       Version: $Revision: 1.9 $  Dated: $Date: 2013-11-14 18:37:21 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import vii_paths
import time
import datetime
import string
import glob
import tim

# 20 minute max time for radar data
MAX_RADAR_TIME_DIFF_SEC = 1200
MAX_HISTORY_TIME_DIFF_SEC = 7200


def get_radar_file(file_prefix, end_time_string, dated_radar_dir, day_radar, hhmm_radar,logg):
    # If the radar file we're looking for doesn't exist, look back
    # 20 minutes and get the most recent file within that 20 minutes
    # otherwise, don't use radar data
    radar_fname_path = ""
    check_radar_fname = "%s.2d.merged.%s.%s.nc" % (file_prefix,day_radar, hhmm_radar)
    check_radar_fpath = os.path.join(dated_radar_dir, check_radar_fname)
    logg.write_info("checking for radar data in %s" % check_radar_fpath)
    if os.path.exists(check_radar_fpath):
        radar_fname_path = "%s.2d.merged.%s.%s.nc" % (file_prefix, day_radar, hhmm_radar)
        logg.write_info("radar path %s exists" % check_radar_fpath)
    else:
        file_name_string = "%s.2d.merged.%s.*.nc" % (file_prefix, day_radar)
        glob_path = os.path.join(dated_radar_dir, file_name_string)
        glob_list = glob.glob(glob_path)
        file_list = []
        if glob_list != []:
            file_list = sorted(glob_list)
            latest_file = file_list[-1]
            split_file_list = latest_file.split(".")
            latest_radar_time = "%s%s" % (split_file_list[3], split_file_list[4])
            unix_end_time = tim.date2sec(end_time_string)
            unix_latest_radar_time =  tim.date2sec(latest_radar_time)
            if ((unix_end_time - unix_latest_radar_time) < MAX_RADAR_TIME_DIFF_SEC):
                radar_fname_path = "%s.2d.merged.%s.%s.nc" % (file_prefix, split_file_list[3], split_file_list[4])
                logg.write_info("found radar fname path %s" % radar_fname_path)
            else:
                logg.write_info("Radar data is too old.  Latest files available are %s.2d.merged.%s.%s.nc" % (file_prefix, split_file_list[3], split_file_list[4]))
                radar_fname_path = ""

    return radar_fname_path

                
def main():

    usage_str = "%prog"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-d", "--date", dest="date", help="date")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 0:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage(vii_paths.Run_vdt4_mn_log_file, "pyl")

    # Get the begin time and end time
    begin_time = time.mktime(time.gmtime()) - 300
    if options.date:
        begin_time = time.mktime(time.strptime(options.date,"%Y%m%d.%H%M"))
    begin_time = begin_time - (begin_time % 300)
    #print begin_time
    end_time = begin_time + 300

    # Get the radar data times
    begin_time_radar = time.mktime(time.gmtime()) - 240
    if options.date:
        begin_time_radar = time.mktime(time.strptime(options.date,"%Y%m%d.%H%M"))
    begin_time_radar = begin_time_radar - (begin_time_radar % 120)
    end_time_radar = begin_time_radar + 120
    begin_time_tup_radar = time.gmtime(begin_time_radar)
    day_radar = time.strftime("%Y%m%d", begin_time_tup_radar)
    hhmm_radar = time.strftime("%H%M", begin_time_tup_radar)

    # Get previous hour and/or day for model data
    model_time = begin_time - 3600
    model_time_tup = time.gmtime(model_time)
    model_day = time.strftime("%Y%m%d", model_time_tup)
    model_hhmm = time.strftime("%H%M", model_time_tup)

    # Convert begin time and end time
    begin_time_tup = time.gmtime(begin_time)
    day = time.strftime("%Y%m%d", begin_time_tup)
    hhmm = time.strftime("%H%M", begin_time_tup)

    end_time_tup = time.gmtime(end_time)
    end_day = time.strftime("%Y%m%d", end_time_tup)
    end_hhmm = time.strftime("%H%M", end_time_tup)
    begin_time_string = day + hhmm
    end_time_string = end_day + end_hhmm
    #print begin_time
    #print end_time
    #print model_day, model_hhmm

    logg.write_starting()
    logg.write_info("Executing: %s" % " ".join(sys.argv))

    # Prepare file names and paths
    dated_radar_dir = os.path.join(vii_paths.RADAR_DIR, day_radar)
    dated_radar_cref_dir = os.path.join(vii_paths.RADAR_CREF_DIR, day_radar)
    radar_fname_path = get_radar_file('BASE_REFL', end_time_string, dated_radar_dir, day_radar, hhmm_radar, logg)
    radar_cref_fname_path = get_radar_file('COMP_REFL', end_time_string, dated_radar_cref_dir, day_radar, hhmm_radar, logg)
   
    # If the history file we're looking for doesn't exist, look back
    # one hour and get the most recent file within that one hour
    # otherwise don't use the history file
    # MAX_HISTORY_TIME_DIFF_SEC is currently set to 7200 or two hours
    # because the HHMM on the file is the begin history time so we need
    # to look back "two hours".
    history_dir = vii_paths.MN_HISTORY_DIR
    history_fname_path = ""
    check_history_fname = "history_probe_msg_qc.%s.%s.60.nc" % (day, hhmm)
    check_history_fpath = os.path.join(history_dir, check_history_fname)
    if os.path.exists(check_history_fpath):
        history_fname_path = "history_probe_msg_qc.%s.%s.60.nc" % (day, hhmm)
    else:
        file_name_string = "history_probe_msg_qc.%s.*.60.nc" % (day)
        glob_path = os.path.join(history_dir, file_name_string)
        glob_list = glob.glob(glob_path)
        file_list = []
        if glob_list  != []:
            file_list = sorted(glob_list)
            latest_file = file_list[-1]
            split_file_list = latest_file.split(".")
            latest_history_time = "%s%s" % (split_file_list[1], split_file_list[2])
            unix_end_time = tim.date2sec(end_time_string)
            unix_latest_history_time = tim.date2sec(latest_history_time)
            if ((unix_end_time - unix_latest_history_time) < MAX_HISTORY_TIME_DIFF_SEC):
                history_fname_path = "history_probe_msg_qc.%s.%s.60.nc" % (split_file_list[1], split_file_list[2])
            else:
                logg.write_info("History data is too old.  Latest file available is history_probe_msg_qc.%s.%s.60.nc" % (split_file_list[1], split_file_list[2]))
                history_fname_path = ""

    rtma_fname = "rtma2p5.t%sz.2dvarges_ndfd.nc" % (model_hhmm[:2])
    cloud_fname = "cloud.%s.%s.nc" % (day, hhmm)
    metar_fname = "metar.%s.%s.nc" % (day, hhmm)
    rwis_fname = "madis.%s.%s00.nc" % (day, hhmm[0:2])

    # truncate to the nearest 15 minutes
    probe_message_fname_time = tim.truncate_secs(begin_time, 900)
    #probe_time_tup = time.gmtime(probe_message_fname_time)
    probe_time_tup = time.gmtime(begin_time)
    #print "probe_time_tup: ", probe_time_tup
    probe_full_date = time.strftime("%Y%m%d%H%M", probe_time_tup)
    logg.write_info("probe_full_date, begin_time, probe_message_fname_time: %s, %ld, %ld" % (probe_full_date, begin_time, probe_message_fname_time))
    

    probe_day = probe_full_date[0:8]
    probe_hhmm = probe_full_date[8:12]

    probe_message_fname = "probe_message.%s.%s.nc" % (probe_day, probe_hhmm)
    #probe_message_fname = "vdt.%s.%s.nc" % (probe_day, probe_hhmm)

    if not (radar_fname_path == ""):
        radar_fpath= os.path.join(dated_radar_dir, radar_fname_path)
    else:
        radar_fpath = ""

    if not (radar_cref_fname_path == ""):
        radar_cref_fpath= os.path.join(dated_radar_cref_dir, radar_cref_fname_path)
    else:
        radar_cref_fpath = ""
        
    rtma_fpath = os.path.join(vii_paths.RTMA_NC_DIR, model_day, rtma_fname)

    if not (history_fname_path == ""):
        history_fpath = os.path.join(history_dir, history_fname_path)
    else:
        history_fpath = ""

    #print rtma_fpath
    # /d2/vii/data/raw/rtma/20130315/rtma.t20z.2dvarges_ndfd.grb2
    cloud_fpath = os.path.join(vii_paths.CLOUD_MASK_DIR, day, cloud_fname)
    #print cloud_fpath
    # /d2/vii/data/processed/satellite/cloud_mask/netcdf/blend/20130315/cloud.20130315.2035.nc
    metar_fpath = os.path.join(vii_paths.METAR_DIR, day, metar_fname)
    rwis_fpath = os.path.join(vii_paths.RWIS_DIR, day, rwis_fname)
    if not os.path.exists(rwis_fpath):
        rwis_fname = "madis.%s.%s00.nc" % (model_day, model_hhmm[0:2])
        rwis_fpath = os.path.join(vii_paths.RWIS_DIR, model_day, rwis_fname)

    # /d2/ldm/data/dec_data/obs/madis/20130315/madis.20130315.2000.nc
    #in_dir = os.path.join(vii_paths.PROCESSED_DIR, "mn_combined_probe")
    in_dir = os.path.join(vii_paths.PROCESSED_DIR, "mn_probe_message")
    probe_message_input_file = os.path.join(in_dir, day, probe_message_fname)
    out_dir = os.path.join(vii_paths.PROCESSED_DIR, "mn_vdt_output")
    out_day_dir = os.path.join(out_dir, day)
    qc_out_fname = "vdt.%s.%s.nc" % (day, hhmm)
    qc_out_fpath = os.path.join(out_day_dir, qc_out_fname)
    log_dir = vii_paths.LOG_DIR
    log_file = "%s/vdt4_mn" % log_dir
    seg_file = os.path.join(vii_paths.CDL_DIR, "mn_roads.nc")
    #config_file = os.path.join(vii_paths.CONFIG_DIR, "mn_vdt_config.cfg")
    #config_file = os.path.join(vii_paths.CONFIG_DIR, "vdt_mn_config.cfg")
    #Command for testing the new vdt with dynamic configurability
    config_file = os.path.join(vii_paths.CONFIG_DIR, "vdt_mn_config_new.cfg")
    segment_statistics_out_fname = "segment_statistics.%s.%s.nc" % (day, hhmm)
    segment_statistics_out_fpath = os.path.join(out_day_dir, segment_statistics_out_fname)
    grid_cell_statistics_out_fname = "grid_cell_statistics.%s.%s.nc" % (day, hhmm)
    grid_cell_statistics_out_fpath = os.path.join(out_day_dir, grid_cell_statistics_out_fname)

    if not os.path.exists(out_day_dir):
        cmd = "mkdir -p %s" % out_day_dir
        logg.write_info(cmd)
        os.system(cmd)
    opts = {"-m" : (metar_fpath,True),
            "-r" : (radar_fpath,False),
            "-p" : (radar_cref_fpath,False),
            "-w" : (rwis_fpath,True),
            "-c" : (cloud_fpath,True),
            "-a" : (rtma_fpath,True)}
    
    cmd = ["vdt45", "-s", "-l %s" % log_file]
    for opt in opts.keys():
        (fpath,check) = opts[opt]
        #print fpath, check
        if check and not os.path.exists(fpath):
            logg.write_info("skipping option %s, %s not found" % (opt, fpath))
        elif not check:
            # Added for radar data which passes in a pattern not the exact filename
            if (fpath == radar_fpath):
                if not radar_fpath == "":
                    cmd.append("%s %s" % (opt, fpath))
            elif (fpath == radar_cref_fpath):
                if not radar_cref_fpath == "":
                    cmd.append("%s %s" % (opt, fpath))
            else:
                cmd.append("%s %s" % (opt, fpath))               
        else:
            cmd.append("%s %s" % (opt, fpath))

    # History file
    if (history_fpath != ""):
        history_option = "-H %s" % (history_fpath)
        cmd.append(history_option)

    # Grid cell statistics file
    grid_cell_statistics_output = "-g %s" % grid_cell_statistics_out_fpath
    cmd.append(grid_cell_statistics_output)

    # Config file
    cmd.append(config_file)

    # Begin time 
    cmd.append(begin_time_string)

    # End time 
    cmd.append(end_time_string)

    # Road segment file
    cmd.append(seg_file)

    # Probe message file
    cmd.append(probe_message_input_file)

    # Probe message qc'd output file
    cmd.append(qc_out_fpath)

    # Prboe message qc statistics output file
    cmd.append(segment_statistics_out_fpath)

    cmd = string.join(cmd," ")

    # Sleep to allow for the input probe message file to arrive
    time.sleep(30)

    # Execute command
    print "cmd: %s\n" % cmd
    logg.write_info("Executing: %s" % cmd)
    ret = os.system(cmd)
    ret = 0
    logg.write_ending(ret)
    return ret

if __name__ == "__main__":

   sys.exit(main())
