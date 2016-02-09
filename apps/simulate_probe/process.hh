//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//==============================================================================

/**
 *
 * @file process.hh
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
#include "vdt4/vdt_road_segment.hh"

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

  /** @brief arguments object */
  const arguments &args;

  /**
   * Destructor
   */
  ~process();

  int do_simulation(unsigned int ct, time_t current_time, const config_reader &cfg_reader, const vector<vector<string> > &file_lists, const vector<string> &field_names, const vector<string> &field_units, int wiper_status_field_number, const vector<float> &deviations, const vector<vdt_road_segment> &input_segments);

  /**
   * @brief run do processing
   * Processing steps:
   *  Read vdt configuration file
   *  Read current vehicle probe messages into container
   *  Read road segments file and meteorological datasets consisting of 
   *   radar, metar, rwis, rtma, and cloud class files
   *  Call the update_probe_messages() method in vdt_probe_message_datasets
   *  Configure and construct qc_manager object. This object is responsible
   *   for performing standard probe message qc checks
   *  Perform persistence and step checks requiring historical probe
   *   message data and suitable vehicle identifiers
   *  Create road segment statistics
   *  Create grid cell statistics
   *  Output probe messages with associated qc information
   *  Output road segment statistics
   *  Perform road segment assessment processing
   */
  int run();

  /**
   * @brief construction error description
   */
  string error;
};


#endif /* PROCESS_HH */
