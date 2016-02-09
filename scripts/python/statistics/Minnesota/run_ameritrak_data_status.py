#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2006
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import os, sys
import string
import time
import calendar
import log_msg
from optparse import OptionParser

#in_dir = "/d3/jpresto/test/ameritrak_data_status/"
in_dir = "/d2/vii/data/raw/ameritrak/"
#out_dir = "/d3/jpresto/data/ameritrak_data_status/"
#out_dir = "/d2/vii/data/statistics/ameritrak_data_status/"
out_dir = "/var/autofs/mnt/ftp/restricted/ftp_vdt/root/download/minnesota/daily_statistics/"
#log_prefix = "/d3/jpresto/log/ameritrak_data_status"
log_prefix = "/d2/vii/data/log/ameritrak_data_status"

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
    
    usage_str = " %prog begin_date end_date\n\tNote: begin_date and end_date should be in the format YYYYMMDD"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="log")

    (options, args) = parser.parse_args()

    if len(sys.argv) < 3:
        parser.print_help()
        sys.exit(2)

    begin_date = args[0]
    end_date = args[1]

    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting("run_ameritrak_data_status.py")    

    mydatelist = gen_date_list(begin_date, end_date)

    for index, mydate in enumerate(mydatelist):

        cmd = "ameritrak_data_status.py -l %s %s %s %s" % (log_prefix, mydate, in_dir, out_dir)
        #cmd = "ameritrak_data_status.py %s %s %s" % (mydate, in_dir, out_dir)
        logg.write_info(cmd)
        os.system(cmd)
        

    logg.write_ending(0)


if __name__ == "__main__":

   main()
 
