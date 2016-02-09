// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:33:35 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: write_creation_time.cc
 *
 * Author: Melissa Petty
 *
 * Date:   11/19/99
 *
 * Description:  Gets present time and writes to netcdf file as creation_time
 *     variable.
 *     
 */

// Include files 
#include <time.h>
#include "nxncf.hh"

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

int NXNCF_write_creation_time(NcFile &ncfile)
{
  time_t curr_time;
  double creat_time;
  
  time(&curr_time);
  creat_time = (double) curr_time;

  NcVar *creat_var = ncfile.get_var(CREATION_TIME_NAME);
  if (!creat_var)
    return(-1);

  if (!creat_var->put(&creat_time))
    return(-1);

  return(0);
}
