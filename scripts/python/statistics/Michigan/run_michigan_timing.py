#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_michigan_timing.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/22 22:10:49 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
import time
import calendar
from optparse import OptionParser

def gen_date_list(begin_date, end_date):
    """Generates a list of dates of the form yyyymmdd from a being date to end date
    Inputs:
      begin_date -- such as "20070101"
      end_date -- such as "20070103"
    Returns:
      date_list -- such as ["20070101","20070102","20070103"]
    """
    begin_tm = time.strptime(begin_date, "%Y%m%d")
    end_tm = time.strptime(end_date, "%Y%m%d")
    begin_tv = calendar.timegm(begin_tm)
    end_tv = calendar.timegm(end_tm)
    date_list = []
    for tv in xrange(begin_tv, end_tv+86400, 86400):
        date_list.append(time.strftime("%Y%m%d", time.gmtime(tv)))
    return date_list                         


def main():

    usage_str = "%prog begin_date end_date in_dir out_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    begin_date = args[0]
    end_date = args[1]
    in_dir = args[2]
    out_dir = args[3]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting("run_michigan_timing.py")

    mydatelist = gen_date_list(begin_date, end_date)

    for date in mydatelist:
        dated_in_dir = os.path.join(in_dir, date)
        if os.path.exists(dated_in_dir):
            cmd = "michigan_timing.py %s %s %s" % (date, in_dir, out_dir)
            os.system(cmd)
        else:
            logg.write_info("%s does not exist, skipping" % dated_in_dir)
    
    logg.write_ending(0, "run_michigan_timing.py")

if __name__ == "__main__":

   main()
