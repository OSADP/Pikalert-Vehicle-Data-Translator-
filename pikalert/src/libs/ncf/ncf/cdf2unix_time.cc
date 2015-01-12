// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: cdf2unix_time.cc
 *
 * Author: Melissa Petty
 *
 * Date:   4/6/00
 *
 * Description:
 *     
 */

#include <ncf/ncf.hh>
#include <ctime>
using namespace std;

/* Converts time units used by Unidata's gribtonc to unix time */

double NCF_cdf2unix_time(double input_time)
{
  struct tm init_tm_struct = {0, 0, 0, 1, 0, 92};
  time_t init_time_t;
  double unix_val;

  init_time_t = NCF_mkgtime(&init_tm_struct);
  unix_val = (double) (init_time_t + 3600*input_time);
  return(unix_val);
}
