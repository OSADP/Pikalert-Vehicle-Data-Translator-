#!/usr/bin/env python

"""Get all data from begin_date through end_date from weather telematics"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_wxt_data.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2014/09/30 16:28:09 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import string
import sys
from optparse import OptionParser
from subprocess import Popen, PIPE
import time

WXToken = "X3mbFssSR7dVeA9J55ii"
Bufsize = 16384
MIN_RETRIEVE = 20000
MAX_QUERY_PER_SECOND = 5
QUERY_LIMIT = 24 * 10000

# http://api.weathertelematics.com/filtered.xml?auth_token=Tb3aJthRYamRV7BNcYCE3&offset=130200&end_date=2012-11-14T13:21:05Z

def standard_time(yyyymmddhhmmss):
    """Return date in form yyyy-mm-ddTHH:MM:SSZ such as 2012-08-11T16:21:00Z"""
    
    s = yyyymmddhhmmss
    return "%s-%s-%sT%s:%s:%sZ" % (s[0:4], s[4:6], s[6:8], s[8:10], s[10:12], s[12:14])

def get_single_update(begin_yyyymmddhhmmss, end_yyyymmddhhmmss, offset):

    begin_time = standard_time(begin_yyyymmddhhmmss)
    end_time = standard_time(end_yyyymmddhhmmss)

    if offset == "":
        command = 'curl "http://api.weathertelematics.com/filtered/%s.xml?start_date=%s&end_date=%s" | gzip -d' % (WXToken, begin_time, end_time)
        #command = 'curl "http://api.weathertelematics.com/filtered/%s.xml?start_date=%s&end_date=%s"' % (WXToken, begin_time, end_time)
    else:
        command = 'curl "http://api.weathertelematics.com/filtered/%s.xml?offset=%s&end_date=%s" | gzip -d' % (WXToken, offset, end_time)
        #command = 'curl "http://api.weathertelematics.com/filtered/%s.xml?offset=%s&end_date=%s"' % (WXToken, offset, end_time)

    print "command: ", command
    pipe = Popen(command, shell=True, stdout=PIPE).stdout
    content = pipe.read()
    return content
    
def get_wxt_data(run_eval, number, offset, begin_yyyymmddhhmmss, end_yyyymmddhhmmss, out_file_base):
    """
    Get Weather Telematics data starting from begin_yyyymmddhhmmss and ending at end_yyyymmddhhmmss
    Write data to daily files using out_file_base.yyyymmdd.xml template for file names.

    Loop:

    . Use curl to fetch data
    . Append data to appropriate daily file
    . Find record number in fetched data
    . Use record number to fetch next data set
    . Stop when reach final data period

    Wx telematics token: See WXToken above
    """


    for ind in range(number):
        content = get_single_update(begin_yyyymmddhhmmss, end_yyyymmddhhmmss, offset)
        if run_eval:
            k = eval(content)
            output = k[0]
        else:
            output = content
            
        if len(output) < MIN_RETRIEVE:
            print "Info: query return size %d < minimum retrieval size %d" % (len(output), MIN_RETRIEVE)
            print output
            return
            
        pos = output.find("?offset")
        start_offset = pos + len("?offset") + 1
        offset_len = output[start_offset:].find("&")
        offset = output[start_offset:start_offset+offset_len]
        try:
            time_val = time.time()
            millisec = time_val-int(time_val)
            millisec_str = "%03d" % int(1000*millisec)
            time_str = time.strftime("%Y%m%d%H%M%S", time.gmtime(time_val))
            out_file = "%s.%s:%s.%s.xml" % (out_file_base, time_str[0:8], time_str[8:], millisec_str)
            print out_file
            fp_out = file(out_file, "w")
            fp_out.write(output)
            fp_out.close()
            if ind % MAX_QUERY_PER_SECOND == 0:
                time.sleep(.2)
            offset_value = int(offset)
        except ValueError:
            print "Error: bad offset value %s" % offset
            return
        
        #content = content.replace('\"', '')
        #trans_tab = string.maketrans("", "")
        #deletions = '[]"'
        #content = string.translate(content, trans_tab, deletions)
    
    # print content
    # xml request
    # http://api.weathertelematics.com/filtered/Tb3aJthRYamRV7BNcYCF.xml?start_date=2012-11-14T16:21:00Z&end_date=2012-11-14T16:22:00Z
    # http://api.weathertelematics.com/filtered.xml?auth_token=Tb3aJthRYamRV7BNcYCE3&offset=130200&end_date=2012-11-14T13:21:05Z
    #command = 'curl "http://api.weathertelematics.com/filtered/%s" | gzip -d > api_results.txt' % WXToken

def main():

    usage_str = "%prog begin_yyyymmddhhmmss end_yyyymmddhhmmss out_file_base\nPurpose: Get weather telematics for specified time interval"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-e", "--eval", dest="evaluate", action="store_true", help="use eval (useful when gunzipping is required)")
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
    get_wxt_data(options.evaluate, number, offset, begin_yyyymmddhhmmss, end_yyyymmddhhmmss, out_file)
    logg.write_ending()

if __name__ == "__main__":

   main()
