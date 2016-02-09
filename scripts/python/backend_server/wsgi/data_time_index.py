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

"""Get latest data time based on utilizing python generated index files"""

import os
import tim
import time
import bisect

# NOTE: Currently this function does not check the success status from Processed files
def get_latest_file_using_time(time_delta, date_string, directory):
    """Gets the latest file prior to and within time_delta seconds of the specified time in date_string of form yyyymmddhhmm. Note that data_time is in unix seconds"""

    time_value = tim.datetogmt(date_string)
    yyyymmdd = date_string[0:8]

    dirlist = os.listdir(directory)
    index_dict = {}
    for name in dirlist:
        if name[0:5] == 'index' or name[0:9] == 'Processed' and name[-1] != '~':
            spl = name.split('.')
            date = spl[1]
            index_dict[date] = name

    sorted_dates = sorted(index_dict.keys())
    ind = bisect.bisect_left(sorted_dates, yyyymmdd)
    index_try_list = []

    if ind < len(sorted_dates):
        if yyyymmdd == sorted_dates[ind]:
            if ind > 0:
                index_try_list = [ind, ind-1]
            else:
                index_try_list = [ind]
        else:
            previous_date = time.strftime("%Y%m%d", time.gmtime(time_value - 86400))
            if previous_date == sorted_dates[ind]:
                index_try_list = [ind]
    elif ind == len(sorted_dates):
        previous_date = time.strftime("%Y%m%d", time.gmtime(time_value - 86400))
        if previous_date == sorted_dates[ind-1]:
            index_try_list = [ind-1]
        
    # look in potential index files for dates near date_string
    return_path = ""
    return_date = ""

    for ind in index_try_list:
        index_file = index_dict[sorted_dates[ind]]
        path = os.path.join(directory, index_file)
        date_list = []
        file_list = []

        # get date and file list in index file
        with open(path) as in_fp:
            lines = in_fp.readlines()
            for line in lines:
                spl = line.split()
                if len(spl) >= 2:
                    file_components = spl[0].split('.')
                    if len(file_components) >= 3:
                        date_list.append(file_components[1] + file_components[2])
                        file_list.append(spl[0])


        if len(date_list) == 0:
            continue
        
        # look for date_string in date_list or nearest previous date
        ind = bisect.bisect_left(date_list, date_string)
        if ind < len(date_list):
            if date_string == date_list[ind]:
                return_path = os.path.join(directory, date_string[0:8], file_list[ind])
                return_date = date_list[ind]
                break
            else:
                if ind > 0:
                    # check time difference
                    file_time_value = tim.datetogmt(date_list[ind-1])
                    if time_value - file_time_value < time_delta:
                        return_path = os.path.join(directory, date_list[ind-1][0:8], file_list[ind-1])
                        return_date = date_list[ind-1]
                        break
        else:
            # ind == len(date_list)
            file_time_value = tim.datetogmt(date_list[ind-1])
            if time_value - file_time_value < time_delta:
                return_path = os.path.join(directory, date_list[ind-1][0:8], file_list[ind-1])
                return_date = date_list[ind-1]
                break

    return (return_path, return_date)

def get_latest_file(directory):
    """Gets the latest file name and data time for a directory using index files or processed files."""
    dirlist = os.listdir(directory)
    filter_list = []
    for name in dirlist:
        if name[0:5] == 'index' or name[0:9] == 'Processed':
            filter_list.append(name)
    sorted_filter_list = sorted(filter_list)
    sorted_filter_list.reverse()

    yyyymmddhhmm = ""
    filter_list = sorted_filter_list
    if len(filter_list) > 0:
        path_list = []
        for fname in filter_list:
            path_list.append(os.path.join(directory, fname))
        (latest_file, latest_line) = get_last_entire_line(path_list)

        if latest_file != "":
            spl = latest_line.split()
            last_file = spl[0]
            file_spl = last_file.split('.')
            if len(file_spl) >= 3:
                yyyymmdd = file_spl[1]
                hhmm = file_spl[2]
                yyyymmddhhmm = yyyymmdd + hhmm
                return (last_file, yyyymmddhhmm)
        else:
            return ("", "")
    else:
       return ("", "")


def get_last_entire_line(path_list):
    """Find first file having at least one entire line. Return file path and entire line. Useful for finding last entry in an index file generated by index_file.py."""
    for path in path_list:
        lines = file(path).readlines()
        if len(lines) == 0:
            continue
        elif len(lines) == 1 and lines[0][-1] == '\n':
            return (path, lines[0])
        else:
            if lines[-1][-1] != '\n':
                return (path, lines[-2])
            else:
                return (path, lines[-1])
    return ("", "")
            
class DataTimeIndex(object):

    def __init__(self, product_dictionary):
        self.product_dictionary = product_dictionary
        self.directories = set(product_dictionary.values())
        
    def get_latest_data_time(self, product):
        """Get latest data time for a particular product"""
        if product in self.product_dictionary:
            (file_name, file_time) = get_latest_file(self.product_dictionary[product])
            return file_time
        else:
            return ""
        
    def get_latest_data_time_dict(self):
        """Get latest data time for a particular product in the form of a dictionary"""
        data_time_dict = {}
        for product in self.product_dictionary:
            (file_name, file_time) = get_latest_file(self.product_dictionary[product])
            data_time_dict[product] = file_time
        return data_time_dict    
        
    def get_latest_data_time_list(self):
        """Get latest data time for a particular product in the form of a list"""
        data_time_list = []
        for product in self.product_dictionary:
            (file_name, file_time) = get_latest_file(self.product_dictionary[product])
            data_time_list.append({"dir" : product, "latest_time" : file_time})
        return data_time_list
        
        
def main():

    print get_latest_file("/d2/vii/data/processed/co_noaa_watch_warning_json")
    """
    print get_latest_file_using_time(3600, "201311200025", "/d2/dicast/rctm/logicast/rdwx_fcst")
    print get_latest_file_using_time(3600, "2013112100", "/d2/dicast/rctm/logicast/rdwx_fcst")
    print get_latest_file_using_time(3600, "201311230001", "/d2/dicast/rctm/logicast/rdwx_fcst")

    import backend_sys_path
    product_dictionary = backend_sys_path.State_dictionary['minnesota'].product_dictionary
    print product_dictionary
    data_time_index = DataTimeIndex(product_dictionary)
    #print data_time_index.get_dict()
    print data_time_index.get_latest_data_time("district_alerts")
    #print data_time_index.get_latest_data_time("rec_treatment")
    #print data_time_index.get_latest_data_time_dict()
    print data_time_index.get_latest_data_time_list()
    """
    
if __name__ == "__main__":
    main()
