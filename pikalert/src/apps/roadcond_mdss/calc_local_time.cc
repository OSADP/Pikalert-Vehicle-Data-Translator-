// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: calc_local_time.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: This file contains routines which calculate local time at
 *              a particular location. Note that these calculations
 *              generally have nothing to do with political time zone boundaries.
 *     
 *       calc_local_time_diff......Calculates the difference in (floating point) seconds
 *                                 from GMT for a particular longitude.
 *       void calc_local_time......Calculates local time julian day and hour from a 
 *                                 unix time and offset info.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>

/* Constant definitions / Macro definitions / Type definitions */
const int HR_PER_DAY = 24;
const int MIN_PER_HOUR = 60;

/* External global variables / Non-static global variables / Static globals */

extern Log *proc_log;


//  
//  Routine: calc_local_time_diff()
//  
//  Description:
//      Calculates difference in "time zone" from GMT at a particular longitude.
//  
//  Inputs: 
//    lon................Longitude of relevant point. Longitude should be negative in the USA.
//  
//  Outputs:
//    None.
//  
//  Returns:
//    offset.............Time difference in seconds from GMT at this location.
//  

float calc_local_time_diff(float lon)
{
  float offset = SEC_PER_DAY * lon/360.;
  //printf("lon %f   offset %f\n", lon, offset);

  return(offset);
}


//  
//  Routine: calc_local_time()
//  
//  Description:
//      Calculates local time julian day and hour from a unix time (in GMT), a local 
//  time offset, and lead time in hours.
//  
//  Inputs: 
//    time_offset.......The time zone offset in floating point hours.
//    nh................The number of hours ahead (or behind) the "local time".
//    dforc_time........The base line unix time (GMT).
//  
//  Outputs:
//    local_day.........The julian day in the "local" time zone.
//    local_hour........The hour of the day in the "local" time zone.
//  
//  Returns:
//    None.
//  

void calc_local_time(float time_offset, int nh, double dforc_time, 
		     float *local_day, float *local_hour)
{
  time_t ftime;
  struct tm *t;

  ftime = (time_t)(dforc_time + nh * SEC_PER_HOUR - time_offset);
  t = gmtime(&ftime); 
  *local_day = t->tm_yday + 1;  // Fortran Module wants DoY, not days since Jan 1
  *local_hour = t->tm_hour + (float)t->tm_min/MIN_PER_HOUR + (float)t->tm_sec/SEC_PER_HOUR;

  //printf("hour %d   min %d   sec %d\n", t->tm_hour, t->tm_min, t->tm_sec);

  return;
}
