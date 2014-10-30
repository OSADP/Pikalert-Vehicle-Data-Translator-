/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Code extracted from utim.c in the toolsa library 
 ** Code extracted from TaXml.cc in the toolsa library 
 **
 **
 ** Copyright UCAR (c) 1992 - 1997
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization
 ** 1997/9/26 14:18:54
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef Utilities_hh
#define Utilities_hh

#include <string>
#include <vector>
#include <time.h>

using namespace std;


/////////////////////////////////////////////////////////////
// From TaXml.hh
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// May 2006
//
///////////////////////////////////////////////////////////////
//
// Simple XML support for reading and writing XML
//
// No IO actually occurs. All reads/writes are to/from strings.
// These strings are referred to as buffers in this class.
//
////////////////////////////////////////////////////////////////

class TaXml {

public:
  
  // read array of strings from XML buffer, given a tag.
  // One entry in array for each tag found
  // returns 0 on success, -1 on failure

  static int readStringArray(const string &xmlBuf,
                             const string &tag,
                             vector<string> &valArray);

protected:
private:

};

typedef struct UTIMstruct_ {
  long	year;		/* YYYY e.g. 1992,2001 etc */
  long	month;		/* 1-12 */
  long	day;		/* 1-31 */
  long	hour;		/* 0-24 */
  long	min;		/* 0-60 */
  long	sec;		/* 0-60 */
  time_t	unix_time; 	/* secs since Jan 1, 1970 */
} UTIMstruct;

extern time_t UTIMdate_to_unix( UTIMstruct *date );
/* convert UTIM date structure to unix time 
   return the unix time */

extern void UTIMunix_to_date( time_t unix_time, UTIMstruct *date );
/* convert unix time to UTIM date structure */

extern long UTIMjulian_date( int day, int month, int year );
/* 	convert day, month, year to julian date
	return the julian date (= days from 0 BC ?)
	year = e.g. 1992
	month = 1-12
	day = 1-31
*/

extern void UTIMcalendar_date(long jdate,
			      long *day, long *month, long *year);
/* convert julian date to day, month, year
   input : jdate = julian date
   output: year, month, day
*/


#endif

