#!/usr/bin/env python

"""Call get_unlim and collect results"""

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
import subprocess

def main():

    usage_str = "%prog in_file(s)"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    cmd = "get_unlim %s" % " ".join(args)
    bufsize = len(args) * 32
    pipe = subprocess.Popen(cmd, shell=True, bufsize=bufsize, stdout=subprocess.PIPE).stdout
    lines = pipe.readlines()
    print map(int, lines[0].split())
    
if __name__ == "__main__":

   main()
       

    
