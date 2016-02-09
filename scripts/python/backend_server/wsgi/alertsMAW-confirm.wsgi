
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
import datetime

Product_dictionary = {"road_wx_dir" : "/d1/dicast/rctm/rdwx_fcst", "rec_treatment" : "/d1/dicast/rctm/merge_rec_tmt"}

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

sys.path.append("/home/dicast/akmdss_view")

def send_response(output, start_response):
    response_headers = [("Cache-Control","no-cache, no-store"),
                        ("Pragma","no-cache"),
                        ("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):
    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    prev_sequence_no = int( form.getfirst("prev_sequence_no", "-999") )
    session_id = form.getfirst("session_id", "UNKNOWN")
    output_dict = {}

    road_cond_alert_images = {}
    obs_alert_images = {}
    rdwx_alert_images = {}
    time_alert_images = {}
    curr_time = datetime.datetime.now()
    curr_time_str = curr_time.strftime('%Y%m%d%H%M%S')

    sequence_no = prev_sequence_no + 1
    output_dict["session_id"] = session_id
    output_dict["confirmed_sequence_no"] = str(sequence_no)
    output_dict["response"] = "OK"

    json_response = json.dumps(output_dict)
    return send_response(json_response, start_response)
