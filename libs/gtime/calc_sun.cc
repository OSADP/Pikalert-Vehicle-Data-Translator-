// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:26:11 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "calc_sun.hh"
#include "gtime.h"

/* moon stuff */
#define SEC_PER_DAY                              (24 * 60 * 60)
#define DEGREE_TO_RADIAN                  ((2.0 * PI)/ 360.)
#define J1970   /* 24 */40587.5 /* VAX clock Epoch 1970 Jan 1 (0h UT) */
#define J2000   /* 24 */51545.0 /* Epoch 2000 Jan 1 (12h UT) */
/* end moon stuff */

/* sun stuff */
#define SUNRISE (double) 0.
#define SUNSET  (double) 1.
#define PI (double) M_PI
#define QUAD1 (double) (PI/2.)
#define QUAD2 (double) PI
#define QUAD3 (double) (3.*PI/2.)
#define QUAD4 (double) (2.*PI)
#define DEG2RAD (double) (PI/180.)

#define RISET  (double) -0.0145439  /* cosine  90 degrees 50 minutes */


/*====================================================*/
/* calc_sun
* input:
*       latitude - your latitude
*       longitude - your longitude
*       tz - your time zone
*       dst - if your doing daylight savings
*       year - the year (years since 1900)
*       month - the month (0 = january)
*       mday - day of the month
* output:
*       sunrise - time of sunrise (hh:mm) must pass a buffer
*       sunset - time of sunset (hh:mm) must pass a buffer
*/      
void calc_sun( float latitude, float longitude, int tz, int dst, 
	       int year, int month, int mday, char * sunrise, char * sunset )
{ 
  double  event, k, z;
  double mm;
  double doy;
  double dd;
  double lat;
  double lng;
  double tmzn;
  double rtasc;
  double estm;
  double hr;
  double min;

  int sunrzH;             /* Rise Hours   Sun     */
  int sunrzM;             /* Rise Minutes Sun     */
  int sunsetH;            /* Set  Hours   Sun     */
  int sunsetM;            /* Set  Minutes Sun     */

  lat = DEG2RAD * latitude;
  lng = DEG2RAD * longitude * -1;

  tmzn= (tz - dst) * 0.261799;

  doy  = year + 1900;
  mm  = month + 1;
  dd  = mday;

  /*  day of year  */
  k = (int) ((mm+9.)/12.);
  z = doy/4. - (int) (doy/4.);
  if ( z != 0. )  k *= 2.;
  doy = (int) (275. * mm/9.);
  doy = doy + dd - k - 30.;

  /*  rising phenomena  */
  event = riseset( QUAD1, doy, lng, &rtasc, &estm);

  cooco( SUNRISE, RISET, event, lat, lng, tmzn, rtasc, 
	 estm, &hr, &min );
  sunrzH = (int) hr;  
  sunrzM = (int) min;

  /*  setting phenomena  */
  event = riseset(QUAD3, doy, lng, &rtasc, &estm );
  cooco(SUNSET, RISET, event, lat, lng, tmzn, rtasc, 
	estm, &hr, &min );
  sunsetH = (int) hr;  
  sunsetM = (int) min;

  /* time fix just because things seem to be running a little behind */
  sunrzM+=1;
  if (sunrzM > 59) {
    sunrzM-=60;
    sunrzH+=1;
  }
  sunsetM+=1;
  if (sunsetM > 59) {
    sunsetM-=60;
    sunsetH+=1;
  }

  /*
  if ( sunrzH > 12 ) sunrzH -= 12;
  if ( sunsetH > 12 ) sunsetH -= 12;
  */

  sprintf( sunrise, "%d:%.2d", sunrzH, sunrzM );
  sprintf( sunset, "%d:%.2d", sunsetH, sunsetM );

  return;
}

/*=============================================================*/
void calc_sun2( float latitude, float longitude, double fc_time, double &sunr_time, double &suns_time)
{ 
  double  event, k, z;
  double mm;
  double doy;
  double dd;
  double lat;
  double lng;
  double rtasc;
  double estm;
  double hr;
  double min;
  int tmzn = 0;

  double sunrzH;             /* Rise Hours   Sun     */
  double sunrzM;             /* Rise Minutes Sun     */
  double sunsetH;            /* Set  Hours   Sun     */
  double sunsetM;            /* Set  Minutes Sun     */

  lat = DEG2RAD * latitude;
  lng = DEG2RAD * longitude * -1;

  time_t new_fc_time = (time_t) fc_time;
  struct tm *fc_gtm = gmtime(&new_fc_time);

  doy  = fc_gtm->tm_year + 1900;
  mm  = fc_gtm->tm_mon + 1;
  dd  = fc_gtm->tm_mday;

  struct tm fc_day;
  fc_day.tm_sec = 0;
  fc_day.tm_min = 0;
  fc_day.tm_hour = 0;
  fc_day.tm_year = fc_gtm->tm_year;
  fc_day.tm_mon = fc_gtm->tm_mon;
  fc_day.tm_mday = fc_gtm->tm_mday;

  sunr_time = suns_time = (double) GT_mkgtime(&fc_day);

  /*  day of year  */
  k = (int) ((mm+9.)/12.);
  z = doy/4. - (int) (doy/4.);
  if ( z != 0. )  k *= 2.;
  doy = (int) (275. * mm/9.);
  doy = doy + dd - k - 30.;

  /*  rising phenomena  */
  event = riseset( QUAD1, doy, lng, &rtasc, &estm);

  cooco( SUNRISE, RISET, event, lat, lng, tmzn, rtasc, estm, &hr, &min );
  sunrzH = hr;  
  sunrzM = min;

  /*  setting phenomena  */
  event = riseset(QUAD3, doy, lng, &rtasc, &estm );
  cooco(SUNSET, RISET, event, lat, lng, tmzn, rtasc, estm, &hr, &min );
  sunsetH = hr;  
  sunsetM = min;

  /* time fix just because things seem to be running a little behind */
  sunrzM+=1;
  if (sunrzM > 59) {
    sunrzM-=60;
    sunrzH+=1;
  }
  sunsetM+=1;
  if (sunsetM > 59) {
    sunsetM-=60;
    sunsetH+=1;
  }

  /*
  if ( sunrzH > 12 ) sunrzH -= 12;
  if ( sunsetH > 12 ) sunsetH -= 12;
  */

  // printf("sunrise2 %2.0f:%2.0f\n", sunrzH, sunrzM );
  // printf("sunset2 %2.0f:%2.0f\n", sunsetH, sunsetM );

  /* Do we want this in here or does it matter???
  if (sunsetH < sunrzH)
    sunsetH+=24.;
  */

  sunr_time = sunr_time + (sunrzH*60 + sunrzM)*60;
  suns_time = suns_time + (sunsetH*60 + sunsetM)*60;

  return;
}

/*====================================================*/
/* riseset
 * input:
 *       quad - quadrants of a circle in radians
 *       doy - day of year
 *       lng - longitude
 * output:
 *       rtasc - something to do with right ascention
 *       estm - approximate rise/set time
 * returns:
 *       some angle having to do with rise or setting
 */
double riseset( double quad, double doy, double lng, double *rtasc, 
		double * estm ) 
{

  double L, q, m, z;
  double n;

  /*      approximate rise-or-set time  */
  *estm = doy + ((quad+lng)/QUAD4);

  /* solar mean anomaly */
  L = *estm * .017202 - .0574039;

  /* solar true longitude  */
  m = L + .0334405 * sin(L) + 3.49066e-04 * sin(L*2.) + 4.93289;

  /* Quadrant Determination  */
  while ( m < 0. )         m += QUAD4;
  while ( m >=QUAD4 ) m -= QUAD4;
  z = m/QUAD1 - (int) (m/QUAD1);
  if ( z == 0. ) m += 4.84814e-06;
  n  = (m > QUAD3) ? 2. : 1.;
  n  = (m > QUAD1) ? n  : 0.;

  /*  solar right ascension  */
  *rtasc = sin(m) / cos(m);
  *rtasc = atan( 0.91746 * (*rtasc) );

  /*  quadrant adjustment  */
  /*if ( n!=0. ) *rtasc = (n==2.) ? *rtasc+=QUAD4 : *rtasc+=QUAD2;*/

  if (n!=0.)
    {
      if (n==2.) {
	*rtasc+=QUAD4;
      } else {
	*rtasc+=QUAD2;
      }
    }
        
  /*  solar declination  */
  q = .39782 * sin(m);
  q = atan( q / sqrt(1. - q*q) );
  return q;
}

/*====================================================*/
/*      COOrdinate COnversion  */
/* cooco
* input:
*       updn -
*       code -
*       evtm - some angle of rise or set
*       lat - your latitude
*       lng - your longitude
*       tmzn - your time zone
*       rtasc - some ascention angle
*       estm - estimated time
* output:
*       hr - hour of rise or set
*       min - minute of rise or set
* returns:      
*       something to do with rise or set
*/
double cooco(double updn, double code, double evtm, double lat, double lng, 
	     double tmzn, double rtasc, double estm, double * hr, double * min ) {

  double  s, t, u, v, w, x, y, z;

  s = code - (sin(evtm)*sin(lat));
  s = s   / (cos(evtm)*cos(lat));

         /*  NULL phenomenon  */
  if ( fabs(s) > 1. )  {
    *hr = 0;
    *min = 0;
    return (0.);
  }

  /*  Adjustment */
  s = s / sqrt(1. - s*s);
  s = QUAD1 - atan(s);
  if ( updn != 1. ) s = QUAD4 - s;

  /*  Local Apparent Time  */
  t = s + rtasc - 0.0172028*estm - 1.73364;

  /*  Universal Time  */
  u = t + lng;

  /*  Wall Clock Time  */
  v = u - tmzn;

  /*  Decimal to SEXAgesimal  */
  z = v;
  while ( z < 0.  )  z += QUAD4;
  while ( z >=QUAD4 )  z -= QUAD4;
  z = z * 3.81972;
  v = (int) z ;
  w = (z - v) * 60.;
  x = (int) w ;
  y = w - x;
  if ( y >= .5 )  x++;
  if ( x >= 60.)  v++, x=0.;
  *hr = v;
  *min= x;
  return  v + x/100.;
}



#ifdef THIS_IS_NOT_USED
/*====================================================*/
double fmod(double x, double m)
{
  long i;

  i = (int) (fabs(x)/m); /* compute integer part of x/m */
  if (x < 0) {
    return( x + (i+1)*m);
  } else {
    return( x - i*m);
  }
}
#endif
