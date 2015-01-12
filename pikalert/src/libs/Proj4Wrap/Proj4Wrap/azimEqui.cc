// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

// Include files 

#include <stdio.h>
#include <string>
#include <vector>
#include <projects.h>
#include "Proj4Wrap.hh"

//#include <proj_api.h>

// Constant, macro and type definitions 

// Global variables 

// Functions and objects

using namespace std;

int MAX_LINE = 132;


int main(int argc, char **argv)
{
  double la1 = 40.65;
  double lo1 = -102.6559;
  double dx = 4000;
  double dy = 4000;

  string paramString = string("+proj=aeqd +R=6371200 +lon_0=-102.6559 +lat_0=40.65");
  p4w::Proj4Wrap aeqdProj(paramString, p4w::Proj4Wrap::EASTING_NORTHING_TYPE, 0, 0, dx, dy);

  double xc = -216000;
  double yc = -156000;
  double lon;
  double lat;

  aeqdProj.xy2ll(xc, yc, &lon, &lat);
  printf("[%lg %lg]: %lg %lg\n", xc, yc, lon, lat);

  lon = lo1;
  lat = la1;
  aeqdProj.ll2xy(lon, lat, &xc, &yc);
  printf("%lg %lg\n", xc, yc);


  lon = -105.157;
  lat = 39.218;
  aeqdProj.ll2xy(lon, lat, &xc, &yc);
  printf("%lg %lg\n", xc, yc);

  lon = -99.998;
  lat = 42.060;
  aeqdProj.ll2xy(lon, lat, &xc, &yc);
  printf("%lg %lg\n", xc, yc);

}
