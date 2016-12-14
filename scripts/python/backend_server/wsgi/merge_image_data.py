#!/usr/bin/env python

"""Get latest image data"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 


import get_latest_files
import log_msg
import os
import sys
from optparse import OptionParser
import subprocess

def run_cmd(cmd_list, logg):
    logg.write_info(" ".join(cmd_list))
    ret = subprocess.call(cmd_list)
    if ret != 0:
        logg.write_error("failed executing: %s" % " ".join(cmd_list))
        logg.write_ending(ret)
    return ret

def merge_image_data(dir_dict, output_image_file, logg):
    """
    Merge image data in dir_dict.

    Parameters
    ----------
    base_dir : str base directory of dir_dict
    dir_dict : dictionary containing pairs of directories and associated files
    output_image_file : str output image file string

    Returns
    -------
    (err_code, file_list)
    
    err_code : int
        Non-zero value indicates error code, or zero on success.
    file_list : list of files merged
    """

    file_list = []
    for direct in dir_dict:
        file_name = dir_dict[direct]
        if file_name != "":
            file_list.append(os.path.join(direct, file_name))


    if len(file_list) > 0:
        command_list = ["montage"] + file_list + ["-mode"] + ["Concatenate"] + [output_image_file]
        ret = run_cmd(command_list, logg)
        return (ret, file_list)
    else:
        return (-1, file_list)

def main():

    usage_str = "%prog time_delta base_dir subdir[s] out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 4:
        parser.print_help()
        sys.exit(2)

    time_delta = int(args[0])
    base_dir = args[1]
    subdir_list = args[2:-1]
    out_file = args[-1]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    
    dir_dict = get_latest_files.get_subdir_files(base_dir, subdir_list)
    latest_dict = get_latest_files.get_latest_files(time_delta, dir_dict)
    merge_image_data(latest_dict, out_file, logg)
    logg.write_ending()

if __name__ == "__main__":

   main()
