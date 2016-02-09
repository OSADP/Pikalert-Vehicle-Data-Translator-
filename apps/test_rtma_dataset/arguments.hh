//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/10/24 20:30:41 $
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

  /** @brief input file */
  string input_file;                           

  /** @brief input file */
  string output_file;                           

  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */
