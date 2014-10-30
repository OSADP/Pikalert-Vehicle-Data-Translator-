// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: put_var_attr.cc
 *
 * Author: Jim Cowie
 *
 * Date:   1/24/02
 *
 * Description: Functions to write a variables attribute value given the
 *              ncfile pointer, variable name and attribute name.
 *
 *              Returns  0 success
 *                      -1 failure
 *     
 */

#include <ncf/ncf.hh>
using namespace std;

  
// float version
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, float value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);
  
  if ((*var_ptr).add_att(attr_name, value))
    return (0);
  else
    return(-1);
}


// int version
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);
  
  if ((*var_ptr).add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// double version
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, double value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if ((*var_ptr).add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// short version
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, short value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if ((*var_ptr).add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// char version - make sure space has been allocated for value
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, const char *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  if ((*var_ptr).add_att(attr_name, value))
    return (0);
  else
    return(-1);
}


