#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

""" Manifest for VDT TOPR4 

This file contains the list of files and directories to copy from cvs to create 
a tar distribution.

"""

# The following lists consist of (source directory, destination directory) pairs

applications = [
    ("projects/vdt4/apps/vdt4", "apps/vdt4"),
    ("projects/rwx/apps/rwa", "apps/rwa"),
    ("projects/rwx/apps/rwh", "apps/rwh"),
    ("projects/rwx/apps/probe_to_json", "apps/probe_to_json"),
    ("projects/rwx/apps/cdf_to_csv", "apps/cdf_to_csv"),
    ("apps/mdss/src/roadcond_mdss","apps/roadcond_mdss")]

build_install = [
    ("projects/vdt4/build_install", "build_install")]

libraries = [
    ("libs/log/src", "libs/log/src"),
    #("libs/stat_tools/src", "src/libs/stat_tools"),
    ("third_party/open/libs/metro/src", "libs/metro"),
    ("libs/Proj4Wrap/src", "libs/Proj4Wrap/src"),
    ("libs/ncf/src", "libs/ncf"),
    ("libs/ncfc/src", "libs/ncfc"),
    ("libs/nextcast/nxncf/src", "libs/nxncf"),
    ("libs/nextcast/site_list/src", "libs/site_list"),
    ("libs/gtime/src", "libs/gtime"),
    ("projects/vdt4/libs/vdt4", "libs/vdt4"),
    ("projects/rwx/libs/rwx", "libs/rwx")]

scripts = [
    ("projects/vdt4/scripts", "scripts"),
    #("projects/rwx/scripts", "src/")
    ]

static_data = [
    ("projects/vdt4/static_data/cdl", "static_data/cdl"),
    ("projects/vdt4/static_data/config", "static_data/config"),
    ("projects/rwx/static_data/cdl", "static_data"),
    ("projects/rwx/static_data/config", "static_data")]

# Libraries should be built before applications and tools
Build_list = libraries + applications

# Code list consists of all directories that should ultimately be copied into the code directory
Code_list = scripts + build_install

# Data list consists of all directories that should ultimately be copied into the data directory
Data_list = static_data

# Manifest list consists of everything
Manifest_list = Build_list + Code_list + Data_list
