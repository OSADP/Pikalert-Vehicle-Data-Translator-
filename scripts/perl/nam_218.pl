#!/usr/bin/perl
#
# Configuration file used to retreive NAM grid 218 data found on the
# NCEP FTP server.
#----------------------------------------------------------------------------
#
# Source of data on ftp server and name syntax
#
$ftpHost = "ftpprd.ncep.noaa.gov";
$ftpDir  = "/pub/data/nccf/com/nam/prod/nam.${today}";
$ftpFglob = "nam.t${refhour}z.awphys*.grb2.tm00";
#
# Local base name and file extension of raw model data
#
$model = "nam_218";
$ext = "grb2";
#
# Forecast times to get
#
@ftimes = ("00", "03", "06", "09", "12", "15", "18", "21", "24", "27", "30", "33", "36", "39", "42", "45", "48", "51", "54", "57", "60", "63", "66", "69", "72", "75", "78", "81", "84");
#
# Base name for decoded model data, cdl and site list files for grib2site
#
$dcdname = "nam";
$cdlfile =  "$ENV{RWFS_ROOT_DIR}/static_data/cdl/nam.cdl";
$sitelist = "$ENV{RWFS_ROOT_DIR}/static_data/site_list/core_sites.asc";
#
