// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: gtime.c
 *
 * Author: Gerry Wiener
 *
 * Date:   10/17/97
 *
 * Description:
 *     time functions broken out from tetwws
 */

#include <ctime>
#include <ncf/gtime.h>
#include <sys/types.h>
using namespace std;

/* Convert UTC time given in tm struct to UNIX time in UTC
   seconds. NOTE: the fields in *timeptr are not reset as is done in
   mktime and the const interface reflects that fact. */

time_t NCF_mkgtime(const struct tm *timeptr)
{
  time_t curr_time;
  static int32_t once;
  time_t tim;
  static int32_t save_timezone;
  struct tm tms;

  if (once == 0)
    {
      time(&curr_time); 
      localtime_r(&curr_time, &tms);
      save_timezone = timezone;	  /* timezone is always the same value 
				     regardless of dst */
      once = 1;
    }

  tms = *timeptr;

  tms.tm_isdst = 0;
  tim = mktime(&tms);
  if (tim == -1)
    return((time_t)(-1));

  return(tim - save_timezone);
}


