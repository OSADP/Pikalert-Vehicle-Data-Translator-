//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: lambertGrid.cc,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2012-01-06 19:57:45 $
//
//==============================================================================

/**
 * @file cloudGrid.cc
 *
 * Convert lat-longs cloud grid coordinates
 *
 * @date 3/25/10
 */

// Include files 
#include <stdio.h>
#include <string>
#include <vector>
#include <projects.h>
#include <boost/format.hpp>
#include "LambertConfigReader.hh"
#include "Proj4Wrap.hh"

using boost::format;
using std::string;
using std::vector;

// Functions



void usage(char *programName)
{
  fprintf(stderr, "Usage: %s configFile\n", programName);
}


int main(int argc, char **argv)
{
  printf("argc %d\n", argc);

  if (argc != 2)
    {
      usage(argv[0]);
      exit(2);
    }

  LambertConfigReader cfg(argv[1]);
  if (cfg.error != string(""))
    {
      printf("Error: configuration file error %s\n", cfg.error.c_str());
      return 1;
    }

  // Uses latitude where lambert conformal projection is true and
  // median aligned with cartesian y-axis
  string paramString = str(format("+proj=lcc +R=6371200 +lon_0=%1% +lat_0=%2% +lat_1=%3% +lat_2=%4%") % cfg.lov % cfg.latin1 % cfg.latin1 % cfg.latin2);

  printf("paramString %s\n", paramString.c_str());
  p4w::Proj4Wrap lambertProj(paramString, p4w::Proj4Wrap::LON_LAT_TYPE, cfg.lo1, cfg.la1, cfg.dx, cfg.dy);
  double xc;
  double yc;
  double lon;
  double lat;
  
  lambertProj.ll2xy(cfg.lo1, cfg.la1, &xc, &yc);
  printf("lower corner for %g %g: xc, yc: %g %g\n", cfg.lo1, cfg.la1, xc, yc);
  lambertProj.ll2xy(-57.383, 55.481, &xc, &yc);
  printf("upper corner for -57.383, 55.481 : xc, yc: %g %g\n", xc, yc);
  lambertProj.xy2ll(0, 0, &lon, &lat);
  printf("0,0 lon, lat: %g %g\n", lon, lat);
  lambertProj.xy2ll(1, 0, &lon, &lat);
  printf("1,0 lon, lat: %g %g\n", lon, lat);
  lambertProj.xy2ll(2, 0, &lon, &lat);
  printf("2,0 lon, lat: %g %g\n", lon, lat);
  lambertProj.xy2ll(cfg.nx-1, cfg.ny-1, &lon, &lat);
  printf("%d, %d, lon, lat: %g %g\n", cfg.nx-1, cfg.ny-1, lon, lat);
  /*
  */

  vector<float> xvec;
  vector<float> yvec;

  for (int i=0; i<cfg.ny; i++)
    {
      for (int j=0; j<cfg.nx; j++)
	{
	  lat = cfg.la1 + i * cfg.dy;
	  lon = cfg.lo1 + j * cfg.dx;

	  lambertProj.ll2xy(lon, lat, &xc, &yc);
	  xvec.push_back(xc);
	  yvec.push_back(yc);
	}
    }

  // Print cdl header

  printf("netcdf coord {\n");
  printf("dimensions:\n");
  printf("y = %d ;\n", cfg.ny);
  printf("x = %d ;\n", cfg.nx);
  printf("variables:\n");
  printf("float x(y, x) ;\n");
  printf("x:long_name = \"x coordinate\";\n") ;
  printf("float y(y, x) ;\n");
  printf("y:long_name = \"y coordinate\";\n") ;
  printf("data:\n\n");

  printf("x = \n");
  for (uint i=0; i<xvec.size()-1; i++)
    {
      printf("%.7f,\n", xvec[i]);
    }

  printf("%.7f;\n", xvec[xvec.size()-1]);

  printf("y = \n");

  for (uint i=0; i<yvec.size()-1; i++)
    {
      printf("%.7f,\n", yvec[i]);
    }
  printf("%.7f;\n", yvec[yvec.size()-1]);


  printf("}\n");
}


