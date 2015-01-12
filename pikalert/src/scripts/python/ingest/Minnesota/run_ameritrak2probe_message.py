#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: run_ameritrak2probe_message.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/09/11 16:15:10 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import vii_paths

def main():

    usage_str = "%prog"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-d", "--date", dest="date", help="date")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 0:
        parser.print_help()
        sys.exit(2)

    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")

    date_str = ""
    if options.date:
        date_str = "-d %s" % options.date

    logg.write_starting()
    cdl = os.path.join(vii_paths.CDL_DIR, "probe_message.cdl")
    spn_file = os.path.join(vii_paths.STATIC_DATA_DIR, "ameritrak_pgn_list.txt")
    in_dir = os.path.join(vii_paths.RAW_DATA_DIR, "ameritrak")
    out_dir = os.path.join(vii_paths.PROCESSED_DIR, "mn_probe_message")
    log_dir = vii_paths.LOG_DIR
    log_file = "%s/ameritrak2probe_message" % log_dir

    cmd = "ameritrak2probe_message.py -l %s %s %s %s %s %s" % (log_file, date_str, in_dir, out_dir, cdl, spn_file)
    logg.write_info("%s" % cmd)
    ret = os.system(cmd)
    logg.write_ending(ret)
    return ret


if __name__ == "__main__":

   sys.exit(main())
