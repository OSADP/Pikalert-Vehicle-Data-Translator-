//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file great_circle.hh
 *
 * @class Great_Circle
 *
 *  <file/class description>
 *  
 * @date 2/4/15
 */

#ifndef GREAT_CIRCLE_HH
#define GREAT_CIRCLE_HH

#include <math.h>


const float GRC_PI = M_PI;
const float GRC_PI2 = GRC_PI/2;
const float GRC_PI4 = GRC_PI/4;
const float GRC_D2R = GRC_PI/180.;
const float GRC_R2D = 180/GRC_PI;
const float GRC_RADIUS = 6371229.;
const float GRC_OMEGA4 = 4.0*GRC_PI/86400.;
const float GRC_ONEDEG = (GRC_RADIUS*GRC_PI)/180.0;

const float GRC_EARTH_RADIUS = 6367.0;
const float GRC_EARTH_RADIUS_M = 6371200.0;

double great_circle_distance(double lat1, double lon1, double lat2, double lon2);

#endif /* GREAT_CIRCLE_HH */
