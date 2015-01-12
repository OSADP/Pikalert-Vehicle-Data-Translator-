/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * Copyright (c) 1995-2002 UCAR
 * University Corporation for Atmospheric Research(UCAR)
 * National Center for Atmospheric Research(NCAR)
 * Research Applications Program(RAP)
 * P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 * All rights reserved. Licenced use only.
 * $Date: 2004/05/17 20:24:50 $
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*
 * Module: test_gtime.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   8/30/99
 *
 * Description:
 *     Test the gtime module.
 */

// Include files 
#include <stdio.h>
#include "../include/gtime/gtime.h"

// Constant, macro and type definitions 

// Global variables 

// Functions and objects




int main()
{
  int last_tm_year = 0;
  struct tm *gtms;
  struct tm tms;
  time_t tim;
  time_t otim;

  /* exercise GT_mkgtime over a 29 year period every 30 seconds */
  for (tim = 860288400-3600; tim < 860288400 + 29 * 86400 * 365; tim += 30)
    {
      /* get gmt for tim */
      gtms = gmtime(&tim);
      tms = *gtms;

      /* set non-essentials to 0 since users will do the same */
      tms.tm_wday = 0;
      tms.tm_yday = 0;
      tms.tm_isdst = 0;

      /* do inverse */
      otim = GT_mkgtime(&tms);
      if (last_tm_year != tms.tm_year)
	{
	  printf("date: %4d-%02d-%02d %02d-%02d-%02d\n", tms.tm_year, tms.tm_mon+1, tms.tm_mday+1, tms.tm_hour, tms.tm_min, tms.tm_sec);
	  last_tm_year = tms.tm_year;
	}

      /* check output against original time */
      if (otim != tim)
	printf("error occurred when tim = %ld, otim %ld\n", tim, otim);
    }

  return(0);
}
