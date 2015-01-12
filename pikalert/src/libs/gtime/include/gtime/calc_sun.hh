// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 *   Module: calc_sun.hh
 *
 *   Author: Melissa Petty
 *
 *   Date:   11/18/99
 *
 *   Description: module to determine sunrise and sunset at a particular
 *      location.
 */

#ifndef CALC_SUN_HH
#define CALC_SUN_HH

double riseset( double quad, double doy, double lng, double *rtasc, 
		double * estm );
double cooco(double updn, double code, double evtm, double lat, double lng, 
	     double tmzn, double rtasc, double estm, double * hr, double * min );

void calc_sun(float latitude, float longitude, int tz, int dst, int year, int month, int mday, char * sunrise, char * sunset );

void calc_sun2(float latitude, float longitude, double fc_time, double &sunr_time, double &suns_time);


#endif /* CALC_SUN_HH */
