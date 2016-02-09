#!/usr/bin/env python

"""Convert Alaska data to probe message netcdf"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: alaska2probe_message.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/12/18 18:41:01 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import alaska_reader
import write_probe_message_cdf

# TEST:
# alaska2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /home/vii/tmp/test_alaska.20131217.csv /home/vii/tmp/alaska_20131217.nc 0
# alaska2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /home/vii/tmp/alaska.20131217.csv /home/vii/tmp/alaska_20131217.nc 0

def alaska2probe_message(cdl, in_file, out_file, alaska_ret, logg):

    alaska = alaska_reader.AlaskaImo2(in_file, logg)
    print "CHECK"
    ret = write_probe_message_cdf.write_probe_message(cdl, alaska, out_file, alaska.Field_dict)
    if ret != 0:
        logg.write_error("error calling write_probe_message")
        logg.write_ending(1, "alaska2probe_message.py")
        alaska_ret = -1
        return -1
    else:
        alaska_ret = 0
        return 0

def main():

    usage_str = "%prog cdl in_file out_file alaska_ret"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cdl = args[0]
    in_file = args[1]
    out_file = args[2]
    alaska_ret = args[3]


    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    ret = alaska2probe_message(cdl, in_file, out_file, alaska_ret, logg)
    if ret != 0:
        logg.write_error("error calling alaska2probe_message")
        logg.write_ending(1, "alaska2probe_message.py")
        return -1        
    
    logg.write_ending()


if __name__ == "__main__":

   main()
