
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import sys,os
import json
import cgi
import string
import time
import datetime
import sys_path
import maw_alert_test

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

def send_response(output, start_response):
    response_headers = [("Cache-Control","no-cache, no-store"),
                        ("Pragma","no-cache"),
                        ("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):
    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    lat = float( form.getfirst("lat", "0.0") )
    lon = float( form.getfirst("lon", "0.0") )
    state = form.getfirst("state", None)

    if state == None or not state in set(["michigan", "minnesota", "nevada"]):
        error_dict = {"error_string" : "must provide a state name such as michigan, minnesota, nevada, etc."}
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)

    if lat == 0 or lon == 0:
        error_dict = {"error_string" : "either lat or lon is 0"}
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)

    prev_sequence_no = int( form.getfirst("prev_sequence_no", "-999") )
    session_id = form.getfirst("session_id", "UNKNOWN")

    output_dict = {}
    alert_dict = {}
    output = []

    curr_time = datetime.datetime.now()
    curr_time_str = curr_time.strftime('%Y%m%d%H%M%S')

    state_alert_dict = maw_alert_test.get_state_alert_dict()
    result_dict = maw_alert_test.get_alert(state_alert_dict, state, lat, lon)

    sequence_no = prev_sequence_no + 1
    alert_dict["sequence_no"] = str(sequence_no)
    alert_dict["session_id"] = session_id
    alert_dict["alert_gen_time"] = curr_time_str
    alert_dict["alert_request_time"] = curr_time_str
    alert_dict["alert_code_precip"] = result_dict["alert_code_precip"]
    alert_dict["alert_code_pavement"] = result_dict["alert_code_pavement"]
    alert_dict["alert_code_visibility"] = result_dict["alert_code_visibility"]
    alert_dict["alert_action_code"] = result_dict["alert_action_code"]

    output.append(alert_dict)

    output_dict["current_alert"] = output
    output_dict["data_time"] = curr_time_str

    json_response = json.dumps(alert_dict)
    return send_response(json_response, start_response)
