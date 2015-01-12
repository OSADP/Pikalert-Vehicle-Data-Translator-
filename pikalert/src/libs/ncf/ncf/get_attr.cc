// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_attr.cc
 *
 * Author: Jason Craig (based on get_var_attr.cc by Jim Cowie)
 *
 * Date:   10/14/05
 *
 * Description: Functions to get a global attribute value given the
 *              ncfile pointer and attribute name.
 *
 *              Returns  0 success
 *                      -1 failure
 *     
 */

#include <ncf/ncf.hh>
using namespace std;

  
// float version
int NCF_get_attr(NcFile *ncfp, const char *attr_name, float *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcAtt *attp = ncfp->get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_float(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}


// int version
int NCF_get_attr(NcFile *ncfp, const char *attr_name, int *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcAtt *attp = ncfp->get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_int(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// double version
int NCF_get_attr(NcFile *ncfp, const char *attr_name, double *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcAtt *attp = ncfp->get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_double(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// short version
int NCF_get_attr(NcFile *ncfp, const char *attr_name, short *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcAtt *attp = ncfp->get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_short(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// char version - make sure space has been allocated for value
int NCF_get_attr(NcFile *ncfp, const char *attr_name, char **value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcAtt *attp = ncfp->get_att(attr_name);
  if (attp && attp->is_valid())
    {
      *value = attp->as_string(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

