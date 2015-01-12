//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/09/27 16:47:01 $
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

  /** @brief configuration file for running the road weather hazzard applicaton */
  string cfg_file;                             
  
  /** @brief ascii file with road segment site list entries for retrieving forecast data */
  string fcst_road_seg_site_list_file;                        
  
  /** @brief netcdf file with road segments data */
  string vdt_road_seg_file;                        
  
  /** @brief output file for road weather hazard data */
  string output_file;     
  
  /** @brief input file for road segment weather forecast data */
  string road_wx_fcst_file;
  
  /** @brief input file for road segment conditions forecast data based */
  string road_cond_fcst_file;
  
  /** @brief input file for VDT Stage II Segment Statistics data */
  string vdt_seg_stats_file;            
  
  /** @brief input file for VDT Stage II Road-Weather Assignment data */
  string vdt_seg_assign_wx_file;            
  
  /** @brief usage 
   * provides usage information
   * @param[in] prog_name program name
   */
  void usage(char *prog_name);
    
  /** @brief datetime_2_utime 
   * convert date-time string to unix time
   * @param[in] datetime
   */
  time_t datetime_2_utime(char *datetime);
};

#endif /* ARGUMENTS_HH */
