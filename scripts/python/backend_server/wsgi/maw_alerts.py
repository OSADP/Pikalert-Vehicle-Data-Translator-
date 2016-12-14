#!/usr/bin/env python

# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/04/02 23:53:46 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import json
import numpy
import os
import road_wx_numpy
import backend_sys_path
import math


Precip_alerts = {
    
    -9999 : "unknown",
    0 : "clear",
    1 : "light rain",
    2 : "moderate rain",
    3 : "heavy rain", 
    4 : "light rain/snow mix",
    5 : "moderate rain/snow mix",
    6 : "heavy rain/snow mix",
    7 : "light snow",
    8 : "moderate snow",
    9 : "heavy snow"
    }

Pavement_alerts = {
    -9999 : "unknown",
    0 : "clear",
    1 : "wet roads",
    2 : "snowy roads",
    3 : "snowy, slick roads",
    4 : "icy roads",
    5 : "icy, slick roads",
    6 : "hydroplaning possible",
    7 : "black ice",
    8 : "icy roads possible",
    9 : "icy, slick roads possible"
    }

Visibility_alerts = {
    -9999 : "unknown",
    0 : "clear",
    1 : "low visibility",
    2 : "blowing snow",
    3 : "fog",
    4 : "haze",
    5 : "dust",
    6 : "smoke"
    }

Indicated_action = {
    -9999 : "unknown",
    0 : "none",
    1 : "use caution",
    2 : "drive slowly and use caution",
    3 : "delay travel, seek alternate route, or drive slowly and use extreme caution"
    }

    
def get_degree_radius(lat, radius_m):
    """
    Calculates a degree radius equivalent to 2*radius_m
    that accounts for the latitude.
    """
    #WGS84 equatorial radius in m
    R = 6378137.0
    m_per_deg_eq = R*math.pi/180.0
    m_per_deg_lat = m_per_deg_eq * math.cos(lat*math.pi/180.0)
    return (2*radius_m)/m_per_deg_lat

            
def find_worst_weather(segment_indices, maw_dict):

    precip_priority_list = []
    precip_condition_list = []
    precip_action_list = []

    pavement_priority_list = []
    pavement_condition_list = []
    pavement_action_list = []

    visibility_priority_list = []
    visibility_condition_list = []
    visibility_action_list = []

    # Extract and collect precip, pavement and visibility information associated
    # with segment indices
    for index in segment_indices:
        item_dict = maw_dict.get(str(index), None)
        if item_dict != None:
            precip_priority_list.append(item_dict["maw_precip_priority"])
            precip_condition_list.append(item_dict["maw_precip_condition_code"])
            precip_action_list.append(item_dict["maw_precip_action_code"])

            pavement_priority_list.append(item_dict["maw_pavement_priority"])
            pavement_condition_list.append(item_dict["maw_pavement_condition_code"])
            pavement_action_list.append(item_dict["maw_pavement_action_code"])

            visibility_priority_list.append(item_dict["maw_visibility_priority"])
            visibility_condition_list.append(item_dict["maw_visibility_condition_code"])
            visibility_action_list.append(item_dict["maw_visibility_action_code"])

    
    # Find highest priority and associated action for each category.

    precip_max_priority_position = -1
    precip_max_condition = 0
    precip_max_action_priority = 0
    if len(precip_priority_list) > 0:
        precip_max_priority_position = numpy.argmax(precip_priority_list)
        precip_max_action_priority = precip_action_list[precip_max_priority_position]
        precip_max_condition = precip_condition_list[precip_max_priority_position]
        
    pavement_max_priority_position = -1
    pavement_max_condition = 0
    pavement_max_action_priority = 0
    if len(pavement_priority_list) > 0:
        pavement_max_priority_position = numpy.argmax(pavement_priority_list)
        pavement_max_action_priority = pavement_action_list[pavement_max_priority_position]
        pavement_max_condition = pavement_condition_list[pavement_max_priority_position]

    visibility_max_priority_position = -1
    visibility_max_condition = 0
    visibility_max_action_priority = 0
    if len(visibility_priority_list) > 0:
        visibility_max_priority_position = numpy.argmax(visibility_priority_list)
        visibility_max_action_priority = visibility_action_list[visibility_max_priority_position]
        visibility_max_condition = visibility_condition_list[visibility_max_priority_position]

    max_action_priority = max(precip_max_action_priority, pavement_max_action_priority, visibility_max_action_priority)
    maw_alert_dict = {}

    maw_alert_dict["alert_code_precip"] = precip_max_condition
    maw_alert_dict["alert_code_pavement"] = pavement_max_condition
    maw_alert_dict["alert_code_visibility"] = visibility_max_condition
    maw_alert_dict["alert_action_code"] = max_action_priority
    return maw_alert_dict
    


def get_maw_alerts(cf, latitude, longitude, radius_meters, maw_json, bearing=None, sector_half_theta=None):

    # Create road weather locator for state
    state_wx_locator = road_wx_numpy.RoadWeatherLocator(cf.maw_nearest_neighbor_files[1], cf.maw_nearest_neighbor_files[2], get_degree_radius(latitude, radius_meters))

    # Find nearest road segments in state
    (nnidx, nndistances) = state_wx_locator.radius(latitude, longitude, radius_meters, bearing, sector_half_theta)

    maw_alert_dict = {}
    maw_alert_dict["alert_code_precip"] = -9999
    maw_alert_dict["alert_code_pavement"] = -9999
    maw_alert_dict["alert_code_visibility"] = -9999
    maw_alert_dict["alert_action_code"] = -9999

    if len(nnidx) == 0:
        return maw_alert_dict
    
    segment_indices = state_wx_locator.seg_id[nnidx]
    
    # Read in json file containing weather associated with road segments
    maw_dict = {}
    if maw_json != "":
        if os.path.exists(maw_json):
            maw_dict = json.load(open(maw_json))
            
    maw_alert_dict = find_worst_weather(segment_indices, maw_dict)
 
    return maw_alert_dict

def main():

    import backend_sys_path

    cf = backend_sys_path.State_dictionary["minnesota_vdt"]


    latitude = 46.0438
    longitude = -96.4943
    radius_meters = 1000
    
    print get_maw_alerts(cf, latitude, longitude, radius_meters, "/d2/vii/data/processed/minnesota/maw_json/20151020/maw.20151020.1620.json", 180.0, 45)
    
if __name__ == "__main__":

    main()
