// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
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
#include <ncf/ncf.hh>
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
