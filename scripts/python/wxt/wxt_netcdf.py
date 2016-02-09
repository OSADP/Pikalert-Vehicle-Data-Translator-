#!/usr/bin/env python

"""Create output wxt cdf file"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: wxt_netcdf.py,v $                                           #
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
        self.nc_id.createDimension('num_obs', 0)
        for ind in range(len(self.field_object.names)):
            var_type = self.field_object.types[ind]
            if var_type == "a32":
                var_type = str
                variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',))
                data = self.field_object.data[ind].astype(object)
                variable[:] = data[:]
            else:
                if self.field_object.missing[ind] != None:
                    variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',), fill_value=self.field_object.missing[ind])
                else:
                    variable = self.nc_id.createVariable(self.field_object.names[ind], var_type, ('num_obs',))
                print ind, self.field_object.names[ind], self.field_object.data[ind][:]
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
    sizes = [10]
    field_object = fields.Fields(data, names, types, units, missing, sizes)
    WxtCdf("out.nc", field_object)
    
def test2():

    data = [numpy.arange(10) + 0.1]
    names = ["data"]
    types = ["f4"]
    units = ["degrees_north"]
    missing = [-9999.]
    sizes = [10]
    field_object = fields.Fields(data, names, types, units, missing, sizes)
    WxtCdf("out.nc", field_object)
    
def test3():

    data = [numpy.arange(10) + 0.1, numpy.arange(10) + 0.1]
    names = ["latitudes", "longitudes"]
    units = ["degrees_north", "degrees_east"]
    missing = [-9999., -9999.]
    types = ["f4", "f4"]
    sizes = [10, 10]
    field_object = fields.Fields(data, names, types, units, missing, sizes)
    WxtCdf("out.nc", field_object)
    
def test4():

    data = [numpy.arange(10) + 0.1, numpy.arange(10) + 0.1, numpy.arange(10) + 0.1, numpy.arange(10) + 0.1, numpy.arange(10) + 0.1]
    names = ["obs_time", "rec_time", "latitude", "longitude", "elevation"]
    missing = [-9999., -9999., -9999., -9999., -9999.]
    types = ["f8", "f8", "f8", "f8", "f4"]
    units = ["seconds since 1970-1-1 00:00", "seconds since 1970-1-1 00:00", "degrees_north", "degrees_east", "m"]
    sizes = [10, 10, 10, 10, 10]
    field_object = fields.Fields(data, names, types, units, missing, sizes)
    WxtCdf("out.nc", field_object)
    
def main():
    test4()

if __name__ == "__main__":

   main()
