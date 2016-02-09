#!/usr/bin/env python
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
# ** Copyright UCAR (c) 1992 - 2015 
# ** University Corporation for Atmospheric Research(UCAR) 
# ** National Center for Atmospheric Research(NCAR) 
# ** Research Applications Laboratory(RAL) 
# ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
# ** See LICENCE.TXT if applicable for licence details 
# ** 2015/02/02 20:17:38 
# *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

import fname
import os, sys
import sys_path
import rctm_path
import time
import tim
import get_file
import index_file


def get_obs_file(int_obs_dir, date_string):
    """ Get obs file corresponding to date_string"""
    obs_fno = fname.daily_fname_obj(sys_path.Int_obs_base, "", sys_path.NC_SUFFIX)
    
    obs_file = "%s/%s/%s" % (int_obs_dir, date_string, obs_fno.make_name(date_string))

    return obs_file
    

def get_concat_meso_file(concat_meso_dir, date_string):

    ttup = time.strptime(date_string, "%Y%m%d.%H%M%S")
    time_val = tim.mkgmtime(ttup)
    
    index = index_file.Index_file(concat_meso_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)
    f, d = get_file.get_file(index, time_val-(12*3600), time_val, 1, 1)

    if (len(f) == 0):
        obs_file = "none"
    else:
        obs_file = f[0]

    return obs_file
    

# mk_obs_conf creates obs config file located in path, using unix time utime
# and consisting of at most max_files.

def mk_obs_conf(int_obs_dir, out_dir, utime, max_files):
    
    index = index_file.Index_file(int_obs_dir, sys_path.Index_base, sys_path.Index_period, "r", sys_path.Index_wait)

    obs_list, date_list = get_file.get_file(index, utime - max_files * 86400, utime, get_file.LAST, max_files)

    if len(obs_list) == 0:
        return ""

    obs_base = "obs_conf"
    curr_date = time.strftime("%Y%m%d", time.gmtime(utime))
    curr_time = time.strftime("%H%M", time.gmtime(utime))
    conf_file = "%s/%s.%s.%s.%s" % (out_dir, obs_base, curr_date, curr_time, sys_path.ASC_SUFFIX)

    try:
        if os.path.isdir(out_dir) == 0:
            os.makedirs(out_dir)

        f = open(conf_file, "w")

        for file in obs_list:
            f.write(file)
            f.write("\n")

        f.close()
        return conf_file

    except:
        return ""
  

if __name__ == "__main__":
    import time
    curr_time = time.time()
    tstring = time.strftime("%Y%m%d.%H%M%S", time.gmtime(curr_time))
    date_string = tstring[:8]
    time_string = tstring[9:11]
    
    print "current obs file: ", get_obs_file(date_string)
    print "config file, last 10 days:", mk_obs_conf("/tmp", curr_time, 10)
    print "config file, null test:", mk_obs_conf("/tmp", curr_time+10*86400, 5)

