#!/usr/bin/env python

"""Run simulate_probe"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import numpy
import os
import sys
from optparse import OptionParser
import time
import tim

class MetData(object):

    def __init__(self, data_dir, template, update_period, max_time_delta):

        self.data_dir = data_dir
        self.template = template
        self.update_period = update_period
        self.max_time_delta = max_time_delta

# Set the following to directories where the archived met data sets are located
# Include: Directory, file name pattern, update time period in seconds, max time delta
Madis_data = MetData("/d4/aander/topr5_data/madis", "madis.%s.%s.nc", 3600, 1800)
Metar_data = MetData("/d4/aander/topr5_data/metar", "metar.%s.%s.nc", 300, 1800)
Radar_tile_data = MetData("/d4/aander/topr5_data/radar", "radar.2d.tile%%d.%s.%s.nc", 120, 1800)
Rtma_data = MetData("/d4/gerry/rtma", "rtma.%s.%s.nc", 3600, 1800)
Rtma2p5_data = MetData("/d4/gerry/rtma2p5", "rtma2p5.%s.%s.nc", 3600, 1800)

Max_time_diff = 1800

Nssl_date_change = "201504010000"

class FileTimeSelector(object):

    def __init__(self, base_dir):

        self.base_dir = base_dir
        self.file_list = sorted(os.listdir(base_dir))
        self.file_times = []
        self.file_time_strings = []
        for fname in self.file_list:
            yyyymmdd = fname[-16:-8]
            hhmm = fname[-7:-3]
            date_time = yyyymmdd + hhmm
            file_time = tim.datetogmt(date_time)
            self.file_times.append(file_time)
            self.file_time_strings.append(date_time)
            
    def get_file_times(self):
        return self.file_times
    
    def get_nearest_indices(self, time_list):

        # first get sort positions
        indices = numpy.searchsorted(self.file_times, time_list, side='left')
        nearest_indices = []
        time_deltas = []
        for ind, value in enumerate(indices):
            if value == len(self.file_times):
                left_difference = abs(self.file_times[value-1] - time_list[ind])
                nearest_indices.append(value-1)
                time_deltas.append(left_difference)
                
            elif 1 <= value:
                left_difference = abs(self.file_times[value-1] - time_list[ind])
                right_difference = abs(self.file_times[value] - time_list[ind])
                if left_difference <= right_difference:
                    nearest_indices.append(value-1)
                    time_deltas.append(left_difference)
                else:
                    nearest_indices.append(value)
                    time_deltas.append(right_difference)

            else:
                nearest_indices.append(value)
                right_difference = abs(self.file_times[value] - time_list[ind])
                time_deltas.append(right_difference)
                    
        return (nearest_indices, time_deltas)

    def get_time_deltas(self, indices, time_list):
        deltas = abs(numpy.array(self.file_times)[indices] - numpy.array(time_list))
        return deltas
        
    def get_closest_file(self, time_index):

        ind = -1
        if time_index < 0 or len(self.file_times) == 0 or time_index > len(self.file_times):
            return None
        elif time_index < len(self.file_times):
            return (os.path.join(self.base_dir, self.file_list[time_index]), self.file_times[time_index], self.file_time_strings[time_index])
        else:
            ind = time_index - 1
            return (os.path.join(self.base_dir, self.file_list[ind]), self.file_times[ind], self.file_time_strings[ind])

    
Met_data_dict = {
    "metar" : Metar_data,
    "madis" : Madis_data,
    "radar_tile" : Radar_tile_data,
    "rtma2p5" : Rtma2p5_data,
    "rtma" : Rtma_data
    }


def create_radar_template(actual_radar_file_name):
    """Create radar template file name from actual radar file name

    Parameters
    ----------
    actual_radar_file_name : string

    Returns
    -------
    radar_file_name_template : string or empty string

    """

    # Search for the word "tile". If not found, return empty string.
    # Otherwise, replace with "%%d".
    #
    # For example:
    # radar.2d.tile1.20140531.0500.nc -> radar.2d.tile%d.20140531.0500.nc

    pos = actual_radar_file_name.find("tile")
    if pos == -1:
        return ""

    out_string = "%stile%%d%s" % (actual_radar_file_name[0:pos], actual_radar_file_name[pos+5:])
    return out_string
    

def simulation(wipers_only, density, config_file, data_dict, update_period, begin_date, end_date, output_dir, logg):
    """
    Parameters
    ----------
    wipers_only : true - only do wipers processing, false - do standard simulation
    density : 0 - less dense, 1 - more dense
    config_file : config file name string
    data_dict : data dictionary
    begin_date : begin date string
    end_date : end date string
    update_period : update period in seconds
    Returns
    -------
    err_code : int
    	     0 on success, -1 on failure
    """

    # Generate underlying hourly date list using time_delta seconds for the update period
    time_list = []
    logg.write_info("begin date, end date, update_period: %s, %s, %d" % (begin_date, end_date, update_period))
    time_list = tim.utc_time_list_delta(begin_date, end_date, update_period)

    # Goal: create file sets consisting of files for each data type for each time listed in time_list
    # For each data type in data_dict read in file list and create dictionary according to time
    # Formulate madis, rwis, radar, rtma file lists based on date/time

    logg.write_info("preparing simulation data file lists")
    simulation_files = {}
    file_count_dict = {}

    for data_type in sorted(Met_data_dict.keys()):
        met_data_obj = Met_data_dict[data_type]
        fts = FileTimeSelector(met_data_obj.data_dir)
        (time_indices, time_deltas) = fts.get_nearest_indices(time_list)

        max_time_delta = met_data_obj.max_time_delta

        # Create the file list for this data type for the simulation
        simulation_file_list = []
        actual_file_count = 0
        for ind, time_delta in enumerate(time_deltas):
            file_name = ""
            if time_delta <= max_time_delta:
                if data_type == "radar_tile":
                    # do special handling for radar tiles of the old vintage prior to April 2015,
                    file_time_string = fts.file_time_strings[time_indices[ind]]
                    if file_time_string > Nssl_date_change:
                        logg.write_error("simulator cannot handle radar files after April 2015 at this time")
                        return -1

                    file_name = create_radar_template(fts.file_list[time_indices[ind]])
                else:
                    file_name = fts.file_list[time_indices[ind]]

                simulation_file_list.append("%s\n" % os.path.join(met_data_obj.data_dir, file_name))
                actual_file_count += 1
            else:
                simulation_file_list.append("\n")

        file_count_dict[data_type] = actual_file_count
        
        # Make the output directory if necessary
        if not os.path.isdir(output_dir):
            os.makedirs(output_dir)
            
        # Output the file list
        out_file_path = os.path.join(output_dir, "%s.%s-%s.txt" % (data_type, begin_date, end_date))

        simulation_files[data_type] = out_file_path
        with open(out_file_path, "w") as fp:
            fp.writelines(simulation_file_list)
            
    # Create the simulation command

    # Example command: simulate_probe -m metar_files.txt -w madis_files.txt -a rtma_files.txt -r radar_files.txt  mn_simulation.cfg 300 201412142355 201412150000 probe_message.txt csv_out_file.csv
    probe_message_file = os.path.join(output_dir, "probe_message")
    road_data_file = os.path.join(output_dir, "road_data")
    wipers_only_string = ""
    if wipers_only:
        wipers_only_string = "-W"

    # Try to use rtma2p5 if possible otherwise fall back to older rtma files of lower resolution
    if file_count_dict["rtma2p5"] > 0:
        command = "simulate_probe %s -c %d -m %s -w %s -r %s -a %s %s %d %s %s %s %s" % (wipers_only_string, density, simulation_files["metar"], simulation_files["madis"], simulation_files["radar_tile"], simulation_files["rtma2p5"], config_file, update_period, begin_date, end_date, probe_message_file, road_data_file)
    else:
        command = "simulate_probe %s -c %d -m %s -w %s -r %s -a %s %s %d %s %s %s %s" % (wipers_only_string, density, simulation_files["metar"], simulation_files["madis"], simulation_files["radar_tile"], simulation_files["rtma"], config_file, update_period, begin_date, end_date, probe_message_file, road_data_file)
        
    logg.write_info("executing simulation command: %s" % command)
    ret = os.system(command)
    if ret != 0:
        logg.write_error("command failed: %s" % command)
        return 1
    else:
        logg.write_info("command succeeded: %s" % command)
        return 0

def main():

    usage1 = "Standard simulation examples:\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201405300500 201406010500 /d4/gerry/simulation_dir/simulation_may_mn_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201405300500 201406010500 /d4/gerry/simulation_dir/simulation_may_mn_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201405300500 201406010500 /d4/gerry/simulation_dir/simulation_may_mi_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201405300500 201406010500 /d4/gerry/simulation_dir/simulation_may_mi_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201404050000 201404070000 /d4/gerry/simulation_dir/simulation_april_mn_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201404050000 201404070000 /d4/gerry/simulation_dir/simulation_april_mn_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201404050000 201404070000 /d4/gerry/simulation_dir/simulation_april_mi_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201404050000 201404070000 /d4/gerry/simulation_dir/simulation_april_mi_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201403301200 201404011200 /d4/gerry/simulation_dir/simulation_march_mn_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201403301200 201404011200 /d4/gerry/simulation_dir/simulation_march_mn_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201403301200 201404011200 /d4/gerry/simulation_dir/simulation_march_mi_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201403301200 201404011200 /d4/gerry/simulation_dir/simulation_march_mi_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201402240000 201402260000 /d4/gerry/simulation_dir/simulation_feb_mn_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201402240000 201402260000 /d4/gerry/simulation_dir/simulation_feb_mn_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201402240000 201402260000 /d4/gerry/simulation_dir/simulation_feb_mi_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201402240000 201402260000 /d4/gerry/simulation_dir/simulation_feb_mi_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201401040500 201401060500 /d4/gerry/simulation_dir/simulation_jan_mn_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mn_simulation.cfg 300 201401040500 201401060500  /d4/gerry/simulation_dir/simulation_jan_mn_less_dense_no_winds\n\tsimulator.py -d 1 /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201401040500 201401060500  /d4/gerry/simulation_dir/simulation_jan_mi_more_dense_no_winds\n\tsimulator.py /d4/gerry/simulation_dir/simulation_config/mi_simulation.cfg 300 201401040500 201401060500 /d4/gerry/simulation_dir/simulation_jan_mi_less_dense_no_winds"

    usage2 = "\n\n\tWipers only examples:\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mn_simulation_wipers.cfg 3600 201405310500 201406010500 /d4/gerry/wiper_simulation_dir/simulation_may_mn_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mi_simulation_wipers.cfg 3600 201405310500 201406010500 /d4/gerry/wiper_simulation_dir/simulation_may_mi_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mn_simulation_wipers.cfg 3600 201404060000 201404070000 /d4/gerry/wiper_simulation_dir/simulation_april_mn_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mi_simulation_wipers.cfg 3600 201404060000 201404070000 /d4/gerry/wiper_simulation_dir/simulation_april_mi_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mn_simulation_wipers.cfg 3600 201403311200 201404011200 /d4/gerry/wiper_simulation_dir/simulation_march_mn_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mi_simulation_wipers.cfg 3600 201403311200 201404011200 /d4/gerry/wiper_simulation_dir/simulation_march_mi_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mn_simulation_wipers.cfg 3600 201402250000 201402260000 /d4/gerry/wiper_simulation_dir/simulation_feb_mn_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mi_simulation_wipers.cfg 3600 201402250000 201402260000 /d4/gerry/wiper_simulation_dir/simulation_feb_mi_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mn_simulation_wipers.cfg 3600 201401050500 201401060500 /d4/gerry/wiper_simulation_dir/simulation_jan_mn_wipers_only\n\tsimulator.py -W /d4/gerry/wiper_simulation_dir/simulation_config/mi_simulation_wipers.cfg 3600 201401050500 201401060500 /d4/gerry/wiper_simulation_dir/simulation_jan_mi_wipers_only"    

    usage_str = "%prog config_file time_delta begin_date end_date output_dir\n\t" + usage1 + usage2
    parser = OptionParser(usage = usage_str)
    parser.add_option("-d", "--density", dest="density", help="less dense - 0, more dense - 1")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-W", "--wipers_only", dest="wipers_only", action="store_true", help="only do wipers processing")
    (options, args) = parser.parse_args()

    if len(args) < 5:
        parser.print_help()
        sys.exit(2)

    density = 0
    if options.density != None:
        density = int(options.density)
        
    config_file = args[0]
    time_delta = int(args[1])
    begin_date = args[2]
    end_date = args[3]
    output_dir = args[4]
    
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    ret = simulation(options.wipers_only, density, config_file, Met_data_dict, time_delta, begin_date, end_date, output_dir, logg)
    logg.write_ending(ret)

if __name__ == "__main__":

   main()
