#!/usr/bin/env python

import os
import sys
from optparse import OptionParser
import time
import log


# Parse command options

usage_str = "%prog [options] config\n\nrequired:\n  config\t\tbase name of python config file containing definitions\n\t\t\tfor instance. (Omit path and .py file extension.)"

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


# Process date, if any (not used here)

ptime = time.time()
date_str = ""
if options.date != None:
    date_str = "-d %s" % (options.date)
    ptime = time.mktime(time.strptime(options.date,"%Y%m%d.%H%M"))


# Set up command line arguments

proc_script = "metar2nc.pl"

logpath = cf.Log_dir  # The decoder script will create the log filename
#sitefile = cf.Metar_site_list_file
sitefile = cf.NA_site_list_file
tmpdir = cf.Tmp_dir
cdlfile = cf.Metar2nc_cdl_file
indir = cf.Raw_metar_ascii_dir

# Set up command line to decode the current and last hour's data
pres_tup = time.gmtime(ptime)
curfile = os.path.join(indir, time.strftime("%Y%m%d_%H.metar", pres_tup))

rawfiles = ""
if os.path.exists(curfile):
    rawfiles = curfile
  
prev_tup = time.gmtime(ptime-3600)
prevfile = os.path.join(indir, time.strftime("%Y%m%d_%H.metar", prev_tup))


if os.path.exists(prevfile):
    rawfiles = rawfiles + " " + prevfile

if rawfiles == "":
    if options.test:
        print "No recent input files found."
    else:
        logg = log.Log(os.path.join(logpath, "metar2nc"), "pyl")
        logg.write_time("Error: No recent input files found.\n")

    sys.exit(1)


# Create output filename if using the -f option
fname = time.strftime("metar.%Y%m%d.%H%M.nc", pres_tup)
dated_dir = time.strftime("%Y%m%d", pres_tup)
outdir = os.path.join(cf.Raw_metar_netcdf_dir, dated_dir)
outfile = os.path.join(outdir, fname)
tmpfile = os.path.join(tmpdir, fname)

if not os.path.isdir(outdir):
    os.makedirs(outdir)

# Use this form to decode to output files with names chosen by the decoder
#cmd_args = "-T %s -l %s -s %s -d %s -c %s" % (tmpdir, logpath, sitefile, outdir, cdlfile)

# This decodes all obs to a specific file
#cmd_args = "-T %s -l %s -s %s -f %s -c %s" % (tmpdir, logpath, sitefile, tmpfile, cdlfile)
if options.date != None:
    cmd_args = "-T %s -l %s -s %s -f %s -c %s -y %s" % (tmpdir, logpath, sitefile, tmpfile, cdlfile, options.date[0:4])
else:
    cmd_args = "-T %s -l %s -s %s -f %s -c %s" % (tmpdir, logpath, sitefile, tmpfile, cdlfile)

command = "cat %s | %s %s" % (rawfiles, proc_script, cmd_args)

if options.test:
    print command
else:
    logg = log.Log(os.path.join(logpath, "metar2nc"), "pyl")
    logg.write_time("Running: %s\n" % command)
    ret = os.system(command)
    if (ret != 0):
        sys.exit(1)
    else:
        command = "mv -f %s %s" % (tmpfile, outfile)
        logg.write_time("Executing: %s\n" % command)
        ret = os.system(command)
        if (ret != 0):
            sys.exit(1)
