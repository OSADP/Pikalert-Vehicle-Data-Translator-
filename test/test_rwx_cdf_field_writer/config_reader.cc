//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/08/25 18:51:01 $
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

      // get char fields
      Setting &char_field_names_setting = cfg.lookup("RWX.input.char_field_names");
      for (int i=0; i<char_field_names_setting.getLength(); i++)
        {
          string field_name = char_field_names_setting[i];
          char_field_names.push_back(field_name);
        }

      // get int fields
      Setting &int_field_names_setting = cfg.lookup("RWX.input.int_field_names");
      for (int i=0; i<int_field_names_setting.getLength(); i++)
        {
          string field_name = int_field_names_setting[i];
          int_field_names.push_back(field_name);
        }

      // get float fields
      Setting &float_field_names_setting = cfg.lookup("RWX.input.float_field_names");
      for (int i=0; i<float_field_names_setting.getLength(); i++)
        {
          string field_name = float_field_names_setting[i];
          float_field_names.push_back(field_name);
        }

      // get double fields
      Setting &double_field_names_setting = cfg.lookup("RWX.input.double_field_names");
      for (int i=0; i<double_field_names_setting.getLength(); i++)
        {
          string field_name = double_field_names_setting[i];
          double_field_names.push_back(field_name);
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
