//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/04/02 22:08:09 $
//
//==============================================================================

/**
 *
 * @file config_reader.cc
 *
 *  Configuration file reader class implementation
 *
 */

// Include files
#include <stdlib.h>
#include <unordered_set>
#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_types.hh"
#include "config_reader.hh"
#include "libconfig.h++"

using namespace libconfig;

// Constant and macros
const int MAX_LINE = 132;

// Types, structures and classes

// Global variables

// Functions

config_reader::config_reader()
{
}

config_reader::config_reader(const string &input_file_path_arg) : input_file_path(input_file_path_arg)
{
  Config cfg;

  error = string("");

  FILE *fp = fopen(input_file_path.c_str(), "r");

  if (fp == NULL)
    {
      error = string("could not load config file ") + input_file_path;
      return;
    }

  try
    {
      cfg.setAutoConvert(true);
      cfg.read(fp);
      fclose(fp);

      std::unordered_set<string> field_set;

      // get fields
      Setting &field_names_setting = cfg.lookup("RWX.input.field_names");
      for (int i=0; i<field_names_setting.getLength(); i++)
        {
          string field_name = field_names_setting[i];
          field_names.push_back(field_name);
	  field_set.insert(field_name);
        }


      // get time field name
      const char *time_field_char_string = cfg.lookup("RWX.input.time_field_name");
      time_field_name = time_field_char_string;

      // get vehicle id name
      const char *vehicle_id_char_string = cfg.lookup("RWX.input.vehicle_id_name");
      vehicle_id_name = vehicle_id_char_string;

      // get vehicle id ignore prefix 
      const char *vehicle_id_ignore_prefix_char_string = cfg.lookup("RWX.input.vehicle_id_ignore_prefix");
      vehicle_id_ignore_prefix = vehicle_id_ignore_prefix_char_string;

      // get input output field name map
      Setting &input_output_field_setting = cfg.lookup("RWX.input.input_output_field_tuples");
      for (int i=0; i<input_output_field_setting.getLength(); i++)
	{
	  Setting &tuple_setting = input_output_field_setting[i];

	  if (tuple_setting.getLength() != 5)
	    {
	      error = string("input output map tuples should contain exactly 5 elements not %d", tuple_setting.getLength());
	      return;
	    }
	  string input_name = tuple_setting[0];
	  string output_name = tuple_setting[1];
	  string precision = tuple_setting[2];
	  string input_units = tuple_setting[3];
	  string output_units  = tuple_setting[4];
	  input_output_field_t input_output_fields;
	  input_output_fields.input_name = input_name;
	  input_output_fields.output_name = output_name;
	  input_output_fields.precision = precision;
	  input_output_fields.input_units = input_units;
	  input_output_fields.output_units = output_units;

	  input_output_map[input_name] = input_output_fields;
	  if (field_set.find(input_name) == field_set.end())
	    {
	      error = string("input name in output field pair: ") + input_name + string(", ") + output_name + string(" cannot be found in the field names");
	      return;
	    }
	}

      const Setting& root = cfg.getRoot();

      const Setting &district_setting = root["RWX"]["districts"];
      int count = district_setting.getLength();
      for(int i=0; i<count; ++i)
	{
	  const Setting &district = district_setting[i];
	  rwx::district_t district_struct;

	  if (!(district.lookupValue("min_lat", district_struct.min_lat))
	      || !(district.lookupValue("max_lat", district_struct.max_lat))
	      || !(district.lookupValue("min_lon", district_struct.min_lon))
	      || !(district.lookupValue("max_lon", district_struct.max_lon))
	      )
	    {
	      continue;
	    }

	  const char *district_name;
	  district.lookupValue("district_name", district_name);
	  district_struct.district_name = district_name;

	  const char *display_name;
	  district.lookupValue("display_name", display_name);
	  district_struct.display_name = display_name;
	  districts.push_back(district_struct);
	}
    }
  catch (ParseException ce)
    {
      const int size = MAX_LINE;
      char line[size];
      snprintf(line, size, " on line %d", ce.getLine());
      error = string("Config reader parse exception occurred: ") + string(ce.getError()) + line;
    }
  catch (SettingException ce)
    {
      error = string("Config reader setting exception occurred on: ") + ce.getPath();
    }
  catch (FileIOException ce)
    {
      error = string("Config reader file exception occurred on: ") + input_file_path;
    }
  catch (ConfigException ce)
    {
      error = string("Config reader generic exception occurred");
    }
}
