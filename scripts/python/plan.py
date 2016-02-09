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


import sys,os
import string
import time
import index_file
import sys_path
import rctm_path

# mk_current_plan(base_dir)
#
#  Creates a current plan file which consists of all the individual site
#  plan files concatenated together. "base_dir" is the directory containing
#  the site_plans and current_plan directories.
#

def mk_current_plan(base_dir, utime = time.time()):

    in_dir = "%s/%s" % (base_dir, rctm_path.Site_plans_base)
    out_base = rctm_path.Current_plan_base
    out_dir = "%s/%s" % (base_dir, out_base)

    # list input dir, find files that contain pattern, make list.
    cur_str = "curr_"
    files = os.listdir(in_dir)
    for f in files:
        if string.find(f, cur_str) == -1:
            files.remove(f)

    if len(files) == 0:
        return 1

    files.sort()
    #print files
    
    #utime = time.time()
    date_str = time.strftime("%Y%m%d", time.gmtime(utime))
    time_str = time.strftime("%H%M%S", time.gmtime(utime))

    dest_dir = "%s/%s" % (out_dir, date_str)
    out_file = "%s.%s.%s.%s" % (out_base, date_str, time_str, sys_path.ASC_SUFFIX)
    out_path = "%s/%s" % (dest_dir, out_file)

    #print dest_dir
    # Create destination directory as needed
    if os.path.isdir(dest_dir) == 0:
        try:
            os.makedirs(dest_dir)
        except:
            return 1

    # Concatenate all the files
    file_str = ""
    for f in files:
        file_str = "%s/%s" % (in_dir, f)   
        command = "cat %s >> %s" % (file_str, out_path)
        ret = os.system(command)
        if (ret != 0):
            return 1

    # Create index entry
    index = index_file.Index_file(out_dir, sys_path.Index_base, sys_path.Index_period, "w", sys_path.Index_wait)
    out_line = "%s %d" % (out_file, utime)
    if (index != ""):
        index.write(out_line, utime)
    
    return 0


# mk_def_plan() takes a site list file and base directory and
# creates a default plan file if one does not already exist. This
# just ensures that all the sites in the site list have a default 
# plan. 

def mk_def_plan(base_dir, site_list):

    plan_dir = "%s/%s" % (base_dir, rctm_path.Site_plans_base)

    if os.path.isdir(plan_dir) == 0:
        try:
            os.makedirs(plan_dir)
        except:
            return 1

    sfile = open(site_list, "r")
    site_lines = sfile.readlines()
    sfile.close()

    for line in site_lines:

        if line[0] == "#" or line[0] == " ":
            continue

        site_id = line[:8]
        plan_file = "%s/curr_%s.asc" % (plan_dir, site_id)
        
        if not os.path.exists(plan_file):
            mk_site_plan(plan_dir, site_id, "%s\n0\n" % (site_id))


    return 0
#
# mk_site_plan() takes a site_id and treatment plan string (which includes the
# site_id) and creates a site_plan file in the base_dir directory.
#

def mk_site_plan(dir, site_id, plan_str):

    plan_path = "%s/curr_%s.asc" % (dir, site_id)
    plan_file = open(plan_path, "w")
    plan_file.write(plan_str)
    plan_file.close()



if __name__ == '__main__':

    tmp_dir = "/tmp"
    site_list = "%s/%s" % (rctm_path.Site_list_dir, "road_cond_sites.asc")

    mk_def_plan(tmp_dir, site_list)
    ret = mk_current_plan(tmp_dir)

    print "ret=",ret
