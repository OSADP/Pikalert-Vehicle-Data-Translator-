#!/usr/bin/env python

"""Convert umtri data to probe message netcdf"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: umtri2probe_message.py,v $                                           #
#       Version: $Revision: 1.5 $  Dated: $Date: 2014/06/18 20:08:47 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import umtri_reader
import write_probe_message_cdf

# TEST:
# /d1/local/enthought/bin/python umtri2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d2/vii/data/raw/umtri/20130603/1FTNF2_03062013_154040_v3.5.10.csv /d4/test/umtri/1FTNF2_03062013_154040_v3.5.10.nc

# /d4/test/umtri/zero_size/umtri2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d4/test/umtri/zero_size/7346459475_05192014_093832_v3.7.1.csv /d4/test/umtri/zero_size/7346459475_05192014_093832_v3.7.1.nc 0

# /d4/test/umtri/zero_size/umtri2probe_message.py /d2/vii/data/static/cdl/probe_message.cdl /d4/test/umtri/zero_size/1000007INRIX_05192014_150803_v3.7.1.csv /d4/test/umtri/zero_size/1000007INRIX_05192014_150803_v3.7.1.nc 0

def umtri2probe_message(cdl, in_file, out_file, umtri_ret, logg):
    
    umtri = umtri_reader.UmtriImo2(in_file, logg)
    if umtri.get_latitude() != None:
        ret = write_probe_message_cdf.write_probe_message(cdl, umtri, out_file, umtri.Field_dict)
        if ret != 0:
            logg.write_error("error calling write_probe_message")
            logg.write_ending(1, "umtri2probe_message.py")
            umtri_ret = -1
            return -1
        else:
            umtri_ret = 0
            return 0
    else:
        logg.write_error("Problem with input file.")
        logg.write_ending(1, "umtri2probe_message.py")
        umtri_ret = -1
        return -1
        
def main():

    usage_str = "%prog cdl in_file out_file umtri_ret"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    cdl = args[0]
    in_file = args[1]
    out_file = args[2]
    umtri_ret = args[3]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()

    ret = umtri2probe_message(cdl, in_file, out_file, umtri_ret, logg)
    if ret != 0:
        logg.write_error("error calling umtri2probe_message")
        logg.write_ending(1, "umtri2probe_message.py")
        return -1        
    
    logg.write_ending()

if __name__ == "__main__":

   main()
