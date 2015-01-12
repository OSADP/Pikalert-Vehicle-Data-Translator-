//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: alert_conditions.hh,v $
//       Version: $Revision: 1.20 $  Dated: $Date: 2014/03/31 19:03:35 $
//
//==============================================================================

/**
 * @file alert_conditions.hh
 *
 * @class Alert_Conditions
 *
 *  <file/class description>
 *  
 * @date 10/17/13
 */

#ifndef ALERT_CONDITIONS_HH
#define ALERT_CONDITIONS_HH

#include <string>
#include <unordered_set>
#include <vector>
#include "rwh_record.hh"


static const float MISSING = -9999;

static const string MISSING_STRING = "missing";
static const string CLEAR_STRING = "clear";
static const string ALERT_STRING = "alert";

enum rdwx_alert_codes {AC_NA=-1, AC_MISSING, AC_CLEAR, AC_WARNING, AC_ALERT};
enum alert_time_classes {ATC_OBS=0, ATC_FCST, ATC_BOTH};
enum alert_hour_offsets {HR00, HR06, HR24, HR72};

class wx_time_alert
{
public:
  wx_time_alert();

  time_t time;
  rdwx_alert_codes rdwx_alert_code;
  std::string pavement;
  std::string precip;
  std::string visibility;
  std::string maw_precip_action;
  int maw_precip_action_code;
  std::string maw_precip_condition;
  int maw_precip_condition_code;
  int maw_precip_priority;
  int maw_pavement_action_code;
  std::string maw_pavement_action;
  std::string maw_pavement_condition;
  int maw_pavement_condition_code;
  int maw_pavement_priority;
  std::string maw_visibility_action;
  int maw_visibility_action_code;
  std::string maw_visibility_condition;
  int maw_visibility_condition_code;
  int maw_visibility_priority;

  rdwx_alert_codes treatment_alert_code;
  std::string plow;
  std::string chemicals;
  float road_temperature;

  // obs info
  time_t obs_time;
  float model_air_temp;
  float model_bar_press;
  float nss_air_temp_mean;
  float nss_bar_press_mean;
  float radar_ref;
};

void get_max_alert_codes(const std::vector<wx_time_alert> &alert_time_series, vector<rdwx_alert_codes> &alert_codes);

/**
 *   precip_type:long_name = "derived precip type field" ;
 *   precip_type:_FillValue = -9999s ;
 *   precip_type:flag_values = 0, 1, 2, 3 ;
 *   precip_type:flag_meanings = "none rain mix snow" ;
 *   precip_type_confidence:long_name = "derived precip type confidence field (0-1)" ;
 *   precip_type_confidence:_FillValue = -9999.f ;
 *
 *   precip_intensity:long_name = "derived precip intensity field" ;
 *   precip_intensity:_FillValue = -9999s ;
 *   precip_intensity:flag_values = 0, 1, 2, 3, 4 ;
 *   precip_intensity:flag_meanings = "none light moderate heavy road_splash" ;
 *
 *   precip_intensity_confidence:long_name = "derived precip intensity confidence field (0-1)" ;
 *   precip_intensity_confidence:_FillValue = -9999.f ;
 *
 *   pavement_condition:long_name = "derived pavement condition field" ;
 *   pavement_condition:_FillValue = -9999s ;
 *   pavement_condition:flag_values = 0, 1, 2, 3, 4, 5, 6, 7 ;
 *   pavement_condition:flag_meanings = "dry wet snow_covered ice_covered hydroplane black_ice dry_wet dry_frozen" ;
 *
 *   pavement_condition_confidence:long_name = "derived pavement condition confidence field (0-1)" ;
 *   pavement_condition_confidence:_FillValue = -9999.f ;
 *
 *   visibility:long_name = "derived visibility field" ;
 *   visibility:_FillValue = -9999s ;
 *   visibility:flag_values = 0, 1, 2, 3, 4, 5, 5, 7, 8 ;
 *   visibility:flag_meanings = "normal low heavy_rain heavy_snow blowing_snow fog haze dust smoke" ;
 *
 *   visibility_confidence:long_name = "derived visibility confidence field (0-1)" ;
 *   visibility_confidence:_FillValue = -9999.f ;
 *
 *   pavement_slickness:long_name = "derived pavement slickness field" ;
 *   pavement_slickness:_FillValue = -9999s ;
 *   pavement_slickness:flag_values = 0, 1 ;
 *   pavement_slickness:flag_meanings = "normal slick" ;
 *
 **/


class op_name_value_class
{
public:
  op_name_value_class(const std::string &operation_arg, const std::string &name_arg, double value_arg);
  op_name_value_class() {};


  enum Operator {EQ, NEQ, GT, GE, LT, LE};
  Operator op;			        // one of GT, GE, EQ, NEQ, LT, LE
  std::string name;			// like "precip_type", "precip_intensity", etc.
  double value;			        // float flag value indicating the condition (see above information for potential values)
  rwh_info::rwh_info_value_types value_type;
  std::string error;
};

class alert_class_type
{
public:
  std::string alert_class;	// precip, pavement, visibility
  std::string alert_type;	// "light snow", "moderate snow", etc.
  std::string maw_condition; // maw phone app condition
  std::string maw_action; // maw phone app action
  int maw_action_code; // maw phone app action code
  int maw_priority;       // maw phone app priority
  int maw_condition_code;  // maw phone app condition code

  rdwx_alert_codes alert_code;	// missing, clear, warning, alert
  alert_time_classes alert_time_class; // obs, fcst, both_obs_fcst
};

/**
 * @class alert_conditions observation and forecast alert conditions. These are typically set using a configuration file.
 * Here are some examples:
 *  ("EQ", "precip_type", 3), ("EQ", "precip_intensity", 1)) # light snow condition
 *  ("EQ", "precip_type", 3), ("EQ", "precip_intensity", 2)) # moderate snow condition
 * The interior vector contains conditions that are AND'ED together
 * The external vector will OR the interior conditions together.
 */
class alert_conditions_class
{
public:
  alert_conditions_class(const std::vector<std::vector<op_name_value_class> > &conditions, const std::vector<alert_class_type> &class_type);
  static std::unordered_set<int> op_set; // legitimate operations
  static std::unordered_set<std::string> name_set; // legitimate names
  static std::unordered_set<std::string> class_set; // legitimate alert classes

  static const std::string precip_class_string;
  static const std::string pavement_class_string;
  static const std::string visibility_class_string;
  

  alert_conditions_class();

  /** @brief condition class type */
  std::vector<alert_class_type> class_type;

  /** @brief conditions */
  std::vector<std::vector<op_name_value_class> > conditions;

  /** @brief observation condition indices */
  std::vector<size_t> obs_condition_indices;

  /** @brief forecast condition indices */
  std::vector<size_t> fcst_condition_indices;

  void create_rwh_info_alerts(double obs_time, const rwh_info &info, wx_time_alert &time_alert) const;

  void create_rwh_record_alerts(const rwh_record &record, std::vector<wx_time_alert> &alert_time_series) const;


  bool check_all_alert_conditions(std::string &error);

};

#endif /* ALERT_CONDITIONS_HH */
