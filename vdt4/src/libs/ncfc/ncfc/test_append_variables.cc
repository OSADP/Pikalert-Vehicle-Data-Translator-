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
// Module: test_append_variables.cc
//
// Author: Gerry Wiener
//
// Date:   5/25/07
//
// Description:
//     Sample code illustrating NCFC_append_variables() in ncfc
//----------------------------------------------------------------------

// Include files 
#include <string>
#include <vector>
#include "../include/ncfc/ncfc.hh"
#include <netcdf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

// Initialize output variable information including variable names, data pointers, variable types and variable sizes
void init_out_var_info(int size, vector<string> &var_names, vector<void *> &var_ptrs, vector<long int> &var_sizes)
{
  char *cptr = 0;
  double *dptr = 0;
  float *fptr = 0;
  int *iptr = 0;

  // Must be in the following order enum var_names {MARK, TIME, LATITUDE, LONGITUDE, TEMPERATURE, STATIONS};

  var_names.push_back("mark");
  dptr = new double[1];
  printf("dptr %p\n", dptr);
  var_ptrs.push_back(dptr);
  var_sizes.push_back(1);

  var_names.push_back("time");
  dptr = new double[size];
  printf("dptr %p\n", dptr);
  var_ptrs.push_back(dptr);
  var_sizes.push_back(size);

  fptr = new float[size];
  printf("fptr %p\n", fptr);
  var_names.push_back("latitude");
  var_ptrs.push_back(fptr);
  var_sizes.push_back(size);

  var_names.push_back("longitude");
  fptr = new float[size];
  printf("fptr %p\n", fptr);
  var_ptrs.push_back(fptr);
  var_sizes.push_back(size);

  var_names.push_back("temperature");
  iptr = new int[size];
  printf("iptr %p\n", iptr);
  var_ptrs.push_back(iptr);
  var_sizes.push_back(size);

  const int STATION_NAME_LENGTH = 32;
  const int NUM_STATIONS = 2;
  const int STATION_SIZE = STATION_NAME_LENGTH * NUM_STATIONS;
  var_names.push_back("stat_field_names");
  cptr = new char[STATION_SIZE];
  printf("cptr %p\n", cptr);
  var_ptrs.push_back(cptr);
  var_sizes.push_back(STATION_SIZE);
}

void usage(char *prog_name)
{
  fprintf(stderr, "Usage: %s [-c cdl_file] out_file\n\n", prog_name);
  fprintf(stderr, "For examples: test_append_variables -c test_append_variables.cdl test.nc\n");
}


int main(int argc, char **argv)
{
  char *cdl_file = 0;
  int errflg = 0;

  // Process command line arguments
  int c;

  while ((c = getopt(argc, argv, "c:")) != EOF)
    switch (c)
      {
      case 'c':
	cdl_file = optarg;
	break;

      case '?':
	errflg = 1;
        break;
      }

  if (errflg)
    {
      usage(argv[0]);
      return(2);
    }

  if (argc - optind < 1)
    {
      usage(argv[0]);
      exit(2);
    }
  
  char *out_file = argv[optind];
  const int COMMAND_LEN = 2048;
  char command[COMMAND_LEN];

  if (cdl_file != 0)
    {
      // Make netcdf output file
      int ret = snprintf(command, COMMAND_LEN, "ncgen %s -o %s", cdl_file, out_file);
      if (ret == COMMAND_LEN)
	{
	  fprintf(stderr, "Error: command line args to long, command buffer size = %d\n", COMMAND_LEN);
	  return 1;
	}
      ret = system(command);
      if (ret != 0)
	{
	  fprintf(stderr, "Error: could not ncgen output file %s using %s\n", out_file, cdl_file);
	  return 1;
	}
    }

  int size = 10;

  vector<string> var_names;
  vector<void *> var_ptrs;
  vector<int> var_types;
  vector<long int> var_sizes;
  
  enum Var_names {MARK, TIME, LATITUDE, LONGITUDE, TEMPERATURE, STATIONS};

  init_out_var_info(size, var_names, var_ptrs, var_sizes);
  
  //  ((double *)var_ptrs[MARK])[0] = 30;

  double *dptr = (double *)var_ptrs[MARK];
  dptr[0] = 45;

  double *dptr_time = (double *)var_ptrs[TIME];
  float *fptr_lat = (float *)var_ptrs[LATITUDE];
  float *fptr_lon = (float *)var_ptrs[LONGITUDE];
  int *iptr_temp = (int *)var_ptrs[TEMPERATURE];
  char *cptr_stations = (char *)var_ptrs[STATIONS];

  for (int i=0; i<size; i++)
    {
      dptr_time[i] = 5;
      fptr_lat[i] = 6;
      fptr_lon[i] = 7;
      iptr_temp[i] = 8;
    }
  memcpy(cptr_stations, "1234567887654321", 16);

  int out_ncid;
  int status = nc_open(out_file, NC_WRITE, &out_ncid);
  if (status != NC_NOERR)
    return -1;

  string error;
  int ret = NCFC_append_variables(out_ncid, var_names, var_ptrs, var_sizes, error);
  ret = NCFC_append_variables(out_ncid, var_names, var_ptrs, var_sizes, error);
  status = nc_close(out_ncid);

  if (ret < 0 || status != NC_NOERR)
    {
      fprintf(stderr, "Error: output error occured when writing %s, %s\n", out_file, error.c_str());
      return -1;
    }
  else
    return 0;
}
