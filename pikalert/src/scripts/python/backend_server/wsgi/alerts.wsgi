
import sys
import os
import json
import cgi
import string
import time
import datetime
from netCDF4 import Dataset
from numpy import ma
import sites
from alerts import RdwxAlerts,TmtAlerts,ObsAlerts,RwisAlerts
import Image,ImageDraw
import base64
import StringIO
import matplotlib
matplotlib.use('Agg')  # force the antigrain backend
from matplotlib import rc
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.cbook import iterable
import datetime
import sys_path
from numpy import arange
from matplotlib.dates import DayLocator, HourLocator, DateFormatter, drange
import data_time_index

time.strptime("20121129","%Y%m%d") #(http://bugs.python.org/issue7980) bug in python makes strptime not thread safe, causing random complaints about strptime import failing, calling it here from the main thread should fix it, hopefully it will be fixed in future versions.

sys.path.append("/home/dicast/akmdss_view")

def send_response(output, start_response):
    response_headers = [("Content-type","text/html"),
			("Content-length",str(len(output)))]
    start_response("200 OK", response_headers)
    return [output]

def application(environ, start_response):

    form = cgi.FieldStorage(fp=environ["wsgi.input"], environ=environ)
    date_str = form.getfirst("date_str", time.strftime("%Y%m%d.%H%M"))
    alert_type = form.getfirst("alert_type", "wx")
    state = "alaska"
    cf = sys_path.State_dictionary[state]

    data_time_index_obj = data_time_index.DataTimeIndex(cf.product_dictionary)
    ptime = time.mktime(time.strptime(date_str,"%Y%m%d.%H%M"))
    output_dict = {}
    output = []
    all_sites = sites.get_rdwx_sites(cf)
    cond_sites = sites.get_road_cond_sites(cf)
    obs_sites = sites.get_wx_obs_sites(cf)
    rwis_sites = sites.get_rwis_sites(cf)

    road_cond_alert_images = {}
    obs_alert_images = {}
    rdwx_alert_images = {}
    time_alert_images = {}

    if alert_type == "road_cond":
        tmt_alerts = TmtAlerts(cf, ptime)
        road_cond_alert_images = tmt_alerts.get_alerts()
    elif alert_type == "obs":
        obs_alerts = ObsAlerts(cf, ptime)
        obs_alert_imgs = obs_alerts.get_alerts(obs_sites)
        rwis_alerts = RwisAlerts(cf, ptime)
        rwis_alert_imgs = rwis_alerts.get_alerts(rwis_sites)

        for k in obs_alert_imgs.keys():
	    obs_alert_images[k] = obs_alert_imgs[k]
        for k in rwis_alert_imgs.keys():
            obs_alert_images[k] = rwis_alert_imgs[k]
    else:
        rdwx_alerts = RdwxAlerts(cf, ptime)
        rdwx_alert_images = rdwx_alerts.get_alerts()
        # Paddy added these
        obs_alerts = ObsAlerts(cf, ptime)
        obs_alert_imgs = obs_alerts.get_alerts(obs_sites)
        tmt_alerts = TmtAlerts(cf, ptime)
        road_cond_alert_images = tmt_alerts.get_alerts()
        time_alert_images = rdwx_alerts.get_time_alerts()

    ks = all_sites.keys()
    ks.sort()
    for k in ks:
        site = all_sites[k]
        site_num = site["site_num"]
	if time_alert_images.has_key(site_num):
	    site["hr72_alert_image"] = time_alert_images[site_num][3]
	    site["hr24_alert_image"] = time_alert_images[site_num][2]
	    site["hr06_alert_image"] = time_alert_images[site_num][1]
	    site["obs_alert_image"] = time_alert_images[site_num][0]

	#if obs_alert_images.has_key(site_num):
	#    site["obs_alert_image"] = obs_alert_images[site_num]
	#if rdwx_alert_images.has_key(site_num):
	#    site["rdwx_alert_image"] = rdwx_alert_images[site_num]
        site["is_road_cond_site"] = False
        site["is_wx_obs_site"] = False
        site["is_rwis_site"] = False
        if cond_sites.has_key(k):
            site["is_road_cond_site"] = True
        if obs_sites.has_key(k):
            site["is_wx_obs_site"] = True
        if rwis_sites.has_key(k):
            site["is_rwis_site"] = True
        output.append(site)

    output_dict["data"] = output
    output_dict["data_time"] = date_str

    json_response = json.dumps(output_dict)
    return send_response(json_response, start_response)
