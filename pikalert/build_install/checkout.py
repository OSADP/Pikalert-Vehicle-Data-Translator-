#!/usr/bin/env python

"""Checkout script"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2007
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import log_msg
import os
import sys
from optparse import OptionParser

def checkout(manifest_list, cvs_dir):

    os.chdir(cvs_dir)

    for item in manifest_list:
        os.system("csh -c 'cvs co %s'" % item[0])
            
        
def main():

    usage_str = "%prog manifest cvs_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-r", "--remove", dest="remove_dest", action="store_true", help="remove cvs checkout directory")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    manifest = args[0]
    cvs_dir = args[1]

    if os.path.exists(cvs_dir):
        if not options.remove_dest:
            print "Error: destination directory %s exists but -r option was not indicated" % cvs_dir
            sys.exit(1)
        else:
            os.system("csh -c 'rm -rf %s'" % cvs_dir)
            
    
    os.mkdir(cvs_dir)
    cf = __import__(manifest, globals(), locals(), [])
    checkout(cf.Manifest_list, cvs_dir)
    


if __name__ == "__main__":

   main()
