#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/02/02 20:17:38 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import os
import string
import sys
import time
import sys_path
import log
import fname
import index_file
import tim
import get_file

# Generic script for running processes. Allows multiple input file
# directories, and a list of static files with a single output file directory.
# Also allows optional log file base and TDRP params file name. The command
# line which is executed follows the form:
#
# cmd [unix_time(s)] [static_file(s)] input_file(s) output_file [-l log_base] [-p params]
#
# or
#
# cmd [unix_time(s)] input_file(s) [static_file(s)] output_file [-l log_base] [-p params]
#
# Arguments and options can be mixed in any order. However the relative
# position of the input dirs and static files is retained when building the
# command string. If multiple input and or static files are listed they will
# be added to the command line in the order given.
#
# Additional options can be forwarded to 'cmd' using '-f'. All text following
# the -f option is passed as-is to cmd without interpretation. Hence, it must
# be the last option specified on the command line.
#

def usage(prog_name):
    print "Usage: %s [-h] [-d date] [-hms] [-l log_base] [-L] [-p params_file] [-T seconds] [-t] [-u time] [-x suffix] -a max[:min](s) -e cmd, [-s static_file(s)] -i input_dir(s) -o output_dir -f args...\n" % prog_name
    print "Description: Generalized script for running a process.\n"
    print "Required arguments:"
    print "-a max[:min](s)\tmax and min age for each of the input file dir(s) in seconds."
    print "\t\tmin may be omitted, in which case the current time is used."
    print "-e cmd\t\tcommand to execute"
    print "-i dir(s)\tone or more input file directories"
    print "-o output_dir\toutput file directory"
    print "\nOptions:"
    print "-h\t\tprint this help message"
    print "-d date\t\tuse date (YYYYMMDD.HHMMSS) as current time"
    print "-hms\t\tuse HHMMSS format for time in output file (default is HHMM)"
    print "-l log_base\tbase path for log file"
    print "-L\t\tdo NOT include log path when executing cmd"
    print "-p params_file\tTDRP params file"
    print "-s file(s)\tone or more static files"
    print "-T seconds\ttruncate output file time to this number of seconds (default=1)"
    print "-t\t\ttesting - echo command line but do not execute"
    print "-u time(s)\tinclude 'time(s)' on command line as the fcst time"
    print "-x suffix\tuse suffix for output file instead of %s" % sys_path.NC_SUFFIX
    print "-f args...\tforwards all remaining args to cmd as-is. MUST BE LAST OPTION SPECIFIED."


if __name__ == "__main__":

    utime = ""
    testing = 0
    log_base = ""
    use_log = 1
    input_dirs = []
    static_files = ""
    params_file = ""
    inputs_first = 0
    fcst_times = ""
    forward_args = ""
    min_age = []
    max_age = []
    suffix = sys_path.NC_SUFFIX
    time_len = 4
    truncate_sec = 1

    # Check that required args are present
    req_args = ["-a", "-e", "-i", "-o"]
    for arg in req_args:
        try:
            i = sys.argv.index(arg)
        except:
            print "Required argument %s is missing" % arg
            usage(sys.argv[0])
            sys.exit(2)

    # Determine input dirs & static files order
    for arg in sys.argv:
        if arg == "-i":
            inputs_first = 1
            break
        elif arg == "-s":
            break
 
    # Get command line args
    arg_len = len(sys.argv)
    i = 1
    while (i<arg_len):
        if sys.argv[i] == "-a":
            while (i<arg_len and sys.argv[i+1][0] != "-"):
                age = string.split(sys.argv[i+1], ":", 2)
                max_age.append(int(age[0]))
                if len(age) == 2:
                    min_age.append(int(age[1]))
                else:
                    min_age.append(0)                    
                i = i + 1
        elif sys.argv[i] == "-d":
            utime = sys.argv[i+1]
            if len(utime) != 15:
                print "%s: error - incorrect date format" % sys.argv[0]
                usage(sys.argv[0])
                sys.exit(2)
        elif sys.argv[i] == "-e":
            cmd = sys.argv[i+1]
        elif sys.argv[i] == "-f":
            for a in sys.argv[i+1:]:
                forward_args = forward_args + a + " "
            break
        elif sys.argv[i] == "-hms":
            time_len = 6
        elif sys.argv[i] == "-h":
            usage(sys.argv[0])
            sys.exit(2)
        elif sys.argv[i] == "-i":
            while (i<arg_len and sys.argv[i+1][0] != "-"):
                input_dirs.append(sys.argv[i+1])
                i = i + 1
        elif sys.argv[i] == "-l":
            log_base = sys.argv[i+1]
        elif sys.argv[i] == "-L":
            use_log = 0
        elif sys.argv[i] == "-n":
            normalize_time = 1
        elif sys.argv[i] == "-o":
            output_dir = sys.argv[i+1]
        elif sys.argv[i] == "-p":
            params_file = sys.argv[i+1]
        elif sys.argv[i] == "-s":
            while (i<arg_len and sys.argv[i+1][0] != "-"):
                static_files = "%s %s" % (static_files, sys.argv[i+1])
                i = i + 1
        elif sys.argv[i] == "-T":
            truncate_sec = int(sys.argv[i+1])
        elif sys.argv[i] == "-t":
            testing = 1
        elif sys.argv[i] == "-u":
            while (i<arg_len and sys.argv[i+1][0] != "-"):
                fcst_times = "%s %s" % (fcst_times, sys.argv[i+1])
                i = i + 1
        elif sys.argv[i] == "-x":
            suffix = sys.argv[i+1]

        i = i + 1

    # Set up log file
    if log_base:
        logf = log.Log(log_base)
        logf.set_suffix(sys_path.LOG_PY_SUFFIX)
        log_str = "-l %s" % log_base
    else:
        logf = log.Log("")
        log_str = ""

    logf.write_time("Starting %s\n" % sys.argv[0])

    # Set up strings for user-supplied date and real-time
    if utime:
        curr_date = utime[:8]
        curr_time = utime[9:]
    else:
	curr_date = time.strftime("%Y%m%d", time.gmtime(time.time()))
        curr_time = time.strftime("%H%M%S", time.gmtime(time.time()))

    # Create a Unix time from date/time info
    dt = "%s.%s" % (curr_date, curr_time)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)


    # Determine the input file list by getting the latest files
    # using information from the input list
    input_file_list = ""
    for i in xrange(len(input_dirs)):
        index = index_file.Index_file(input_dirs[i], sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
        file = get_file.get_last_file(index, time_val - max_age[i], time_val - min_age[i])
        if (file != ""):
            input_file_list = "%s %s" % (input_file_list, file)
	else:
            input_file_list = "%s %s" % (input_file_list, "None")

    if input_file_list == "":
        logf.write_time("Error: No input files available\n")
        logf.write_time("Ending. Exit status = 1.\n")
        sys.exit(1)


    # Create the output time info, truncated to number of seconds
    out_time_val = int(time_val/truncate_sec) * truncate_sec
    out_date = time.strftime("%Y%m%d", time.gmtime(out_time_val))
    out_time = time.strftime("%H%M%S", time.gmtime(out_time_val))

    # Build output file name path
    base_name = os.path.basename(output_dir)
    ofo = fname.data_fname_obj(base_name, "", suffix)
    output_name = ofo.make_name(out_date, out_time[:time_len])
    dest_dir = os.path.join(output_dir, out_date)
    output_path = os.path.join(dest_dir, output_name)

    # Build command string
    command_str = cmd
    if fcst_times:
        command_str = "%s %s" % (command_str, fcst_times)
        
    if inputs_first:
        command_str = "%s %s %s %s" % (command_str, input_file_list, static_files, output_path)
    else:
        command_str = "%s %s %s %s" % (command_str, static_files, input_file_list, output_path)

    if params_file:
        command_str = "%s -params %s" % (command_str, params_file)
 
    if log_base and use_log:
        command_str = "%s -l %s" % (command_str, log_base)
 
    if forward_args:
        command_str = "%s %s" % (command_str, forward_args)


    # Make dest_dir if needed
    if os.path.isdir(dest_dir) == 0:
        logf.write_time("Info: Making directory %s\n" % dest_dir)
        if not testing:
	    try:
                os.makedirs(dest_dir)
            except:
                logf.write_time("Error: Could not make directory %s\n" % dest_dir)
                logf.write_time("Ending. Exit status = 1.\n")
                sys.exit(1)

    # Execute the command
    logf.write_time("Executing: %s\n" % command_str)
    if not testing:
        ret = os.system(command_str)
        if os.WIFSIGNALED(ret):
            logf.write_time("Error: Command failed due to signal %d.\n" % os.WTERMSIG(ret))
            logf.write_time("Ending. Exit status = 1.\n")
            sys.exit(1)
        elif os.WIFEXITED(ret):
            if os.WEXITSTATUS(ret) != 0:
                logf.write_time("Error: Command exited with status %d.\n" % os.WEXITSTATUS(ret))
                logf.write_time("Ending. Exit status = 1.\n")
                sys.exit(1)

    
    # Update the output index file
    if not testing:
        output_index = index_file.Index_file(output_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
        out_line = "%s %d" % (output_name, out_time_val)
        if (output_index != ""):
            output_index.write(out_line, out_time_val)
     
    logf.write_time("Ending. Exit status = 0.\n")
    sys.exit(0)

