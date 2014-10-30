//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.16 $  Dated: $Date: 2014/02/05 00:31:02 $
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
#include <boost/lexical_cast.hpp>
#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_types.hh"
#include "config_reader.hh"
#include "libconfig.h++"
#include "site_alert.hh"

using boost::lexical_cast;
using namespace libconfig;

// Constant and macros
const int MAX_LINE = 132;

// Types, structures and classes

// Global variables

// Functions

config_reader::config_reader()
{
}

int get_alert_conditions(const Config &cfg, const string &condition_id, vector<alert_class_type> &alert_condition_type, vector<vector<op_name_value_class> > &conditions, string &error)
{
  // get obs alert conditions
  Setting &condition_setting = cfg.lookup(condition_id);
  if (condition_setting.getLength() > 0)
    {
      conditions.resize(condition_setting.getLength());
    }

  for (int i=0; i<condition_setting.getLength(); i++)
    {
      Setting &condition_list_setting = condition_setting[i];
      if (condition_list_setting.getLength() > 0)
	{
	  conditions[i].resize(condition_list_setting.getLength()-1);
	}

      for (int j=0; j<condition_list_setting.getLength(); j++)
	{
	  int end_index = condition_list_setting.getLength()-1;
	  if (j == end_index)
	    {
	      Setting &condition_class_type_setting = condition_list_setting[end_index];
	      if (condition_class_type_setting.getLength() != 9)
		{
		  error = "condition_class_type_setting is of incorrect length";
		  return -1;
		}
	      
	      string condition_class = condition_class_type_setting[0];
	      string condition_type = condition_class_type_setting[1];
	      string condition_code = condition_class_type_setting[2];
	      string condition_alert_time_class = condition_class_type_setting[3];
	      string maw_condition = condition_class_type_setting[4];
	      string maw_action = condition_class_type_setting[5];
	      int maw_priority = condition_class_type_setting[6];
	      int maw_condition_code = condition_class_type_setting[7];
	      int maw_action_code = condition_class_type_setting[8];

	      alert_class_type class_type;
	      class_type.alert_class = condition_class;
	      class_type.alert_type = condition_type;
	      class_type.maw_condition = maw_condition;
	      class_type.maw_action = maw_action;
	      class_type.maw_priority = maw_priority;
	      class_type.maw_condition_code = maw_condition_code;
	      class_type.maw_action_code = maw_action_code;

	      if (condition_code == "clear")
		{
		  class_type.alert_code = AC_CLEAR;
		}
	      else if (condition_code == "warning")
		{
		  class_type.alert_code = AC_WARNING;
		}
	      else if (condition_code == "alert")
		{
		  class_type.alert_code = AC_ALERT;
		}
	      else if (condition_code == "missing")
		{
		  class_type.alert_code = AC_MISSING;
		}

	      if (condition_alert_time_class == "obs")
		{
		  class_type.alert_time_class = ATC_OBS;
		}
	      else if (condition_alert_time_class == "fcst")
		{
		  class_type.alert_time_class = ATC_FCST;
		}
	      else if (condition_alert_time_class == "both")
		{
		  class_type.alert_time_class = ATC_BOTH;
		}

	      alert_condition_type.push_back(class_type);
	      break;
	    }

	  Setting &condition_item_setting = condition_list_setting[j];
	  if (condition_item_setting.getLength() != 3)
	    {
	      error = "condition_item_setting is not of length 3";
	      return -1;
	    }

	  string op = condition_item_setting[0];
	  string name = condition_item_setting[1];
	  double value = condition_item_setting[2];
	  op_name_value_class op_name_value(op, name, value);
	  if (op_name_value.error != "")
	    {
	      error = op_name_value.error;
	      return -1;
	    }
	  conditions[i][j] = op_name_value;
	}
    }
      
  return 0;
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

      // get road segment file name 
      const char *road_segment_file_char_string = cfg.lookup("RWA.road_segment.road_segment_file");

      road_segment_file = road_segment_file_char_string;

      // get road segment na
      road_segment_na = cfg.lookup("RWA.road_segment.road_segment_na");
      

      // get rwh time field name
      const char *rwh_time_field_char_string = cfg.lookup("RWA.input.rwh_time_field_name");
      rwh_time_field_name = rwh_time_field_char_string;

      // get treatment forecast time field name
      const char *treatment_forecast_time_field_char_string = cfg.lookup("RWA.input.treatment_forecast_time_field_name");
      treatment_forecast_time_field_name = treatment_forecast_time_field_char_string;

      alert_conditions_class cfg_alert_conditions;

      // get alert conditions
      if (get_alert_conditions(cfg, "RWA.input.conditions", cfg_alert_conditions.class_type, cfg_alert_conditions.conditions, error) != 0)
	{
	  return;
	}

      alert_conditions = new alert_conditions_class(cfg_alert_conditions.conditions, cfg_alert_conditions.class_type);

      // check alert conditions
      if (alert_conditions->check_all_alert_conditions(error) == false)
	{
	  return;
	}

      // get rwh fields
      Setting &rwh_field_names_setting = cfg.lookup("RWA.input.rwh_field_names");
      for (int i=0; i<rwh_field_names_setting.getLength(); i++)
        {
          string rwh_field_name = rwh_field_names_setting[i];
          rwh_field_names.push_back(rwh_field_name);
        }

      // get treatment fields
      Setting &treatment_field_names_setting = cfg.lookup("RWA.input.treatment_field_names");
      for (int i=0; i<treatment_field_names_setting.getLength(); i++)
        {
          string treatment_field_name = treatment_field_names_setting[i];
          treatment_field_names.push_back(treatment_field_name);
        }

      // get observation statistics fields
      Setting &obs_stats_field_names_setting = cfg.lookup("RWA.input.obs_stats_field_names");
      for (int i=0; i<obs_stats_field_names_setting.getLength(); i++)
        {
          string obs_stats_field_name = obs_stats_field_names_setting[i];
          obs_stats_field_names.push_back(obs_stats_field_name);
        }

      // get road segment fields
      Setting &road_segment_field_names_setting = cfg.lookup("RWA.road_segment.road_segment_field_names");
      for (int i=0; i<road_segment_field_names_setting.getLength(); i++)
        {
          string road_segment_field_name = road_segment_field_names_setting[i];
          road_segment_field_names.push_back(road_segment_field_name);
        }

    const Setting& root = cfg.getRoot();

    const Setting &district_setting = root["RWA"]["districts"];
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

config_reader::~config_reader()
{
  delete alert_conditions;
}
