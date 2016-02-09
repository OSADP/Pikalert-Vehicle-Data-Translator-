// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:26:11 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: gtime.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   10/17/97
 *
 * Description:
 *     time functions broken out from tetwws
 */

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include "gtime.h"


static int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Convert UTC time given in tm struct to UNIX time in UTC
// seconds. NOTE: the fields in *timeptr are not reset as is done in
// mktime and the const interface reflects that fact. Note that
// timeptr->tm_isdst is ignored.
time_t GT_mkgtime(const struct tm *timeptr)
{
  time_t curr_time;
  static int32_t once = 0;
  time_t tim;
  static int32_t save_timezone;
  struct tm tms;
  struct tm *ptm;

  if (once == 0)
    {
      time(&curr_time); 
      ptm = localtime(&curr_time);
      save_timezone = timezone;	  // timezone is always the same value regardless of dst 
      once = 1;
    }

  tms = *timeptr;

  tms.tm_isdst = 0;		// ensure daylight savings time is 0
  tim = mktime(&tms);
  if (tim == -1)
    return((time_t)(-1));

  return(tim - save_timezone);
}


// Reset tms to 0
void GT_cleartm(struct tm *tms)
{
  tms->tm_year = 0;
  tms->tm_mon = 0;
  tms->tm_mday = 0;
  tms->tm_hour = 0;
  tms->tm_min = 0;
  tms->tm_sec = 0;
  tms->tm_yday = 0;
  tms->tm_wday = 0;
  tms->tm_isdst = 0;
}

// Convert ascii string to time structure. Do bounds checking. 
int32_t GT_atotm
(
 const char *string,
 struct tm *tms
 )
{
  tms->tm_year = 0;
  tms->tm_mon = 0;
  tms->tm_mday = 0;
  tms->tm_hour = 0;
  tms->tm_min = 0;
  tms->tm_sec = 0;
  tms->tm_yday = 0;
  tms->tm_wday = 0;
  tms->tm_isdst = 0;
  if (sscanf(string, "%4d%2d%2d%2d%2d%2d", &tms->tm_year, &tms->tm_mon, &tms->tm_mday, &tms->tm_hour, &tms->tm_min, &tms->tm_sec) != 6)
    return(-1);
  if (tms->tm_year < 1900 || tms->tm_year > 9999)
    return(-1);
  else
    tms->tm_year -= 1900;
  if (tms->tm_mon < 1 || tms->tm_mon > 12)
    return(-1);
  else
    tms->tm_mon -= 1;

  
  if (tms->tm_mon == 1 && (tms->tm_year % 4 == 0))
    {
      // leap year
      if (tms->tm_mday < 1 || tms->tm_mday > 29)
	return(-1);
    }
  else
    {
      if (tms->tm_mday < 1 || tms->tm_mday > month_days[tms->tm_mon])
	return(-1);
    }
    
  if (tms->tm_hour < 0 || tms->tm_hour > 23)
    return(-1);
  if (tms->tm_min < 0 || tms->tm_min > 59)
    return(-1);
  if (tms->tm_sec < 0 || tms->tm_sec > 59)
    return(-1);

  return(0);
}
