// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <math.h>

#include "netcdf.hh"

const float WATER_DENSITY = 1000.; // kg/m^3
const float DEFAULT_RATIO = 10.;

float calc_snow_to_swe_ratio(float T, float wspd)
{
  float snow_density;

  if (T == NC_FILL_FLOAT || wspd == NC_FILL_FLOAT)
    return(DEFAULT_RATIO);

  // convert T to Kelvin
  T += 273.15;

  // For extremely warm temps, set ratio to that of warmest
  // temp in formula (36.5F)
  if (T > 275.65)
    T = 275.65;

  if (T > 260.15)   // 8.6F
    {
      float exponent = -1.4 * pow(278.15 - T, -1.15) - 0.008 * pow(wspd, 1.7);
      snow_density = 500. * (1. - 0.951 * exp(exponent));
      //printf("exp %f  density %f\n", exponent, snow_density);
    }
  else
    {
      float exponent = -0.008 * pow(wspd, 1.7);
      snow_density = 500. * (1. - 0.904 * exp(exponent));
    }

  if (snow_density <= 0)
    return(DEFAULT_RATIO);

  return(WATER_DENSITY / snow_density);
}
