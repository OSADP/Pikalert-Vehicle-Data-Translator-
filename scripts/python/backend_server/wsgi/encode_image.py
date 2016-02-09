#!/usr/bin/env python

"""Encode image file as base64"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser

def encode_image(in_file):
    """
    Parameters
    ----------
    in_file : str input file name string

    Returns
    -------
    int
        0 on success and -1 on failure
    """

    with open(in_file, "rb") as fp:
        image_data = fp.read()
        encoded_data = image_data.encode("base64")
    return encoded_data

def main():

    usage_str = "%prog in_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    in_file = args[0]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    print encode_image(in_file)
    logg.write_ending()

if __name__ == "__main__":

   main()
