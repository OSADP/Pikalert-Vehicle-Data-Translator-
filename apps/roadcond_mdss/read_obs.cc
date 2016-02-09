// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: read_obs.cc
 *
 * Author: Bill Myers
 *
 * Date:   01/04/07
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 01/29/07
 *  
 * Description: 
 *              This file contains one routine which reads a file
 * containing weather observational information specific to estimates
 * of road conditions. This info is stored in a weather observation
 * time series structure.
 *     
 *       read_obs_data()...Main routine described above.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "layer.hh"
#include "Obs.hh"

/* Constant definitions / Macro definitions / Type definitions */

/* External global variables / Non-static global variables / Static globals */
extern Log *proc_log;

/* External functions / Internal global functions / Internal static functions */
extern int find_precip_type(float T, float qpf, float prob_precip,
                            float cprob_rain, float cprob_snow, float cprob_ice);



void init_obs_ts(ObsTS *obs_ts, int site_index, int nh)
{
  for (int h = 0; h < nh; h++)
    {
      obs_ts[site_index].obs[h].T = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].wspd = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].dewpt = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].Precip = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].road_condition = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].road_T = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].bridge_T = OBS_MISSING_VALUE;
      obs_ts[site_index].obs[h].subsurface_T = OBS_MISSING_VALUE;
    }
}


//  
//  Routine: read_obs_data()
//  
//  Description:
//      This module reads in weather observations for many forecast sites. The input
//  file is in netcdf format. It reads several observational variables and stores the
//  observational data for each road conditions forecast site. The resultant time series 
//  starts at the weather observations starting hour, not the road conditions forecast 
//  starting hour. 
//  
//  Inputs: 
//    obs_file_name...................The name of the file containing the obs data.
//    output_site_array..............An array containing the id's of the output forecast site list.
//    num_sites......................The number of sites in the output forecast site list.
//  
//  Outputs:
//    obs_start_time.....The unix time of the first observation in the weather data obs time series.
//    obs_ts.............An array of structures describing an obs time series.
//  
//  Returns:
//    0..................If no fatal errors were found.
//    1..................If a fatal processing condition was encountered.
//


int read_obs_data(char *obs_file_name, int *output_site_array, int num_sites, 
		  double *obs_start_time, ObsTS *obs_ts)
{
  NcFile *nc_obs = 0;
  NcVar *ptr;
  long *dim_array;
  long *site_index_array;
  int num_vals;
  int ret;
  int i;
  int si;
  int nh;
  int exit_status;
  int num_times;
  double obs_time;

  float *T = NULL, *wspd = NULL, *dewpt = NULL;
  float *road_T = NULL, *bridge_T, *subsurface_T = NULL;
  int *Precip = NULL, *road_condition = NULL;

  nc_obs = new NcFile(obs_file_name, NcFile::ReadOnly);
  if (!nc_obs){
    proc_log->write_time("Error: Unable to open file %s.\n", nc_obs);
    exit_status = 1;
    return(exit_status);
  }

  /* create index array for weather forecast sites */
  ret = NXNCF_create_index_array(output_site_array, num_sites, nc_obs, 
				 &site_index_array);
  if (ret < 0) {
    proc_log->write_time("Error: failure in call to obs create_index_array.\n");
    return(1);
  }   
  
  /* Get time of first forecast in obs file */
  ptr = nc_obs->get_var("obs_time");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "obs_time");
      return(1);
    }

  dim_array = ptr->edges();
  ret = ptr->get(&obs_time, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "obs_time");
      return(1);
    }

  *obs_start_time = obs_time;

  /* Get frequency of forecasts per day */

  ptr = nc_obs->get_var("T");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "T");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();
  num_times = dim_array[1];

  T = new float[num_vals];

  ret = ptr->get(T, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "T");
      return(1);
    } 

  ptr = nc_obs->get_var("wind_speed");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "wind_speed");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  wspd = new float[num_vals];

  ret = ptr->get(wspd, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "wind_speed");
      return(1);
    } 


  ptr = nc_obs->get_var("dewpt");
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

  ptr = nc_obs->get_var("Precip");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "Precip");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  Precip = new int[num_vals];

  ret = ptr->get(Precip, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "Precip");
      return(1);
    } 

  ptr = nc_obs->get_var("road_T");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "road_T");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  road_T = new float[num_vals];

  ret = ptr->get(road_T, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "road_T");
      return(1);
    } 

  ptr = nc_obs->get_var("bridge_T");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "bridge_T");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  bridge_T = new float[num_vals];

  ret = ptr->get(bridge_T, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "bridge_T");
      return(1);
    } 

  ptr = nc_obs->get_var("road_condition");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "road_condition");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  road_condition = new int[num_vals];

  ret = ptr->get(road_condition, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "road_condition");
      return(1);
    } 

  ptr = nc_obs->get_var("subsurface_T");
  if (ptr == NULL)
    {
      proc_log->write_time("Error: Unable to get ptr to var %s.\n", 
			   "subsurface_T");
      return(1);
    }

  delete [] dim_array;
  dim_array = ptr->edges();
  num_vals = ptr->num_vals();

  subsurface_T = new float[num_vals];

  ret = ptr->get(subsurface_T, dim_array);
  if (ret < 0)
    {
      proc_log->write_time("Error: Unable to get var %s.\n", 
			   "subsurface_T");
      return(1);
    } 

  // Extract forecast values for each output forecast site at each lead time
  for (i=0; i<num_sites; i++)
    {

      init_obs_ts(obs_ts, i, MAX_OBS_HISTORY);

      si = site_index_array[i];

      // skip sites without obs
      if (si < 0)
	continue;

      for (nh = 0; nh < MAX_OBS_HISTORY; nh++)
	{
	  int index = si * num_times + nh;

	  obs_ts[i].obs[nh].T = T[index];
	  obs_ts[i].obs[nh].wspd = wspd[index];
	  obs_ts[i].obs[nh].dewpt = dewpt[index];
	  obs_ts[i].obs[nh].Precip = Precip[index];
	  obs_ts[i].obs[nh].road_T = road_T[index];
	  obs_ts[i].obs[nh].bridge_T = bridge_T[index];
	  obs_ts[i].obs[nh].road_condition = road_condition[index];
	  obs_ts[i].obs[nh].subsurface_T = subsurface_T[index];
	  /*
	  printf("site %d   T %.2f   dewpt %.2f  Precip %d  road_T %.2f\n", i,
		 obs_ts[i].obs[nh].T, obs_ts[i].obs[nh].dewpt, obs_ts[i].obs[nh].Precip,
		 obs_ts[i].obs[nh].road_T);
	  */
	}
    }

  delete [] dim_array;
  delete[] T;
  delete[] wspd;
  delete[] dewpt;
  delete[] Precip;
  delete[] road_T;
  delete[] bridge_T;
  delete[] road_condition;
  delete[] subsurface_T;
  delete[] site_index_array;
  delete nc_obs;

  return(0);
}

