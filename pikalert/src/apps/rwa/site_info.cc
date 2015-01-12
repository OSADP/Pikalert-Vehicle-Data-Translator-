//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: site_info.cc,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013/12/16 21:45:49 $
//
//==============================================================================

/**
 * @file site_info.cc
 *
 * <file description>
 *
 * @date 11/13/13
 */

#define strtk_no_tr1_or_boost

// Include files 
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "site_info.hh"
#include "strtk.hpp"

// Constants and macros
const int SITE_LIST_NUM_FIELDS = 9;

// Types

// Structures, unions and classes

// Global variables 

// Functions

site_info::site_info(const std::string &file_name)
{
  std::ifstream input_stream(file_name);
  std::string line;

  strtk::single_delimiter_predicate<std::string::value_type> predicate(';');
  while (getline(input_stream, line))
    {
      site_fields site_field_record;
      
      strtk::parse(line, ";", site_field_record.id, site_field_record.wmo_id, site_field_record.aux_id_string, site_field_record.lat, site_field_record.lon, site_field_record.altitude, site_field_record.wmo_region, site_field_record.long_name, site_field_record.state, site_field_record.country);

      fcst_site_map[site_field_record.aux_id_string] = site_field_record;
    }
}

site_info::~site_info()
{
}
