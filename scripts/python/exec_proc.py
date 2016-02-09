#!/usr/bin/env python
#/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# * Copyright (c) 1995-2002 UCAR
# * University Corporation for Atmospheric Research(UCAR)
# * National Center for Atmospheric Research(NCAR)
# * Research Applications Program(RAP)
# * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# * All rights reserved. Licenced use only.
# * $Date: 2013-09-24 22:32:08 $
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


"""

Class for execute processes and monitoring them 

 Inputs:
    Process string with list of arguments.
    Log file

 Algorithm:
    Read command line arguments
    Fork
    Execute process
    If the child process runs too long, kill it
    When the child process exits, exit

"""

import os, sys
import log
import log_msg
import signal
import time

def terminate():
    sys.exit(-1)

class Process:
    """Class for executing and monitoring individual processes"""
    def __init__(self, args, log_file):
        self.args = args
        self.expiration = 0
        self.monitor_log_file = ""
        self.monitor_log_time = time.time()
        self.monitor_log_expiration = 0
        self.kill_sleep = 5
        self.monitor_sleep = 2
        self.pid = -1
        self.log = log_file
        

    def get_pid(self):
        return(self.pid)

    def set_expiration(self, expiration):
        self.expiration = expiration

    def set_monitor_log(self, monitor_log_file, monitor_log_expiration):
        self.monitor_log_file = monitor_log_file
        self.monitor_log_expiration = monitor_log_expiration
        
    # exec the process specified by self.args
    def execp(self):
        self.pid = os.fork()
        if self.pid < 0:
            self.log.write_error("Couldn't fork. Terminating.")
            terminate()

        if self.pid == 0:
            try:
                # Child - set the process group id for the child and all its children
                # to be equal to the pid of the child.  This will be used for process cleanup if
                # necessary
                os.setpgid(0, 0)

                # Exec the child process
                ret = os.execvp(self.args[0], self.args)

            except:
                self.log.write_error("Couldn't exec. Terminating.")
                terminate()
                
        else:
            # Parent - monitor the child
            self.start_time = time.time()
            self.log.write_info("Exec'd child, %s, pid %d." % (self.args[0], self.pid))

    # monitor the process specified by self.args
    def monitorp(self):            
        options = os.WNOHANG
        loop = 1
        while loop:
            try:
                ret = os.waitpid(self.pid, options)
                if ret[0] == self.pid:
                    if os.WIFEXITED(ret[1]):
                        self.log.write_info("Child %d exited, status = %d." % (self.pid, os.WEXITSTATUS(ret[1])))
                    elif os.WIFSIGNALED(ret[1]):
                        self.log.write_info("Child %d exited due to signal %d." % (self.pid, os.WTERMSIG(ret[1])))
                    else:
                        log_file.write_info("Child %d exited abnormally." % self.pid )
                    return(ret[1])

                if self.expiration > 0:
                    # check whether child has been running too long
                    if time.time() - self.start_time > self.expiration:
                        os.kill(self.pid, signal.SIGTERM)
                        ret = os.waitpid(self.pid, options)

                        # wait a bit and then kill again with SIGKILL to ensure the kill
                        time.sleep(self.kill_sleep)
                        os.kill(self.pid, signal.SIGKILL)
                        self.log.write_error("Process %s, pid %d, ran past expiration time. Terminating." % (self.args[0], self.pid))
                        return(-1)

                if self.monitor_log_file != "" and self.monitor_log_expiration > 0:
                    # check whether child has modified its log file recently
                    timeval = time.time()
                    mod_time = log.log_mod_time(self.monitor_log_file, timeval)
                    if mod_time > 0:
                        self.monitor_log_time = mod_time
                        
                    if timeval - self.monitor_log_time > self.monitor_log_expiration:
                        #print "timeval %d, mod_time %d, timeval - mod_time %d" % (timeval, mod_time, timeval - mod_time)
                        os.kill(self.pid, signal.SIGTERM)
                        ret = os.waitpid(self.pid, options)

                        # wait a bit and then kill again with SIGKILL to ensure the kill
                        time.sleep(self.kill_sleep)
                        os.kill(self.pid, signal.SIGKILL)
                        self.log.write_error("Process %s, pid %d, log file stale. Terminating." % (self.args[0], self.pid))
                        return(-1)
                    
            except:
                self.log.write_error("Process %s, pid %d: failure in waitpid() section. Exception %s, %s. Terminating." % (self.args[0], self.pid, sys.exc_type, sys.exc_value))
                return(-1)

            if loop == 1:
                time.sleep(self.monitor_sleep)

        return(0)

def log_test():
    # exec a process to touch a file
    args = ['sleep.py']
    log_path = "log_file"
    log_file = log_msg.LogMessage(log_path)
    log_file.write_starting()
    log_file.write_info("log test")

    proc = Process(args, log_file)
    proc.set_monitor_log("child_log", 3)
                    
    # exec the process
    proc.execp()

    # monitor it
    ret = proc.monitorp()
    log_file.write_info("monitor returned %d" % ret)
    log_file.write_ending(0, "exec_proc.py")

def touch_test():
    """ test involving touching files """
    args = ['touch', 'afile']
    log_path = "log_file"
    log_file = log_msg.LogMessage(log_path)
    log_file.write_starting()
    log_file.write_info("touch test")

    proc = Process(args, log_file)
    
    # exec the process
    proc.execp()

    # monitor it
    ret = proc.monitorp()
    log_file.write_info("monitor returned %d" % ret)
    log_file.write_ending(0, "exec_proc.py")

def sleep_test():
    """ test involving sleeping and failing the expiration time condition """
    args = ['sleep.py']
    log_path = "log_file"
    log_file = log_msg.LogMessage(log_path)
    log_file.write_starting()
    log_file.write_info("Sleep test")
    proc = Process(args, log_file)
    proc.set_expiration(3)
    
    # exec the process
    proc.execp()

    # monitor it
    ret = proc.monitorp()
    log_file.write_info("monitor returned %d" % ret)
    log_file.write_ending(0, "exec_proc.py")
    
def no_kill_test():
    """ exec a process that can't be killed using SIGTERM to pause and fail the expiration time test """
    args = ['no_kill.py']
    log_path = "log_file"
    log_file = log_msg.LogMessage(log_path)

    log_file.write_starting()
    log_file.write_info("no kill test")

    proc = Process(args, log_file)
    proc.set_expiration(5)
    
    # exec the process
    proc.execp()

    # monitor it
    ret = proc.monitorp()
    log_file.write_info("monitor returned %d" % ret)
    log_file.write_ending(0, "exec_proc.py")
    
if __name__ == "__main__":
    log_test()
    no_kill_test()
    touch_test()
    sleep_test()

