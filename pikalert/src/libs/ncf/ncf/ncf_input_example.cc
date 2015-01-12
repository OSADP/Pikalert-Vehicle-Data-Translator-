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
// Module: ncf_input_example.cc
//
// Author: Gerry Wiener
//
// Date:   10/9/06
//
// Description:
//     Example on how to use ncf_input.hh calls.
//----------------------------------------------------------------------

// Include files 
#include <netcdf.hh>
#include <cstdlib>

#include <ncf/ncf_io.hh>

using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

int main(int argc, char **argv)
{
  map<string, long> var_dim;

  // Open input
  NCF_io nc("ncf_input_ex.nc", NCF_ReadOnly);

  // Check for error
  if (nc.error() != string(""))
    {
      fprintf(stderr, "Error: %s\n", nc.error().c_str());
      exit(1);
    }

  // Get global dimension information
  nc.get_dim_info(var_dim);

  // Print global dimension information out
  printf("id_len %ld\n", var_dim["id_len"]);
  printf("num_score %ld\n", var_dim["num_score"]);
  printf("recNum %ld\n", var_dim["recNum"]);


  // Now use NCF_input_map class -- note that no freeing is required here
  map<string, int> double_input_map;
  double_input_map["reftime"] = 0;
  double_input_map["valtime"] = 0;

  NCF_input_map<double> imd(double_input_map);
  nc.get_var_info(imd);
  printf("reftime imd.var_data = %g, reftime imd.var_num_dims %d\n", imd.var_data["reftime"][0], imd.var_num_dims["reftime"]);
  printf("valtime imd.var_data = %g, valtime imd.var_num_dims %d\n", imd.var_data["valtime"][0], imd.var_num_dims["valtime"]);

  map<string, int> float_input_map;
  float_input_map["latitude"] = 0;
  float_input_map["longitude"] = 0;

  NCF_input_map<float> imf(float_input_map);
  nc.get_var_info(imf);
  printf("imf.var_data = %g\n", imf.var_data["latitude"][0]);
  printf("imf.var_data = %g\n", imf.var_data["longitude"][0]);


  // Now set up some maps for the other NCF_io get_var_info() methods
  map<string, double *> var_double;
  map<string, float *> var_float;
  map<string, int *> var_int;
  map<string, char *> var_char;

  map<string, long *> var_dims_double;
  map<string, int> var_num_dims_double;
  map<string, map<string, NcValues *> *> var_atts_double;
  map<string, long> var_dim_double;

  map<string, long *> var_dims_float;
  map<string, int> var_num_dims_float;
  map<string, map<string, NcValues *> *> var_atts_float;
  map<string, long> var_dim_float;

  map<string, long *> var_dims_int;
  map<string, int> var_num_dims_int;
  map<string, map<string, NcValues *> *> var_atts_int;
  map<string, long> var_dim_int;

  map<string, long *> var_dims_char;
  map<string, int> var_num_dims_char;
  map<string, map<string, NcValues *> *> var_atts_char;
  map<string, long> var_dim_char;

  // Formulate list of double variables of interest
  var_double["valtime"] = 0;
  var_double["reftime"] = 0;
  
  // Extract double variable data from file and store in maps
  nc.get_var_info(var_double, var_num_dims_double, var_dims_double, var_atts_double);

  // Print some information
  double *reftime = var_double["reftime"];
  double *valtime = var_double["valtime"];

  printf("reftime num_dims: %d\n", var_num_dims_double["reftime"]);
  printf("valtime num_dims: %d\n", var_num_dims_double["valtime"]);
  printf("reftime: %g\n", reftime[0]);
  printf("valtime: %g\n", valtime[0]);


  // Formulate list of floating point variables of interest
  var_float["latitude"] = 0;
  var_float["longitude"] = 0;
  var_float["elevation"] = 0;
  
  // Extract float data from file and store in maps. Note that
  // var_num_dims, var_dims and var_atts are re-used from before --
  // the field names for the float variables have to be different from
  // those of the previous variables so the previous variable
  // information will not be overwritten.
  nc.get_var_info(var_float, var_num_dims_float, var_dims_float, var_atts_float);

  // Print some information
  float *lat = var_float["latitude"];
  float *lon = var_float["longitude"];
  float *elev = var_float["elevation"];
  for (int i=0; i<var_dims_float["latitude"][0]; i++)
    {
      printf("lat[%d]: %g\n", i, lat[i]);
    }
  for (int i=0; i<var_dims_float["longitude"][0]; i++)
    {
      printf("lon[%d]: %g\n", i, lon[i]);
    }
  for (int i=0; i<var_dims_float["elevation"][0]; i++)
    {
      printf("elev[%d]: %g\n", i, elev[i]);
    }


  // Formulate list of integer variables of interest
  var_int["ceiling_score"] = 0;
  var_int["visibility_score"] = 0;
  
  // Extract integer data from file and store in maps. Note that
  // var_num_dims, var_dims and var_atts are re-used from before --
  // the field names for the integer variables have to be different from
  // those of the previous variables so the previous variable information
  // will not be overwritten.
  nc.get_var_info(var_int, var_num_dims_int, var_dims_int, var_atts_int);

  // Print some information
  int *ceiling_score = var_int["ceiling_score"];
  int *visibility_score = var_int["visibility_score"];

  printf("ceiling scores\n");
  for (int i=0; i<var_dims_int["ceiling_score"][0]; i++)
    {
      for (int j=0; j<var_dims_int["ceiling_score"][1]; j++)
	{
	  printf("%d", ceiling_score[i*var_dims_int["ceiling_score"][1] + j]);
	}
      printf("\n");
    }

  printf("visibility scores\n");
  for (int i=0; i<var_dims_int["visibility_score"][0]; i++)
    {
      for (int j=0; j<var_dims_int["visibility_score"][1]; j++)
	{
	  printf("%d", visibility_score[i*var_dims_int["visibility_score"][1] + j]);
	}
      printf("\n");
    }

  // Formulate list of character variables of interest
  var_char["station_id"] = 0;
  
  // Extract float variable data from file and store in maps
  nc.get_var_info(var_char, var_num_dims_char, var_dims_char, var_atts_char);

  printf("stations\n");

  // Print some information
  for (int i=0; i<var_dims_char["station_id"][0]; i++)
    {
      for (int j=0; j<var_dims_char["station_id"][1]; j++)
	{
	  printf("%c", var_char["station_id"][i*var_dims_char["station_id"][1] + j]);
	}
      printf("\n");
    }

  nc.free_var_info(var_float, var_dims_float, var_atts_float);
  nc.free_var_info(var_double, var_dims_double, var_atts_double);
  nc.free_var_info(var_int, var_dims_int, var_atts_int);
  nc.free_var_info(var_char, var_dims_char, var_atts_char);


  // Now try to get a non-existent variable
  var_char["non_existent"] = 0;
  
  // Extract float variable data from file and store in maps
  int ret = nc.get_var_info(var_char, var_num_dims_char, var_dims_char, var_atts_char);

  printf("non_existent variable return = %d, error = %s\n", ret, nc.error().c_str());
}

