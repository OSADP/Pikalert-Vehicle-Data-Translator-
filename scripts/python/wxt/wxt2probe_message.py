#!/usr/bin/env python

import numpy
import os
import sys
import time
import tim
from optparse import OptionParser
import log_msg
from netCDF4 import Dataset
from pytz import timezone
import datetime
import calendar

MPH_TO_MPS = 0.44704

def process(cdfout, cdl, logg, in_file, out_file):

    data = None

    if cdfout:
        logg.write_info("opening and reading %s" % in_file)
        data = read_wxt(in_file)
        logg.write_info("writing %s" % out_file)
        write_wxt_nc(cdl, data, out_file)
        logg.write_info("finished writing %s" % out_file)
    else:
        f = open(in_file,"r")

        data_dict = {}
        for l in f.readlines():
            ls = l.strip().split(",")
            #print ls
            add_wxt(ls, logg, data_dict)
        f.close()

        if len(data_dict.keys()) == 0:
            logg.write_info("no valid data found")
            logg.write_ending(0)
            return 0

        write_nc(data_dict, logg, out_file, cdl, ptime)
    
    return 0

def main():
    usage_str = "%prog cdl_file in_file out_file"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-D", "--directory", dest="directory", action="store_true", help="process all files in directory")
    parser.add_option("-d", "--date", dest="date", help="date")
    parser.add_option("-l", "--log", dest="log", help="log")
    parser.add_option("-n", "--netcdf", dest="netcdf", action="store_true", help="netcdf input")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    cdl = args[0]
    in_file = args[1]
    out_file = args[2]
    
    # ED need to change 300 to a parameter fed in on the command line
    ptime = time.mktime(time.gmtime()) - 300
    if options.date:
        ptime = time.mktime(time.strptime(options.date,"%Y%m%d.%H%M"))

    ptime = ptime - (ptime % 300)
    if options.log:
        logg = log_msg.LogMessage(options.log,"pyl")
    else:
        logg = log_msg.LogMessage("")

    logg.write_starting()
    logg.write_info("Executing: %s" % " ".join(sys.argv))
    logg.write_info("reading obs list from %s" % in_file)

    if not os.path.exists(in_file):
        logg.write_info("%s not found" % in_file)
        logg.write_ending(0)
        return

    final_ret = 0
    if options.directory:
        listdir = os.listdir(in_file)
        for fname in sorted(listdir):
            in_path = os.path.join(in_file, fname)
            out_path = os.path.join(out_file, fname)
            ret = process(options.netcdf, cdl, logg, in_path, out_path)
            if ret != 0:
                final_ret = ret
    else:
        final_ret = process(options.netcdf, cdl, logg, in_file, out_file)
    logg.write_ending(final_ret)
    return final_ret

def write_wxt_nc(cdl, data_dict, out_file):
    spds = []
    lats = []
    lons = []
    elev = []
    headings = []
    air_temps2 = []
    sur_temps = []
    nc_obstimes = []
    bar_press = []
    air_temps = []
    humiditys = []
    nc_vids = []
    source_ids = []
    ofpath = out_file
    cmd = "ncgen -o %s %s" % (ofpath, cdl)
    ret = os.system(cmd)
    if ret != 0:
        return -1
    nc = Dataset(ofpath,"a")

    nc.variables["latitude"][:] = data_dict["latitude"]
    nc.variables["longitude"][:] = data_dict["longitude"]
    nc.variables["elevation"][:] = data_dict["elevation"]
    nc.variables["air_temp2"][:] = data_dict["air_temp"]
    nc.variables["humidity"][:] = data_dict["relative_humidity"]
    nc.variables["surface_temp"][:] = data_dict["road_temp"]
    nc.variables["obs_time"][:] = data_dict["observation_time"]
    nc.variables["rec_time"][:] = data_dict["insertion_time"]
    nc.variables["bar_pressure"][:] = data_dict["atmospheric_pressure"]
    array_list = []
    for vehicle_id in data_dict["vehicle_id"]:
        id_list = list(['\0'] * 32)
        id_list[0:len(vehicle_id)] = vehicle_id[:]
        array_list.append(id_list)
    nc.variables["vehicle_id"][:] = numpy.array(array_list)
    
    #nc_vids.append(list(vid.ljust(32,'\0')))
    #source_ids.append(data_dict[obstime][vid]["source_id"])
    source_id_list = ['\0'] * 32
    source_id_list[0] = 'w'
    source_id_list[1] = 'x'
    source_id_list[2] = 't'
    nc.variables["source_id"][:] = numpy.array([source_id_list] * len(data_dict["latitude"]))
    nc.close()
    return 0

def write_nc(data_dict, logg, out_file, cdl, ptime):
    spds = []
    lats = []
    lons = []
    elev = []
    headings = []
    air_temps2 = []
    sur_temps = []
    nc_obstimes = []
    bar_press = []
    air_temps = []
    humiditys = []
    nc_vids = []
    source_ids = []
    ofpath = out_file
    cmd = "ncgen -o %s %s" % (ofpath, cdl)
    logg.write_info(cmd)
    os.system(cmd)
    logg.write_info("opening %s for writing" % ofpath)
    nc = Dataset(ofpath,"a")
    obstimes = data_dict.keys()
    obstimes.sort()
    for obstime in obstimes:
        if obstime < (ptime-30*60) or obstime > (ptime + 30*60):
            logg.write_info("%s out of time range" % (time.strftime("%Y%m%d %H:%M:%S", time.gmtime(obstime))))

            continue
        vids = data_dict[obstime].keys()
        vids.sort()
        for vid in vids:
            nc_obstimes.append(obstime)
            lats.append(data_dict[obstime][vid]["lat"])
            lons.append(data_dict[obstime][vid]["lon"])
            elev.append(data_dict[obstime][vid]["lon"])
            headings.append(data_dict[obstime][vid]["heading"])
            air_temps.append(data_dict[obstime][vid]["air_temp"])
            humiditys.append(data_dict[obstime][vid]["humidity"])
            air_temps2.append(data_dict[obstime][vid]["air_temp2"])
            sur_temps.append(data_dict[obstime][vid]["surface_temp"])
            spds.append(data_dict[obstime][vid]["speed"])
            bar_press.append(data_dict[obstime][vid]["bar_pressure"])
            nc_vids.append(list(vid.ljust(32,'\0')))
            source_ids.append(data_dict[obstime][vid]["source_id"])

    current_time_value = time.time()
    nc_rectimes = [current_time_value] * len(nc_obstimes)

    nc.variables["latitude"][:] = lats
    nc.variables["longitude"][:] = lons
    nc.variables["elevation"][:] = elev
    nc.variables["air_temp"][:] = air_temps
    nc.variables["humidity"][:] = humiditys
    nc.variables["air_temp2"][:] = air_temps2
    nc.variables["surface_temp"][:] = sur_temps
    nc.variables["obs_time"][:] = nc_obstimes
    nc.variables["rec_time"][:] = nc_rectimes
    nc.variables["speed"][:] = spds
    nc.variables["heading"][:] = headings
    nc.variables["bar_pressure"][:] = bar_press
    nc.variables["vehicle_id"][:] = nc_vids
    nc.variables["source_id"][:] = source_ids
    nc.close()

def read_wxt(fname):

    data = {}
    nc = Dataset(fname,"r")
    rec_num = nc.dimensions["num_obs"]
    data["vehicle_id"] = nc.variables["id"][:]
    data["latitude"] = nc.variables["latitude"][:]
    data["longitude"] = nc.variables["longitude"][:]
    data["elevation"] = nc.variables["elevation"][:]
    insertion_time = nc.variables["insertion_time"][:]
    insertion_time_list = []
    for insertion_time_str in insertion_time:
        insertion_time_list.append(get_obs_time(insertion_time_str))
    data["insertion_time"] = numpy.array(insertion_time_list)

    observation_time = nc.variables["observation_time"][:]
    obs_time_list = []
    for obs_time_str in observation_time:
        obs_time_list.append(get_obs_time(obs_time_str))
    data["observation_time"] = numpy.array(obs_time_list)
        
    data["air_temp"] = nc.variables["air_temp"][:]
    data["atmospheric_pressure"] = nc.variables["atmospheric_pressure"][:]
    data["light_level"] = nc.variables["light_level"][:]
    data["rain_intensity"] = nc.variables["rain_intensity"][:]
    data["relative_humidity"] = nc.variables["relative_humidity"][:]
    data["road_temp"] = nc.variables["road_temp"][:]
    
    nc.close()
    return data

def add_wxt(ls, logg, data_dict):
    vid = ls[1]
    obs_time_str = ls[2]
    obstime = get_obs_time(obs_time_str)
    if obstime == None:
        print "obstime failed"
        return

    lat = get_value(ls[3])
    lon = get_value(ls[4])
    elev = get_value(ls[5])
    air_temp = get_value(ls[6])
    light_level = get_value(ls[7])
    rain_intensity = get_value(ls[8])
    humidity = get_value(ls[9])
    surface_temp = get_value(ls[10])

    if not data_dict.has_key(obstime):
        data_dict[obstime] = {}
    if not data_dict[obstime].has_key(vid):
        data_dict[obstime][vid] = get_fill_val()
    data_dict[obstime][vid]["source_id"] = list("wxt".ljust(32, '\0'))
    data_dict[obstime][vid]["lat"] = lat
    data_dict[obstime][vid]["lon"] = lon
    data_dict[obstime][vid]["elev"] = elev
    if air_temp > -900:
        data_dict[obstime][vid]["air_temp"] = air_temp
    else:
        data_dict[obstime][vid]["air_temp"] = -9999.
    if surface_temp > -900:
        data_dict[obstime][vid]["surface_temp"] = surface_temp
    else:
        data_dict[obstime][vid]["surface_temp"] = -9999.
    if humidity > -900:
        data_dict[obstime][vid]["humidity"] = humidity
    else:
        data_dict[obstime][vid]["humidity"] = -9999.
    #print obstime, lat, lon, len(data_dict.keys())
    #print data_dict[obstime], "X", data_dict[obstime][vid] 

def get_obs_time(obs_time_str):
    year = obs_time_str[0:4]
    month = obs_time_str[5:7]
    mday = obs_time_str[8:10]
    hour = obs_time_str[11:13]
    minute = obs_time_str[14:16]
    second = obs_time_str[17:19]

    # Set up strings for user-supplied date and real-time
    obs_date = "%s%s%s" % (year, month, mday)
    obs_time = "%s%s%s" % (hour, minute, second)

    # Create a Unix time from date/time info
    dt = "%s.%s" % (obs_date, obs_time)
    ttup = time.strptime(dt,"%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)

    return (time_val)

def get_fill_val():
    return {"lat":-9999,
            "lon":-9999,
            "obstime":-9999,
            "air_temp":-9999,
            "air_temp2":-9999,
            "surface_temp":-9999,
            "speed":-9999,
            "heading":-9999,
            "bar_pressure":-9999}

def f_to_c(v):
    return (v - 32.0) * 5.0/9.0

def mph_to_mps(v):
    return v  * MPH_TO_MPS

def get_value(v,cnv = None):

    try:
        v = float(v)
    except:
        return -9999

    if cnv != None:
        return cnv(v)
    else:
        return v

if "__main__" == __name__:
    sys.exit(main())
