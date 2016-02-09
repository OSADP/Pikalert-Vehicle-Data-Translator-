#!/usr/bin/env python

import os,sys
import ncvdefs
from optparse import OptionParser
import log

# Author Beth Weekley @ UCAR/RAL  6/06


# -------------------------
# Parse command options
usage_str = "%prog [options] config\n\nrequired:\n  config\t\tbase name of python config file containing definitions\n\t\t\tfor NCV instance. (Omit path and .py file extension.)"

parser = OptionParser(usage = usage_str)
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

proc_script = "blend_latest.py"
cdl_file = cf.Sat_cloud_mask_blend_cdl_file
output_dir = cf.Satellite_cloud_mask_blend

logpath = os.path.join(cf.Log_dir, "blend_latest")

command = "%s %s %s %s %s %s %s -L %s" % (proc_script, cf.Satellite_west_blend_in_dir, cf.Satellite_west_in_base, cf.Satellite_east_blend_in_dir, cf.Satellite_east_in_base, cdl_file, output_dir, logpath)

if options.test:
    print command
else:
    logg = log.Log(logpath, ncvdefs.PYL_SUFFIX)
    logg.write_time("Running: %s\n" % command)
    logg.close()
    ret = os.system(command)
    if (ret != 0):
        sys.exit(1)
