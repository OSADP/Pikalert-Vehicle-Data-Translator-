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
// Module: get_unlim.cc
//
// Author: GTG Nowcast
//
// Date:   10/9/07
//
// Description:
//     
//----------------------------------------------------------------------

// Include files 
#include "../include/ncfc/ncfc.hh"

// Constant, macro and type definitions 

// Global variables 

// Functions and objects


int NCFC_get_unlim(int in_ncid, string &unlim_name, int *unlim_id, int *unlim_size)
{
  int status = nc_inq_unlimdim(in_ncid, unlim_id); // get ID of unlimited dimension 
  if (status != NC_NOERR)
    {
      return -1;
    }

  char recname[NC_MAX_NAME+1];
  size_t num_rec = 0;

  // get unlimited dimension name and current length 
  status = nc_inq_dim(in_ncid, *unlim_id, recname, &num_rec);
  if (status != NC_NOERR)
    {
      return -1;
    }

  unlim_name = string(recname);
  *unlim_size = num_rec;
  return 0;
}


#ifdef NOTNOW

void usage(char *prog)
{
  fprintf(stderr, "Usage: %s in_file(s) ", prog);
  fprintf(stderr, "For examples: get_unlim test.nc\n");
}

int main(int argc, char **argv)
{
  char *in_file = 0;

  if (argc < 2)
    {
      usage(argv[0]);
      return 2;
    }

  
  for (int i=1; i<argc; i++)
    {
      in_file = argv[i];
      int in_ncid = -1;

      int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
      if (status != NC_NOERR)
	{
	  return -1;
	}

      string unlim_name;
      int unlim_id;
      int unlim_size;
      int ret = NCFC_get_unlim(in_ncid, unlim_name, &unlim_id, &unlim_size);
      printf("%s: %s, %d, %d\n", in_file, unlim_name.c_str(), unlim_id, unlim_size);
      nc_close(in_ncid);
    }

  return 0;
}


#endif
