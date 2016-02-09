#!/usr/bin/env python

import time
import os
import sys
import string
from optparse import OptionParser
import log,log_msg

ftp_site = "ftp://ftp.ncep.noaa.gov/pub/data/nccf/com/rtma/prod"
ftp_dir = "rtma2p5.%Y%m%d"

def main():
    parser = OptionParser(usage = "%prog [options] out_dir")
    parser.add_option("-d","--date",dest="date",help="date")
    parser.add_option("-l","--log",dest="log",help="base name of log file")
    (options,args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    out_dir = args[0]

    if options.log:
        logg = log_msg.LogMessage(options.log, "pyl")
    else:
        logg = log_msg.LogMessage("")

    date_str = time.strftime("%Y%m%d.%H%M")
    if options.date != None:
        date_str = options.date

    time_tup = time.strptime(date_str, "%Y%m%d.%H%M")
    ptime = time.mktime(time_tup)
    ptime = ptime - (ptime % 300) - 300
    time_tup = time.gmtime(ptime)
    day_str = time.strftime("%Y%m%d", time_tup)
    hh = time.strftime("%H", time_tup)

    logg.write_starting("")
    day_ftp_dir = time.strftime(ftp_dir, time_tup)
    fnames = ["rtma2p5.t%sz.2dvarges_ndfd.grb2" % hh,"rtma2p5.t%sz.2dvaranl_ndfd.grb2" % hh]
    out_day_dir = os.path.join(out_dir, day_str)
    cmd = "mkdir -p %s" % out_day_dir
    logg.write_info(cmd)
    os.system(cmd)
    for fname in fnames:
        fpath = "%s/%s/%s" % (ftp_site, day_ftp_dir, fname)
        ofpath = os.path.join(out_day_dir, fname)
        cmd = "curl %s > %s" % (fpath, ofpath)
        logg.write_info(cmd)
        ret = os.system(cmd)
        if ret != 0:
            logg.write_error("couldn't grab %s, cleaning up..." % fpath)
            cmd = "rm -f %s" % ofpath
            logg.write_info(cmd)
            os.system(cmd)
            logg.write_ending(ret)
            return ret

    logg.write_ending(0)
    return 0

if "__main__" == __name__:
    sys.exit(main())
