//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: ruc2wrfgrid.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2010-05-05 19:29:17 $
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
  double rucla1 = 16.281;
  double ruclo1 = -126.138;
  double ruclatin1 = 25.0;
  double ruclatin2 = 25.0;
  double rucdx = 13545.087;
  double rucdy = 13545.087;
  int rucnx = 451;
  int rucny = 337;

  string rucParamString = str(format("+proj=lcc +R=6371200 +lon_0=%1% +lat_0=%2% +lat_1=%3% +lat_2=%4%") % ruclov % ruclatin1 % ruclatin1 % ruclatin2);




  p4w::Proj4Wrap rucLambertProj(rucParamString, p4w::Proj4Wrap::LON_LAT_TYPE, ruclo1, rucla1, rucdx, rucdy);
  printf("ruc false easting, false northing: %g %g\n", rucLambertProj.getFalseEasting(), rucLambertProj.getFalseNorthing());
  double xc;
  double yc;
  double lon;
  double lat;
  
  rucLambertProj.ll2xy(ruclo1, rucla1, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj.ll2xy(-57.383, 55.481, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj.xy2ll(0, 0, &lon, &lat);
  printf("lon, lat: %g %g\n", lon, lat);


  p4w::Proj4Wrap rucLambertProj1(rucParamString, p4w::Proj4Wrap::EASTING_NORTHING_TYPE, 3.33214e+06, 588890, rucdx, rucdy);
  printf("EASTINGNORTHING\n");
  printf("ruc lon lat: %g %g\n", rucLambertProj1.getOriginLon(), rucLambertProj1.getOriginLat());
  
  rucLambertProj1.ll2xy(ruclo1, rucla1, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj1.ll2xy(-57.383, 55.481, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj1.xy2ll(0, 0, &lon, &lat);
  printf("lon, lat: %g %g\n", lon, lat);

  p4w::Proj4Wrap rucLambertProj2(rucParamString, p4w::Proj4Wrap::EASTING_NORTHING_TYPE, 0, 0, rucdx, rucdy);
  printf("EASTINGNORTHING\n");
  printf("ruc lon lat: %g %g\n", rucLambertProj2.getOriginLon(), rucLambertProj2.getOriginLat());
  
  rucLambertProj2.ll2xy(ruclo1, rucla1, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj2.ll2xy(-57.383, 55.481, &xc, &yc);
  printf("xc, yc: %g %g\n", xc, yc);
  rucLambertProj2.xy2ll(0, 0, &lon, &lat);
  printf("lon, lat: %g %g\n", lon, lat);

  double wrflov = -95;
  double wrfla1 = 1.066;
  double wrflo1 = -128.499;
  double wrflatin1 = 45.0;
  double wrflatin2 = 45.0;
  double wrfdx = 13545.087;
  double wrfdy = 13545.087;

  printf("\n");
  string wrfParamString = str(format("+proj=lcc +R=6370000 +lon_0=%1% +lat_1=%2% +lat_2=%3%") % wrflov % wrflatin1 % wrflatin2);

  p4w::Proj4Wrap wrfLambertProj(wrfParamString, p4w::Proj4Wrap::LON_LAT_TYPE, wrflo1, wrfla1, wrfdx, wrfdy);

  printf("netcdf rucwrf_coord {\n");
  printf("dimensions:\n");
  printf("\ty = %d;\n", rucny);
  printf("\tx = %d;\n", rucnx);
  printf("variables:\n");
  printf("float x(y, x);\n");
  printf("x:long_name = \"x coordinate\";\n");
  printf("float y(y, x);\n");
  printf("y:long_name = \"y coordinate\";\n");
  printf("data:\n");
  printf("x = \n");

  vector<float> xvec;
  vector<float> yvec;
  for (int i=0; i<337; i++)
    for (int j=0; j<451; j++)
      {
	double xc = j;
	double yc = i;
	double lat;
	double lon;
	double lat1;
	double lon1;

	rucLambertProj.xy2ll(xc, yc, &lon, &lat);
	wrfLambertProj.ll2xy(lon, lat, &xc, &yc);
	wrfLambertProj.xy2ll(xc, yc, &lon1, &lat1);

	if (fabs(lat-lat1) > 0.000001 || fabs(lon-lon1) > 0.000001)
	  printf("ERROR: lat diff, lon diff: %g %g\n", fabs(lat-lat1), fabs(lon-lon1));
	xvec.push_back(xc);
	yvec.push_back(yc);
      }

  int totalCt = rucnx * rucny;
  for (int i=0; i<totalCt-1; i++)
    {
      printf("%.7lf,\n", xvec[i]);
    }
  printf("%.7lf;\n", xvec[totalCt-1]);

  printf("y = \n");

  for (int i=0; i<totalCt-1; i++)
    {
      printf("%.7lf,\n", yvec[i]);
    }
  printf("%.7lf;\n", yvec[totalCt-1]);

  printf("}\n");
}


