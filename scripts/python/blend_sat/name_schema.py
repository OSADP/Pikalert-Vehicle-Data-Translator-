#!/usr/bin/env python

#/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# * Copyright (c) 1995-2007 UCAR
# * University Corporation for Atmospheric Research(UCAR)
# * National Center for Atmospheric Research(NCAR)
# * Research Applications Program(RAP)
# * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# * All rights reserved. Licenced use only.
# * $Date: 2007-03-23 16:28:27 $
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


import re
import string
import time
import tim

#
# time patterns
#
# Year-month-day - YYYYMMDD
#
# Acceptable Issue time patterns:
#
# HHMMSS 
# HHMM
# HH
#
# Example Forecast time patterns:
#
# HHH..HMMSS
# HHMMSS
# HHH..HMM
# HH..H
# H  (forecast period indicator)
#

# Format tokens
# %B - base name
# %D - year-month-day
# %I - issue time
# %F - forecast times
# %S - suffix
#
# All tokens must have the same token length and use the same starting
# delimiter. This delimiter cannot be used in the format string except
# to delimit tokens
Token_len = 2
Delimiter = "%"

# Token list
Format = ["%B", "%D", "%I", "%F", "%S"]

# Token and pattern indices
[BASE_IND, DATE_IND, ISSUE_IND, FCST_IND, SUFFIX_IND, NUM_IND] = range(6)

class Token(object):
    """ Store information about a token """

    def __init__(self, length, token_string):
        """Initialize Token_info
        Inputs:
            length -- length of pattern string for token
            token_string -- the token string
        """
        self.ind = Format.index(token_string) # token index
        self.length = length            # token pattern string length
        return
    
    def pr(self):
        print "\tind: ", self.ind
        print "\tlen: ", self.length
        return
    
    def get_ind(self):
        return self.ind

    def get_length(self):
        return self.length


class TokenPos(object):
    """Store token plus location and position information"""

    def __init__(self, token, foff, off, ct):
        """Initialize Token_info
        Inputs:
            foff -- offset of token in format string
            off -- offset of token in standard file string
            ct -- 1 + number of tokens previous to this token in format string
        """
        self.token = token
        self.foff = foff                # token string offset in format string
        self.off = off                  # string offset position of token in standard string
        self.ct = ct                  # token offset position in standard string
        return
    
    def pr(self):
        print "token: ", self.token.pr()
        print "foff: ", self.foff
        print "off: ", self.off
        return
    
    def get_length(self):
        return self.token.get_length()

    def get_off(self):
        return self.off

    def get_ct(self):
        return self.ct

    def get_ind(self):
        return self.token.get_ind()

    def get_foff(self):
        return self.foff

    
def valid_issue_pattern(pat):
    return pat in ["", "HH", "HHMM", "HHMMSS"]


def valid_forecast_pattern(pat):

    if pat != "":
        p = re.compile("H+")
        m = p.match(pat)
        if m == None:
            return 0
        span = m.span()
        if len(pat) - span[1] >= 2:
            if pat[span[1]:span[1]+2] != "MM":
                return 0
            if (span[1] + 4 == len(pat)) and (pat[span[1]:span[1]+4] != "MMSS"):
                return 0
    return 1

def hms_forecast_pattern(pat):

    if pat != "":
        p = re.compile("H+")
        m = p.match(pat)
        if m == None:
            return None
        span = m.span()
        if len(pat) - span[1] == 4:
            if pat[span[1]:span[1]+4] != "MMSS":
                return None
            else:
                return (span[1] - span[0], 2, 2)
        elif len(pat) - span[1] == 2:
            if pat[span[1]:span[1]+2] != "MM":
                return None
            else:
                return (span[1] - span[0], 2, 0)
        elif len(pat) - span[1] == 0:
            return (span[1] - span[0], 0, 0)
        return None

    return None

class Name_schema:
    
    def __init__(self, format_string, patterns):
        # All format tokens must be used in format_string
        # even if they are not used. If they are not used
        # make sure that the corresponding pattern is the
        # null string, ""
        # The tokens in format_string can be in any order. The strings in
        # the patterns list must be in the order specified in Pattern indices above.
        #
        # Here's an example format_string: "%B.%D.i%I.f%F.%S"
        # Here's an example patterns list: ["pirep", "YYYYMMDD", "", "", "nc"]
        # Note that both the issue time pattern and the forecast pattern
        # are null in this case and thus won't be used.
        # See ns_examples() or run_tests() below for additional examples.
        
        self.fill = 0

        token_len = 0
        for k in patterns:
            token_len += len(k)
        self.name_len = len(format_string) - 2 * len(Format) + token_len
        self.format_string = format_string
        self.format = string.replace(format_string, "%B", "%s")
        self.format = string.replace(self.format, "%D", "%s")
        self.format = string.replace(self.format, "%I", "%s")
        self.format = string.replace(self.format, "%F", "%s")
        self.format = string.replace(self.format, "%S", "%s")
        self.patterns = patterns
        
        # store format offsets of tokens
        store = [0] * NUM_IND
        foff = [0] * NUM_IND
        start = 0
        for i in xrange(NUM_IND):
            format_pos = string.find(format_string, Delimiter, start)
            start = format_pos + 1
            store[i] = Token(len(patterns[i]), format_string[format_pos:format_pos+Token_len])

            # Check issue and forecast patterns for validity
            # Establish number of characters for fcst_hms
            pat = patterns[i]
            if store[i].get_ind() == ISSUE_IND:
                if not valid_issue_pattern(pat):
                    raise ValueError, "Issue time pattern %s must be in [\"\", \"H\", \"HH\", \"HHMM\", \"HHMMSS\"]" % pat
            elif store[i].get_ind() == FCST_IND:
                self.fcst_hms = hms_forecast_pattern(pat)
                if pat == "":
                    self.fcst_hms_format = ""
                else:
                    if self.fcst_hms == None:
                        raise ValueError, "Forecast time pattern %s must be in [\"\", \"H\", \"HH...H\", \"HH...HMM\", \"HH...HMMSS\"]" % pat
                    self.fcst_hms_format = "%%0%dd%%02d%%02d" % self.fcst_hms[0]
                    
            foff[i] = format_pos
            

        # determine actual offsets
        sum_pattern_len = 0
        token_pos = [0] * NUM_IND
        for i in xrange(NUM_IND):
            offset = foff[i] - i * 2 + sum_pattern_len
            token_pos[i] = TokenPos(store[i], foff[i], offset, i)            
            sum_pattern_len += store[i].get_length()
            
        # record token index order
        self.info = [0] * NUM_IND
        for i in xrange(NUM_IND):
            self.info[token_pos[i].get_ind()] = token_pos[i]
        return
    
    def set_fill(self):
        """ Fill empty fields with 0's according to input patterns """
        self.fill = 1
        return

    def pr(self):
        print "format_string: ", self.format_string
        print "format: ", self.format
        print "patterns: ", self.patterns
        for i in xrange(NUM_IND):
            self.info[i].pr()
            
    def get_base(self):
        return self.patterns[BASE_IND]

    def get_suffix(self):
        return self.patterns[SUFFIX_IND]

    def get_date(self, fname):
        """Return date string for given file fname"""
        begin = self.info[DATE_IND].get_off()
        end = begin + self.info[DATE_IND].get_length()

        if -self.name_len+end == 0:
            return fname[-self.name_len + begin:]
        else:
            return fname[-self.name_len + begin:-self.name_len + end]

    def get_date_pattern(self):
        """Return date pattern string"""
        return self.patterns[self.info[DATE_IND].get_ct()]

    def get_ft(self, fname):
        """Return forecast time string for given file fname"""
        begin = self.info[FCST_IND].get_off()
        end = begin + self.info[FCST_IND].get_length()
        if -self.name_len+end == 0:
            return fname[-self.name_len + begin:]
        else:
            return fname[-self.name_len + begin:-self.name_len + end]

    def get_ft_pattern(self):
        """Return ft pattern string"""
        return self.patterns[self.info[FCST_IND].get_ct()]

    def get_it(self, fname):
        """Return issue time string for given file fname"""
        begin = self.info[ISSUE_IND].get_off()
        end = begin + self.info[ISSUE_IND].get_length()
        if -self.name_len+end == 0:
            return fname[-self.name_len + begin:]
        else:
            return fname[-self.name_len + begin:-self.name_len + end]

    def get_it_pattern(self):
        """Return it pattern string"""
        return self.patterns[self.info[ISSUE_IND].get_ct()]

    def get_dtime(self, fname):
        """Return data time object for fname"""
        return Data_time(self.get_date(fname), self.get_it(fname), self.get_ft(fname))

    def get_utime(self, fname):
        """get issue and forecast utc times"""
        date = self.get_date(fname)
        it = self.get_it(fname)
        if it != "":
            date_tuple = tim.datetotuple("%s" % date)
            date_utime = tim.mkgmtime(date_tuple) # The unix time of the date in the file's name
            it_utime = date_utime + tim.tp2sec(self.get_it_pattern(), it)
        else:
            it_utime = None
            
        ft = self.get_ft(fname)
        if it != "" and ft != "":
            ft_utime = it_utime + tim.tp2sec(self.get_ft_pattern(), ft)
        else:
            ft_utime = None
        return (it_utime, ft_utime)
    
    def make_name(self, date, it, ft):
        """Make file name using the strings date, it and ft"""

        # get position of base name
        out = [""] * NUM_IND
        base_pos = self.info[BASE_IND].get_ct()
        out[base_pos] = self.patterns[base_pos]
        out[self.info[DATE_IND].get_ct()] = date

        # check if filling is set on, and then fill
        if self.fill:
            if it == "":
                length = len(self.patterns[ISSUE_IND])
                it = "0" * length
            if ft == "":
                length = len(self.patterns[FCST_IND])
                ft = "0" * length

        out[self.info[ISSUE_IND].get_ct()] = it
        out[self.info[FCST_IND].get_ct()] = ft
        suffix_pos = self.info[SUFFIX_IND].get_ct()
        out[suffix_pos] = self.patterns[suffix_pos]
        tup = tuple(out)
        return self.format % tup

    def make_name_dtime(self, dtime):
        """ make file name using dtime """
        date = dtime.get_date()
        it = dtime.get_it()
        ft = dtime.get_ft()

	# Handle forecast period indicators
	if self.get_ft_pattern() == "H":
	    ft = tim.h2fpind(int(ft))

        return self.make_name(date, it, ft)
        
    def make_name_utime(self, it_utime, ft_utime):
        """Make file name using issue and forecast times in UTC -- currently assumes forecast delta < 24h"""
        tstring = time.strftime("%Y%m%d.%H%M%S", time.gmtime(it_utime))
        date = tstring[0:8]
        istr = tstring[9:9+self.info[ISSUE_IND].get_length()]
        if ft_utime != None:
            (h, m, s) = tim.hms(ft_utime-it_utime)
            fstr = self.fcst_hms_format % (h, m, s)
            fstr = fstr[0:self.info[FCST_IND].get_length()]
        else:
            fstr = ""

	# Handle forecast period indicators
	if self.get_ft_pattern() == "H":
	    fstr = tim.h2fpind(int(fstr))

        return self.make_name(date, istr, fstr)

class Data_time:
    """Data time class -- container class for date, issue time and forecast time"""

    def __init__(self, date, issue_time, fcst_time):
	""" date - YYYYMMDD, issue_time - hhmmss, fcst_time - hhmmss """

        self.date = date
        self.it = issue_time
        self.ft = fcst_time

    def get_date(self):
        return self.date

    def get_it(self):
        return self.it

    def get_ft(self):
        return self.ft

    def pr(self):
        print "date: ", self.date
        print "it: ", self.it
        print "ft: ", self.ft


# Deprecated in favor of Name_schema
class Daily_fname:
    """Class for dealing with daily file names."""
	
    def __init__(self, base, suffix):
	""" base_name - base name of file. Note that the variables below depend on base."""

        # general file name format except for base name
        # base_name.YYYYMMDD.suffix

        self.base_name = base
        self.suffix = suffix
        self.suffix_len = len(self.suffix)
        self.setup()

    def setup(self):
        self.format = "%s.%s.%s"
        self.date_beg = -9 - self.suffix_len
        self.date_end = -1 - self.suffix_len

        
    def get_base(self):
	return self.base_name
    
    def get_date(self, fname):
        """ return date of fname """
        return fname[self.date_beg:self.date_end]

    def get_dtime(self, fname):
        """ return data time for fname of this name schema class """
        return Data_time(self.get_date(fname), "", "")
            
    def get_suffix(self):
        """ return suffix """
        return self.suffix

    def get_utime(self, fname):
        """ get unix time of fname """
        date = self.get_date(fname)
        date_tuple = tim.datetotuple(date)
        date_utime = tim.mkgmtime(date_tuple)
        return date_utime

    def make_name(self, date):
        """ make file name using base, suffix and date """
        return self.format % (self.base_name, date, self.suffix) 

    def make_name_dtime(self, dtime):
        """ make file name using base and data time object """
        date = dtime.get_date()
        return self.make_name(date)
        
    def make_name_utime(self, utime):
        """ make file name using base, suffix and utime """
        date = time.strftime("%Y%m%d", time.gmtime(utime))
        return self.format % (self.base_name, date, self.suffix) 
    


# Deprecated in favor of Name_schema
class Fcst_fname(Daily_fname):
    """Class for dealing with hourly forecast file names."""
	
    # general file name format except for base name
    # base_name.YYYYMMDD.ihh.fhh.suffix

    def setup(self):
        self.format = "%s.%s.i%s.f%s.%s"
        self.date_beg = -len("YYYYMMDD.ihh.fhh.") - self.suffix_len
        self.date_end = -len(".ihh.fhh.") - self.suffix_len
        self.it_beg = -len("hh.fhh.") - self.suffix_len
        self.it_end = -len(".fhh.") - self.suffix_len
        self.ft_beg = -len("hh.") - self.suffix_len
        self.ft_end = -self.suffix_len - 1
            

    def get_fcst_times(self, fname_list):
        """ Get the set of forecast times from an fname_list """
        out_list = []
        for f in fname_list:
            if f != None:
                out_list.append(self.get_utime(f)[1])
            else:
                out_list.append(None)
        return out_list

    def get_dtime(self, fname):
        """ return data time for fname of this name schema class """
        return Data_time(self.get_date(fname), self.get_it(fname), self.get_ft(fname))

    def get_ft(self, fname):
        return fname[self.ft_beg:self.ft_end]

    def get_it(self, fname):
        return fname[self.it_beg:self.it_end]

    def get_utime(self, fname):
        # get utime of date
        date = self.get_date(fname)
        it = self.get_it(fname)
        ft = self.get_ft(fname)
        it_tuple = tim.datetotuple("%s%s" % (date, it))
        it_utime = tim.mkgmtime(it_tuple)
        fh = int(ft)
        ft_utime = it_utime + 3600 * fh
        return (it_utime, ft_utime)
    
    def make_name(self, date, it, ft):
        """ make file name using base, suffix date, it and ft """
        return self.format % (self.base_name, date, it, ft, self.suffix) 

    def make_name_dtime(self, dtime):
        """ make file name using base, suffix and dtime """
        date = dtime.get_date()
        it = dtime.get_it()[0:2]
        ft = dtime.get_ft()[0:2]
        return self.make_name(date, it, ft)
        
    def make_name_utime(self, it, ft):
        """ make file name using base, suffix it and ft """
        tstring = time.strftime("%Y%m%d.%H", time.gmtime(it))
        date = tstring[0:8]
        istr = tstring[9:11]
        num_hours = (ft - it) / 3600
        fstr = "%02d" % num_hours
        return self.format % (self.base_name, date, istr, fstr, self.suffix) 


# Deprecated in favor of Name_schema
class Fcst_fname_min(Fcst_fname):
    """Class for dealing with forecast file names with minute information. """
	
    def setup(self):
        # general file name format except for base name
        # base_name.YYYYMMDD.ihhmm.fhhmm.suffix

        self.format = "%s.%s.i%s.f%s.%s"
        self.date_beg = -len("YYYYMMDD.ihhmm.fhhmm.") - self.suffix_len
        self.date_end = -len(".ihhmm.fhhmm.") - self.suffix_len
        self.it_beg = -len("hhmm.fhhmm.") - self.suffix_len
        self.it_end = -len(".fhhmm.") - self.suffix_len
        self.ft_beg = -len("hhmm.") - self.suffix_len
        self.ft_end = -self.suffix_len - 1
            
    def get_dtime(self, fname):
        """ return data time for fname of this name schema class """
        return Data_time(self.get_date(fname), self.get_it(fname), self.get_ft(fname))

    def get_ft(self, fname):
        return fname[self.ft_beg:self.ft_end]

    def get_it(self, fname):
        return fname[self.it_beg:self.it_end]

    def get_utime(self, fname):
        # get utime of date
        date = self.get_date(fname)
        it = self.get_it(fname)
        ft = self.get_ft(fname)
        it_tuple = tim.datetotuple("%s%s" % (date, it))
        it_utime = tim.mkgmtime(it_tuple)
        fh = int(ft[0:2])
        fm = int(ft[2:4])
        ft_utime = it_utime + 3600 * fh + 60 * fm
        return (it_utime, ft_utime)
    
    def make_name(self, date, it, ft):
        """ make file name using base, suffix date, it and ft """
        return self.format % (self.base_name, date, it, ft, self.suffix) 

    def make_name_dtime(self, dtime):
        """ Make output file name given data time object """
        date = dtime.get_date()
        it = dtime.get_it()[0:4]
        ft = dtime.get_ft()[0:4]
        return self.make_name(date, it, ft)
        
    def make_name_utime(self, it, ft):
        tstring = time.strftime("%Y%m%d.i%H%M", time.gmtime(it))
        date = tstring[0:8]
        istring = tstring[10:14]
        it = (int(it) / 60) * 60 # round to the nearest minute
        delta = int(ft - it)
        num_hour = int(delta / 3600)
        num_min = (delta - num_hour * 3600) / 60
        hstr = "%02d" % num_hour          
        mstr = "%02d" % num_min
        fstring = "%s%s" % (hstr, mstr)
        return self.format % (self.base_name, date, istring, fstring, self.suffix) 


# Deprecated in favor of Name_schema
class Data_fname(Daily_fname):
    """Class for dealing with data (non-forecast) file names."""
    def setup(self):
        # general file name format except for base name
        # base_name.YYYYMMDD.hhmm.suffix
        self.format = "%s.%s.%s.%s"
        self.date_beg = -14 - self.suffix_len
        self.date_end = -6 - self.suffix_len
        self.it_beg = -5 - self.suffix_len
        self.it_end = -1 - self.suffix_len
        
    def get_dtime(self, fname):
        """ return data time for fname of this name schema class """
        return Data_time(self.get_date(fname), self.get_it(fname), "")

    def get_it(self, fname):
        return fname[self.it_beg:self.it_end]

    def get_ih(self, fname):
        return fname[self.it_beg:self.it_beg+2]

    def get_im(self, fname):
        return fname[self.it_beg+2:self.it_beg+4]

    def get_utime(self, fname):
        """ get unix time using date, hhmm information """
        date = self.get_date(fname)
        it = self.get_it(fname)
        it_tuple = tim.datetotuple("%s%s" % (date, it))
        it_utime = tim.mkgmtime(it_tuple)
        return it_utime

    def make_name(self, date, time_string):
        return self.format % (self.base_name, date, time_string, self.suffix) 

    def make_name_dtime(self, dtime):
        """ make file name using base, suffix and dtime """
        date = dtime.get_date()
        it = dtime.get_it()[0:4]
        return self.make_name(date, it)
        
    def make_name_utime(self, utime):
        """ make file name using base, suffix and utime """
        tstring = time.strftime("%Y%m%d.%H%M", time.gmtime(utime))
        return self.format % (self.base_name, tstring[:8], tstring[9:], self.suffix)

    
def test_fname_obj():
    dtime = Data_time("20020130", "010203", "040506")
    in_base = "climo_fcst"
    fo = Daily_fname(in_base, "nc")
    sample = "climo_fcst.19990304.nc"
    print "sample ", sample
    base = fo.get_base()
    print "base: ", base
    date = fo.get_date(sample)
    print "date: ", date
    suffix = fo.get_suffix()
    print "suffix: ", suffix
    utime = fo.get_utime(sample)
    print "utime: ", utime
    utime_name = fo.make_name_utime(utime)
    print "name from utime: ", utime_name
    print "name from data time: ", fo.make_name_dtime(dtime)
    dtime.pr()
    dt = fo.get_dtime(fo.make_name_dtime(dtime))
    dt.pr()
    print "\n"
    
    
    in_base = "mrf"
    fo = Fcst_fname(in_base, "nc")
    sample = "mrf.19990304.i15.f03.nc"
    print "sample ", sample
    base = fo.get_base()
    print "base: ", base
    date = fo.get_date(sample)
    print "date: ", date
    suffix = fo.get_suffix()
    print "suffix: ", suffix
    utime = fo.get_utime(sample)
    print "utime: ", utime
    utime_name = fo.make_name_utime(utime[0], utime[1])
    print "name from utime: ", utime_name
    print "it ", fo.get_it(sample)
    print "ft ", fo.get_ft(sample)
    name = fo.make_name("19990630", "12", "09")
    print 'calling fo.make_name("19990630", "12", "09")'
    print "result: %s" % name
    print "name from data time: ", fo.make_name_dtime(dtime)
    dtime.pr()
    dt = fo.get_dtime(fo.make_name_dtime(dtime))
    dt.pr()
    print "\n"

    in_base = "int_fcst"
    fo = Fcst_fname_min(in_base, "nc")
    sample = "%s.19990304.i1515.f0045.nc" % in_base
    print "sample ", sample
    base = fo.get_base()
    print "base: ", base
    date = fo.get_date(sample)
    print "date: ", date
    suffix = fo.get_suffix()
    print "suffix: ", suffix
    utime = fo.get_utime(sample)
    print "utime: ", utime
    utime_name = fo.make_name_utime(utime[0], utime[1])
    print "name from utime: ", utime_name
    print "it ", fo.get_it(sample)
    print "ft ", fo.get_ft(sample)
    curr_time = int(time.time())
    curr_hour = tim.truncate_hour(curr_time)
    utime_name = fo.make_name_utime(curr_time, curr_hour + 3600)
    print "curr_time: ", curr_time
    print time.gmtime(curr_time)
    print "curr_hour + 3600: ", curr_hour + 3600
    print time.gmtime(curr_hour)
    print "diff: ", curr_hour + 3600 - curr_time
    print "name: ", utime_name
    name = fo.make_name("19990630", "1215", "0900")
    print 'calling fo.make_name("19990630", "1215", "0900")'
    print "result: %s" % name
    print "name from data time: ", fo.make_name_dtime(dtime)
    dtime.pr()
    dt = fo.get_dtime(fo.make_name_dtime(dtime))
    dt.pr()
    print "\n"

    in_base = "metar_interp"
    fo = Data_fname(in_base, "nc")
    sample = "metar_interp.20000806.0002.nc"
    print "sample ", sample
    base = fo.get_base()
    print "base: ", base
    date = fo.get_date(sample)
    print "date: ", date
    suffix = fo.get_suffix()
    print "suffix: ", suffix
    utime = fo.get_utime(sample)
    print "utime: ", utime
    utime_name = fo.make_name_utime(utime)
    print "name from utime: ", utime_name
    print "it ", fo.get_it(sample)
    print "ih ", fo.get_ih(sample)
    print "im ", fo.get_im(sample)
    name = fo.make_name("19990630", "0030")
    print 'calling fo.make_name("19990630", "0030")'
    print "result: %s" % name

    print "\n"
    print "making a name from the current time"
    name =  fo.make_name_utime(time.time())
    print "result: %s" % name
    print "name from data time: ", fo.make_name_dtime(dtime)
    dtime.pr()
    dt = fo.get_dtime(fo.make_name_dtime(dtime))
    dt.pr()


        
def run_ns_tests():
    patterns = [0] * NUM_IND
    ct = 1
    fname = "sat.20010130.i12.f13.nc"
    suffix = "nc"
    it = "12"
    itf = "HH"
    ft = "13"
    ftf = "HH"
    date = "20010130"
    datef = "YYYYMMDD"
    format_string = "%B.%D.i%I.f%F.%S"
    (patterns[BASE_IND], patterns[DATE_IND], patterns[ISSUE_IND], patterns[FCST_IND], patterns[SUFFIX_IND]) = "sat", datef, itf, ftf, suffix
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 2
    fname = "20010131.i10.pireps.f11.grib"
    suffix = "grib"
    it = "10"
    itf = "HH"
    ft = "11"
    ftf = "HH"
    date = "20010131"
    format_string = "%D.i%I.%B.f%F.%S"
    patterns = ["YYYYMMDD", itf, "pireps", ftf, suffix]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 3
    fname = "20110131.f10.i11.metars.txt"
    suffix = "txt"
    it = "11"
    itf = "HH"
    ft = "10"
    ftf = "HH"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 4
    fname = "20110131.f1011.i1112.metars.txt"
    suffix = "txt"
    it = "1112"
    itf = "HHMM"
    ft = "1011"
    ftf = "HHMM"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 5
    fname = "20110131.f101101.i121112.metars.txt"
    suffix = "txt"
    it = "121112"
    itf = "HHMMSS"
    ft = "101101"
    ftf = "HHMMSS"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 6
    fname = "20050130_i12_f100_RUC13kmDEV2p_20.grb"
    suffix = "grb"
    it = "12"
    itf = "HH"
    ft = "100"
    ftf = "HHH"
    date = "20050130"
    format_string = "%D_i%I_f%F_%B.%S"
    patterns = ["YYYYMMDD", "HH", "HHH", "RUC13kmDEV2p_20", "grb"]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 7
    fname = "20050130_i123045_f54330_RUC13kmDEV2p_20.grb"
    suffix = "grb"
    it = "123045"
    itf = "HHMMSS"
    ft = "54330"
    ftf = "HHHMM"
    date = "20050130"
    format_string = "%D_i%I_f%F_%B.%S"
    patterns = ["YYYYMMDD", "HHMMSS", "HHHMM", "RUC13kmDEV2p_20", "grb"]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)

    ct = 8
    fname = "ruc2.bgrb.20070118.i15.f12"
    suffix = ""
    it = "15"
    itf = "HH"
    ft = "12"
    ftf = "HH"
    date = "20070118"
    format_string = "%B.%D.i%I.f%F%S"
    patterns = ["ruc2.bgrb", "YYYYMMDD", "HH", "HH", ""]
    test_name_schema(ct, format_string, patterns, date, it, ft, suffix, fname)


def test_name_schema(test_num, format_string, patterns, date, it, ft, suffix, fname):
    ns = Name_schema(format_string, patterns)
    if ns.get_date(fname) != date:
        print fname
        print "ns.get_date(fname) = %s, date = %s" % (ns.get_date(fname), date)
        print "Failed test:", test_num
        
    if ns.get_base() != patterns[BASE_IND]:
        print fname
        print "ns.get_base(fname) = %s, patterns[BASE_IND] = %s" % (ns.get_base(), patterns[BASE_IND])
        print "Failed test: ", test_num

    if ns.get_it(fname) != it:
        print fname
        print "ns.get_it(): %s, it: %s" % (ns.get_it(fname), it)
        print "Failed test: ", test_num

    if ns.get_ft(fname) != ft:
        print "ns.get_ft(): %s, ft: %s" % (ns.get_ft(fname), ft)
        print "Failed test: ", test_num

    if ns.get_suffix() != suffix:
        print "fname: %s, suffix: %s" % (fname, suffix)
        print "Failed test: ", test_num

    ut = ns.get_utime(fname)

    name = ns.make_name_utime(ut[0], ut[1])
    if name != fname:
        print "fname: %s, name: %s" % (fname, name)
        print "Failed test: ", test_num
        
def ns_examples():

    Ruc_in_ns_pattern = ["YYYYMMDD", "HH", "HHH", "RUC13kmDEV2p_20", "grb"]
    Ruc_in_ns_format = "%D_i%I_f%F_%B.%S"
    ns = Name_schema(Ruc_in_ns_format, Ruc_in_ns_pattern)
    print "it pattern string: ", ns.get_it_pattern()
    print "ft pattern string: ", ns.get_ft_pattern()
    print "date pattern string: ", ns.get_date_pattern()
    fname = "20050130_i12_f012_RUC13kmDEV2p_20.grb"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name

    Ruc_in_ns_pattern = ["YYYYMMDD", "HHMM", "HHH", "RUC13kmDEV2p_20", "grb"]
    Ruc_in_ns_format = "%D_i%I_f%F_%B.%S"
    ns = Name_schema(Ruc_in_ns_format, Ruc_in_ns_pattern)
    print "it pattern string: ", ns.get_it_pattern()
    print "ft pattern string: ", ns.get_ft_pattern()
    print "date pattern string: ", ns.get_date_pattern()
    fname = "20050130_i2330_f987_RUC13kmDEV2p_20.grb"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name

    patterns = ["HH", "HH", "pirep", "YYYYMMDD", "nc"]
    format_string = "f%F.i%I.%B.%D.%S"
    ns = Name_schema(format_string, patterns)
    print "it pattern string: ", ns.get_it_pattern()
    print "ft pattern string: ", ns.get_ft_pattern()
    print "date pattern string: ", ns.get_date_pattern()
    
    fname = "f12.i13.pirep.20021112.nc"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name

    name = ns.make_name("20030130", "01", "23")
    print "making name from: '20030130', '01', '23'"
    print "name: ", name
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name

    print "\nnew pattern"
    patterns = ["YYYYMMDD", "sat", "HHMMSS", "HHMM", "nc"]
    ns = Name_schema("%D.%B.f%F.i%I.%S", patterns)
    print "base: ", ns.get_base()
    print "it pattern string: ", ns.get_it_pattern()
    print "ft pattern string: ", ns.get_ft_pattern()
    fname = "20021112.sat.f120102.i1312.nc"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    print "fname: ", fname
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "make name utime: ", name
    name = ns.make_name("20030130", "012345", "023001")
    print "making name from: '20030130', '012345', '023001'"
    print "name: ", name

    print "\nnew pattern"
    patterns = ["pirep", "YYYYMMDD", "", "", "nc"]
    ns = Name_schema("%B.%D.%I%F%S", patterns)
    print "base: ", ns.get_base()
    fname = "pirep.20021112.nc"
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name("20030130", "", "")
    print "making name from: '20030130', '', ''"
    print "name: ", name


    print "\nnew pattern"
    patterns = ["ruc_ta", "YYYYMMDD", "HH", "HH", "nc"]
    ns = Name_schema("%B.%D.i%I.f%F.%S", patterns)
    print "base: ", ns.get_base()
    fname = "ruc_ta.20031007.i06.f02.nc"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name
    name = ns.make_name("20030130", "01", "02")
    print "making name from: '20030130', '01', '02'"
    print "name: ", name


    print "\nnew pattern"
    patterns = ["YYYYMMDD", "HHMM", "ncv-fcst", "H", "grb"]
    ns = Name_schema("%D%I_%B_%F.%S", patterns)
    print "base: ", ns.get_base()
    fname = "200310070635_ncv-fcst_A.grb"
    print "fname: ", fname
    print "date: ", ns.get_date(fname)
    print "it: ", ns.get_it(fname)
    print "ft: ", ns.get_ft(fname)
    print "dtime: "
    ns.get_dtime(fname).pr()
    ut = ns.get_utime(fname)
    print "utime: ", ut
    name = ns.make_name_utime(ut[0], ut[1])
    print "utime name: ", name
    print "making name from: '20030130', '0125', '03'"
    dt = Data_time("20030130", "0125", "03")
    name = ns.make_name_dtime(dt)
    print "name: ", name



if __name__ == '__main__':
    run_ns_tests()
    #ns_examples()
    #test_fname_obj()

