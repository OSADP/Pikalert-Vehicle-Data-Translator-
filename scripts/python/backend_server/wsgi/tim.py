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

import re
import time
import calendar
import string

SECS_PER_DAY = 86400

TM_YEAR = 0
TM_MON = 1
TM_DAY = 2
TM_HOUR = 3
TM_MIN = 4
TM_SEC = 5
TM_WDAY = 6
TM_JDAY = 7
TM_DST = 8

# Forecast period indicators
fpind_list = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M"]

def utc_time_list_delta(begin_date, end_date, delta_seconds):
    """Generate a list of daily date strings between begin_date and
    end_date inclusive incrementing by delta_seconds"""
    begin_utc_secs = datetogmt(begin_date)
    end_utc_secs = datetogmt(end_date)
    date = begin_utc_secs
    return_list = []
    while date <= end_utc_secs:
        return_list.append(date)
        date += delta_seconds
    return return_list


def gen_time_list(time_format, begin_utc_time, end_utc_time, time_increment):
    """
    Parameters
    ----------
    time_format : time format string used in strftime such as "%Y%m%d%H%M%S"
    begin_utc_time : utc time in seconds 
    end_utc_time : utc time in seconds 
    time_increment : time increment in seconds

    Returns
    -------
    list : list of file times in the form 20130907215000 yyyymmddhhmmss
    """
    
    time_list = []
    time_value = begin_utc_time
    while time_value <= end_utc_time:
        yyyymmddhhmmss = time.strftime(time_format, time.gmtime(time_value))
        time_list.append(yyyymmddhhmmss)
        time_value += time_increment
    return time_list

def date_list(begin_date, end_date):
    """Generate a list of daily date strings between begin_date and
    end_date inclusive"""
    begin_utc_secs = calendar.timegm(time.strptime(begin_date, "%Y%m%d"))
    end_utc_secs = calendar.timegm(time.strptime(end_date, "%Y%m%d"))
    date = begin_utc_secs
    return_list = []
    while date <= end_utc_secs:
        return_list.append(time.strftime("%Y%m%d", time.gmtime(date)))
        date += 86400
    return return_list

def hour_strings(init_time, valid_time):
    """Return date, init_hour and fcst_hour strings for given init_time and valid_time"""
    date_hour = time.strftime("%Y%m%d%H", time.gmtime(init_time))
    date = date_hour[0:8]
    init_hour = date_hour[8:10]
    fcst_hour = "%02d" % ((valid_time - init_time) / 3600)
    return (date, init_hour, fcst_hour)

def hour_min_strings(init_time, valid_time):
    """Return date, init_hour_min and fcst_hour_min strings for given init_time and valid_time"""
    date_hour_min = time.strftime("%Y%m%d%H%M", time.gmtime(init_time))
    date = date_hour_min[0:8]
    init_hour_min = date_hour_min[8:12]
    (fcst_hour, fcst_min, fcst_sec) = hms(valid_time - init_time)
    fcst_hour_min = "%02d%02d" % (fcst_hour, fcst_min)
    return (date, init_hour_min, fcst_hour_min)

def h2fpind(h):
    if h >= 0 and h < len(fpind_list):
        return fpind_list[h]
    return None

def fpind2h(fpind):
    for i in xrange(len(fpind_list)):
	if fpind == fpind_list[i]:
	    return i
    return None

def truncate_int(tval, time_int):
    """Truncate tval to nearest time interval."""
    return((int(tval)/time_int) * time_int)

def truncate_secs(tval, secs):
    """Truncate tval to nearest number of seconds. Generalizes the truncate code below. """
    return((int(tval)/secs) * secs)

def round_secs(tval, secs):
    """Round tval to nearest number of seconds. Generalizes the rounding code below. """
    return((round(tval)/secs) * secs)

def ceil_hour(tval):
    """ Find ceiling hour """
    return(math.ceil(tval/3600.0) * 3600)
    
def round_hour(tval):
    """Round tval to nearest hour."""
    return(round(tval/3600) * 3600)

def truncate_hour(tval):
    """Truncate tval to nearest hour."""
    return((int(tval)/3600) * 3600)

def truncate_day(tval):
    """Truncate tval to 0Z."""
    return((int(tval)/86400) * 86400)

def round_day(tval):
    """Round tval to nearest day."""
    return(round(tval/86400) * 86400)

def ceil_day(tval):
    """ Find ceiling day """
    return(math.ceil(tval/86400.0) * 86400.0)
    
def zhour(tval):
    """Find time difference between tval and 0Z."""
    return (tval - truncate_day(tval))

def get_time_window(date, assim_hour_str, forecast_hour_str, time_before_str, time_after_str):
    assim_hour = int(assim_hour_str)
    forecast_hour = int(forecast_hour_str)
    time_before = int(time_before_str)
    time_after = int(time_after_str)

    time_list = list(datetotuple(date))
    start_time = time_list[:]
    end_time = time_list[:]
    start_time[TM_HOUR] = assim_hour + forecast_hour
    end_time[TM_HOUR] = assim_hour + forecast_hour
    start_time[TM_MIN] = time_before
    end_time[TM_MIN] = time_after
    return(start_time, end_time)

def hms(ts):
    """Return hours, minutes, seconds"""
    tval = int(ts)
    hours = tval/3600
    leftover = tval - hours * 3600
    minutes = leftover/60
    seconds = leftover - 60 * minutes
    return (hours, minutes, seconds)
    
def ts2sec(ts):
    # Convert time string of form HHMMSS, HHMM, HH to seconds
    length = len(ts)
    if length == 2:
        return 3600 * int(ts)
    elif length == 4:
        return 3600 * int(ts[0:2]) + 60 * int(ts[2:4])
    elif length == 6:
        return 3600 * int(ts[0:2]) + 60 * int(ts[2:4]) + int(ts[4:6])
    else:
        return -1
    
def tp2sec(pattern, ts):
    """Time pattern to seconds. Pattern is of form HHHH..HMMSS otherwise return -1"""
    if len(pattern) != len(ts):
        return -1
    
    p = re.compile("H+")
    m = p.match(pattern)
    if m == None:
        return -1

    # Check for forecast period indicators (A-M)
    if pattern == "H":
	h = 3600 * fpind2h(ts)
	return h

    span = m.span()
    hour = 3600 * int(ts[span[0]:span[1]])

    if span[1] == len(pattern):
        return hour
    elif len(pattern) - span[1] >= 2:
        if pattern[span[1]:span[1]+2] == "MM":
            minute = 60 * int(ts[span[1]:span[1]+2])
            if span[1] + 2 == len(pattern):
                return hour + minute
            elif (span[1] + 4 == len(pattern)) and (pattern[span[1]:span[1]+4] == "MMSS"):
                minute = 60 * int(ts[span[1]:span[1]+2])
                return hour + minute + int(ts[span[1]+2:span[1]+4])
            
    return -1

def mkgmtime(tup):
    return calendar.timegm(tup)

def datetotuple(str):
    """Convert year month day string to time tuple. Works with YYYYMMDDHHMMSS, YYMMDDHHMM, YYMMDDHH, YYMMDD"""
    length = len(str)
    if length == 14:
	return (int(str[:4]), int(str[4:6]), int(str[6:8]), int(str[8:10]), int(str[10:12]), int(str[12:14]), 0, 0, 0)
    elif length == 8:
	return (int(str[:4]), int(str[4:6]), int(str[6:8]), 0, 0, 0, 0, 0, 0)
    elif length == 10:
	return (int(str[:4]), int(str[4:6]), int(str[6:8]), int(str[8:10]), 0, 0, 0, 0, 0)
    elif length == 12:
	return (int(str[:4]), int(str[4:6]), int(str[6:8]), int(str[8:10]), int(str[10:12]), 0, 0, 0, 0)
    
def datetogmt(str):
    """ Convert date string to gmt time. """
    date_tuple = datetotuple(str)
    return mkgmtime(date_tuple)

#----------------------------------------------------------
def date2sec(date):

    """
    This function converts the input date to seconds

    Inputs:
        date:     date in either format YYYYMMDD, YYYYMMDDHH
                  or YYYYMMDDHHMM to be converted to seconds
        
    Returns:
        date_sec: the date converted to seconds

    """
    if len(date) == 8:
        date_str = date + '000000'
    elif len(date) == 10:
        date_str = date + '0000'
    elif len(date) == 12:
        date_str = date + '00'

    date_tup = time.strptime("%s" % (date_str),"%Y%m%d%H%M%S")
    #date_sec = int(time.mktime(date_tup) )
    date_sec = calendar.timegm(date_tup)

    return(date_sec)

#----------------------------------------------------------
def sec2date(date):

    """
    This function converts the input date in seconds to YYYYMMDDHH

    Inputs:
        date:   date in sec
        
    Returns:
        date_str:       date  YYYYMMDD
        hr_str:         hour  HH
    """
    
    date_str_long = time.gmtime(date)
    date_str = "%s%s%s" %(date_str_long[0],string.zfill(date_str_long[1],2),string.zfill(date_str_long[2],2)) 
    hr_str = "%s" %(string.zfill(date_str_long[3],2))
    
    return(date_str, hr_str)

#----------------------------------------------------------   
if __name__ == '__main__':
    #l = time_list(0, SECS_PER_DAY, 10)
    # print l
    print "hms(3*3600 + 7 * 60 + 9): ", hms(3*3600 + 7 * 60 + 9)
    print "tp2sec: ", tp2sec("HHHMMSS", "1000102")

    seconds = time.time()
    print "seconds: ", seconds
    gm_tuple = time.gmtime(seconds)
    print "gmt:        ", gm_tuple
    local_tuple = time.localtime(time.time())
    print "local time: ", local_tuple
    print "seconds from mkgmtime: ", mkgmtime(gm_tuple)
    print "seconds from mktime:   ", time.mktime(local_tuple)
    
    print datetotuple("19981015013245")
    a=list(datetotuple("19981015013245"))
    a[TM_HOUR] = a[TM_HOUR] - 3
    print a
    print tuple(a)
    print datetotuple("1998101501")
    print datetotuple("19981015")
    print datetogmt("1998101501")
    print datetogmt("19981015")
    trunc_day = truncate_day(seconds)
    print "gmt trunc day: ", time.gmtime(trunc_day)

    print "Forecast period A = hour", fpind2h("A")
    print "Forecast hour 0 = period", h2fpind(0)
    print "Forecast period M = hour", fpind2h("M")
    print "Forecast hour 12 = period", h2fpind(12)
