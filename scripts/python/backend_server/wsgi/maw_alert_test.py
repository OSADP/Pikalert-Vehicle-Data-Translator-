#!/usr/bin/env python

"""Get nearest alerts"""

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import os
import sys
from optparse import OptionParser
import numpy
import time

def get_mi_alerts():
    """Used for setting up alerts for locations in Michigan"""
    state_alert_list = []

    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 42.336412
    state_alert_list[-1]["lon"] = -83.00012
    state_alert_list[-1]["alert_code_precip"] = 1
    state_alert_list[-1]["alert_code_pavement"] = 1
    state_alert_list[-1]["alert_code_visibility"] = 1
    state_alert_list[-1]["alert_action_code"] = 1

    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 42.337
    state_alert_list[-1]["lon"] = -83.001
    state_alert_list[-1]["alert_code_precip"] = 2
    state_alert_list[-1]["alert_code_pavement"] = 2
    state_alert_list[-1]["alert_code_visibility"] = 2
    state_alert_list[-1]["alert_action_code"] = 2

    return state_alert_list


def get_mn_alerts():

    """Used for setting up alerts for locations in Minnesota"""
    state_alert_list = []

    return state_alert_list


def get_nv_alerts():

    """Used for setting up alerts for locations in Nevada"""
    state_alert_list = []

    """Out of Range"""
    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 43.76610
    state_alert_list[-1]["lon"] = -72.18246
    state_alert_list[-1]["alert_code_precip"] = -9999
    state_alert_list[-1]["alert_code_pavement"] = -9999
    state_alert_list[-1]["alert_code_visibility"] = -9999
    state_alert_list[-1]["alert_action_code"] = -9999

    """Corner"""
    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 43.76608
    state_alert_list[-1]["lon"] = -72.18237
    state_alert_list[-1]["alert_code_precip"] = 2
    state_alert_list[-1]["alert_code_pavement"] = 2
    state_alert_list[-1]["alert_code_visibility"] = 2
    state_alert_list[-1]["alert_action_code"] = 2

    """Bridge"""
    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 43.76606
    state_alert_list[-1]["lon"] = -72.1828
    state_alert_list[-1]["alert_code_precip"] = 1
    state_alert_list[-1]["alert_code_pavement"] = 1
    state_alert_list[-1]["alert_code_visibility"] = 1
    state_alert_list[-1]["alert_action_code"] = 1

    """Mailbox"""
    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 43.76604
    state_alert_list[-1]["lon"] = -72.18227
    state_alert_list[-1]["alert_code_precip"] = 0
    state_alert_list[-1]["alert_code_pavement"] = 0
    state_alert_list[-1]["alert_code_visibility"] = 0
    state_alert_list[-1]["alert_action_code"] = 0

    """Wood Pile"""
    state_alert_list.append({})
    state_alert_list[-1]["lat"] = 43.76592
    state_alert_list[-1]["lon"] = -72.1819
    state_alert_list[-1]["alert_code_precip"] = 2
    state_alert_list[-1]["alert_code_pavement"] = 2
    state_alert_list[-1]["alert_code_visibility"] = 2
    state_alert_list[-1]["alert_action_code"] = 2

    return state_alert_list

def get_location_list(state_alert_list):
    """Get lat, lon list from state alert list"""
    locations = []
    for item in state_alert_list:
        locations.append([item["lat"], item["lon"]])
    return locations
    
def get_state_alert_dict():

    state_alert_dict = {}

    mi_alerts = get_mi_alerts()
    mi_locations = numpy.array(get_location_list(mi_alerts))
    mn_alerts = get_mn_alerts()
    mn_locations = numpy.array(get_location_list(mn_alerts))
    nv_alerts = get_nv_alerts()
    nv_locations = numpy.array(get_location_list(nv_alerts))
    alert_location_dict = {}
    alert_location_dict["alerts"] = mi_alerts
    alert_location_dict["locations"] = mi_locations
    state_alert_dict["michigan"] = alert_location_dict 

    mn_alert_location_dict = {}
    mn_alert_location_dict["alerts"] = mn_alerts
    mn_alert_location_dict["locations"] = mn_locations
    state_alert_dict["minnesota"] = mn_alert_location_dict 

    nv_alert_location_dict = {}
    nv_alert_location_dict["alerts"] = nv_alerts
    nv_alert_location_dict["locations"] = nv_locations
    state_alert_dict["nevada"] = nv_alert_location_dict 

    return state_alert_dict
    

def get_alert(state_alert_dict, state, lat, lon):
    """Look up state, then location, then alert. Return None if there is no associated alert."""

    result_dict = {}
    result_dict["alert_code_precip"] = -9999
    result_dict["alert_action_code"] = -9999
    result_dict["alert_code_pavement"] = -9999
    result_dict["alert_code_visibility"] = -9999

    state_alerts = state_alert_dict.get(state)
    if state_alerts == None:
        return result_dict

    locations = state_alerts.get("locations")
    if locations == None:
        return result_dict
    
    query = numpy.array([lat, lon])

    index = numpy.argmin([numpy.inner(query-x, query-x) for x in locations])
    alerts = state_alerts.get("alerts")
    alert_dict = alerts[index]

    result_dict["alert_code_precip"] = alert_dict["alert_code_precip"]
    result_dict["alert_code_pavement"] = alert_dict["alert_code_pavement"]
    result_dict["alert_code_visibility"] = alert_dict["alert_code_visibility"]
    result_dict["alert_action_code"] = alert_dict["alert_action_code"]
    return result_dict
    

def main():

    state_alert_dict = get_state_alert_dict()
    lat = 42.336412
    lon = -83.00012

    alert = get_alert(state_alert_dict, "michigan", lat, lon)
    print alert

if __name__ == "__main__":

   main()
