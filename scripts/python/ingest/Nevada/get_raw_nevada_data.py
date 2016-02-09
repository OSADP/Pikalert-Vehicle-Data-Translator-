#!/usr/bin/env python

"""Gets Nevada data from URL"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: get_raw_nevada_data.py,v $                                           #
#       Version: $Revision: 1.3 $  Dated: $Date: 2013-10-09 21:07:34 $           #
#                                                                                #
# ============================================================================== #


#/d2/vii/data/raw/nevada
url = "http://134.197.27.248:8001/NCAR/Transmitted/"

import log_msg
import os
import sys
import urllib2
from optparse import OptionParser

# The file is not really empty, but has only html in it that says the files was not found
EMPTY_FILE_SIZE = 229

def main():

    usage_str = "%prog config_file date \n\tNote: Date must be in YYYYMMDD format"
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")

    (options, args) = parser.parse_args()

    if len(args) < 2:
        parser.print_help()
        sys.exit(2)

    config_file = args[0]
    date = args[1]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")
        
    if config_file[-3:] == ".py":
        config_file = config_file[:-3]

    cf = __import__(config_file, globals(), locals(), [])

    logg.set_suffix("%s" % cf.paths.LOG_PY_SUFFIX)    
    logg.write_starting("get_raw_nevada_data.py")

    if (len(date) != 8):
        logg.write_info("The date %s must be in the format YYYYMMDD." % date)

    # Make dated output directory, if necessary
    out_day_dir = os.path.join(cf.paths.NEVADA_RAW_DIR, date)
    print out_day_dir
    if not (os.path.exists(out_day_dir)):
        cmd  = "mkdir -p %s" % out_day_dir
        logg.write_info(cmd)
        ret = os.system(cmd)
        if ret != 0 :
            logg.write_error("Problem making directory %s" % (out_day_dir))
            return ret

    dated_filename = "%s-%s-%s.txt" % (date[0:4], date[4:6], date[6:8])
    file_path = url + dated_filename
    
    tmp_out_filename = "/.nevada_" + date + ".csv"
    out_filename = "/nevada_" + date + ".csv"
    tmp_out_file_path = out_day_dir + tmp_out_filename
    out_file_path = out_day_dir + out_filename

    cmd = "curl %s > %s" % (file_path, tmp_out_file_path)
    logg.write_info(cmd)
    ret = os.system(cmd)
    if ret != 0:
        logg.write_error("Couldn't grab %s, cleaning up..." % file_path)
        cmd = "rm -f %s" % tmp_out_file_path
        logg.write_info(cmd)
        os.system(cmd)
        logg.write_ending(ret)
        return ret
    else:
        statinfo = os.stat(tmp_out_file_path)
        # 
        if (int(statinfo.st_size) > EMPTY_FILE_SIZE):
            cmd = "mv %s %s" % (tmp_out_file_path, out_file_path)
            logg.write_info(cmd)
            ret = os.system(cmd)
            if ret != 0 :
                logg.write_error("Problem moving %s to %s" % (tmp_out_file_path, out_file_path))
                return ret
    
    logg.write_ending(0, "get_raw_nevada_data.py")
    return 0

if __name__ == "__main__":

   main()
