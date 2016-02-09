//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_climatology_file_reader_test.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 * @file vdt_climatology_file_reader_test.cc
 *
 */

// Include files 
#include <stdio.h>
#include "vdt_climatology_file_reader_test.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

vdt_climatology_file_reader_test::vdt_climatology_file_reader_test()
{
  for (int i=1; i<12; i++)
    {
      month.push_back(i);
    }

  double boulder_lat = 40.0150;
  double boulder_lon = 105.2700;

  latitude.push_back(boulder_lat);
  longitude.push_back(boulder_lon);

  lower_left_latitude = 30;
  lower_left_longitude = 100;
  upper_right_latitude = 50;
  upper_right_longitude = 130;
}

