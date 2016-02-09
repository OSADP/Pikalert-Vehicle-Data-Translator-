// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:20 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <vector>
#include <map>
#include <nxncf/nxncf.hh>
#include <log/log.hh>
#include "data.h"

extern Log *logfile;

int dump_meso_data(int num_files, vector<char *> &variables, vector<site *> &sites, vector <ls *> &site_var_list, os *obs_data, char *outputFileName);

int dump_meso_data(int num_files, vector<char *> &variables, vector<site *> &sites, vector <ls *> &site_var_list, os *obs_data, char *outputFileName){
  
  int ret;
  int ind;
  int vector_ind;
  int slen;
  int site_ind;
  int v, mv, f, s, sv, ss;
  int d, ftpd;
  int flag;
  
  int *obs_site_ind[num_files];
  double *obs_times[num_files];
  
  vector <float> obs;
  
  ////////////////////////////////////////////////////////////////////
  // Get meso files' site indicies and obs_times
  for(f = 0; f < num_files; f++){
    
    obs_site_ind[f] = new int[sites.size()];
    obs_times[f] = new double[sites.size()];

    if(obs_data[f].is_valid == 0){  
      
      for(s = 0; s < sites.size(); s++){
	
	slen = 0;
	slen = strlen(sites[s]->icao_id);	
	
	obs_site_ind[f][s] = MISSING;
	obs_times[f][s] = MISSING;
	
	if(slen < 6){
	  
	  for(ss = 0; ss < obs_data[f].num_obs; ss++){
	    if(strcmp(sites[s]->icao_id, &obs_data[f].station_ids[ss*obs_data[f].stnid_len]) == 0){
	      obs_site_ind[f][s] = ss;
	      break;
	    }
	  }
	  
	} else{
	  
	  for(ss = 0; ss < obs_data[f].num_obs; ss++){
	    if(strncmp(sites[s]->icao_id, &obs_data[f].station_ids[ss*obs_data[f].stnid_len], slen) == 0){
	      obs_site_ind[f][s] = ss;
	      break;
	    }
	  }
	  
	} // end else
	
	if(obs_site_ind[f][s] != MISSING){
	  obs_times[f][s] = obs_data[f].obs_times[ss];
	} else{
	  obs_times[f][s] = MISSING;
	}
	
	//cout << "sites[" << s << "]->icao_id = " << sites[s]->icao_id << " obs_site_ind[" << f << "][" << s << "] = " << obs_site_ind[f][s] << " obs_times[" << f << "][" << s << "] = " << obs_times[f][s] << endl;
	

      } // end loop over sites
      
    } else{
      
      for(s = 0; s < sites.size(); s++){
        obs_site_ind[f][s] = MISSING;
        obs_times[f][s] = MISSING;
	//cout<<"obs_site_ind["<<f<<"]["<<s<<"] = "<<obs_site_ind[f][s]<<'\n';
      }

    }
  } // end loop over files
  
  
  ////////////////////////////////////////////////////////////////////
  // Put meso data into a vector

  int meso_break_point[variables.size() + 1];
  meso_break_point[0] = 0;
  
  float obs_used;
  double obs_time_used;

  int v_ind = 0;
  vector_ind = 0;
  for(v = 0; v < variables.size(); v++){
    
    v_ind = v * num_files;
    
    int obs_ind;
    
    for(f = 0; f < num_files; f++){ 
      
      obs_ind = v_ind + f;
      
      for(s = 0; s < sites.size(); s++){
	
	if(obs_data[obs_ind].var_is_valid == 0){ 
	  
	  site_ind = obs_site_ind[f][s];	  
	  int obs_value_ind = site_ind;
	  
	  if(site_ind == MISSING){
	    obs_used = MISSING;
	  } else if(obs_data[obs_ind].var_ptrs[obs_value_ind] == MISSING || obs_data[obs_ind].var_ptrs[obs_value_ind] >= FILL_VAL){
	    obs_used = MISSING;
	  } else if(strcmp(obs_data[obs_ind].units[0], "kelvin") == 0){
	    if(obs_data[obs_ind].var_ptrs[obs_value_ind] <= 223){
	      obs_used = MISSING;
	    } else{
	      obs_used = obs_data[obs_ind].var_ptrs[obs_value_ind] - 273.15;
	    }
	  } else{
	    obs_used = obs_data[obs_ind].var_ptrs[obs_value_ind];
	  }
	  obs.push_back(obs_used);
	  
	  logfile->write_time(4, "Info: variables[%d]=%s, f=%d, s=%d, seg_meso_map[%d]=%s, vector_ind=%d, obs=%f\n", v, variables[v], f, s, sites[s]->dicast_id, sites[s]->icao_id, vector_ind, obs[vector_ind]);
	  
	  vector_ind++;
	  
	} else{
	  obs.push_back(MISSING);
	  logfile->write_time(4, "Info: variables[%d]=%s, f=%d, s=%d, seg_meso_map[%d]=%s, vector_ind=%d, obs=%f\n", v, variables[v], f, s, sites[s]->dicast_id, sites[s]->icao_id, vector_ind, obs[vector_ind]);
	  vector_ind++; 
	}
	
      } 
    }
    
    meso_break_point[v+1] = vector_ind;
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Dump obs data

  ofstream of(outputFileName, ios::out);
  
  int meso_break_point_used;
  int meso_vector_ind;
  
  for(s = 0; s < sites.size(); s++){

    if(sites[s]->dicast_id != MISSING){
      
      for(v = 0; v < variables.size(); v++){
    
	char *output_var_name;
	if(strncmp(variables[v], "roadTemperature", 15) == 0){
	  if(sites[s]->roadTemp_type[v] == 0){
	    output_var_name = (char *)"roadTemperature";
	  } else if(sites[s]->roadTemp_type[v] == 1){
	    output_var_name = (char *)"bridgeTemperature";
	  }
	} else if(strncmp(variables[v], "roadSubsurfaceTemp", 18) == 0){
	  output_var_name = (char *)"roadSubsurfaceTemp";
	} else{
	  output_var_name = variables[v];
	}
	
	flag = 0;
	for(sv = 0; sv < site_var_list[s]->variables.size(); sv++){
	  if(strcmp(variables[v], site_var_list[s]->variables[sv]) == 0){
	    flag = 1;
	  }
	}
	
	for(f = 0; f < num_files; f++){
	  
	  //cout << "obs_times[" << f << "][" << s << "] = " << obs_times[f][s] << endl;
	  
	  if(obs_times[f][s] == MISSING || obs_times[f][s] >= FILL_VAL){
	    if(v == 0){
	      logfile->write_time(2, "Info: obs_time is MISSING for site[%d]: %s, file %d, skipping output for all variables\n", s, sites[s]->icao_id, f);
	    }
	    continue;
	  }
	  
	  time_t obs_time = (time_t)obs_times[f][s];
	  
	  struct tm *ot;
	  ot = localtime(&obs_time);
	  ot->tm_year += 1900;
	  ot->tm_mon += 1;
	  
	  char obs_date[MAX_STRING_LEN];
	  sprintf(obs_date, "%4d%02d%02d", ot->tm_year, ot->tm_mon, ot->tm_mday);
	  //cout << "obs_date: " << obs_date << endl;
	
	  char obs_hour[MAX_STRING_LEN];
	  sprintf(obs_hour, "%02d", ot->tm_hour);
	  //cout << "obs_hour: " << obs_hour << endl;
	  
	  ot->tm_year -= 1900;
	  ot->tm_mon -= 1;
	  ot->tm_min = 0;
	  ot->tm_sec = 0;
	  
	  time_t output_time = mktime(ot);
	  
	  int meso_vector_ind = meso_break_point[v] + s + (f * sites.size());
	  
	  if(obs[meso_vector_ind] <= MISSING || obs[meso_vector_ind] >= MISSING*(-1)){
	    obs[meso_vector_ind] = MISSING;
	  }
	  
	  if(sites[s]->dicast_id == 72469100){
	    obs[meso_vector_ind] = MISSING;
	  }
	
	  if(obs[meso_vector_ind] != MISSING){
	    // QC road temperature values
	    if(strncmp(variables[v], "roadTemperature", 15) == 0){
	      if(obs[meso_vector_ind] < MIN_ROAD_T || obs[meso_vector_ind] > MAX_ROAD_T){
		obs[meso_vector_ind] = MISSING;
	      }
	    }
	  
	    // QC road subsurface temperature values
	    if(strncmp(variables[v], "roadSubsurfaceTemp", 18) == 0){
	      if(obs[meso_vector_ind] < MIN_SUBSURFACE_T || obs[meso_vector_ind] > MAX_SUBSURFACE_T){
		obs[meso_vector_ind] = MISSING;
	      }
	    }
	  
	    // QC air temperature values
	    if(strncmp(variables[v], "temperature", 11) == 0){
	      if(obs[meso_vector_ind] < MIN_AIR_T || obs[meso_vector_ind] > MAX_AIR_T){
		obs[meso_vector_ind] = MISSING;
	      }
	    }
	  }
	
	  logfile->write_time(4, "Info: sites[%d]=%d, variables[%d]=%s, f=%d, obs[%d]=%f\n", s, sites[s]->dicast_id, v, variables[v], f, meso_vector_ind, obs[meso_vector_ind]);
	
	  char of_str[MAX_STRING_LEN];
	  sprintf(of_str, "%8d %-25s %12d %10.4f", sites[s]->dicast_id, output_var_name, (int)output_time, obs[meso_vector_ind]);
	  
	  if(flag){
	    of<<of_str<<'\n';
	    logfile->write_time(3, "%8d %-25s %8s %2s %10.4f\n", sites[s]->dicast_id, variables[v], obs_date, obs_hour, obs[meso_vector_ind]);
	  }
	  
	} // end loop over files
      } // end loop over variables
    }
  } // end loop over sites
  
  return(0);
}
