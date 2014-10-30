/** Code extracted from utim.c in the toolsa library 
 ** Code extracted from TaXml.cc in the toolsa library
 **
 **
 ** Copyright UCAR (c) 1992 - 2001
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization
 ** 1997/9/26 14:18:54
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/



#include <stdio.h>
#include <cerrno>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include "Utilities.hh"


//////////////////////////////
// read array of strings
//
// returns 0 on success, -1 on failure

int TaXml::readStringArray(const string &xmlBuf,
                           const string &tag,
                           vector<string> &valArray)
  
{
  
  valArray.clear();
  
  // compute start and end tokens
  
  string startTok = "<" + tag + ">";
  string endTok = "</" + tag + ">";
  
  size_t startPos = 0;
  size_t endPos = 0;
  
  while (startPos != string::npos && endPos != string::npos) {
    
    // find start position
    
    startPos = xmlBuf.find(startTok, endPos);
    if (startPos == string::npos) {
      break;
    }
    startPos += startTok.size();
    
    // find end position
    
    endPos = xmlBuf.find(endTok, startPos);
    if (endPos == string::npos) {
      break;
    }
    
    string val = xmlBuf.substr(startPos, endPos - startPos);
    valArray.push_back(val);
    
    endPos += endTok.size();

  }

  if (valArray.size() == 0) {
    return -1;
  }

  return 0;

}

/*************************************************************************
 *	JULIAN_DATE: Calc the Julian calandar Day Number
 *	As Taken from Computer Language- Dec 1990, pg 58
 */

long UTIMjulian_date(int day, int month, int year)
{
    long 	a,b;
    double	yr_corr;

    /* correct for negative year */
    yr_corr = (year > 0? 0.0: 0.75);
    if(month <=2) {
	year--;
	month += 12;
    }
    b=0;

    /* Cope with Gregorian Calandar reform */
    if(year * 10000.0 + month * 100.0 + day >= 15821015.0) {
	a = year / 100;
	b = 2 - a + a / 4;
    }
    return((long)((365.25 * year - (double)yr_corr) + 
	   (long) (30.6001 * ((long)month +1)) + (long)day + 1720994L + b));
}

/*************************************************************************
 *	CALENDAR_DATE: Calc the calandar Day from the Julian date
 *	As Taken from Computer Language- Dec 1990, pg 58
 */

void UTIMcalendar_date(long jdate,long *day, long *month, long *year)
{
	long	a,b,c,d,e,z,alpha;

	z = jdate +1;

	/* Gregorian reform correction */
	if (z < 2299161) { 
		a = z; 
	} else {
		alpha = (long) ((z - 1867216.25) / 36524.25);
		a = z + 1 + alpha - alpha / 4;
	}

	b = a + 1524;
	c = (long) ((b - 122.1) / 365.25);
	d = (long) (365.25 * c);
	e = (long) ((b - d) / 30.6001);
	*day = (int) b - d - (long) (30.6001 * e);
	*month = (int) (e < 13.5)? e - 1 : e - 13;
	*year = (int) (*month > 2.5)? (c - 4716) : c - 4715;
}

time_t UTIMdate_to_unix( UTIMstruct *date)
{
	long	u_day,day,days;
	long	u_time;

	u_day = UTIMjulian_date(1,1,1970);
	day = UTIMjulian_date(date->day,date->month,date->year);

	days = day - u_day;

	u_time = (days * 86400) + (date->hour * 3600) + (date->min * 60) + date->sec;

	date->unix_time = u_time;

	return u_time;
}

/*************************************************************************
 *
 */

void UTIMunix_to_date(time_t utime, UTIMstruct *date)
{
	long	u_day, day;

	u_day = UTIMjulian_date(1,1,1970);

	day = (utime / 86400);
	if ( day < 0 ) day--;
	
	UTIMcalendar_date((u_day + day), &date->day, &date->month, &date->year);

	day = (utime % 86400);
	if ( day < 0 ) day += 86400;
	
	date->hour = day / 3600;
	date->min = (day / 60) - (date->hour * 60);
	date->sec = day % 60;
	date->unix_time = utime;

}
