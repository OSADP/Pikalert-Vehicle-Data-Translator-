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

Nevada_image_table = {
    72488056 : ("Mount Rose Summit", "000314"),
    72488057 : ("Bullseye", "000316"),
    72488058 : ("I-580/S. Virginia", "000231"),
    72488070 : ("Kingsbury", "000325"),
    72488072 : ("Highland Flats", "000241"),
    72488076 : ("Arrow Head", "000250"),
    72488079 : ("SR-429 South NV US NV DOT", "000247"),
    72488089 : ("Incline", "000310"),
    72489028 : ("Black Springs", "000207"),
    72489030 : ("Reno Yard", "000222"),
    72581010 : ("Pilot", "001110"),
    72581011 : ("Pequop Summit", "001109"),
    72582024 : ("Moor/Wells", "001112"),
    72582025 : ("Halleck", "001108"),
    72582026 : ("ELKO SUMMIT NV US NV DOT", "001106"),
    72582027 : ("Elko West", "001100"),
    72582028 : ("Emigrant", "001104"),
    72583030 : ("Golconda Summit", "001107")
}

Nevada_dicast_site_array = []

for (key, value) in Nevada_image_table.items():
    Nevada_dicast_site_array.append(value[1])
    
Nevada_dicast_site_set = set(Nevada_dicast_site_array)


