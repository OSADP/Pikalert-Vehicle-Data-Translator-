// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:32:06 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
//----------------------------------------------------------------------
// Module: put_var.cc
//
// Author: Gerry Wiener
//
// Date:   7/10/00
//
// Description:
//     Functions to put variables in netcdf files
//----------------------------------------------------------------------

// Include files 
#include "ncf.hh"

using namespace std;

// Constant, macro and type definitions 
class Proc_err {};

// Global variables 

// Functions and objects

// Good for putting variables.
int NCF_put_var(NcFile *ncfp, const char *var_name, const float *val, const long *dim)
{
   try
    {
      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      if (!var_ptr->put(val, dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      return(-1);
    }

  return (0);
}

int NCF_put_var(NcFile *ncfp, const char *var_name, const int *val, const long *dim)
{
   try
    {
      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      if (!var_ptr->put(val, dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      return(-1);
    }

  return (0);
}

int NCF_put_var(NcFile *ncfp, const char *var_name, const short *val, const long *dim)
{
   try
    {
      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      if (!var_ptr->put(val, dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      return(-1);
    }

  return (0);
}

int NCF_put_var(NcFile *ncfp, const char *var_name, const double *val, const long *dim)
{
   try
    {
      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      if (!var_ptr->put(val, dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      return(-1);
    }

  return (0);
}

int NCF_put_var(NcFile *ncfp, const char *var_name, const char *val, const long *dim)
{
   try
    {
      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      if (!var_ptr->put(val, dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      return(-1);
    }

  return (0);
}
