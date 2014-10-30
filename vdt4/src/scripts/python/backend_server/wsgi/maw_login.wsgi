
import sys
import os
import fcntl
import json
import cgi
import time
import phone_ids

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
    
    # Open global data file
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
    print "phone_id is trying to login: ", phone_id
    if not phone_id in Phone_id_set:
        error_dict = {"error_string" : "phone not registered"}
	json_str = json.dumps(error_dict)
        return send_response(json_str, start_response)
    
    action = form.getfirst("action", "")
    if not action in Login_actions:
        error_dict = {"error_string" : "action must be login or logout"}
	json_str = json.dumps(error_dict)
        return send_response(json_str, start_response)
    
    session = {}
    if action == "login":
        # start a new session
	session_id = int(time.time())
	session = {"session_id" : session_id, "sequence_no" : 1}
	Session_dict[phone_id] = session
    elif action == "logout":
        if not phone_id in Session_dict:
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
	session = {"session_id" : session_id, "sequence_no" : -9999}
	Session_dict[phone_id] = session

    # Open maw global data file
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
    print "json_str: ", json_str
    ret = os.write(fd, json_str)
    print "ret: ", ret
    fcntl.lockf(fd, fcntl.LOCK_UN)
    fo.close()

    login_dict = { "session_id" : Session_dict[phone_id]["session_id"], "sequence_no" : Session_dict[phone_id]["sequence_no"]}

    json_response = json.dumps(login_dict)
    return send_response(json_response, start_response)
