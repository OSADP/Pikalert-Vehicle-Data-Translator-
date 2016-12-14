#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2016 University Corporation for Atmospheric Research (UCAR).  #
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

class CameraSite(object):

    def __init__(self, site_id, lat, lon, elev, jpeg_url):

        self.site_id = site_id
        self.lat = lat
        self.lon = lon
        self.elev = elev
        self.jpeg_url = jpeg_url

    def __repr__(self):
        return "site_id: %s, lat: %f, lon: %f, elev: %f, jpeg_url: %s" % (self.site_id, self.lat, self.lon, self.elev, self.jpeg_url)
        
class NevadaSites(object):

    Nevada_dict = {"Reno" : "000", "Elko" : "001", "Las Vegas" : "002"} 

    @staticmethod
    def create_site_id(source, camera_id):
        if source in NevadaSites.Nevada_dict:
            site_id = NevadaSites.Nevada_dict[source] + camera_id
        return site_id
        
    @staticmethod
    def update_site_dict_nevada(site_id, lat, lon, elev, jpeg_url, site_dict):
        site_dict["site_id"].append(site_id)
        site_dict["lat"].append(lat)
        site_dict["lon"].append(lon)
        site_dict["elev"].append(elev)
        site_dict["jpeg_url"].append(jpeg_url)


    def __init__(self, in_file):
        """Gets site information from a nevada csv site file"""
    
        # Source,Camera_Name,Camera_ID,Windows_Media_URL,JPEG_URL,MPEG4_URL,Latitude,Longitude
        #self.site_dict = {"site_id" : [], "lat" : [], "lon" : [], "elev" : [], "jpeg_url" : []}
        self.camera_site_dict = {}
        self.error = ""
        first_line = True
        self.site_dict = {"site_id" : [], "lat" : [], "lon" : [], "elev" : [], "jpeg_url" : []}
        for line in open(in_file).readlines():
            if first_line:
                first_line = False
                continue

            spl = line.strip().split(",")
            (source, camera_name, camera_id, windows_media_url, jpeg_url, mpeg4_url, lat, lon) = spl
            elev = -9999.
            site_id = NevadaSites.create_site_id(source, camera_id)
            if site_id == "":
                self.error = "source %s not in Nevada_dict - fix Nevada_dict" % source
                return
            self.camera_site_dict[site_id] = CameraSite(site_id, float(lat), float(lon), float(elev), jpeg_url)

            # Source,Camera_Name,Camera_ID,Windows_Media_URL,JPEG_URL,MPEG4_URL,Latitude,Longitude
            NevadaSites.update_site_dict_nevada(site_id, float(lat), float(lon), float(elev), jpeg_url, self.site_dict)
        return

def main():

    usage_str = "%prog in_file"
    parser = OptionParser(usage = usage_str)

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    in_file = args[0]

    nevada_sites = NevadaSites(in_file)
    if nevada_sites.error:
        print "Error: problem reading nevada sites file: %s" % nevada_sites.error
    for key in nevada_sites.camera_site_dict.keys():
        print nevada_sites.camera_site_dict[key]
        


if __name__ == "__main__":

   main()
