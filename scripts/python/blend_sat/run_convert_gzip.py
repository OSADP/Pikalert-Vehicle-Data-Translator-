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
# Beth Weekley @ UCAR/RAL  5/06

import os,sys
import ncvdefs
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


proc_script = "convert_gzip.py"
logpath = os.path.join(cf.Log_dir, "convert_gzip")
logfile_str = "%s %s" % ("-l", logpath)

input_base = ncvdefs.Gini2nc_out_base
output_base = ncvdefs.Projection_out_base
in_dir_base = cf.Satellite_goes_combine
var_names_file = cf.Sat_vars_file
cdl_file = cf.Sat_convert_cdl_file
out_dir_base = cf.Satellite_goes_grid_convert
tmp_dir = cf.Tmp_dir

# -------------
# loop over the satellite list to process both east and west
# -------------
for sat in xrange(len(cf.Satellite_list)):
    out_dir = os.path.join(out_dir_base, cf.Satellite_list[sat])
    in_dir = os.path.join(in_dir_base, cf.Satellite_list[sat])
    in_base = "%s_%s" %(cf.Satellite_list[sat], input_base)
    out_base = "%s_%s" %(cf.Satellite_list[sat], output_base)
    command = "%s %s %s %s %s %s %s %s %s" % (proc_script, logfile_str, in_base, in_dir, var_names_file, cdl_file, out_base, out_dir, tmp_dir)

    # print command
    if options.test:
        print command
    else:
        logg = log.Log(logpath, ncvdefs.PYL_SUFFIX)
        logg.write_time("Running: %s\n" % command)
        logg.close()
        ret = os.system(command)
            
# -------------------------
