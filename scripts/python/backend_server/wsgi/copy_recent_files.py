#!/usr/bin/env python

"""Copy latest files from source directory to destination directory"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).  #
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
import shutil
import time
import tim


def get_date(file_name):
    """
    Parameters
    ----------
    file_name : file name string

    Returns
    -------
    date string for file in the form yyyymmddhhmm
    """

    # Examples:
    # Vid-000351067-00-06-2014-10-29-13-20.jpg
    # Vid-000330038-00-02-2016-01-14-19-22.jpg
    date = None
    if len(file_name) > 21:
        date_string = file_name[-20:-4]
        date = "%s%s%s%s%s" % (date_string[0:4], date_string[5:7], date_string[8:10], date_string[11:13], date_string[14:16])

    return date

def get_site_id(file_name):
    """
    Parameters
    ----------
    file_name : file name string

    Returns
    -------
    site id string for file
    """

    # Vid-000351067-00-06-2014-10-29-13-20.jpg
    return file_name[4:19]
    
def organize_recent_files(file_list):
    """
    Parameters
    ----------
    file_list : list of files

    Returns
    -------
    dictionary whose keys are site identifiers and items are images files for those sites in time sort order
    """

    site_dict = {}
    for file_name in file_list:
        site_id = get_site_id(file_name)
        
    date_string = fname[20:36]
    date = "%s%s%s%s%s" % (date_string[0:4], date_string[5:7], date_string[8:10], date_string[11:13], date_string[14:16])
    

def get_recent_files(delta_seconds, file_list):
    """
    Parameters
    ----------
    delta_seconds : only use files within delta_seconds of current time
    file_list : list of files of interest

    Returns
    -------
    list of latest files

    """
    reduced_list = []
    current_time = time.time()
    for fname in file_list:
        date = get_date(fname)

        try:
            value = int(date)
        except ValueError:
            continue
        except TypeError:
            continue
        

        # Vid-000351067-00-06-2014-10-29-13-20.jpg
        file_time = tim.datetogmt(date)
        delta_time = abs(file_time - current_time)
        print "delta_time: ", delta_time, delta_seconds
        if delta_time < delta_seconds:
            reduced_list.append(fname)
            print "appending: ", fname

    return reduced_list

def transform_name(in_file):
    """
    Parameters
    ----------
    in_file : string
        Input file name

    Returns
    -------
    output file name string
    """
    # Vid-000351067-00-06-2014-10-29-13-20.jpg
    
def copy_recent_files(delta_seconds, in_dir, out_dir):
    """
    Parameters
    ----------
    delta_seconds : only use files within delta_seconds of current time
    in_dir : string
        Input directory
    out_dir : string
        Output directory

    Returns
    -------

    number of files copied
    """

    file_list = os.listdir(in_dir)
    reduced_list = get_recent_files(delta_seconds, file_list)
    dir_lists = {}
    for direct in os.listdir(out_dir):
        dir_lists[direct] = set(os.listdir(os.path.join(out_dir, direct)))

    for file_name in reduced_list:

        # Get site_id from file. Check if in dir_lists dictionary. If
        # not, make subdirectory for site_id
        site_id = get_site_id(file_name)

        site_id_file_list = dir_lists.get(site_id, None)
        if site_id_file_list == None:
            dir_lists[site_id] = set([])
            site_out_dir = os.path.join(out_dir, site_id)
            if not os.path.exists(site_out_dir):
                os.mkdir(site_out_dir)
            site_id_file_list = []

        # Check if file is already in destination. If not, copy to
        # destination.
        if not file_name in site_id_file_list:
            in_file = os.path.join(in_dir, file_name)
            dot_target_name = ".%s" % file_name
            dot_out_file = os.path.join(out_dir, site_id, dot_target_name)
            shutil.copyfile(in_file, dot_out_file)
            out_file = os.path.join(out_dir, site_id, file_name)
            os.rename(dot_out_file, out_file)

    #os.system("rsync --size-only %s %s" % (" ".join(reduced_list), out_dir))
    #sorted_file_list = sorted(file_list, cmp=lambda x,y: cmp(x[20:36], y[20:36]))
    return len(reduced_list)

def main():

    usage_str = "%prog delta_seconds in_dir out_dir\nFor example:\n\tcopy_recent_files.py 600 /ftp/restricted/alaska/root/upload_images /d2/vii/data/raw/alaska_images/latest"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    delta_seconds = int(args[0])
    in_dir = args[1]
    out_dir = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        

    logg.write_starting()
    file_count = copy_recent_files(delta_seconds, in_dir, out_dir)
    logg.write_info("copying %d files from %s to %s\n" % (file_count, in_dir, out_dir))
    logg.write_ending()

if __name__ == "__main__":

   main()
