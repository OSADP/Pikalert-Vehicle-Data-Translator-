#!/usr/bin/env python

""" Script for executing and monitoring a child process. """

#/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# * Copyright (c) 1995-2002 UCAR
# * University Corporation for Atmospheric Research(UCAR)
# * National Center for Atmospheric Research(NCAR)
# * Research Applications Program(RAP)
# * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# * All rights reserved. Licenced use only.
# * $Date: 2013-09-24 22:30:31 $
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


import exec_proc
from fcntl import *
import getopt
import log_msg
import os,time,mt_time
import string
import sys
import signal 
import sig_list


def kill_child():
    try:
        os.kill(-proc.get_pid(), signal.SIGTERM)
        time.sleep(2)
        os.kill(-proc.get_pid(), signal.SIGKILL)
    except:
        None
    return

def handler(signum, frame):
    """ Handles signals. """
    kill_child()
    log_file.write_time("Error: received signal %s." % sig_list.sig_list[signum])
    log_file.write_time("Ending: exit status = 1.")    
    sys.exit(1)

def usage():
    """ Prints usage statement. """
    print "usage: %s [-c child_log_path] [-e child_log_expiration] [-E process_expiration] [-h help] [-L lock_file] [-l log_path] [-r restart_script args (quotes are reqd if args are used)] [-R restart_number] [-s log_suffix] process_name [process_args]" % sys.argv[0]
    print "child_log_path: the log file path to monitor for child log expiration (only used if -e is specified)"
    print "child_log_expiration: if child_log_path has not been modified in child_log_expiration seconds, kill the child"
    print "process_expiration: if the child process has not finished in process_expiration seconds, kill the child"
    print "lock_file: if this file can't be locked, simply exit - this can be used to prevent multiple process invocations"
    print "log_path: write log messages here"
    print "restart_script: if the process does not exit with an exit code of 0, restart using this script"
    print "restart_number: maximum number of times to restart (defaults to 1)"
    print "log_suffix: suffix to append to log_path name"

if __name__ == "__main__":
    global proc	
    global log_file
    
    # Set the signal handler for SIGTERM
    signal.signal(signal.SIGTERM, handler)
    signal.signal(signal.SIGINT, handler)
    signal.signal(signal.SIGHUP, handler)
    signal.signal(signal.SIGQUIT, handler)

    # exec a process 
    arg_len = len(sys.argv)
    if arg_len == 1:
	usage()
	sys.exit(0)

    restart_number = 1
    restart_script = []
    
    try:
	optlist, args = getopt.getopt(sys.argv[1:], "c:e:E:hl:L:r:R:s:")
    except:
	print "%s: error in parsing options, %s" % (sys.argv[0], sys.exc_value)
	usage()
	sys.exit(1)
	
    #script = sys.argv[arg_len-1]
    child_log_expiration = 0
    child_path = ""
    expiration = 0
    lock_file = ""
    log_suffix = ""

    # set default log path
    #    root_dir = os.environ["DICAST_ROOT_DIR"]
    #    log_path = "%s/log/ep.log" % root_dir
    log_path = ""

    for i in xrange(0, len(optlist)):
	if optlist[i][0] == "-h":
            usage()
	elif optlist[i][0] == "-c":
	    child_path = optlist[i][1]
	elif optlist[i][0] == "-e":
	    child_log_expiration = int(optlist[i][1])
	elif optlist[i][0] == "-E":
	    expiration = int(optlist[i][1])
	elif optlist[i][0] == "-L":
	    lock_file = optlist[i][1]
	elif optlist[i][0] == "-l":
	    log_path = optlist[i][1]
	elif optlist[i][0] == "-r":
	    restart_script = string.split(optlist[i][1])
	elif optlist[i][0] == "-R":
            try:
                restart_number = int(optlist[i][1])
            except:
                print "Error: restart_number must be an integer. Exiting."
                sys.exit(0)
	elif optlist[i][0] == "-s":
	    log_suffix = optlist[i][1]

         
    # Create log file
    log_file = log_msg.LogMessage(log_path)

    if log_suffix != "":
        log_file.set_suffix(log_suffix)
        
    log_file.write_starting()
    
    if child_log_expiration != 0 and child_path == "":
        log_file.write_error("Child log expiration != 0 and child_path is empty. Exiting.")
        log_file.write_ending(0, "ep.py")
        sys.exit(0)
        
    # If locking is desired, lock the given file before continuing
    if (lock_file != ""):
        try:
            lock_fp = open(lock_file, "w+")
            flock(lock_fp.fileno(), LOCK_EX | LOCK_NB)

        except:
            sys.exit(1)

    log_file.write_info("Pid %d." % os.getpid())

    exec_list = args

    for i in xrange(restart_number):
        if i > 0:
            log_file.write_info("Restarting using %s" % exec_list[0])
            
        proc = exec_proc.Process(exec_list, log_file)

        if expiration != 0:
            proc.set_expiration(expiration)

        if child_path != "":
            proc.set_monitor_log(child_path, child_log_expiration)

        # exec the process
        proc.execp()

        # wait a bit
        time.sleep(2)

        # monitor it
        ret = proc.monitorp()    
        log_file.write_info("Monitor returned %d" % ret)
        
        if ret < 0:
            break
        
        # if there is a restart script and the process exited with a 0 return, break out
        if restart_script != []:
            exec_list = restart_script
            if ret != -1:
                if os.WIFEXITED(ret):
                    if os.WEXITSTATUS(ret) == 0:
                        break
        
    # unlock if locked
    if (lock_file != ""):
        flock(lock_fp.fileno(), LOCK_UN)
        lock_fp.close()
    	
    # kill child processes
    kill_child()

    # Return 1 if ret is non-zero, and return 0 otherwise.
    return_value = ret != 0

    # log termination
    log_file.write_ending(return_value, "ep.py")

    # terminate
    sys.exit(return_value)
    


