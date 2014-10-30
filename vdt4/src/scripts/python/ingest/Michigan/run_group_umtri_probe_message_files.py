#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_group_umtri_probe_message_files.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2014/06/18 20:06:55 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import time
from optparse import OptionParser

LOOKBACK_INTERVAL_MIN = 10
LOOKBACK_INTERVAL_SEC = LOOKBACK_INTERVAL_MIN * 60

def main():

    usage_str = "%prog"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 0:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    logg.set_suffix(".pyl")
    logg.write_starting("run_create_probe_msg_history.py")

    curr_time = time.time()
    end_time = time.mktime(time.gmtime(curr_time))
    begin_time = end_time - LOOKBACK_INTERVAL_SEC
    
    end_time_tup = time.gmtime(end_time)
    day = time.strftime("%Y%m%d", end_time_tup)
    hhmm = time.strftime("%H%M", end_time_tup)

    begin_time_tup = time.gmtime(begin_time)
    begin_day = time.strftime("%Y%m%d", begin_time_tup)
    begin_hhmm = time.strftime("%H%M", begin_time_tup)

    end_time = day + hhmm
    begin_time = begin_day + begin_hhmm
    
    
    cmd = "group_umtri_probe_msg_files.py -l /d2/vii/data/log/group_umtri_probe_msg_files vii_paths.py %s %s /d2/vii/data/static/cdl/probe_message.cdl $VII_DATA/tmp $VII_DATA/processed/mi_csv2nc $VII_DATA/processed/mi_probe_message" % (begin_time, end_time)
    #cmd = "group_umtri_probe_msg_files.py vii_paths.py %s %s /d2/vii/data/static/cdl/probe_message.cdl $VII_DATA/tmp $VII_DATA/processed/mi_csv2nc /d2/test/lookback_test" % (begin_time, end_time)
    #cmd = "group_umtri_probe_msg_files.py vii_paths.py 201405081052 201405081122 /d2/vii/data/static/cdl/probe_message.cdl $VII_DATA/tmp $VII_DATA/processed/mi_csv2nc /d2/test/lookback_test" 
    logg.write_info("executing: %s" % cmd)
    ret = os.system(cmd)
    if ret != 0:
        logg.write_error("group_umtri_probe_msg_files.py returned %d" % ret)
        logg.write_ending(1, "run_group_umtri_probe_msg_files.py")
    else:
        logg.write_ending(0, "run_group_umtri_probe_msg_files.py")
    
    
if __name__ == "__main__":

   main()
