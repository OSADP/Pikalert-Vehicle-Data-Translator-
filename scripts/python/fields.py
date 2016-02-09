#!/usr/bin/env python

"""Fields class"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fields.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/10/24 20:18:32 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser

class Fields(object):
    """A container class to support generic netcdf IO"""
    
    def __init__(self, data, names, types, units, missing, sizes):

        self._data = data
        self._names = names
        self._types = types
        self._units = units
        self._missing = missing
        self._sizes = sizes
        self._missing_dict = {}
        for ind in range(len(names)):
            self._missing_dict[names[ind]] = missing[ind]
            
    def __eq__(self, other):
        return (self._data, self._names, self._types, self._units, self._missing, self._sizes) == (other._data, other._names, other._types, other._units, other._missing, other._sizes)
    
    @property
    def data(self):
        return self._data
    
    @data.setter
    def data(self, value):
        self._data = value
    
    @property
    def names(self):
        return self._names
    
    @names.setter
    def names(self, value):
        self._names = value
    
    @property
    def types(self):
        return self._types

    @types.setter
    def types(self, value):
        self._types = value

    @property
    def units(self):
        return self._units

    @units.setter
    def units(self, value):
        self._units = value

    @property
    def missing(self):
        return self._missing
    
    @missing.setter
    def missing(self, value):
        self._missing = value
    
    @property
    def sizes(self):
        return self._sizes
    
    @sizes.setter
    def sizes(self, value):
        self._sizes = value

    @property
    def missing_dict(self):
        return self._missing_dict
    
    @missing_dict.setter
    def missing_dict(self, value):
        self._missing_dict = value
    
def main():

    import numpy
    data = [numpy.arange(10)]
    names = ["obs_time", "rec_time", "latitude", "longitude", "elevation"]
    missing = [-9999., -9999., -9999., -9999., -9999.]
    types = ["f8", "f8", "f8", "f8", "f4"]
    units = ["seconds since 1970-1-1 00:00", "seconds since 1970-1-1 00:00", "degrees_north", "degrees_east", "m"]
    sizes = [10, 10, 10, 10, 10]
    field_object = Fields(data, names, types, units, missing, sizes)

    print "names: ", field_object.names
    print "sizes: ", field_object.sizes
    
if __name__ == "__main__":

   main()
