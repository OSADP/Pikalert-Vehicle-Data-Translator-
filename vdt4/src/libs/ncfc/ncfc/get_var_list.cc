//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: get_var_list.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2008/05/08 20:25:29 $
//
//==============================================================================

/**
 *
 * @file get_var_list.cc
 *
 * Get list of variables and their types from a netcdf file.
 *
 * @date 5/2/08
 *
 */

// Include files 
#include "../include/ncfc/ncfc.hh"
#include <stdio.h>

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions


int NCFC_get_var_list(int ncid, vector<string> &var_name, vector<nc_type> &var_type)
{
  int ndims;
  int nvars;
  int ngatts;
  int unlimdimid;

  int status = nc_inq(ncid, &ndims, &nvars, &ngatts, &unlimdimid);
  if (status != NC_NOERR)
    return -1;

  char name[NC_MAX_NAME+1];
  nc_type single_var_type;
  int ndim;
  int dimid[NC_MAX_VAR_DIMS];
  int natt;

  for (int i=0; i<nvars; i++)
    {
      nc_inq_var(ncid, i, name, &single_var_type, &ndim, dimid, &natt);
      var_name.push_back(name);
      var_type.push_back(single_var_type);
    }

  return 0;
}

#ifdef NOTNOW
int main(int argc, char **argv)
{
  int ncid;
  int status = nc_open("/scratch/gerry/weatherbug.nc", NC_WRITE, &ncid);
  if (status != NC_NOERR)
    return 1;
  get_var_list(ncid);
  
}
#endif
