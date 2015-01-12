//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: get_var_info.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2009/07/09 16:48:39 $
//
//==============================================================================

/**
 *
 * @file get_var_info.cc
 *
 * Get variable information
 *
 * @date 7/9/09
 */

// Include files 
#include "../include/ncfc/ncfc.hh"

using namespace std;

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions

int NCFC_get_var_info
(
 int in_ncid,			// I - input netcdf file id
 const vector<string> &var_names,	// I - variable names of interest
 vector<void *> &var_fill_ptrs,	// O - pointer is 0 if no fill value for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] var_fill_ptrs[i])
 vector<nc_type> &var_types,	// O - type of variable using netcdf types NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT and NC_DOUBLE
 vector<string> &var_units,	// O - variable units name strings
 vector<int> &var_ndims,	// O - number of dimensions of variable 
 vector<size_t *> &var_dims,	// O - actual dimensions of variable (contents can be deleted by caller using delete [] var_dims[i])
 vector<long int> &var_sizes,	// O - size of variable 
 string &error			// O - error description
)
{
  int status = 0;
  error = string("");

  for (unsigned int i=0; i<var_names.size(); i++)
    {
      int v_dims[NC_MAX_VAR_DIMS];     // variable shape 
      int v_id;
      int v_ndims;                     // number of dims for variable
      int v_natts;                      // number of attributes for variable
      nc_type v_type;		// variable type

      status = nc_inq_varid (in_ncid, var_names[i].c_str(), &v_id);
      if (status != NC_NOERR)
	{
	  error = string("could not get id for variable ") + var_names[i];
	  return -1;
	}
      
      status = nc_inq_var (in_ncid, v_id, 0, &v_type, &v_ndims, v_dims,
			   &v_natts);
      if (status != NC_NOERR)
	{
	  error = string("could not get type, dimension and attribute information for variable ") + var_names[i];
	  return -1;
	}
      

      // Get dimension information and variable size
      var_ndims.push_back(v_ndims);
      var_dims.push_back(new size_t[v_ndims]);

      long int size = 1;
      for (int j=0; j<v_ndims; j++)
	{
	  size_t dimlen;

	  status = nc_inq_dimlen(in_ncid, v_dims[j], &dimlen);
	  if (status != NC_NOERR)
	    {
	      error = string("could not get dimension length information for variable ") + var_names[i];
	      return -1;
	    }
      
	  var_dims[i][j] = dimlen;
	  size *= dimlen;
	}
      var_sizes.push_back(size);


      // Get variable type
      status = nc_inq_vartype(in_ncid, v_id, &v_type);
      if (status != NC_NOERR)
	{
	  error = string("could not get type for variable ") + var_names[i];
	  return -1;
	}
      

      // Get variable's units
      size_t attlen = 0;
      status = nc_inq_attlen(in_ncid, v_id, "units", &attlen);
      if (status != NC_NOERR)
	{
	  var_units.push_back(string(""));
	}
      else
	{
	  char units[attlen+1];
	  status = nc_get_att_text(in_ncid, v_id, "units", units);
	  units[attlen] = '\0';
	  var_units.push_back(string(units));
	}
      
      // Process variable according to type
      switch (v_type)
	{
	case NC_FLOAT:
	  {
	    float attr;
	    var_types.push_back(NC_FLOAT);

	    status = nc_get_att_float(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		float *new_attr_ptr = new float[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	case NC_INT:
	  {
	    int attr;
	    var_types.push_back(NC_INT);
	    status = nc_get_att_int(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		int *new_attr_ptr = new int[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	case NC_SHORT:
	  {
	    short attr;
	    var_types.push_back(NC_SHORT);
	    status = nc_get_att_short(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		short *new_attr_ptr = new short[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	case NC_CHAR:
	  {
	    char attr;
	    var_types.push_back(NC_CHAR);
	    status = nc_get_att_text(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		char *new_attr_ptr = new char[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	case NC_BYTE:
	  {
	    signed char attr;
	    var_types.push_back(NC_BYTE);
	    status = nc_get_att_schar(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		signed char *new_attr_ptr = new signed char[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	case NC_DOUBLE:
	  {
	    double attr;
	    var_types.push_back(NC_DOUBLE);
	    status = nc_get_att_double(in_ncid, v_id, "_FillValue", &attr);
	    if (status != NC_NOERR)
	      var_fill_ptrs.push_back((void *)0);
	    else
	      {
		double *new_attr_ptr = new double[1];
		*new_attr_ptr = attr;
		var_fill_ptrs.push_back(new_attr_ptr);
	      }
	  }
	  break;

	default:
	  // NC_NAT is not allowed
	  error = string("NC_NAT type is not supported for variable ") + var_names[i];
	  return -1;
	  break;
	}
    }

  return 0;
}
