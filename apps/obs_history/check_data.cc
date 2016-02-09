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
#include <math.h>
#include <log/log.hh>
#include <ncf/ncf.hh>
#include "obs_history.h"

extern Log *logfile;

float calc_def_subsurface_T(int site_ind, int num_out_times, float *T);
void print_float_data(int site_ind, double start_time, int num_out_times, float *data);
void print_int_data(int site_ind, double start_time, int num_out_times, int *data);

int check_data(int num_out_times, double start_time, double cur_time, ds *data)
{

  int s, t;
  
  double diff = (cur_time - start_time) / 3600;
  int lead_ind = (int)diff;
  
  int size = data->num_sites * num_out_times;  
  
  // Set non-finite values (such as nan's) to missing
  NCF_finite(data->T, size, MISSING);
  NCF_finite(data->dewpt, size, MISSING);
  NCF_finite(data->wind_speed, size, MISSING);
  NCF_finite(data->road_T, size, MISSING);
  NCF_finite(data->bridge_T, size, MISSING);
  NCF_finite(data->subsurface_T, size, MISSING);
  
  //
  // Check the data for each site
  //
  
  for(s = 0; s < data->num_sites; s++){
    
    int site_ind = s*num_out_times;
    int ind1 = site_ind + lead_ind;
    int ind2 = site_ind + lead_ind - 1;
    
    int road_flag = 1;
    int bridge_flag = 1;
    int subsurface_flag = 1;
    int air_T_flag = 1;
    int lbls0_flag = 1;
    int lbls1_flag = 1;
    
    //
    // Check to see if the critical road data is missing
    //
    
    //printf("data->road_T[%d] = %f, data->road_T[%d] = %f\n", ind1, data->road_T[ind1], ind2, data->road_T[ind2]);  
    //printf("data->subsurface_T[%d] = %f, data->subsurface_T[%d] = %f\n", ind1, data->subsurface_T[ind1], ind2, data->subsurface_T[ind2]);  
    
    if(data->road_T[ind1] == MISSING || data->road_T[ind2] == MISSING){
      road_flag = 0; 
    }
    
    if(data->bridge_T[ind1] == MISSING || data->bridge_T[ind2] == MISSING){
      bridge_flag = 0;
    }
    
    if(data->subsurface_T[ind1] == MISSING || data->subsurface_T[ind2] == MISSING){
      subsurface_flag = 0;
    }
    
    if(data->T[ind1] == MISSING || data->T[ind2] == MISSING){
      air_T_flag = 0;
    }
    
    if(data->T_lbls0[ind1] == MISSING || data->T_lbls0[ind2] == MISSING){
      lbls0_flag = 0;
    }
    
    if(data->T_lbls1[ind1] == MISSING || data->T_lbls1[ind2] == MISSING){
      lbls1_flag = 0;
    }
    
    logfile->write_time(3, "road_flag = %d, bridge_flag = %d, subsurface_flag = %d, air_T_flag = %d, lbls0_flag = %d, lbls1_flag = %d\n", road_flag, bridge_flag, subsurface_flag, air_T_flag, lbls0_flag, lbls1_flag);
    
    // Get the defualt subsurface_T value
    float def_subsurface_T = calc_def_subsurface_T(site_ind, num_out_times, data->T);
    if(def_subsurface_T == MISSING){
      logfile->write_time("Warning: def_subsurface_T (air-T data) is missing for site %d.\n", data->dicast_id[s]);
    }
    
    //
    // If the critical road data is missing use soil forecast values 
    // to supplement the data, if the soil data is missing use air-T
    //
    for(t = 0; t < num_out_times; t++){
      int data_ind = site_ind + t;
      
      if(road_flag == 0 && air_T_flag == 1){
	if(t == 0){
	  logfile->write_time(3, "Info: using air-T data to supplement some of the road_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->road_T[data_ind] == MISSING){
	  data->road_T[data_ind] = data->T[data_ind];
	}
      } else if(road_flag == 0 && lbls0_flag == 1){
	if(t == 0){
	  logfile->write_time(3, "Info: using T_lbls0 data to supplement some of the road_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->road_T[data_ind] == MISSING){
	  data->road_T[data_ind] = data->T_lbls0[data_ind];
	}
      } else if(road_flag == 0 && air_T_flag == 0 && lbls0_flag == 0){	
	if(t == 0){
	  logfile->write_time("Warning: can not use air-T data or T_lbls0 data to supplement some of the road_T data for site %d.\n", data->dicast_id[s]);
	}
      }
      
      if(bridge_flag == 0 && air_T_flag == 1){
	if(t == 0){
	  logfile->write_time(3, "Info: using air-T data to supplement some of the bridge_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->bridge_T[data_ind] == MISSING){
	  data->bridge_T[data_ind] = data->T[data_ind];
	}
      } else if(bridge_flag == 0 && lbls0_flag == 1){
	if(t == 0){
	  logfile->write_time(3, "Info: using T_lbls0 data to supplement some of the bridge_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->bridge_T[data_ind] == MISSING){
	  data->bridge_T[data_ind] = data->T_lbls0[data_ind];
	}
      } else if(bridge_flag == 0 && air_T_flag == 0 && lbls0_flag == 0){
	if(t == 0){
	  logfile->write_time("Warning: can not use air-T data or T_lbls0 data to supplement some of the bridge_T data for site %d.\n", data->dicast_id[s]);
	}
      }
      
      if(subsurface_flag == 0 && def_subsurface_T != MISSING){
	if(t == 0){
	  logfile->write_time(3, "Info: using def_subsurface_T value to supplement some of the subsurface_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->subsurface_T[data_ind] == MISSING){
	  data->subsurface_T[data_ind] = def_subsurface_T;
	}
      } else if(subsurface_flag == 0 && lbls1_flag == 1){
	if(t == 0){
	  logfile->write_time(3, "Info: using T_lbls1 data to supplement some of the subsurface_T data for site %d.\n", data->dicast_id[s]);
	}
	if(data->subsurface_T[data_ind] == MISSING){
	  data->subsurface_T[data_ind] = data->T_lbls1[data_ind];
	}
      } else if(subsurface_flag == 0 && def_subsurface_T == MISSING && lbls1_flag == 0){
	if(t == 0){
	  logfile->write_time("Warning: can not use def_subsurface_T data or T_lbls1 data to supplement some of the subsurface_T data for site %d.\n", data->dicast_id[s]);
	}
      }
      
    } // end loop over out times
    
    //
    // Make sure there is at least one value per variable
    // at the critical times (cur_time and cur_time - 1hour)
    //
    
    // Check T
    //if(data->T[ind1] == MISSING || data->T[ind2] == MISSING){
    //logfile->write_time("Warning: Missing critical data for site %d, variable T.\n", data->dicast_id[s]);
    //}
    logfile->write(4,"Info: T data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->T);
    
    // Check dewpt
    //if(data->dewpt[ind1] == MISSING || data->dewpt[ind2] == MISSING){
    //logfile->write_time("Warning: Missing critical data for site %d, variable dewpt.\n", data->dicast_id[s]);
    //}
    logfile->write(4,"Info: dewpt data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->dewpt);
    
    // Check wind_speed
    if(data->wind_speed[ind1] == MISSING || data->wind_speed[ind2] == MISSING){
      logfile->write_time("Warning: Missing critical data for site %d, variable wind_speed.\n", data->dicast_id[s]);
    }
    logfile->write(4,"Info: wind_speed data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->wind_speed);
    
    // Check Precip
    //if(data->Precip[ind1] == MISSING || data->Precip[ind2] == MISSING){
    //logfile->write_time("Warning: Missing critical data for site %d, variable Precip.\n", data->dicast_id[s]);
    //}
    logfile->write(4,"Info: Precip data for site %d\n", data->dicast_id[s]);
    print_int_data(site_ind, start_time, num_out_times, data->Precip);
    
    // Check road_T
    if(data->road_T[ind1] == MISSING || data->road_T[ind2] == MISSING){
      logfile->write_time("Warning: Missing critical data for site %d, variable road_T.\n", data->dicast_id[s]);
    }
    logfile->write(4,"Info: road_T data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->road_T);
    
    // Check bridge_T
    if(data->bridge_T[ind1] == MISSING || data->bridge_T[ind2] == MISSING){
      logfile->write_time("Warning: Missing critical data for site %d, variable bridge_T.\n", data->dicast_id[s]); 
    }
    logfile->write(4,"Info: bridge_T data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->bridge_T);
    
    // Check road_condition
    //if(data->road_condition[ind1] == MISSING || data->road_condition[ind2] == MISSING){
    //logfile->write_time("Warning: Missing critical data for site %d, variable road_condition.\n", data->dicast_id[s]);
    //}
    logfile->write(4,"Info: road_condition data for site %d\n", data->dicast_id[s]);
    print_int_data(site_ind, start_time, num_out_times, data->road_condition);
    
    // Check subsurface_T
    if(data->subsurface_T[ind1] == MISSING || data->subsurface_T[ind2] == MISSING){
      logfile->write_time("Warning: Missing critical data for site %d, variable subsurface_T.\n", data->dicast_id[s]);
    }
    logfile->write(4,"Info: subsurface_T data for site %d\n", data->dicast_id[s]);
    print_float_data(site_ind, start_time, num_out_times, data->subsurface_T);
    
  } // end loop over sites
  
  return(0);
}

float calc_def_subsurface_T(int site_ind, int num_out_times, float *T)
{
  float cnt = 0;
  float sum = 0;
  float subsurface_T;
  
  for(int t = 0; t < num_out_times; t++){
    int data_ind = site_ind + t;  
    if(T[data_ind] != MISSING){
      //cout << "T[" << data_ind << "] = " << T[data_ind] << endl;
      sum = sum + T[data_ind];
      cnt++;
    }
  } // end loop over out times
  
  if(cnt > 0){
    subsurface_T = sum / cnt;
  } else{
    subsurface_T = MISSING;
  }
  
  //cout << "sum = " << sum << " cnt = " << cnt << " subsurface_T = " << subsurface_T << endl;
  
  return(subsurface_T);
}

void print_float_data(int site_ind, double start_time, int num_out_times, float *data)
{
  int cnt = 0;
  double loop_time = start_time;
  for(int t = 0; t < num_out_times; t++){
    int data_ind = site_ind + t;
    logfile->write(4, "cnt = %d, time = %d, data[%d] = %f\n", cnt, (int)loop_time, data_ind, data[data_ind]);	
    cnt++;
    loop_time = loop_time + 3600;
  }
}

void print_int_data(int site_ind, double start_time, int num_out_times, int *data)
{
  int cnt = 0;
  double loop_time = start_time;
  for(int t = 0; t < num_out_times; t++){
    int data_ind = site_ind + t;
    logfile->write(4, "cnt = %d, time = %d, data[%d] = %d\n", cnt, (int)loop_time, data_ind, data[data_ind]);	
    cnt++;
    loop_time = loop_time + 3600;
  }
}
