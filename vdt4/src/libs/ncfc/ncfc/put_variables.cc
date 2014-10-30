/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2007
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
// Module: put_variables.cc
//
// Author: Gerry Wiener
//
// Date:   5/23/07
//
// Description: Output variable data into netCDF file 
//     
//----------------------------------------------------------------------

// Include files 
#include <assert.h>
#include <stdlib.h>
#include "../include/ncfc/ncfc.hh"

using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

// set_count
// Set count fills in a count vector for use in nc_put_vara_*() based on the first parameter, size.
// On output the dimensions in the count vector will abide by the following:
// a. All dimensions in the count vector will be >= 1
// b. All dimensions = 1 in the count vector will be continguous and at the left of the count vector (For example, {1, 1, 3, 5}
// c. All dimensions > 1 other than the leftmost dimension > 1 in the count vector must be equivalent to the dimension stated for their positions in dims
// In this way the count vector represents a given number of complete rows, or complete planes, or complete volumes, etc.
// The size parameter must be set to satisfy the constraints established above (integral number of rows less than one plane, or an integral number of planes less than one volume, etc.
int set_count(int size, int ndims, size_t *dims, size_t *count)
{
  div_t divt;

  int prod = 1;
  int products[ndims];

  if (size <= 0 || ndims < 0)
    {
      return 1;
    }

  for (int i=0; i<ndims; i++)
    {
      count[i] = 1;
    }

  if (ndims == 0)
    {
      // case for scalar variables
      count[0] = 1;
      return 0;
    }

  for (int i=0; i<ndims; i++)
    {
      products[ndims-1-i] = prod;

      // Check for un-initialized unlimited dimension
      if ((ndims-1-i) == 0 && dims[ndims-1-i] == 0)
	{
	  dims[ndims-1-i] = size/prod;
	}

      prod = prod * dims[ndims-1-i];
      
      if (size < prod)
	{
	  divt = div(size, products[ndims-1-i]);
	  count[ndims-1-i] = divt.quot;

	  if (divt.rem == 0)
	    return 0;
	  else
	    return 1;
	}
      else
	{
	  count[ndims-1-i] = dims[ndims-1-i];
	}
    }

  return 0;
}


// set_append_count fills a count vector for use
// in nc_put_vara_*() based on the first parameter, size.  The
// parameter, size, must be an integral multiple of the product of
// dims[1], dims[2], ..., dims[ndims-1]. 
// Returns 0 on success and -1 on failure
int set_append_count(int size, int ndims, size_t *dims, size_t *count)
{
  div_t divt;

  if (size <= 0)
    {
      return -1;
    }
  
  size_t prod = 1;
  for (int i=1; i<ndims; i++)
    {
      prod *= dims[i];
    }

  divt = div(size, prod);

  if (divt.rem != 0)
    {
      return -1;
    }

  for (int i=1; i<ndims; i++)
    {
      count[i] = dims[i];
    }

  count[0] = divt.quot;

  return 0;
}


// Put variables of interest into netcdf file. The sizes are needed
// for the case of the unlimited dimension.  All variables in var_ptrs
// are assumed to be of a single dimension.  Returns 0 on success
// and -1 on failure

int NCFC_put_variables
(
 int out_ncid,			// I - output netcdf file id
 const vector<string> &var_names,	// I - variable names 
 const vector<void *> &var_ptrs,	// I - will contain variable data
 const vector<long int> &var_sizes,	// I - size of variable 
 string &error			// O - error string
)
{
  error = string("");
  for (int i=0; i<(int)var_names.size(); i++)
    {
      int var_dim_ids[NC_MAX_VAR_DIMS];     // variable shape 
      size_t var_dims[NC_MAX_VAR_DIMS];     // variable shape 
      int var_id;
      int var_ndims;                    // number of dims for variable
      int var_natts;                    // number of attributes for variable
      nc_type var_type;			// variable type

      int status = nc_inq_varid (out_ncid, var_names[i].c_str(), &var_id);
      if (status != NC_NOERR)
	{
	  error = string("NCFC_put_variables(): could not get id for variable ") + var_names[i];
	  return -1;
	}

      status = nc_inq_var(out_ncid, var_id, 0, &var_type, &var_ndims, var_dim_ids, &var_natts);
      if (status != NC_NOERR)
	{
	  error = string("NCFC_put_variables(): could not get type, dimension and attribute information for variable ") + var_names[i];
	  return -1;
	}

      for (int j=0; j<var_ndims; j++)
	{
	  status = nc_inq_dimlen(out_ncid, var_dim_ids[j], &var_dims[j]);
	  if (status != NC_NOERR)
	    {
	      error = string("NCFC_put_variables(): could not get dimension length information for variable ") + var_names[i];
	      return -1;
	    }
	}

      if (var_ndims >= 0)
	{
	  int out_ndims = var_ndims;
	  if (var_ndims == 0)
	    out_ndims = 1;
	  
	  size_t start[out_ndims];
	  size_t count[out_ndims];
	  
	  for (int j=0; j<out_ndims; j++)
	    {
	      start[j] = 0;
	    }


	  if (set_count(var_sizes[i], var_ndims, var_dims, count) != 0)
	    {
	      return -1;
	    }

	  switch (var_type)
	    {
	    case NC_FLOAT:
	      // put values to netCDF file
	      status = nc_put_vara_float(out_ncid, var_id, start, count, (float *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_INT:
	      // put values to netCDF file
	      status = nc_put_vara_int(out_ncid, var_id, start, count, (int *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_SHORT:
	      // put values to netCDF file
	      status = nc_put_vara_short(out_ncid, var_id, start, count, (short *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_DOUBLE:
	      // put values to netCDF file
	      status = nc_put_vara_double(out_ncid, var_id, start, count, (double *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_CHAR:
	      // put values to netCDF file
	      
	      status = nc_put_vara_text(out_ncid, var_id, start, count, (char *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_BYTE:
	      // put values to netCDF file
	      status = nc_put_vara_schar(out_ncid, var_id, start, count, (signed char *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_put_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    default:
	      // NC_NAT is not allowed
	      {
		error = string("NCFC_put_variables(): NC_NAT type is not supported for variable ") + var_names[i];
		return -1;
	      }
	      break;
	    }
	}
    }

  return 0;
}

// Append variables of interest to netcdf file. Assumes that the
// variables under consideration are using the unlimited record
// variable as the first dimension. The size specified in var_sizes
// must be an integral multiple of the product of the corresponding
// variable's dimensions other than the unlimited dimension. This
 // ensures that an integral number of records will be written. Returns
// 0 on success and -1 on failure.
int NCFC_append_variables
(
 int out_ncid,			// I - output netcdf file id
 const vector<string> &var_names,	// I - variable names 
 const vector<void *> &var_ptrs,	// I - will contain variable data
 const vector<long int> &var_sizes,	// I - size of variable 
 string &error			// O - error string
)
{
  error = string("");

  string unlim_name;
  int unlim_dim_id;
  int unlim_size;
  int ret = NCFC_get_unlim(out_ncid, unlim_name, &unlim_dim_id, &unlim_size);
  if (ret == -1)
    {
      error = string("NCFC_append_variables(): unlimited dimension is required in NCFC_append_variables()");
      return -1;
    }

  for (int i=0; i<(int)var_names.size(); i++)
    {
      int var_dim_ids[NC_MAX_VAR_DIMS];     // variable shape 
      size_t var_dims[NC_MAX_VAR_DIMS];     // variable shape 
      int var_id;
      int var_ndims;                    // number of dims for variable
      int var_natts;                    // number of attributes for variable
      nc_type var_type;			// variable type

      int status = nc_inq_varid (out_ncid, var_names[i].c_str(), &var_id);
      if (status != NC_NOERR)
	{
	  error = string("NCFC_append_variables(): could not get id for variable ") + var_names[i];
	  return -1;
	}

      status = nc_inq_var(out_ncid, var_id, 0, &var_type, &var_ndims, var_dim_ids, &var_natts);
      if (status != NC_NOERR)
	{
	  error = string("NCFC_append_variables(): could not get type, dimension and attribute information for variable ") + var_names[i];
	  return -1;
	}

      for (int j=0; j<var_ndims; j++)
	{
	  status = nc_inq_dimlen(out_ncid, var_dim_ids[j], &var_dims[j]);
	  if (status != NC_NOERR)
	    {
	      error = string("NCFC_append_variables(): could not get dimension length information for variable ") + var_names[i];
	      return -1;
	    }
	}

      if (var_ndims >= 0)
	{
	  int out_ndims = var_ndims;
	  if (var_ndims == 0)
	    out_ndims = 1;
	  
	  size_t start[out_ndims];
	  size_t count[out_ndims];
	  
	  // For array variables,
	  if (var_ndims > 0 && var_dim_ids[0] == unlim_dim_id)
	    {
	      start[0] = unlim_size;
	    }
	  else
	    {
	      start[0] = 0;
	    }

	  for (int j=1; j<out_ndims; j++)
	    {
	      start[j] = 0;
	    }

	  
	  if (set_append_count(var_sizes[i], var_ndims, var_dims, count) != 0)
	    {
	      error = string("NCFC_append_variables(): could not set_append_count() for variable: ") + var_names[i];
	      return -1;
	    }

	  switch (var_type)
	    {
	    case NC_FLOAT:
	      // put values to netCDF file
	      status = nc_put_vara_float(out_ncid, var_id, start, count, (float *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_INT:
	      // put values to netCDF file
	      status = nc_put_vara_int(out_ncid, var_id, start, count, (int *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_SHORT:
	      // put values to netCDF file
	      status = nc_put_vara_short(out_ncid, var_id, start, count, (short *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_DOUBLE:
	      // put values to netCDF file
	      status = nc_put_vara_double(out_ncid, var_id, start, count, (double *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_CHAR:
	      // put values to netCDF file
	      
	      status = nc_put_vara_text(out_ncid, var_id, start, count, (char *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    case NC_BYTE:
	      // put values to netCDF file
	      status = nc_put_vara_schar(out_ncid, var_id, start, count, (signed char *)var_ptrs[i]);
	      if (status != NC_NOERR)
		{
		  error = string("NCFC_append_variables(): could not put values for variable ") + var_names[i];
		  return -1;
		}
	      break;

	    default:
	      // NC_NAT is not allowed
	      {
		error = string("NCFC_append_variables(): NC_NAT type is not supported for variable ") + var_names[i];
		return -1;
	      }
	      break;
	    }
	}
    }

  return 0;
}

