

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
import time
import backend_sys_path as sys_path
import data_time_index
import log_msg
import plots

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

def send_response(output, start_response):
    response_headers = [("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):

    """
    test code
    #plots.main()
    test()
    output = json.dumps({})
    return send_response(output, start_response)
    """

    site_plots_log = os.path.join(sys_path.log_dir, sys_path.site_plots_log)
    logg = log_msg.LogMessage(site_plots_log, "pyl")
    logg.write_starting(sys_path.site_plots_log)

    # Get cgi form information
    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    date_str = form.getfirst("date_str", time.strftime("%Y%m%d.%H%M"))
    site = form.getfirst("site", None)

    if site == None:
        logg.write_error("must provide a site number as integer")
        logg.write_ending(1, "plots.wsgi")
        return send_response("must provide a site number as integer", start_response)
    try:
        site = int(site)
    except:
        return send_reponse("couldn't parse %s as integer for site number" % site, start_response)
    state = form.getfirst("state", None)
    if state == None or not state in sys_path.State_dictionary:
        logg.write_error("state not provided or not in State_dictionary")
        logg.write_ending(1, "plots.wsgi")
        return send_response("must provide a state name such as michigan, minnesota, nevada, etc.", start_response)

    # Resolve state of interest
    cf = sys_path.State_dictionary[state]

    # Replace time with current time
    #ptime = time.mktime(time.strptime(date_str,"%Y%m%d.%H%M"))
    ptime = time.time()
    date_str = time.strftime("%Y%m%d.%H%M", time.gmtime(ptime))
    yyyymmddhhmm = date_str[0:8] + date_str[9:13]
    
    # Get latest road weather and treatment forecast file paths and times
    logg.write_info("processing state, site, time: %s, %d, %s" % (state, site, yyyymmddhhmm))
    (rdwx_fpath, rdwx_fpath_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, yyyymmddhhmm, cf.rdwx_dir)
    (tmt_fpath, tmt_fpath_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, yyyymmddhhmm, cf.tmt_dir)
    logg.write_info("read rdwx and treatment files: %s, %s" % (rdwx_fpath, tmt_fpath))

    # Get plotting data
    output = {}
    current_utc_hour = int(date_str[9:11])
    logg.write_info("getting summary plot information")
    if rdwx_fpath != None and os.path.exists(rdwx_fpath):
        summary_plot = plots.get_summary_plot(logg, cf, rdwx_fpath, tmt_fpath, site, current_utc_hour, cf.timezone)
        #summary_plot = plots.get_summary_plot(logg, cf, rdwx_fpath, tmt_fpath, site, current_utc_hour, cf.timezone, "/tmp/outfile.png")
        #summary_plot = "R0lGODlhDwAPAKECAAAAzMzM/////wAAACwAAAAADwAPAAACIISPeQHsrZ5ModrLlN48CXF8m2iQ3YmmKqVlRtW4MLwWACH+H09wdGltaXplZCBieSBVbGVhZCBTbWFydFNhdmVyIQAAOw=="
        output["summary_plot"] = summary_plot

    # Create json output
    logg.write_info("dumping json final output for site, state: %s, %s" % (site, state))

    json_output = json.dumps(output)
    logg.write_ending(0, sys_path.site_plots_log)
    return send_response(json_output, start_response)
