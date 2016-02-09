#!/usr/bin/env python

"""Convert generic data to probe message netcdf"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: generic2probe_message.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/08/20 15:59:40 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import generic_reader
import write_probe_message_cdf

# TEST:
# /d1/local/enthought/bin/python generic2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d2/vii/data/raw/generic/20130603/1FTNF2_03062013_154040_v3.5.10.csv /d4/test/generic/1FTNF2_03062013_154040_v3.5.10.nc

def generic2probe_message(cdl, in_file, out_file, logg):
    
    generic = generic_reader.GenericProbeReader(in_file, logg)
    if generic.empty == True:
        logg.write_error("not writing output file %s since input is empty" % out_file)
        logg.write_ending(1, "generic2probe_message.py")
        return -1
    else:
        logg.write_info("writing output file %s" % out_file)
        ret = write_probe_message_cdf.write_probe_message(cdl, generic, out_file, generic.Field_dict)
        if ret != 0:
            logg.write_error("error calling write_probe_message")
            logg.write_ending(1, "generic2probe_message.py")
            return -1
        else:
            return 0
        
def main():

    usage_str = "%prog cdl in_file out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cdl = args[0]
    in_file = args[1]
    out_file = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    ret = generic2probe_message(cdl, in_file, out_file, logg)
    if ret != 0:
        logg.write_error("error calling generic2probe_message")
        logg.write_ending(1, "generic2probe_message.py")
        return -1        
    
    logg.write_ending()

if __name__ == "__main__":

   main()
