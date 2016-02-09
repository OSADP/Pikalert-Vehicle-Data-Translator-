
import sys
import os
import fcntl
import json
import cgi
import time
import phone_ids
import log_msg
import backend_sys_path as sys_path

Maw_global_data_file = "/tmp/maw_phone_app.json"
Login_actions = set(["login", "logout"])

Phone_ids = phone_ids.Phone_ids
Phone_id_set = set(Phone_ids)

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

Session_dict = {}

def send_response(output, start_response):
    response_headers = [("Cache-Control","no-cache, no-store"),
                        ("Pragma","no-cache"),
                        ("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

# Sample request
# http://www.ral.ucar.edu/projects/rdwx_mdss/AK_mdss/proxy.php?path=/maw_login&phone_id=7202261093&action=login

def application(environ, start_response):

    global Session_dict
    
    phone_login_log = os.path.join(sys_path.log_dir, sys_path.phone_login_log)
    logg = log_msg.LogMessage(phone_login_log, "pyl")
    logg.write_starting(sys_path.phone_login_log)

    # Open global data file
    logg.write_info("opening Maw_global_data_file %s to read session info" % Maw_global_data_file)
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
	
    
    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)

    phone_id = form.getfirst("phone_id", "")
    logg.write_info("phone_id %s is accessing maw_login.wsgi" % phone_id)

    if not phone_id in Phone_id_set:
        logg.write_error("phone_id %s has not been registered" % phone_id)
        error_dict = {"error_string" : "phone not registered"}
	json_str = json.dumps(error_dict)
        return send_response(json_str, start_response)
    
    action = form.getfirst("action", "")
    if not action in Login_actions:
        logg.write_error("phone action must be login or logout")
        error_dict = {"error_string" : "action must be login or logout"}
	json_str = json.dumps(error_dict)
        return send_response(json_str, start_response)
    
    session = {}
    if action == "login":
        # start a new session
	session_id = int(time.time())
        logg.write_info("starting new login session %d for phone %s" % (session_id, phone_id))
	session = {"session_id" : session_id, "sequence_no" : 1}
	Session_dict[phone_id] = session
    elif action == "logout":
        if not phone_id in Session_dict:
	    logg.write_info("phone %s cannout logout since no active session for phone" % phone_id)
	    error_dict = {"error_string" : "cannot logout since no active session for phone"}
	    json_str = json.dumps(error_dict)
            return send_response(json_str, start_response)
        
        # close the current session
	session = Session_dict[phone_id]
	sequence_no = session["sequence_no"]
	if sequence_no == -9999:
	    error_dict = {"error_string" : "cannot logout since phone is not logged in"}
	    json_str = json.dumps(error_dict)
            return send_response(json_str, start_response)
	
	session_id = session["session_id"]
        logg.write_info("phone %s is logging out of session %d" % (phone_id, session_id))
	session = {"session_id" : session_id, "sequence_no" : -9999}
	Session_dict[phone_id] = session

    # Open maw global data file
    logg.write_info("opening maw global data file %s for updating" % Maw_global_data_file)

    fd = os.open(Maw_global_data_file, os.O_RDWR|os.O_CREAT)

    # Lock it
    fcntl.lockf(fd, fcntl.LOCK_EX)

    # Now get a file object for the above file.
    fo = os.fdopen(fd, "w+")

    # Load the information
    try:
        Session_dict = json.load(fo)
    except ValueError:
        pass
	    
    Session_dict[phone_id] = session
    
    json_str = json.dumps(Session_dict)
    os.ftruncate(fd, 0)
    os.lseek(fd, 0, 0)
    ret = os.write(fd, json_str)
    fcntl.lockf(fd, fcntl.LOCK_UN)
    fo.close()

    login_dict = { "session_id" : Session_dict[phone_id]["session_id"], "sequence_no" : Session_dict[phone_id]["sequence_no"]}

    json_response = json.dumps(login_dict)
    logg.write_ending(0, sys_path.phone_login_log)

    return send_response(json_response, start_response)
