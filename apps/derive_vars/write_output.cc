// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
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
		 int  *output_site_array, long *max_output_sites,
		 long *num_output_days, long *num_output_times_per_day)
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
      proc_log->write_time("Error: Unable to put data for var %s\n", FORC_TIME_NAME);
      return(1);
    }
  else
    proc_log->write_time(1,"Info: Forecast time is %f\n",forc_time);


  // Get site dimension, handle unlimited size.
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
  *num_output_times_per_day = num_output_times_dim->size();

  NcDim *num_output_days_dim = output_file->get_dim(DAYS_DIM_NAME);
  *num_output_days = num_output_days_dim->size();

  *max_output_sites = max_sites;


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
