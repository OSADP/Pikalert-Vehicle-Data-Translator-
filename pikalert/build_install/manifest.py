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
    ("projects/vdt4/apps/vdt4", "src/apps/vdt4"),
    ("projects/rwx/apps/rwa", "src/apps/rwa"),
    ("projects/rwx/apps/rwh", "src/apps/rwh"),
    ("projects/rwx/apps/probe_to_json", "src/apps/probe_to_json"),
    ("projects/rwx/apps/cdf_to_csv", "src/apps/cdf_to_csv"),
    ("third_party/open/libs/metro/src", "src/apps/metro"),
    ("apps/mdss/src/roadcond_mdss","src/apps/roadcond_mdss")]

build_install = [
    ("projects/vdt4/build_install", "build_install")]

libraries = [
    ("libs/log/src", "src/libs/log"),
    ("libs/stat_tools/src", "src/libs/stat_tools"),
    ("libs/Proj4Wrap/src", "src/libs/Proj4Wrap"),
    ("libs/ncf/src", "src/libs/ncf"),
    ("libs/ncfc/src", "src/libs/ncfc"),
    ("libs/nextcast/nxncf/src", "src/libs/nextcast/nxncf"),
    ("libs/nextcast/site_list/src", "src/libs/nextcast/site_list"),
    ("libs/gtime/src", "src/libs/gtime"),
    ("projects/vdt4/libs/vdt4", "src/libs/vdt4"),
    ("projects/rwx/libs/rwx", "src/libs/rwx")]

scripts = [
    ("projects/vdt4/scripts", "src/scripts"),
    ("projects/rwx/scripts", "src/")]

static_data = [
    ("projects/vdt4/static_data/cdl", "static/cdl"),
    ("projects/vdt4/static_data/config", "static/config"),
    ("projects/rwx/static_data/cdl", "static"),
    ("projects/rwx/static_data/config", "static")]

# Libraries should be built before applications and tools
Build_list = libraries + applications

# Code list consists of all directories that should ultimately be copied into the code directory
Code_list = scripts + build_install

# Data list consists of all directories that should ultimately be copied into the data directory
Data_list = static_data

# Manifest list consists of everything
Manifest_list = Build_list + Code_list + Data_list
