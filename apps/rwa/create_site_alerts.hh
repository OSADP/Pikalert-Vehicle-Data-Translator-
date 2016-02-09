//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: create_site_alerts.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013/10/18 23:35:41 $
//
//==============================================================================

/**
 * @file create_site_alerts.hh
 *
 * @class Create_Site_Alerts
 *
 *  <file/class description>
 *  
 * @date 10/17/13
 */

#ifndef CREATE_SITE_ALERTS_HH
#define CREATE_SITE_ALERTS_HH


#include "alert_conditions.hh"
#include "rwh_record.hh"
#include "site_alert.hh"


void create_site_alerts(double obs_time, const alert_conditions_class &alert_conditions, const unordered_map<int, rwh_record> &rwh_record_map, site_alert_class site_alerts);


#endif /* CREATE_SITE_ALERTS_HH */
