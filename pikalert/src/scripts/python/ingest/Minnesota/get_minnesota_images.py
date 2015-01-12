#!/usr/bin/env python

""""""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_minnesota_images.py,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2013/12/23 23:12:23 $           #
#                                                                                #
# ============================================================================== #

import log_msg
import os
import sys
from optparse import OptionParser
import urllib2
import shutil
import processed_file

error_ret_flag = 0

def get_minnesota_images(out_dir, logg):

    processed_fname_base = "Processed" 

    ftp_dir_path = "ftp://fhwa:1fhwa-vdt@rwis.dot.state.mn.us/images"

    response = urllib2.urlopen(ftp_dir_path)
    #print "The URL is: ", response.geturl()

    for line in response:
        line_rstrip = line.rstrip()
        file_name_split = line_rstrip.split()
        file_name = file_name_split[-1]
        #print "file_name: ", file_name

        
        date_split = file_name.split("-")
        #print (date_split[-1][-4:] == ".jpg")
        #print (date_split[0] == 'Vid')
        if (date_split[-1][-4:] == ".jpg") and (date_split[0] == 'Vid'):
            date = date_split[4] + date_split[5] + date_split[6]
            #print date_split
            #print "file_name: ", file_name

            dated_out_dir = os.path.join(out_dir, date)
            if not os.path.exists(dated_out_dir):
                cmd = "mkdir %s" % (dated_out_dir)
                ret = os.system(cmd)
                if ret != 0:
                    logg.write_error("Unable to create directory %s" % (dated_out_dir))
                    logg.write_ending(1, "get_minnesota_images.py")

            #print date_split

            proc_file = processed_file.ProcessedFile(out_dir, processed_fname_base)
            proc_stat_obj = proc_file.process_status(file_name, date)
            #print proc_stat_obj, file_name, date

            if proc_stat_obj != "" and proc_stat_obj[0] == processed_file.PROC_SUCCESS:
                print "processed: ", file_name
                continue
            #print file_name, date

            full_output_path = os.path.join(dated_out_dir, file_name)
            local_file = open(full_output_path, "wb")
            full_input_path = os.path.join(ftp_dir_path, file_name)
            #print "full_input_path: ", full_input_path
            
            
            ftpfile = urllib2.urlopen(full_input_path)
            shutil.copyfileobj(ftpfile, local_file)
                        
            if os.path.exists(full_output_path):
                proc_file.write_processed(date, file_name, processed_file.PROC_SUCCESS)
                logg.write_info("Successfully processed: %s" % (file_name))
            else:
                error_ret_flag = 1
                logg.write_error("Problem processing: %s" % (file_name))
    response.close()    
        
def main():

    usage_str = "%prog out_dir"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        sys.exit(2)

    out_dir = args[0]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    logg.write_starting("get_minnesota_images.py")
    
    get_minnesota_images(out_dir, logg)

    logg.write_ending(error_ret_flag, "get_minnesota_images.py")

if __name__ == "__main__":

   main()
