#!/usr/bin/env python

"""
Gets data from begin_data through end_data from weather telematics,
converts response json to netcdf, and then converts the netCDF to
the probe netCDF format.
"""

# ============================================================================== #
#                                                                                #
#   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).  #
#       All rights reserved.                                                     #
#                                                                                #
#       File: $RCSfile: fileheader,v $                                           #
#       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $           #
#                                                                                #
# ============================================================================== #

import sys
import os.path
import log_msg
from optparse import OptionParser
import get_wxt_json
import wxt_parse_json
import wxt2probe_message
from datetime import datetime
from datetime import timedelta
import collections
import json
import shutil
import time
import copy
import group_probe_message_files as group_msg
import tim
import name_schema

cdl="/home/vii/cvs/projects/vdt4/static_data/cdl/probe_message.cdl"
file_base = "wxt"
time_interval = 300
filter_gte_min = 5
cdl_file = "/d2/vii/data/static/cdl/probe_message.cdl"
work_dir = "/d2/vii/data/tmp"
name_schema_base = "wxt"
config_file = "vii_paths"

state_bounds = {"mi" : {"min_lat" : 40.4, "min_lon" : -91.4, "max_lat" : 48.7, "max_lon" : -80.7},\
                "mn" : {"min_lat" : 43.4, "min_lon" : -97.7, "max_lat" : 49.7, "max_lon" : -88.5},\
                "nv" : {"min_lat" : 34.5, "min_lon" : -120.8, "max_lat" : 42.8, "max_lon" : -113.1}}


#Controls the destination of a copy of the probe file for those states that contain an obs in the probe
group_dest_map = {"mi" : "/d2/vii/data/processed/mi_wxt_probe", \
              "mn" : "/d2/vii/data/processed/mn_wxt_probe", \
              "nv" : "/d2/vii/data/processed/nv_wxt_probe"}

class funchain(collections.Sequence):
    """
    class creates a sequence of functions that are executed in order by calling exec_chain
    """
    def __init__(self, logg, *funs):
	self.funlist = list(funs)
	self.logg = logg

    def __getitem__(self, index):
	return self.funlist[index]

    def __len__(self):
	return len(self.funlist)

    def append(self, fun):
	self.funlist.append(fun)

    def exec_chain(self, val0, bailonfail=True):
	"""
	Execute the chain of functions starting with the first function applied at val0.
	Function 2 is then applied to this result.  Function 3 is in turn applied to
	this result and so on f3(f2(f1(val0))).  If bailonfail==True then exit when the
	first function returns a value that evaluates to False (i.e. [], False, "", etc.).
	
	Returns the result of the last function.
	"""
	if len(self.funlist) == 0:
	    return None
	
	ret = val0
	for fun in self.funlist:		 
	    ret = fun(ret)
	    if not ret:
		self.logg.write_error("function %s returned %s" % (fun.__name__, ret))
		if bailonfail:
		    break
	    
	return ret


def in_bounds(obs, bounds):
    lat = obs['latitude']
    lon = obs['longitude']
    
    if lat < bounds['min_lat'] or lat > bounds['max_lat'] or \
        lon < bounds['min_lon'] or lon > bounds['max_lon']:
        return False

    return True


def state_json_key(state):
    return "%s_jsonfile" % state

def state_cdf_key(state):
    return "%s_cdffile" % state

def state_probe_key(state):
    return "%s_probefile" % state
    
def partition_json_state(params):
    """
    Partition the WXT json file into separate json files that only contain observation sets within state bounds.
    """
    jsonfile = params["jsonfile"]
    jsonfilebase = os.path.basename(jsonfile)

    with open(jsonfile) as f:
        jsond = json.loads(f.read())

    for state, bounds in state_bounds.items():
        jsondcpy = copy.deepcopy(jsond)
        jsondcpy['observation_sets'][:] = [obs for obs in jsondcpy['observation_sets'] if in_bounds(obs, bounds)]
        if len(jsondcpy['observation_sets']) > 0:
            outfile = "%s/%s.%s" % (params["out_base"], state, jsonfilebase)
            f = open(outfile, 'w')
            f.write(json.dumps(jsondcpy, indent=4, sort_keys=True))
            f.close()
            params[state_json_key(state)] = outfile

    return params


def get_json_data(params):
    begin = params["begin"]
    end = params["end"]
    logg = params["logg"]
    
    response = get_wxt_json.get_single_update(begin,end,"",logg)
    if not response:
        return response
    
    outfile = "%s/%s.%s.json" % (params["out_base"],file_base,begin)
    f = open(outfile, 'w')
    f.write(response)
    f.close()

    #print outfile
    params["jsonfile"] = outfile
    return params
    

def json_2_cdf(params):
    logg = params["logg"]

    for state in state_bounds:
        if state_json_key(state) in params:
            outfile = "%s/%s.%s.raw.%s.nc" % (params["out_base"], state, file_base, params["begin"])
            infile = params[state_json_key(state)]
            files_written = wxt_parse_json.parse(infile, outfile, True, logg)
            params[state_cdf_key(state)] = files_written[0]
        
    return params


def rm_json_in(params):
    jsonfile = params["jsonfile"]
    os.unlink(jsonfile)
    params["rm_jsonfile"] = True
    return params


def rm_json_state(params):
    for state in state_bounds:
        if state_json_key(state) in params:
            os.unlink(params[state_json_key(state)])
            params["rm_state_jsonfile"] = True
            
    return params


def rm_nc_in(params):
    for state in state_bounds:
        if state_cdf_key(state) in params:
            os.unlink(params[state_cdf_key(state)])
            params["rm_state_cdffile"] = True
      
    return params


def cdf_2_probe(params):
    logg = params["logg"]
    
    for state in state_bounds:
        if state_cdf_key(state) in params:
            infile = params[state_cdf_key(state)]
            outfile = "%s/%s.%s.probe.%s.nc" % (params["out_base"], state, file_base, params["begin"])      
            wxt2probe_message.process(True, cdl, logg, infile, outfile)
            params[state_probe_key(state)] = outfile
            
    return params


def update_state_group(params):
    logg = params["logg"]
    dt = datetime.utcnow() - timedelta(minutes=filter_gte_min)
    filter_gte = int(tim.datetogmt(dt.strftime('%Y%m%d%H%M00')))
    #params["filter_gte"] = filter_gte
    #Configuration required before calling group_msg.group_probe_message_files
    cf = __import__(config_file, globals(), locals(), [])
    name_format = "%s_format" % name_schema_base
    name_patterns = "%s_patterns" % name_schema_base
    out_name_schema = name_schema.Name_schema(getattr(cf, name_format), getattr(cf, name_patterns))

    for state in state_bounds:
        if state_probe_key(state) in params:
            group_msg.group_probe_message_files(params[state_probe_key(state)], time_interval, cdl_file, work_dir, \
                                                group_dest_map[state], out_name_schema, logg, filter_gte)

    params["update_state_group"] = True
    return params

        	
def main():
    usage_str = "%s yyyymmddhhmmss yyyymmddhhmmss outbase" % sys.argv[0]
    parser = OptionParser(usage = usage_str)
    parser.add_option("-l", "--log", dest="log", help="write log messages to specified file")
    parser.add_option("-k", "--keep", action="store_true", dest="keep", help="keep all intermediate file (defaults to False)", default=False)
    
    (options, args) = parser.parse_args()
    
    if len(args) < 3:
        print parser.print_help()
	exit(2)

    #Dictionary of parameters that is passed through the chain of functions
    params = {}
    
    params["begin"] = args[0]
    params["end"]   = args[1]
    params["out_base"] = args[2]

    if options.log:
        logg = log_msg.LogMessage(options.log)
    else:
        logg = log_msg.LogMessage("")

    params["logg"] = logg
    
    logg.write_starting()

    funcs = funchain(logg)
    funcs.append(get_json_data)
    funcs.append(partition_json_state)
    funcs.append(json_2_cdf)
    funcs.append(cdf_2_probe)
    funcs.append(update_state_group)
    #Clean Up
    if not options.keep:
        funcs.append(rm_json_in)
        funcs.append(rm_json_state)
        funcs.append(rm_nc_in)
    ret = funcs.exec_chain(params)
    
    logg.write_debug(ret)
    
    exit_code = int(not ret)
    logg.write_ending(exit_code)
    exit(exit_code)

if __name__ == "__main__":
    main()
