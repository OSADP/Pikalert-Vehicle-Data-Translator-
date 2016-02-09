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
#include "consts.hh"


// Constant, macro and type definitions 
float MIN_CONFIDENCE = -0.00001;

// Global variables 
extern Log *proc_log;
extern Params conf_params;

// Functions and objects


int read_fcst_info(NcFile *input_file, int * num_input_sites, int * model_id, 
		   double *forc_time, long *num_input_times_per_day,
		   long *num_input_day, long *max_site_num,
		   long *num_param_vals)
{
  NcVar *input_var_ptr;
  double ft;
  int ret;

  input_var_ptr = input_file->get_var(NUM_SITES_NAME);
  if (!input_var_ptr)
  {
    proc_log->write_time("Error: Unable to get ptr for %s\n", NUM_SITES_NAME);
    return(1);
  } else {
    ret = input_var_ptr->get(num_input_sites, 1);
    if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
			   input_var_ptr->name());
      return(1);
    }
  }

  input_var_ptr = input_file->get_var("model_id");
  if (input_var_ptr)
  {
    ret = input_var_ptr->get(model_id, 1);
    if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
			   input_var_ptr->name());
      return(1);
    }
  } else {
    *model_id = NC_FILL_INT;
  }

  input_var_ptr = input_file->get_var(FORC_TIME_NAME);
  if (!input_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", FORC_TIME_NAME);
      return(1);
    }
  ft = input_var_ptr->as_double(0);
  *forc_time = ft;

  NcDim *num_input_times_dim = input_file->get_dim("fc_times_per_day");
  if (!num_input_times_dim)
  {
    proc_log->write_time("Error: Unable to get ptr fc_times_per_day\n");
    return(1);
  }
  *num_input_times_per_day = num_input_times_dim->size();

  NcDim *num_input_days_dim = input_file->get_dim("days");
  if (!num_input_days_dim)
  {
    proc_log->write_time("Error: Unable to get ptr days\n");
    return(1);
  }
  *num_input_day = num_input_days_dim->size();

  NcDim *max_site_dim = input_file->get_dim("max_site_num");
  if (!max_site_dim)
  {
    proc_log->write_time("Error: Unable to get ptr max_site_num\n");
    return(1);
  }
  *max_site_num = max_site_dim->size();

  NcDim *param_vals_dim = input_file->get_dim("param_vals");
  param_vals_dim = (param_vals_dim) ? param_vals_dim : input_file->get_dim(QUANTILES_VAR);
  if (!param_vals_dim)
    *num_param_vals = 1;
  else
    *num_param_vals = param_vals_dim->size();

  return(0);
}

int read_fcst_data(NcFile *input_file, char *var_name, 
		   long *times_per_day, float *data)
{
  NcVar *input_var_ptr;
  int i, ret;
  long num_dims;
  int raw_data_size;
  static float *raw_data = NULL;
  static int max_data_size = 0;


  input_var_ptr = input_file->get_var(var_name);
  if (!input_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", var_name);
      return(1);
    }

  long *in_dim_array = input_var_ptr->edges();
  *times_per_day = in_dim_array[2];

  num_dims = input_var_ptr->num_dims();

  // determine raw data size
  raw_data_size = 1;
  for (i=0; i<num_dims; i++)
    raw_data_size *= in_dim_array[i];

  // allocate space for raw data if necessary
  
  if (raw_data == NULL || max_data_size < raw_data_size )
    {
      delete [] raw_data;
      raw_data = new float[raw_data_size];
      max_data_size = raw_data_size;
    }

  ret = input_var_ptr->get(raw_data, in_dim_array);
  delete [] in_dim_array;
  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get raw data for var %s\n",
		       input_var_ptr->name());
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Successfully read %s\n",
		     input_var_ptr->name());

  // If input file is in post_proc format, just copy data
  //if (num_dims < IND_FM_NUM_VAR_DIM)
  //{
  memcpy(data, raw_data, sizeof(float)*raw_data_size);
  return(0);
      //}
  /*

  // Input file is in FM format
  int num_params = in_dim_array[IND_FM_PVALS_DIM];
  int num_sites = in_dim_array[IND_FM_SITE_DIM];
  int num_days = in_dim_array[IND_FM_DAYS_DIM];
  int num_times = in_dim_array[IND_FM_TIME_DIM];

  for (int ns = 0; ns < num_sites; ns++)
    for (int nd = 0; nd < num_days; nd++)
      for (int nt = 0; nt < num_times; nt++)
	{
	  int raw_index = ns * num_days * num_times * num_params +
	    nd * num_times * num_params + nt * num_params + IND_FM_MEAN;
	  int raw_index_conf = raw_index - IND_FM_MEAN + IND_FM_MEAN_CONF;

	  int index = ns * num_days * num_times + nd * num_times + nt;

	  if (raw_data[raw_index_conf] > MIN_CONFIDENCE)
	    data[index] = raw_data[raw_index];
	}
  */

  return(0);
}

