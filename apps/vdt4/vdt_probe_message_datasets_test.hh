//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

/**
 *
 * @file vdt_probe_message_datasets_test.hh
 *
 * @class vdt_probe_message_datasets
 *
 * Class to handle met data
 *
 */

#ifndef VDT_PROBE_MESSAGE_DATASETS_TEST_H
#define VDT_PROBE_MESSAGE_DATASETS_TEST_H

#include <iostream>
#include "vdt4/vdt_surface_observation.hh"
#include "vdt_probe_message_datasets.hh"
#include <string>

using namespace std;
using namespace libconfig;

//ED needs Doxygen documentation


/**
 * @class vdt_probe_message_datasets_test
 * tests vdt probe message datasets
 */

class vdt_probe_message_datasets_test : public vdt_probe_message_datasets
{
public:

  /**
   * @brief  Gets the nearest surface station observations within the NSS_SEARCH_RANGE and NSS_TIME_RANGE
   *         for a given lat, lon, and obstime
   *
   * @param[in] target  point containing lat, lon and height
   * @param[in] obstime  observation time
   * @param[in] search_range_km  spatial surface station search range in kilometers
   * @param[in] time_range_secs  spatial surface station time range in seconds
   * @param[out] near  vector of vdt_surface_observation objects
   */
  void get_near_surface_obs(const vdt_point &target, double obstime, float search_range_km, int time_range_secs, vector<vdt_surface_observation> &near);

  void update_probe_messages(const config_reader& cfg, const vector<vdt_probe_message>& in_msgs, vector<vdt_probe_message_qc> &out_msgs);

};

#endif
