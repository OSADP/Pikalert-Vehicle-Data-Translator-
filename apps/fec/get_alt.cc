// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: fec.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/5/02
 *
 * Description:
 *
 */

// Include files
#include <new>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <log/log.hh>
#include <math.h>

#include <ncf/ncf.hh>
#include <nxncf/nxncf.hh>
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "netcdf.h"

// Global variables 
extern Log *proc_log;

float get_alt(int site_id, int *output_site_array, int num_output_sites, float *alt)
{
  int i;

  if (site_id < 0)
    return(NC_FILL_FLOAT);

  i = 0;
  while (i < num_output_sites && site_id != output_site_array[i])
    i++;

  if (i == num_output_sites)
    return(NC_FILL_FLOAT);
  else
    return(alt[i]);
}

