// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_var_attr.cc
 *
 * Author: Jim Cowie
 *
 * Date:   1/24/02
 *
 * Description: Functions to get a variables attribute value given the
 *              ncfile pointer, variable name and attribute name.
 *
 *              Returns  0 success
 *                      -1 failure
 *     
 */

#include <ncf/ncf.hh>
using namespace std;

  
// float version with index into attribute vector
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, float *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_float(index);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// float version for scalar attributes, backward compatible
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, float *value)
{
  return(NCF_get_var_attr(ncfp, var_name, attr_name, 0, value));
}


// int version with index into attribute vector
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, int *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_int(index);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// int version for scalar attributes, backward compatible
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int *value)
{
  return(NCF_get_var_attr(ncfp, var_name, attr_name, 0, value));
}


// double version with index into attribute vector
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, double *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_double(index);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// double version for scalar attributes, backward compatible
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, double *value)
{
  return(NCF_get_var_attr(ncfp, var_name, attr_name, 0, value));
}


// short version with index into attribute vector
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, short *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_short(index);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// short version for scalar attributes, backward compatible
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, short *value)
{
  return(NCF_get_var_attr(ncfp, var_name, attr_name, 0, value));
}


// char version with index into attribute vector - make sure space has been allocated for value
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, char **value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_string(index);
    }
  else
    return(-1);

  delete attp;

  return(0);
}


// char version for scalar attributes, backward compatible
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, char **value)
{
  return(NCF_get_var_attr(ncfp, var_name, attr_name, 0, value));
}
