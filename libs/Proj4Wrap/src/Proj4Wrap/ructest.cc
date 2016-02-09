//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: ructest.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010-05-20 21:21:04 $
//
//==============================================================================

/**
 * @file ruc2wrfgrid.cc
 *
 * Convert RUC 130 conus lat-longs to WRF grid coordinates
 *
 * @date 3/25/10
 */

// Include files 
#include <stdio.h>
#include <string>
#include <vector>
#include <projects.h>
#include <boost/format.hpp>
#include "Proj4Wrap.hh"

using boost::format;
using std::string;
using std::vector;

// Functions

int main(int argc, char **argv)
{
  // Uses latitude where lambert conformal projection is true and
  // median aligned with cartesian y-axis
  double ruclov = -95;
  double rucla1 = 45.2931;
  double ruclo1 = -97.588;
  double ruclatin1 = 25.0;
  double ruclatin2 = 25.0;
  double rucdx = 13545.087;
  double rucdy = 13545.087;
  int rucnx = 5;
  int rucny = 4;

  string rucParamString = str(format("+proj=lcc +R=6371200 +lon_0=%1% +lat_0=%2% +lat_1=%3% +lat_2=%4%") % ruclov % ruclatin1 % ruclatin1 % ruclatin2);




  p4w::Proj4Wrap rucLambertProj(rucParamString, p4w::Proj4Wrap::LON_LAT_TYPE, ruclo1, rucla1, rucdx, rucdy);
  printf("ruc false easting, false northing: %g %g\n", rucLambertProj.getFalseEasting(), rucLambertProj.getFalseNorthing());
  double xc;
  double yc;
  double lon;
  double lat;
  
  rucLambertProj.ll2xy(ruclo1, rucla1, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);

  double la2 = 45.6415;
  double lo2 = -96.9482;

  rucLambertProj.ll2xy(lo2, la2, &xc, &yc);
  printf("xc, yc for lo2, la2: %g %g\n", xc, yc);
  rucLambertProj.xy2ll(0, 0, &lon, &lat);
  printf("lon, lat for origin: %g %g\n", lon, lat);

  rucLambertProj.xy2ll(4, 3, &lon, &lat);
  printf("lon, lat for 4,3: %g %g\n", lon, lat);

  double la3 = 45.5245;
  double lo3 = -97.27;
  rucLambertProj.ll2xy(lo3, la3, &xc, &yc);
  printf("xc, yc for lo3, la3: %g %g\n", xc, yc);
}


