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

"""Tools for reading and writing a file containing information as to whether data files have been processed or not"""


import os
import sys
import time

PROC_FAILURE = "failure"
PROC_ATTEMPT = "attempt"
PROC_SUCCESS = "success"
PROC_NONE = "not_processed"

def write_dict(pdict, output_file):
    """write_dict -- write dictionary consisting of pairs of strings to output file sorting keys"""

    fp_out = open(output_file, "w")
    for key in sorted(pdict.keys()):
        fp_out.write("%s %s %f\n" % (key, pdict[key][0], float(pdict[key][1])))

    fp_out.close()

def write_processed_file(output_file, file_name, status_string):
    """write_processed_file -- appends processed file_name plus process status_string to output_file. Output file is written atomically using mv."""

    try:
        # Try to read lines into dictionary
        pdict = get_processed_files(output_file)
        pdict[file_name] = status_string
        
        # Open output file with prepended period
        basename = os.path.basename(output_file)
        dirname = os.path.dirname(output_file)
        dot_out_file = os.path.join(dirname, ".%s" % basename)

        write_dict(pdict, dot_out_file)
        os.rename(dot_out_file, output_file)
        return 0
    
    except OSError:
        return -1
    

def get_processed_dict(input_file):
    """This function tacitly assumes that the input files consists of lines each having the same number of fields. This can be guaranteed if the file is placed atomically in its destination directory using mv"""
    
    processed_dict = {}

    if os.path.exists(input_file):
        fp = open(input_file, "r")
        processed = fp.readlines()
        fp.close()

        for line in processed:
            split = line.split()
            processed_dict[split[0]] = (split[1], split[2])
       
    return processed_dict

def get_last_processed(input_file):
    processed_dict = get_processed_dict(input_file)
    keys = processed_dict.keys()
    if len(keys) > 0:
        sorted_keys = sorted(keys)
        return sorted_keys[-1]
    return ""

    
    
def is_processed(file_name, pdict):
    """Return string value for file_name in pdict"""
    if pdict.has_key(file_name):
        return pdict[file_name]
    else:
        return ""
    
class ProcessedFile(object):

    def __init__(self, base_dir, base_file_name):

        self.base_dir = base_dir
        self.base_file_name = base_file_name
        self.dates = {}

    def process_file_name(self, date):
        """Return the processed file name for a particular date"""
        return "%s.%s" % (self.base_file_name, date)
    
    def process_file_path(self, date):
        """Return the processed file name for a particular date"""
        process_file = self.process_file_name(date)
        process_file_path = os.path.join(self.base_dir, process_file)
        return process_file_path
    
    def write_processed(self, date, file_name, status_string):
        """Update dictionary in cache with file_name. Write file_name and status_string in processed file for given date."""

        process_file = self.process_file_name(date)
        process_file_path = os.path.join(self.base_dir, process_file)
        
        # If date is not in cache, fetch information from process_file_path
        if not self.dates.has_key(date):
            self.dates[date] = get_processed_dict(process_file_path)

        # Update cache
        self.dates[date][file_name] = (status_string, time.time())

        # Create temporary output file name
        dot_path = os.path.join(self.base_dir, ".%s" % process_file)

        write_dict(self.dates[date], dot_path)
        os.rename(dot_path, process_file_path)
        
    def process_status(self, file_name, date):

        process_file = self.process_file_name(date)
        process_file_path = os.path.join(self.base_dir, process_file)

        # If date is not in cache, fetch information from process_file_path
        if not self.dates.has_key(date):
            self.dates[date] = get_processed_dict(process_file_path)
            
        if self.dates[date].has_key(file_name):
            return self.dates[date][file_name]
         
        return (PROC_NONE, 0)
        
def test():

    date = "20070130"
    file_name = "file1"
    status_string = PROC_ATTEMPT
    proc_file = ProcessedFile(".", "processed_test")
    print "status for %s, %s" % (file_name, date), proc_file.process_status(file_name, date)
    proc_file.write_processed(date, file_name, status_string)
    print "status for %s, %s" % (file_name, date), proc_file.process_status(file_name, date)
    status_string = "success"
    proc_file.write_processed(date, file_name, status_string)
    print "status for %s, %s" % (file_name, date), proc_file.process_status(file_name, date)
    
if __name__ == "__main__":

    test()
