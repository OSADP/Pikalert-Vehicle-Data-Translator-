//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 18:32:17 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/config_reader.cc
 *
 *  Configuration file reader class implementation
 *
 * @date 02/24/12
 */

// Include files
#include <stdlib.h>
#include <boost/format.hpp>

using boost::format;

#include "config_reader.hh"
#include "libconfig.h++"

using namespace libconfig;

// Constant and macros
static const int MAX_LINE = 1024;
static const float VALID_LAT_MIN = -90.0;
static const float VALID_LAT_MAX = 90.0;
static const float VALID_LON_MIN = -180.0;
static const float VALID_LON_MAX = 180.0;


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


      Setting &fields_setting = cfg.lookup("Vdt.fields");
      for (int i=0; i<fields_setting.getLength(); i++)
	{
	  string field_string = fields_setting[i];
	  field_names.push_back(field_string);
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
