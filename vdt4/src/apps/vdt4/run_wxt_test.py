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

""""""

import os
import time
import tim

Road_segment_file = "/d1/gerry/wxt_test/20130214/I81_segments.nc"

def wxt_file_parse(fname):
    first_period = fname.find(".")
    base_name = fname[0:first_period]
    second_period = first_period + 9
    yyyymmdd = fname[first_period+1:second_period]
    hh = fname[second_period+1:second_period+3]
    return (base_name, yyyymmdd, hh)

def process(base_dir, out_dir):
    for date in sorted(os.listdir(base_dir)):
        subdir = os.path.join(base_dir, date)
        for fname in sorted(os.listdir(subdir)):
            file_path = os.path.join(base_dir, subdir, fname)
            (base_name, yyyymmdd, hh) = wxt_file_parse(fname)
            base_time = tim.datetogmt(yyyymmdd+hh)
            begin_time = base_time - 1800
            end_time = base_time + 1800
            begin_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(begin_time))
            end_time_string = time.strftime("%Y%m%d%H%M", time.gmtime(end_time))
            input_probe_message_file = file_path

            probe_message_qc_file_name = "%s.%s.%s.qc.nc" % (base_name, yyyymmdd, hh)
            grid_file_name = "%s.%s.%s.grid.nc" % (base_name, yyyymmdd, hh)
            stat_file_name = "%s.%s.%s.stat.nc" % (base_name, yyyymmdd, hh)
            assess_file_name = "%s.%s.%s.assess.nc" % (base_name, yyyymmdd, hh)

            output_probe_message_qc_file = os.path.join(out_dir, probe_message_qc_file_name)
            output_grid_file = os.path.join(out_dir, grid_file_name)
            output_stat_file = os.path.join(out_dir, stat_file_name)
            output_assess_file = os.path.join(out_dir, assess_file_name)
           
            command_template = "./vdt4 -g %s -n vdt_config.cfg %s %s %s %s %s %s %s" % (output_grid_file, begin_time_string, end_time_string, Road_segment_file, input_probe_message_file, output_probe_message_qc_file, output_stat_file, output_assess_file)
            os.system(command_template)
            
def main():

    base_dir = "/d2/gerry/wxt_obs"
    out_dir = "/d2/gerry/wxt_obs_output"
    process(base_dir, out_dir)

if __name__ == "__main__":

   main()
