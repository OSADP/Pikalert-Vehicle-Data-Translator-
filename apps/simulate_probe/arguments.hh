//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014-02-21 18:07:04 $
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
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
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

  /** @brief flag for adding road segments having no probe messages to road seg stats vector */
  bool add_non_associated_segments;

  /** @brief flag for debugging simulation output for segments */
  bool debug_segments;

  /** @brief car_density {0 - less dense, 1 - more dense} */
  enum Car_density {LESS_DENSE=0, MORE_DENSE=1};
  int car_density;

  /** @brief program name */
  string program_name;		               

  /** @brief begin time string in form yyyymmddhhmm of time window of interest */
  string begin_time_string;		       

  /** @brief begin time of time window of interest */
  time_t begin_time;			       

  /** @brief command invocation string */
  string command_string;                       

  /** @brief file for log output */
  string log_file;		               

  /** @brief end time in form yyyymmddhhmm of time window of interest */
  string end_time_string;		       

  /** @brief end time of time window of interest */
  time_t end_time;			       

  /** @brief error message */
  string error;			               

  /** @brief perform debugging at specified debug level */
  int debug;		   	               

  /** @brief test application if true  */
  bool test;				       

  /** @brief time delta */
  unsigned int time_delta;

  /** @brief file list names */
  vector<string> file_list_names;

  /** @brief file list position */
  unordered_map<string, size_t> file_list_position;

  /** @brief configuration file name */
  string cfg_file;                             

  /** @brief probe message output file base */
  string probe_message_out_file_base;

  /** @brief csv output file base name */
  string csv_out_file_base;

  /** @brief dicast csv output file base name */
  string dicast_csv_out_file_base;

  /** @brief do_wipers_only only output wipers information in specialized CSV format. This makes use of dicast_csv_out_file_base. */
  bool do_wipers_only;

  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */
