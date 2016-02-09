#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2004
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Beth Weekley @ UCAR/RAL  5/06

import os,sys
import ncvdefs
from optparse import OptionParser
import ncvdefs
import log
import string
import get_cloud_mask_files

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

proc_script = "cloud_mask_byte.py"
in_base = ncvdefs.Projection_out_base 

# --------------
# for the cloud_id code
# --------------
tmp_dir = "%s/%s" %(cf.Tmp_dir, "sat_proc")
config_file_cloud_mask = "%s/%s" %(cf.Static_satellite,"cloudmask_threshold.conf")

intermed_cdl = cf.Sat_cloud_mask_gini_interm_cdl_file
cloud_mask_cdl = cf.Sat_cloud_mask_gini_cdl_file
log_dir = cf.Log_dir
logpath = "%s/%s" % (log_dir, "cloud_mask_byte")

# -------------
# loop over the satellite list to process both east and west
# -------------
for sat in xrange(len(cf.Satellite_list)):

    # --------------
    # for the code to get the east satellite files to process 
    # --------------
    config_file = "%s/%s" %(cf.Static_satellite, cf.Satellite_config_list[sat])
    input_unproc_dir =  "%s/%s" %(cf.Satellite_goes_grid_convert, cf.Satellite_list[sat])
    input_store_dir = "%s/%s" %(cf.Satellite_goes_history, cf.Satellite_list[sat])    
    output_dir = "%s/%s/" %(cf.Satellite_cloud_mask_text, cf.Satellite_list[sat])
    output_cloud_id_file = "%s/%s" %(cf.Satellite_cloud_mask_netcdf, cf.Satellite_list[sat])
    input_base = cf.Satellite_list[sat] + "_" +  in_base

    command = "%s %s %s %s %s %s %s %s %s %s %s %s %s %s" %(proc_script, input_unproc_dir, input_store_dir, output_dir, tmp_dir, config_file, input_base, cf.Satellite_list[sat], cf.Satellite_scan_type_list[sat], log_dir, intermed_cdl, cloud_mask_cdl, output_cloud_id_file, config_file_cloud_mask) 


    if options.test:
        print command
    else:
        logg = log.Log(logpath, ncvdefs.PYL_SUFFIX)
        logg.write_time("Running: %s\n" % command)
        logg.close()
        ret = os.system(command)

# --------------
