//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 18:32:17 $
//
//==============================================================================

/**
 *
 *  @file apps/probe_message_dataset_manager/arguments.hh
 *
 *
 *  @class arguments
 *
 *   Simple class for parsing command line arguments.
 *
 * @date 02/24/2012
 */

#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * Arguments class
 */
class arguments
{
public:
  /** Constructor */
  arguments(int argc, char **argv);

  string program_name;		               //< program name
  string command_string;                       //< command invocation string
  string log_file;		               //< file for log output
  string error;			               //< error message
  int debug;		   	               //< perform debugging at specified debug level
  string cfg_file;                             //< configuration file
  vector<string> probe_message_file_list;                   //< input file with probe message data
  string out_file;
  unsigned int time_delta;		//< defaults 3600 seconds for one hour but up to user
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */
