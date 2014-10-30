// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_var_fill_val.cc
 *
 * Author: Missy Petty
 *
 * Date:   7/9/00
 *
 * Description:
 *     
 */

#include <ncf/ncf.hh>
using namespace std;

int NCF_get_var_fill_val(NcVar *var_ptr, float *fill_val)
{
  
  NcAtt *fv_ptr = var_ptr->get_att("_FillValue");
  if (!fv_ptr)
    return (-1);

  *fill_val = fv_ptr->as_float(0);

  delete fv_ptr;

  return (0);
}
  

int NCF_get_var_fill_val(NcVar *var_ptr, int *fill_val)
{
  
  NcAtt *fv_ptr = var_ptr->get_att("_FillValue");
  if (!fv_ptr)
    return (-1);

  *fill_val = fv_ptr->as_int(0);

  delete fv_ptr;

  return (0);
}
  
// Get fill value given NcFile pointer and variable name
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, float *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att("_FillValue");
  if (attp->is_valid())
    {
      *value = attp->as_float(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}


// Get fill value given NcFile pointer and variable name
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, int *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att("_FillValue");
  if (attp->is_valid())
    {
      *value = attp->as_int(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}

// Get fill value given NcFile pointer and variable name
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, unsigned char *value)
{
  if (!ncfp->is_valid())
    return (-1);

  NcVar *var_ptr = ncfp->get_var(var_name);
  if (!var_ptr)
    return (-1);

  NcAtt *attp = (*var_ptr).get_att("_FillValue");
  if (attp->is_valid())
    {
      *value = attp->as_int(0);
    }
  else
    return(-1);

  delete attp;

  return(0);
}


