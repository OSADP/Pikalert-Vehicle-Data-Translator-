//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: config_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/11/18 22:07:15 $
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

  /** @brief input_file_path input file path to config file */
  string input_file_path;

  /** @brief char field names to be read */
  vector<string> char_field_names;

  /** @brief short field names to be read */
  vector<string> short_field_names;

  /** @brief int field names to be read */
  vector<string> int_field_names;

  /** @brief float field names to be read */
  vector<string> float_field_names;

  /** @brief double field names to be read */
  vector<string> double_field_names;

  /** @brief association of input field names to output csv names */
  vector<std::pair<string, string> > input_output_csv;
  
  /** @brief association of input field names to their types: char, short, int, float, double and indices in the appropriate field name list mentioned above */
  unordered_map<string, std::pair<int, int> > input_type_index_map;
  
  /** @brief error error string */
  string error;

private:
  config_reader & operator=(const config_reader &cfg_reader);
  config_reader(config_reader  &cfg_reader);

};


#endif /* CONFIG_READER_HH */
