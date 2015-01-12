#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_create_probe_msg_history.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/04/08 19:30:58 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import time
import datetime
import string


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

    end_time = time.mktime(time.gmtime()) - 300
    end_time = end_time - (end_time % 300)
    begin_time = end_time - 3600
    
    end_time_tup = time.gmtime(end_time)
    day = time.strftime("%Y%m%d", end_time_tup)
    hhmm = time.strftime("%H%M", end_time_tup)

    begin_time_tup = time.gmtime(begin_time)
    begin_day = time.strftime("%Y%m%d", begin_time_tup)
    begin_hhmm = time.strftime("%H%M", begin_time_tup)
    end_time = day + hhmm + "00"
    begin_time = begin_day + begin_hhmm + "00"

    cmd = "create_probe_msg_history.py -l /d2/vii/data/log/create_probe_msg_history_mn vii_config /d2/vii/data/processed/mn_vdt_output/ /d2/vii/data/history/ -b %s -e %s" % (begin_time, end_time)
    logg.write_info("executing: %s" % cmd)
    ret = os.system(cmd)
    if ret != 0:
        logg.write_error("create_probe_msg_history.py returned %d" % ret)
        logg.write_ending(1, "run_create_probe_msg_history.py")
    else:
        logg.write_ending(0, "run_create_probe_msg_history.py")
    
if __name__ == "__main__":

   main()
