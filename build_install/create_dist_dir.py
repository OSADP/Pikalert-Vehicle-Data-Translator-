#!/usr/bin/env python

"""Create distribution directory for tar using manifest"""

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

def create_dist(manifest_list, cvs_dir, dist_dir):

    for item in manifest_list:
        in_name = item[0]
        out_name = item[1]

        if in_name[-1] == '/':
            in_name = in_name[:-1]
        if out_name[-1] == '/':
            out_name = out_name[:-1]

        in_path = os.path.join(cvs_dir, in_name)
        out_path = os.path.join(dist_dir, out_name)
        if os.path.isdir(in_path):
            # Make sure parent dir of destination path exists before using cp -r
            parent_dir = os.path.dirname(out_path)
            if not os.path.exists(parent_dir):
                os.makedirs(parent_dir)
                
        command = "csh -c 'cp -r %s %s'" % (in_path, out_path)
        print command
        os.system(command)
            
def main():

    usage_str = "%prog manifest cvs_dir dist_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-r", "--remove", dest="remove_dest", action="store_true", help="remove destination directory")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    manifest = args[0]
    cvs_dir = args[1]
    dist_dir = args[2]
    if os.path.exists(dist_dir):
        if not options.remove_dest:
            print "Error: destination directory %s exists but -r option was not indicated" % dist_dir
            sys.exit(1)
        else:
            os.system("csh -c 'rm -rf %s'" % dist_dir)
            
    cf = __import__(manifest, globals(), locals(), [])
    create_dist(cf.Manifest_list, cvs_dir, dist_dir)
    


if __name__ == "__main__":

   main()
