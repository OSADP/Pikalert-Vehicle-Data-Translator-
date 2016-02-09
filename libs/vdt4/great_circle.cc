//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file great_circle.cc
 *
 * <file description>
 *
 * @date 2/4/15
 */

// Include files 
#include "great_circle.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

// Return the great circle distance between two lat, lons
double great_circle_distance(double lat1, double lon1, double lat2, double lon2)
{
  double rlat1 = lat1 * GRC_D2R;
  double rlat2 = lat2 * GRC_D2R;
  double rlon1 = lon1 * GRC_D2R;
  double rlon2 = lon2 * GRC_D2R;
  double delta_lat = rlat2 - rlat1;
  double delta_lon = rlon2 - rlon1;
  double temp;
  double distance;

  // Find Great Circle distance 
  temp = pow(sin(delta_lat / 2.0), 2) + cos(rlat1) * cos(rlat2) * pow(sin(delta_lon / 2.0), 2);

  distance = GRC_EARTH_RADIUS * 2 * atan2(sqrt(temp), sqrt(1 - temp));

  return (distance);
}
