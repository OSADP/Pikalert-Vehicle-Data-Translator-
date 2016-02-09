
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import data_time_index
import sys
import os
import fcntl
import json
import cgi
import time
import datetime
import maw_alerts
import backend_sys_path as sys_path
import log_msg

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

# sys.path.append("/home/dicast/akmdss_view")

Session_dict = {}

def send_response(output, start_response):
    response_headers = [("Cache-Control","no-cache, no-store"),
                        ("Pragma","no-cache"),
                        ("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]


class CgiFields(object):
    """Extract cgi fields from request. Perform error checking and set self.json_error_string accordingly. Sample request:

    http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_alerts_adhoc&lat=43.019999999999996&lon=-72.2&state=michigan
    """
    
    def __init__(self, environ):
        self.form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
        self.error_string = ""

        # get latitude, longitude
        self.latitude = float(self.form.getfirst("lat", 0.0))
        self.longitude = float(self.form.getfirst("lon", 0.0))
        if self.latitude == 0.0 or self.longitude == 0.0:
            self.error_string = "0.0 latitude or longitude is not allowed"
            return

        # get bearing, speed and accuracy but do not fail if not available
        self.bearing = -9999
        self.speed = -9999
        self.accuracy = -9999
	try:
            self.bearing = float(self.form.getfirst("bearing", "-9999"))
            self.speed = float(self.form.getfirst("speed", "-9999"))
            self.accuracy = float(self.form.getfirst("accuracy", "-9999"))
        except ValueError:
            pass
                                  
        # get state
        self.state = self.form.getfirst("state", None)
        if self.state == None or not self.state in sys_path.State_dictionary:
            self.error_string = "must provide a state name such as michigan, minnesota, nevada, etc."
            return
        

def application(environ, start_response):

    global Session_dict
    
    phone_log_path = os.path.join(sys_path.log_dir, sys_path.adhoc_phone_log)
    logg = log_msg.LogMessage(phone_log_path, "pyl")
    logg.write_starting(sys_path.adhoc_phone_log)

    cgi_fields = CgiFields(environ)
    if cgi_fields.error_string != "":
        error_dict = {"error_string" : cgi_fields.error_string}
        print "%s" % (cgi_fields.error_string)
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)
        
    cf = sys_path.State_dictionary[cgi_fields.state]


    curr_time = datetime.datetime.now()
    expanded_time_string = curr_time.strftime("%Y%m%d%H%M%S.%f")
    short_time_string = expanded_time_string[0:14]

    (maw_json, maw_json_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, short_time_string, cf.maw_json_dir)
    logg.write_info("latest maw json file, short_time_string: %s %s" % (maw_json, short_time_string))

    # get maw alerts
    logg.write_info("state, latitude, longitude, bearing, radius, road_segments_file: %s %f %f %f %f %s" % (cgi_fields.state, cgi_fields.latitude, cgi_fields.longitude, cgi_fields.bearing, cf.radius, cf.road_segments_file))
    
    maw_alert_dict = maw_alerts.get_maw_alerts(cf, cgi_fields.latitude, cgi_fields.longitude, cf.radius, maw_json, cgi_fields.bearing if cgi_fields.bearing != -9999 else None, 45.0)
    maw_alert_dict["alert_time"] = short_time_string
    maw_alert_dict["alert_gen_time"] = short_time_string
    maw_alert_dict["alert_request_time"] = maw_json_time
    logg.write_info("alert_time %s, alert_gen_time %s, alert_request_time %s" % (maw_alert_dict["alert_time"], maw_alert_dict["alert_gen_time"], maw_alert_dict["alert_request_time"]))


    revised_precip_alert = maw_alerts.Precip_alerts[maw_alert_dict["alert_code_precip"]].replace(",", "-")
    revised_pavement_alert = maw_alerts.Pavement_alerts[maw_alert_dict["alert_code_pavement"]].replace(",", "-")
    revised_visibility_alert = maw_alerts.Visibility_alerts[maw_alert_dict["alert_code_visibility"]].replace(",", "-")
    revised_action = maw_alerts.Indicated_action[maw_alert_dict["alert_action_code"]].replace(",", "-")
    
    logg.write_info("creating json response with indicated action %s" % revised_action)

    json_response = json.dumps(maw_alert_dict)

    logg.write_ending(0, sys_path.adhoc_phone_log)

    return send_response(json_response, start_response)
