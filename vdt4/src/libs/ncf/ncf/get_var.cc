// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_var.cc
 *
 * Author: Melissa Petty
 *
 * Date:   6/1/00
 *
 * Description: Functions to get variables from netcdf file
 *     
 */

#include <ncf/ncf.hh>
using namespace std;

class Proc_err {};

int NCF_get_var(NcFile *ncfp, const char *var_name, float *val)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if (var_ptr->num_dims() > 0)
    return (-1);

  if (!var_ptr->get(val)) 
    return (-1);

  return (0);
}


int NCF_get_var(NcFile *ncfp, const char *var_name, int *val)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if (var_ptr->num_dims() > 0)
    return (-1);

  if (!var_ptr->get(val)) 
    return (-1);

  return (0);
}


int NCF_get_var(NcFile *ncfp, const char *var_name, short *val)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if (var_ptr->num_dims() > 0)
    return (-1);

  if (!var_ptr->get(val)) 
    return (-1);

  return (0);
}



int NCF_get_var(NcFile *ncfp, const char *var_name, double *val)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if (var_ptr->num_dims() > 0)
    return (-1);

  if (!var_ptr->get(val)) 
    return (-1);

  return (0);
}


// Good for getting variable data and their dimensions. Remember to delete
// dim and val when no longer needed. Memory is allocated using new [].

// float type

int NCF_get_var(NcFile *ncfp, const char *var_name, float **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new float[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}


// int type

int NCF_get_var(NcFile *ncfp, const char *var_name, int **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new int[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}


// double type

int NCF_get_var(NcFile *ncfp, const char *var_name, double **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new double[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}


// short type

int NCF_get_var(NcFile *ncfp, const char *var_name, short **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new short[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}


// char type

int NCF_get_var(NcFile *ncfp, const char *var_name, char **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new char[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}


#ifndef NO_NCBYTE

// ncbyte type (ncbyte was an unsigned char until 3.6.2 when it bacame signed)
int NCF_get_var(NcFile *ncfp, const char *var_name, ncbyte **val, long **dim, int *num_dim, long *size)
{
   try
    {
      *val = 0;
      *dim = 0;

      if (!ncfp->is_valid())
	throw Proc_err();

      NcVar *var_ptr = ncfp->get_var(var_name);
      if (!var_ptr)
	throw Proc_err();

      *num_dim = var_ptr->num_dims();
      if (*num_dim <= 0)
	throw Proc_err();	

      *dim = var_ptr->edges();

      *size = 1;
      for (int i=0; i<*num_dim; i++)
	*size *= (*dim)[i];

      if (*size <= 0)
	throw Proc_err();

      *val = new ncbyte[*size];

      if (!var_ptr->get(*val, *dim)) 
	throw Proc_err();
    }

  catch (Proc_err)
    {
      delete [] *val;

      *dim = 0;
      *val = 0;
      *size = 0;
      *num_dim = 0;
      return(-1);
    }

  return (0);
}
#endif
