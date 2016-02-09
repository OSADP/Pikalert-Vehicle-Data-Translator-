// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:07 
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

int dump_fcst_data(int num_files, vector<char *> &variables, vector<char *> &output_vars, vector<site *> &sites, fs *fcst_data, char *outputFileName, int fcst_hour){
  
  int ret;
  int ind;
  int vector_ind;
  int site_ind;
  int v, mv, f, s, ss;
  int d, ftpd;
  int flag;
  
  time_t init_time;
  time_t cur_time;
  time_t obs_time;
  time_t output_obs_time;
  
  int *fcst_site_ind[num_files];
  
  vector <float> fcst;
  
  
  ////////////////////////////////////////////////////////////////////
  // Get fcst files' site indicies
  for(f = 0; f < num_files; f++){  
    fcst_site_ind[f] = new int[sites.size()];
    
    if(fcst_data[f].is_valid == 0){
      
      for(s = 0; s < sites.size(); s++){
	fcst_site_ind[f][s] = MISSING;
	for(ss = 0; ss < fcst_data[f].num_sites; ss++){
	  if(sites[s]->dicast_id == fcst_data[f].site_list[ss]){
	    fcst_site_ind[f][s] = ss;
	    break;
	  }
	}
      }
    
    } else{
      
      for(s = 0; s < sites.size(); s++){
	fcst_site_ind[f][s] = MISSING;
      }
    
    }
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Set days and fc_times_per_day for each variable

  int days[variables.size()];
  int fc_times_per_day[variables.size()];

  ind = 0;
  for(v = 0; v < variables.size(); v++){
    flag = 0;
    for(f = 0; f < num_files; f++){
      if(fcst_data[ind].var_is_valid == 0 && flag == 0){
        days[v] = fcst_data[ind].dim[1];
        fc_times_per_day[v] = fcst_data[ind].dim[2];
        flag = 1;
      }
      ind++;
    }
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Put fcst data into a vector

  int break_point[variables.size() + 1];
  break_point[0] = 0;
  
  float fcst_used;
  int fcst_value_ind;
  
  ind = 0;
  vector_ind = 0;
  for(v = 0; v < variables.size(); v++){
    for(f = 0; f < num_files; f++){ 
      
      if(fcst_data[ind].var_is_valid == 0){
	
	for(s = 0; s < sites.size(); s++){
	  
	  site_ind = fcst_site_ind[f][s];
	  
	  logfile->write_time(4, "Info: site_ind=%d\n", site_ind);
	  if(site_ind == MISSING){
	    logfile->write_time(4, "Info: site %d not found in fcst file %d\n", sites[s]->dicast_id, f);
	    for(d = 0; d < days[v]; d++){
	      for(ftpd = 0; ftpd < fc_times_per_day[v]; ftpd++){
		fcst.push_back(MISSING);
		logfile->write_time(4, "Info: variables[%d]=%s, f=%d, dicast_ids[%d]=%d, d=%d, ftpd=%d, vector_ind=%d, fcst=%f\n", v, variables[v], f, s, sites[s]->dicast_id, d, ftpd, vector_ind, fcst[vector_ind]);
		vector_ind++;
	      }
	    }
	    continue;
	  }
	  
	  if(fcst_data[ind].num_dim == 4){
	    if(strcmp(variables[v], "road_subsurface_T") == 0){
	      fcst_value_ind = (site_ind*days[v]*fc_times_per_day[v] * fcst_data[ind].dim[3]) + 7;
	    } else{
	      fcst_value_ind = site_ind*days[v]*fc_times_per_day[v] * fcst_data[ind].dim[3];
	    }
	  } else{
	    fcst_value_ind = site_ind*days[v]*fc_times_per_day[v];
	  }
	  
	  for(d = 0; d < days[v]; d++){
	    for(ftpd = 0; ftpd < fc_times_per_day[v]; ftpd++){    
	      
	      fcst_used = fcst_data[ind].var_ptrs[fcst_value_ind];	      
	      fcst.push_back(fcst_used);
	      
	      logfile->write_time(4, "Info: variables[%d]=%s, f=%d, ind=%d, dicast_ids[%d]=%d, d=%d, ftpd=%d, vector_ind=%d, fcst=%f\n", v, variables[v], f, ind, s, sites[s]->dicast_id, d, ftpd, vector_ind, fcst[vector_ind]);
	      
	      if(fcst_data[ind].num_dim == 4){
		fcst_value_ind = fcst_value_ind + fcst_data[ind].dim[3];
	      } else{
		fcst_value_ind++;
	      } 
	      vector_ind++;
	    }  
	  }
	  
	}
	
      } else{
	
	for(s = 0; s < sites.size(); s++){
	  for(d = 0; d < days[v]; d++){
	    for(ftpd = 0; ftpd < fc_times_per_day[v]; ftpd++){ 
	      fcst.push_back(MISSING);
	      logfile->write_time(4, "Info: variables[%d]=%s, f=%d, dicast_ids[%d]=%d, d=%d, ftpd=%d, vector_ind=%d, fcst=%f\n", v, variables[v], f, s, sites[s]->dicast_id, d, ftpd, vector_ind, fcst[vector_ind]);
	      vector_ind++;
	    }
	  }
	}
	
      }
      
      ind++;
    }
    
    break_point[v+1] = vector_ind;
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Dump fcst data
  
  ofstream of(outputFileName, ios::out);
  
  for(s = 0; s < sites.size(); s++){
    
    for(v = 0; v < variables.size(); v++){
      
      for(f = 0; f < num_files; f++){
	
	time_t forc_time = (time_t)fcst_data[f].forc_time;
	
	init_time = 0;
	struct tm *ct;
	ct = gmtime(&init_time);
        ct = localtime(&forc_time);
        ct->tm_year += 1900;
        ct->tm_mon += 1;
	
	char gen_date[MAX_STRING_LEN];
        sprintf(gen_date, "%4d%02d%02d", ct->tm_year, ct->tm_mon, ct->tm_mday);
        //cout << "gen_date: " << gen_date << endl;
	
	//int forc_run_time = ct->tm_hour;
	int forc_run_time = fcst_hour;
	//cout << "forc_run_time: " << forc_run_time << endl;
	
	int hind_cast_limit;
	if(forc_run_time == 0 || fc_times_per_day[v] == 1){
	  hind_cast_limit = 0;
	} else if(fc_times_per_day[v] == 4){
	  if(forc_run_time == 3 || forc_run_time == 6){
	    hind_cast_limit = 1;
	  }
	  if(forc_run_time == 9 || forc_run_time == 12){
	    hind_cast_limit = 2;
	  }
	  if(forc_run_time == 15 || forc_run_time == 18){
	    hind_cast_limit = 3;
	  }
	  if(forc_run_time == 21){
	    hind_cast_limit = 4;
	  }
	} else if(fc_times_per_day[v] == 8){
	  hind_cast_limit = forc_run_time / 3;
	} else {
	  hind_cast_limit = forc_run_time;
	}
	//cout << "hind_cast_limit: " << hind_cast_limit << endl;
	
	ct->tm_year -= 1900;
        ct->tm_mon -= 1;
	ct->tm_hour = 0;
	ct->tm_min = 0;
	ct->tm_sec = 0;
	
	cur_time = mktime(ct);
	//cout << "cur_time: " << cur_time << endl;
	
	int lead_time = 0;
	int cnt = 0;
	
	obs_time = cur_time;
	
	for(d = 0; d < days[v]; d++){
	  
	  struct tm *ot;
          ot = gmtime(&init_time);
          ot = localtime(&obs_time);
          ot->tm_year += 1900;
          ot->tm_mon += 1;
	  
	  char fcst_date[MAX_STRING_LEN];
          sprintf(fcst_date, "%4d%02d%02d", ot->tm_year, ot->tm_mon, ot->tm_mday);
          //cout << "fcst_date: " << fcst_date << endl;
	  
	  for(ftpd = 0; ftpd < fc_times_per_day[v]; ftpd++){  
	    
	    //cout << "cnt: " << cnt << endl;
	    
	    vector_ind = break_point[v] + (s * days[v] * fc_times_per_day[v]) + (f * sites.size() * days[v] * fc_times_per_day[v]) + cnt;
	    
	    if(fcst[vector_ind] <= MISSING || fcst[vector_ind] >= MISSING*(-1)){
	      fcst[vector_ind] = MISSING;
	    }	    
	    
	    logfile->write_time(4, "Info: sites[%d]=%d, variables[%d]=%s, f=%d, d=%d, ftpd=%d, fcst[%d]=%f\n", s, sites[s]->dicast_id, v, variables[v], f, d, ftpd, vector_ind, fcst[vector_ind]);
	    
	    int hour_out;
	    int valid_time;
	    if(fc_times_per_day[v] == 1){
	      hour_out = d;
	      valid_time = 0;
	    } else if(fc_times_per_day[v] == 4){
	      hour_out = lead_time * 6;
	      valid_time = ftpd * 6;
	    } else if(fc_times_per_day[v] == 8){
	      hour_out = lead_time * 3;
	      valid_time = ftpd * 3;
	    } else{
	      hour_out = lead_time;
	      valid_time = ftpd;
	    }
	    
	    output_obs_time = obs_time + (valid_time * 3600);
	    
	    logfile->write_time(3, "%8d %-25s %8s %8s %02d %10.4f\n", sites[s]->dicast_id, output_vars[v], gen_date, fcst_date, valid_time, fcst[vector_ind]);
	    
	    char of_str[MAX_STRING_LEN];
	    sprintf(of_str, "%8d %-25s %12d %10.4f", sites[s]->dicast_id, output_vars[v], (int)output_obs_time, fcst[vector_ind]);
	    
	    if(d == 0 && ftpd >= hind_cast_limit){
	      if(hour_out < 48){
		of<<of_str<<'\n';
	      }
	      lead_time++;
	    }
	    
	    if(d > 0){
	      if(hour_out < 48){
		of<<of_str<<'\n';
	      }
	      lead_time++;
	    }
	    
	    cnt++;
	  }
	  obs_time += 86400;
	}
      }
    }
  }
  
  
  return(0);
}
  
