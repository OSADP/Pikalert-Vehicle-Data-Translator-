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
// Module: get_dims.cc
//
// Author: Gerry Wiener
//
// Date:   10/2/07
//
// Description:
//     Get dimensions given a vector of dimension names
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"

// Constant, macro and type definitions 

// Global variables 

// Functions and objects
int NCFC_get_dims
(
  int in_ncid,				// I - input netcdf file id
  const vector<string> &dim_names,	// I - dimension names of interest
  vector<size_t> &dim_len,		// O - dimension lengths
  string &error				// O - error string
)
{
  int dim_id = -1;
  size_t dim_length;
  int error_occured = 0;
  int status;

  error = string("");
  for (unsigned int i=0; i<dim_names.size(); i++)
    {
      status = nc_inq_dimid(in_ncid, dim_names[i].c_str(), &dim_id);  // get ID for dimension 
      if (status != NC_NOERR)
	{
	  error = string("could not get dimension id for ") + dim_names[i];
	  error_occured = -1;
	  return error_occured;
	}

      status = nc_inq_dimlen(in_ncid, dim_id, &dim_length); // get dimension length 

      if (status != NC_NOERR) 
	{
	  error = string("could not get dimension length for ") + dim_names[i];
	  error_occured = -1;
	  return error_occured;
	}

      dim_len.push_back(dim_length);
    }

  return error_occured;
}
