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
import phone_ids
import PhoneLog
import log_msg

Maw_global_data_file = "/tmp/maw_phone_app.json"
Maw_phone_app_error = "maw phone app error: "
Phone_ids = phone_ids.Phone_ids
Phone_id_set = set(Phone_ids)

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

    http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_alerts_dev&phone_id=5555215554&lat=43.019999999999996&lon=-72.2&session_id=1391719633&prev_sequence_no=496&state=michigan
    """
    
    def __init__(self, environ):
        self.form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
        self.error_string = ""

        # get phone id
        self.phone_id = self.form.getfirst("phone_id", "")
        if not self.phone_id in Phone_id_set:
            self.error_string = "phone %s not registered" % self.phone_id
            return

        # get latitude, longitude
        self.latitude = float(self.form.getfirst("lat", 0.0))
        self.longitude = float(self.form.getfirst("lon", 0.0))
        if self.latitude == 0.0 or self.longitude == 0.0:
            self.error_string = "0.0 latitude or longitude is not allowed for phone %s" % self.phone_id
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
            self.error_string = "must provide a state name such as michigan, minnesota, nevada, etc. for phone %s" % self.phone_id
            return

        # get session id
        try:
            session_id_string = self.form.getfirst("session_id", "-9999")
            self.session_id = int(session_id_string)
        except ValueError:
            self.error_string = "session id %s must be integer for phone %s" % (session_id_string, self.phone_id)
            return
        if self.session_id == -9999:
            self.error_string = "session id must not be the missing value -9999"
            return
            
        # get previous sequence number
        try:
            prev_sequence_no_string = self.form.getfirst("prev_sequence_no", "-9999")
            self.prev_sequence_no = int(prev_sequence_no_string)
        except ValueError:
            self.error_string = "previous sequence number %s must be integer for phone %s" % (prev_sequence_no_string, self.phone_id)
            return
        if self.prev_sequence_no == -9999:
            self.error_string = "previous sequence number must not be the missing value -9999"
            return

    def log_header(self):

        header_string = "phone id,state,latitude,longitude,bearing,session_id,phone_previous_sequence_number,"
        return header_string
        
    def log_message(self):

        log_string = "%s,%s,%f,%f,%f,%d,%d," % (self.phone_id, self.state, self.latitude, self.longitude, self.bearing, self.session_id, self.prev_sequence_no)
        return log_string
        

def application(environ, start_response):

    global Session_dict
    
    phone_log_path = os.path.join(sys_path.log_dir, sys_path.phone_log)
    logg = log_msg.LogMessage(phone_log_path, "pyl")
    logg.write_starting(sys_path.phone_log)

    # open maw global file
    logg.write_info("using maw global data file %s" % Maw_global_data_file)
    if os.path.exists(Maw_global_data_file):
        fd = os.open(Maw_global_data_file, os.O_RDWR|os.O_CREAT)

    	# Lock it
    	fcntl.lockf(fd, fcntl.LOCK_EX)

        # Now get a file object for the above file.
	fo = os.fdopen(fd, "r")

	try:
	    Session_dict = json.load(fo)
	except ValueError:
	    pass

	fcntl.lockf(fd, fcntl.LOCK_UN)
	fo.close()

    cgi_fields = CgiFields(environ)
    if cgi_fields.error_string != "":
        error_dict = {"error_string" : cgi_fields.error_string}
	logg.write_error("%s: %s" % (Maw_phone_app_error, cgi_fields.error_string))
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)
        
    cf = sys_path.State_dictionary[cgi_fields.state]
    logg.write_info("called CgiFields, state: %s" % cgi_fields.state)

    # check session id and sequence numbers
    # note that a sequence number of -9999 signifies that the session has been retired and is no longer active
    logg.write_info("getting session information")
    session = None
    session = Session_dict.get(cgi_fields.phone_id, None)
    if session == None:
        error_string = "no active session for phone %s" % cgi_fields.phone_id
	logg.write_error(error_string)
        error_dict = {"error_string" : error_string}
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)

    if session["sequence_no"] == -9999:
        error_string = "session id not active in request from phone %s" % cgi_fields.phone_id
	logg.write_error(error_string)
        error_dict = {"error_string" : error_string}
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)

    if session["session_id"] != cgi_fields.session_id:
        error_string = "incorrect session id for phone %s" % cgi_fields.phone_id
	logg.write_error(error_string)
        error_dict = {"error_string" : error_string}
        json_error = json.dumps(error_dict)
        return send_response(json_error, start_response)

    session["sequence_no"] += 1

    logg.write_info("input session id: %s, phone id: %s" % (cgi_fields.session_id, cgi_fields.phone_id))
    curr_time = datetime.datetime.now()
    expanded_time_string = curr_time.strftime("%Y%m%d%H%M%S.%f")
    short_time_string = expanded_time_string[0:14]

    (maw_json, maw_json_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, short_time_string, cf.maw_json_dir)
    logg.write_info("latest maw json file, current time : %s, %s" % (maw_json, short_time_string))

    # get maw alerts
    maw_alert_dict = maw_alerts.get_maw_alerts(cf, cgi_fields.latitude, cgi_fields.longitude, cf.radius, maw_json, cgi_fields.bearing if cgi_fields.bearing != -9999 else None, 45.0)
    maw_alert_dict["sequence_no"] = str(session["sequence_no"])
    maw_alert_dict["session_id"] = str(session["session_id"])
    maw_alert_dict["alert_time"] = short_time_string
    maw_alert_dict["alert_gen_time"] = short_time_string
    maw_alert_dict["alert_request_time"] = maw_json_time
    #logg.write_info("maw latitude %f, longitude %f, alert sequence_no %s, session_id %s, alert_time %s, alert_gen_time %s, alert_request_time %s" % (latitude, longitude, maw_alert_dict["sequence_no"], maw_alert_dict["session_id"], maw_alert_dict["alert_time"], maw_alert_dict["alert_gen_time"], maw_alert_dict["alert_request_time"]))


    # prepare log messages
    phone_log = PhoneLog.PhoneLog(sys_path.phone_log_dir)
    log_header_string = "date,hour,minute,second," + cgi_fields.log_header()
    log_request_string = "%s,%s,%s,%s,%s" % (expanded_time_string[0:8], expanded_time_string[8:10], expanded_time_string[10:12], expanded_time_string[12:], cgi_fields.log_message())
    log_response_header_string = "server_sequence_number,precip_alert,pavement_alert,visibility_alert,indicated_action,Received,Correct,Comments\n"
    revised_precip_alert = maw_alerts.Precip_alerts[maw_alert_dict["alert_code_precip"]].replace(",", "-")
    revised_pavement_alert = maw_alerts.Pavement_alerts[maw_alert_dict["alert_code_pavement"]].replace(",", "-")
    revised_visibility_alert = maw_alerts.Visibility_alerts[maw_alert_dict["alert_code_visibility"]].replace(",", "-")
    revised_action = maw_alerts.Indicated_action[maw_alert_dict["alert_action_code"]].replace(",", "-")
    log_response_string = "%s,%s,%s,%s,%s,,,\n" % (maw_alert_dict["sequence_no"], revised_precip_alert, revised_pavement_alert, revised_visibility_alert, revised_action)
    
    # formulate phone log file name
    phone_log_file_name = phone_log.generate_file_name(cgi_fields.phone_id, short_time_string[0:8])
    if os.path.exists(phone_log_file_name):
        phone_log.write(phone_log_file_name, "%s%s" % (log_request_string, log_response_string))
    else:
        phone_log.write(phone_log_file_name, "%s%s%s%s" % (log_header_string, log_response_header_string, log_request_string, log_response_string))
    logg.write_info("wrote phone log %s" % phone_log_file_name)

    # open maw global file
    fd = os.open(Maw_global_data_file, os.O_RDWR|os.O_CREAT)

    # lock maw global file
    fcntl.lockf(fd, fcntl.LOCK_EX)
    
    # load session information from maw global file
    try:
        Session_dict = json.load(fo)
    except ValueError:
        pass
	    
    # update session information in maw global file
    Session_dict[cgi_fields.phone_id] = session
    
    json_str = json.dumps(Session_dict)

    # open and write temporary file
    temp_file_name = Maw_global_data_file + ".tmp"
    temp_fp = open(temp_file_name, "w")
    temp_fp.write(json_str)
    temp_fp.close()

    # move temporary file to MAW global file
    # note that this will atomically replace the original MAW global file
    os.rename(temp_file_name, Maw_global_data_file)

    fcntl.lockf(fd, fcntl.LOCK_UN)
    os.close(fd)

    json_response = json.dumps(maw_alert_dict)
    logg.write_ending(0, sys_path.phone_log)
    return send_response(json_response, start_response)
