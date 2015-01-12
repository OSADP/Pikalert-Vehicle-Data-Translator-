//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: convert.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2014/04/04 18:39:53 $
//
//==============================================================================

/**
 * @file convert.cc
 *
 * <file description>
 *
 * @date 4/4/14
 */

// Include files 
#include "convert.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

Convert::Convert()
{
  ut_set_error_message_handler(ut_ignore);
  unitSystem = ut_read_xml(NULL);
}

float Convert::convert(const string &input_units, const string &output_units, float value)
{
  const char* from_string = input_units.c_str();
  const char* to_string = output_units.c_str();

  ut_unit*      from = ut_parse(unitSystem, from_string, UT_ASCII);
  ut_unit*      to = ut_parse(unitSystem, to_string, UT_ASCII);
  cv_converter* converter = ut_get_converter(from, to);
  float toValue = cv_convert_float(converter, value);
  
  cv_free(converter);
  return toValue;
}

double Convert::convert(const string &input_units, const string &output_units, double value)
{
  const char* from_string = input_units.c_str();
  const char* to_string = output_units.c_str();

  ut_unit*      from = ut_parse(unitSystem, from_string, UT_ASCII);
  ut_unit*      to = ut_parse(unitSystem, to_string, UT_ASCII);
  cv_converter* converter = ut_get_converter(from, to);
  double toValue = cv_convert_double(converter, value);
  
  cv_free(converter);
  return toValue;
}

Convert::~Convert()
{
  free(unitSystem);
}

