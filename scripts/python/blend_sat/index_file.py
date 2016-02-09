#!/usr/bin/env python


import os
from stat import *
import string
import time

IndexError = "Index_file module error: "

DAILY = 2
HOURLY = 3
MONTHLY = 4

DAILY_FORMAT = "%Y%m%d"
HOURLY_FORMAT = "%Y%m%d.%H"
MONTHLY_FORMAT = "%Y%m"

# Return position after last newline or 0
def last_newline(file):
    try:
        fp = open(file, "r")
        buffer = fp.read()
        pos = string.rfind(buffer, "\n") + 1
        fp.close()
    except:
        pos = 0

    return (pos)

    
def last_line(file):
    """ Return position of beginning of last line or 0 """
    try:
        fp = open(file, "r")
        buffer = fp.read()
        pos = string.rfind(buffer, "\n", 0, len(buffer)-1) + 1
        fp.close()
    except:
        pos = 0

    return (pos)

    
def read_line_offset(file, offset):
    """ Seek to offset in file, read a line and return the line and new offset """
    fp = open(file, "r")
    fp.seek(offset, 0)
    line = fp.readline()
    offset = fp.tell()
    fp.close()
    return (line, offset)


def index_file_path(base_dir, index_name, date_string):
    """
    Create path to index file.
    The arg date_string can be in one of two forms: yyyymmdd or yyyymmdd.hh.
    """
    ifp = os.path.join(base_dir, "%s.%s" % (index_name, date_string))
    return ifp

class Index_file:
    """Class for dealing with directory index files."""

    MAX_WAIT = 3600
    
    def __init__(self, base_dir, index_name, hours, rdwr, wait):
	# base_dir - the base directory where new files will be stored
	# index_name - the root name of the index files
	# hours - either 1 or 24
	# rdwr - "r" or "w"

        if (wait > Index_file.MAX_WAIT):
	    raise IndexError, "Index file wait period must be < %d seconds" % Index_file.MAX_WAIT
            
	if (hours != 1 and hours != 24 and hours != (24 * 30)):
	    raise IndexError, "Index file hours must be 1, 24, 24 * 30"
            
        if (hours == 24):
	    self.format = DAILY_FORMAT
	    self.period = DAILY
	elif hours == 1:
	    self.format = HOURLY_FORMAT
	    self.period = HOURLY
        elif hours == 24 * 30:
	    self.format = MONTHLY_FORMAT
	    self.period = MONTHLY

	if (rdwr != "w" and rdwr != "r"):
	    raise IndexError, "Index file was not open for reading or writing."
	self.rdwr = rdwr

        if (base_dir[-1] == "\n"):
            self.base_dir = base_dir[:-1]
        else:
            self.base_dir = base_dir
            
        if (rdwr == "r"):
            date = time.time() - wait
        else:
            date = time.time()
            
	self.index_name = index_name
        self.hours = hours
        self.wait = wait
        
        self.date_string = self.get_date_string(date)
        
        self.curr_index_file = index_file_path(self.base_dir, self.index_name, self.date_string)
#        self.curr_index_file = "%s/%s.%s" % (self.base_dir, self.index_name, self.date_string)

        self.last_offset = last_newline(self.curr_index_file)
        

    # Reset position in index file back to 0
#    def reset(self):
#        self.last_offset = 0

    def readlines(self, date):
	"""
        Read all lines in index file determined by date string.
        If last line does not have a newline separator, delete it.
        """

        if type(date) != type("string"):
            date_string = self.get_date_string(date)
        else:
            date_string = date

        curr_index_file = index_file_path(self.base_dir, self.index_name, date_string)
        # print 'curr_index_file ', curr_index_file
        try:
            fp = open(curr_index_file, "r")
            lines = fp.readlines()
            if lines[-1][-1] != '\n':
                del lines[-1]
            return(lines)
        except:    
            return([])
        
    
    def readlines_num(self, date_list, num, rev):
        """
        Read num lines from index files corresponding to date_list. If rev != 0, the lines
        are read in reverse order so that newer lines are output first.  Return a list
        of the lines and corresponding dates found which may be [], [].
        """
        lines = []
        dates = []
        for date in date_list:
            new_lines = self.readlines(date)
            if rev != 0:
                new_lines.reverse()
            lines = lines + new_lines
            len_lines = len(lines)
            dates = dates + [self.get_date_string(date)] * len_lines
            if len(lines) > num:
                break

        return(lines[0:num], dates[0:num])


    def write(self, line, date):
	"""
        Write new entry in 'line' to index file named according to the given date.
        Include additional newline separator.
        Note that date_string is provided by the caller and is not
        determined in the routine.  This allows the caller to:

        a. Find the date
        b. Move a file into the appropriate dated directory
        c. Update the appropriate index file
        d. Reset the current index file
        """

        if type(date) != type("string"):
            date_string = self.get_date_string(date)
        else:
            date_string = date

	if (self.rdwr != "w"):
	    raise IndexError, "Index file was not initialized for writing."
        
        if (date_string != self.date_string):
            
	    # Note that we do not check whether the last entry in a
	    # previous index file has a newline separator at the
	    # end. This can happen due to process failure. If it does
	    # not have a newline separator, the read routine will
	    # eventually skip the entry after the new index file has
	    # been created.

	    # Generate new index file name 
	    self.date_string = date_string
	    self.curr_index_file = index_file_path(self.base_dir, self.index_name, self.date_string)

        if (not os.path.exists(self.curr_index_file)):
	    try:
		fp = open(self.curr_index_file, "w")
		fp.write("%s\n" % line)
		fp.close()
	    except:
		raise IndexError, "Could not create index file %s" % self.curr_index_file
	else:

	    # The index file will be modified in place even though
	    # writes may not occur atomically and no file locking is
	    # in force. Note that index files support only one
	    # writer. The read() function will not return an entry
	    # that does not have a newline separator. The write
	    # routine will always check that the last entry ends in a
	    # newline separator otherwise it will delete the last
	    # entry before appending a new entry. This will prevent
	    # index file corruption that can occur if an index file writer
            # exits abnormally.

	    try:
		fp = open(self.curr_index_file, "r+")
		fp.seek(-1, 2)
		buf = fp.read(1)
                ret = fp.tell()

		if (len(buf) > 0):
		    if (buf[0] == "\n"):
                        # next seek is needed on Sun systems 
                        fp.seek(0, 2)
			fp.write("%s\n" % line)
		    else:
			# seek to beginning of file
			fp.seek(0, 0)

			# read all lines in the file
			line_list = fp.readlines()

			tmp_file = "%s.tmp" % self.curr_index_file
			try:
			    tmp_fp = open(tmp_file, "w")

			    # write all lines excluding damaged line to temporary file
			    tmp_fp.writelines(line_list[:-1])

			    # append new line
			    tmp_fp.write("%s\n" % line)

			    tmp_fp.close()

			    # replace current index file with new one
			    ret = os.system("mv %s %s" % (tmp_file, self.curr_index_file))
			    if (ret != 0):
				raise IndexError, "Could not rename temporary file %s to %s after fixing damaged line." % (tmp_file, self.curr_index_file)

			except:
			    raise IndexError, "Could not open temporary file %s" % tmp_file

		else:
		    fp.write("%s\n" % line)
		fp.close()

	    except:
		raise IndexError, "IO error occured for file %s" % self.curr_index_file
	    

    # Check whether current index file has changed in size. If so
    # return 1, else return 0
    def change_size(self):
	try:
	    stat_ret = os.stat(self.curr_index_file)
	    curr_offset = stat_ret[ST_SIZE]
	    if (curr_offset > self.last_offset):
		return 1
	    else:
		return 0
	except:
	    return 0


    def read(self):
	"""Return new entry from index file if available in the form [date_string, line]"""
	if (self.rdwr != "r"):
	    raise IndexError, "Index file was not initialized for reading."
	    
	if (self.change_size()):
	    # The current index file has changed in size
	    (line, new_offset) = read_line_offset(self.curr_index_file, self.last_offset)
	    if (line[-1] == "\n"):
		self.last_offset = new_offset
		return (self.date_string, line)
	else:
	    # The current index file is not ok or has not changed in size so check whether the
	    # time period has changed.
	    curr_date = time.time()
            date_string = self.get_date_string(curr_date)
            if (date_string != self.date_string):
		# Generate new index file name
		curr_index_file = index_file_path(self.base_dir, self.index_name, date_string)

		# Check whether a new index file exists
		if (os.path.exists(curr_index_file)):
		    
		    # One last check to see if current index file has changed in size.
		    # If we're unsuccessful, we go on to the new index file
		    if (self.change_size()):
			# The current index file has changed in size
			(line, last_offset) = read_line_offset(self.curr_index_file, self.last_offset)
			if (line[-1] == "\n"):
			    self.last_offset = last_offset
			    return (self.date_string, line)

		    # The new index file exists and the old one does
		    # not contain a new valid entry so go on to the new index file.
		
		    # Generate new index file name
		    self.date_string = date_string
		    self.curr_index_file = index_file_path(self.base_dir, self.index_name, self.date_string)
		    self.last_offset = 0

		    if (self.change_size()):
			# The current index file has changed in size
			(line, last_offset) = read_line_offset(self.curr_index_file, self.last_offset)
			if (line[-1] == "\n"):
			    self.last_offset = last_offset
			    return (self.date_string, line)

	return ("", "")


    def file_processed(self, input_fname, date):

        proc_list = self.readlines(date)
        for fstr in proc_list:
            if (string.find(fstr, input_fname) >= 0):
                return 1
        return 0
    
    # returns 1 if all files in input_list are found in index file, 0 otherwise
    def files_processed(self, input_list, date):
        ret = 0

        proc_list = self.readlines(date)
        for fstr in proc_list:
            ret = 0
            for f in input_list:
                if (string.find(fstr, f) >= 0):
                    ret = 1
                    break
            if (ret == 0):
                break    

        return (ret)

    def get_base_dir(self):
	"""Return the current base directory."""
        return self.base_dir

    def get_date_string(self, date):
        """Return date string corresponding to date."""
	date_tuple = time.gmtime(date)
        return time.strftime(self.format, date_tuple)
        
    def get_format(self):
	"""Return the format."""
        return self.format

    def get_hours(self):
	"""Return the index hours."""
        return self.hours

    def get_index_name(self):
	"""Return the index name."""
        return self.index_name

    def get_index_file(self):
	"""Return the current index file path."""
        return self.curr_index_file

    def get_index_date(self):
	"""Return the current index file date."""
        return self.date_string
    
    def get_wait(self):
	"""Return the wait time."""
        return self.wait


def test_write(name, num, date):
    index_file = Index_file(".", name, 24, "w", 0)
    for i in xrange(num):
        index_file.write("line %d" % i, date)

def test_read(index_file):
    while 1:
	(ds, line) = index_file.read()
	print ds, line
        if ds == "" or line == "":
            break

def test_readlines(index_file):
    lines = index_file.readlines(time.time())
    print lines
    
if __name__ == '__main__':
    print "testing index file"
    index_file = Index_file(".", "testindex", 24, "r", 300)
    curr_time = time.time()
    times = [curr_time, curr_time - 24 * 3600, curr_time - 2 * 24 * 3600]
    test_write("testindex", 50, times[0])
    test_write("testindex", 50, times[1])
    test_write("testindex", 50, times[2])
    test_read(index_file)
    test_readlines(index_file)
    rev,dates = index_file.readlines_num(times, 101, 1)
    print 'rev'
    print rev
    print 'dates'
    print dates
    
