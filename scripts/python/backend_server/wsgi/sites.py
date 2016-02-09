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

STANDARD_SITE_LIST_LEN = 10
EXTENDED_STANDARD_SITE_LIST_LEN = 11

class Site(object):

    # Note that site_type is either SEGMENT or OBS_SITE
    def __init__(self, site_num, site_id, lat, lon, desc, site_type):
        self.site = {"site_num" : site_num, "site_id" : site_id, "lat" : lat, "lon" : lon, "desc" : desc, "site_type" : site_type}
        
    def get_dict(self):
        return self.site

def get_sites(fpath):
    """Gets site information from a DICast site file"""
    sites = []
    for line in file(fpath):
        spl = line.strip().split(";")
        if spl[0] == "site_num":
            # skip csv header
            continue
        if len(spl) == STANDARD_SITE_LIST_LEN:
            (site_num, n, site_id, lat, lon, elev, n, desc, state, co) = spl
            site_type = ""
        elif len(spl) == EXTENDED_STANDARD_SITE_LIST_LEN:
            (site_num, n, site_id, lat, lon, elev, n, desc, state, co, site_type) = spl
        else:
            return []
        new_site = Site(int(site_num), site_id, float(lat), float(lon), desc, site_type)
        sites.append(new_site.get_dict())
    return sites

def get_site_dict(sites):
    all_sites = {}
    for site in sites:
        site_num = site["site_num"]
        all_sites[site_num] = site
    return all_sites

def get_rdwx_sites(cf):
    return get_site_dict(get_sites(cf.all_sites_file))

def get_road_cond_sites(cf):
    return get_site_dict(get_sites(cf.road_cond_sites_file))

def get_wx_obs_sites(cf):
    return get_site_dict(get_sites(cf.wx_obs_sites_file))

def get_all_site_dict(all_sites_file):
    all_site_dict = get_site_dict(get_sites(all_sites_file))
    return all_site_dict

def get_rwis_site_dict(all_site_dict):
    
    rwis_site_dict = {}
    for site_num in all_site_dict:
        site_dict = all_site_dict[site_num]
        if site_dict["site_type"] == "OBS_SITE":
            rwis_site_dict[site_num] = site_dict
    return rwis_site_dict

def get_road_segment_site_dict(all_site_dict):

    road_segment_site_dict = {}
    for site_num in all_site_dict:
        site_dict = all_site_dict[site_num]
        if site_dict["site_type"] == "SEGMENT":
            road_segment_site_dict[site_num] = site_dict
    return road_segment_site_dict

def get_rwis_sites(cf):
    all_site_dict = get_site_dict(get_sites(cf.rwis_sites_file))
    
    rwis_site_dict = {}
    for site_num in all_site_dict:
        site_dict = all_site_dict[site_num]
        if site_dict["site_type"] == "OBS_SITE":
            rwis_site_dict[site_num] = site_dict
    return rwis_site_dict

def get_road_segment_sites(cf):
    all_site_dict = get_site_dict(get_sites(cf.rwis_sites_file))
    road_segment_site_dict = {}
    for site_num in all_site_dict:
        site_dict = all_site_dict[site_num]
        if site_dict["site_type"] == "SEGMENT":
            road_segment_site_dict[site_num] = site_dict
    return road_segment_site_dict

def main():
    import backend_sys_path
    cf = backend_sys_path.State_dictionary["minnesota_vdt"]
    #print cf.rwis_sites_file
    all_sites_dict = get_all_site_dict(cf.rwis_sites_file)
    #print all_sites_dict

    #print get_road_segment_site_dict
    #print get_rdwx_sites(cf)
    #print get_road_cond_sites(cf)
    #print get_wx_obs_sites(cf)
    #print get_rwis_sites(cf)
    #print get_road_segment_sites(cf)

if __name__ == "__main__":
    main()
    
