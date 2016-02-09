#!/usr/bin/env python

"""
This is the driver script for the gini2nc code.  
"""
# Author Beth Weekley @ UCAR/RAL  5/06

import os
import sys
from optparse import OptionParser
import ncvdefs
import log

# -------------------------
# Parse command options
usage_str = "%prog [options] config\n\nrequired:\n  config\t\tbase name of python config file containing definitions\n\t\t\tfor NCV instance. (Omit path and .py file extension.)"

parser = OptionParser(usage = usage_str)
parser.add_option("-d", "--date", dest="date", help="use date (yyyymmdd.hhmm) for current date")
parser.add_option("-t", "--test", action="store_true", default=False, dest="test", help="test - echo but do not execute commands")
(options, args) = parser.parse_args()

if len(args) != 1:
    print "Error: insufficient command line arguments"
    parser.print_help()
    sys.exit(2)

config = args[0]


# Import the config file
try:
    cf = __import__(config, globals(), locals(), [])
except:
    print "Error: importing config file: %s" % config
    parser.print_help()
    sys.exit(1)


params_dir = cf.Static_satellite
in_base_path = cf.Satellite_goes_init
out_base_dir = cf.Satellite_goes_combine
logpath = os.path.join(cf.Log_dir, "gini2nc")

channel_list = ncvdefs.sat_channels
proc_script = "gini2nc.py"

# -------------
# loop over the satellite list to process both east and west
# -------------
for sat in xrange(len(cf.Satellite_list)):
    out_dir = os.path.join(out_base_dir,cf.Satellite_list[sat])
    sat_in_dir = os.path.join(in_base_path, cf.Satellite_list[sat])
    # -------------
    # loop over the channel directories   
    # -------------
    for dir in xrange(len(channel_list)):
        curr_in_dir = os.path.join(sat_in_dir, channel_list[dir])
        command = "%s -l %s %s %s %s %s %s %s" %(proc_script, logpath, curr_in_dir, out_dir, cf.Gini_cdl_list[sat], params_dir, channel_list[dir],cf.Satellite_list[sat])

        #print "command = %s " % command
        if options.test:
            print command
        else:
            logg = log.Log(logpath, ncvdefs.PYL_SUFFIX)
            logg.write_time("Running: %s\n" % command)
            logg.close()
            ret = os.system(command)

# -------------------------
