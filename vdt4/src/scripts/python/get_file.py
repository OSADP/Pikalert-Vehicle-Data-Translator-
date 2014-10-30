#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import index_file
import index_file_def
import os
import string
import tim

FIRST = 0
LAST = 1


# Get file name list from a list of index file lines 
def get_files(line_list):
    flist = []
    for line in line_list:
        l = string.split(line)
        flist.append(l[0])
    return(flist)


# Generate a time list beginning with start_time and ending with end_time using
# intervals spaced delta secs apart
def time_list(start_time, end_time, delta):
    if end_time < start_time:
        return []
    elif end_time == start_time:
        return [start_time]
    
    tval = start_time
    l = []
    while tval < end_time:
        l.append(tval)
        tval = tval + delta

    l.append(end_time)
    return l



# We need to modify this to take num_input_files into account
def get_latest_file_list_orig(input_list, time_val, num_input_files):
    """
    The input list consists of index files.
    """

    date_string = time.strftime("%Y%m%d", time.gmtime(time_val))
    prev_date_string = time.strftime("%Y%m%d", time.gmtime(time_val - 86400))

    out = []
    for rind in input_list:
        # print "rind: dir", rind.get_base_dir(), rind.get_index_date()

        lines = rind.readlines(time_val)
        final_date_string = date_string
        if lines == []:
            if num_input_files > 0:
                lines = rind.readlines(time_val - 86400)
                final_date_string = prev_date_string

        if lines != []:
            lines = lines[-1]
            str = string.split(lines)
            out.append("%s/%s/%s" % (rind.get_base_dir(), final_date_string, str[0]))
        else:
            out.append("None")

    # print out
    
    return out


def get_latest_file_list_orig1(input_list, start_time, num_files):
    """
    Return a list of file names, trying to get one from each index file in input_list.
    The starting time is start_time and the number of days to investigate is num_days.
    """

    out = []
    for rind in input_list:
        # Create time_list
        time_list = time_list(start_time, rind.get_hours() * 3600, num_files)

        # print "rind: dir", rind.get_base_dir(), rind.get_index_date()

        line_list, index_date_list = rind.readlines_list_rev(time_list, 1)
        flist = get_files(line_list)
        if flist != []:
            out.append("%s/%s/%s" % (rind.get_base_dir(), index_date_list[0], flist[0]))
        else:
            out.append("None")

    print out
    
    return out


def get_latest_file_orig3(ifile, start_time, end_time):
    """
    Search for the latest file given starting and ending times. The 
    """

    # Create time_list
    delta = ifile.get_hours() * 3600
    time_list = time_list(start_time, end_time, ifile.get_hours() * 3600)
    time_list.reverse()

    # print "ifile: dir", ifile.get_base_dir(), ifile.get_index_date()

    line_list, index_date_list = ifile.readlines_num(time_list, 1, 1)
    flist = get_files(line_list)
    if flist != []:
        out = "%s/%s/%s" % (ifile.get_base_dir(), index_date_list[0], flist[0])
    else:
        out = ""

    return out


def get_first_file(ifile, start_time, end_time):
    f,d = get_file(ifile, start_time, end_time, FIRST, 1)
    if f == []:
        return ""
    else:
        return f[0]

def get_last_file(ifile, start_time, end_time):
    f,d = get_file(ifile, start_time, end_time, LAST, 1)
    if f == []:
        return ""
    else:
        return f[0]

def get_last_filetime(ifile, start_time, end_time):
    f,d = get_file(ifile, start_time, end_time, LAST, 1)
    if d == []:
        return ""
    else:
        return d[0]

def get_last_pres_day_file(ifile, start_time, end_time):
    f,d = get_present_day_file(ifile, start_time, end_time, LAST, 1)
    if f == []:
        return ""
    else:
        return f[0]
    
def get_hour_file(ifile, start_time, pos):
    """
    Map start_time to nearest hourly interval. Search for file in this interval.
    """
    start_hour = tim.truncate_hour(start_time)
    end_hour = start_hour + 3600 - 1
    return(get_file(ifile, start_hour, end_hour, pos, 1))


def get_file(ifile, start_time, end_time, pos, num):
    """
    Search for num files given a starting and ending time value. If pos == FIRST, file will be first
    available between these two values. If pos == LAST, file will be last available between these
    values.
    """

    file_list = []
    date_list = []
    
    if (num <= 0):
        return file_list, date_list

    # Starting with start_time, open consecutive index files searching for a file
    # having a time between start_time and end_time. Stop when a file is found
    # or the index file corresponding to end_time has been searched unsuccessfully.

    # Create index_file time list 
    tlist = time_list(start_time, end_time, ifile.get_hours() * 3600)
    if pos == LAST:
        tlist.reverse()

    ct = 0

    # Check that the last time in tlist corresponds to a different index file hour than the
    # previous one.
    tlist_len = len(tlist)
    if tlist_len > 0:
        if ifile.get_date_string(tlist[-1]) == ifile.get_date_string(tlist[-2]):
            tlist = tlist[:-1]

    # Check that the second time in tlist corresponds to a different index file hour than
    # the first one.
    if tlist_len > 2:
        if ifile.get_date_string(tlist[0]) == ifile.get_date_string(tlist[1]):
            tlist = tlist[1:]
    
    # print 'procd tlist ', tlist
    for date in tlist:
        lines = ifile.readlines(date)
        if pos == LAST:
            lines.reverse()
        #print 'lines: '
        #print lines
        for line in lines:
            sp_line = string.split(line)
            tval = int(sp_line[index_file_def.TIME_FIELD])
            #print "field :", tval
            if start_time <= tval and tval <= end_time:
                #print "acc line: ", line
                ipath = index_file.index_file_path(ifile.get_base_dir(), ifile.get_index_name(), ifile.get_date_string(date))
                file_list.append("%s/%s/%s" % (ifile.get_base_dir(), ifile.get_date_string(date), sp_line[index_file_def.FILE_FIELD]))
                date_list.append(sp_line[index_file_def.TIME_FIELD])
                ct = ct + 1
                if ct == num:
                    return file_list, date_list

    return file_list, date_list

def get_present_day_file(ifile, start_time, curr_time, pos, num):
    """
    Get files only from present day's index file. Present day is determined by
    the input parameter curr_time. The parameter start_time is not used.
    """
    time_00 = tim.truncate_day(curr_time)
    return get_file(ifile, time_00, curr_time, pos, num)
    


def get_file_list(dir, pattern, suffix, sort_order):
    """
    Makes a sorted list of files with fully-qualified path which have
    pattern in the filename, and end with suffix. Sorted in the 
    order specified. NOTE: does NOT use index files to make list.
    """
    dir_list = os.listdir(dir)

    filtered_list = []
    for file in dir_list:
        if string.find(file, pattern) >= 0 and string.find(file, suffix, -len(suffix)) >= 0:
            filtered_list.append(os.path.join(dir, file))

    filtered_list.sort()
    if sort_order == FIRST:
        filtered_list.reverse()

    return filtered_list
 


if __name__ == "__main__":
    import time
    import sys
    
    if len(sys.argv) < 4:
        print "usage: %s index_file_base_dir start_time_offset end_time_offset number_files"
        print "The start_time_offset and end_time_offset variables are in hours relative"
        print "to the current time. They should be negative to go back in time."
        sys.exit(2)

    
    ifile = index_file.Index_file(sys.argv[1], "index", 24, "r", 0)
    # print ifile
    start_time_off = int(sys.argv[2])
    end_time_off = int(sys.argv[3])
    num_files = int(sys.argv[4])

    date = int(time.time())

    # print 'date ', date
    # print 'num_files ', num_files
    # f,d = get_present_day_file(ifile, date + 3600 * start_time_off, date + 3600 * end_time_off, FIRST, num_files)
    f,d = get_file(ifile, date + 3600 * start_time_off, date + 3600 * end_time_off, FIRST, num_files)
    print f
    print d
