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


int read_nbr_info(NcFile *nbr_file, long *max_nbr, int *num_nbr_sites,
		  int **nbr_site_list, double **nbr_list)
{
  NcVar *nbr_var_ptr;
  int ret;

  NcDim *max_nbr_dim =nbr_file->get_dim("max_nbr");
  *max_nbr = max_nbr_dim->size();

  nbr_var_ptr = nbr_var_ptr = nbr_file->get_var(NUM_SITES_NAME);
  if (!nbr_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", NUM_SITES_NAME);
      return(1);
    }

  *num_nbr_sites = nbr_var_ptr->as_int(0);
  //printf("num nbr sites %d\n", *num_nbr_sites);

  nbr_var_ptr =nbr_file->get_var(SITE_LIST_NAME);
  if (!nbr_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for %s\n", SITE_LIST_NAME);
      return(1);
    }

  long *dim_array =nbr_var_ptr->edges();
  *nbr_site_list = new int[dim_array[0]];

  ret = nbr_var_ptr->get(*nbr_site_list, dim_array);
  delete [] dim_array;

  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
		       nbr_var_ptr->name());
      return(1);
    }


  nbr_var_ptr =nbr_file->get_var("nbr_list");
  if (!nbr_var_ptr)
    {
      proc_log->write_time("Error: Unable to get ptr for nbr_list\n",
			   nbr_var_ptr->name());
      return(1);
    }

  dim_array =nbr_var_ptr->edges();
  *nbr_list = new double[dim_array[0]*(*max_nbr)*2];

  ret = nbr_var_ptr->get(*nbr_list, dim_array);
  delete [] dim_array;

  if (ret == 0)
    {
      proc_log->write_time("Error: Unable to get data for var %s\n",
		       nbr_var_ptr->name());
      return(1);
    }


  return(0);
}

int get_nbr_list(int site_num, double *nbr_list, int *nbr_site_list,
		 int num_nbr_sites, int num_nbr, int *nbr, float *dist)
{
  int i, ns;

  i = 0;
  while (site_num != nbr_site_list[i] && i < num_nbr_sites)
    i++;

  if (i == num_nbr_sites)
    return(1);

  for (ns = 0; ns < num_nbr; ns++)
    {
      if (nbr_list[i*num_nbr*2 + ns*2 + 0] != NC_FILL_DOUBLE &&
	  nbr_list[i*num_nbr*2 + ns*2 + 1] != NC_FILL_DOUBLE)
	{
	  nbr[ns] = (int)nbr_list[i*num_nbr*2 + ns*2 + 0];
	  dist[ns] = (float)nbr_list[i*num_nbr*2 + ns*2 + 1];
	  //printf("nbr %d %.1f\n", nbr[ns], dist[ns]);
	}
      else
	{
	  nbr[ns] = -1;
	  dist[ns] = 99999999.;
	}
    }

  return(0);
}

