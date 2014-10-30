//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: create_site_alerts.cc,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013/10/18 23:35:41 $
//
//==============================================================================

/**
 * @file create_site_alerts.cc
 *
 * <file description>
 *
 * @date 10/17/13
 */

// Include files 
#include "create_site_alerts.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions



void create_site_alerts(double obs_time, const alert_conditions_class &alert_conditions, const unordered_map<int, rwh_record> &rwh_record_map, vector<site_alert_class> &site_alerts)
{
  site_alerts.resize(rwh_record_map.size());

  // Iterate through each object in rwh_record_map determining its alert conditions
  size_t ct = 0;
  for (auto itr=rwh_record_map.begin(); itr!=rwh_record_map.end(); ++itr)
    {
      const vector<rwh_info> &info_vec = itr->second.info_vec;

      // for each rwh_info_t determine associated alert and alert string


      // check whether time of initial info object is observation time or forecast time
      //if (time_itr->first == obs_time)
    }

  // Apply alert conditions to each site

  // Create alerts for each site
}
