

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import encode_image
import sys
import os
import json
import cgi
import string
import time
import sites as sites
import plots as plots
import backend_sys_path as sys_path
import alerts as alerts
import data_time_index
import district_alerts
import log_msg
import get_site_time_series
#import get_latest_alaska_files
import merge_image_data
import tempfile
import glob

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

    plots_log = os.path.join(sys_path.log_dir, sys_path.plots_log)
    logg = log_msg.LogMessage(plots_log, "pyl")
    logg.write_starting(sys_path.plots_log)

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
    logg.write_info("state, site, time: %s, %d, %s\n" % (state, site, yyyymmddhhmm))
    (rdwx_fpath, rdwx_fpath_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, yyyymmddhhmm, cf.rdwx_dir)
    (tmt_fpath, tmt_fpath_time) = data_time_index.get_latest_file_using_time(sys_path.File_time_delta, yyyymmddhhmm, cf.tmt_dir)
    logg.write_info("read rdwx and treatment files: %s, %s\n" % (rdwx_fpath, tmt_fpath))

    # Get plotting data
    output = {}
    current_utc_hour = int(date_str[9:11])
    logg.write_info("getting summary plot information\n")
    if rdwx_fpath != None and os.path.exists(rdwx_fpath):
        summary_plot = plots.get_summary_plot(logg, cf, rdwx_fpath, tmt_fpath, site, current_utc_hour, cf.timezone)
        #summary_plot = "R0lGODlhDwAPAKECAAAAzMzM/////wAAACwAAAAADwAPAAACIISPeQHsrZ5ModrLlN48CXF8m2iQ3YmmKqVlRtW4MLwWACH+H09wdGltaXplZCBieSBVbGVhZCBTbWFydFNhdmVyIQAAOw=="
        output["summary_plot"] = summary_plot

    # Get district alerts time series information
    logg.write_info("processing district alerts for state, time: %s, %s" % (state, yyyymmddhhmm))

    (file_name, yyyymmddhhmm) = data_time_index.get_latest_file(cf.product_dictionary["district_alerts"])
    
    if yyyymmddhhmm != "":
        district_alerts_path = district_alerts.get_district_alerts(cf, yyyymmddhhmm)
    json_districts = {}

    time_series = []
    if district_alerts_path != "":
        time_series = get_site_time_series.get_site_time_series(site, district_alerts_path)
    output["time_series"] = time_series
    
    # Get all sites dict
    logg.write_info("getting all sites dict from %s\n" % cf.rwis_sites_file)
    all_sites_dict = sites.get_all_site_dict(cf.rwis_sites_file)
    logg.write_info("all sites dict keys\n")
    logg.write_info("all sites dict has %d keys\n" % len(all_sites_dict.keys()))
    
    # Get rwis observations
    rwis_site_dict = sites.get_rwis_site_dict(all_sites_dict)
    if rwis_site_dict.has_key(site):
        logg.write_info("site is rwis site")
        rwis_site = rwis_site_dict[site]
        rwis_alerts = alerts.RwisAlerts(cf, ptime)
        rwis = rwis_alerts.get_obs(site)
        for k in rwis.keys():
            rwis[k] = str(rwis[k])
        output["rwis"] = rwis
    else:
        logg.write_info("site is NOT rwis site")

    # Get road segment observations
    road_segment_site_dict = sites.get_road_segment_site_dict(all_sites_dict)
    if road_segment_site_dict.has_key(site):
        logg.write_info("site is road segment site")
        road_segment_site = road_segment_site_dict[site]
        in_segment_dict = plots.get_obs_stats(cf, site)
        out_segment_dict = {}
        for key,value in in_segment_dict.items():
            out_segment_dict[key.encode('ascii', 'ignore')] = value
        output["road_segments"] = out_segment_dict
    else:
        logg.write_info("site is NOT road segment site")


    # Translate site number into appropriate site id
    output["image"] = ""
    if state == "alaska_vdt":
        """
        logg.write_info("getting latest image files for site: %d\n" % site)
        alaska_image_dict = get_latest_alaska_files.get_latest_alaska_files(3600, site, sys_path.alaska_image_dir)

        # Produce image file for site id
        out_file_object = tempfile.NamedTemporaryFile(delete=False)
        out_file_name = out_file_object.name
        out_file_object.close()
        ret = merge_image_data.merge_image_data(alaska_image_dict, out_file_name, logg)
        if ret == 0:
            logg.write_info("successfully merged alaska image data to %s" % out_file_name)
            output["image"] = encode_image.encode_image(out_file_name)

            logg.write_info("removing %s" % out_file_name)
            os.remove(out_file_name)
        else:
            logg.write_info("did not get image data for %d" % site)

        # Base64 encode image file and store
        #output["image"] = encode_image.encode_image("/home/dicast/mdss_view/test.jpg")
        """
    elif state == "minnesota_vdt":
        logg.write_info("getting latest image files for site: %d\n" % site)
        latest_file = get_latest_minnesota_image_file.get_latest_minnesota_image_file(3600, site, sys_path.minnesota_image_dir)
	if latest_file != "":
	    output["image"] = encode_image.encode_image(latest_file)
        else:
            logg.write_info("did not get image data for %d" % site)

        # Base64 encode image file and store
        #output["image"] = encode_image.encode_image("/home/dicast/mdss_view/test.jpg")
	    
    # Create json output
    logg.write_info("dumping json final output for state %s\n" % state)

    json_output = json.dumps(output)
    logg.write_ending(0, sys_path.plots_log)
    return send_response(json_output, start_response)
