#!/usr/bin/env python

"""A class and function for writing consistent log messages"""

#/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# * Copyright (c) 1995-2007 UCAR
# * University Corporation for Atmospheric Research(UCAR)
# * National Center for Atmospheric Research(NCAR)
# * Research Applications Program(RAP)
# * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# * All rights reserved. Licenced use only.
# * $Date: 2014/09/30 18:30:26 $
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


import log
import os
import sys
import subprocess

class LogError(Exception):
    """Base class for exceptions in this module."""
    pass

class RunError(LogError):
     def __init__(self, status, signal, core):
         self.status = status
         self.signal = signal
         self.core = core
         
     def __str__(self):
         return "return: %d, signal: %d, core: %d" % (self.status, self.signal, self.core)

class RunCommand(object):

    def __init__(self, log_file, test):
        self.log_file = log_file
        self.test = test

    def run(self, command):
        if self.test:
            self.log_file.write_info("Test execution: %s" % command)
        else:
            self.log_file.write_info("executing: %s" % command)
            (status, sig, core) = decode_exit(os.system(command))

            if status != 0:
                run_error = RunError(status, sig, core)
                raise run_error
            else:
                return
            
DEBUG = "Debug: "          
INFO = "Info: "
ERROR = "Error: "
WARNING = "Warning: "
STARTING = "Starting: "
ENDING = "Ending: "

class LogMessage(log.Log):

    "Provides methods for writing consistent log messages."

    def __init__(self, path, suffix=""):
        "path is the base file name to which is appended a date"
        log.Log.__init__(self, path, suffix)
        
    def write_debug(self, msg):
        "writes debug message"
        self._write_heading(DEBUG, msg)
        
    def write_info(self, msg):
        "writes info message"
        self._write_heading(INFO, msg)

    def write_error(self, msg):
        "writes error message"
        self._write_heading(ERROR, msg)

    def write_warning(self, msg):
        "writes warning message"
        self._write_heading(WARNING, msg)

    def write_starting(self, msg=""):
        "writes starting message"
        self._write_heading(STARTING, msg)

    def write_ending(self, exit_status=0, msg=""):
        "writes ending message, including given exit status"
        if msg == "":
            self.write_time("%s exit status = %d\n\n" % (ENDING, exit_status))
        else:
            self.write_time("%s%s, exit status = %d\n\n" % (ENDING, msg, exit_status))

    # "private" method
    def _write_heading(self, heading, msg):
        self.write_time("%s%s\n" % (heading, msg))

def decode_exit(exit_code):
    """Decodes the exit code returned by os.system() into a tuple
       containing the exit status, the signal, and 1 if the core was dumped.
       See os.wait() for the specification of exit_code"""

    status = exit_code >> 8		# the high byte
    signal = exit_code & 0x7f		# the lowest 7 bits
    core = (exit_code & 0xff) >> 7	# the high bit of the low byte

    return (status, signal, core)

def run(command, log_file, shell=False):
    log_file.write_info("executing: %s" % command)

    if not shell:
        args = command.split()
    else:
        args = command
        
    try:
        retcode = subprocess.call(args, shell)
        log_file.write_info("return value: %d" % retcode)

        if retcode != 0:
            return 1
        else:
            return 0
    except OSError, e:
        log_file.write_info("return value: 1")
        return 1

    
def run_command(command, log_file):
    """Runs a command and notes it in the log.  Returns the exit status of the
       command, or 1 if the command was terminated by a signal"""

    log_file.write_info("executing: %s" % command)

    (ret, sig, core) = decode_exit(os.system(command))

    if sig == 0:
        log_file.write_info("return value: %d" % ret)
    else:
        core_msg = ""
        if core:
            core_msg = ", core dumped"
        log_file.write_info("received signal %d%s" % (sig, core_msg))
    if ret != 0:
        return ret
    else:
        return sig != 0   

class Command:
    def __init__(self, test_flag=0):
        self.test_flag = test_flag

    def run(self, command, log_file):
        if self.test_flag:
            log_file.write_info("Test executing %s" % command)
            return 0
        else:
            return run(command, log_file)
            
def exit(log_file, exit_status=0):
    "Prints exiting log message and exits with the specified exit status"

    log_file.write_ending(exit_status)
    sys.exit(exit_status)


def test():
    "Logs some test messages to stdout"

    print "Logging to stdout...\n"
    l = LogMessage("")
    run("/bin/echo hello", l)
    run("echo hello", l)
    l.write_starting("log message")
    l.write("plain log mesage\n")
    l.write_time("log message with time\n")
    l.write_info("informative message")
    l.write_warning("warning message")
    l.write_error("error message")
    run_command("echo hello", l)
    run_command("ls xyzabc", l)
    run_command("ls -qxwp", l)
    l.write_ending(0, "log message")

    rc = RunCommand(l, 1)
    rc.run("testCommand")
    rc = RunCommand(l, 0)
    try:
        rc.run("grepppp")
    except RunError, inst:
        l.write_error(inst)
    try:
        l.write_info("trying to mkdir /funny")
        os.mkdir("/funny")
    except OSError, inst:
        l.write_error(inst)
        
    l.write_starting()
    ret = run_command("grepp", l)
    exit(l, ret)   

if __name__ == '__main__':
    test()
     
