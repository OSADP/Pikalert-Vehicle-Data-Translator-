//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/05/13 20:42:40 $
//
//==============================================================================

/**
 *
 *  @file arguments.hh
 *
 *
 *  @class arguments
 *
 *   Simple class for parsing command line arguments.
 *
 */

#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * arguments class
 * handles the arguments of probe_message_dataset_manager
 */
class arguments
{
public:
  /** constructor */
  arguments(int argc, char **argv);

  /** @brief program name */
  string program_name;		               

  /** @brief command invocation string */
  string command_string;                       

  /** @brief file for log output */
  string log_file;		               

  /** @brief error message */
  string error;			               

  /** @brief date time string */
  string date_time_string;

  /** @brief rwh input file */
  string rwh_input_file;                           

  /** @brief obs statistics input file */
  string obs_stats_input_file;                           

  /** @brief maw app output file */
  string maw_output_file;

  /** @brief forecast sites file is semicolon separated value format */
  string site_list_file;                           

  /** @brief treatment input file */
  string treatment_input_file;                           

  /** @brief alerts output file */
  string alerts_output_file;                           

  /** @brief plots output file */
  string plots_output_file;                           

  /** @brief obs stats output file */
  string obs_stats_output_file;                           

  /** @brief configuration file */
  string cfg_file;                             

  /** @brief test application if true  */
  bool test;				       

  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */
