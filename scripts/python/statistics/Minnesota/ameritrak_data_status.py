#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# ** Copyright UCAR (c) 1992 - 2006
# ** University Corporation for Atmospheric Research(UCAR)
# ** National Center for Atmospheric Research(NCAR)
# ** Research Applications Program(RAP)
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# ** All rights reserved. Licenced use only.
# ** Do not copy or distribute without authorization.
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

import os, sys
import string
import time
import calendar
import log_msg
import datetime
import data_statistics
from pytz import timezone
from optparse import OptionParser

STATE = "Minnesota"
MIN_LAT = 43.4
MAX_LAT = 49.7
#MIN_LON = -97.7
#MAX_LON = -88.5
MIN_LON = -88.5
MAX_LON = -97.7

MPH_TO_MPS = 0.44704
MISSING = -9999

Dj3X_lat_lon_error_count = "Dj3X_lat_lon_error_count"
Dj3X_obstime_error_count = "Dj3X_obstime_error_count"

DjTempX_lat_lon_error_count = "DjTempX_lat_lon_error_count"
DjTempX_obstime_error_count = "DjTempX_obstime_error_count"

VaiX_lat_lon_error_count = "VaiX_lat_lon_error_count"
VaiX_obstime_error_count = "VaiX_obstime_error_count"

CanX_lat_lon_error_count = "CanX_lat_lon_error_count"
CanX_obstime_error_count = "CanX_obstime_error_count"

ObdY_lat_lon_error_count = "ObdY_lat_lon_error_count"
ObdY_obstime_error_count = "ObdY_obstime_error_count"


def get_value(v,cnv = None):
    try:
        v = float(v)
    except:
        return MISSING

    if cnv != None:
        return cnv(v)
    else:
        return v

def get_fill_val():
    return {"lat":MISSING,
            "lon":MISSING,
            "obstime":MISSING,
            "air_temp":MISSING,
            "air_temp2":MISSING,
            "surface_temp":MISSING,
            "speed":MISSING,
            "heading":MISSING,
            "bar_pressure":MISSING}

def f_to_c(v):
    """Convert Fahrenheit to Celsius"""
    return (v - 32.0) * 5.0/9.0

def mph_to_mps(v):
    """Convert miles per hour to meter per second"""
    return v  * MPH_TO_MPS

def kPa_to_hPa(v):
    """Convert kilo Pascals to hecto Pascals"""
    return v  * 10

def get_trunc_hour_time(obstime):
    """Truncate obstime to nearest hour"""
    return((int(obstime)/3600) * 3600)

def get_trunc_minute_time(obstime):
    """Truncate obstime to nearest minute"""
    return((int(obstime)/60) * 60)
    
def get_obs_time(obs_time_str, date, logg):
    """Return the observation time in unix time in UTC or return None"""
    
    date_format = "%y-%m-%d %H:%M:%S"
    date_format2 = "%Y-%m-%d %H:%M:%S"
    date_format3 = "%y-%m-%d-%H-%M-%S"
    date_format4 = "%Y-%m-%d %H:%M:"
    date_format5 = "%Y-%m-%d-%H-%M-"
    date_format6 = "%Y-%m-%d %H:%M:%S"
    date_format7 = "%Y-%m-%d-%H-%M-%S"
    obs_time_str = obs_time_str.strip().replace(".0","")
    
    try:
        otime = datetime.datetime.strptime(obs_time_str, date_format)
    except:
        try:
            otime = datetime.datetime.strptime(obs_time_str, date_format2)
        except:
            try:
                otime = datetime.datetime.strptime(obs_time_str, date_format3)
            except:
                try:
                    otime = datetime.datetime.strptime(obs_time_str, date_format4)
                except:
                    try:
                        otime = datetime.datetime.strptime(obs_time_str, date_format5)
                    except:
                        try:
                            otime = datetime.datetime.strptime(obs_time_str, date_format6)
                        except:
                            try:
                                otime = datetime.datetime.strptime(obs_time_str, date_format7)
                            except:
                                logg.write_info("couldn't parse %s using any of the date format strings" % (obs_time_str))
                                return None

    # Reformat input date
    rdate = date[0:4] + "-" + date[4:6] + "-" + date[6:8]
    
    central = timezone("US/Central")
    utc = timezone("UTC")
    otime = central.localize(otime)
    utc_otime = otime.astimezone(utc)

    if (rdate[0:10] != str(utc_otime)[0:10]):
        logg.write_info("The observation date (%s) isn't the same as the date being processed (%s).  This observation will be skipped." % (str(utc_otime)[0:10], rdate[0:10]))
        return -1
        
    obstime = time.mktime(utc_otime.timetuple())
    return (obstime - (obstime % 5))
    

def add_dj(line_split, logg, date, data_dict, error_dict):

    vid = int(get_value(line_split[2]))

    lat = line_split[4]
    lon = line_split[5]
    lat_lon_failure = False

    if (lat == "null") or (lon == "null"):
        if Dj3X_lat_lon_error_count in error_dict:
            error_dict[Dj3X_lat_lon_error_count] += 1
        else:
            error_dict[Dj3X_lat_lon_error_count] = 1
        lat_lon_failure = True
    else:
        lat = float(lat)
        lon = float (lon)
        if (lat == 0) or (lon == 0):
            if Dj3X_lat_lon_error_count in error_dict:
                error_dict[Dj3X_lat_lon_error_count] += 1
            else:
                error_dict[Dj3X_lat_lon_error_count] = 1
            lat_lon_failure = True
            
        
    obs_time_str = line_split[3]
    #print obs_time_str
    obstime = get_obs_time(obs_time_str, date, logg)
    #print obstime
    #print datetime.datetime.fromtimestamp(int(obstime)).strftime('%Y-%m-%d %H:%M:%S')
    obs_time_failure = False
    if obstime == None:
        if Dj3X_obstime_error_count in error_dict:
            error_dict[Dj3X_obstime_error_count] += 1
        else:
            error_dict[Dj3X_obstime_error_count] = 1
        obs_time_failure = True
    elif obstime == -1:
            return -2

    if lat_lon_failure or obs_time_failure:
        return -1
    
        
    heading = get_value(line_split[6])            
    surface_temp = get_value(line_split[13].replace("F",""), f_to_c)
    air_temp = get_value(line_split[14].replace("F",""), f_to_c)
    speed = get_value(line_split[8])
    if (speed != MISSING) or (speed != "null"):
        speed = speed * MPH_TO_MPS

    dict_struct = {}
    dict_struct["vid"] = vid
    dict_struct["obstime"] = obstime
    dict_struct["source_id"] = list("Ameritrak_Dj3X".ljust(32, '\0'))
    dict_struct["speed"] = speed
    dict_struct["lat"] = lat
    dict_struct["lon"] = lon
    dict_struct["heading"] = heading
    dict_struct["air_temp2"] = air_temp
    dict_struct["surface_temp"] = surface_temp

    trunc_hour_time = get_trunc_hour_time(obstime)
    trunc_minute_time = get_trunc_minute_time(obstime)
    if not trunc_hour_time in data_dict:
        data_dict[trunc_hour_time] = {}
    if not trunc_minute_time in data_dict[trunc_hour_time]:
        data_dict[trunc_hour_time][trunc_minute_time] = []
    data_dict[trunc_hour_time][trunc_minute_time].append(dict_struct)

    return 0
    
def add_djtemp(line_split, logg, date, data_dict, error_dict):

    vid = int(get_value(line_split[2]))

    lat = line_split[4]
    lon = line_split[5]
    lat_lon_failure = False
    if (lat == "null") or (lon == "null"):
        if DjTempX_lat_lon_error_count in error_dict:
            error_dict[DjTempX_lat_lon_error_count] += 1
        else:
            error_dict[DjTempX_lat_lon_error_count] = 1
        lat_lon_failure = True
    else:
        lat = float(lat)
        lon = float (lon)
        if (lat == 0) or (lon == 0):
            if DjTempX_lat_lon_error_count in error_dict:
                error_dict[DjTempX_lat_lon_error_count] += 1
            else:
                error_dict[DjTempX_lat_lon_error_count] = 1
            lat_lon_failure = True
            
    obs_time_str = line_split[3]
    obstime = get_obs_time(obs_time_str, date, logg)
    obs_time_failure = False
    if obstime == None:
        if DjTempX_obstime_error_count in error_dict:
            error_dict[DjTempX_obstime_error_count] += 1
        else:
            error_dict[DjTempX_obstime_error_count] = 1
        obs_time_failure = True
    elif obstime == -1:
            return -2

    if lat_lon_failure or obs_time_failure:
        return -1

    heading = get_value(line_split[6])
    gps_quality = line_split[7]
    if gps_quality == "0":
        return
    speed = get_value(line_split[8], mph_to_mps)
    surface_temp = get_value(line_split[10].replace("F",""), f_to_c)
    air_temp = get_value(line_split[11].replace("F",""), f_to_c)
    
    dict_struct = {}
    dict_struct["vid"] = vid
    dict_struct["obstime"] = obstime
    dict_struct["source_id"] = list("Ameritrak_DjTempX".ljust(32, '\0'))
    dict_struct["speed"] = speed
    dict_struct["lat"] = lat
    dict_struct["lon"] = lon
    dict_struct["heading"] = heading
    dict_struct["air_temp2"] = air_temp
    dict_struct["surface_temp"] = surface_temp

    trunc_hour_time = get_trunc_hour_time(obstime)
    trunc_minute_time = get_trunc_minute_time(obstime)
    if not trunc_hour_time in data_dict:
        data_dict[trunc_hour_time] = {}
    if not trunc_minute_time in data_dict[trunc_hour_time]:
        data_dict[trunc_hour_time][trunc_minute_time] = []
    data_dict[trunc_hour_time][trunc_minute_time].append(dict_struct)

    return 0

def add_vaisala(line_split, logg, date, data_dict, error_dict):

    vid = int(get_value(line_split[2]))
    
    lat = line_split[4]
    lon = line_split[5]
    lat_lon_failure = False

    if (lat == "null") or (lon == "null"):
        if VaiX_lat_lon_error_count in error_dict:
            error_dict[VaiX_lat_lon_error_count] += 1
        else:
            error_dict[VaiX_lat_lon_error_count] = 1
        lat_lon_failure = True
    else:
        lat = float(lat)
        lon = float (lon)
        if (lat == 0) or (lon == 0):
            if VaiX_lat_lon_error_count in error_dict:
                error_dict[VaiX_lat_lon_error_count] += 1
            else:
                error_dict[VaiX_lat_lon_error_count] = 1
            lat_lon_failure = True
            
    obs_time_str = line_split[3]
    obstime = get_obs_time(obs_time_str, date, logg)
    obs_time_failure = False
    if obstime == None:
        if VaiX_obstime_error_count in error_dict:
            error_dict[VaiX_obstime_error_count] += 1
        else:
            error_dict[VaiX_obstime_error_count] = 1
        obs_time_failure = True
    elif obstime == -1:
            return -2

    if lat_lon_failure or obs_time_failure:
        return -1

    heading = get_value(line_split[6])
    gps_quality = line_split[7]
    if gps_quality == "0":
        return
    speed = get_value(line_split[8], mph_to_mps)
    surface_temp = get_value(line_split[10].replace("F",""), f_to_c)
    air_temp = get_value(line_split[11].replace("F",""), f_to_c)
    dew_temp = get_value(line_split[12].replace("F",""), f_to_c)
    humidity = get_value(line_split[13].replace("F",""))

    dict_struct = {}
    dict_struct["vid"] = vid
    dict_struct["obstime"] = obstime
    dict_struct["source_id"] = list("Ameritrak_VaiX".ljust(32, '\0'))
    dict_struct["speed"] = speed
    dict_struct["lat"] = lat
    dict_struct["lon"] = lon
    dict_struct["heading"] = heading
    dict_struct["air_temp2"] = air_temp
    dict_struct["surface_temp"] = surface_temp
    dict_struct["dew_temp"] = dew_temp
    dict_struct["humidity"] = humidity

    trunc_hour_time = get_trunc_hour_time(obstime)
    trunc_minute_time = get_trunc_minute_time(obstime)
    if not trunc_hour_time in data_dict:
        data_dict[trunc_hour_time] = {}
    if not trunc_minute_time in data_dict[trunc_hour_time]:
        data_dict[trunc_hour_time][trunc_minute_time] = []
    data_dict[trunc_hour_time][trunc_minute_time].append(dict_struct)

    return 0

def add_can(line_split, logg, date, data_dict, error_dict):

    if (len(line_split) == 10):
        spn_list = line_split[9].split("|")
    elif (len(line_split) == 11):
        spn_list = line_split[10].split("|")

    vid = int(get_value(line_split[2]))

    lat = line_split[4]
    lon = line_split[5]
    lat_lon_failure = False
    if (lat == "null") or (lon == "null"):
        if CanX_lat_lon_error_count in error_dict:
            error_dict[CanX_lat_lon_error_count] += 1
        else:
            error_dict[CanX_lat_lon_error_count] = 1
        lat_lon_failure = True
    else:
        lat = float(lat)
        lon = float (lon)
        if (lat == 0) or (lon == 0):
            if CanX_lat_lon_error_count in error_dict:
                error_dict[CanX_lat_lon_error_count] += 1
            else:
                error_dict[CanX_lat_lon_error_count] = 1
            lat_lon_failure = True

    obs_time_str = line_split[3]
    obstime = get_obs_time(obs_time_str, date, logg)
    obs_time_failure = False
    if obstime == None:
        if CanX_obstime_error_count in error_dict:
            error_dict[CanX_obstime_error_count] += 1
        else:
            error_dict[CanX_obstime_error_count] = 1
        obs_time_failure = True
    elif obstime == -1:
            return -2

    if lat_lon_failure or obs_time_failure:
        return -1
    
    
    heading = get_value(line_split[6])
    speed = get_value(line_split[8]) * MPH_TO_MPS

    dict_struct = {}
    dict_struct["vid"] = vid
    dict_struct["obstime"] = obstime
    dict_struct["source_id"] = list("Ameritrak_CanX".ljust(32, '\0'))
    dict_struct["speed"] = speed
    dict_struct["lat"] = lat
    dict_struct["lon"] = lon
    dict_struct["heading"] = heading

    for spn in spn_list:
        spn_line_split = spn.split(" ")
        if len(spn_line_split) < 3:
            continue
        (num,val,unit) = spn_line_split
        num = float(num)
        val = float(val)
        if num == 108:
            dict_struct["bar_pressure"] = kPa_to_hPa(val)
        if num == 171:
            dict_struct["air_temp"] = val - 273.0

    trunc_hour_time = get_trunc_hour_time(obstime)
    trunc_minute_time = get_trunc_minute_time(obstime)
    if not trunc_hour_time in data_dict:
        data_dict[trunc_hour_time] = {}
    if not trunc_minute_time in data_dict[trunc_hour_time]:
        data_dict[trunc_hour_time][trunc_minute_time] = []
    data_dict[trunc_hour_time][trunc_minute_time].append(dict_struct)

    return 0

def add_obdy(line_split, logg, date, data_dict, error_dict):

    vid = int(get_value(line_split[2]))

    lat = line_split[4]
    lon = line_split[5]
    lat_lon_failure = False
    if (lat == "null") or (lon == "null"):
        if ObdY_lat_lon_error_count in error_dict:
            error_dict[ObdY_lat_lon_error_count] += 1
        else:
            error_dict[ObdY_lat_lon_error_count] = 1
        lat_lon_failure = True
    else:
        lat = float(lat)
        lon = float (lon)
        if (lat == 0) or (lon == 0):
            if ObdY_lat_lon_error_count in error_dict:
                error_dict[ObdY_lat_lon_error_count] += 1
            else:
                error_dict[ObdY_lat_lon_error_count] = 1
            lat_lon_failure = True

    obs_time_str = line_split[3]
    obstime = get_obs_time(obs_time_str, date, logg)
    obs_time_failure = False
    if obstime == None:
        if ObdY_obstime_error_count in error_dict:
            error_dict[ObdY_obstime_error_count] += 1
        else:
            error_dict[ObdY_obstime_error_count] = 1
        obs_time_failure = True
    elif obstime == -1:
            return -2

    if lat_lon_failure or obs_time_failure:
        return -1
    
    
    heading = get_value(line_split[6])
    speed = get_value(line_split[8]) * MPH_TO_MPS
    bar_press = get_value(line_split[17])
    bar_press = kPa_to_hPa(bar_press)
    air_temp2  = get_value(line_split[18])
    
    dict_struct = {}
    dict_struct["vid"] = vid
    dict_struct["obstime"] = obstime
    dict_struct["source_id"] = list("Ameritrak_ObdY".ljust(32, '\0'))
    dict_struct["speed"] = speed
    dict_struct["lat"] = lat
    dict_struct["lon"] = lon
    dict_struct["heading"] = heading
    dict_struct["bar_pressure"] = bar_press
    dict_struct["air_temp2"] = air_temp2

    trunc_hour_time = get_trunc_hour_time(obstime)
    trunc_minute_time = get_trunc_minute_time(obstime)
    if not trunc_hour_time in data_dict:
        data_dict[trunc_hour_time] = {}
    if not trunc_minute_time in data_dict[trunc_hour_time]:
        data_dict[trunc_hour_time][trunc_minute_time] = []
    data_dict[trunc_hour_time][trunc_minute_time].append(dict_struct)

    return 0

def get_data(in_dir, input_file_list, data_dict, error_dict, date, logg):
    #count = 0
    for input_file in input_file_list:

        input_file_path = in_dir + input_file
        if not os.path.exists(input_file_path):
            logg.write_info("%s not found" % input_file_path)
            logg.write_ending(0)
            sys.exit(0)

        logg.write_info("opening %s" % input_file_path)
        f = open(input_file_path,"r")
        for line in f.readlines():
            ret = 0
            
            if "GABE" in line:
                continue
            #print "line: ", line
            if line[-2] == ";":
                line = line[:-2]
                
            line_split = line.strip().split(",")
            
            if ">Dj3X:" in line_split[0]:
                if not "Dj3X"  in data_dict:
                    data_dict["Dj3X"] = {}
                ret = add_dj(line_split, logg, date, data_dict["Dj3X"], error_dict)

            elif ">DjTempX:" in line_split[0]:
                if not "DjTempX"  in data_dict:
                    data_dict["DjTempX"] = {}
                ret = add_djtemp(line_split, logg, date, data_dict["DjTempX"], error_dict)
                
            elif ">VaiX:" in line_split[0]:
                if not "VaiX"  in data_dict:
                    data_dict["VaiX"] = {}
                ret = add_vaisala(line_split, logg, date, data_dict["VaiX"], error_dict)
                
            elif ">CanX:" in line_split[0]:
                if not "CanX"  in data_dict:
                    data_dict["CanX"] = {}
                ret = add_can(line_split, logg, date, data_dict["CanX"], error_dict)

            elif ">ObdY:" in line_split[0]:
                if not "ObdY"  in data_dict:
                    data_dict["ObdY"] = {}
                ret = add_obdy(line_split, logg, date, data_dict["ObdY"], error_dict)
                
            if (ret == -1):
                logg.write_info("Unable to process the following line due to lat/lon or obstime bad values: %s" % (line))               

        f.close()
    #print count
    #print data_dict
    #print error_dict

    return 0

    
def get_statistics(data_dict, stat_dict, lat_lon_cell_dict):

    #print "1"
    data_statistics.create_stat_dict(data_dict, stat_dict)
    data_statistics.create_lat_lon_cell_dict(data_dict, MIN_LAT, MAX_LAT, MIN_LON, MAX_LON, lat_lon_cell_dict)

def write_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, state, logg):
    
    data_statistics.output_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, state, logg)


def main():
    
    usage_str = " %prog date in_dir out_dir \n\tNote: date should be in the format YYYYMMDD"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="log")

    (options, args) = parser.parse_args()

    if len(sys.argv) < 3:
        parser.print_help()
        sys.exit(2)

    date = args[0]
    in_dir = args[1]
    out_dir = args[2]

    dated_in_dir = in_dir + date + "/"
    
    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")


    input_file_list = []
    data_dict = {}
    error_dict = {}
    stat_dict = {}
    lat_lon_cell_dict = {}
    
    logg.write_starting("ameritrak_data_status.py")    

    input_file_list = os.listdir(dated_in_dir)
    
    get_data(dated_in_dir, input_file_list, data_dict, error_dict, date, logg)
    get_statistics(data_dict, stat_dict, lat_lon_cell_dict)
    write_statistics(data_dict, stat_dict, lat_lon_cell_dict, error_dict, date, out_dir, STATE, logg)

    logg.write_ending(0, "ameritrak_data_status.py")


if __name__ == "__main__":

   main()
             
