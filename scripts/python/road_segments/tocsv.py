#!/usr/bin/env python

"""Print netcdf record data in csv format."""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: tocsv.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014/08/20 18:52:58 $           #
#                                                                                #
# ============================================================================== #

import os
import sys
from optparse import OptionParser
import numpy
import time
from netCDF4 import Dataset
import curses.ascii

# remove nonprintable characters
Delchars = ''.join(c for c in map(chr, range(256)) if not curses.ascii.isprint(c))



def tocsv(header, time_ind, sort_option, field_list, field_types, in_files, out_file):

    fp_out = file(out_file, "w")

    if header:
        fields = []
        for ind in range(len(field_list)):
            if time_ind == ind:
                for item in ["year", "month", "day", "hour", "minute"]:
                    fields.append(item)
            else:                
                fields.append(field_list[ind])


        header_line = ",".join(fields)
        fp_out.write("%s\n" % header_line)
            
    for fname in in_files:
        print "processing %s" % fname
        out_line_list = []
        in_nc = Dataset(fname)
        arrays = []
        for field in field_list:
            arrays.append(in_nc.variables[field][:])

        length = len(arrays[0])
        for ind in range(length):
            line = []
            for off,array in enumerate(arrays):
                if off == time_ind:
                    line.append(time.strftime("%Y,%m,%d,%H,%M", time.gmtime(array[ind])))
                elif field_types[off] == 's':
                    char_string = array[ind].tostring()
                    char_string = char_string.translate(None, Delchars)                    
                    line.append(char_string)
                elif field_types[off] == 'u':
                    unicode_string = array[ind]
                    line.append(unicode_string)
                else:
                    out_string = str(array[ind])
                    pos1 = out_string.find('[')
                    pos2 = out_string.find(']')
                    if pos1 >= 0 and pos2 > pos1:
                        new_string = out_string[pos1+1:pos2].strip()
                        out_string_list = new_string.split()
                        out_string = "[%s]" % ",".join(out_string_list)
                        line.append(out_string)
                    else:
                        line.append(out_string)

            out_line = ",".join(line)
            out_line_list.append(out_line)

        if sort_option:
            out_line_list = sorted(out_line_list)

        for line in out_line_list:
            fp_out.write("%s\n" % line)

        in_nc.close()
    fp_out.close()

def main():

    usage_str = "%prog fields field_types in_file(s) out_file\nfields is a comma separated list of the names of the desired fields in the netcdf file and field_types should be a comma separated list such as s,u,d,f,... giving the type of each field. Use s for character strings and u for unicode strings.\nFor example:\n\ttocsv.py node_id,obs_time,wind_speed,power,sum_turbine_power s,f,f,f,f *nc output.txt\n\ttocsv.py stn_id,HUB_HGT_SPD s,f *nc output.txt\n\ttocsv.py -D *.nc output.txt\n\ttocsv.py id,unit_id,insertion_time,observation_time,latitude,longitude,elevation,air_temp,atmospheric_pressure,light_level,rain_intensity,relative_humidity,road_temp u,u,u,u,f,f,f,f,f,f,f,f,f in.nc out.txt\n\ttocsv.py -t 1 vehicle_id,obs_time,latitude,longitude,wiper_status,abs  s,f,f,f,d,d *nc output.txt"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-D", "--dicast_site", dest="dicast_site", action="store_true", help="print data in DICast all site files")
    parser.add_option("-n", "--no_header", dest="no_header", action="store_true", help="don't use a header in the output csv file")
    parser.add_option("-s", "--sort", dest="sort", action="store_true", help="sort output into alphabetical order")
    parser.add_option("-t", "--time", dest="time", help="provide index of time column")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if options.dicast_site:
        if len(args) < 2:
            parser.print_help()
            sys.exit(2)

        fields = "stn_id,HUB_HGT_SPD"
        field_types = "s,f"
        in_files = args[:-1]
        out_file = args[-1]
    elif len(args) < 4:
        parser.print_help()
        sys.exit(2)
    else:
        fields = args[0]
        field_types = args[1]
        in_files = args[2:-1]
        out_file = args[-1]

    header = True
    if options.no_header:
        header = False
        
    time_ind = -1
    if options.time:
        time_ind = int(options.time)

    field_list = fields.split(",")
    field_types = field_types.split(",")
    tocsv(header, time_ind, options.sort, field_list, field_types, in_files, out_file)
    
if __name__ == "__main__":

   main()
       

    
