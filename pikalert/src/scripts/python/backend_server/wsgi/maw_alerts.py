#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2014 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2014/10/20 13:00:00 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import json
import numpy
import os
import road_wx_numpy as road_wx_locator

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
    print "segment indices: ", segment_indices
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
    print "precip priority list: ", precip_priority_list
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

    print "precip, pavement, visibility priorities: ", precip_max_action_priority, pavement_max_action_priority, visibility_max_action_priority
    max_action_priority = max(precip_max_action_priority, pavement_max_action_priority, visibility_max_action_priority)
    print "precip_condition_list: ", precip_condition_list
    print "precip_priority_list: ", precip_priority_list
    print "pavement_priority_list: ", pavement_priority_list
    print "visibility_priority_list: ", visibility_priority_list
    maw_alert_dict = {}

    maw_alert_dict["alert_code_precip"] = precip_max_condition
    maw_alert_dict["alert_code_pavement"] = pavement_max_condition
    maw_alert_dict["alert_code_visibility"] = visibility_max_condition
    maw_alert_dict["alert_action_code"] = max_action_priority
    return maw_alert_dict
    


def get_maw_alerts(cf, latitude, longitude, deg_radius, maw_json):
    
    # Create road weather locator for state
    state_wx_locator = road_wx_locator.RoadWeatherLocator(cf.maw_nearest_neighbor_files[0], cf.maw_nearest_neighbor_files[1], cf.maw_nearest_neighbor_files[2])
    print "state flann files: ", cf.maw_nearest_neighbor_files[0], cf.maw_nearest_neighbor_files[1], cf.maw_nearest_neighbor_files[2]
    print state_wx_locator.seg_pts
    print state_wx_locator.seg_id

    # Find nearest road segments in state
    print "calling state_wx_locator radius: ", deg_radius
    (nnidx, nndistances) = state_wx_locator.radius(latitude, longitude, deg_radius)
    print "nnidx: ", nnidx
    
    maw_alert_dict = {}
    maw_alert_dict["alert_code_precip"] = -9999
    maw_alert_dict["alert_code_pavement"] = -9999
    maw_alert_dict["alert_code_visibility"] = -9999
    maw_alert_dict["alert_action_code"] = -9999

    if len(nnidx) == 0:
        return maw_alert_dict
    
    segment_indices = state_wx_locator.seg_id[nnidx]
    print "closest segment_indices: ", segment_indices
    
    # Read in json file containing weather associated with road segments
    maw_dict = {}
    if maw_json != "":
        if os.path.exists(maw_json):
            maw_dict = json.load(open(maw_json))
            
    print "maw_json file: ", maw_json

    # Map road segment indices to ids

    # Check weather for nearest road segments
    print "calling find worst weather"
    maw_alert_dict = find_worst_weather(segment_indices, maw_dict)
    print "maw_alert_dict: ", maw_alert_dict
    return maw_alert_dict

def main():

    import backend_sys_path

    cf = backend_sys_path.State_dictionary["minnesota_vdt"]

    latitude = 44.
    longitude = -84.
    degree_radius = 0.6
    
    get_maw_alerts(cf, latitude, longitude, degree_radius, "/d1/vii/data/processed/minnesota/maw_json/20140204/maw.20140204.2240.json")
    
if __name__ == "__main__":

    main()
