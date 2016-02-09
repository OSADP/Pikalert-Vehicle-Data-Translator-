//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: main.cc,v $
//       Version: $Revision: 1.10 $  Dated: $Date: 2013/08/25 18:46:51 $
//
//==============================================================================

/**
 * @file main.cc
 *
 * <file description>
 *
 * @date 7/1/13
 */

// Include files 
#include "rwx/rwx_cdf_field_writer.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

using namespace rwx;

void example1()
{
  // Set dimensions
  unordered_map <string, size_t> dimension_map;

  dimension_map["count"] = 10;

  // Open output file
  rwx_cdf_field_writer::rwx_cdf_field_writer cdf_file("test_write.nc", dimension_map);

  vector<short> test_short_field(10, 5);
  vector<string> field_dimension_names;

  field_dimension_names.push_back("count");
  string long_name = "bogus_long_name";
  string units = "bogus";
  short missing = -99;
  string error;
  cdf_file.add_field("test_short_field", netCDF::ncShort, field_dimension_names, long_name, units, missing, error);
  if (error != "")
    {
      printf("error: %s\n", error.c_str());
    }

  int missing_int = -999;
  vector<int> test_int_field(10, 4);
  cdf_file.add_field("test_int_field", netCDF::ncInt, field_dimension_names, long_name, units, missing_int, error);
  if (error != "")
    {
      printf("error: %s\n", error.c_str());
    }

  cdf_file.put_field("test_short_field", test_short_field, error);
  cdf_file.put_field("test_int_field", test_int_field, error);
}

void example2()
{
  // Open output file
  rwx_cdf_field_writer::rwx_cdf_field_writer cdf_file("test_write.cdl", "test_write2.nc");

  vector<short> test_short_field(10, 5);
  vector<int> test_int_field(10, 4);
  string error;

  cdf_file.put_field("test_short_field", test_short_field, error);
  cdf_file.put_field("test_int_field", test_int_field, error);
}

int main(int argc, char **argv)
{
  example2();
  return 0;
}
