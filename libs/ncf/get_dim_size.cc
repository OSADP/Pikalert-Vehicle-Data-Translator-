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
 * Module: get_dim_size.cc
 *
 * Author: Missy Petty
 *
 * Date:   7/11/00
 *
 * Description:
 *     
 */

// Include files 
#include "ncf.hh"

using namespace std;


// Functions and objects

int NCF_get_dim_size(NcFile *infile, const char *dim_name)
{
  NcDim *dim_ptr = 0;

  dim_ptr = infile->get_dim(dim_name);
  if (dim_ptr == 0)
    return (-1);

  int dim_size = dim_ptr->size();

  return (dim_size);
}
