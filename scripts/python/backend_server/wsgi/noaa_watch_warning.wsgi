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
import dataset
import districts
import data_time_index
import log_msg

def send_response(output, start_response):
    response_headers = [("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

json_response = """{
    "Begin Time": "201510131854", 
    "End Time": "201510132000", 
    "Weather Watch/Warning Text": "http://www.weather.gov/regsci/gis/text/MFLSMW.1510131854", 
    "Type": "SMW", 
    "coordinates": [
        {
            "lat": 25.45, 
            "lon": -80.19
        }, 
        {
            "lat": 25.51, 
            "lon": -80.18
        }, 
        {
            "lat": 25.5, 
            "lon": -80.19
        }, 
        {
            "lat": 25.45, 
            "lon": -80.2
        }, 
        {
            "lat": 25.49, 
            "lon": -80.34
        }, 
        {
            "lat": 25.57, 
            "lon": -80.3
        }, 
        {
            "lat": 25.61, 
            "lon": -80.31
        }, 
        {
            "lat": 25.64, 
            "lon": -80.3
        }, 
        {
            "lat": 25.63, 
            "lon": -80.29
        }, 
        {
            "lat": 25.64, 
            "lon": -80.29
        }, 
        {
            "lat": 25.64, 
            "lon": -80.27
        }, 
        {
            "lat": 25.68, 
            "lon": -80.26
        }, 
        {
            "lat": 25.69, 
            "lon": -80.17
        }, 
        {
            "lat": 25.69, 
            "lon": -80.15
        }, 
        {
            "lat": 25.7, 
            "lon": -80.11
        }, 
        {
            "lat": 25.43, 
            "lon": -80.13
        }, 
        {
            "lat": 25.45, 
            "lon": -80.19
        }
    ]
}{
    "Begin Time": "201510131854", 
    "End Time": "201510132000", 
    "Weather Watch/Warning Text": "http://www.weather.gov/regsci/gis/text/MFLSMW.1510131854", 
    "Type": "SMW", 
    "coordinates": [
        {
            "lat": 25.45, 
            "lon": -80.19
        }, 
        {
            "lat": 25.51, 
            "lon": -80.18
        }, 
        {
            "lat": 25.5, 
            "lon": -80.19
        }, 
        {
            "lat": 25.45, 
            "lon": -80.2
        }, 
        {
            "lat": 25.49, 
            "lon": -80.34
        }, 
        {
            "lat": 25.57, 
            "lon": -80.3
        }, 
        {
            "lat": 25.61, 
            "lon": -80.31
        }, 
        {
            "lat": 25.64, 
            "lon": -80.3
        }, 
        {
            "lat": 25.63, 
            "lon": -80.29
        }, 
        {
            "lat": 25.64, 
            "lon": -80.29
        }, 
        {
            "lat": 25.64, 
            "lon": -80.27
        }, 
        {
            "lat": 25.68, 
            "lon": -80.26
        }, 
        {
            "lat": 25.69, 
            "lon": -80.17
        }, 
        {
            "lat": 25.69, 
            "lon": -80.15
        }, 
        {
            "lat": 25.7, 
            "lon": -80.11
        }, 
        {
            "lat": 25.43, 
            "lon": -80.13
        }, 
        {
            "lat": 25.45, 
            "lon": -80.19
        }
    ]
}"""

def application(environ, start_response):

    noaa_watch_warning_log = os.path.join(sys_path.log_dir, sys_path.noaa_watch_warning_wsgi_log)
    logg = log_msg.LogMessage(noaa_watch_warning_log, "pyl")
    logg.write_starting(sys_path.noaa_watch_warning_wsgi_log)

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)

    # get state
    state = form.getfirst("state", "")
    logg.write_info("state inputted: %s" % state)

    if state == "" or not state in sys_path.State_dictionary:
        logg.write_error("state not provided or not in State_dictionary")
        logg.write_ending(1, sys_path.noaa_watch_warning_wsgi_log)
        return send_response("must provide a state name such as colorado, michigan, minnesota, nevada, etc.", start_response)

    cf = sys_path.State_dictionary[state]

    # get date
    input_date_string = form.getfirst("date_str", "")

    # look up noaa watch warning directory for state
    json_response = '{}'
    if state in sys_path.watch_warning_dict:
       # get appropriate watch warning file for state
       if input_date_string == "":
           (file_name, yyyymmddhhmm) = data_time_index.get_latest_file(sys_path.watch_warning_dict[state])
       else:
           yyyymmddhhmm = "%s%s" % (input_date_string[0:8], input_date_string[9:13])
    
       file_path = os.path.join(sys_path.watch_warning_dict[state], yyyymmddhhmm[0:8], file_name)
       if os.path.exists(file_path):
           logg.write_info("processing file: %s" % file_path)
    	   json_response = open(file_path, 'r').read()
	
    logg.write_ending(0, sys_path.noaa_watch_warning_wsgi_log)
    return send_response(json_response, start_response)
