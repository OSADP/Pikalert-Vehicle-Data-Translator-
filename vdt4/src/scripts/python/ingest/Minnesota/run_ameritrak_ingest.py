#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_ameritrak_ingest.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/09/11 16:06:32 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import vii_paths

def main():

    usage_str = "%prog config"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage(vii_paths.Run_ameritrak_ingest_log_file)

    logg.write_starting()

    cfg = __import__(args[0], globals(), locals(), [])
    acf = __import__("ameritrak_config", globals(), locals(), [])
    log_dir = vii_paths.LOG_DIR

    try:
        pidf = open(acf.Ameritrak_ingest_pid_file,"r")
        pid_line = pidf.readline().strip()
        pid = int(pid_line)
        os.kill(pid,0)
    except:
        cmd = "ameritrak_ingest.py -l %s/ameritrak_ingest ameritrak_config start" % log_dir
        logg.write_info("process not running, restarting...")
        logg.write_info(cmd)
        os.system(cmd)

    logg.write_ending(0)

if __name__ == "__main__":

   main()
