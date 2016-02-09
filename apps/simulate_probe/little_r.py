#!/usr/bin/env python

"""Write out little R format"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).  #
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


def write_little_r_header(latitude, longitude, identifier, name, platform, source, elevation, num_valid_field, num_error, num_warning, sequence_num, num_duplicates, is_sounding, bogus, discard, utc_seconds, julian, date, sea_level_pressure, sea_level_pressure_qc, ref_pressure, ref_pressure_qc, ground_temp, ground_temp_qc, sea_surface_temp, sea_surface_temp_qc, surface_pressure, surface_pressure_qc, precip, precip_qc, max_temp, max_temp_qc, min_temp, min_temp_qc, night_min_temp, night_min_temp_qc, pressure_change_3h, pressure_change_3h_qc, pressure_change_24h, pressure_change_24h_qc, cloud_cover, cloud_cover_qc, ceiling, ceiling_qc, fp_out):
    """
    Write little_r header for observations to the file fp_out
    
    Little R header information

    Variable, Fortran I/O format, Description

    latitude, F20.5, station latitude (north positive)
    longitude, F20.5, station longitude (east positive)
    id, A40, ID of station
    name, A40, Name of station
    platform, A40, Description of the measurement device
    source, A40, GTS, NCAR/ADP, BOGUS, etc.
    elevation, F20.5, station elevation (m)
    num_vld_fld, I10, Number of valid fields in the report
    num_error, I10, Number of errors encountered during the decoding of this observation
    num_warning, I10, Number of warnings encountered during decoding of this observation.
    seq_num, I10, Sequence number of this observation
    num_dups, I10, Number of duplicates found for this observation
    is_sound, L10, T/F Multiple levels or a single level
    bogus, L10, T/F bogus report or normal one
    discard, L10, T/F Duplicate and discarded (or merged) report.
    sut, I10, Seconds since 0000 UTC 1 January 1970
    julian, I10, Day of the year
    date_char, A20, YYYYMMDDHHmmss
    slp, qc, F13.5, I7, Sea-level pressure (Pa) and a QC flag
    ref_pres, qc, F13.5, I7, Reference pressure level (for thickness) (Pa) and a QC flag
    ground_t, qc, F13.5, I7, Ground Temperature (T) and QC flag
    sst, qc, F13.5, I7, Sea-Surface Temperature (K) and QC
    psfc, qc, F13.5, I7, Surface pressure (Pa) and QC
    precip, qc, F13.5, I7, Precipitation Accumulation and QC
    t_max, qc, F13.5, I7, Daily maximum T (K) and QC
    t_min, qc, F13.5, I7, Daily minimum T (K) and QC
    t_min_night, qc, F13.5, I7, Overnight minimum T (K) and QC
    p_tend03, qc, F13.5, I7, 3-hour pressure change (Pa) and QC
    p_tend24, qc, F13.5, I7, 24-hour pressure change (Pa) and QC
    cloud_cvr, qc, F13.5, I7, Total cloud cover (oktas) and QC
    ceiling, qc, F13.5, I7, Height (m) of cloud base and QC
    """

    fp_out.write("%20.5f%20.5f%40s%40s%40s%40s%20.5f%10d%10d%10d%10d%10d%10s%10s%10s%10d%10d%20s%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d\n" % (latitude, longitude, identifier, name, platform, source, elevation, num_valid_field, num_error, num_warning, sequence_num, num_duplicates, is_sounding, bogus, discard, utc_seconds, julian, date, sea_level_pressure, sea_level_pressure_qc, ref_pressure, ref_pressure_qc, ground_temp, ground_temp_qc, sea_surface_temp, sea_surface_temp_qc, surface_pressure, surface_pressure_qc, precip, precip_qc, max_temp, max_temp_qc, min_temp, min_temp_qc, night_min_temp, night_min_temp_qc, pressure_change_3h, pressure_change_3h_qc, pressure_change_24h, pressure_change_24h_qc, cloud_cover, cloud_cover_qc, ceiling, ceiling_qc))
    return
        
def print_little_r_header():


    """
    "%20.5f%20.5f%40%40s%40s"
      WRITE ( UNIT = iunit , ERR = 19 , FMT = rpt_format ) 
     *        xlat,xlon, string1 , string2 , 
     *        string3 , string4 , ter, kx*6, 0,0,iseq_num,0, 
     *        .true.,bogus,.false., 
     *         -888888, -888888, date_char , 
     *         slp,0,-888888.,0, -888888.,0, -888888.,0, -888888.,0, 
     *               -888888.,0, 
     *               -888888.,0, -888888.,0, -888888.,0, -888888.,0, 
     *               -888888.,0, 
     *               -888888.,0, -888888.,0
     """







# latitude
# longitude
# station_id
# name
# platform
# source
# elevation
# num_valid_fields
# num_error
# num_warning, seq_num, num_dups, is_sound, bogus, discard, utc_seconds, julian_day, yyyymmddhhmmss, sea_level_pres, sea_level_pres_qc, ref_pres, ref_pres_qc, ground_temp, ground_temp_qc, sst, sst_qc, surface_pres, surface_pres_qc, precip_accum, precip_accum_qc, temp_max, temp_max_qc, 
def little_r():
    """

    SUBROUTINE write_obs ( p , z , t , td , spd , dir , 
     *                      slp , ter , xlat , xlon , mdate , kx , 
     * string1 , string2 , string3 , string4 , bogus , iseq_num ,
     * iunit )
):

    @type cf: config file import
    @param cf:
    @type in_file: string
    @param in_file: input file
    @type out_file: string
    @param out_file: output file
    @rtype: number
    @return: 0 on success, -1 on failure

    """

    return m       


def write_little_r_obs(pressure, pressure_qc, height, height_qc, temperature, temperature_qc, dew_point, dew_point_qc, wind_speed, wind_speed_qc, wind_direction, wind_direction_qc, u, u_qc, v, v_qc, rh, rh_qc, thickness, thickness_qc, fp_out):
    """
    Write obs in little_r format to the file fp_out
    
    Little R observation data record information:

    pressure, qc, %13.5f, %7d, pascals Pa
    height, qc, %13.5f, %7d, height (m MSL) 
    temperature, qc, %13.5f, %7d, temperature K
    dew_point, qc, %13.5f, %7d, dewpoint K
    wind speed, qc, %13.5f, %7d, wind speed m/s
    wind direction, qc, %13.5f, %7d, wind direction degrees
    u, qc, %13.5f, %7d, u wind m/s
    v, qc, %13.5f, %7d, v wind m/s
    rh, qc, %13.5f, %7d, relative humidity %
    thickness, qc, %13.5f, %7d, thickness m
    """

    fp_out.write("%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d%13.5f%7d\n" %  (pressure, pressure_qc, height, height_qc, temperature, temperature_qc, dew_point, dew_point_qc, wind_speed, wind_speed_qc, wind_direction, wind_direction_qc, u, u_qc, v, v_qc, rh, rh_qc, thickness, thickness_qc))
        
    """
      do 100 k = 1 , kx
         WRITE ( UNIT = iunit , ERR = 19 , FMT = meas_format ) 
     *          p(k), 0, z(k),0, t(k),0, td(k),0, 
     *          spd(k),0, dir(k),0, 
     *          -888888.,0, -888888.,0,-888888.,0, -888888.,0

      SUBROUTINE write_obs ( p , z , t , td , spd , dir , 
     *                      slp , ter , xlat , xlon , mdate , kx , 
     * string1 , string2 , string3 , string4 , bogus , iseq_num ,
     * iunit )

    """

def main():

    usage_str = "%prog in_file out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    in_file = args[1]
    out_file = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])

    logg.write_starting()
    little_r(cf, in_file, out_file)
    logg.write_ending()

if __name__ == "__main__":

   main()
