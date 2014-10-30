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
// Module: get_variables.cc
//
// Author: Gerry Wiener
//
// Date:   5/15/07
//
// Description: Ingest variable data from netCDF file and store in vector
//     
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"

using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

// Extract variables of interest from netcdf file. All of variables contents will be returned.
// All variables are assumed to be of a single dimension.
// Returns number of records or -1 on failure
int NCFC_get_variables
(
 int in_ncid,			// I - input netcdf file id
 const vector<string> &var_names,	// I - variable names of interest
 vector<void *> &var_ptrs,	// O - allocates storage for each variable (contents can be deleted by caller using delete [] var_ptrs[i])
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


#ifdef NOTNOW
  int rec_id;
  status = nc_inq_unlimdim(in_ncid, &rec_id); // get ID of unlimited dimension 
  if (status != NC_NOERR)
    {
      error = string("could not get record id for unlimited dimension");
      return status;
    }

  char recname[NC_MAX_NAME+1];
  size_t num_rec = 0;

  // get unlimited dimension name and current length 
  status = nc_inq_dim(in_ncid, rec_id, recname, &num_rec);
  if (status != NC_NOERR)
    {
      error = string("could not get unlimited dimension");
      return status;
    }

#endif

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

	    var_ptrs.push_back(new float[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_float(in_ncid, v_id, (float *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
	      }

	  }
	  break;

	case NC_INT:
	  {
	    int attr;
	    var_ptrs.push_back(new int[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_int(in_ncid, v_id, (int *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
	      }
	  }
	  break;

	case NC_SHORT:
	  {
	    short attr;
	    var_ptrs.push_back(new short[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_short(in_ncid, v_id, (short *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
	      }
	  }
	  break;

	case NC_CHAR:
	  {
	    char attr;
	    var_ptrs.push_back(new char[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_text(in_ncid, v_id, (char *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
	      }
	  }
	  break;

	case NC_BYTE:
	  {
	    signed char attr;
	    var_ptrs.push_back(new signed char[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_schar(in_ncid, v_id, (signed char *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
	      }
	  }
	  break;

	case NC_DOUBLE:
	  {
	    double attr;
	    var_ptrs.push_back(new double[size]);
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

	    // read values from netCDF variable 
	    status = nc_get_var_double(in_ncid, v_id, (double *)var_ptrs[i]);
	    if (status != NC_NOERR)
	      {
		error = string("could not get values for variable ") + var_names[i];
		return -1;
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

#ifdef GET_VARIABLES
int main(int argc, char **argv)
{
  int in_ncid;
  int out_ncid;

  char *in_file = "junk.nc";
  char *out_file = "out.nc";
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    return 1;

  status = nc_open(out_file, NC_WRITE, &out_ncid);
  if (status != NC_NOERR)
    return 1;

  vector<string> var_names;
  vector<void *> var_fill_ptrs;
  vector<void *> var_ptrs;
  vector<nc_type> var_types;
  vector<long> var_sizes;

  var_names.push_back("xyz");
  var_names.push_back("lat");
  var_names.push_back("lon");
  var_names.push_back("time");
  var_names.push_back("fl_level");
  var_names.push_back("wx_tmp");
  var_names.push_back("wx_wdr");
  var_names.push_back("wx_wsp");
  var_names.push_back("trb1_base");
  var_names.push_back("trb1_top");
  var_names.push_back("trb1_frq");
  var_names.push_back("trb1_int");
  var_names.push_back("trb1_type");
  var_names.push_back("trb2_base");
  var_names.push_back("trb2_top");
  var_names.push_back("trb2_frq");
  var_names.push_back("trb2_int");
  var_names.push_back("trb2_type");

  int ret = NCFC_get_variables(in_ncid, var_names, var_ptrs, var_fill_ptrs, var_types, var_sizes);

  if (ret < 0)
    {
      printf("error %s\n", nc_strerror(ret));
      return -1;
    }

  for (unsigned int i=0; i<var_names.size(); i++)
    {
      if (var_types[i] == NC_FLOAT)
	{
	  printf("field %s\n", var_names[i].c_str());
	  if (var_fill_ptrs[i] != 0)
	    printf("fill %f\n", ((float *)var_fill_ptrs[i])[0]);
	  for (int j=0; j<(int)var_sizes[i]; j++)
	    {
	      printf("%f\n", ((float *)var_ptrs[i])[j]);
	    }
	}
      else if (var_types[i] == NC_DOUBLE)
	{
	  printf("field %s\n", var_names[i].c_str());
	  if (var_fill_ptrs[i] != 0)
	    printf("fill %g\n", ((double *)var_fill_ptrs[i])[0]);
	  for (int j=0; j<(int)var_sizes[i]; j++)
	    {
	      printf("%lf\n", ((double *)var_ptrs[i])[j]);
	    }
	}
      else if (var_types[i] == NC_INT)
	{
	  printf("field %s\n", var_names[i].c_str());
	  if (var_fill_ptrs[i] != 0)
	    printf("fill %d\n", ((int *)var_fill_ptrs[i])[0]);
	  for (int j=0; j<(int)var_sizes[i]; j++)
	    {
	      printf("%d\n", ((int *)var_ptrs[i])[j]);
	    }
	}
      else if (var_types[i] == NC_SHORT)
	{
	  printf("field %s\n", var_names[i].c_str());
	  if (var_fill_ptrs[i] != 0)
	    printf("fill %hd\n", ((short *)var_fill_ptrs[i])[0]);
	  for (int j=0; j<(int)var_sizes[i]; j++)
	    {
	      printf("%hd\n", ((short *)var_ptrs[i])[j]);
	    }
	}
    }
}
#endif
