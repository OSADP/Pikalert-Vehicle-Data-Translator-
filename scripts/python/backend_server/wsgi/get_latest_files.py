#!/usr/bin/env python

"""Get latest image files"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import copy_recent_files
import log_msg
import os
import sys
from optparse import OptionParser

def get_latest_files(time_delta, dir_dict):
    """
    Get latest file in each directory of dir_dict that is within time_delta seconds of the current time.
    
    Parameters
    ----------
    time_delta : int time delta seconds
    dir_dict : dictionary containing file listings of directories

    Returns
    -------
    dictionary
        Dictionary of latest files whose dates are within time_delta seconds of the current time
    """

    latest_dict = {}
    for direct in dir_dict:
        file_list = dir_dict[direct]
        recent_files = copy_recent_files.get_recent_files(time_delta, file_list)
        if recent_files != []:
            latest_dict[direct] = max(recent_files)
        else:
            latest_dict[direct] = ""
    return latest_dict
    
def get_subdir_files(base_dir, subdir_list):
    """
    Get files from each subdirectory in subdir_list
    
    Parameters
    ----------
    in_dir : str input directory
    subdir_list : list of subdirectories of interest

    Returns
    -------
    dictionary
        Dictionary
    """

    dir_lists = {}
    for direct in subdir_list:
        dir_lists[direct] = os.listdir(os.path.join(base_dir, direct))

    return dir_lists

def main():

    usage_str = "%prog time_delta in_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    time_delta = int(args[0])
    in_dir = args[1]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    get_latest_files(time_delta, in_dir)
    logg.write_ending()

if __name__ == "__main__":

   main()
