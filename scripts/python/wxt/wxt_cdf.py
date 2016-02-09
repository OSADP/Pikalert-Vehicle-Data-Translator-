#!/usr/bin/env python

"""Create output wxt cdf file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: wxt_cdf.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/09/29 18:35:29 $           #
#                                                                                #
# ============================================================================== #

import fields
import log_msg
import os
import sys
from optparse import OptionParser
import numpy
from netCDF4 import Dataset

    
class WxtCdf(object):

    def __init__(self, out_file, field_object):

        self.out_file = out_file
        self.field_object = field_object
        
        # create a new dataset.
        self.nc_id = Dataset(out_file, 'w')

        # establish field names, units, missing values
        self.nc_id.createDimension('num_obs', len(self.field_object.data[0]))
        for ind in range(len(self.field_object.names)):
            var_type = self.field_object.types[ind]
            if var_type == "a32":
                var_type = str
                variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',))
                data = self.field_object.data[ind].astype(object)
                variable[:] = data[:]
            else:
                if self.field_object.missing[ind] != None:
                    variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',), fill_value=self.field_object.missing[ind], zlib=True, shuffle=True, complevel=4)
                else:
                    variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',),  zlib=True, shuffle=True, complevel=4)
                variable[:] = self.field_object.data[ind][:]

            if self.field_object.units[ind] != None:
                variable.units = self.field_object.units[ind]
        self.nc_id.close()
        
def test1():

    data = [numpy.arange(10)]
    names = ["data"]
    missing = [-9999]
    types = ["i4"]
    units = [""]
    field_object = fields.Fields(data, names, types, units, missing)
    WxtCdf("out.nc", field_object)
    
def test2():

    data = [numpy.arange(10) + 0.1]
    names = ["data"]
    types = ["f4"]
    units = ["degrees_north"]
    missing = [-9999.]
    field_object = fields.Fields(data, names, types, units, missing)
    WxtCdf("out.nc", field_object)
    
def test3():

    data = [numpy.arange(10) + 0.1, numpy.arange(10) + 0.1]
    names = ["latitudes", "longitudes"]
    units = ["degrees_north", "degrees_east"]
    missing = [-9999., -9999.]
    types = ["f4", "f4"]
    field_object = fields.Fields(data, names, types, units, missing)
    WxtCdf("out.nc", field_object)
    
def main():
    test3()

if __name__ == "__main__":

   main()
