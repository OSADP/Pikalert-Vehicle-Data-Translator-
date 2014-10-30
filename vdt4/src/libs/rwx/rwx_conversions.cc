//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_conversions.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/25 17:24:39 $
//
//==============================================================================

/**
 * @file rwx_conversions.cc
 *
 * @date 10/25/13
 */

// Include files 

#include "rwx_conversions.hh"

const float rwx_conversions::MPH_TO_MS = 0.44704;
const float rwx_conversions::KNOT_TO_MS = 0.514444444;
const float rwx_conversions::MI_TO_KM = 1.609344;
const float rwx_conversions::IN_TO_MM = 25.4;
const float rwx_conversions::K_TO_C = 273.15;
const float rwx_conversions::PA_TO_MB = 0.01;

const float rwx_conversions::FILL_VALUE = -9999.0f;
const float rwx_conversions::MAX_MB = 1090.0;
const float rwx_conversions::MIN_MB = 580.0;

