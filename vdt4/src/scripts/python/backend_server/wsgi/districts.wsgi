
import sys
import os
import json
import cgi
import sites
import time
import sys_path
import districts

## USE SAME STRUCTURE IN ALERTS FOR DISTRICTS


def send_response(output, start_response):
    response_headers = [("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):
    output = ""
    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    cf = sys_path.state_dictionary["minnesota"]
    district_list = cf.districts
    state = cf.state
    district_dict = {}
    district_dict["districts"] = district_list
    district_dict["state"] = state
    output = json.dumps(district_dict)
    return send_response(output, start_response)

