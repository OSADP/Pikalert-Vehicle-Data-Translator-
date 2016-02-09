#!/usr/bin/env python

"""Create generic csv for say Michigan for demonstration"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import time
import numpy

def sample():
    """Create samples based on a Gaussian distribution"""
    print numpy.random.normal(loc=0.0, scale=0.1, size=50)

def create_generic_belle_isle(base_time, out_file):
    """
    """

    fp_out = open(out_file, "w")
    vehicle_id_list = ["ncar1", "ncar2", "ncar3", "ncar4", "ncar5"]
    lat_long = [(42.336412,-83.000058), (42.336592,-82.999269), (42.336941,-82.997649), (42.337136,-82.996683), (42.3370,-82.9971)]

    obs_time = time.strftime("%Y%m%d%H%M%S", time.gmtime(base_time))
    elevation = 878.71
    air_temperature2 = 32
    heading = 90
    speed_mph = 5
    abs_brakes = 0
    trac = 0
    
    header = "vehicle_id,time,latitude,longitude,elevation,air_temperature2,heading,speed_mph,wiper_status,abs,trac\n"
    fp_out.write(header)
    for ind in range(len(vehicle_id_list)):
        if ind == 2:
            wiper_status = 0
            abs_brakes = 0
            trac = 0
        elif ind == 3:
            wiper_status = 0
            abs_brakes = 1
            trac = 0
        elif ind == 4:
            wiper_status = 0
            abs_brakes = 0
            trac = 0
        else:
            wiper_status = 2
            abs_brakes = 0
            trac = 0
            
        line = "%s,%s,%f,%f,%f,%f,%f,%f,%d,%d,%d\n" % (vehicle_id_list[ind], obs_time, lat_long[ind][0], lat_long[ind][1], elevation, air_temperature2, heading, speed_mph, wiper_status, abs_brakes, trac)
        fp_out.write(line)
    fp_out.close()
    
def create_generic_belle_isle_clear(base_time, out_file):
    """
    """

    fp_out = open(out_file, "w")
    vehicle_id_list = ["ncar1", "ncar2", "ncar3", "ncar4"]
    lat_long = [(42.336412,-83.000058), (42.336592,-82.999269), (42.336941,-82.997649), (42.337136,-82.996683)]

    obs_time = time.strftime("%Y%m%d%H%M%S", time.gmtime(base_time))
    elevation = 878.71
    air_temperature2 = 32
    heading = 90
    speed_mph = 5
    abs_brakes = 1

    header = "vehicle_id,time,latitude,longitude,elevation,air_temperature2,heading,speed_mph,wiper_status,abs\n"
    fp_out.write(header)
    for ind in range(len(vehicle_id_list)):
        wiper_status = 0
        line = "%s,%s,%f,%f,%f,%f,%f,%f,%d,%d\n" % (vehicle_id_list[ind], obs_time, lat_long[ind][0], lat_long[ind][1], elevation, air_temperature2, heading, speed_mph, wiper_status, abs_brakes)
        fp_out.write(line)
    fp_out.close()
    
def create_generic_belle_isle_low(base_time, out_file):
    """
    """

    fp_out = open(out_file, "w")
    vehicle_id_list = ["ncar1", "ncar2", "ncar3", "ncar4"]
    lat_long = [(42.336412,-83.000058), (42.336592,-82.999269), (42.336941,-82.997649), (42.337136,-82.996683)]

    obs_time = time.strftime("%Y%m%d%H%M%S", time.gmtime(base_time))
    elevation = 878.71
    air_temperature2 = 32
    heading = 90
    speed_mph = 5
    abs_brakes = 1

    header = "vehicle_id,time,latitude,longitude,elevation,air_temperature2,heading,speed_mph,wiper_status,abs\n"
    fp_out.write(header)
    for ind in range(len(vehicle_id_list)):
        wiper_status = 2
        line = "%s,%s,%f,%f,%f,%f,%f,%f,%d,%d\n" % (vehicle_id_list[ind], obs_time, lat_long[ind][0], lat_long[ind][1], elevation, air_temperature2, heading, speed_mph, wiper_status, abs_brakes)
        fp_out.write(line)
    fp_out.close()
    
def create_generic_belle_isle_high(base_time, out_file):
    """
    """

    fp_out = open(out_file, "w")
    vehicle_id_list = ["ncar1", "ncar2", "ncar3", "ncar4"]
    lat_long = [(42.336412,-83.000058), (42.336592,-82.999269), (42.336941,-82.997649), (42.337136,-82.996683)]

    obs_time = time.strftime("%Y%m%d%H%M%S", time.gmtime(base_time))
    elevation = 878.71
    air_temperature2 = 32
    heading = 90
    speed_mph = 5
    abs_brakes = 1

    header = "vehicle_id,time,latitude,longitude,elevation,air_temperature2,heading,speed_mph,wiper_status,abs\n"
    fp_out.write(header)
    for ind in range(len(vehicle_id_list)):
        wiper_status = 3
        line = "%s,%s,%f,%f,%f,%f,%f,%f,%d,%d\n" % (vehicle_id_list[ind], obs_time, lat_long[ind][0], lat_long[ind][1], elevation, air_temperature2, heading, speed_mph, wiper_status, abs_brakes)
        fp_out.write(line)
    fp_out.close()
    

def create_generic_mdot(base_time, out_file):
    """
    """

    fp_out = open(out_file, "w")
    vehicle_id_list = ["ncar1", "ncar2"]
    lat_long = [(42.365552,-83.048965), (42.367328,-83.050553)]

    obs_time = time.strftime("%Y%m%d%H%M%S", time.gmtime(base_time))
    elevation = 878.71
    air_temperature2 = 32
    heading = 90
    speed_mph = 5
    abs_brakes = 1

    header = "vehicle_id,time,latitude,longitude,elevation,air_temperature2,heading,speed_mph,wiper_status,abs\n"
    fp_out.write(header)
    for ind in range(len(vehicle_id_list)):
        if ind >= 2:
            wiper_status = 0
        else:
            wiper_status = 3
        line = "%s,%s,%f,%f,%f,%f,%f,%f,%d,%d\n" % (vehicle_id_list[ind], obs_time, lat_long[ind][0], lat_long[ind][1], elevation, air_temperature2, heading, speed_mph, wiper_status, abs_brakes)
        fp_out.write(line)
    fp_out.close()
    

def main():

    usage_str = "%prog out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    out_file = args[0]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    create_generic_belle_isle(out_file)
    logg.write_ending()

if __name__ == "__main__":

   main()
