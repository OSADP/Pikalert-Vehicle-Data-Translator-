#!/usr/bin/env python
"""
This contains C&V system functions
"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2006
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
# Author Beth Weekley @ UCAR/RAL  5/06

import sys
import os
import string

# -------------------------
class IndexFileIo:
    """
    This is a class which has a method to parse an index file line
    and another method to write an index file line

    """
    
    def create_index_line(self, name, time):
        """
        This module creates the name and date for the index file.  
        """
        
        line = "%s %s" % (name, int(time))

        return line

    def parse_index_line(self, line):
        """
        This module returns the file name and date from the index line
        """

        sp_line = string.split(line)
        if (sp_line != []):
            name = sp_line[0]
            date = sp_line[1]

        return(name, date)
