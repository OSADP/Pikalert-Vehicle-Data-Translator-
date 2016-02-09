// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: snow_density.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/21/02
 *
 *  * Description: 
 *              This file contains one routine which calculates
 * the snow-to-liquid ratio of falling precip given certain 
 * environmental conditions. Based on results from Jordan and
 * Andreas, Apr 15 1999, "Heat Budget of snow-covered sea ice
 * at North Pole 4", JGR vol 104, no.c4, pp7785-7806. Values
 * from plots were eye-balled and inserted into tables below.
 * It is not clear how correct these estimates are since they 
 * were generated from Antarctic data. Instead of using these
 * values, an idealized snow-to-liquid 10-1 ratio is assumed in
 * the MDSS.
 *     
 *       snow_density()...Main routine described above.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "layer.hh"
#include "WxFcst.hh"

/* Constant definitions / Macro definitions / Type definitions */
const int num_air_temp_curves = 3;
const float air_temp[num_air_temp_curves] = {-13, -3, 2};
const int num_wind_speeds = 8;
const float wspd[num_wind_speeds] = {0, 3, 5, 10, 15, 20, 25, 30};
const float density[num_wind_speeds][num_air_temp_curves] =
{{ 45,  80, 175},
 { 70, 100, 200},
 {100, 130, 215},
 {200, 220, 290},
 {300, 310, 370},
 {390, 395, 420},
 {435, 440, 450},
 {470, 470, 475}};

/* External global variables / Non-static global variables / Static globals */
extern Log *proc_log;

float snow_density(float T, float ws)
{
  float ratio;
  int wspd_cat;
  int temp_cat;
  float interp;
  float dens;

  wspd_cat = 0;
  while (wspd_cat < num_wind_speeds && 
	 ws <= wspd[wspd_cat])
    wspd_cat++;

  temp_cat = 0;
  while (temp_cat <= num_air_temp_curves &&
	 T < air_temp[temp_cat])
    temp_cat++;

  if (temp_cat == 0)
    {
      if (wspd_cat == 0)
	ratio = 1000. / density[wspd_cat][temp_cat];
      else if (wspd_cat == num_wind_speeds)
	ratio = 1000. / density[wspd_cat-1][temp_cat];
      else
	{
	  // interpolate between wspds
	  interp = (ws - wspd[wspd_cat-1]) /
	    (wspd[wspd_cat] - wspd[wspd_cat-1]);
	  dens = (1. - interp) * density[wspd_cat-1][temp_cat] + 
	    interp * density[wspd_cat][temp_cat];
	  ratio = 1000. / dens;
	}
    }
  else if (temp_cat == num_air_temp_curves)
    {
      if (wspd_cat == 0)
	ratio = 1000. / density[wspd_cat][temp_cat-1];
      else if (wspd_cat == num_wind_speeds)
	ratio = 1000. / density[wspd_cat-1][temp_cat-1];
      else
	{
	  // interpolate between wspds
	  interp = (ws - wspd[wspd_cat-1]) /
	    (wspd[wspd_cat] - wspd[wspd_cat-1]);
	  dens = (1. - interp) * density[wspd_cat-1][temp_cat-1] + 
	    interp * density[wspd_cat][temp_cat-1];
	  ratio = 1000. / dens;
	}
    }
  else
    {
      if (wspd_cat == 0)
	{
	  // interpolate between temp
	  interp = (T - air_temp[temp_cat-1]) /
	    (air_temp[temp_cat] - air_temp[temp_cat-1]);
	  dens = (1. - interp) * density[wspd_cat][temp_cat] + 
	    interp * density[wspd_cat][temp_cat];
	  ratio = 1000. / dens;
	}
      else if (wspd_cat == num_wind_speeds)
	{
	  // interpolate between temps
	  interp = (T - air_temp[temp_cat-1]) /
	    (air_temp[temp_cat] - air_temp[temp_cat-1]);
	  dens = (1. - interp) * density[wspd_cat-1][temp_cat] + 
	    interp * density[wspd_cat-1][temp_cat];
	  ratio = 1000. / dens;
	}
      else 
	{
	  // do cheap bilinear interp
	  interp = (T - air_temp[temp_cat-1]) /
	    (air_temp[temp_cat] - air_temp[temp_cat-1]);
	  float dens0 = (1. - interp) * density[wspd_cat][temp_cat-1] + 
	    interp * density[wspd_cat-1][temp_cat-1];
	  float dens1 = (1. - interp) * density[wspd_cat][temp_cat] + 
	    interp * density[wspd_cat-1][temp_cat];

	  interp = (ws - wspd[wspd_cat-1]) /
	    (wspd[wspd_cat] - wspd[wspd_cat-1]);

	  dens = (1. - interp) * dens0 + interp * dens1;
	  ratio = 1000. / dens;
	}
    }


  return(ratio);
}
