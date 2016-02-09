// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:32:06 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: write_var.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   4/24/00
 *
 * Description:
 *     Convenience functions for writing netcdf variables.
 */

// Include files 
#include "ncf.hh"

using namespace std;


// Constant, macro and type definitions 

// Global variables 

// Functions and objects


int NCF_write_var(NcFile *ncfp, const char *var_name, double *val)
{
  long *edges;			// edges for output variable
  long num_vals;		// number of values of variable
  int ret;

  edges = 0;
  num_vals = 0;


  // Get a pointer to the variable
  NcVar *var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    return(-1);

  num_vals = var_ptr->num_vals();
  edges = var_ptr->edges();

  if (num_vals == 1)
    ret = var_ptr->put(val);
  else
    ret = var_ptr->put(val, edges);

  delete [] edges;
  if (ret == 0)
    return(-1);
  else
    return(0);
}

int NCF_write_var(NcFile *ncfp, const char *var_name, float *val)
{
  long *edges;			// edges for output variable
  long num_vals;		// number of values of variable
  int ret;

  edges = 0;
  num_vals = 0;


  // Get a pointer to the variable
  NcVar *var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    return(-1);

  num_vals = var_ptr->num_vals();
  edges = var_ptr->edges();

  if (num_vals == 1)
    ret = var_ptr->put(val);
  else
    ret = var_ptr->put(val, edges);
  
  delete [] edges;
  if (ret == 0)
    return(-1);
  else
    return(0);
}

int NCF_write_var(NcFile *ncfp, const char *var_name, int *val)
{
  long *edges;			// edges for output variable
  long num_vals;		// number of values of variable
  int ret;

  edges = 0;
  num_vals = 0;


  // Get a pointer to the variable
  NcVar *var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    return(-1);

  num_vals = var_ptr->num_vals();
  edges = var_ptr->edges();

  if (num_vals == 1)
    ret = var_ptr->put(val);
  else
    ret = var_ptr->put(val, edges);
  
  delete [] edges;
  if (ret == 0)
    return(-1);
  else
    return(0);
}

int NCF_write_var(NcFile *ncfp, const char *var_name, char *val)
{
  long *edges;			// edges for output variable
  long num_vals;		// number of values of variable
  int ret;

  edges = 0;
  num_vals = 0;


  // Get a pointer to the variable
  NcVar *var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    return(-1);

  num_vals = var_ptr->num_vals();
  edges = var_ptr->edges();

  if (num_vals == 1)
    ret = var_ptr->put(val);
  else
    ret = var_ptr->put(val, edges);
  
  delete [] edges;
  if (ret == 0)
    return(-1);
  else
    return(0);
}

// You may need this if using an older (than 3.6.2) version of netCDF.
//
//int NCF_write_var(NcFile *ncfp, const char *var_name, unsigned char *val)
//

int NCF_write_var(NcFile *ncfp, const char *var_name, ncbyte *val)
{
  long *edges;			// edges for output variable
  long num_vals;		// number of values of variable
  int ret;

  edges = 0;
  num_vals = 0;


  // Get a pointer to the variable
  NcVar *var_ptr = ncfp->get_var(var_name);

  if (var_ptr == 0)
    return(-1);

  num_vals = var_ptr->num_vals();
  edges = var_ptr->edges();

  if (num_vals == 1)
    ret = var_ptr->put(val);
  else
    ret = var_ptr->put(val, edges);
  
  delete [] edges;
  if (ret == 0)
    return(-1);
  else
    return(0);
}
