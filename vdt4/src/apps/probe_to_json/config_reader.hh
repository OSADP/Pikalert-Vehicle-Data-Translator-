//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: config_reader.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2013/11/26 17:40:14 $
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

  /** @brief vehicle id name */
  string vehicle_id_name;

  /** @brief time field name */
  string time_field_name;

  /** @brief input_file_path input file path to config file */
  string input_file_path;

  /** @brief field names to be read */
  vector<string> field_names;

  /** @brief district vector */
  vector<rwx::district_t> districts;
  
  struct input_output_field_t
  {
    string input_name;
    string output_name;
    string precision;
    string input_units;
    string output_units;
  };

  /** @brief association of input field names to output name and precision */
  unordered_map<string, input_output_field_t> input_output_map;
  
  /** @brief association of input field names to their types: char, short, int, float, double and indices in the appropriate field name list mentioned above */
  //  unordered_map<string, pair<int, int> > input_type_index_map;
  
  /** @brief error error string */
  string error;

private:
  config_reader & operator=(const config_reader &cfg_reader);
  config_reader(config_reader  &cfg_reader);

};


#endif /* CONFIG_READER_HH */
