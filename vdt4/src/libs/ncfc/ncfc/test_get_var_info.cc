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
// Module: test_get_var_info.cc
//
// Author: Gerry Wiener
//
// Date:   5/25/07
//
// Description:
//     Sample code illustrating NCFC_get_var_info() in ncfc
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"
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
  fprintf(stderr, "For examples: test_get_variables test.nc temperature\n");
}

int read_file(char *in_file,   vector<string> &in_var_names)
{
  int in_ncid;

  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      fprintf(stderr, "Error: could not open %s\n", in_file);
      return -1;
    }

  vector<void *> in_var_fill_ptrs;
  vector<nc_type> in_var_types;
  vector<int> in_var_ndims;
  vector<size_t *> in_var_dims;
  vector<long> in_var_sizes;
  vector<string> in_var_units;
  string error;

  int ret = NCFC_get_var_info(in_ncid, in_var_names, in_var_fill_ptrs, in_var_types, in_var_units, in_var_ndims, in_var_dims, in_var_sizes, error);
  if (ret < 0)
    {
      fprintf(stderr, "Error: NCFC_get_var_info() failed, error: %s, return: %s\n", error.c_str(), nc_strerror(ret));
      return -1;
    }

  for (unsigned int i=0; i<in_var_names.size(); i++)
    {
      if (in_var_types[i] == NC_FLOAT)
	{
	  printf("var %s, units %s, type NC_FLOAT\n", in_var_names[i].c_str(), in_var_units[i].c_str());
	  if (in_var_fill_ptrs[i] != (void *)0)
	    {
	      printf("fill_value[%d] = %f\n", i, ((float *)in_var_fill_ptrs[i])[0]);
	    }
	  else
	    {
	      printf("no fill value\n");
	    }
	}
      else if (in_var_types[i] == NC_INT)
	{
	  printf("var %s, units %s, type NC_INT\n", in_var_names[i].c_str(), in_var_units[i].c_str());	  
	  if (in_var_fill_ptrs[i] != (void *)0)
	    {
	      printf("fill_value[%d] = %d\n", i, ((int *)in_var_fill_ptrs[i])[0]);
	    }
	  else
	    {
	      printf("no fill value\n");
	    }
	}
      else if (in_var_types[i] == NC_DOUBLE)
	{
	  printf("var %s, units %s, type NC_DOUBLE\n", in_var_names[i].c_str(), in_var_units[i].c_str());	  	  
	  if (in_var_fill_ptrs[i] != (void *)0)
	    {
	      printf("fill_value[%d] = %lf\n", i, ((double *)in_var_fill_ptrs[i])[0]);
	    }
	  else
	    {
	      printf("no fill value\n");
	    }
	}
    }

  // If you're not exiting and want to release memory before continuing
  for (unsigned int i=0; i<in_var_fill_ptrs.size(); i++)
    {
      delete [] (size_t *)in_var_dims[i];

      if (in_var_types[i] == NC_FLOAT)
	{
	  delete [] (float *)in_var_fill_ptrs[i];
	}
      else if (in_var_types[i] == NC_INT)
	{
	  delete [] (int *)in_var_fill_ptrs[i];
	}
      if (in_var_types[i] == NC_DOUBLE)
	{
	  delete [] (double *)in_var_fill_ptrs[i];
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

  int ret = read_file(in_file, in_var_names);

  if (ret < 0)
    {
      fprintf(stderr, "Error: read file problem\n");
      return 1;
    }
  else
    {
      return 0;
    }
}


