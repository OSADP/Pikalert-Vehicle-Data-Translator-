#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import datetime
import time
import os
import numpy
from numpy import ma
from netCDF4 import Dataset
import StringIO
import base64
import string
import sites

font_file = "/usr/share/fonts/truetype/freefont/FreeSerif.ttf"

MISSING_VALUE = -9999

(MISSING, CLEAR, WARN, ALERT) = range(4)
(ALERTS, CODES, GROUPS) = range(3)

WARN_LETTER = "w"
ALERT_LETTER = "a"
CLEAR_LETTER = "c"
MISSING_LETTER = "m"

WARN_COLOR = (255,255,0,255)
ALERT_COLOR = (255,0,0,255)
CLEAR_COLOR = (0,128,0,255)
MISSING_COLOR = (190,190,190,255)

WARN_CODE = "warning"
ALERT_CODE = "alert"
CLEAR_CODE = "clear"
MISSING_CODE = "missing"

def max_code(code1, code2):
    """Find the maximum of the two alert codes"""
    if code1 == ALERT_CODE or code2 == ALERT_CODE:
        return ALERT_CODE
    elif code1 == WARN_CODE or code2 == WARN_CODE:
        return WARN_CODE
    elif code1 == CLEAR_CODE or code2 == CLEAR_CODE:
        return CLEAR_CODE
    else:
        return MISSING_CODE
    
alert_to_letter = {
    WARN : WARN_LETTER,
    ALERT : ALERT_LETTER,
    CLEAR : CLEAR_LETTER,
    MISSING : MISSING_LETTER
    }

alert_to_code = {
    CLEAR : CLEAR_CODE,
    WARN : WARN_CODE,
    ALERT : ALERT_CODE,
    MISSING : MISSING_CODE
    }

alert_to_color = {
    CLEAR : CLEAR_COLOR,
    WARN : WARN_COLOR,
    ALERT : ALERT_COLOR,
    MISSING : MISSING_COLOR
    }

def get_fpath(ptime, in_dir, basename, suffix):
    """Get forecast system file paths"""
    dir_dict = {}
    for m in range(60):
        etime = ptime + 60 - 60 * m
        time_tup = time.gmtime(etime)
        date = time.strftime("%Y%m%d%H%M", time_tup)
        fname = "%s.%s.%s.%s" % (basename, date[0:8], date[8:12], suffix)
        day_dir = os.path.join(in_dir, date[0:8])

        if os.path.exists(day_dir):
            if not day_dir in dir_dict:
                dir_dict[day_dir] = set(os.listdir(day_dir))
            if fname in dir_dict[day_dir]:
                fpath = os.path.join(in_dir, date[0:8], fname)
                return (fpath, etime)
    return (None, None)

class Alerts:

    def __init__(self, fpath, cf, ptime):
        self.nc = None
        self.nc_sites = []
        self.num_days = 0
        self.num_fc_times = 0
        
        if fpath != None:
            self.nc = Dataset(fpath, "r")
            self.nc_sites = self.nc.variables["site_list"][:]
            self.num_days = len(self.nc.dimensions["days"])
            #self.forecast_time = self.nc.variables["forc_time"][:]
            #self.alert_time = self.forecast_time
            self.num_fc_times = len(self.nc.dimensions["fc_times_per_day"])
        else:
            self.error = "problem loading netcdf file: %s" % fpath

        self.obs_sites = sites.get_wx_obs_sites(cf)
        self.rwis_sites = sites.get_rwis_sites(cf)
        self.obs_alerts = ObsAlerts(cf, ptime)
        self.rwis_alerts = RwisAlerts(cf, ptime)

    def get_alert(self, i, d, h):
        return MISSING

    def get_time_alerts(self, start_hour):
        alerts = {}
        alert_codes = {}
        alert_groups = {}

        for i in range(len(self.nc_sites)):
            site_num = self.nc_sites[i]
            alerts[site_num] = []
            groups = [[],[],[]]
            for d in range(self.num_days):
                for h in range(self.num_fc_times):
                    hr = d * 24 + h
                    if hr < start_hour or hr >= 72 + start_hour:
                        continue
                    
                    alert = self.get_alert(i, d, h)
                    alerts[site_num].append(alert)
                    if hr >= start_hour and hr < start_hour + 6:
                        groups[0].append(alert)
                    if hr >= start_hour + 6 and hr < start_hour + 24:
                        groups[1].append(alert)
                    if hr >= start_hour + 24 and hr < start_hour + 72:
                        groups[2].append(alert)

            max_alert = map(lambda x: max(x), groups)
            (a6, a24, a72) = max_alert
            ob = MISSING
            if self.obs_sites.has_key(site_num):
                obs = self.obs_alerts.get_obs(site_num)
                ob = self.obs_alerts.get_obs_alert(obs)
            if self.rwis_sites.has_key(site_num):
                # Note that rwis takes precedence over observation if
                # it exists
                obs = self.rwis_alerts.get_obs(site_num)
                ob = self.rwis_alerts.get_rwis_alert(obs)

            alert_group0  = alert_to_letter[ob]
            alert_group6 = alert_to_letter[a6]
            alert_group24 = alert_to_letter[a24]
            alert_group72 = alert_to_letter[a72]
            alert_groups[site_num] = [alert_group0, alert_group6, alert_group24, alert_group72]
            alert_codes[site_num] = [alert_to_code[ob], alert_to_code[a6], alert_to_code[a24], alert_to_code[a72]]

        return (alerts, alert_codes, alert_groups)

class RdwxAlerts(Alerts):

    def __init__(self, cf, ptime):
        self.cf = cf
        (self.file_name, self.file_path_time) = get_fpath(ptime, cf.rdwx_dir, "rdwx_fcst", "nc")
        Alerts.__init__(self, self.file_name, cf, ptime)
        #self.alert_time = self.forecast_time
        self.alert_time = ptime - ptime % 3600
        self.precip_types = None
        if self.nc != None:
            self.precip_types = self.nc.variables["precip_type"][:]

    def get_alert(self, i, d, h):
        if self.precip_types == None:
            return MISSING
        precip_type = self.precip_types[i][d][h]
        if precip_type == 1:
            return WARN
        if precip_type == 2 or precip_type == 5:
            return ALERT
        return CLEAR

    def get_alert_time(self):
        return self.alert_time
    
    def get_file_name(self):
        return self.file_name
    
class TmtAlerts(Alerts):

    def __init__(self, cf, ptime):
        self.cf = cf
        (self.file_name, self.file_path_time) = get_fpath(ptime, cf.tmt_dir, cf.tmt_base_name, "nc")
        Alerts.__init__(self, self.file_name, cf, ptime)
        self.alert_time = ptime - ptime % 3600
        self.chems = None
        self.plows = None
        self.road_temps = None

        if self.nc != None:
            self.chems = self.nc.variables["apply_chem"][:]
            self.plows = self.nc.variables["do_plowing"][:]
            self.road_temps = self.nc.variables["road_TempF"][:]
    
    def get_alert(self, i, d, h):
        if self.chems != None and self.plows != None and self.road_temps != None:
            chem = self.chems[i][d][h]
            plow = self.plows[i][d][h]
            road_temp = self.road_temps[i][d][h]
            if road_temp <= 15 and (chem > 0 or plow > 0):
                return ALERT
            if road_temp <= 32 and (chem > 0 or plow > 0):
                return WARN
            # if road_temp <= 15  and fabs(air_temp - dewpoint_temp) < 3:
            # return ALERT
            # if 15 < road_temp and road_temp <= 32 and fabs(air_temp - dewpoint_temp) < 3:
            # return WARN
            # For treatments chemical is indicated only if > 0, and should be Chemical,
            # For treatments plow is indicated only if > 0, and should be Plow,
            # Then include road temperature
            # OPTIONAL: Up arrow for road temperature + 6 hours is greater than now
            # Change from missing to treatments not configured for this site
            return CLEAR

        else:
            return MISSING

    def get_treatments(self, start_hour):
        plow_dict = {}
        chem_dict = {}
        road_temps_dict = {}
        
        for i in range(len(self.nc_sites)):
            site_num = self.nc_sites[i]
            plow_dict[site_num] = []
            chem_dict[site_num] = []
            road_temps_dict[site_num] = []
            for d in range(self.num_days):
                for h in range(self.num_fc_times):
                    hr = d * 24 + h
                    if hr < start_hour or hr >= 72 + start_hour:
                        continue
                    
                    chem = self.chems[i][d][h]
                    chem_dict[site_num].append(chem)
                    plow = self.plows[i][d][h]
                    plow_dict[site_num].append(plow)
                    road_temp = self.road_temps[i][d][h]
                    if road_temp > 200:
                        road_temp = -9999
                    road_temps_dict[site_num].append(road_temp)

        return (plow_dict, chem_dict, road_temps_dict)

    def get_alert_time(self):
        return self.alert_time

    def get_file_name(self):
        return self.file_name
    
class ObsAlerts(Alerts):

    def __init__(self, cf, ptime):
        self.cf = cf
        self.nc_var_names = self.get_nc_var_names()
        self.nc_files = []
        self.nc_data = {}
        self.ptime = ptime
        self.alert_time = 0

        btime = ptime
        etime = btime - 3600
        fnames = []
        while btime > etime:
            time_tup = time.gmtime(btime)
            day = time.strftime("%Y%m%d", time_tup)
            hhmm = time.strftime("%H%M", time_tup)
            fname = self.get_fname(day, hhmm)
            fname = os.path.join(self.get_in_dir(), day, fname)
            if not fname in fnames and os.path.exists(fname):
                if self.alert_time == 0:
                    self.alert_time = etime
                fnames.append(fname)
            btime -= 60
        for fname in fnames:
            nc = Dataset(fname, "r")
            dimension = nc.dimensions.get("recNum", None)
            if dimension != None:
                if len(dimension) == 0:
                    continue
            self.nc_files.append(nc)
            self.nc_data[nc] = {}
            for k in self.nc_var_names:
                if k == "stationId":
                    self.nc_data[nc][k] = numpy.ma.getdata(nc.variables[k][:])
                    self.nc_data[nc][k] = map(lambda x: string.join(x,""), self.nc_data[nc][k])
                else:
                    self.nc_data[nc][k] = numpy.ma.filled(nc.variables[k][:], MISSING_VALUE)

        if self.alert_time == 0:
            self.alert_time = ptime

    def get_alert_time(self):
        return self.alert_time
    
    def get_fname(self, day, hhmm):
        return "int_obs.%s.nc" % day

    def get_in_dir(self):
        return self.cf.wx_obs_dir

    def get_nc_var_names(self):
        return [
            "site_list",
            "time_nominal",
            self.cf.met_vars.wx_temp_var,
            self.cf.met_vars.wx_dewp_var,
            self.cf.met_vars.wx_wind_spd_var
            ]

    def get_obs(self, site):
        for nc in self.nc_files:
            nc_data = self.nc_data[nc]
            site_i = numpy.where(nc_data["site_list"] == site)
            obs_times = nc_data["time_nominal"]
            if len(site_i[0]) == 0:
                continue
            time_i = min(range(len(obs_times)), key=lambda i: abs(obs_times[i] - self.ptime))
            temp = nc_data[self.cf.met_vars.wx_temp_var][site_i][0][time_i]
            dewp = nc_data[self.cf.met_vars.wx_dewp_var][site_i][0][time_i]
            wind_spd = nc_data[self.cf.met_vars.wx_wind_spd_var][site_i][0][time_i]
            obs_time = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(obs_times[time_i]))
            temp_units = nc.variables[self.cf.met_vars.wx_temp_var].units
            dewp_units = nc.variables[self.cf.met_vars.wx_dewp_var].units
            wind_spd_units = nc.variables[self.cf.met_vars.wx_wind_spd_var].units

            # convert the temp and wind units to English units
            if temp_units=='degrees Celsius' and temp > MISSING_VALUE:
               temp=(9.0/5.0)*(temp)+32
               temp_units='deg F'
            if dewp_units=='degrees Celsius' and dewp > MISSING_VALUE:
               dewp=(9.0/5.0)*(dewp)+32          
               dewp_units='deg F'
            if wind_spd_units=='meters per second' and wind_spd > MISSING_VALUE: 
               wind_spd=2.23694*wind_spd
               wind_spd_units='mph'

            return {
                "temp_val":temp,
                "dewp_val":dewp,
                "wind_spd":wind_spd,
                "temp": "missing" if MISSING_VALUE == temp else "%.2f %s" % (temp, temp_units),
                "dewp": "missing" if MISSING_VALUE == temp else "%.2f %s" % (dewp, dewp_units),
                "wind_spd": "missing" if MISSING_VALUE == wind_spd else "%.2f %s" % (wind_spd, wind_spd_units),
                "obstime": obs_time
                }
        return {}

    def get_obs_alert(self, obs):
        if obs == None:
            return MISSING
        if not obs.has_key("temp_val"):
            return MISSING
        temp = obs["temp_val"]
        if temp == MISSING_VALUE:
            return MISSING
        if temp <= 0:
            return WARN
        return CLEAR

class RwisAlerts(ObsAlerts):

    def __init__(self, cf, ptime):
        """Initialize rwis alerts"""
        ObsAlerts.__init__(self, cf, ptime)
        self.cf = cf
        self.site_idxs = {}
        for nc in self.nc_files:
            stn_ids = self.nc_data[nc]["stationId"]
            for i in range(len(stn_ids)):
                stn_id = stn_ids[i]
                if not self.site_idxs.has_key(nc):
                    self.site_idxs[nc] = {}
                if not self.site_idxs[nc].has_key(stn_id):
                    self.site_idxs[nc][stn_id] = []
                self.site_idxs[nc][stn_id].append(i)

    def get_fname(self, day, hhmm):
        """Get mesonet file name"""
        return "mesonet.%s.%s00.nc" % (day, hhmm[:2])

    def get_in_dir(self):
        """Get input mesonet directory"""
        return self.cf.mesonet_dir

    def get_nc_var_names(self):
        """Get the netcdf variable names from ascii sites variable file"""
        f = open(self.cf.rwis_sites_var_file, "r")
        self.site_vars = {}
        all_vars = set()
        for l in f.readlines():
            (site_num, site_id, sub_surface_2, sub_surface_1, road_temp_2, road_temp_1, wind_dir, wind_spd, rel_hum, temp) = l.strip().split(';')
            site_num = int(site_num)
            self.site_vars[site_num] = (site_num, site_id, sub_surface_2, sub_surface_1, road_temp_2, road_temp_1, wind_dir, wind_spd, rel_hum, temp)
            all_vars.add(sub_surface_1)
            all_vars.add(sub_surface_2)
            all_vars.add(road_temp_2)
            all_vars.add(road_temp_1)
            all_vars.add(wind_dir)
            all_vars.add(wind_spd)
            all_vars.add(rel_hum)
            all_vars.add(temp)
        f.close()
        all_vars = list(all_vars)
        all_vars.append("observationTime")
        all_vars.append("stationId")
        return all_vars

    def get_obs(self, site):
        """Get observations from input forecast system netcdf file"""
        if not site in self.site_vars:
            return {}
        
        (site_num,site_id,sub_surface_2_var_name,sub_surface_1_var_name,road_temp_2_var_name,road_temp_1_var_name,wind_dir_var_name,wind_spd_var_name,rel_hum_var_name,temp_var_name) = self.site_vars[site]
        for nc in self.nc_files:
            nc_data = self.nc_data[nc]
            obs_times = nc_data["observationTime"]
            if not self.site_idxs[nc].has_key(site_id):
                continue
            stn_i = self.site_idxs[nc][site_id]
            stn_i = (numpy.array(stn_i),)
            if len(stn_i[0]) == 0:
                continue
            sub_surface_2_var = nc.variables[sub_surface_2_var_name]
            sub_surface_1_var = nc.variables[sub_surface_1_var_name]
            road_temp_2_var = nc.variables[road_temp_2_var_name]
            road_temp_1_var = nc.variables[road_temp_1_var_name]
            wind_dir_var = nc.variables[wind_dir_var_name]
            wind_spd_var = nc.variables[wind_spd_var_name]
            rel_hum_var = nc.variables[rel_hum_var_name]
            temp_var = nc.variables[temp_var_name]
            obs_times = obs_times[stn_i]
            sub_surface_2_data = nc_data[sub_surface_2_var_name][stn_i]
            sub_surface_1_data = nc_data[sub_surface_1_var_name][stn_i]
            road_temp_2_data = nc_data[road_temp_2_var_name][stn_i]
            road_temp_1_data = nc_data[road_temp_1_var_name][stn_i]
            wind_dir_data = nc_data[wind_dir_var_name][stn_i]
            wind_spd_data = nc_data[wind_spd_var_name][stn_i]
            rel_hum_data = nc_data[rel_hum_var_name][stn_i]
            temp_data = nc_data[temp_var_name][stn_i]
            time_i = min(range(len(obs_times)), key=lambda i: abs(obs_times[i] - self.ptime))
            local_obs_datetime = datetime.datetime.fromtimestamp(obs_times[time_i], tz=self.cf.timezone) # only plot 3 days
            local_obs_time_string = local_obs_datetime.strftime("%Y-%m-%d %H:%M:%S")
            obs_time = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(obs_times[time_i]))
            sub_surface_2 = sub_surface_2_data[time_i]
            sub_surface_1 = sub_surface_1_data[time_i]
            road_temp_2 = road_temp_2_data[time_i]
            road_temp_1 = road_temp_1_data[time_i]
            wind_dir = wind_dir_data[time_i]
            wind_spd = wind_spd_data[time_i]
            rel_hum = rel_hum_data[time_i]
            temp = temp_data[time_i]

            # convert to English units
            temp_units=temp_var.units
            road_temp_1_units=road_temp_1_var.units 
            road_temp_2_units=road_temp_2_var.units
            sub_surface_1_units=sub_surface_1_var.units
            sub_surface_2_units=sub_surface_2_var.units
            wind_spd_units=wind_spd_var.units
            if temp_var.units=='kelvin' and temp>(-9000):
               temp=(9.0/5.0)*(temp-273.15)+32
               temp_units='deg F'
            if road_temp_1_var.units=='kelvin' and road_temp_1 > MISSING_VALUE:
               road_temp_1=(9.0/5.0)*(road_temp_1-273.15)+32
               road_temp_1_units='deg F'
            if road_temp_2_var.units=='kelvin' and road_temp_2 > MISSING_VALUE:
               road_temp_2=(9.0/5.0)*(road_temp_2-273.15)+32
               road_temp_2_units='deg F'
            if sub_surface_1_var.units=='kelvin' and sub_surface_1 > MISSING_VALUE:
               sub_surface_1=(9.0/5.0)*(sub_surface_1-273.15)+32
               sub_surface_1_units='deg F'
            if sub_surface_2_var.units=='kelvin' and sub_surface_2 > MISSING_VALUE:
               sub_surface_2=(9.0/5.0)*(sub_surface_2-273.15)+32
               sub_surface_2_units='deg F'
            if wind_spd_var.units=='meter/sec' and wind_spd > MISSING_VALUE:
               wind_spd=2.23694*wind_spd
               wind_spd_units='mph'

            return {
                "road_temp_val":road_temp_1,
                "temp_val":temp,
                "sub_surface_2":"missing" if MISSING_VALUE == sub_surface_2 else "%.2f %s" % (sub_surface_2, sub_surface_2_units),
                "sub_surface_1":"missing" if MISSING_VALUE == sub_surface_1 else "%.2f %s" % (sub_surface_1, sub_surface_1_units),
                "road_temp_2":"missing" if MISSING_VALUE == road_temp_2 else "%.2f %s" % (road_temp_2, road_temp_2_units),
                "road_temp_1":"missing" if MISSING_VALUE == road_temp_1 else "%.2f %s" % (road_temp_1, road_temp_1_units),
                "wind_dir":"missing" if MISSING_VALUE == wind_dir else "%.2f %s" % (wind_dir, wind_dir_var.units),
                "wind_spd":"missing" if MISSING_VALUE == wind_spd else "%.2f %s" % (wind_spd, wind_spd_units),
                "rel_hum":"missing" if MISSING_VALUE == rel_hum else "%.2f %s" % (rel_hum, rel_hum_var.units),
                "temp":"missing" if MISSING_VALUE == temp else "%.2f %s" % (temp, temp_units),
                "obstime":local_obs_time_string
                }
        return {}

    def get_rwis_alert(self, obs):
        """Derive rwis station alerts from temperature and/or road temperature"""
        if obs == {}:
            return MISSING
        temp = MISSING_VALUE
        road_temp = MISSING_VALUE
        if obs.has_key("temp_val"):
            temp = obs["temp_val"]
        if obs.has_key("road_temp_val"):
            road_temp = obs["road_temp_val"]
        if temp != MISSING_VALUE:
            temp = temp * (9.0/5.0) - 459.67
        if road_temp != MISSING_VALUE:
            road_temp = road_temp * (9.0/5.0) - 459.67
        if road_temp != MISSING_VALUE and road_temp <= 32:
            return ALERT
        if temp != MISSING_VALUE and temp <= 32:
            return WARN
        return MISSING

if "__main__" == __name__:
    ptime = 1354160700
    time_tup = time.gmtime(ptime)
    print time.strftime("%Y%m%d.%H%M", time_tup)
    import backend_sys_path
    cf = backend_sys_path.State_dictionary["minnesota"]
    r = RwisAlerts(cf, ptime)
    print get_fpath(time.time() - 7200, cf.tmt_dir, cf.tmt_base_name, "nc")
