/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2006
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
// Module: ncf_output_example.cc
//
// Author: Gerry Wiener
//
// Date:   10/9/06
//
// Description:
//     Example using ncf_output.hh methods.
//----------------------------------------------------------------------

// Include files 
#include <ncf/ncf_io.hh>

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

int main(int argc, char **argv)
{
  map<string, double *> var_map_double;
  map<string, float *> var_map_float;
  map<string, int *> var_map_int;
  map<string, char *> var_map_char;
  map<string, long *> var_map_dims;
  map<string, int> var_map_num_dims;
  map<string, map<string, NcValues *> *> var_atts;
  map<string, long> var_map_dim;

  float longitude[5] = {0,1,2,3,4};
  long lon_dim[1] = {5};
  var_map_float["longitude"] = longitude;
  var_map_dims["longitude"] = lon_dim;

  float latitude[6] = {6,5,4,3,2,1};
  long lat_dim[1] = {6};
  var_map_float["latitude"] = latitude;
  var_map_dims["latitude"] = lat_dim;

  NCF_io nc("ncf_output_ex.nc", NCF_Write);

  printf("putting float\n");
  nc.put_var_info(var_map_float, var_map_dims);

  double reftime = 56;
  var_map_double["reftime"] = &reftime;
  long reftime_dim[1] = {1};
  var_map_dims["reftime"] = reftime_dim;
  printf("putting reftime\n");

  int ret = nc.put_var_info(var_map_double, var_map_dims);
  printf("ret = %d\n", ret);

  double non_existent = 56;
  var_map_double["non_existent"] = &non_existent;
  long non_existent_dim[1] = {1};
  var_map_dims["non_existent"] = non_existent_dim;
  printf("putting map containing non-existent referent\n");

  ret = nc.put_var_info(var_map_double, var_map_dims);
  printf("ret = %d, error %s\n", ret, nc.error().c_str());
}
