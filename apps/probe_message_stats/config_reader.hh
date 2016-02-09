//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: config_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 18:32:17 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/config_reader.hh
 *
 * @class config_reader
 *
 *  Configuration file reader class
 *
 * @date 10-20-2010
 *
 */

#ifndef CONFIG_READER_HH
#define CONFIG_READER_HH

#include <string>
#include <vector>

using std::string;
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

  /** @brief field_names field names to create statistics for */
  vector<string> field_names;

  /** @brief error error string */
  string error;

private:
  config_reader & operator=(const config_reader &cfg_reader);
  config_reader(config_reader  &cfg_reader);

};


#endif /* CONFIG_READER_HH */
