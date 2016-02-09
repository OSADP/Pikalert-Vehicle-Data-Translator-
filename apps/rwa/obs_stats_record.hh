//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: obs_stats_record.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file obs_stats_record.hh
 *
 * @class obs_stats_record
 *
 *  <file/class description>
 *  
 * @date 10/16/13
 */

#ifndef OBS_STATS_RECORD_HH
#define OBS_STATS_RECORD_HH

#include "rwx/rwx_vector_collection_nc.hh"
#include "site_alert.hh"

#include <string>
#include <vector>
#include <unordered_map>

class obs_stats_info
{
public:
  time_t time;
  float model_air_temp;         // in fahrenheit
  float model_bar_press;	// in mb
  float nss_air_temp_mean;	// in fahreheit
  float nss_bar_press_mean;	// in mb
  float radar_ref;		// in dbZ
};

/**
 * @class obs_stats_record
 */
class obs_stats_record
{
public:
  static const std::string id_string;
  static const std::string begin_time_string;
  static const std::string end_time_string;
  static const std::string model_air_temp_string;
  static const std::string model_bar_press_string;
  static const std::string nss_air_temp_mean_string;
  static const std::string nss_bar_press_mean_string;
  static const std::string radar_ref_string;

  int site_num;
  vector<obs_stats_info> info_vec;
};

// create map mapping obs_stats site ids to obs_stats information
int create_obs_stats_record_map(int road_segment_na, const rwx_vector_collection_nc *obs_stats_collection, std::unordered_map<int, obs_stats_record> &obs_stats_record_map, string &error);

// incorporate obs stats info in site alerts
void incorporate_obs_stats(int site_offset, const std::unordered_map<int, obs_stats_record> &obs_stats_record_map, vector<site_alert_class> &site_alerts);

#endif /* OBS_STATS_RECORD_HH */
