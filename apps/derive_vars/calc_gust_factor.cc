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

#include <netcdf.h>

const float DEFAULT_GUST_FACTOR = 2.0; // Default gust factor is it cannot be otherwise estimated

int calc_gust_ratio(float prev_wspd, float next_wspd, float max_gust, float *gust_factor)
{
  float prev_gfac, next_gfac;

  prev_gfac = NC_FILL_FLOAT;
  if (prev_wspd != 0.)
    prev_gfac = max_gust / prev_wspd;

  next_gfac = NC_FILL_FLOAT;
  if (next_wspd != 0.)
    next_gfac = max_gust / next_wspd;

  //printf("prev %f  next %f  gust %f  prev_gf %f  next_gf %f\n", prev_wspd, next_wspd, max_gust, prev_gfac, next_gfac);

  if (prev_gfac == NC_FILL_FLOAT && next_gfac == NC_FILL_FLOAT)
    *gust_factor = DEFAULT_GUST_FACTOR;
  else if (prev_gfac == NC_FILL_FLOAT)
    *gust_factor = next_gfac;
  else if (next_gfac == NC_FILL_FLOAT)
    *gust_factor = prev_gfac;
  else
    {
      if (prev_gfac < next_gfac)
	*gust_factor = prev_gfac;
      else
	*gust_factor = next_gfac;
    }

  return(0);
}
