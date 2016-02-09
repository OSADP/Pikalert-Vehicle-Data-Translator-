#!/usr/bin/env python

"""Get site time series from district alerts json file"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import log_msg
import os
import json
import sys
from optparse import OptionParser

def get_site_time_series(site_number, da_json_file):
    """
    Parameters
    ----------
    site_number : site number
    da_json_file : district alerts file name string

    Returns
    -------
    time series list or on failure, empty list

    """

    json_districts = {}
    time_series = []
    if da_json_file != "":
        try:
            json_district_data = open(da_json_file, 'r').read()
            json_districts = json.loads(json_district_data)
        except IOError:
            pass

        districts = json_districts["districts"]
        for district in districts:
            sites = district["sites"]
        
            # Extract time series for specified site
            for site in sites:
                if site["site_num"] == site_number:
                    time_series = site["time_series"]
                    break
                
    return time_series

def main():

    usage_str = "%prog site in_file"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    site_number = int(args[0])
    in_file = args[1]

    get_site_time_series(site_number, in_file)

if __name__ == "__main__":

   main()
