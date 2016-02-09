// Include files 
#include <new>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include "log/log.hh"
#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "netcdf.h"
#include "Params.hh"
#include "consts.hh"

// Constant, macro and type definitions 

// Global variables 
extern Log *proc_log;
extern Params conf_params;
long max_sites;


// Functions and objects


int create_output_file(char *cdl_name, char * output_name)
{
  int ret;

  ret = NCF_cdl2nc(cdl_name, output_name);
  if (ret != 0)
    {
      proc_log->write_time("Error: Unable to generate output file %s\n",output_name);
      return(1);
    }

  return(0);
}

int write_header(NcFile *output_file, double forc_time, int num_output_sites,
		 int model_id, int  *output_site_array, long *max_output_sites,
		 long *num_output_days, long *num_output_times_per_day,
		 long *num_output_param_vals)
{
  NcVar *out_var_ptr;
  int ret;

  // Write creation time
  if (NXNCF_write_creation_time(*output_file))
    {
      proc_log->write_time("Error: Unable to write creation time to output file\n");
      return(1);
    }


  // Copy over Forecast time
  out_var_ptr = output_file->get_var(FORC_TIME_NAME);
  if (!out_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", FORC_TIME_NAME);
      return(1);
    }
    
  ret = out_var_ptr->put(&forc_time, 1);
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to put data for var %s\n", 
		       out_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Forecast time is %f\n",forc_time);

  // Copy over Model ID
  out_var_ptr = output_file->get_var("model_id");
  if (out_var_ptr)
    {
      ret = out_var_ptr->put(&model_id, 1);
      if (ret == 0)
	{
	  proc_log->write_time("Error: Unable to put data for var %s\n", 
			       out_var_ptr->name());
	  return(1);
	}
      else
	proc_log->write_time(1,"Info: Model ID is %f\n",forc_time);  
    }
  

  // Get site dimension, handle unlimited dimension
  NcDim *max_sites_dim = output_file->get_dim(MAX_SITE_NUM_NAME);
  if (max_sites_dim->is_unlimited())
    max_sites = num_output_sites;
  else
    max_sites = max_sites_dim->size();
  
  if (num_output_sites > max_sites)
    {
      proc_log->write_time("Warning: Site list size too large, truncating to %d\n",max_sites);
      num_output_sites = max_sites;
    }

  // Write out num_sites
  out_var_ptr = output_file->get_var(NUM_SITES_NAME);
  if (!out_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", NUM_SITES_NAME);
      return(1);
    }
  
  ret = out_var_ptr->put(&num_output_sites, 1);
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to put data for var %s\n",out_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Number of output sites is %d\n",num_output_sites);


  // Write out Site List
  out_var_ptr = output_file->get_var(SITE_LIST_NAME);
  if (!out_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", SITE_LIST_NAME);
      return(1);
    }

  ret = out_var_ptr->put(output_site_array, num_output_sites);
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to put data for var %s\n",
		       out_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Successfully wrote %s\n",
		     out_var_ptr->name());


  NcDim *num_output_times_dim = output_file->get_dim(FC_TIMES_PER_DAY_NAME);
  if (!num_output_times_dim)
    {
      proc_log->write_time("Error: Unable to get ptr %s\n", FC_TIMES_PER_DAY_NAME);
      return(1);
    }
  *num_output_times_per_day = num_output_times_dim->size();
  
  NcDim *num_output_days_dim = output_file->get_dim(DAYS_DIM_NAME);
  if (!num_output_days_dim)
    {
      proc_log->write_time("Error: Unable to get ptr %s\n", DAYS_DIM_NAME);
      return(1);
    }
  *num_output_days = num_output_days_dim->size();
  
  *max_output_sites = max_sites;

  NcDim *param_vals_dim = output_file->get_dim(PARAM_VALS_DIM_NAME);
  param_vals_dim = (param_vals_dim) ? param_vals_dim : output_file->get_dim(QUANTILES_VAR);
  if (!param_vals_dim)
    *num_output_param_vals = 1;
  else
    *num_output_param_vals = param_vals_dim->size();
  
  return(0);
}


int write_mod_fcst(NcFile *output_file, char *var_name,
		   float *data)
{
  NcVar *output_var_ptr;
  int ret;

  output_var_ptr = output_file->get_var(var_name);
  if (!output_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", var_name);
      return(1);
    }

  long *dims = output_var_ptr->edges();
  dims[0] = max_sites;

  ret = output_var_ptr->put(data, dims);
  delete [] dims;
  if (ret == 0) 
    {
      proc_log->write_time("Error: Unable to put data for var %s\n",
		       output_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Successfully wrote %s\n",
		     output_var_ptr->name());


  return(0);
}
