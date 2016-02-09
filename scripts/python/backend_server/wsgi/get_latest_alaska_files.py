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
import get_latest_files
import glob

alaska_site_dict = {
    70251005 : ("Parks Hwy @ Talkeetna Road MP 98.3", "000351009"),
    70259014 : ("Sterling Hwy @ Jean Lake Hill MP 62.2", "000351048"),
    70260006 : ("Parks Hwy @ Nenana Hills MP 325.4", "000351038"),
    70261019 : ("Steese Hwy @ Cleary Summit MP 20.9", "000351029"),
    70261020 : ("Richardson Hwy @ Badger Interchange MP 358", "000351027"),
    70261021 : ("University Avenue Fairbanks @ Chena River Bridge", "000351028"),
    70261130 : ("14th Avenue @ Eielson Street Fairbanks", "000351063"),
    70261131 : ("Airport Way @ Cowles Street Fairbanks", "000351065"),
    70261132 : ("Lathrop Street @ 21st Avenue Fairbanks", "000351066"),
    70261133 : ("Badger Road @ Old Richardson Hwy", "000351064"),
    70264007 : ("Parks Hwy @ Broad Pass Summit MP 201.4", "000351019"),
    70264008 : ("Parks Hwy @ Antler Creek MP 244", "000351039"),
    70265000 : ("FAIRBANKS / EIELSON AIR FORCE BASE", None),
    70265008 : ("District South-Side. Richardson Hwy @ Badger Rd (North Pole)", None),
    70265009 : ("Richardson Hwy @ Birch Lake MP 307.2", "000351022"),
    70265060 : ("Badger Road @ Elvira Avenue", "000351067"),
    70266008 : ("Richardson Hwy @ Trims DOT Camp MP 218.2", "000351024"),
    70266009 : ("Alaska Hwy @ Dot Lake MP 1360.4", "000351025"),
    70267007 : ("Richardson Hwy @ Tenderfoot MP 292.6", "000351036"),
    70268009 : ("Glenn Hwy @ Gunsight Mountain MP 117", "000351018"),
    70268011 : ("Glenn Hwy @ MP 176.6", "000351042"),
    70270024 : ("Seward Hwy @ Portage Glacier Road MP 78.9", "000351000"),
    70270025 : ("Seward Hwy @ Bird Point MP 96.3", "000351001"),
    70270026 : ("Glenn Hwy @ Eagle River Bridge MP 12.8", "000351003"),
    70270027 : ("Glenn Hwy @ S Curves MP10", "000351006"),
    70270028 : ("Whittier Access Road @ Bear Valley Tunnel MP 6.5", "000351033"),
    70270029 : ("Glenn Hwy @ Thunderbird Falls MP 24.5", "000351046"),
    70271017 : ("Richardson Hwy @ Edgerton MP 82.7", "000351021"),
    70272016 : ("Hillside Road @ Upper Huffman Road", "000351004"),
    70273010 : ("Seward Hwy @ Huffman Road MP 120", "000351002"),
    70273011 : ("Seward Hwy @ McHugh Creek MP 111.8", "000351005"),
    70274023 : ("Parks Hwy @ Hawk Lane MP 53.2", "000351008"),
    70274028 : ("Glenn Hwy @ 2nd Knik River Bridge MP 31.1", "000351007"),
    70275010 : ("Richardson Hwy @ Sheep Creek MP 18.6", "000351020"),
    70275013 : ("Richardson Hwy @ Thompson Pass MP 25.7", "000351026"),
    70277021 : ("Seward Hwy @ Summit Lake Lodge MP 45.8", "000351011"),
    70277022 : ("Sterling Hwy @ Russian River Ferry MP 54.8", "000351012"),
    70277023 : ("Sterling Hwy @ Tern Lake MP 37", "000351045"),
    70291012 : ("Alaska Hwy @ Canadian Border MP 1221.8", "000351037"),
    70291018 : ("Tok Cutoff @ Mentasta Pass MP 79.2", "000351023"),
    70341018 : ("Sterling Hwy @ Ninilchik River Bridge MP 135.2", "000351013"),
    70350017 : ("Pasagshak Road @ KodiakMP 2.1", "000351047"),
    70362016 : ("Haines Hwy @ Chilkat River Bridge MP 23.8", "000351017"),
    70362017 : ("Haines Hwy @ Klehini MP 36.6", "000351030"),
    70381025 : ("Egan Drive/Glacier Hwy @ MP 3", "000351014"),
    70381034 : ("Mendenhall Loop Road @ Goat Hill - bottom", "000351015"),
    70381035 : ("Glacier Hwy @ Cohen Drive MP 21", "000351016"),
    70381036 : ("North Douglas Hwy MP 4.5", "000351034")
    }


def get_latest_alaska_files(time_delta, dicast_site, base_dir):
    """
    Parameters
    ----------
    time_delta : float - get files within time_delta seconds of now
    dicast_site : str - dicast site id string
    base_dir : str - base directory string

    Returns
    -------
    dict
        Dictionary mapping subdirectory name to list of files in subdirectory
    """

    
    alaska_site_pair = alaska_site_dict.get(int(dicast_site), None)
    if alaska_site_pair == None:
        return ()
    alaska_site = alaska_site_pair[1]

    if alaska_site == None:
        return {}
    subdir_list = glob.glob("%s/%s*" % (base_dir, alaska_site))
    alaska_subdir_list = get_latest_files.get_subdir_files(base_dir, subdir_list)
    dir_dict = get_latest_files.get_subdir_files(base_dir, alaska_subdir_list)
    latest_dict = get_latest_files.get_latest_files(time_delta, dir_dict)
    return latest_dict

def main():

    # 
    usage_str = "%prog time_delta site base_dir\nFor example:\n\t%prog 1800 /var/autofs/mnt/sentinel_d2_vi"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
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
    print get_latest_alaska_files(time_delta, site, base_dir)
    logg.write_ending()

if __name__ == "__main__":

   main()
