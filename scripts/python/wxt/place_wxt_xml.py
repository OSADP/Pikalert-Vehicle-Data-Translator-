#!/usr/bin/env python

"""Identify observation date and then move file into appropriate subdirectory"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: place_wxt_xml.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/09/30 16:28:09 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import subprocess

def place_xml(in_files):
    #   <observation-time type="datetime">2012-12-19T11:23:46Z</observation-time>
    for in_file in in_files:
        date = ""
        for line in file(in_file):
            if line.find("observation-time") > 0:
                pos = line.find(">")
                date = line[pos+1:pos+5] + line[pos+6:pos+8] + line[pos+9:pos+11]
                break
        
        if date:
            if not os.path.exists(date):
                os.mkdir(date)
            elif not os.path.isdir(date):
                print "%s is not directory but file??" % date
                return -1
                
            command_list = ["/bin/mv", in_file, date]
            process = subprocess.Popen(command_list, shell=False)
            ret = process.wait()
            if ret != 0:
                command_string = " ".join(command_list)
                print "command execution failure: %s" % command_string
                return -1
            
def main():

    usage_str = "%prog in_file[s]"
    parser = OptionParser(usage = usage_str)
    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    in_files = args
    place_xml(in_files)

if __name__ == "__main__":

   main()
