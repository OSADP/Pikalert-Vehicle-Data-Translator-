//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file main.cc
 *
 * Main program for cdf_to_csv
 *
 */

// Include files
#include <log/log.hh>
#include <string.h>
#include <vector>
#include "arguments.hh"
#include "process.hh"

using std::string;
using std::vector;


// Global variables
int Debug_level = 0;
Log *Logg;

// Functions
int main(int argc, char **argv)
{
  // Get command line arguments and store in args
  arguments args(argc, argv);
  if (args.error != string(""))
    {
      fprintf(stderr, "Error: command line arguments problem: %s\n", args.error.c_str());
      return 2;
    }

  // Set up logging global Logg
  Logg = new Log(args.log_file.c_str());

  Logg->write_time_starting(args.program_name.c_str());
  Logg->write_time("Info: executed: %s\n", args.command_string.c_str());

  if (args.error != "")
    {
      Logg->write_time("Error: %s\n", args.error.c_str());
      Logg->write_time_ending(args.program_name.c_str(), 1);
      return 1;
    }

  // Initialize processing object
  process process(args);

  if (process.error != string(""))
    {
      Logg->write_time("Error: process initialization failed, %s\n", process.error.c_str());
      Logg->write_time_ending(args.program_name.c_str(), 1);
      return 1;
    }

  // Run processing
  if (process.run() < 0)
    {
      Logg->write_time("Error: processing failed\n");
      Logg->write_time_ending(args.program_name.c_str(), 1);
      return 1;
    }

  Logg->write_time_ending(args.program_name.c_str(), 0);

  return 0;
}
