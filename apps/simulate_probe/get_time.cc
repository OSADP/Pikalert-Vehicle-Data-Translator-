//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file get_time.cc
 *
 * <file description>
 *
 * @date 12/15/14
 */

// Include files 
#include "get_time.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


int get_time(const string &time_string, time_t *time_val)
{
  struct tm tms;
  zero_tms(&tms);

  if (time_string.size() == 4)
    {
      char *sret = strptime(time_string.c_str(), "%Y", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }
  else if (time_string.size() == 6)
    {
      char *sret = strptime(time_string.c_str(), "%Y%m", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }
  else if (time_string.size() == 8)
    {
      char *sret = strptime(time_string.c_str(), "%Y%m%d", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }
  else if (time_string.size() == 10)
    {
      char *sret = strptime(time_string.c_str(), "%Y%m%d%H", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }
  else if (time_string.size() == 12)
    {
      char *sret = strptime(time_string.c_str(), "%Y%m%d%H%M", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }
  else if (time_string.size() == 14)
    {
      char *sret = strptime(time_string.c_str(), "%Y%m%d%H%M%S", &tms);
      if (sret == NULL)
	{
	  return -1;
	}
    }

  *time_val = timegm(&tms);
  return 0;
}

void zero_tms(struct tm *ptm)
{
  ptm->tm_sec = 0;
  ptm->tm_min = 0;
  ptm->tm_hour = 0;
  ptm->tm_mday = 0;
  ptm->tm_mon = 0;
  ptm->tm_year = 0;
  ptm->tm_wday = 0;
  ptm->tm_yday = 0;
  ptm->tm_isdst = 0;
}

