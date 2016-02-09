//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2013/10/28 19:27:14 $
//
//==============================================================================

/**
 *
 * @file apps/rwh/config_reader.cc
 *
 *  Configuration file reader class implementation
 *
 */

// Include files
#include <netcdf>
#include <stdlib.h>
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
      
      // CDL file paths
      //
      const char *rwh_cdl_file_string = cfg.lookup("RWH.output_config.rwh_cdl");
      rwh_cdl_file = rwh_cdl_file_string;
      
      // Forecast input
      //
      // get rdwx (road weather forecast) fields
      Setting &rdwx_field_names_setting = cfg.lookup("RWH.forecast_input.rdwx_field_names");
      for (int i=0; i<rdwx_field_names_setting.getLength(); i++)
        {
          string field_name = rdwx_field_names_setting[i];
          rdwx_field_names.push_back(field_name);
        }
      
      // get rcst (road condition suff-tmt forecast) fields
      Setting &rcst_field_names_setting = cfg.lookup("RWH.forecast_input.rcst_field_names");
      for (int i=0; i<rcst_field_names_setting.getLength(); i++)
        {
          string field_name = rcst_field_names_setting[i];
          rcst_field_names.push_back(field_name);
        }

      // Previous RWH file input variables
      //
      // get rwh fields
      Setting &rwh_field_names_setting = cfg.lookup("RWH.prev_rwh_input.rwh_field_names");
      for (int i=0; i<rwh_field_names_setting.getLength(); i++)
        {
          string field_name = rwh_field_names_setting[i];
          rwh_field_names.push_back(field_name);
        }
      
      // Input (RWX cdf field reader input)
      //
      // get char fields
      Setting &char_field_names_setting = cfg.lookup("RWH.input.char_field_names");
      for (int i=0; i<char_field_names_setting.getLength(); i++)
        {
          string field_name = char_field_names_setting[i];
          char_field_names.push_back(field_name);
	  std::pair<int, int> pair(NC_CHAR, i);
          input_type_index_map[field_name] = pair;
        }

      // get short fields
      Setting &short_field_names_setting = cfg.lookup("RWH.input.short_field_names");
      for (int i=0; i<short_field_names_setting.getLength(); i++)
        {
          string field_name = short_field_names_setting[i];
          short_field_names.push_back(field_name);
	  std::pair<int, int> pair(NC_SHORT, i);
          input_type_index_map[field_name] = pair;
        }

      // get int fields
      Setting &int_field_names_setting = cfg.lookup("RWH.input.int_field_names");
      for (int i=0; i<int_field_names_setting.getLength(); i++)
        {
          string field_name = int_field_names_setting[i];
          int_field_names.push_back(field_name);
	  std::pair<int, int> pair(NC_INT, i);
          input_type_index_map[field_name] = pair;
        }

      // get float fields
      Setting &float_field_names_setting = cfg.lookup("RWH.input.float_field_names");
      for (int i=0; i<float_field_names_setting.getLength(); i++)
        {
          string field_name = float_field_names_setting[i];
          float_field_names.push_back(field_name);
	  std::pair<int, int> pair(NC_FLOAT, i);
          input_type_index_map[field_name] = pair;
        }

      // get double fields
      Setting &double_field_names_setting = cfg.lookup("RWH.input.double_field_names");
      for (int i=0; i<double_field_names_setting.getLength(); i++)
        {
          string field_name = double_field_names_setting[i];
          double_field_names.push_back(field_name);
	  std::pair<int, int> pair(NC_DOUBLE, i);
          input_type_index_map[field_name] = pair;
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
