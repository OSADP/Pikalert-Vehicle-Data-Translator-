// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include "concat_meso_data.h"

//extern Log *logfile;
extern Log *proc_log;




int get_data(double start_time, int num_out_times, vector<site *> sites, char *inputFileName, ds *data){
  
  char var[MAX_STRING_LEN];
  char site_id_str[MAX_STRING_LEN];
  char time_str[MAX_STRING_LEN];
  char value_str[MAX_STRING_LEN];
  
  int line_cnt = 0;
  int site_id;
  double obs_time, diff;
  
  int s, t;
  int flag;
  float value;
  char *str;
  int lead_time;
  int ind, site_ind;
 
  vector<int> sites_found;
  
  FILE *fp;
  fp = fopen(inputFileName, "r");
  if(fp == NULL){
    proc_log->write_time("Error: Unable to open input file %s\n",inputFileName);
    proc_log->write_time("Ending. Exit status = 1.\n");
    exit(1);
  }
  
  // Get data from each line of the file
  while (!feof(fp)) {
    
    int ret = fscanf(fp, "%s %s %s %s", site_id_str, var, time_str, value_str);
    if(ret != 4){
      break;
    }
    
    site_id = atoi(site_id_str);
    obs_time = atof(time_str);
    value = atof(value_str);
    
    //printf("%8d %25s %12d %10.4f\n", site_id, var, (int)obs_time, value);
    
    diff = (obs_time - start_time) / 3600;
    lead_time = (int)diff;
    //printf("lead_time=%d\n", lead_time);
    
    // Check if the site is already found
    flag = 0;
    for(s = 0; s < sites_found.size(); s++){
      if(sites_found[s] == site_id){
	//cout << "site already exists" << endl;
	flag = 1;
	break;
      }
    } // end loop over site list
    
    // Get the site list index for new sites
    if(flag == 0){
      site_ind = MISSING;
      for(s = 0; s < sites.size(); s++){
	if(site_id == sites[s]->dicast_id){
	  //cout << "new site found" << endl;
	  site_ind = s;
	  sites_found.push_back(site_id);
	  break;
	}
	
      } // end loop over site list
    }
    
    //cout << "site_ind = " << site_ind << endl;
    
    // Get the values for each variable
    if(lead_time >= 0 && lead_time < num_out_times && site_ind != MISSING){
      
      // Set index for variable arrays
      ind = (site_ind*num_out_times) + lead_time;
      //printf("site_ind=%d, num_out_time=%d, lead_time=%d, ind=%d\n", site_ind, num_out_times, lead_time, ind);
      
      // Get temperature
      if(strcmp(var, "temperature") == 0){
	data->T[ind] = value;
	//printf("data->T[%d]=%f\n", ind, data->T[ind]);
      }
    
      // Get dewpoint
      if(strcmp(var, "dewpoint") == 0){
	data->dewpt[ind] = value;
      }
    
      // Get wind_speed
      if(strcmp(var, "windSpeed") == 0){
	data->wind_speed[ind] = value;
      }
    
      // Get Precip
      if(strcmp(var, "precipRate") == 0){
	if(value > 0){
	  data->Precip[ind] = 1;
	} else{
	  data->Precip[ind] = 0;
	}
      }
    
      // Get road_T
      if(strcmp(var, "roadTemperature") == 0){
	data->road_T[ind] = value;
      }
      
      // Get bridge_T
      if(strcmp(var, "bridgeTemperature") == 0){
	data->bridge_T[ind] = value;
      }
      
      // Get road_condition
      if(strcmp(var, "road_condition") == 0){
	data->road_condition[ind] = (int)value;
      }
    
      // Get subsurface_T
      if(strcmp(var, "roadSubsurfaceTemp") == 0){
	data->subsurface_T[ind] = value;
      }
      
      // Get T_lbls0
      if(strcmp(var, "soilTemperature") == 0){
	data->T_lbls0[ind] = value;
	//printf("data->T_lbls0[%d]=%f\n", ind, data->T_lbls0[ind]);
      }
      
      // Get T_lbls1
      if(strcmp(var, "soilSubsurfaceTemp") == 0){
	data->T_lbls1[ind] = value;
      }
      
    }
    
  } // End loop over lines in the file
  
  return(0);
}


