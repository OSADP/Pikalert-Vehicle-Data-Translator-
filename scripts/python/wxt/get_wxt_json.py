#!/usr/bin/env python

"""Get all data from begin_date through end_date from weather telematics"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_wxt_json.py,v $                                           #
#       Version: $Revision: 1.4 $  Dated: $Date: 2014/10/06 20:48:47 $           #
#                                                                                #
# ============================================================================== #

import calendar
import json
import log_msg
import os
import string
import sys
from optparse import OptionParser
from subprocess import Popen, PIPE
import time
import tim
from StringIO import StringIO
from gzip import GzipFile
import urllib2

WXToken = "X3mbFssSR7dVeA9J55ii"
Bufsize = 16384
MIN_RETRIEVE = 20000
MAX_QUERY_PER_SECOND = 5
QUERY_LIMIT = 24 * 10000


def standard_time(yyyymmddhhmmss):
    """Return date in form yyyy-mm-ddTHH:MM:SSZ such as 2012-08-11T16:21:00Z"""
    
    s = yyyymmddhhmmss
    return "%s-%s-%sT%s:%s:%sZ" % (s[0:4], s[4:6], s[6:8], s[8:10], s[10:12], s[12:14])


def get_request(url, logg):
    """
    Function retrieves data at url.  It automatically attempts to get and deocde GZIP compressed data
    but falls back on standard text responses.  If an error is encountered trying to connect to the URL,
    None is returned.
    """
    
    content = None
    
    try:
        logg.write_info("Requesting: %s" % url)
        request = urllib2.Request(url)
        request.add_header("Accept-Encoding", "gzip")
        
        response = urllib2.urlopen(request)
        if response.getcode() != 200:
            logg.write_error("Response code %d for url: %s" % (response.getcode(), url))
            return content

        content = response.read()
        if response.info().get("Content-Encoding") == "gzip":
            read = StringIO(content)
            content = GzipFile(fileobj=read).read()
        
    except urllib2.URLError as ex:
        logg.write_error(ex)

    return content
        
    
def get_single_update(begin_yyyymmddhhmmss, end_yyyymmddhhmmss, offset, logg):

    begin_time = standard_time(begin_yyyymmddhhmmss)
    end_time = standard_time(end_yyyymmddhhmmss)

    if offset == "":
        url = "http://api.weathertelematics.com/filtered/%s?start_date=%s&end_date=%s" % (WXToken, begin_time, end_time)
    else:
        url = "http://api.weathertelematics.com/filtered/%s?offset=%s&end_date=%s" % (WXToken, offset, end_time)

    return get_request(url, logg)

def pprint_json_txt(txt):
    pprint_json(json.loads(txt))
    
def pprint_json(jsondict):
    print json.dumps(jsondict, indent=4, sort_keys=True)
    
def get_wxt_data(number, offset, begin_yyyymmddhhmmss, end_yyyymmddhhmmss, out_file_base, logg):
    """
    Get Weather Telematics data starting from begin_yyyymmddhhmmss and ending at end_yyyymmddhhmmss
    Write data to daily files using out_file_base.yyyymmdd.xml template for file names.

    Loop:

    . Use urllib2 to fetch data
    . Append data to appropriate daily file
    . Find record number in fetched data
    . Use record number to fetch next data set
    . Stop when reach final data period

    Wx telematics token: See WXToken above
    """

    time_delta = 1
    unix_end_time = calendar.timegm(time.strptime(end_yyyymmddhhmmss, "%Y%m%d%H%M%S"))
    
    begin_time = tim.datetogmt(begin_yyyymmddhhmmss)
    for ind in range(number):
        content = get_single_update(begin_yyyymmddhhmmss, end_yyyymmddhhmmss, offset, logg)
        try:
            diction = json.loads(content)
            #pprint_json(diction)
        except ValueError:
            logg.write_error("ValueError thrown parsing json")
            begin_time += 300
            offset = ""
            time.sleep(1)
            continue
        except TypeError as te:
            logg.write_error("%s. Returning from get_wxt_data" % te)
            return
        
        # store observations in hourly buckets based on observation time and not insertion time
        observation_sets = diction["observation_sets"]
        if len(observation_sets) > 1:
            insertion_time = observation_sets[-1]["insertion_time"]
            unix_time = calendar.timegm(time.strptime(insertion_time, "%Y-%m-%dT%H:%M:%SZ"))
            begin_time = unix_time + time_delta
        else:
            begin_time += 300
            
        if diction["errors"] != None:
            logg.write_error("errors in diction: %s" % diction["errors"])
            continue
        try:
            out_file = "%s.%s.%s.json" % (out_file_base, begin_yyyymmddhhmmss[0:8], begin_yyyymmddhhmmss[8:])
            logg.write_debug("out_file: %s" % out_file)
            fp_out = file(out_file, "w")
            fp_out.write(content)
            fp_out.close()
            if ind % MAX_QUERY_PER_SECOND == 0:
                time.sleep(.2)
        except ValueError:
            logg.write_error("bad offset value %s" % offset)
            return

        if begin_time >= unix_end_time:
            logg.write_debug("begin_time is less than unix end_time so returning")
            return
        
        begin_yyyymmddhhmmss = time.strftime("%Y%m%d%H%M%S", time.gmtime(begin_time))

        
def main():

    usage_str = "%prog begin_yyyymmddhhmmss end_yyyymmddhhmmss out_file_base\nPurpose: Get weather telematics for specified time interval"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-o", "--offset", dest="offset", help="specify starting offset")
    parser.add_option("-n", "--number", dest="number", help="number of queries")

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(2)

    begin_yyyymmddhhmmss = args[0]
    end_yyyymmddhhmmss = args[1]
    out_file = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    offset = ""
    number = 1
    if options.offset:
        offset = options.offset
    if options.number:
        number = int(options.number)
        if number > QUERY_LIMIT:
            print "Error: exceeded query limit of %d\n" % QUERY_LIMIT

    logg.write_starting()
    get_wxt_data(number, offset, begin_yyyymmddhhmmss, end_yyyymmddhhmmss, out_file, logg)
    logg.write_ending()

if __name__ == "__main__":

   main()
