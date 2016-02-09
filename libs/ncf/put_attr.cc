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
 * Module: put_attr.cc
 *
 * Author: Jason Craig (based on put_var_attr.cc by Jim Cowie)
 *
 * Date:   10/14/05
 *
 * Description: Functions to write a global attribute value given the
 *              ncfile pointer and attribute name.
 *
 *              Returns  0 success
 *                      -1 failure
 *     
 */

#include "ncf.hh"

using namespace std;

  
// float version
int NCF_put_attr(NcFile *ncfp, const char *attr_name, float value)
{
  if (!ncfp->is_valid())
    return (-1);

  if (ncfp->add_att(attr_name, value))
    return (0);
  else
    return(-1);
}


// int version
int NCF_put_attr(NcFile *ncfp, const char *attr_name, int value)
{
  if (!ncfp->is_valid())
    return (-1);
  
  if (ncfp->add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// double version
int NCF_put_attr(NcFile *ncfp, const char *attr_name, double value)
{
  if (!ncfp->is_valid())
    return (-1);

  if (ncfp->add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// short version
int NCF_put_attr(NcFile *ncfp, const char *attr_name, short value)
{
  if (!ncfp->is_valid())
    return (-1);

  if (ncfp->add_att(attr_name, value))
    return (0);
  else
    return(-1);
}

// char version - make sure space has been allocated for value
int NCF_put_attr(NcFile *ncfp, const char *attr_name, const char *value)
{
  if (!ncfp->is_valid())
    return (-1);

  if (ncfp->add_att(attr_name, value))
    return (0);
  else
    return(-1);
}


