#!/usr/bin/env python

"""Get latest alaska image files for a particular site"""

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
import sys
from optparse import OptionParser
import copy_recent_files
import get_latest_files
import time

# site dictionary consists of:
# key - DICast site identifier
# value - (description, MNDOT key)
site_dict = {
    71441003 : ("MN-7 Mile Post 11", "330023"),
    71748003 : ("Ely MN-1 Mile Post 275", "330018"),
    72644073 : ("Dexter I-90 Mile Post 195", "330009"),
    72644075 : ("Byron US-14 Mile Post 200", "330028"),
    72644078 : ("Rochester US-63 Mile Post 32", "330069"),
    72649073 : ("Lester Prairie MN-7 Mile Post 161", "330021"),
    72649074 : ("Winthrop MN-19 Mile Post 108", "330031"),
    72649075 : ("Madelia US-60 Mile Post 86", "330044"),
    72649076 : ("Mankato US-169 Mile Post 52", "330072"),
    72649077 : ("St. Peter US-169 Mile Post 69", "330073"),
    72649079 : ("New Prague MN-19 Mile Post 149", "330078"),
    72649080 : ("Maple Grove I-94 Mile Post 217", "330086"),
    72650076 : ("I-90 Mile Post 35", "330006"),
    72650077 : ("I-90 Mile Post 67", "330007"),
    72650078 : ("Blue Earth I-90 Mile Post 119", "330008"),
    72651054 : ("Beaver Creek I-90 Mile Post 4", "330005"),
    72655078 : ("Rush City I-35 Mile Post 157", "330003"),
    72655079 : ("Alexandria I-94 Mile Post 104", "330014"),
    72655080 : ("Sauk Center I-94 Mile Post 128", "330015"),
    72655082 : ("MN-23 Mile Post 215", "330033"),
    72655083 : ("Georgeville MN-55 Mile Post 98", "330042"),
    72655084 : ("Rice US-10 Mile Post 162", "330063"),
    72655085 : ("Milaca US-169 Mile Post 200", "330075"),
    72655086 : ("Hinckley I-35 Mile Post 181", "330076"),
    72658093 : ("Red Wing US-61 Mile Post 101", "330046"),
    72658119 : ("Little Chicago I-35 Mile Post 70", "330002"),
    72658120 : ("Cannon Falls US-52 Mile Post 90", "330041"),
    72658123 : ("Burnsville I-35W Mile Post 4", "330085"),
    72658125 : ("Cayuga Street Br. I-35E Mile Post 109", "330088"),
    72658126 : ("MN-110 and I-494 MN-10 Mile Post 5", "330089"),
    72745085 : ("Ball Bluff MN-65 Mile Post 145", "330050"),
    72745099 : ("Rutledge I-35 Mile Post 198", "330004"),
    72745101 : ("Cotton US-53 Mile Post 41", "330065"),
    72745102 : ("Thompson Hill I-35 Mile Post 248", "158000"),
    72745104 : ("I-35 and Garfield Avenue", "158001"),
    72745105 : ("MN-53 and 135 - Midway", "330083"),
    72745106 : ("Twig MN-53 Mile Post 21", "158003"),
    72747029 : ("MN-11 Mile Post 199", "330026"),
    72747030 : ("Effie MN-1 Mile Post 194", "330051"),
    72747031 : ("Ash Lake US-53 Mile Post 129", "330066"),
    72747032 : ("Margie US-71 Mile Post 379", "330071"),
    72750057 : ("Emily MN-6 Mile Post 29", "330019"),
    72750059 : ("Lastrup MN-25 Mile Post 140", "330034"),
    72750060 : ("Chamberlain MN-64 Mile Post 42", "330048"),
    72750061 : ("Pliny MN-65 Mile Post 98", "330049"),
    72750062 : ("Ottertail MN-78 Mile Post 34", "330054"),
    72750063 : ("Verndale US-10 Mile Post 94", "330062"),
    72753065 : ("Tenney US-75 Mile Post 188", "330053"),
    72753074 : ("Dilworth I-94 Mile Post 5", "330012"),
    72753075 : ("Rothsay I-94 Mile Post 38", "330013"),
    72753076 : ("Mahnomen US-59 Mile Post 296", "330043"),
    72753078 : ("Anthony MN-200 Mile Post 9", "330057"),
    72753079 : ("East Grand Forks US-2 Mile Post 8", "330059"),
    72753080 : ("Lake Park US-10 Mile Post 32", "330061"),
    72755037 : ("Shooks MN-1 Mile Post 34", "330017"),
    72755043 : ("Squaw Lake MN-46 Mile Post 25", "330040"),
    72755044 : ("N. of Upper Red Lake MN-72 Mile Post 40", "330052"),
    72755046 : ("Brooks US-59 Mile Post 331", "330067"),
    72756035 : ("Gatzke MN-89 Mile Post 96", "330055"),
    72756036 : ("Roosevelt MN-11 Mile Post 108", "330077"),
    74341087 : ("Hendricks MN-19 Mile Post 2", "330029"),
    74341088 : ("Morton MN-19 Mile Post 78", "330030"),
    74341089 : ("Hanley Falls MN-23 Mile Post 94", "330032"),
    74341090 : ("Graceville MN-28 Mile Post 19", "330035"),
    74341091 : ("Langhei Township MN-29 Mile Post 46", "330036"),
    74341093 : ("Mehurin US-212 Mile Post 1", "330058"),
    74341094 : ("MN-71 Mile Post 43", "330070")
    }


def contain_string(substring, file_list):

    out_list = []
    for fname in file_list:
        if fname.find(substring) > 0:
            out_list.append(fname)
    return out_list

def get_latest_minnesota_image_file(time_delta, dicast_site, base_dir):
    r"""Get latest minnesota image file for a site

    Parameters
    ----------
    time_delta : float - get files within time_delta seconds of now
    dicast_site : int - dicast site id string
    base_dir : str - base directory string

    Returns
    -------
    string : file name string or ""
    """
    
    site_pair = site_dict.get(dicast_site, None)
    if site_pair == None:
        return ("", "")
    site = site_pair[1]

    if site == None:
        return ("", "")

    # Get files for today's and yesterday's dates
    epoch_today = time.time()
    epoch_yesterday = epoch_today - 86400
    date_today = time.strftime("%Y%m%d", time.gmtime(epoch_today))
    date_yesterday = time.strftime("%Y%m%d", time.gmtime(epoch_yesterday))

    # Get file names for today and yesterday
    subdir_list = [date_today, date_yesterday]
    dir_dict = get_latest_files.get_subdir_files(base_dir, subdir_list)

    # Get files containing site for today and yesterday
    all_files = dir_dict[date_today] + dir_dict[date_yesterday]
    site_files = contain_string(site, all_files)
    
    # Reduce list to those that are sufficiently recent of nonzero size
    reduced_list = copy_recent_files.get_recent_files(time_delta, site_files)
    
    # Sort list in ascending date order and return last in list whose size is > 0
    sorted_list = sorted(reduced_list)
    if len(sorted_list) > 0:
        for ind in range(len(sorted_list), 1, -1):
            file_date = copy_recent_files.get_date(sorted_list[ind-1])
            file_path = os.path.join(base_dir, file_date[0:8], sorted_list[ind-1])
            if os.path.getsize(file_path) > 0:
                return (file_path, file_date)
    return ("", "")

def main():

    usage_str = "%prog time_delta site base_dir\nFor example:\n\t%prog 1800 72644078 /d2/vii/data/raw/minnesota_images\n\t%prog 1800 74341091 /d2/vii/data/raw/minnesota_images\n\t%prog 1800 74341090 /d2/vii/data/raw/minnesota_images"


    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    time_delta = int(args[0])
    site = args[1]
    base_dir = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    image_file = get_latest_minnesota_image_file(time_delta, int(site), base_dir)
    print "image_file: ", image_file
    logg.write_ending()

if __name__ == "__main__":

   main()
