# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import sys
import backend_sys_path as sys_path
import os
import json
import cgi
import time
import sites
import alerts
import districts
import district_alerts
import data_time_index
import log_msg

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

def send_response(output, start_response):
    response_headers = [("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):

    district_alerts_log = os.path.join(sys_path.log_dir, sys_path.district_alerts_log)
    logg = log_msg.LogMessage(district_alerts_log, "pyl")
    logg.write_starting(sys_path.district_alerts_log)

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    state = form.getfirst("state", None)
    if state == None or not state in sys_path.State_dictionary:
        logg.write_error("state not provided or not in State_dictionary")
        logg.write_ending(1, "plots.wsgi")
        return send_response("must provide a state name such as michigan, minnesota, nevada, etc.", start_response)

    cf = sys_path.State_dictionary[state]
    input_date_string = form.getfirst("date_str", "")
    
    if input_date_string == "":
	(file_name, yyyymmddhhmm) = data_time_index.get_latest_file(cf.product_dictionary["district_alerts"])
    else:
        yyyymmddhhmm = "%s%s" % (input_date_string[0:8], input_date_string[9:13])
    
    output_path = ""
    logg.write_info("processing state, time: %s, %s" % (state, yyyymmddhhmm))
    if yyyymmddhhmm != "":
        output_path = district_alerts.get_district_alerts(cf, yyyymmddhhmm)
    if output_path != "":
        json_response = open(output_path,'r').read()
    else:
        json_response = '{}'

    logg.write_info("utilized output path: %s" % output_path)
    logg.write_ending(0, sys_path.district_alerts_log)
    return send_response(json_response, start_response)
