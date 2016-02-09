// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <log/log.hh>
#include "concat_meso_data.h"

//extern Log *logfile;
extern Log *proc_log;

int alloc_data(ds *data, vector<site *> sites, int num_out_times){
  
  int size = sites.size();
  
  data->creation_time = MISSING;
  data->obs_time = MISSING;
  data->num_obs = MISSING;
  data->num_sites = size;
  data->type = MISSING;
  data->time_nominal = new double[num_out_times];
  data->dicast_id = new int[size];
  data->T = new float[size*num_out_times];
  data->dewpt = new float[size*num_out_times];
  data->wind_speed = new float[size*num_out_times];
  data->Precip = new int[size*num_out_times];
  data->road_T = new float[size*num_out_times];
  data->bridge_T = new float[size*num_out_times];
  data->road_condition = new int[size*num_out_times];
  data->subsurface_T = new float[size*num_out_times];
  data->T_lbls0 = new float[size*num_out_times];
  data->T_lbls1 = new float[size*num_out_times];
  data->is_valid = 1;
  
  for (int i=0; i<size; i++){
    data->dicast_id[i] =  sites[i]->dicast_id;   
    for (int j=0; j< num_out_times; j++){
      data->T[i*num_out_times+j] = MISSING;
      data->dewpt[i*num_out_times+j] = MISSING;
      data->wind_speed[i*num_out_times+j] = MISSING;
      data->Precip[i*num_out_times+j] = MISSING;
      data->road_T[i*num_out_times+j] = MISSING;
      data->bridge_T[i*num_out_times+j] = MISSING;
      data->road_condition[i*num_out_times+j] = MISSING;
      data->subsurface_T[i*num_out_times+j] = MISSING;
      data->T_lbls0[i*num_out_times+j] = MISSING;
      data->T_lbls1[i*num_out_times+j] = MISSING;
    }
  }
  
  return(0);
}
