#!/usr/bin/env python

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_AK_mobile_data.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/12/16 21:55:59 $           #
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

    parser = OptionParser(usage = "%prog [options] ftp_input_dir output_dir")
    parser.add_option("-l","--log",dest="log",help="base name of log file")

    (options,args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    input_dir = args[0]
    output_dir = args[1]

    input_file_name = "AkDOT_Road_Temps.csv"
    input_path = "%s/%s" % (input_dir, input_file_name)
    
    ret = 0
    
    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("get_AK_mobile_data.py")
    
    # Resolve date and time
    curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
    curr_hhmmss = time.strftime("%H%M%S", time.gmtime(time.time()))
    curr_hhmm = time.strftime("%H%M", time.gmtime(time.time()))
    
    dt = "%s.%s" % (curr_date, curr_hhmmss)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)

    # Make output dir if necessary
    dest_dir = "%s/%s" % (output_dir, curr_date)
    mkdir_path(dest_dir)
    output_file_name = "alaska.%s.%s.csv" % (curr_date, curr_hhmm)
    output_path = "%s/%s" % (dest_dir, output_file_name)
    
    # Open input file and read in lines
    # Create a dictionary that contains the data for each site
    site_data = {}
    num_columns = 5
    num_valid_lines = 0
    if os.path.exists(input_path):
        in_fh = open(input_path, "r")
        for line in in_fh:
            line = line.rstrip('\n' +'\r')
            fields = line.split(',')
            if(len(fields) != 6):
                logg.write_error("Found %d fields in line %s, expecting 6" % (len(fields), line))
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

                num_valid_lines = num_valid_lines + 1

    # Check if we have any valid data, if not exit
    #print "num_valid_lines: %d" % num_valid_lines
    if(num_valid_lines == 0):
        logg.write_warning("No data found in most recent input file, not creating output file")
        logg.write_ending(exit_status=0, msg="get_AK_mobile_data.py")
        sys.exit(0)
    
    # Loop over the data dictionary and output the data
    # The key is the site-id
    out_fh = open(output_path, "w")
    
    # Write header
    header_line = "VehicleID,Time(UTC),Latitude,Longitude,AirTemp,RelativeHumidity,RoadTemp,DewpointTemp"
    out_fh.write("%s\n" % header_line)
    
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
        
        # Convert to UTC = AKST + 9hours
        site_time_val = site_time_val + (9 * 3600)
        
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
        
        # Get the data value (T, rh, road-T, dewpt)
        # The data values is the third column in the dict (x = 2)
        # The first row (y = 0) is T, second row (y = 1) is rh, third row (y = 2) is road-T, fourth row (y = 3) is dewpt
        x = 2

        T = float(site_data[key][x][0])
        rh = float(site_data[key][x][1])
        road_T = float(site_data[key][x][2])
        dewpt = float(site_data[key][x][3])

        # QC the value
        if(T < -70.0 or T > 70.0):
            T_str = ""
        else:
            T_str = "%.2f" % T
        
        if(rh < 0.0 or rh > 100.0):
            rh_str = ""
        else:
            rh_str = "%.0f" % rh
        
        if(road_T < -70.0 or road_T > 70.0):
            road_T_str = ""
        else:
            road_T_str = "%.2f" % road_T
        
        if(dewpt < -70.0 or dewpt > 70.0):
            dewpt_str = ""
        else:
            dewpt_str = "%.2f" % dewpt
        
        output_line = "%s,%s,%s,%s,%s,%s,%s,%s" % (key, site_time_val, lat, lon, T_str, rh_str, road_T_str, dewpt_str)
        #print "%s" % output_line
        out_fh.write("%s\n" % output_line)
        
    out_fh.close()
    
    logg.write_ending(exit_status=ret, msg="get_AK_mobile_data.py")
    return ret

if __name__ == "__main__":
    main()
