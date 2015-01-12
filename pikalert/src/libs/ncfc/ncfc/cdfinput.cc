//==============================================================================
//
//   (c) Copyright, 2007 University Corporation for Atmospheric Research (UCAR).
//
//       National Center for Atmospheric Research(NCAR)
//       Research Applications Program(RAP)
//       P.O. Box 3000, Boulder, Colorado, 80307-3000, USA
//       All rights reserved. Licenced use only.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: cdfinput.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2009/12/03 15:44:45 $
//
//==============================================================================

/**
 *
 *  @file cdfinput.cc
 *
 *   Helper function to get variable input from cdf file
 *  
 *  @date 12/10/07
 */

// Include files 
#include "../include/ncfc/ncfc.hh"

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions

int NCFC_cdfinput(const char *in_file, const vector<string> &var_names, Var_input **var_input, const vector<string> &dim_names, vector<size_t> &dim_len, string &error)
{
  // Ingest netcdf input
  int in_ncid;
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      error = string("Error: could not open ") + string(in_file) + string("\n");
      return -1;
    }

  Var_input *var_input_ptr = new Var_input(in_ncid, var_names);
  int ret = var_input_ptr->error_status();
  if (ret != 0)
    {
      error = string("Error: var_input constructor failed, error: ") + var_input_ptr->error() + string(", return: ") + string(nc_strerror(ret)) + string("\n");
      return -1;
    }

  ret = NCFC_get_dims(in_ncid, dim_names, dim_len, error);
  if (ret != 0)
    {
      error = string("Error: NCFC_get_dims failed, error: ") + error + string("\n");
      return -1;
    }

  nc_close(in_ncid);

  *var_input = var_input_ptr;
  return 0;
}

int NCFC_var_dims_input(const char *in_file, const vector<string> &var_names, Var_input **var_input, const vector<string> &dim_names, vector<size_t> &dim_len, string &error)
{
  // Ingest netcdf input
  int in_ncid;
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      error = string("Error: could not open ") + string(in_file) + string("\n");
      return -1;
    }

  Var_input *var_input_ptr = new Var_input(in_ncid, var_names);
  int ret = var_input_ptr->error_status();
  if (ret != 0)
    {
      error = string("Error: var_input constructor failed, error: ") + var_input_ptr->error() + string(", return: ") + string(nc_strerror(ret)) + string("\n");
      return -1;
    }

  ret = NCFC_get_dims(in_ncid, dim_names, dim_len, error);
  if (ret != 0)
    {
      error = string("Error: NCFC_get_dims failed, error: ") + error + string("\n");
      return -1;
    }

  nc_close(in_ncid);

  *var_input = var_input_ptr;
  return 0;
}
