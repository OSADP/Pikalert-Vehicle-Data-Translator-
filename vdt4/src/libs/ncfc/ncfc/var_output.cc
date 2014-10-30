//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: var_output.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2010/10/06 19:23:44 $
//
//==============================================================================

/**
 *
 * @file var_output.cc
 *
 * Class for initializing variable output information to write to netcdf files.
 *
 * @date 5/2/08
 *
 */

// Include files 
#include "../include/ncfc/ncfc.hh"

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions

Var_output::Var_output()
{
  error = "";
}

Var_output::Var_output(const vector<string> &var_names_param, const vector<nc_type> &var_types_param, const vector<long int> &var_sizes_param)
{
  error = "";

  var_names = var_names_param;
  var_types = var_types_param;
  var_sizes = var_sizes_param;
  for (int i=0; i<(int)var_names_param.size(); i++)
    {
      long int size = var_sizes_param[i];

      switch (var_types_param[i])
	{
	case NC_BYTE:
	  var_ptrs.push_back(new signed char[size]);
	  break;

	case NC_CHAR:
	  var_ptrs.push_back(new char[size]);
	  break;

	case NC_SHORT:
	  var_ptrs.push_back(new short[size]);
	  break;

	case NC_INT:
	  var_ptrs.push_back(new int[size]);
	  break;

	case NC_FLOAT:
	  var_ptrs.push_back(new float[size]);
	  break;

	case NC_DOUBLE:
	  var_ptrs.push_back(new double[size]);
	  break;

	case NC_NAT:
	  error = "NC_NAT not supported by var_output";
	  return;

	default:
	  error = "undefined type";
	  return;
	}
    }
}

Var_output::~Var_output()
{
  for (int i=0; i<(int)var_names.size(); i++)
    {
      switch (var_types[i])
	{
	case NC_BYTE:
	  delete [] (signed char *)(var_ptrs[i]);
	  break;

	case NC_CHAR:
	  delete [] (char *)(var_ptrs[i]);
	  break;

	case NC_SHORT:
	  delete [] (short *)(var_ptrs[i]);
	  break;

	case NC_INT:
	  delete [] (int *)(var_ptrs[i]);
	  break;

	case NC_FLOAT:
	  delete [] (float *)(var_ptrs[i]);
	  break;

	case NC_DOUBLE:
	  delete [] (double *)(var_ptrs[i]);
	  break;

	case NC_NAT:
	  // not supported
	  break;
	}
    }
}

