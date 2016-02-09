#!/usr/bin/env python

"""Convert Nevada data to probe message netcdf"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: nevada2probe_message.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013-09-11 16:21:21 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import nevada_reader
import write_probe_message_cdf

# TEST:
# nevada2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d2/vii/data/raw/nevada/20130904/nevada_20130904.csv /d4/test/nevada/nevada_20130904.nc 0
# nevada2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d4/test/nevada/nevada_20130904.csv /d4/test/nevada/nevada_20130904.nc 0


def nevada2probe_message(cdl, in_file, out_file, nevada_ret, logg):

    nevada = nevada_reader.NevadaImo2(in_file, logg)
    ret = write_probe_message_cdf.write_probe_message(cdl, nevada, out_file, nevada.Field_dict)
    if ret != 0:
        logg.write_error("error calling write_probe_message")
        logg.write_ending(1, "nevada2probe_message.py")
        nevada_ret = -1
        return -1
    else:
        nevada_ret = 0
        return 0

def main():

    usage_str = "%prog cdl in_file out_file nevada_ret"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cdl = args[0]
    in_file = args[1]
    out_file = args[2]
    nevada_ret = args[3]


    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    ret = nevada2probe_message(cdl, in_file, out_file, nevada_ret, logg)
    if ret != 0:
        logg.write_error("error calling nevada2probe_message")
        logg.write_ending(1, "nevada2probe_message.py")
        return -1        
    
    logg.write_ending()


if __name__ == "__main__":

   main()
