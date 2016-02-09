#include <iostream>
#include <time.h>
#include <ctime>
#include <ctype.h>
#include <dirent.h>
#include <log/log.hh>
#include "StatUtils.hh"

StatUtils::StatUtils(Log *plog)
{
  log = plog;
}

StatUtils::~StatUtils()
{
}

int StatUtils::get_site_info(char *fname, vector<site *>* sites)
{  
  Site_list *siteList;
  
  int ret;
  int i;
  long num_sites;
  char *icao_id_str;
  char *long_name_str;
  site_loc site_info;

  siteList = new Site_list(fname);
  if(siteList->error()){
    log->write_time("Error: Unable to create Site_list object from %s\n", fname);
    return -1;
  }
  
  num_sites = siteList->get_num_sites();
  
  for(i = 0; i < num_sites; i++){
  
    site *site_data = new site;
  
    ret = siteList->site(i, &site_info);
    if(ret){
      log->write_time("Error: Unable to get site_info for site %d\n", i);
      return -1;
    }
    
    site_data->dicast_id = site_info.id;
    site_data->wmo_id = site_info.wmo_id;
    
    icao_id_str = strdup(site_info.icao_id);
    site_data->icao_id = icao_id_str;
    
    site_data->lat = site_info.lat;
    site_data->lon = site_info.lon;
    site_data->elev = site_info.alt;
    
    long_name_str = strdup(site_info.long_name);
    site_data->long_name = long_name_str;
    
    (*sites).push_back(site_data);
  }

  // Clean up
  delete siteList;
  
  return 0;
}


int StatUtils::get_site_list_ind(ncfc_io *data, int dicast_id)
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "site_list") == 0 ){
      for(int j=0; j<data->var_sizes[i]; j++){
	if(((int *)data->var_ptrs[i])[j] == dicast_id){
	  ind = j;
	  break;
	}
      } // end loop over values
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::get_stn_name_ind(ncfc_io *data, char *stn_name)
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "stn_name") == 0 ){
      int num_sites = data->var_dims[i][0];
      int stn_name_len = data->var_dims[i][1];
      for(int j=0; j<num_sites; j++){
	string id_str(&(((char *)data->var_ptrs[i])[j*stn_name_len]), stn_name_len);
	if(strcmp(id_str.c_str(), stn_name) == 0){
	  ind = j;
	  break;
	}
      } // end loop over num_sites
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::get_StationName_ind(ncfc_io *data, char *stn_name)
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "StationName") == 0 ){
      int num_sites = data->var_dims[i][0];
      int stn_name_len = data->var_dims[i][1];
      for(int j=0; j<num_sites; j++){
	string id_str(&(((char *)data->var_ptrs[i])[j*stn_name_len]), stn_name_len);
	if(strcmp(id_str.c_str(), stn_name) == 0){
	  ind = j;
	  break;
	}
      } // end loop over num_sites
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::get_stn_id_ind(ncfc_io *data, char *stn_id)
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "stn_id") == 0 ){
      int num_sites = data->var_dims[i][0];
      int stn_id_len = data->var_dims[i][1];
      for(int j=0; j<num_sites; j++){
	string id_str(&(((char *)data->var_ptrs[i])[j*stn_id_len]), stn_id_len);
	if(strcmp(id_str.c_str(), stn_id) == 0){
	  ind = j;
	  break;
	}
      } // end loop over num_sites
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::get_site_id_ind(ncfc_io *data, char *site_id)
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "site_id") == 0 ){
      int num_sites = data->var_dims[i][0];
      int site_id_len = data->var_dims[i][1];
      for(int j=0; j<num_sites; j++){
	string id_str(&(((char *)data->var_ptrs[i])[j*site_id_len]), site_id_len);
	if(strcmp(id_str.c_str(), site_id) == 0){
	  ind = j;
	  break;
	}
      } // end loop over num_sites
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::get_wmoId_ind(ncfc_io *data, int wmo_id) // for syn_proc
{
  int ind = MISSING;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if( strcmp(data->var_names[i].c_str(), "wmoId") == 0 ){
      for(int j=0; j<data->var_sizes[i]; j++){
	if(((int *)data->var_ptrs[i])[j] == wmo_id){
	  ind = j;
	  break;
	}
      } // end loop over values
    }
  } // end loop over variable names
  
  return ind;
}


int StatUtils::is_dir(char *dir_path){
  DIR *dir = opendir(dir_path);
  if(dir == NULL){
    //log->write_time("The directory %s does not exist\n", dir_path);
    return 0;
  } else{
    return 1;
  }
}


int StatUtils::get_dir_ent(char *dir_path, vector<char *>* entries){
  
  char *file;
  char *file_str;
  
  DIR *dir = opendir(dir_path);
  if(dir == NULL){
    log->write_time("The directory %s does not exist\n", dir_path);
    return -1;
  }
  
  struct dirent *entry;
  
  while((entry = readdir(dir)) != NULL){
    file = entry->d_name;
    
    //if(isalpha(file[0])){ 
    //  file_str = strdup(file);
    //  entries.push_back(file_str);
    //}
    
    file_str = strdup(file);
    (*entries).push_back(file_str);
  }
  
  return 0;
}


time_t StatUtils::date2utime(char *date)
{
  struct tm ct;
  
  int ret = sscanf(date, "%4d%02d%02d", &ct.tm_year, &ct.tm_mon, &ct.tm_mday);
  if(ret != 3){
    log->write_time("Error: invalid date: %s, format must be YYYYMMDD\n", date);
    return -1;
  }
  
  ct.tm_year -= 1900;
  ct.tm_mon -= 1;
  ct.tm_hour = 0;
  ct.tm_min = 0;
  ct.tm_sec = 0;
  
  time_t utime = timegm(&ct);
  return utime;
}

void StatUtils::utime2date(time_t utime, char** date)
{
  *date = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*date, "%4d%02d%02d", ct->tm_year, ct->tm_mon, ct->tm_mday);
}

void StatUtils::utime2year(time_t utime, char** year)
{
  *year = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*year, "%4d", ct->tm_year);
}

void StatUtils::utime2month(time_t utime, char** month)
{
  *month = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*month, "%02d", ct->tm_mon);
}

void StatUtils::utime2day(time_t utime, char** day)
{
  *day = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*day, "%02d", ct->tm_mday);
}

void StatUtils::utime2hour(time_t utime, char** hour)
{
  *hour = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*hour, "%02d", ct->tm_hour);
}

void StatUtils::utime2minute(time_t utime, char** minute)
{
  *minute = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*minute, "%02d", ct->tm_min);
}

void StatUtils::utime2hhmm(time_t utime, char** hhmm)
{
  *hhmm = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*hhmm, "%02d%02d", ct->tm_hour, ct->tm_min);
}

void StatUtils::utime2FileDate(time_t utime, char** date)
{
  *date = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*date, "%4d%02d%02d%02d", ct->tm_year, ct->tm_mon, ct->tm_mday, ct->tm_hour);
}
