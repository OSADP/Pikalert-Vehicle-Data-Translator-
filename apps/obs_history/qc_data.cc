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

int qc_data(int num_out_times, ds *data){
  
  int s, t;
  int ind;
  
  // QC variables
  for(s = 0; s < data->num_sites; s++){
    for(t = 0; t < num_out_times; t++){
      
      ind = (s*num_out_times) + t;
      
      // QC T
      if(data->T[ind] > MAX_T || data->T[ind] < MIN_T){
	data->T[ind] = MISSING;
      }
      //printf("data->dicast_id[%d]=%d, data->time_nominal[%d]=%.0f, data->T[%d]=%f\n", s, data->dicast_id[s], t, data->time_nominal[t], ind, data->T[ind]);
      
      // QC dewpt
      if(data->dewpt[ind] > MAX_T || data->dewpt[ind] < MIN_T){
	data->dewpt[ind] = MISSING;
      }
      
      // QC wind_speed
      // don't need to do anything
      
      // QC road_T
      if(data->road_T[ind] > MAX_ROAD_T || data->road_T[ind] < MIN_ROAD_T){
	data->road_T[ind] = MISSING;
      }
      
      // QC bridge_T
      if(data->bridge_T[ind] > MAX_ROAD_T || data->bridge_T[ind] < MIN_ROAD_T){
	data->bridge_T[ind] = MISSING;
      }
      
      // Set road_condition based on precip and temperature
      if(data->Precip[ind] == 1 && data->T[ind] > 1.5){
	data->road_condition[ind] = 2;
      } else if(data->Precip[ind] == 1 && data->T[ind] > -1.5 && data->T[ind] <= 1.5){
	data->road_condition[ind] = 4;
      } else if(data->Precip[ind] == 1 && data->T[ind] <= -1.5){
	data->road_condition[ind] = 3;
      } else{
	data->road_condition[ind] = 1;
      }
      
      // QC subsurface_T
      if(data->subsurface_T[ind] > MAX_ROAD_T || data->subsurface_T[ind] < MIN_ROAD_T){
	data->subsurface_T[ind] = MISSING;
      }
      
      // QC T_lbls0
      if(data->T_lbls0[ind] > MAX_ROAD_T || data->T_lbls0[ind] < MIN_ROAD_T){
	data->T_lbls0[ind] = MISSING;
      }
      
      // QC T_lbls1
      if(data->T_lbls1[ind] > MAX_ROAD_T || data->T_lbls1[ind] < MIN_ROAD_T){
	data->T_lbls1[ind] = MISSING;
      }
      
    } // end loop over obs times
  } // end loop over sites
  
  return(0);
}
