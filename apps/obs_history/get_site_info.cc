// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:22:27 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <log/log.hh>
#include <site_list/site_list.hh>
#include "obs_history.h"

extern Log *logfile;

int get_site_info(vector<site *> &sites, char *fname){
  
  Site_list *siteList;

  int ret;
  int i;
  long num_sites;
  int site_offset;
  char *icao_id_str;
  char *long_name_str;
  site_loc site_info;

  siteList = new Site_list(fname);
  if(siteList->error()){
    logfile->write_time("Error: Unable to create Site_list object from %s\n", fname);
    return(1);
  }
  
  num_sites = siteList->get_num_sites();
  
  for(i = 0; i < num_sites; i++){
  
    site *site_data = new site;
  
    ret = siteList->site(i, &site_info);
    if(ret){
      logfile->write_time("Error: Unable to get site_info for site %d\n", i);
      return(1);
    }
    
    site_data->dicast_id = site_info.id;
    
    icao_id_str = strdup(site_info.icao_id);
    site_data->icao_id = icao_id_str;
    
    site_data->lat = site_info.lat;
    site_data->lon = site_info.lon;
    
    long_name_str = strdup(site_info.long_name);
    site_data->long_name = long_name_str;
    
    //cout << "site_data->long_name = " << site_data->long_name << endl;
    
    sites.push_back(site_data);
  }

  return(0);
}
