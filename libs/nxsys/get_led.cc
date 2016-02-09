// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:33:48 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_led.cc
 *
 * Author: Jim Cowie
 *
 * Date:   03/04/08
 *
 * Description: Checks the license expiration date against the system time
 *              and returns whether the binary is expired (-1), not expired
 *              or doesn't expire (0), or the number of days left until the
 *              binary expires if it is in the grace period.
 */

#include <time.h>
#include <stdio.h>
#include "led.h"
#include "unix2gmtime.h"

int get_led()
{
  time_t curr_time;
  double sys_time;
  
  //
  // Get current system time
  //
  time(&curr_time);
  sys_time = (double) curr_time;

  int led_day = (int)led;
  int sys_day = (int)(sys_time/86400);

  //
  // Check led_time and return accordingly
  //
  if (led_day == 0 || sys_day < (led_day - gp))
    return(0);
  else if (sys_day < led_day)
    return(led_day-sys_day);
  else
    return(-1);
}
