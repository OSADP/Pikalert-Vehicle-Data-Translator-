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
// Module: test_get_variables.cc
//
// Author: Gerry Wiener
//
// Date:   5/25/07
//
// Description:
//     Sample code illustrating NCFC_get_variables() in ncfc
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"
#include <netcdf.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

void usage(char *prog)
{
  fprintf(stderr, "Usage: %s in_file field(s)\n\n", prog);
  fprintf(stderr, "For examples: test_var_input test.nc temperature\n");
}

int print_fields(const vector<string> &var_names, const Var_input &var_input)
{
  const vector<string> in_var_units = var_input.get_var_units();
  const vector<long int> in_var_sizes = var_input.get_var_sizes();
  const vector<nc_type> in_var_types = var_input.get_var_types();
  const vector<void *> in_var_ptrs = var_input.get_var_ptrs();
  const vector<size_t *> in_var_dims = var_input.get_var_dims();
  const vector<int> in_var_ndims = var_input.get_var_ndims();

  for (int i=0; i<(int)in_var_dims.size(); i++)
    {
      printf("var %d has %d dims are: \n", i, in_var_ndims[i]);
      for (int j=0; j<(int)in_var_ndims[i]; j++)
	{
	  printf("%ld ", in_var_dims[i][j]);
	}
      printf("\n");
    }

  for (unsigned int i=0; i<var_names.size(); i++)
    {
      printf("var %s, units %s\n", var_names[i].c_str(), in_var_units[i].c_str());

      for (int j=0; j<in_var_sizes[i]; j++)
	{
	  if (in_var_types[i] == NC_FLOAT)
	    {
	      printf("value[%d] = %f\n", j, ((float *)in_var_ptrs[i])[j]);
	    }
	  else if (in_var_types[i] == NC_INT)
	    {
	      printf("value[%d] = %d\n", j, ((int *)in_var_ptrs[i])[j]);
	    }
	  else if (in_var_types[i] == NC_DOUBLE)
	    {
	      printf("value[%d] = %lf\n", j, ((double *)in_var_ptrs[i])[j]);
	    }
	}
    }

  return 0;
}


int main(int argc, char **argv)
{
  char *in_file = 0;
  vector<string> in_var_names;

  if (argc < 3)
    {
      usage(argv[0]);
      exit(2);
    }

  in_file = argv[1];

  for (int i=2; i<argc; i++)
    {
      in_var_names.push_back(string(argv[i]));
    }

  vector<string> dim_names;
  vector<size_t> dim_len;
  Var_input *var_input = 0;
  string error;
  int ret = NCFC_cdfinput(in_file, in_var_names, &var_input, dim_names, dim_len, error);
  if (ret < 0)
    {
      fprintf(stderr, "Error: input failure on %s\n", in_file);
      return 1;
    }
  
  ret = print_fields(in_var_names, *var_input);

  if (ret < 0)
    {
      fprintf(stderr, "Error: could not dump fields from %s\n", in_file);
      return 1;
    }
  else
    {
      return 0;
    }

  delete var_input;
}
