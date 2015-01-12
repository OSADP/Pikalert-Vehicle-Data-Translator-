//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: site_alert.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/02/13 22:08:31 $
//
//==============================================================================

/**
 * @file site_alert.cc
 *
 * <file description>
 *
 * @date 10/20/13
 */

// Include files 
#include <math.h>
#include "alert_conditions.hh"
#include "site_alert.hh"

// Constants and macros
const int THIRTY_MIN = 1800;

// Types

// Structures, unions and classes

// Global variables 

// Functions

site_alert_class::site_alert_class()
{
  obs_alert_code = AC_NA;
  hour06_alert_code = AC_NA;
  hour24_alert_code = AC_NA;
  hour72_alert_code = AC_NA;
  lat = 0;
  lon = 0;
  is_road_cond_site = false;
  is_rwis_site = false;
  is_wx_obs_site = false;
  site_id = "";
  site_num = -1;
}

// Generate summary alert codes for each site based on its time series
void site_alert_class::set_time_alert_codes(time_t obs_time)
{
  //printf("alert_time_series.size() %ld\n", alert_time_series.size());
  for (size_t i=0; i<alert_time_series.size(); i++)
    {
      if (fabs(alert_time_series[i].time - obs_time) < 300)
	{
	  if (obs_alert_code < alert_time_series[i].rdwx_alert_code)
	    {
	      obs_alert_code = alert_time_series[i].rdwx_alert_code;
	    }
	}
      //printf("diff, obs_time, alert_time_series[i].time: %ld %ld %ld\n", alert_time_series[i].time - obs_time, obs_time, alert_time_series[i].time);
      if (alert_time_series[i].time <= obs_time + 6 * 3600)
	{
	  if (hour06_alert_code < alert_time_series[i].rdwx_alert_code)
	    {
	      hour06_alert_code = alert_time_series[i].rdwx_alert_code;
	    }
	}
      else if (obs_time + 6 * 3600 <= alert_time_series[i].time && alert_time_series[i].time <= obs_time + 24 * 3600)
	{
	  //printf("resetting 24\n");
	  if (hour24_alert_code < alert_time_series[i].rdwx_alert_code)
	    {
	      hour24_alert_code = alert_time_series[i].rdwx_alert_code;
	    }
	}
      else if (obs_time + 24 * 3600 <= alert_time_series[i].time && alert_time_series[i].time <= obs_time + 72 * 3600)
	{
	  //printf("resetting 72\n");
	  if (hour72_alert_code < alert_time_series[i].rdwx_alert_code)
	    {
	      hour72_alert_code = alert_time_series[i].rdwx_alert_code;
	    }
	}

    }
}


// Get summary alert codes for all sites
void site_alert_class::get_max_summary_codes(const vector<site_alert_class> &site_alerts, vector<rdwx_alert_codes> &alert_codes)
{
  if (alert_codes.size() == 0)
    {
      alert_codes.resize(4, AC_NA);
    }

  for (size_t i=0; i<site_alerts.size(); i++)
    {
      if (alert_codes[HR00] < site_alerts[i].obs_alert_code)
	{
	  alert_codes[HR00] = site_alerts[i].obs_alert_code;
	}
      if (alert_codes[HR06] < site_alerts[i].hour06_alert_code)
	{
	  alert_codes[HR06] = site_alerts[i].hour06_alert_code;
	}
      if (alert_codes[HR24] < site_alerts[i].hour24_alert_code)
	{
	  alert_codes[HR24] = site_alerts[i].hour24_alert_code;
	}
      if (alert_codes[HR72] < site_alerts[i].hour72_alert_code)
	{
	  alert_codes[HR72] = site_alerts[i].hour72_alert_code;
	}
    }
}
