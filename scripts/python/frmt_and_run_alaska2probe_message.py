#!/usr/bin/env python

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: frmt_and_run_alaska2probe_message.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/22 14:46:11 $           #
#                                                                                #
# ============================================================================== #

import log, log_msg
import os
import time
from optparse import OptionParser
import sys
import vii_paths
import subprocess
import os
import errno
import processed_file
import tim
import re

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

def main():

    parser = OptionParser(usage = "%prog [options] ftp_input_dir raw_output_dir cdl_file processed_output_dir")
    parser.add_option("-l","--log",dest="log",help="base name of log file")

    (options,args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    input_dir = args[0]
    raw_output_dir = args[1]
    cdl_file = args[2]
    proc_output_dir = args[3]
    
    
    ret = 0
    
    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("frmt_and_run_alaska2probe_message.py")
    
    # Resolve date and time
    curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
    curr_hhmmss = time.strftime("%H%M%S", time.gmtime(time.time()))
    curr_hhmm = time.strftime("%H%M", time.gmtime(time.time()))
    
    dt = "%s.%s" % (curr_date, curr_hhmmss)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)


    # Find the latest csv file in the ftp directory
    # Loop back 24 hours and look for a file that has a specific month/day/hour format
    #
    input_file_prefix = "AKD"
    found = 0
    loop_time = time_val
    end_time = time_val - (24 * 3600)
    while(loop_time >= end_time):
        loop_year = time.strftime("%Y", time.gmtime(loop_time))
        loop_month = time.strftime("%m", time.gmtime(loop_time))
        loop_day = time.strftime("%d", time.gmtime(loop_time))
        tmp_loop_hh = curr_hhmm = time.strftime("%H", time.gmtime(loop_time))
        dec_loop_hh = int(tmp_loop_hh)
        loop_mm = curr_hhmm = time.strftime("%M", time.gmtime(loop_time))
        
        #print "loop_year: %s, loop_month: %s, loop_day: %s, dec_loop_hh: %d, loop_mm: %s" % (loop_year, loop_month, loop_day, dec_loop_hh, loop_mm)

        input_file_name = "AKD%s%s%d%s.csv" % (loop_month, loop_day, dec_loop_hh, loop_mm)
        input_path = "%s/%s" % (input_dir, input_file_name)

        if(os.path.exists(input_path)):
            #print "FOUND: %s" % input_path
            found = 1
            break
        
        loop_time = loop_time - 60

        
    if(not found):
        logg.write_warning("No recent input file found, not creating output file")
        logg.write_ending(exit_status=0, msg="frmt_and_run_alaska2probe_message.py")
        sys.exit(0)

    #print "input_path: %s" % input_path
    

    # Open input file and read in lines
    # Create a dictionary that contains the data for each site
    site_data = {}
    num_columns = 7
    num_valid_lines = 0
    if os.path.exists(input_path):
        in_fh = open(input_path, "r")
        for line in in_fh:
            line = line.rstrip('\n' +'\r')
            fields = line.split(',')
            if(len(fields) != 8):
                logg.write_error("Found %d fields in line %s, expecting 8" % (len(fields), line))
                continue

            # Skip the header
            if(re.match("label", fields[0])):
                continue

            # Valid entries start with the site-id which is a decimal (integer)
            if(re.match('\d', line)):
                #print "line: %s" % line
                site = fields[0]
                
                if site not in site_data.keys():
                    site_data[site] = []
                    for x in xrange(0, num_columns):
                        site_data[site].append([])

                site_data[site][0].append(fields[1]) # Date-Time (0)
                site_data[site][1].append(fields[2]) # Variable-name (1)
                site_data[site][2].append(fields[3]) # Value (2)
                site_data[site][3].append(fields[4]) # Latitude (3) 
                site_data[site][4].append(fields[5]) # Longitude (4)
                site_data[site][5].append(fields[6]) # Speed (5)
                site_data[site][6].append(fields[7]) # Heading (6)
                
                num_valid_lines = num_valid_lines + 1

    # Check if we have any valid data, if not exit
    if(num_valid_lines == 0):
        logg.write_warning("No data found in most recent input file, not creating output file")
        logg.write_ending(exit_status=0, msg="frmt_and_run_alaska2probe_message.py")
        sys.exit(0)

    # Make raw output dir if necessary
    raw_dest_dir = "%s/%s" % (raw_output_dir, curr_date)
    mkdir_path(raw_dest_dir)
    raw_output_file_name = "alaska.%s.csv" % curr_date
    raw_output_path = "%s/%s" % (raw_dest_dir, raw_output_file_name)
        
    # Check if output file exists, if it does don't write header
    header_flag = 1
    if(os.path.exists(raw_output_path)):
        header_flag = 0
    
    # Open the output file and append data to it. 
    out_fh = open(raw_output_path, "a")
    
    # Write header
    header_line = "VehicleID,Time(UTC),Latitude,Longitude,Speed,Heading,AirTemp,RelativeHumidity,RoadTemp,DewpointTemp"
    if(header_flag):
        out_fh.write("%s\n" % header_line)
    
    # Loop over the data dictionary and output the data
    # The key is the site-id
    for key in site_data.keys():
        #print "key : %s" % key
        
        # Get the date-time for each site
        # The date-time is the first column in the dict (x = 0)
        # Use the first date-time row in the dict for output (y = 0)
        x = 0
        y = 0
            
        date_time_fields = site_data[key][x][y].split()
        
        date_fields = date_time_fields[0].split('/')
        month = date_fields[0]
        day = date_fields[1]
        year = date_fields[2]
        
        time_fields =  date_time_fields[1].split(':')
        hour = time_fields[0]
        minute = time_fields[1]
        second = time_fields[2]
        
        site_dt = "%s%s%s.%s%s%s" % (year, month, day, hour, minute, second)
        site_ttup = time.strptime(site_dt,"%Y%m%d.%H%M%S")
        site_time_val = tim.mkgmtime(site_ttup)
        
        # Convert to UTC = AKST + 9hours for AST
        # Convert to UTC = AKST + 8hours for ADT
        site_time_val = site_time_val + (8 * 3600)
        
        # Get the latitude for each site
        # The latitude is the fourth column in the dict (x = 3)
        # Use the first latitude row in the dict (y = 0)
        x = 3
        y = 0
        lat = "%.4f" % float(site_data[key][x][y])
        
        # Get the longitude for each site
        # The longitude is the fith column in the dict (x = 4)
        # Use the first longitude row in the dict (y = 0)
        x = 4
        y = 0
        lon = "%.4f" % float(site_data[key][x][y])

        # Get the speed for each site
        # The speed is the sixth column in the dict (x = 5)
        # Use the first speed row in the dict (y = 0)
        x = 5
        y = 0
        speed = float(site_data[key][x][y])

        # Get the heading for each site
        # The heading is the seventh column in the dict (x = 6)
        # Use the first speed row in the dict (y = 0)
        x = 6
        y = 0
        heading = float(site_data[key][x][y])
        
        # Get the data value (T, rh, road-T, dewpt)
        # The data values is the third column in the dict (x = 2)
        # The first row (y = 0) is T, second row (y = 1) is rh, third row (y = 2) is road-T, fourth row (y = 3) is dewpt
        x = 2

        T = float(site_data[key][x][0])
        rh = float(site_data[key][x][1])
        road_T = float(site_data[key][x][2])
        dewpt = float(site_data[key][x][3])

        # QC the values and convert units (degF to degC)
        #
        # T is in degF
        # rh is in %
        # road_T is in degF
        # dewpt is in degF
        #
        if(T < -70.0 or T > 90.0):
            T_str = ""
        else:
            T_str = "%.2f" % ((T-32.0)/1.8)
        
        if(rh < 0.0 or rh > 100.0):
            rh_str = ""
        else:
            rh_str = "%.0f" % rh
        
        if(road_T < -70.0 or road_T > 90.0):
            road_T_str = ""
        else:
            road_T_str = "%.2f" % ((road_T-32.0)/1.8)
        
        if(dewpt < -70.0 or dewpt > 90.0):
            dewpt_str = ""
        else:
            dewpt_str = "%.2f" % ((dewpt-32.0)/1.8)

        if(speed < 0 or speed > 200):
            speed_str = ""
        else:
            speed_str = "%.2f" % (speed * 0.44704)
        
        if(heading < 0 or heading > 360):
            heading_str = ""
        else:
            heading_str = "%.0f" % heading
        
        output_line = "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" % (key, site_time_val, lat, lon, speed_str, heading_str, T_str, rh_str, road_T_str, dewpt_str)
        #print "%s" % output_line
        out_fh.write("%s\n" % output_line)
        
    out_fh.close()

    #
    # Run alaska2probe_message.py (this uses the alaska_reader.py class)
    #

    # Make processed output dir if necessary
    proc_dest_dir = "%s/%s" % (proc_output_dir, curr_date)
    mkdir_path(proc_dest_dir)
    proc_output_file_name = "alaska.%s.nc" % curr_date
    proc_output_path = "%s/%s" % (proc_dest_dir, proc_output_file_name)
    
    cmd_str = "alaska2probe_message.py %s %s %s 0" % (cdl_file, raw_output_path, proc_output_path)
    
    # Run the command line
    ret = run_cmd(cmd_str, logg)
    if ret !=0:
        logg.write_error("run_cmd() failed for alaska2probe_message.py command_str.")

    
    logg.write_ending(exit_status=ret, msg="frmt_and_run_alaska2probe_message.py")
    return ret

if __name__ == "__main__":
    main()
