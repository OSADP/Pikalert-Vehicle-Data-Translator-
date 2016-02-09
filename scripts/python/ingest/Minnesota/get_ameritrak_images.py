#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_ameritrak_images.py,v $                                           #
#       Version: $Revision: 1.2 $  Dated: $Date: 2014-02-13 22:46:43 $           #
#                                                                                #
# ============================================================================== #

import urllib2
import base64
import os
import sys
from optparse import OptionParser

from HTMLParser import HTMLParser

from HTMLParser import HTMLParser
import time
import calendar
import log_msg

# create a subclass and override the handler methods
class MyHTMLParser(HTMLParser):

    
    def handle_starttag(self, tag, attrs):
        self.last_attributes = attrs
        
    def handle_endtag(self, tag):
        pass

    
    def handle_data(self, data):

        out = base64.b64decode(data)
        if len(out) > 0:
            if hasattr(self, "last_attributes"):
                if len(self.last_attributes) > 0:
                    attr_dict = dict(self.last_attributes)
                    if "name" in attr_dict:
                        file_name = attr_dict["name"].replace("$", "_")
                        fp_out = open(file_name, "wb")
                        fp_out.write(out)
                        fp_out.close()


# instantiate the parser and fed it some HTML


def main():

    usage_str = "%prog begin_yyyymmddhhmm end_yyyymmddhhmm"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-n", "--nothing", dest="nothing", action="store_true", help="nothing at all")
    parser.add_option("-c", "--config_file", dest="config", help="read specified configuration file")
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) != 2:
        parser.print_help()
        sys.exit(2)

    begin_yyyymmddhhmm = args[0]
    end_yyyymmddhhmm = args[1]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting()
    
    begin_time = calendar.timegm(time.strptime(begin_yyyymmddhhmm, "%Y%m%d%H%M"))
    end_time = calendar.timegm(time.strptime(end_yyyymmddhhmm, "%Y%m%d%H%M"))
    for time_value in range(begin_time, end_time, 60):
        time_string = time.strftime("%Y%m%d%H%M", time.gmtime(time_value))
        #response = urllib2.urlopen('http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s&Length=1' % time_string)
        #print 'http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s&Length=1' % time_string
        #http_cmd = 'http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s&Length=1' % time_string
        response = urllib2.urlopen('http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s UTC&Length=1' % time_string)
        print 'http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s UTC&Length=1' % time_string
        http_cmd = 'http://mndotcam.ameritrak.biz/getATImageFiles.php?Agency=NCAR&BeginTime=%s UTC&Length=1' % time_string
        logg.write_info("Executing: %s" % (http_cmd))
        html = response.read()
        parser = MyHTMLParser()
        parser.feed(html)
        time.sleep(1)

    logg.write_ending()
    
if __name__ == "__main__":

   main()
