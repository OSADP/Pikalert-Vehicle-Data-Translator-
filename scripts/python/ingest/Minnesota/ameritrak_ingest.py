#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: ameritrak_ingest.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013-09-11 16:06:32 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
from daemon import Daemon
import time
import socket
import signal

class AmeritrakIngest(Daemon):
    def __init__(self, pid_file, addr, port, user, passwd, out_dir, logg):
        """initialize daemon processing"""
        Daemon.__init__(self, pid_file, stdout="/dev/null", stderr="/dev/null")
        self.addr = addr
        self.port = port
        self.user = user
        self.passwd = passwd
        self.out_dir = out_dir
        self.running = False
        self.logg = logg
        self.last_ping_time = 0.0

    def readline(self, sock):
        """read a line of input from the socket"""
        line = ""
        while True:
            try:
                c = sock.recv(1)
            except (Exception, socket.timeout), e:
                self.logg.write_info("caught %s" % str(e))
                return None
            if c == '\r':
                continue
            if c == '\n':
                return line
            line += c
        return None

    def get_out_path(self, mod = 300):
        """create dated output file path"""
        ptime = time.mktime(time.gmtime())
        ptime = ptime - (ptime % mod)
        time_tup = time.gmtime(ptime)
        day_str = time.strftime("%Y%m%d", time_tup)
        hhmm = time.strftime("%H%M", time_tup)
        out_day_dir = os.path.join(self.out_dir, day_str)
        if not os.path.exists(out_day_dir):
            cmd = "mkdir -p %s" % out_day_dir
            self.logg.write_info(cmd)
            os.system(cmd)
        fname = "ameritrak.%s.%s.txt" % (day_str, hhmm)
        fpath = os.path.join(out_day_dir, fname)
        return fpath

    def run(self):
        """Create socket if necessary. Otherwise try to ingest new data. Append any new data to a file."""
        line_count = 0
        self.logg.write_starting("")
        sock = None
        self.running = True
        def stop(sig,func=None):
            self.logg.write_info("closing connection...")
            #sock.send(">Unsubscribe:;\r\n")
            #sock.send(">Logoff:;\r\n")
            sock.close()
            self.running = False
        signal.signal(signal.SIGTERM, stop)
        exit_code = 0
        while self.running:
            if sock == None:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(70.0)
                self.logg.write_info("connecting to %s:%s" % (self.addr, self.port))
                sock.connect((self.addr,self.port))
                sock.send(">Logon:%s,%s;\r\n" % (self.user,self.passwd))
                time.sleep(2)
                sock.send(">Subscribe:;\r\n")
            line = self.readline(sock)
            if line == None:
                exit_code = 1
                cur_time = time.mktime(time.gmtime())
                if (cur_time - self.last_ping_time) > 45:
                    self.logg.write_error("connection may have failed, no ping in over 45 seconds, restarting...")
                break
            if "Ping" in line:
                un = line[6:]
                sock.send(">Pong:%s\r\n" % un)
                self.last_ping_time = time.mktime(time.gmtime())
                self.logg.write_info("received ping at %s, line_count = %d" % (time.strftime("%Y-%m-%d %H:%M:%S"), line_count))
                continue
            line_count += 1
            fpath = self.get_out_path()
            f = open(fpath, "a")
            f.write("%s\n" % line)
            f.close()
        self.logg.write_ending(exit_code)

def main():

    usage_str = "%prog config [start|stop|restart]"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cf = cf = __import__(args[0], globals(), locals(), [])
    act = args[1]
    pid_file = cf.Ameritrak_ingest_pid_file

    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")

    d = AmeritrakIngest(pid_file, cf.Ameritrak_ip, cf.Ameritrak_port, cf.Ameritrak_user, cf.Ameritrak_pwd, cf.Ameritrak_out_dir, logg)
    if "start" == act:
        d.start()
    elif "restart" == act:
        d.restart()
    elif "stop" == act:
        d.stop()

if __name__ == "__main__":

   main()
