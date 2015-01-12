//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/02/21 18:07:04 $
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

  /** @brief flag for adding road segments having no probe messages to road seg stats vector */
  bool add_non_associated_segments;

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

  /** @brief input file with metar data */
  string metar_file;                           

  /** @brief probe_message_qc_output if set to true, write output else do not write  */
  bool probe_message_qc_output;

  /** @brief output file for grid cell statistics data */
  string grid_cell_statistics_out_file;        

  /** @brief input file with historic probe message data */
  string history_probe_message_file;           

  /** @brief input file pattern for files with radar data */
  string radar_file_pattern;                   

  /** @brief input file with rwis data */
  string rwis_file;                            

  /** @brief input file with cloud classification data */
  string cloud_class_file;                     

  /** @brief input file wiith rtma data */
  string rtma_file;                           

  /** @brief configuration file */
  string cfg_file;                             

  /** @brief input file with road segments data */
  string road_seg_file;                        

  /** @brief input file with probe message data */
  string probe_message_file;                   

  /** @brief output file for probe message qc data */
  string probe_message_qc_out_file;            

  /** @brief output file for probe message qc statistics data */
  string probe_message_qc_statistics_out_file; 

  /** @brief valid time of data in unix time format */
  double validtime;                            

  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
};

#endif /* ARGUMENTS_HH */
