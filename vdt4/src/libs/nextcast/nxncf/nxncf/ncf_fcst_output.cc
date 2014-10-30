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
 * Module: nc_fcst_output.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   6/21/99
 *
 * Description:
 *     
 */

// Include files 
#include <stdio.h>
#include <netcdf.hh>
#include <nxncf/nxncf.hh>

// Constant, macro and type definitions 

// Global variables 

// Functions 


NXNCF_fcst_output::NXNCF_fcst_output(char *cdl_file, int *site_list, int num_sites, double forc_time, char *output_file)
{
  NcDim *dptr;
  long *dims;
  int i;
  int ret;
  int *store;
  NcVar *vptr;

  var_ptr = 0;
  edges = 0; 
  num_dims = 0;
  num_vals = 0;
  error_str = 0;
  ncfp = 0;

  // Generate netcdf file from cdl_file
  ret = NCF_cdl2nc(cdl_file, output_file);
  if (ret < 0)
    {
      error_str = "Could not generate output file from cdl";
      return;
    }

  // Open the netcdf output file
  ncfp = new NcFile(output_file, NcFile::Write);

  if (!ncfp->is_valid())
    {
      error_str = "Unable to open output file";
      return;
    }

  dptr = ncfp->get_dim(MAX_SITE_NUM_NAME);
  if (dptr == 0)
    {
      string err = string("Could not get ptr to dimension ") + string(MAX_SITE_NUM_NAME);
      error_str = err.c_str();
      return;
    }

  max_site_num = dptr->size();

  vptr = ncfp->get_var(NUM_SITES_NAME);
  if (vptr == 0)
    {
      string err = string("Could not get ptr to variable ") + string(NUM_SITES_NAME);
      error_str = err.c_str();
      return;
    }

  // If max_site_num dimension is unlimited, set it to num_sites so we can
  // handle fixed and variable number of sites.
  if (dptr->is_unlimited())
    max_site_num = num_sites;

  if (num_sites > max_site_num)
    {
      string err = string(NUM_SITES_NAME) + string(" > ") + string(MAX_SITE_NUM_NAME);
      error_str = err.c_str();
      return;
    }
    
  vptr->put(&num_sites);

  vptr = ncfp->get_var(SITE_LIST_NAME);
  if (vptr == 0)
    {
      string err = string("Could not get ptr to variable ") + string(SITE_LIST_NAME);
      error_str = err.c_str();
      return;
    }

  if (num_sites != max_site_num)
    {
      store = new int[max_site_num];
      for (i=0; i<num_sites; i++)
	store[i] = site_list[i];
      for (i=num_sites; i<max_site_num; i++)
	store[i] = NC_FILL_INT;
   
      dims = vptr->edges();
      dims[0] = max_site_num;
      vptr->put(store, dims);
      delete [] store;
      delete [] dims;
    }
  else
    {
      dims = vptr->edges();
      dims[0] = max_site_num;
      vptr->put(site_list, dims);
      delete [] dims;
    }

  vptr = ncfp->get_var(FORC_TIME_NAME);
  if (vptr == 0)
    {
      string err = string("Could not get ptr to variable ") + string(FORC_TIME_NAME);
      error_str = err.c_str();
      return;
    }

  vptr->put(&forc_time);

  if (NXNCF_write_creation_time(*ncfp)<0)
    {
      error_str = "Unable to write creation time";
      return;
    }
}


NXNCF_fcst_output::~NXNCF_fcst_output()
{
  if(0 != ncfp)
    delete [] edges;

  if(0 != ncfp)
    delete ncfp;
  //ncfp->close();
}


int NXNCF_fcst_output::set_var(const char *var_name)
{
  if(0 != edges)
    {
      delete [] edges;
      edges = 0;
    }
  num_dims = 0;
  num_vals = 0;

  // Get a pointer to the variable
  var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    {
      error_str = "Could not create ptr to variable";
      return(-1);
    }

  num_dims = var_ptr->num_dims();
  edges = var_ptr->edges();
  if (num_dims > 1)
    edges[0] = max_site_num;
  num_vals = 1;
  for (int i=0; i<num_dims; i++)
    num_vals *= edges[i];

  return(0);
}

// ********************  NOTE ****************************************

// Use of the write_var() set of methods below is discouraged because
// it is possible to try and write more data than we actually have. 
// Use the write_var_slice() methods instead which pass the data array
// edges as an argument and also writes only as much as will fit in the
// output file.

// ********************  NOTE ****************************************

int NXNCF_fcst_output::write_var(int *val)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  if (num_vals == 1)
    var_ptr->put(val);
  else
    var_ptr->put(val, edges);
  return(0);
}

int NXNCF_fcst_output::write_var(double *val)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  if (num_vals == 1)
    var_ptr->put(val);
  else
    var_ptr->put(val, edges);
  return(0);
}

int NXNCF_fcst_output::write_var(float *val)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  if (num_vals == 1)
    var_ptr->put(val);
  else
    var_ptr->put(val, edges);
  return(0);
}

int NXNCF_fcst_output::write_var(char *val)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  if (num_vals == 1)
    var_ptr->put(val);
  else
    var_ptr->put(val, edges);
  return(0);
}

int NXNCF_fcst_output::write_var_slice(double *array, long *sl_edges)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  // Adjust edges to prevent writing too much data
  for (int i=0; i<num_dims; i++)
    if (sl_edges[i] > edges[i])
      sl_edges[i] = edges[i];

  if (!var_ptr->put(array, sl_edges)) {
    error_str = "Unable to write variable";
    return (-1);
  }
  else
    return(0);
}

int NXNCF_fcst_output::write_var_slice(float *array, long *sl_edges)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }
  
  // Adjust edges to prevent writing too much data
  for (int i=0; i<num_dims; i++)
    if (sl_edges[i] > edges[i])
      sl_edges[i] = edges[i];

  if (!var_ptr->put(array, sl_edges)) {
    error_str = "Unable to write variable";
    return (-1);
  }
  else
    return(0);
}

int NXNCF_fcst_output::write_var_slice(int *array, long *sl_edges)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }
  
  // Adjust edges to prevent writing too much data
  for (int i=0; i<num_dims; i++)
    if (sl_edges[i] > edges[i])
      sl_edges[i] = edges[i];

  if (!var_ptr->put(array, sl_edges)) {
    error_str = "Unable to write variable";
    return (-1);
  }
  else
    return(0);
}

int NXNCF_fcst_output::write_var_slice(char *array, long *sl_edges)
{
  if (num_vals == 0)
    {
      error_str = "Must call set_var before calling write_var";
      return(-1);
    }

  // Adjust edges to prevent writing too much data
  for (int i=0; i<num_dims; i++)
    if (sl_edges[i] > edges[i])
      sl_edges[i] = edges[i];

  if (!var_ptr->put(array, sl_edges)) {
    error_str = "Unable to write variable";
    return (-1);
  }
  else
    return(0);
}

int NXNCF_fcst_output::write_test_ramp
(
 int dims_to_ramp		// Number of ending dimensions for the ramp (must be less than the number of dimensions for the variable
 )
{
  long *edge_dim;
  int i;
  int size = 1;

  if (edges == 0)
    {
      error_str = "Must call set_var before calling write_test_ramp";
      return(-1);
    }

  edge_dim = new long[num_dims];
  for (i=0; i<num_dims; i++)
    edge_dim[i] = edges[i];

  if (dims_to_ramp > num_dims)
    {
      error_str = "Arg dims_to_ramp > num_dims.";
      return(-1);
    }
  
  for (i=num_dims-dims_to_ramp; i<num_dims; i++)
    size *= edge_dim[i];

  for (i=0; i<num_dims-dims_to_ramp; i++)
    edge_dim[i] = 1;

  float *array = new float[size];
  for (i=1; i<=size; i++)
    array[i-1] = i;

  var_ptr->put(array, edge_dim);

  delete [] array;
  delete [] edge_dim;
  return(0);
}

int NXNCF_fcst_output::get_dim_size(const char *dim_name)
{
  int dim_size = NCF_get_dim_size(ncfp, dim_name);
  return (dim_size);
}

