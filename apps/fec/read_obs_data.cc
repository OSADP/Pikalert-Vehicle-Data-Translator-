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

#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "netcdf.h"
#include "Params.hh"

// Constant, macro and type definitions 

// Global variables 
extern Log *proc_log;
extern Params conf_params;

// Functions and objects

int read_obs_info(NcFile *obs_file, long * num_todays_obs_times_per_day,
		  double **todays_obs_time, int *num_todays_obs_sites,
		  int **todays_obs_site_list)
{
  NcVar *obs_var_ptr;
  int ret;


  NcDim *num_todays_obs_times_dim = obs_file->get_dim("times_per_day");
  *num_todays_obs_times_per_day = num_todays_obs_times_dim->size();

  obs_var_ptr =  obs_file->get_var("time_nominal");
  if (!obs_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", "time_nominal");
      return(1);
    }

  *todays_obs_time = new double [*num_todays_obs_times_per_day];
  ret = obs_var_ptr->get(*todays_obs_time, *num_todays_obs_times_per_day);
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
		       obs_var_ptr->name());
      return(1);
    }

  // Get site list from today's obs file
  obs_var_ptr =  obs_file->get_var(NUM_SITES_NAME);
  if (!obs_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", NUM_SITES_NAME);
      return(1);
    }

  *num_todays_obs_sites = obs_var_ptr->as_int(0);
  //printf("num obs sites %d\n", *num_todays_obs_sites);

  *todays_obs_site_list = new int[*num_todays_obs_sites];
  obs_var_ptr =  obs_file->get_var(SITE_LIST_NAME);
  if (!obs_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", SITE_LIST_NAME);
      return(1);
    }

  ret = obs_var_ptr->get(*todays_obs_site_list, *num_todays_obs_sites);
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
		       obs_var_ptr->name());
      return(1);
    }

  return(0);
}

int read_obs_data(NcFile *obs_file, char *var_name, long *times_per_day,
		   float *data)
{
  NcVar *obs_var_ptr;
  int ret;

  obs_var_ptr = obs_file->get_var(var_name);
  if (!obs_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", var_name);
      return(1);
    }

  long *in_dim_array = obs_var_ptr->edges();
  *times_per_day = in_dim_array[1];

  ret = obs_var_ptr->get(data, in_dim_array);
  delete [] in_dim_array;

  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
		       obs_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Successfully read %s\n",
		     obs_var_ptr->name());

  return(0);
}

int find_nbr_obs(int num_obs_sites, int *obs_site_list,
		 float *obs_data, int times_per_day,
		 int nbr, double *obs_time_array,
		 double obs_time, float *obs)
{

  //cout << "nbr = " << nbr << endl;

  int i;
  int ind;

  i = 0;
  while (i < num_obs_sites && nbr != obs_site_list[i])
    i++;

  if (i < num_obs_sites)
    {
      //printf("found nbr %d at obs index %d\n", nbr, i);
      
      // Find time offset
      ind = 0;
      while (ind < times_per_day && obs_time != obs_time_array[ind])
	ind++;

      if (ind < times_per_day)
	{
	  *obs = obs_data[i*times_per_day + ind];
	}
      else
	{
	  *obs = NC_FILL_FLOAT;
	}
      //printf("obs site index %d  time %d  obs %.1f\n", i, ind, *obs);
    }
  else
    {
      *obs = NC_FILL_FLOAT;
      //printf("could not find %d\n", nbr);
    }


  return(0);
}

int get_site_obs_data(int dicast_id, int num_obs_sites, int *obs_site_list, float *obs_data, int num_times_per_day, float* &site_obs_data)
{
  
  //
  // Get the site index
  //
  int s = 0;
  while (s < num_obs_sites && dicast_id != obs_site_list[s])
    s++;

  //cout << "s = " << s << " num_obs_sites = " << num_obs_sites << endl;
  
  int site_index;
  if(s < num_obs_sites){
    site_index = s;
  }else {
    return -1;
  }
  
  //cout << "site_index = " << site_index << endl;
  
  int num_vals_per_site = num_times_per_day;
  site_obs_data = new float[num_vals_per_site];
  
  // Copy the site data to an array
  int obs_ind = (site_index * num_vals_per_site);
  for(int i = 0; i < num_vals_per_site; i++){
    //cout << "obs_data[" << obs_ind << "]  " << obs_data[obs_ind] << endl;
    site_obs_data[i] = obs_data[obs_ind];
    obs_ind++;
  }
  
  return(0);
}

int concat_obs_data(float *todays_obs_data, float *yesterdays_obs_data, float* &all_obs_data, 
		    double *todays_obs_times, long todays_num_obs_times, double *yesterdays_obs_times, 
		    long yesterdays_num_obs_times, double* &all_obs_times)
{
  int i;
  int ind;

  all_obs_data = new float[yesterdays_num_obs_times+todays_num_obs_times];
  all_obs_times = new double[yesterdays_num_obs_times+todays_num_obs_times];
  
  ind = 0;
  for(i = 0; i < yesterdays_num_obs_times; i++){
    
    if(yesterdays_obs_data == NULL)
      all_obs_data[ind] = NC_FILL_FLOAT;
    else
      all_obs_data[ind] = yesterdays_obs_data[i];
    
    all_obs_times[ind] = yesterdays_obs_times[i];
    ind++;
  }
  
  for(i = 0; i < todays_num_obs_times; i++){
    all_obs_data[ind] = todays_obs_data[i];
    all_obs_times[ind] = todays_obs_times[i];
    ind++;
  }
  
  return(0);
}
