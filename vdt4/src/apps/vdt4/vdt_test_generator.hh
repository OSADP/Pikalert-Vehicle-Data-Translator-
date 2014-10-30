//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_test_generator.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/11 19:12:26 $
//
//==============================================================================

/**
 * @file vdt_test_generator.hh
 *
 * @class vdt_test_generator
 *
 */

#ifndef VDT_TEST_GENERATOR_HH
#define VDT_TEST_GENERATOR_HH


#include <string>
#include "config_reader.hh"
#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"

using std::string;

/**
 * @class vdt_test_generator
 */
class vdt_test_generator
{
public:
  /** @brief constructor
   *  @param[in] cfg_reader configuration reader containing test configuration information
   */
  vdt_test_generator(const config_reader &cfg_reader);

  /** @brief destructor */
  virtual ~vdt_test_generator();

  /** @brief get_road_segments generate test road segment input
   * param[out] road_segments vector for storing test road segments
   */
  void get_road_segments(vector<vdt_road_segment> &road_segments);

  /** @brief get_probe_messages generate test probe messages
   * param[out] messages vector for storing test input probe messages
   */
  void get_probe_messages(vector<vdt_probe_message> &messages);

  /** @brief get_history_probe_messages generate history test probe messages
   * param[out] messages vector for storing test input probe messages
   */
  void get_history_probe_messages(vector<vdt_probe_message_qc> &messages);

  static vdt_point aspen_point;
  static vdt_point boulder_point;
  static vdt_point denver_point;
  static vdt_point glenwood_point;
  static vdt_point east_boulder_point;
  static vdt_point estes_point;
  static vdt_point north_boulder_point;
  static vdt_point steamboat_point;

  static double aspen_lat;
  static double aspen_lon;
  static double aspen_elev;

  static double boulder_lat;
  static double boulder_lon;
  static double boulder_elev;
  
  static float boulder_barnes_temp;
  static float boulder_barnes_dew_temp;
  static float boulder_barnes_bar_press;

  static double denver_lat;
  static double denver_lon;
  static double denver_elev;
  
  static double east_boulder_lat;
  static double east_boulder_lon;
  static double east_boulder_elev;
  
  static double estes_lat;
  static double estes_lon;
  static double estes_elev;
  static float estes_dew_temp;
  
  static double glenwood_lat;
  static double glenwood_lon;
  static double glenwood_elev;
  static float glenwood_spatial_dew_temp;
  static float glenwood_spatial_temp;
  static float glenwood_spatial_bar_press;

  static double north_boulder_lat;
  static double north_boulder_lon;
  static double north_boulder_elev;
  
  static double steamboat_lat;
  static double steamboat_lon;
  static double steamboat_elev;
  
  static double obs_time;
  static double rec_time_past;
  static double rec_time_present;
  static time_t begin_time;
  static string begin_time_string;
  static time_t end_time;
  static string end_time_string;

private:
  const config_reader &cfg_reader;
  vdt_test_generator & operator=(const vdt_test_generator &vdt_test_generator);
  vdt_test_generator(const vdt_test_generator &vdt_test_generator);
};

#endif /* VDT_TEST_GENERATOR_HH */
