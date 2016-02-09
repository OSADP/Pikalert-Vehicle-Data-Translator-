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
import os
import json
import cgi
import string
import time
import backend_sys_path as sys_path
import data_time_index


def send_response(output, start_response):
    response_headers = [("Cache-Control","no-cache, no-store"),
                        ("Pragma","no-cache"),
                        ("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    state = form.getfirst("state", None)
    if state == None or not state in sys_path.State_dictionary:
        return send_response("must provide a state name such as michigan, minnesota, nevada, etc.", start_response)
    cf = sys_path.State_dictionary[state]
    data_time_index_obj = data_time_index.DataTimeIndex(cf.product_dictionary)

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    data_time_list = data_time_index_obj.get_latest_data_time_list()
    output = json.dumps(data_time_list)
    return send_response(output, start_response)
