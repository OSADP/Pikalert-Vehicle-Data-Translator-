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

"""The meta data for a district contains an district_name and a lat-long
box having a min latitude, min longitude and a max
latitude, max longitude. A site is implemented using a dictionary and each site must
at least contain the keys "lat", "lon". Given a list of sites, and a district list, the
ClassifyIntoDistricts class will be able to group sites accorinding to
district"""


def in_district(lat, lon, district):
    """Return True or False depending on whether lat, lon is in district""" 
    if district["min_lat"] <= lat and lat <= district["max_lat"]:
        if district["min_lon"] <= lon and lon <= district["max_lon"]:
            return True
    return False    

class ClassifyIntoDistricts(object):

    def __init__(self, state, district_list, site_list):
        """Classify sites in site_list into the districts they fall into. The list, district_list, consists of a list of dictionaries each of which contains a district district_name with a lat-lon box. The list, site_list, consists of a list of sites. A site has to be implemented using a dictionary and must at least contain the keys "lat", "lon",  """

        self.state = state
        self.district_list = district_list
        self.district_dict_sites = {}
        self.district_dict = {}
        self.district_dict["districts"] = self.district_list
        for site in site_list:
            for ind in range(len(district_list)):
                if in_district(site["lat"], site["lon"], district_list[ind]):
                    district_name = district_list[ind]["district_name"]
                    if district_name in self.district_dict_sites:
                        self.district_dict_sites[district_name].append(site)
                    else:
                        self.district_dict_sites[district_name] = [site]

    def get_district_sites(self, district_name):
        """Gets list of sites for each district"""
        return self.district_dict_sites.get(district_name, [])

    def get_districts(self):
        return self.district_list
    
    def get_district_site_dict(self):
        return self.district_dict_sites

    def get_district_dict(self):
        return self.district_dict
    
    def get_state(self):
        return self.state
    
def main():

    #61.25;-149.80

    import backend_sys_path
    state = backend_sys_path.state
    district_list = backend_sys_path.districts
    site_list = [{"lat" : 5, "lon" : 5}, {"lat" : 15, "lon" : 25}, {"lat" : 50, "lon" : 60}, {"lat" : 55, "lon" : 75}]
    classify = ClassifyIntoDistricts(state, district_list, site_list)
    print classify.get_state()
    print classify.get_district_dict()
    """
    for district in classify.get_districts():
        print "district: ", district
    print "district1 sites: ", classify.get_district_sites("district1")
    print "district2 sites: ", classify.get_district_sites("district2")
    """

    import sites
    all_sites = sites.get_rdwx_sites()
    #print all_sites.values()
    classify = ClassifyIntoDistricts(state, district_list, all_sites.values())
    districts = classify.get_districts()

    #print classify.get_district_sites("district1")
    #print classify.get_district_dict()

if __name__ == "__main__":
    main()
