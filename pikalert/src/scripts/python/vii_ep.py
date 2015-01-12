#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#
# This script controls execution of one or more "run" scripts which are
# executed for a particular domain defined in a config argument, and
# allowed to run a maximum allotted time.
#


import getopt
import os, sys
import string
from optparse import OptionParser


if __name__ == "__main__":

    
    # Parse command options

    usage_str = "%prog [options] config script(s) time\n\nrequired:\n  config\tbase name of python config file containing definitions\n\t\tfor NCV instance. (Omit path and .py file extension.)\n  script(s)\trun script(s) to execute, separated by spaces\n  time\t\tmaximum time (seconds) each script is allowed to run\n\nNote: If a script starts with '#' it is skipped. All scripts are run\nregardless of the exit status of previous scripts."

    parser = OptionParser(usage = usage_str)
    parser.add_option("-t", "--test", action="store_true", default=False, dest="test", help="test - echo but do not execute commands")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        print "Error: insufficient command line arguments"
        parser.print_help()
        sys.exit(2)

    # config file
    config = args[0]

    # Import the config file

    try:
        cf = __import__(config, globals(), locals(), [])
    except:
        print "Error: importing config file: %s" % config
        parser.print_help()
        sys.exit(1)


    # expiration time - verify it is numeric
    exp_time = args[-1]
    try:
	et_int = int(exp_time)
    except:
        print "Error: invalid expiration time: %s" % exp_time
        parser.print_help()
        sys.exit(1)


    # Loop through process scripts. Script names are generally run_*.py,
    # so extract the base (*) part of this.
    i = 0
    while (i < len(args) - 2):

        i = i + 1

        proc_script = args[i]
	if proc_script[0] == '#':
	    continue

	pname = string.replace(proc_script, "ep_", "", 1)
	pname = string.replace(proc_script, "run_", "", 1)
        pname = string.replace(pname, ".py", "", 1)
        log_path = ("%s/%s") % (cf.paths.LOG_DIR, pname)
        lock_path = ("%s/%s.lock") % (cf.paths.LOCK_DIR, pname)

        command_line = "ep.py -E %s -L %s -l %s -s %s %s %s" % (exp_time, lock_path, log_path, "epl", proc_script, config)
            
        if options.test:
            print "command: ", command_line
        else:
            os.system(command_line)

    
                                              
