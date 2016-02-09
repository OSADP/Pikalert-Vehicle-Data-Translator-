#!/usr/bin/env python

""""""

import os
import time
import tim

Exec = "/d1/gerry/cvs/projects/vdt/apps/probe_message_dataset_manager/probe_message_dataset_manager"
Config = "/d2/vii/data/static/config/vdt_config.cfg"
Road_segment_file = "/d2/vii/data/static/cdl/mn_1_mile_road_segments.nc"

def wxt_file_parse(fname):
    first_period = fname.find(".")
    base_name = fname[0:first_period]
    second_period = first_period + 9
    yyyymmdd = fname[first_period+1:second_period]
    hh = fname[second_period:second_period+2]
    return (base_name, yyyymmdd, hh)

def process(base_dir, out_dir):
    time_increment = 300
    for date in sorted(os.listdir(base_dir)):
        subdir = os.path.join(base_dir, date)
        for fname in sorted(os.listdir(subdir)):
            file_path = os.path.join(base_dir, subdir, fname)
            (base_name, yyyymmdd, hh) = wxt_file_parse(fname)
            if hh < "12":
                continue
            
            base_time = tim.datetogmt(yyyymmdd+hh)
            for begin_time in range(base_time, base_time+3600, time_increment):
                end_time = begin_time + time_increment

                begin_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(begin_time))
                end_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(end_time))
                input_probe_message_file = file_path

                yyyymmdd = begin_time_string[0:8]
                hhmm = begin_time_string[8:12]
                probe_message_qc_file_name = "%s.%s.%s.qc.nc" % (base_name, yyyymmdd, hhmm)
                grid_file_name = "%s.%s.%s.grid.nc" % (base_name, yyyymmdd, hhmm)
                stat_file_name = "%s.%s.%s.stat.nc" % (base_name, yyyymmdd, hhmm)
                assess_file_name = "%s.%s.%s.assess.nc" % (base_name, yyyymmdd, hhmm)

                output_probe_message_qc_file = os.path.join(out_dir, probe_message_qc_file_name)
                output_grid_file = os.path.join(out_dir, grid_file_name)
                output_stat_file = os.path.join(out_dir, stat_file_name)
                output_assess_file = os.path.join(out_dir, assess_file_name)

                command_template = "%s -g %s -n %s %s %s %s %s %s %s %s" % (Exec, output_grid_file, Config, begin_time_string, end_time_string, Road_segment_file, input_probe_message_file, output_probe_message_qc_file, output_stat_file, output_assess_file)
                print command_template
                os.system(command_template)
            
def main():

    base_dir = "/d2/vii/data/wxt_probe_data"
    out_dir = "/d2/vii/data/wxt_probe_data_stats_five_min"
    #out_dir = "/d2/vii/data/wxt_probe_data_stats_surf_obs"
    process(base_dir, out_dir)

if __name__ == "__main__":

   main()
