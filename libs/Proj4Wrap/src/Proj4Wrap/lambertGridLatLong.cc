//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: lambertGridLatLong.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2012-01-04 00:33:41 $
//
//==============================================================================

/**
 * @file lambertGridLatLong.cc
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
#include "writeBinary.hh"

using boost::format;
using std::string;
using std::vector;

// Functions



void usage(char *programName)
{
  fprintf(stderr, "Usage: %s configFile latFile lonFile\n", programName);
}


int main(int argc, char **argv)
{
  if (argc != 4)
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

  p4w::Proj4Wrap lambertProj(paramString, p4w::Proj4Wrap::LON_LAT_TYPE, cfg.lo1, cfg.la1, cfg.dx, cfg.dy);
  double xc;
  double yc;
  double lon;
  double lat;
  
  vector<float> xvec;
  vector<float> yvec;

  for (int i=0; i<cfg.ny; i++)
    {
      for (int j=0; j<cfg.nx; j++)
	{
	  xc = j;
	  yc = i;
	  lambertProj.xy2ll(xc, yc, &lon, &lat);
	  xvec.push_back(lon);
	  yvec.push_back(lat);
	}
    }

  string error;
  writeBinary(yvec, argv[2], error);
  writeBinary(xvec, argv[3], error);
}


