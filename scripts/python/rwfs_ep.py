#!/usr/bin/env python

import string
import getopt
import os
import sys
import sys_path
import re


def usage():
    print "usage: %s [-h] [-t] process_script(s)... expiration_time" % sys.argv[0]
    print "Main process execution script for RWFS"
    print "-h\t\tprint this message"
    print "-t\t\ttesting mode"
    print "process_script\tone or more scripts to run, separated by spaces"
    print "expiration_time\ttime script will be allowed to run (seconds)"


if __name__ == "__main__":
    
    testing = 0
    optlist = []
    args = []

    #
    # parse command line arguments
    #
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "ht")
    except:
	print "%s: Error in parsing options, %s" % (sys.argv[0], sys.exc_value)
	usage()
	sys.exit(2)
	
    if (len(args) < 2):
	print "%s: Not enough arguments on command line" % sys.argv[0]
        usage()
        sys.exit(2)
            
    optlist_len = len(optlist)
    for i in xrange(0,optlist_len):
	if optlist[i][0] == "-t":
            testing = 1
        elif optlist[i][0] == "-h":
            usage()
            sys.exit(2)


    #
    # Process expiration time
    #
    exp_time = args[-1]

    #
    # Loop through args (except the last one which is the
    # expiration time). Parse scripts and script arguments.
    #
    i = 0
    while ( i < len(args) - 1):
        #
        # Check for python script suffix:
        #
        if args[i][-3:] == '.py':

            proc_script = args[i]
            
            #
            # Create process base. Remove leading 'ep_'. 
            #
            if proc_script[:3] == 'ep_':
                proc_base = proc_script[3:-3]
            else:
                proc_base = proc_script[:-3]

            #
            # Get args for proc_script
            #
            i = i + 1
            
            p_args = ""
            l_args = ""
             
            while args[i][-3:] != '.py' and i < len(args) - 1:

                p_args = p_args + args[i] + " "
                l_args = l_args + args[i]
                
                i = i + 1


            if len(l_args) > 0:
                l_args = l_args + "_"
                
            log_path = ("%s/%s%s") % (sys_path.Log_dir, l_args, proc_base)

            lock_path = ("%s/%s%s.lock") % (sys_path.Lock_dir, l_args, proc_base)
            
            #
            # Execute proc_script as a child process of ep.py
            #
            command_line = "ep.py -E %s -L %s -l %s -s %s %s %s " % (exp_time, lock_path, log_path, sys_path.LOG_EP_SUFFIX, proc_script, p_args)
            
            if testing:
                print "command: ", command_line
            else:
                os.system(command_line)

