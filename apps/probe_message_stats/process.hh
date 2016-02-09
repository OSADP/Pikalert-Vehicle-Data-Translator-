//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/process.hh
 *
 * @class process
 *
 * The purpose of this modules is to perform quality control checks
 */

#ifndef PROCESS_HH
#define PROCESS_HH

// Include files
#include <string>
#include <vector>
#include "arguments.hh"
#include "config_reader.hh"

using std::string;
using std::vector;

/**
 * @class process
 */
class process
{
public:
  /**
   *
   * Constructor
   *
   * @brief extract information from arguments and configuration
   * objects to prepare for processing
   *
   * @param[in] args_param command line arguments
   */
  process(const arguments &args_param) : args(args_param)
  {
    error = string("");
  };

  const arguments &args;

  /**
   * Destructor
   */
  ~process();

  /**
   * @brief run do processing
   */
  int run();

  /**
   * @brief construction error description
   */
  string error;
};


#endif /* PROCESS_HH */
