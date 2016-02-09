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
 * Module: check_led.cc
 *
 * Author: Jim Cowie
 *
 * Date:   07/24/02
 *
 * Description: Checks the license expiration date against the current date.
 *		If the license has expired (ie;	system date > expiration date)
 *		then the binary	is removed, and this function returns 1.
 *              The expiration date is hard-wired into the binary when
 *		it was built by setting the led value in the led.h file. A
 *              value of 0 indicates the binary will not expire.
 *
 * NOTE:
 *
 * This module is deprecated, use get_led.cc instead.
 *
 */

// Include files 

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "led.h"

// Functions and objects

int check_led(char *prog, double ud)
{
  // Get current system time
  time_t curr_time;
  double sys_time;
  char command[256];
  
  time(&curr_time);
  sys_time = (double) curr_time;

  int led_day = (int)ud;
  int sys_day = (int)(sys_time/86400);

  // Compare times. Return if expired or does not expire
  if (led_day == 0 || sys_day < led_day) {
      return(0);
  }

  // License has expired, so get the path to the binary.
  // If the prog name has a "/" in it, use this as-is,
  // otherwise we use the which command to find the location
  // of the binary file.

  if (strchr(prog, '/') == NULL) {
    sprintf(command, "which %s | xargs rm -f", prog);
  }
  else {
    sprintf(command, "rm -f %s", prog);
  }  

  //printf("%s\n", command);

  // Execute command
  system(command);
  return(1);
}
