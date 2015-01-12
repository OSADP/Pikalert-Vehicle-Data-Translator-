#!/usr/bin/env python

#/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
# * Copyright (c) 1995-2007 UCAR
# * University Corporation for Atmospheric Research(UCAR)
# * National Center for Atmospheric Research(NCAR)
# * Research Applications Program(RAP)
# * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
# * All rights reserved. Licenced use only.
# * $Date: 2014/07/08 23:01:33 $
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


import os
import re
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
# All tokens must have length 2 and use the same starting
# delimiter. This delimiter cannot be used in the format string except
# to delimit tokens
Token_len = 2
Delimiter = "%"

# Token list information
(BASE_LETTER, DATE_LETTER, ISSUE_LETTER, FCST_LETTER, SUFFIX_LETTER) = ["B", "D", "I", "F", "S"]
Format_letters = [BASE_LETTER, DATE_LETTER, ISSUE_LETTER, FCST_LETTER, SUFFIX_LETTER]
Format = ["%%%c" % letter for letter in Format_letters]

def find_delimiter_loc(delimiter, str):
    """Return a list of delimiter locations in the str"""
    
    out = []
    pos = 0
    while pos < len(str):
        if str[pos] == "%":
            out.append(pos)
        pos += 1

    return out


def find_token_intervals(delimiter, format, patterns):
    """Returns list of token intervals and output file name length"""
    locations = find_delimiter_loc(delimiter, format)
    locations.append(len(format))
    curr_location = locations[0]
    out = []
    for ind in range(len(patterns)):
        pattern_len = len(patterns[ind])
        out.append((curr_location, curr_location+pattern_len))
        curr_location += pattern_len + (locations[ind+1] - locations[ind] - 2)
    return (out, curr_location)

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

def format_letters(format):
    """return format letters in format string"""
    out = []
    pos = 0
    while pos < len(format):
        if format[pos] == "%" and format[pos+1] in Format_letters:
            out.append(format[pos+1])
            pos += 2
        else:
            pos += 1
    return "".join(out)

def format_convert(format):
    """replace format letters with s for string processing"""
    out = []
    pos = 0
    while pos < len(format):
        if format[pos] == "%" and format[pos+1] in Format_letters:
            out.append(format[pos])
            out.append("s")
            pos += 2
        else:
            out.append(format[pos])
            pos += 1
    return "".join(out)

def format_indices(format_letter_string):
    """return format indices based on a string of format letters"""
    base_ind = format_letter_string.find(BASE_LETTER)
    date_ind = format_letter_string.find(DATE_LETTER)
    issue_ind = format_letter_string.find(ISSUE_LETTER)
    fcst_ind = format_letter_string.find(FCST_LETTER)
    suffix_ind = format_letter_string.find(SUFFIX_LETTER)
    return (base_ind, date_ind, issue_ind, fcst_ind, suffix_ind)


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
        self.format_letters = format_letters(format_string)
        (self.BASE_IND, self.DATE_IND, self.ISSUE_IND, self.FCST_IND, self.SUFFIX_IND) = format_indices(self.format_letters)
        
        (self.token_intervals, self.name_len) = find_token_intervals(Delimiter, format_string, patterns)
        self.format_string = format_string
        self.format = format_convert(format_string)
        self.patterns = patterns
        self.num_ind = len(patterns)
        
        if not valid_issue_pattern(patterns[self.ISSUE_IND]):
            raise ValueError, "Issue time pattern %s must be in [\"\", \"H\", \"HH\", \"HHMM\", \"HHMMSS\"]" % patterns[self.ISSUE_IND]

        self.fcst_hms = hms_forecast_pattern(patterns[self.FCST_IND])
        if patterns[self.FCST_IND] == "":
            self.fcst_hms_format = ""
        else:
            if self.fcst_hms == None:
                raise ValueError, "Forecast time pattern %s must be in [\"\", \"H\", \"HH...H\", \"HH...HMM\", \"HH...HMMSS\"]" % patterns[self.FCST_IND]
            self.fcst_hms_format = "%%0%dd%%02d%%02d" % self.fcst_hms[0]
        return
    
    def set_fill(self):
        """ Fill empty fields with 0's according to input patterns """
        self.fill = 1
        return

    def pr(self):
        print "format_string: ", self.format_string
        print "format: ", self.format
        print "patterns: ", self.patterns
            
    def get_base(self):
        return self.patterns[self.BASE_IND]

    def get_suffix(self):
        return self.patterns[self.SUFFIX_IND]

    def get_date(self, fname):
        """Return date string for given file fname"""
        interval = self.token_intervals[self.DATE_IND]
        base_fname = os.path.basename(fname)
        return base_fname[interval[0]:interval[1]]

    def get_date_pattern(self):
        """Return date pattern string"""
        return self.patterns[self.DATE_IND]

    def get_ft(self, fname):
        """Return forecast time string for given file fname"""
        interval = self.token_intervals[self.FCST_IND]
        base_fname = os.path.basename(fname)
        return base_fname[interval[0]:interval[1]]

    def get_ft_pattern(self):
        """Return ft pattern string"""
        return self.patterns[self.FCST_IND]

    def get_it(self, fname):
        """Return issue time string for given file fname"""
        interval = self.token_intervals[self.ISSUE_IND]
        base_fname = os.path.basename(fname)
        return base_fname[interval[0]:interval[1]]

    def get_it_pattern(self):
        """Return it pattern string"""
        return self.patterns[self.ISSUE_IND]

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
        out = [""] * self.num_ind
        out[self.BASE_IND] = self.patterns[self.BASE_IND]
        out[self.DATE_IND] = date

        # check if filling is set on, and then fill with 0's
        if self.fill:
            if it == "":
                length = len(self.patterns[self.ISSUE_IND])
                it = "0" * length
            if ft == "":
                length = len(self.patterns[self.FCST_IND])
                ft = "0" * length

        out[self.ISSUE_IND] = it
        out[self.FCST_IND] = ft
        out[self.SUFFIX_IND] = self.patterns[self.SUFFIX_IND]
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
        istr = tstring[9:9+len(self.patterns[self.ISSUE_IND])]

        if ft_utime != None:
            (h, m, s) = tim.hms(ft_utime-it_utime)
            fstr = self.fcst_hms_format % (h, m, s)
            fstr = fstr[0:len(self.patterns[self.FCST_IND])]
        else:
            fstr = ""

	# Handle forecast period indicators
	if self.get_ft_pattern() == "H":
	    fstr = tim.h2fpind(int(fstr))

        return self.make_name(date, istr, fstr)

    def get_name_len(self):
        return self.name_len
    
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


def run_ns_tests():
    patterns = [0] * len(Format)
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
    base = "sat"
    patterns = ["sat", datef, itf, ftf, suffix]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 2
    fname = "20010131.i10.pireps.f11.grib"
    suffix = "grib"
    it = "10"
    itf = "HH"
    ft = "11"
    ftf = "HH"
    date = "20010131"
    format_string = "%D.i%I.%B.f%F.%S"
    base = "pireps"
    patterns = ["YYYYMMDD", itf, "pireps", ftf, suffix]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 3
    fname = "20110131.f10.i11.metars.txt"
    suffix = "txt"
    it = "11"
    itf = "HH"
    ft = "10"
    ftf = "HH"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    base = "metars"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 4
    fname = "20110131.f1011.i1112.metars.txt"
    suffix = "txt"
    it = "1112"
    itf = "HHMM"
    ft = "1011"
    ftf = "HHMM"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    base = "metars"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 5
    fname = "20110131.f101101.i121112.metars.txt"
    suffix = "txt"
    it = "121112"
    itf = "HHMMSS"
    ft = "101101"
    ftf = "HHMMSS"
    date = "20110131"
    format_string = "%D.f%F.i%I.%B.%S"
    base = "metars"
    patterns = ["YYYYMMDD", ftf, itf, "metars", suffix]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 6
    fname = "20050130_i12_f100_RUC13kmDEV2p_20.grb"
    suffix = "grb"
    it = "12"
    itf = "HH"
    ft = "100"
    ftf = "HHH"
    date = "20050130"
    format_string = "%D_i%I_f%F_%B.%S"
    base = "RUC13kmDEV2p_20"
    patterns = ["YYYYMMDD", "HH", "HHH", "RUC13kmDEV2p_20", "grb"]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 7
    fname = "20050130_i123045_f54330_RUC13kmDEV2p_20.grb"
    suffix = "grb"
    it = "123045"
    itf = "HHMMSS"
    ft = "54330"
    ftf = "HHHMM"
    date = "20050130"
    format_string = "%D_i%I_f%F_%B.%S"
    base = "RUC13kmDEV2p_20"
    patterns = ["YYYYMMDD", "HHMMSS", "HHHMM", "RUC13kmDEV2p_20", "grb"]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 8
    fname = "ruc2.bgrb.20070118.i15.f12"
    suffix = ""
    it = "15"
    itf = "HH"
    ft = "12"
    ftf = "HH"
    date = "20070118"
    format_string = "%B.%D.i%I.f%F%S"
    base = "ruc2.bgrb"
    patterns = ["ruc2.bgrb", "YYYYMMDD", "HH", "HH", ""]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)

    ct = 9
    fname = "i18.f02.ruc_20.20050224.000000"
    suffix = "000000"
    it = "18"
    itf = "HH"
    ft = "02"
    ftf = "HH"
    date = "20050224"
    format_string = "i%I.f%F.%B.%D.%S"
    base = "ruc_20"
    patterns = ["HH", "HH", "ruc_20", "YYYYMMDD", "000000"]
    test_name_schema(ct, format_string, patterns, base, date, it, ft, suffix, fname)
    
def test_name_schema(test_num, format_string, patterns, base, date, it, ft, suffix, fname):
    ns = Name_schema(format_string, patterns)

    if ns.get_date(fname) != date:
        print fname
        print "ns.get_date(fname) = %s, date = %s" % (ns.get_date(fname), date)
        print "Failed test:", test_num
        
    if ns.get_base() != base:
        print fname
        print ns.get_base()
        print base
        print "Failed test: ", test_num

    if ns.get_it(fname) != it:
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

    print "------------------------------------"
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
    print "------------------------------------"

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

    print "------------------------------------"
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

    print "------------------------------------"
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

    print "------------------------------------"
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


    print "------------------------------------"
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


    print "------------------------------------"
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

def sample():
    print "20070213_i05_f000_RUC252b.grb"
    print find_token_intervals(Delimiter, "%D_i%I_f%F_%B.%S", ["YYYYMMDD", "HH", "HHH", "RUC252b", "grb"])
    print find_delimiter_loc(Delimiter, "%D_i%I_f%F_%B.%S")
    print format_convert("%D_i%I_f%F_%B.%S")
    

def sample1():
    ns = Name_schema("%D_i%I_f%F_%B.%S", ["YYYYMMDD", "HH", "HHH", "RUC252b", "grb"])
    print ns.get_base()
    print ns.get_suffix()
    print ns.get_date("20070213_i05_f000_RUC252b.grb")
    print ns.get_ft("20070213_i05_f000_RUC252b.grb")
    print ns.get_it("20070213_i05_f000_RUC252b.grb")
    print ns.make_name("20080213", "29", "115")
    print ns.make_name_utime(tim.datetogmt("2008021310"), tim.datetogmt("2008021316"))
    print ns.get_name_len()
    
if __name__ == '__main__':
    sample1()
    #run_ns_tests()

    #ns_examples()
    #test_fname_obj()
