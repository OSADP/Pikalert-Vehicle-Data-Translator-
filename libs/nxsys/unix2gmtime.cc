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
 * Module: unix2gmtime.cc
 *
 * Author: Seth Linden
 *
 * Date:   07/16/03
 *
 * Description: Converts a unix time to a Date/Time string.
 *              Function takes a double and returns a string.
 *              
 *
 */


// Include files

#include<cstdio>
#include <iostream>
#include<ctime>
#include<ctype.h>
#include <string>

using namespace std;


// Constants

const int date_length = 50; 


// Functions

const char *unix2gmtime(double unix_time){

  
  char *date_string = new char[date_length];
  
  time_t u_time;
  u_time = (time_t)unix_time;

  tm *gm_time_struct;
  gm_time_struct = gmtime(&u_time);

  int second = gm_time_struct->tm_sec;
  
  int minute = gm_time_struct->tm_min;
  
  int hour = gm_time_struct->tm_hour;
  
  int day = gm_time_struct->tm_mday;
  
  int month = gm_time_struct->tm_mon;
  month = month + 1;
  
  int year = gm_time_struct->tm_year;
  year = year + 1900;
  
  //sprintf(date_string, "%4d%02d%02d/%02d:%02d:%02d", year, month, day, hour, minute, second);

  sprintf(date_string, "%4d%02d%02d/%02d%02d", year, month, day, hour, minute);
  
  return(date_string);

}
  


