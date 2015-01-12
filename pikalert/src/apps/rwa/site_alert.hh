//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: site_alert.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/02/04 02:21:25 $
//
//==============================================================================

/**
 * @file site.hh
 *
 * @class Site
 *
 *  <file/class description>
 *  
 * @date 10/16/13
 */

#ifndef SITE_ALERT_HH
#define SITE_ALERT_HH

#include <string>
#include <vector>

#include "alert_conditions.hh"

/**
 * @class site_alert
 */
class site_alert_class
{
public:
  site_alert_class();			// constructor

  /** @brief description */
  std::string description;
  bool is_road_cond_site;
  bool is_rwis_site;
  bool is_wx_obs_site;
  float lat;
  float lon;
  std::string site_id;
  int site_num;
  int road_segment_number;
  rdwx_alert_codes obs_alert_code;
  rdwx_alert_codes hour06_alert_code;
  rdwx_alert_codes hour24_alert_code;
  rdwx_alert_codes hour72_alert_code;
  std::vector<wx_time_alert> alert_time_series;

  void set_time_alert_codes(time_t begin_time);	// sets obs_alert_code, hour06_alert_code, hour24_alert_code, hour72_alert_code

  static void get_max_summary_codes(const vector<site_alert_class> &site_alerts, vector<rdwx_alert_codes> &alert_codes);
};

#endif /* SITE_ALERT_HH */
