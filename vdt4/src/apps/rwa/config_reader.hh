//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: config_reader.hh,v $
//       Version: $Revision: 1.11 $  Dated: $Date: 2014/03/31 19:03:35 $
//
//==============================================================================

/**
 *
 * @file config_reader.hh
 *
 * @class config_reader
 *
 *  Configuration file reader class
 *
 */

#ifndef CONFIG_READER_HH
#define CONFIG_READER_HH

#include <string>
#include <unordered_map>
#include <vector>
#include "rwx/rwx_types.hh"
#include "alert_conditions.hh"

using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

/**
 * @class config_reader
 */
class config_reader
{
public:

  /**
   * @brief config_reader constructor
   *
   * @param[in] input_file_path_arg input configuration file
   * @note the variable error is set if the constructor fails
   */
  config_reader(const string &input_file_path_arg);

  /**
   * @brief config_reader constructor
   */
  config_reader();

  ~config_reader();

  /** @brief rwh time field name */
  string rwh_time_field_name;

  /** @brief treatment forecast time field name */
  string treatment_forecast_time_field_name;

  /** @brief forecast alert conditions */
  alert_conditions_class *alert_conditions;

  /** @brief input_file_path input file path to config file */
  string input_file_path;

  /** @brief rwh field names to be read */
  vector<string> rwh_field_names;

  /** @brief observation statistics field names to be read */
  vector<string> obs_stats_field_names;

  /** @brief treatment field names to be read */
  vector<string> treatment_field_names;

  /** @brief road segment file */
  string road_segment_file;

  /** @brief road segment NA */
  int road_segment_na;

  /** @brief field names to be read */
  vector<string> road_segment_field_names;

  /** @brief district vector */
  vector<rwx::district_t> districts;
  
  /** @brief error error string */
  string error;

private:
  config_reader & operator=(const config_reader &cfg_reader);
  config_reader(config_reader  &cfg_reader);

};


#endif /* CONFIG_READER_HH */
