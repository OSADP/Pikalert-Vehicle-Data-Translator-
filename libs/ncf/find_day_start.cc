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
 * Module: find_day_start.cc
 *
 * Author: Bill Myers
 *
 * Date:   8/29/99
 *
 * Description:
 *     
 */

// Include files 
// #include <dmos.hh>
#include <cstdio>
#include <math.h>
#include "ncf.hh"

using namespace std;

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

double NCF_find_day_start(double current_time)
{
  double start_time;

  // Find 0Z of day
  start_time = current_time / SEC_PER_DAY;
  start_time = floor(start_time);
  start_time *= SEC_PER_DAY;

  return(start_time);
}
