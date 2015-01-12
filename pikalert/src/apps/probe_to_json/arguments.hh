//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/09/17 00:33:13 $
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

  /** @brief test application if true  */
  bool test;				       

  /** @brief date time string */
  string date_time_string;

  /** @brief input file */
  string input_file;                           

  /** @brief input file */
  string output_file;                           

  /** @brief configuration file */
  string cfg_file;                             

  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */