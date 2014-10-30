// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: read_wx.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 01/29/07
 * 
 * Description: 
 *              This file contains one routine which reads a file
 * containing weather forecast information specific to prediction
 * of road conditions. This info is stored in a weather forecast
 * time series structure.
 *     
 *       read_wx_data()...Main routine described above.
 *
 */

#define METRO 1

/* System include files / Local include files */
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "layer.hh"
#include "WxFcst.hh"

/* Constant definitions / Macro definitions / Type definitions */

/* External global variables / Non-static global variables / Static globals */
extern Log *proc_log;

/* External functions / Internal global functions / Internal static functions */
extern float snow_density( float t, float wspd );

void init_wx(WxForecast *fcst, int nh)
{
  for (int h = 0; h < nh; h++)
    {
      fcst[h].T = FCST_MISSING_VALUE;
      fcst[h].dewpt = FCST_MISSING_VALUE;
      fcst[h].rh = FCST_MISSING_VALUE;
      fcst[h].wind_speed = FCST_MISSING_VALUE;
      fcst[h].wind_dir = FCST_MISSING_VALUE;
      fcst[h].P_sfc = FCST_MISSING_VALUE;
      fcst[h].cloud_cov = FCST_MISSING_VALUE;
      fcst[h].snow_accum = FCST_MISSING_VALUE;
      fcst[h].qpf01 = FCST_MISSING_VALUE;
      fcst[h].LiquidPrecipRate = FCST_MISSING_VALUE;
      fcst[h].FrozenPrecipRate = FCST_MISSING_VALUE;      
      fcst[h].Ptype = FCST_MISSING_VALUE;
      fcst[h].dlwrf_sfc = FCST_MISSING_VALUE;
      fcst[h].dswrf_sfc = FCST_MISSING_VALUE;      
      fcst[h].blowing_snow_potential = FCST_MISSING_VALUE;      
      fcst[h].blowing_snow_pot_vals = FCST_MISSING_VALUE;
    }
}

void init_wx_ts(WxFcstTS *wx_fcst_ts, int site_index, int nh)
{
  wx_fcst_ts[site_index].fcst = new WxForecast[nh];
  init_wx(wx_fcst_ts[site_index].fcst, nh);
}


//  
//  Routine: read_wx_data()
//  
//  Description:
//      This module reads in a weather forecast for many forecast sites. The input
//  file is in netcdf format. It reads several forecast variables and stores the
//  forecast data for each road conditions forecast site. Some variables are derived 
//  from the input data fields. The resultant time series starts at the weather forecast
//  starting hour, not the road conditions forecast starting hour.
//  
//  Inputs: 
//    wx_file_name.......The name of the file containing the weather forecast data.
//    output_site_array..An array containing the id's of the output forecast site list.
//    num_output_sites...The number of sites in the output forecast site list.
//    forc_time..........First road weather forecast (unix) time.
//  
//  Outputs:
//    num_days...........The number of days in the weather data forecast time series.
//    num_times..........The number of forecast times per day in the weather data forecast.
//    fcst_start_time....The unix time of the first forecast in the weather data forecast.
//    wx_fcst_ts.........An array of structures describing a weather forecast time series.
//  
//  Returns:
//    0..................If no fatal errors were found.
//    1..................If a fatal processing condition was encountered.
//


int read_wx_data(char *wx_file_name, int *output_site_array, int num_sites, 
		 double forc_time, int *num_days, int *num_times, double *fcst_start_time, 
		 WxFcstTS *wx_fcst_ts)
{
  NcFile *nc_wx_fcst = 0;
  NcVar *ptr;
  NcDim *nc_dim;
  long *dim_array;
  long *site_index_array;
  int num_vals;
  int hour_offset;
  int ret;
  int i;
  int si;
  int nh;
  int exit_status;

  float *T = NULL, *wind_speed = NULL, *dewpt = NULL, *wind_dir = NULL;
  float *qpf01 = NULL, *P_sfc = NULL, *rh = NULL;
  float *cloud_cov = NULL, *snow_accum_total = NULL;
  float *dlwrf_sfc = NULL, *dswrf_sfc = NULL;
  float *bs_pot_vals = NULL;
  float *bs_index = NULL;
  float *Ptype = NULL;

  nc_wx_fcst = new NcFile(wx_file_name, NcFile::ReadOnly);
  if (!nc_wx_fcst){
    proc_log->write_time("Error: Unable to open file %s.\n", nc_wx_fcst);
    exit_status = 1;
    return(exit_status);
  }

  /* create index array for weather forecast sites */
  ret = NXNCF_create_index_array(output_site_array, num_sites, nc_wx_fcst, 
				 &site_index_array);
  if (ret < 0) {
    proc_log->write_time("Error: failure in call to wx create_index_array.\n");
    return(1);
  }   
  
  /* Get time of first forecast in wx file */
  ptr = nc_wx_fcst->get_var(FORC_TIME_NAME);
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   FORC_TIME_NAME);
      return(1);
    }

  dim_array = ptr->edges();
  ret = ptr->get(fcst_start_time, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   FORC_TIME_NAME);
      return(1);
    }

  hour_offset = (int)(forc_time - (*fcst_start_time)) / SEC_PER_HOUR;

  /* Get number of forecast days */
  nc_dim = nc_wx_fcst->get_dim(DAYS_DIM_NAME);
  *num_days  = nc_dim->size();
  if (num_days < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", DAYS_DIM_NAME);
    return(1);
  }

  /* Get frequency of forecasts per day */
  nc_dim = nc_wx_fcst->get_dim(FC_TIMES_PER_DAY_NAME);
  *num_times = nc_dim->size();
  if (num_times < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", FC_TIMES_PER_DAY_NAME);
    return(1);
  }
  
  // Calculate the total number of lead times and use this 
  // instead of MAX_FORECAST previously defined in WxFcst.hh
  int num_wx_times = (*num_days) * (*num_times);
  //cout << "num_wx_times = " << num_wx_times << endl;
  
  ptr = nc_wx_fcst->get_var("T");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "T");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  T = new float[num_vals];

  ret = ptr->get(T, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "T");
      return(1);
    } 

  ptr = nc_wx_fcst->get_var("wind_speed");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "wind_speed");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  wind_speed = new float[num_vals];

  ret = ptr->get(wind_speed, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "wind_speed");
      return(1);
    } 

  // Get wind dir
  ptr = nc_wx_fcst->get_var("wind_dir");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n",
                           "wind_dir");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  wind_dir = new float[num_vals];

  ret = ptr->get(wind_dir, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n",
                           "wind_dir");
      return(1);
    }

  ptr = nc_wx_fcst->get_var("cloud_cov");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n",
                           "cloud_cov");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  cloud_cov = new float[num_vals];

  ret = ptr->get(cloud_cov, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n",
                           "cloud_cov");
      return(1);
    }


  ptr = nc_wx_fcst->get_var("dewpt");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "dewpt");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  dewpt = new float[num_vals];

  ret = ptr->get(dewpt, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "dewpt");
      return(1);
    } 

  ptr = nc_wx_fcst->get_var("rh");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "rh");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  rh = new float[num_vals];

  ret = ptr->get(rh, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "rh");
      return(1);
    } 


  // Use precip_rate as one hour precip rate estimate
  ptr = nc_wx_fcst->get_var("precip_rate");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "precip_rate");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  qpf01 = new float[num_vals];

  ret = ptr->get(qpf01, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "precip_rate");
      return(1);
    } 


  // Get precip type
  ptr = nc_wx_fcst->get_var("precip_type");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "precip_type");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  Ptype = new float[num_vals];

  ret = ptr->get(Ptype, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "precip_type");
      return(1);
    } 

  ptr = nc_wx_fcst->get_var("snow_accum_total");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n",
                           "snow_accum_total");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  snow_accum_total = new float[num_vals];

  ret = ptr->get(snow_accum_total, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n",
                           "snow_accum_total");
      return(1);
    }


  ptr = nc_wx_fcst->get_var("P_sfc");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "P_sfc");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  P_sfc = new float[num_vals];

  ret = ptr->get(P_sfc, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "P_sfc");
      return(1);
    } 

  ptr = nc_wx_fcst->get_var("dlwrf_sfc");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n",
                           "dlwrf_sfc");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  dlwrf_sfc = new float[num_vals];

  ret = ptr->get(dlwrf_sfc, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n",
                           "dlwrf_sfc");
      return(1);
    }

  ptr = nc_wx_fcst->get_var("dswrf_sfc");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n",
                           "dswrf_sfc");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  dswrf_sfc = new float[num_vals];

  ret = ptr->get(dswrf_sfc, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n",
                           "dswrf_sfc");
      return(1);
    }

  ptr = nc_wx_fcst->get_var("blowing_snow_potential");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "blowing_snow_potential");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  bs_index = new float[num_vals];

  ret = ptr->get(bs_index, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "blowing_snow_potential");
      return(1);
    } 

  ptr = nc_wx_fcst->get_var("blowing_snow_pot_vals");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "blowing_snow_pot_vals");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  bs_pot_vals = new float[num_vals];

  ret = ptr->get(bs_pot_vals, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "blowing_snow_pot_vals");
      return(1);
    } 


  // Extract forecast values for each output forecast site at each lead time
  for (i=0; i<num_sites; i++)
    {

      init_wx_ts(wx_fcst_ts, i, num_wx_times);
      
      si = site_index_array[i];

      // skip sites without forecasts
      if (si < 0)
	continue;

      for (nh = 0; nh < num_wx_times; nh++)
	{
	  int index = si*(*num_days)*(*num_times) + nh;

          wx_fcst_ts[i].fcst[nh].T = T[index];
          wx_fcst_ts[i].fcst[nh].dewpt = dewpt[index];
          wx_fcst_ts[i].fcst[nh].rh = rh[index];
          wx_fcst_ts[i].fcst[nh].qpf01 = qpf01[index];
	  if (nh == 0)
	    wx_fcst_ts[i].fcst[nh].snow_accum = 0.;
	  else if (snow_accum_total[index-1] == NC_FILL_FLOAT ||
		   snow_accum_total[index] == NC_FILL_FLOAT)
	    wx_fcst_ts[i].fcst[nh].snow_accum = 0.;
	  else
	    wx_fcst_ts[i].fcst[nh].snow_accum = snow_accum_total[index] - snow_accum_total[index-1];
          wx_fcst_ts[i].fcst[nh].wind_speed = wind_speed[index];
          wx_fcst_ts[i].fcst[nh].P_sfc = P_sfc[index];
          wx_fcst_ts[i].fcst[nh].Ptype = (int)Ptype[index];
          wx_fcst_ts[i].fcst[nh].cloud_cov = cloud_cov[index];
          wx_fcst_ts[i].fcst[nh].wind_dir = wind_dir[index];
          wx_fcst_ts[i].fcst[nh].dlwrf_sfc = dlwrf_sfc[index];
          wx_fcst_ts[i].fcst[nh].dswrf_sfc = dswrf_sfc[index];

	  if ( bs_pot_vals[index] > 1.0 ) bs_pot_vals[index] = 0.0;
	  if ( bs_index[index] > 10.0 ) bs_index[index] = 0.0;
	  wx_fcst_ts[i].fcst[nh].blowing_snow_pot_vals = bs_pot_vals[index];
	  wx_fcst_ts[i].fcst[nh].blowing_snow_potential = (int)bs_index[index];

	  wx_fcst_ts[i].fcst[nh].LiquidPrecipRate = qpf01[index];
	  if (wx_fcst_ts[i].fcst[nh].Ptype == C_PTYPE_SNOW)
	    wx_fcst_ts[i].fcst[nh].FrozenPrecipRate = qpf01[index] * snow_density(T[index],wind_speed[index]);
	  else
	    wx_fcst_ts[i].fcst[nh].FrozenPrecipRate = 0.;

	  /*
	    printf("%d %d  T %.2f dewpt %.2f qpf %e\n", i, nh, wx_fcst_ts[i].fcst[nh].T,
	    wx_fcst_ts[i].fcst[nh].dewpt, wx_fcst_ts[i].fcst[nh].qpf01);
	  */
	}
    }
  
  // reset num_days to be the number of days in the weather time series
  //*num_days  = num_wx_times/(*num_times); // this is no longer necessary
  
  delete [] dim_array;
  delete[] T;
  delete[] dewpt;
  delete[] rh;
  delete[] qpf01;
  delete[] snow_accum_total;
  delete[] wind_speed;
  delete[] P_sfc;
  delete[] cloud_cov;
  delete[] wind_dir;
  delete[] dlwrf_sfc;
  delete[] dswrf_sfc;
  delete[] Ptype;
  delete[] bs_pot_vals;
  delete[] bs_index;
  delete[] site_index_array;
  delete nc_wx_fcst;

  return(0);
}

int verify_wxfcst( WxForecast *wxfcst )
{
  int valid = 1;

  if ( wxfcst->T == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->wind_speed == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->dewpt == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->qpf01 == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->P_sfc == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->wind_dir == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->cloud_cov == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->snow_accum == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->LiquidPrecipRate == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->FrozenPrecipRate == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->Ptype == NC_FILL_INT )
    valid = 0;
  if ( wxfcst->dlwrf_sfc == NC_FILL_FLOAT )
    valid = 0;
  if ( wxfcst->dswrf_sfc == NC_FILL_FLOAT )
    valid = 0;

  return(valid);
}
