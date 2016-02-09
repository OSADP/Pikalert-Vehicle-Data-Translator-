#!/usr/bin/env python

from netCDF4 import Dataset
from netCDF4 import default_fillvals
from collections import OrderedDict
import numpy as np
from numpy import ma
import sys

"""
Dumps a Pikalert netcdf segment file to csv.
"""

my_fill = -99999.0

#Set fields to output here
fields = OrderedDict([
          #('latitude',None),
          ('mid_point_latitude',None),
          #('longitude',None),
          ('mid_point_longitude',None),
          #('elevation',None),
          ('seg_name',None),
          ('seg_id',None),
          ('aux_id',None),
          #('point_type',None),
          #('rwfs_id',None),
          #('road_type',None),
          #('route_class',None),
          #('highway_type',None),
          #('speed_mph',None)
        ])


def extract_csv(ncfile, outfile, outfields=fields):
    """
    Extracts the outfields data in ncfile and write to csv in outfile.  
    This functions assumes that all fields in outfields use the point_num dimension.
    """
    nc = Dataset(ncfile, 'r', format='NETCDF4')
    for k in outfields:
        fields[k] = nc.variables[k][:]

    assert all([len(field) == len(nc.dimensions['point_num']) for field in outfields.values()])

    with file(outfile, 'w') as out:
        out.write("%s\n" % ",".join(outfields.keys()))
        for i in range(len(nc.dimensions['point_num'])):
            row = []
            for k in outfields:
                if outfields[k].dtype == '|S1':
                    s = ""
                    for c in outfields[k][i]:
                        if c is not ma.masked:
                            s += c
                    row.append(s)
                else:
                    val = outfields[k][i]
                    #See default_fillvals http://netcdf4-python.googlecode.com/svn/trunk/docs/netCDF4-module.html
                    if str(outfields[k].dtype)[:5] == "float" and val >= 9e+36 \
                       or str(outfields[k].dtype)[:5] == "int32" and val == -2147483647 \
                       or str(outfields[k].dtype)[:5] == "int16" and val ==  -32767:
                        val = my_fill
    
                    row.append(str(val))
                    
            out.write("%s\n" % ",".join(row))
        
    nc.close()

    
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print "USAGE: %s incdf outcsv" % sys.argv[0]
        sys.exit(1)
        
    extract_csv(sys.argv[1], sys.argv[2])

