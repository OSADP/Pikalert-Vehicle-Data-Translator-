#!/usr/bin/env python

#Copies file rounded to minutesago from base dir to dest dir
#and then renames them using a passed in regex

import sys
from datetime import datetime
from datetime import timedelta
import shutil
import os.path
from optparse import OptionParser
import log_msg
import re
import os



def round_date_str(dt, minutesago, group_interval, dateformat):
    #Take off minutesago min
    dt -= timedelta(minutes=minutesago)
    #Round down to the nearest group_interval
    dt -= timedelta(minutes=dt.minute % group_interval,seconds=dt.second,microseconds=dt.microsecond)
    return dt.strftime(dateformat)


def dated_dir_str(dt, minutesago):
    dt -= timedelta(minutes=minutesago)
    return dt.strftime('%Y%m%d')


def test_round_date_str(minutesago, group_interval, dateformat):
    print "time\t\trounded_time"
    for i in range(0, 60):
        dt = datetime(2014, 12, 10, 12, i) 
        print dt.strftime(dateformat), round_date_str(dt, minutesago, group_interval, dateformat)


def main():
    usage_str = "USAGE: %s filebase srcbase destbase \"matchregex\" \"substr\" dateformat minutesago [interval]" \
                "\n\n-filebase is the root of the filename. i.e. wxt.probe." \
                "\n-srcbase is the source directory" \
                "\n-destbase is the destination directory" \
                "\n-matchregex should be a quoted pattern with groups to replace. i.e. \"(wxt\.probe\.\d{14}\.nc)\"" \
                "\n-substr should contain the replacement string. Groups can be referenced in substr using \\1 \\2 etc. i.e. \"mn_\\1\"" \
                "\n-dateformat is a datetime string to match in the source file. i.e. %Y%m%d%H%M00" \
                "\n-minutesago is the number of minutes ago that we would like to grab a file from." \
                "\n-interval is the optional grouping interval.  If interval is not specified then it will be set equal to minutesago."
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-t", "--test", dest="test", help="Executes tests.", action="store_true")
    
    (options, args) = parser.parse_args()
    
    if len(args) < 7:
        print parser.print_help()
	exit(2)

    filebase = args[0]
    srcbase = args[1]
    destbase = args[2]
    matchregex = args[3]
    subs = args[4]
    dateformat = args[5]
    minutesago = int(args[6])
    group_interval = int(args[7]) if len(args) >= 8 else minutesago

    if options.test:
        print "current time\t\trounded_time"
        print datetime.utcnow(), round_date_str(datetime.utcnow(), minutesago, group_interval, dateformat)
        print
        test_round_date_str(minutesago, group_interval, dateformat)
        sys.exit(1)
   
    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
    
    round_dt_str = round_date_str(datetime.utcnow(), minutesago, group_interval, dateformat)
    dated_dir = dated_dir_str(datetime.utcnow(), minutesago)

    filename = "%s%s.nc" % (filebase, round_dt_str)
    srcfile = os.path.join(srcbase, dated_dir, filename)
        
    if not os.path.exists(srcfile):
        logg.write_info("%s not found" % srcfile)
        sys.exit(1)

    match = re.match(matchregex, filename)
    if not match:
        logg.write_error("Invalid regex: %s for file: %s" % (matchregex, filename))
        sys.exit(2)
    
    renamedfile = re.sub(matchregex, subs, filename)
    logg.write_info("Renaming %s to %s" % (filename, renamedfile))

    destdir = os.path.join(destbase, dated_dir)
    if not os.path.exists(destdir):
        logg.write_info("Creating %s" % destdir)
        os.mkdir(destdir)
    
    dstfile = os.path.join(destdir, renamedfile)

    logg.write_info("Copying %s to %s" % (srcfile, dstfile))
    shutil.copyfile(srcfile, dstfile)
    
    sys.exit(0)

    
if __name__ == '__main__':
    main()
