//==============================================================================
//
//   (c) Copyright, 2010 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: Proj4Wrap.cc,v $
//       Version: $Revision: 1.7 $  Dated: $Date: 2013/10/25 05:12:36 $
//
//==============================================================================

/**
 * @file Proj4Wrap.cc
 *
 * @date 3/24/10
 */

// Include files 
#include "../include/Proj4Wrap/Proj4Wrap.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


static int projOffsets(projPJ projRef, double lon, double lat, double *xoffset, double *yoffset)
{
  projUV projData;

  projData.u = lon * DEG_TO_RAD;
  projData.v = lat * DEG_TO_RAD;

  projData = pj_fwd(projData, projRef);

  if (projData.u != HUGE_VAL)
    {
      *xoffset = projData.u;
      *yoffset = projData.v;

      return 0;
    }

  return -1;
}


p4w::Proj4Wrap::Proj4Wrap(string &mapParamStringArg, int originTypeArg, double originXArg, double originYArg, double dxArg, double dyArg) : mapParamString(mapParamStringArg), originType(originTypeArg), dx(dxArg), dy(dyArg)
{
  error = "";

  if (mapParamString.find("x_0") != string::npos || mapParamString.find("y_0") != string::npos)
    {
      error = "do not supply false easting or false northing in the map parameter string when using this constructor";
      return;
    }

  if (originType == LON_LAT_TYPE)
    {
      projRef = pj_init_plus(mapParamString.c_str());
      originLon = originXArg;
      originLat = originYArg;

      int ret = projOffsets(projRef, originLon, originLat, &falseEasting, &falseNorthing);
      if (ret < 0)
	{
	  error = "failed proj4 map initialization";
	}

      falseEasting = -falseEasting;
      falseNorthing = -falseNorthing;
    }
  else if (originType == EASTING_NORTHING_TYPE)
    {
      projRef = pj_init_plus(mapParamString.c_str());
      falseEasting = originXArg;
      falseNorthing = originYArg;

      int ret = xy2ll(0, 0, &originLon, &originLat);

      if (ret < 0)
	{
	  error = "failed proj4 map initialization";
	}
    }
  else
    {
      error = "originType must be LON_LAT_TYPE or EASTING_NORTHING_TYPE";
    }
}


int p4w::Proj4Wrap::ll2xy(double lon, double lat, double *xc, double *yc) const
{
  projUV projData;

  projData.u = lon * DEG_TO_RAD;
  projData.v = lat * DEG_TO_RAD;
  projData = pj_fwd(projData, projRef);

  if (projData.u != HUGE_VAL)
    {
      *xc = (projData.u + falseEasting)/dx;
      *yc = (projData.v + falseNorthing)/dy;
      return 0;
    }

  return -1;
}

int p4w::Proj4Wrap::xy2ll(double xc, double yc, double *lon, double *lat) const
{
  projUV projData;

  projData.u = xc * dx - falseEasting;
  projData.v = yc * dy - falseNorthing;
  projData = pj_inv(projData, projRef);

  if (projData.u != HUGE_VAL)
    {
      *lon = projData.u * RAD_TO_DEG;
      *lat = projData.v * RAD_TO_DEG;
      return 0;
    }

  return 1;
}

p4w::Proj4Wrap::~Proj4Wrap()
{
  pj_free(projRef);  
}
