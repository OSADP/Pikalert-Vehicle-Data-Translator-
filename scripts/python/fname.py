#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/02/02 20:17:38 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import time
#import tim


class filefun_obj:
    """Class for converting from input file names to output file names."""
    def __init__(self, in_base, out_base):
	"""in_base - input base name of file, out_base - output base name of file"""
        self.in_fname_obj = fname_obj(in_base)
        self.out_fname_obj = fname_obj(out_base)

    def in_fname_obj(self):
        return self.in_fname_obj

    def out_fname_obj(self):
        return self.out_fname_obj
    
    def out_ending(self):
        return self.out_fname_obj.get_end()
    
    def filefun(self, infile):
        """Create output file name from input file name"""
        return self.out_fname_obj.make_name(self.in_fname_obj.get_date(infile), self.in_fname_obj.get_gh(infile), self.in_fname_obj.get_fh(infile))
    
    
class fcast_fname_obj:
    """Class for dealing with forecast file names."""
	
    def __init__(self, base, suffix):
	""" base_name - base name of file. Note that the variables below depend on base."""

        # general file name format except for base name
        # base_name.YYYYMMDD.ihhmm.fhhhmm.suffix

        self.base_name = base
        self.suffix = suffix
        self.format = "%s.%s.i%s.f%s.%s"
        self.suffix_len = len(self.suffix)

        self.date_beg = -22 - self.suffix_len
        self.date_end = -14 - self.suffix_len

        self.it_beg = -12 - self.suffix_len
        self.it_end = -8 - self.suffix_len
        self.ft_beg = -6 - self.suffix_len
        
        self.ft_end = -self.suffix_len - 1
            
    def get_suffix(self):
        return self.suffix
    
    def get_base(self):
	return self.base_name
        
    def get_date(self, fname):
        return fname[self.date_beg:self.date_end]

    def get_fh(self, fname):
        return fname[self.ft_beg:self.ft_beg+3]

    def get_fm(self, fname):
        return fname[self.ft_beg+3:self.ft_beg+5]

    def get_ft(self, fname):
        return fname[self.ft_beg:self.ft_end]

    def get_it(self, fname):
        return fname[self.it_beg:self.it_end]

    def get_ih(self, fname):
        return fname[self.it_beg:self.it_beg+2]

    def get_im(self, fname):
        return fname[self.it_beg+2:self.it_beg+4]

    def make_name(self, date, it, ft):
        return self.format % (self.base_name, date, it, ft, self.suffix) 

"""
    def get_time(self, fname):
        date = self.get_date(fname)
        it = self.get_it(fname)
        ft = self.get_ft(fname)
        hour = int(it) + int(ft)
        mod_date = "%s%02d" % (date, hour)
        tup = tim.datetotuple(mod_date)
        return tim.mkgmtime(tup)
"""



class data_fname_obj:
    """Class for dealing with data (non-forecast) file names."""
	
    def __init__(self, base, version, suffix):
	""" base_name - base name of file. Note that the variables below depend on base."""

        # general file name format except for base name
        # base_name.vXX.YYYYMMDD.hhmm.suffix

        self.base_name = base
        self.version = version
        self.suffix = suffix
        self.format = "%s%s.%s.%s.%s"
        self.suffix_len = len(self.suffix)

        self.date_beg = -14 - self.suffix_len
        self.date_end = -6 - self.suffix_len

        self.it_beg = -5 - self.suffix_len
        self.it_end = -1 - self.suffix_len
        
    def get_base(self):
	return self.base_name
        
    def get_suffix(self):
        return self.suffix
    
    def get_version(self):
	return self.version
        
    def get_date(self, fname):
        return fname[self.date_beg:self.date_end]

    def get_it(self, fname):
        return fname[self.it_beg:self.it_end]

    def get_ih(self, fname):
        return fname[self.it_beg:self.it_beg+2]

    def get_im(self, fname):
        return fname[self.it_beg+2:self.it_beg+4]

    def make_name(self, date, time_string):
        return self.format % (self.base_name, self.version, date, time_string, self.suffix) 

    def make_name_utime(self, utime):
        tstring = time.strftime("%Y%m%d.%H%M", utime)
        return self.format % (self.base_name, self.version, tstring[:8], tstring[9:], self.suffix)
    
class daily_fname_obj:
    """Class for dealing with daily file names."""
	
    def __init__(self, base, version, suffix):
	""" base_name - base name of file. Note that the variables below depend on base."""

        # general file name format except for base name
        # base_name.vXX.YYYYMMDD.suffix

        self.base_name = base
        self.version = version
        self.suffix = suffix
        if version != "":
            self.format = "%s%s.%s.%s"
        else:
            self.format = "%s.%s.%s"
            
        self.suffix_len = len(self.suffix)

        self.date_beg = -9 - self.suffix_len
        self.date_end = -1 - self.suffix_len

        
    def get_base(self):
	return self.base_name
        
    def get_suffix(self):
        return self.suffix
    
    def get_version(self):
	return self.version
        
    def get_date(self, fname):
        return fname[self.date_beg:self.date_end]

    def make_name(self, date):
        if self.version == "":
            return self.format % (self.base_name, date, self.suffix) 
        else:
            return self.format % (self.base_name, self.version, date, self.suffix) 

    def make_name_utime(self, utime):
        tstring = time.strftime("%Y%m%d.%H%M", utime)
        if self.version == "":
            return self.format % (self.base_name, date, self.suffix) 
        else:
            return self.format % (self.base_name, self.version, tstring[:8], self.suffix)
    
"""
    def get_time(self, fname):
        date = self.get_date(fname)
        it = self.get_it(fname)
        ft = self.get_ft(fname)
        hour = int(it) + int(ft)
        mod_date = "%s%02d" % (date, hour)
        tup = tim.datetotuple(mod_date)
        return tim.mkgmtime(tup)
        
"""



def test_fname_obj():
    in_base = "mrf"
    fo = fcast_fname_obj(in_base, "nc")
    sample = "mrf.19990304.i1520.f00330.nc"
    print "sample ", sample
    date = fo.get_date(sample)
    print "date ", date
#    time_val = fo.get_time(sample)
#    print "time_val ", time_val
    print "it ", fo.get_it(sample)
    print "ih ", fo.get_ih(sample)
    print "im ", fo.get_im(sample)
    print "ft ", fo.get_ft(sample)
    print "fh ", fo.get_fh(sample)
    print "fm ", fo.get_fm(sample)
    print "base ", fo.get_base()
    print "\n"
    name = fo.make_name("19990630", "0030", "01020")
    print 'calling fo.make_name("19990630", "0030", "01020")'
    print "result: %s" % name

    in_base = "climo_fcst"
    fo = data_fname_obj(in_base, "00", "nc")
    sample = "climo_fcst.19990304.1520.nc"
    print "sample ", sample
    date = fo.get_date(sample)
    print "date ", date
#    time_val = fo.get_time(sample)
#    print "time_val ", time_val
    print "it ", fo.get_it(sample)
    print "ih ", fo.get_ih(sample)
    print "im ", fo.get_im(sample)
    print "base ", fo.get_base()
    print "\n"
    name = fo.make_name("19990630", "0030")
    print 'calling fo.make_name("19990630", "0030")'
    print "result: %s" % name
    name =  fo.make_name_utime(time.gmtime(time.time()))
    print "result: %s" % name

    in_base = "climo_fcst"
    fo = daily_fname_obj(in_base, "00", "nc")
    sample = "climo_fcst.19990304.nc"
    print "sample ", sample
    date = fo.get_date(sample)
    print "date ", date

        
if __name__ == '__main__':
    test_fname_obj()








