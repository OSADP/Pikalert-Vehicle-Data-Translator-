// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// Include files 
#include <new>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <math.h>

#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "netcdf.h"
#include "Params.hh"

#include "concat_meso_data.h"

// Constant, macro and type definitions 

// Global variables 
extern Log *proc_log;
extern Params conf_params;

// Functions

int get_forecast(int site_index, float *fcst_data, int nday, int times_per_day,
                 double forc_time, double corr_time, float *fcst_val)
{
  int offset;

  offset = (int)(corr_time - forc_time) / SEC_PER_HOUR;
  offset /= (24 / times_per_day);

  offset += (site_index * nday * times_per_day);

  *fcst_val = fcst_data[offset];

  //printf("index %d  offset %d  val %.1f\n", site_index,  offset, *fcst_val);

  return(0);
}

//
// This function fec's the forecast data
//
int set_dynamic_forecast(int ns, int site_index, float *output_data,
			 float *corr_factors, float num_corr_hrs,
			 float *fcst_data, int nday, int times_per_day,
			 double forc_time, double corr_time, float fcst_error)
{
  proc_log->write_time(3, "Info: starting Dynamic FEC\n");
  proc_log->write_time(3, "Info: site_index = %d\n", site_index);

  int i;
  int offset;
  int hr, hr_per_step;
  
  offset = (int)(corr_time - forc_time) / SEC_PER_HOUR;
  offset /= (24 / times_per_day);
  
  offset += (site_index * nday * times_per_day);
  
  hr_per_step = 24 / times_per_day;
  for (hr = 0; hr < num_corr_hrs; hr+=hr_per_step)
    {
      float factor = corr_factors[hr];
      
      proc_log->write_time(3, "Info: fcst_val %.2f  error %.2f  factor %.2f  new fcst %.2f\n",
			   fcst_data[offset+hr / hr_per_step], fcst_error, factor, 
			   fcst_data[offset+hr / hr_per_step] + fcst_error * factor);
      
      if (fcst_data[offset+hr / hr_per_step] != NC_FILL_FLOAT &&
          fcst_error != NC_FILL_FLOAT){
	fcst_data[offset+hr / hr_per_step] += factor * fcst_error;
	//cout << "fcst_data[" << offset+hr / hr_per_step << "] = " << fcst_data[offset+hr / hr_per_step] << endl;
      }
    }

  // Copy fec'd fcst data to the output array
  for (int i=0; i<nday*times_per_day; i++)
    output_data[ns*nday*times_per_day+i] = fcst_data[site_index * nday * times_per_day + i];
  
  return(0);
}

//
// This function gets site specific forecast data
//
int get_site_forecast_data(int site_index, float *fcst_data, int num_days, int num_times_per_day, float* &site_fcst_data)
{
  int num_vals_per_site = num_days * num_times_per_day;  
  site_fcst_data = new float[num_vals_per_site];
  
  int fcst_ind = (site_index * num_vals_per_site);
  for(int i = 0; i < num_vals_per_site; i++){
    //cout << "fcst_data[" << fcst_ind << "]  " << fcst_data[fcst_ind] << endl;
    site_fcst_data[i] = fcst_data[fcst_ind];
    fcst_ind++;
  }
  
  return(0);
}

//
// If the current hour ob value is missing,
// this function will take the latest ob (up to 3 hours back)
// and the second fcst value (at curr hour + 1) and come up with
// an interpolated obs value for the current hour
//
float get_interp_obs_val(float *site_obs_data, int obs_start_ind, float *site_fcst_data, int fcst_start_ind, int do_interp)
{

  proc_log->write_time(3, "Info: Getting interpolated obs value for current hour\n");
  
  //cout << "obs_start_ind = " << obs_start_ind << " fcst_start_ind = " << fcst_start_ind << endl;
  
  if(do_interp == 0)
    {
      proc_log->write_time(3, "Info: Using actual obs value (instead of interpolated value) for current hour\n");
      return(site_obs_data[obs_start_ind]);
    }
  
  if(site_obs_data[obs_start_ind] != NC_FILL_FLOAT && site_obs_data[obs_start_ind] != MISSING)
    {
      proc_log->write_time(3, "Info: Using actual obs value (instead of interpolated value) for current hour\n");
      return(site_obs_data[obs_start_ind]);
    }
  
  float obs_wgt[3];
  obs_wgt[0] = 0.75;
  obs_wgt[1] = 0.50;
  obs_wgt[2] = 0.25;
  
  int obs_ind = obs_start_ind;
  int wgt_ind = -1;
  while((site_obs_data[obs_ind] == NC_FILL_FLOAT || site_obs_data[obs_ind] == MISSING) && obs_ind >= 0){
    obs_ind--;
    wgt_ind++;
  }
  
  if(obs_ind < 0 || obs_ind < obs_start_ind-3){
    proc_log->write_time(2, "Info: No obs within 3 hours of the current hour, can not interpolate\n");
    return (NC_FILL_FLOAT);
  }
  
  float fcst_wgt = 1 - obs_wgt[wgt_ind];
  float interp_val = (obs_wgt[wgt_ind] * site_obs_data[obs_ind]) + (fcst_wgt * site_fcst_data[fcst_start_ind+1]);
  
  proc_log->write_time(4, "Info: (obs_wgt[%d]=%.2f * site_obs_data[%d]=%.2f) + (fcst_wgt=%.2f * site_fcst_data[%d]=%.2f) = %.2f\n", wgt_ind, obs_wgt[wgt_ind], obs_ind, site_obs_data[obs_ind], fcst_wgt, fcst_start_ind+1, site_fcst_data[fcst_start_ind+1], interp_val);  
  
  return(interp_val);
}

//
// This function finds an inflection (max or min) point in the data array
// if pos is 0 then it will return the first inflection point
// if pos is 1 then it will return the last inflection point
//
int find_inflection_ind(int start_ind, int num_vals, float *data, int pos)
{
  float last_val = data[start_ind];
  float last_diff = 0;
  float last_dir = 0;
  
  // Set default inflection index
  // used if none can bee found
  int inflection_ind = start_ind + 2;
  
  for(int i = start_ind; i < num_vals; i++){
    float curr_val = data[i];
   
    if(curr_val != NC_FILL_FLOAT){
    
      float curr_diff = curr_val - last_val;
      
      float curr_dir;
      if(curr_diff < 0){
	curr_dir = -1;
      } else if(curr_diff > 0){ 
	curr_dir = 1;
      } else{
	curr_dir = 0;
      }
      
      float future_dir = curr_dir;
      if(i != num_vals-1){
	float future_val = data[i+1];
	float future_diff = future_val - curr_val;
	
	if(future_diff < 0){
	  future_dir = -1;
	} else if(future_diff > 0){ 
	  future_dir = 1;
	} else{
	  future_dir = 0;
	}
      }
      
      //cout << "curr_val = " << curr_val << " curr_diff = " << curr_diff << " curr_dir = " << curr_dir << " future_dir = " << future_dir << endl;
      if( i > start_ind+1 && curr_dir != 0 && curr_dir != last_dir && future_dir == curr_dir ){
	inflection_ind = i;
	if(pos == 0){
	  break;
	}
      }

      last_diff = curr_diff;
      last_dir = curr_dir;
    }

    last_val = curr_val;

  } // end loop over forecast values
  
  inflection_ind--;
  return(inflection_ind);
}
