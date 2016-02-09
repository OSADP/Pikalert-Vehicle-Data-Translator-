#!/usr/bin/env python


# Author Beth Weekley @ UCAR/RAL  6/06
"""
This code calls awc_rename.py which takes AWC input
GINI file names and converts them to our expected
naming convention and creates links to the original
files.  Also it creates initial index files at the
same time.  This function can be used in place of
run_index_gini.py and index_gini.py for AWC GINI
files.  
"""

import os
import sys
from optparse import OptionParser
import ncvdefs
import log

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

# Set up constants
proc_script = "awc_rename.py"

# input vars
in_sat_dir_list = ["goesE", "goesW"]
in_channel_list = ["ir4km_ch4", "ir4km_ch2"]
in_base_sat = ["gE", "gW"]

# output vars
out_sat_dir_list =  ["east", "west"]
out_channel_list = ncvdefs.sat_channels # used to get the input directory path
out_suffix = ncvdefs.GINI_SUFFIX
logpath = os.path.join(cf.Log_dir, "awc_rename")
logfile_str = "%s %s" % ("-l", logpath)

in_dir_base = cf.Raw_awc_gini_dir 
out_link_dir_base = cf.Satellite_goes_init_awc

# if the input dir does not exist then make a link to it from the raw ldm data
if (not os.path.exists(in_dir_base)):
    command = ("ln -s %s %s") %(cf.Raw_awc_gini_dir, in_dir_base)
    ret = os.system(command)

# -------------
# loop over the satellite list to process both east and west
# -------------
for sat in xrange(len(in_sat_dir_list)):
    out_dir = os.path.join(out_link_dir_base, out_sat_dir_list[sat])
    in_dir = os.path.join(in_dir_base, in_sat_dir_list[sat])

    # -------------
    # loop over the channel directories   
    # -------------
    for channel in xrange(len(in_channel_list)):
        curr_in_dir = os.path.join(in_dir, in_channel_list[channel])
	curr_out_link_dir = os.path.join(out_dir, out_channel_list[channel])
        output_base = out_sat_dir_list[sat] + "_" + out_channel_list[channel]
        in_base = in_base_sat[sat]+ "_" + in_channel_list[channel]
        command = "%s %s %s %s %s %s %s" % (proc_script, curr_in_dir, in_base, curr_out_link_dir, output_base,out_suffix, logfile_str)

	#print command
        if options.test:
            print command
        else:
            logg = log.Log(logpath, ncvdefs.PYL_SUFFIX)
            logg.write_time("Running: %s\n" % command)
            logg.close()
            ret = os.system(command)

# -------------------------
