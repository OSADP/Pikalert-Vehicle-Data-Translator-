
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

def application(environ, start_response):

    latest_vehicles_log = os.path.join(sys_path.log_dir, sys_path.latest_vehicles_log)
    logg = log_msg.LogMessage(latest_vehicles_log, "pyl")
    logg.write_starting(sys_path.latest_vehicles_log)

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    state = form.getfirst("state", None)
    if state == None or not state in sys_path.State_dictionary:
        logg.write_error("state not provided or not in State_dictionary")
        logg.write_ending(1, "latest_vehicles.wsgi")
        return send_response("must provide a state name such as michigan, minnesota, nevada, etc.", start_response)

    cf = sys_path.State_dictionary[state]
    input_date_string = form.getfirst("date_str", "")
    print "input_date_string: ", input_date_string
    if input_date_string == "":
	(file_name, yyyymmddhhmm) = data_time_index.get_latest_file(cf.product_dictionary["latest_vehicles"])
    else:
        yyyymmddhhmm = "%s%s" % (input_date_string[0:8], input_date_string[9:13])
    
    
    output_path = ""
    logg.write_info("processing state, time: %s, %s" % (state, yyyymmddhhmm))
    if yyyymmddhhmm != "":
        output_path = dataset.get_dataset(cf.latest_vehicles_dir, sys_path.latest_vehicles_base_name, yyyymmddhhmm, sys_path.latest_vehicles_suffix)
    logg.write_info("utilizing output file %s" % output_path)

    if output_path != "":
        json_response = open(output_path, 'r').read()
    else:
        json_response = '{}'
	
    logg.write_ending(0, sys_path.latest_vehicles_log)
    return send_response(json_response, start_response)
