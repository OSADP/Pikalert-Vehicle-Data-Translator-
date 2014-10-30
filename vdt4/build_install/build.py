#!/usr/bin/env python

"""Build system using distribution directory and manifest"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2007
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import os
import sys
from optparse import OptionParser

def build(build_list, build_command):

    cwd = os.getcwd()
    for dirpath in build_list:
        os.chdir(dirpath)
        print "csh -c 'install %s'" % dirpath
        os.system(build_command)
        os.chdir(cwd)


def copy_dir(source_list, dest_dir):
    """Copy directories from source_list to dest_dir"""

    for item in source_list:
        os.system("csh -c 'cp -r %s %s'" % (item, dest_dir))
    

    
def main():

    usage_str = "%prog manifest distribution_dir code_dir data_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-c", "--clean", dest="clean", action="store_true", help="clean instead of build")
    parser.add_option("-s", "--scons", dest="scons", action="store_true", help="use scones instead of cmake")
   
    (options, args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    manifest = args[0]
    dist_dir = args[1]
    code_dir = args[2]
    data_dir = args[3]
    
    cf = __import__(manifest, globals(), locals(), [])

    # Create directory list from manifest and dist_dir
    build_list = []
    for pairs in cf.Build_list:
        dirpath = os.path.join(dist_dir, pairs[1])
        build_list.append(dirpath)

    if not options.scons:

        command = "cmake .; make install"
        if options.clean:
            command = "make clean"

    else:

        command = "scons -Q install"        
        if options.clean:
            command = "scons -c"
        
    build(build_list, command)

    if not options.clean:

        if not os.path.exists(code_dir):
            os.mkdir(code_dir)

        if not os.path.exists(data_dir):
            os.mkdir(data_dir)


        code_list = []
        for pairs in cf.Code_list:
            dirpath = os.path.join(dist_dir, pairs[1])
            code_list.append(dirpath)

        copy_dir(code_list, code_dir)


        data_list = []
        for pairs in cf.Data_list:
            dirpath = os.path.join(dist_dir, pairs[1])
            data_list.append(dirpath)

        copy_dir(data_list, data_dir)


if __name__ == "__main__":

   main()
