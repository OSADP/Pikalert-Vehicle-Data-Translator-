// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:22:27 
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
#include "obs_history.h"

extern Log *logfile;

int combine_data(int num_out_times, double start_time, double cur_time, ds *fcst_data, ds *meso_data, ds *out_data){
  
  double utime;
  int s, t;
  int ind;
  
  // Set output obs_time
  out_data->obs_time = start_time;
  
  // Set output num_sites and dicast_ids
  
  if( (meso_data->is_valid) && (fcst_data->is_valid) ){
    logfile->write_time(2, "Info: Both meso data and fcst data are not missing.\n");
    out_data->num_sites = meso_data->num_sites;
    for(s = 0; s < meso_data->num_sites; s++){
      out_data->dicast_id[s] = meso_data->dicast_id[s];
    } // end loop over sites
  } else if(! fcst_data->is_valid){
    logfile->write_time(2, "Info: Fcst data is missing.\n");
    out_data->num_sites = meso_data->num_sites;
    for(s = 0; s < meso_data->num_sites; s++){
      out_data->dicast_id[s] = meso_data->dicast_id[s];
    } // end loop over sites
  } else{
    logfile->write_time(2, "Info: Meso data is missing.\n");
    out_data->num_sites = fcst_data->num_sites;
    for(s = 0; s < fcst_data->num_sites; s++){
      out_data->dicast_id[s] = fcst_data->dicast_id[s];
    } // end loop over sites
  }
  
  if(out_data->num_sites == MISSING){
    logfile->write_time("Error: out_data->num_sites is missing\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  // Set output time_nominal
  utime = start_time;
  for(t = 0; t < num_out_times; t++){
    out_data->time_nominal[t] = utime;
    utime = utime + 3600;
  }
  
  double diff = (cur_time - start_time) / 3600;
  int lead_ind = (int)diff;
  
  // Fill output data arrays 
  for(s = 0; s < out_data->num_sites; s++){
    
    int site_ind = s*num_out_times;
    
    //
    // Check if the site reports road-T / bridge-T / subsurface-T
    //
    int ind1 = site_ind + lead_ind;
    int ind2 = site_ind + lead_ind - 1;
    int ind3 = site_ind + lead_ind - 2;
    
    // For latent reporting sites, if we have an ob within 2 hours of the current hour
    // copy this ob foward to the missing hours
    // Do road_T
    if(meso_data->road_T[ind1] == MISSING && meso_data->road_T[ind2] != MISSING){
      meso_data->road_T[ind1] = meso_data->road_T[ind2];
    }
    if(meso_data->road_T[ind1] == MISSING && meso_data->road_T[ind2] == MISSING &&  meso_data->road_T[ind3] != MISSING){
      meso_data->road_T[ind1] = meso_data->road_T[ind3];
      meso_data->road_T[ind2] = meso_data->road_T[ind3];
    }
    // Do bridge_T
    if(meso_data->bridge_T[ind1] == MISSING && meso_data->bridge_T[ind2] != MISSING){
      meso_data->bridge_T[ind1] = meso_data->bridge_T[ind2];
    }
    if(meso_data->bridge_T[ind1] == MISSING && meso_data->bridge_T[ind2] == MISSING &&  meso_data->bridge_T[ind3] != MISSING){
      meso_data->bridge_T[ind1] = meso_data->bridge_T[ind3];
      meso_data->bridge_T[ind2] = meso_data->bridge_T[ind3];
    }
    // Do subsurface_T
    if(meso_data->subsurface_T[ind1] == MISSING && meso_data->subsurface_T[ind2] != MISSING){
      meso_data->subsurface_T[ind1] = meso_data->subsurface_T[ind2];
    }
    if(meso_data->subsurface_T[ind1] == MISSING && meso_data->subsurface_T[ind2] == MISSING &&  meso_data->subsurface_T[ind3] != MISSING){
      meso_data->subsurface_T[ind1] = meso_data->subsurface_T[ind3];
      meso_data->subsurface_T[ind2] = meso_data->subsurface_T[ind3];
    }
    
    for(t = 0; t < num_out_times; t++){
      
      ind = site_ind + t;
      
      // Set T
      //printf("out_data->dicast_id[%d]=%d, out_data->time_nominal[%d]=%.0f", s, out_data->dicast_id[s], t, out_data->time_nominal[t]);
      if(meso_data->T[ind] == MISSING){
	out_data->T[ind] = fcst_data->T[ind];
	//printf(" using FCST data: out_data->T[%d]=%f\n", ind, out_data->T[ind]);
      } else{
	out_data->T[ind] = meso_data->T[ind];
	//printf(" using MESO data: out_data->T[%d]=%f\n", ind, out_data->T[ind]);
      }
      
      // Set dewpt
      if(meso_data->dewpt[ind] == MISSING){
	out_data->dewpt[ind] = fcst_data->dewpt[ind];
      } else{
	out_data->dewpt[ind] = meso_data->dewpt[ind];
      }
      
      // Set wind_speed
      if(meso_data->wind_speed[ind] == MISSING){
	out_data->wind_speed[ind] = fcst_data->wind_speed[ind];
      } else{
	out_data->wind_speed[ind] = meso_data->wind_speed[ind];
      }
      
      // Set Precip
      if(meso_data->Precip[ind] == MISSING){
	out_data->Precip[ind] = fcst_data->Precip[ind];
      } else{
	out_data->Precip[ind] = meso_data->Precip[ind];
      }
      
      // Set road_T
      // Do not use previous fcst data
      out_data->road_T[ind] = meso_data->road_T[ind];
      //printf(" using MESO data: out_data->road_T[%d]=%f\n", ind, out_data->road_T[ind]);
      
      // Set bridge_T
      // Do not use previous fcst data
      out_data->bridge_T[ind] = meso_data->bridge_T[ind];
      //printf(" using MESO data: out_data->bridge_T[%d]=%f\n", ind, out_data->bridge_T[ind]);
      
      // Set subsurface_T
      // Do not use previous fcst data
      out_data->subsurface_T[ind] = meso_data->subsurface_T[ind];
      //printf(" using MESO data: out_data->subsurface_T[%d]=%f\n", ind, out_data->subsurface_T[ind]);
      
      // Set road_condition
      if(meso_data->road_condition[ind] == MISSING){
	out_data->road_condition[ind] = fcst_data->road_condition[ind];
      } else{
	out_data->road_condition[ind] = meso_data->road_condition[ind];
      }
      
      // Set T_lbls0 (this is a forecast variable only)
      out_data->T_lbls0[ind] = fcst_data->T_lbls0[ind];
      
      // Set T_lbls1 (this is a forecast variable only)
      out_data->T_lbls1[ind] = fcst_data->T_lbls1[ind];
      
    } // end loop over obs times
  } // end loop over sites
  
  delete [] fcst_data;
  delete [] meso_data;
 
  return(0);
}
