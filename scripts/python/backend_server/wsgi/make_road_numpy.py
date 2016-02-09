#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import numpy
from netCDF4 import Dataset


def make_road_numpy(in_file, out_file):
    r"""A one-line summary that does not use variable names or the
function name.

    Parameters
    ----------
    var1 : array_like
    Array_like means all those objects -- lists, nested lists, etc. --
    that can be converted to an array. We can also refer to
    variables like `var1`.
    var2 : int
    The type above can either refer to an actual Python type
    (e.g. ``int``), or describe the type of the variable in more
    detail, e.g. ``(N,) ndarray`` or ``array_like``.
    Long_variable_name : {'hi', 'ho'}, optional
    Choices in brackets, default first when optional.
    
    Returns
    -------
    type
    Explanation of anonymous return value of type ``type``.
    describe : type
    Explanation of return value named `describe`.
    out : type
    Explanation of `out`.
    
    Examples
    --------
    These are written in doctest format, and should illustrate how to
    use the function.
    
    >>> a=[1,2,3]
    >>> print [x + 3 for x in a]
    [4, 5, 6]
    >>> print "a\n\nb"
    a
    b

    Notes
    -----
    Notes about the implementation algorithm (if needed).
    
    This can have multiple paragraphs.

    """

def make_numpy_files(road_segment_file, latitude_field_name, longitude_field_name, seg_id_field_name, output_numpy_seg_pts_file, output_numpy_seg_id_file):
    cdf = Dataset(road_segment_file, "r")
    latitude = cdf.variables[latitude_field_name]
    longitude = cdf.variables[longitude_field_name]
    seg_id = cdf.variables[seg_id_field_name]
    seg_pts = numpy.vstack((latitude, longitude)).T
    numpy.save(output_numpy_seg_pts_file, seg_pts)
    numpy.save(output_numpy_seg_id_file, seg_id)
    return 

def main():

    usage_str = "%prog output_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    output_dir = args[0]
    make_numpy_files("/d2/vii/data/static/cdl/co_roads.20150820.nc", "latitude", "longitude", "seg_id", os.path.join(output_dir, "co_numpy_seg_pts"), os.path.join(output_dir, "co_numpy_seg_id"))


if __name__ == "__main__":

   main()
