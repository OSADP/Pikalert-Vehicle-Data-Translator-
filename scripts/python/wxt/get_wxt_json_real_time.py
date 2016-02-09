#!/usr/bin/env python

"""Get wxt data in real time"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_wxt_json_real_time.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/09/30 18:30:26 $           #
#                                                                                #
# ============================================================================== #

from optparse import OptionParser
import log_msg
import os
import sys
import time
import get_wxt_json

def main():

    usage_str = "%prog output_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-c", "--count", dest="count", help="number of times to run")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-n", "--number", dest="number", help="number of queries")
    parser.add_option("-t", "--time_offset", dest="time_offset", help="time offset")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    number = 1
    time_offset = 15

    if options.time_offset:
        time_offset = int(options.time_offset)
        
    if options.number:
        number = int(options.number)
        
    count = 10
    if options.count:
        count = int(options.count)
        
        
    output_file = args[0]
    offset = ""

    logg.write_starting()
    for ind in range(count):
        current_time = time.time()
        start_time = current_time - time_offset
        begin_yyyymmddhhmmss = time.strftime("%Y%m%d%H%M%S", time.gmtime(start_time))
        end_yyyymmddhhmmss = time.strftime("%Y%m%d%H%M%S", time.gmtime(current_time))
        get_wxt_json.get_wxt_data(number, offset, begin_yyyymmddhhmmss, end_yyyymmddhhmmss, output_file, logg)
        time.sleep(time_offset)
    logg.write_ending()




if __name__ == "__main__":

   main()
