//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: alert_conditions.cc,v $
//       Version: $Revision: 1.20 $  Dated: $Date: 2014/03/31 19:03:35 $
//
//==============================================================================

/**
 * @file alert_conditions.cc
 *
 * <file description>
 *
 * @date 10/17/13
 */

// Include files 
#include <math.h>
#include "alert_conditions.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

const double MACH_EPS = 1E-9;

std::unordered_set<int> alert_conditions_class::op_set; // legitimate operations
std::unordered_set<std::string> alert_conditions_class::name_set; // legitimate names
std::unordered_set<std::string> alert_conditions_class::class_set; // legitimate alert classes

const std::string alert_conditions_class::precip_class_string = "precip";
const std::string alert_conditions_class::pavement_class_string = "pavement";
const std::string alert_conditions_class::visibility_class_string = "visibility";

void get_max_alert_codes(time_t obs_time, const std::vector<wx_time_alert> &alert_time_series, vector<rdwx_alert_codes> &alert_codes)
{
  // Get 0, 0-6, 6-24 and 24-72 max alert codes

  if (alert_time_series.size() == 0)
    {
      return;
    }

  alert_codes.resize(4, AC_NA);

  for (size_t i=0; i<alert_time_series.size(); i++)
    {
      if (fabs(alert_time_series[i].time - obs_time) < 1800)
	{
	  if (alert_codes[HR00] < alert_time_series[i].rdwx_alert_code)
	    {
	      alert_codes[HR00] = alert_time_series[i].rdwx_alert_code;
	    }
	}
      if (obs_time < alert_time_series[i].time && alert_time_series[i].time < obs_time + 6 * 3600)
	{
	  if (alert_codes[HR06] < alert_time_series[i].rdwx_alert_code)
	    {
	      alert_codes[HR06] = alert_time_series[i].rdwx_alert_code;
	    }
	}
      else if (obs_time + 6 * 3600 < alert_time_series[i].time && alert_time_series[i].time <= obs_time + 24 * 3600)
	{
	  if (alert_codes[HR24] < alert_time_series[i].rdwx_alert_code)
	    {
	      alert_codes[HR24] = alert_time_series[i].rdwx_alert_code;
	    }
	}
      else if (obs_time + 24 * 3600 < alert_time_series[i].time && alert_time_series[i].time <= obs_time + 72 * 3600)
	{
	  if (alert_codes[HR72] < alert_time_series[i].rdwx_alert_code)
	    {
	      alert_codes[HR72] = alert_time_series[i].rdwx_alert_code;
	    }
	}
    }
}

wx_time_alert::wx_time_alert()
{
  time = 0;
  rdwx_alert_code = AC_NA;
  treatment_alert_code = AC_NA;
  pavement = "";
  precip = "";
  visibility = "";
  maw_precip_action = "";
  maw_precip_condition = "";
  maw_pavement_action = "";
  maw_pavement_condition = "";
  maw_visibility_action = "";
  maw_visibility_condition = "";
}

op_name_value_class::op_name_value_class(const std::string &operation_arg, const std::string &name_arg, double value_arg)
{
  error = "";
  if (operation_arg == "EQ")
    {
      op = EQ;
    }
  else if (operation_arg == "NEQ")
    {
      op = NEQ;
    }
  else if (operation_arg == "GT")
    {
      op = NEQ;
    }
  else if (operation_arg == "GE")
    {
      op = NEQ;
    }
  else if (operation_arg == "LT")
    {
      op = NEQ;
    }
  else if (operation_arg == "LE")
    {
      op = NEQ;
    }
  else
    {
      error = "invalid operation string: " + operation_arg;
    }
  name = name_arg;

  if (name_arg == "precip_type")
    {
      value_type = rwh_info::RI_PRECIP_TYPE;
    }
  else if (name_arg == "precip_type_confidence")
    {
      value_type = rwh_info::RI_PRECIP_TYPE_CONFIDENCE;
    }
  else if (name_arg == "precip_intensity")
    {
      value_type = rwh_info::RI_PRECIP_INTENSITY;
    }
  else if (name_arg == "precip_intensity_confidence")
    {
      value_type = rwh_info::RI_PRECIP_INTENSITY_CONFIDENCE;
    }
  else if (name_arg == "pavement_condition")
    {
      value_type = rwh_info::RI_PAVEMENT_CONDITION;
    }
  else if (name_arg == "pavement_condition_confidence")
    {
      value_type = rwh_info::RI_PAVEMENT_CONDITION_CONFIDENCE;
    }
  else if (name_arg == "visibility")
    {
      value_type = rwh_info::RI_VISIBILITY;
    }
  else if (name_arg == "visibility_confidence")
    {
      value_type = rwh_info::RI_VISIBILITY_CONFIDENCE;
    }
  else if (name_arg == "pavement_slickness")
    {
      value_type = rwh_info::RI_PAVEMENT_SLICKNESS;
    }
  else
    {
      error = "invalid name string: " + name_arg;
    }

  value = value_arg;
}

alert_conditions_class::alert_conditions_class()
{
  if (alert_conditions_class::op_set.size() == 0)
    {
      alert_conditions_class::op_set.insert(op_name_value_class::EQ);
      alert_conditions_class::op_set.insert(op_name_value_class::NEQ);
      alert_conditions_class::op_set.insert(op_name_value_class::GT);
      alert_conditions_class::op_set.insert(op_name_value_class::GE);
      alert_conditions_class::op_set.insert(op_name_value_class::LT);
      alert_conditions_class::op_set.insert(op_name_value_class::LE);
    }
  if (alert_conditions_class::name_set.size() == 0)
    {
      alert_conditions_class::name_set.insert("precip_type");
      alert_conditions_class::name_set.insert("precip_type_confidence");
      alert_conditions_class::name_set.insert("precip_intensity");
      alert_conditions_class::name_set.insert("precip_intensity_confidence");
      alert_conditions_class::name_set.insert("pavement_condition");
      alert_conditions_class::name_set.insert("pavement_condition_confidence");
      alert_conditions_class::name_set.insert("visibility");
      alert_conditions_class::name_set.insert("visibility_confidence");
      alert_conditions_class::name_set.insert("pavement_slickness");
    }
  if (alert_conditions_class::class_set.size() == 0)
    {
      alert_conditions_class::class_set.insert("precip");
      alert_conditions_class::class_set.insert("pavement");
      alert_conditions_class::class_set.insert("visibility");
    }
}

alert_conditions_class::alert_conditions_class(const std::vector<std::vector<op_name_value_class> > &conditions_arg, const std::vector<alert_class_type> &class_type_arg) : class_type(class_type_arg), conditions(conditions_arg)
{
  // initialize op_set, name_set and class_set
  alert_conditions_class();

  // establish obs_condition_indices and fcst_condition_indices
  for (size_t i=0; i<class_type.size(); i++)
    {
      if (class_type[i].alert_time_class == ATC_OBS)
	{
	  obs_condition_indices.push_back(i);
	}
      else if (class_type[i].alert_time_class == ATC_FCST)
	{
	  fcst_condition_indices.push_back(i);
	}
      else if (class_type[i].alert_time_class == ATC_BOTH)
	{
	  obs_condition_indices.push_back(i);
	  fcst_condition_indices.push_back(i);
	}
    }
}

bool check_alert_conditions(std::vector<std::vector<op_name_value_class> > alert_conditions, std::string &error)
{
  for (size_t i=0; i<alert_conditions.size(); i++)
    {
      for (size_t j=0; j<alert_conditions[i].size(); j++)
	{
	  if (alert_conditions_class::op_set.count(alert_conditions[i][j].op) == 0)
	    {
	      error = "illegitimate op: " + alert_conditions[i][j].op;
	      return false;
	    }

	  if (alert_conditions_class::name_set.count(alert_conditions[i][j].name) == 0)
	    {
	      error = "illegitimate name: " + alert_conditions[i][j].name;
	      return false;
	    }
	}
    }

  return true;
}

bool check_alert_class(std::vector<alert_class_type> class_type, std::string &error)
{
  for (size_t i=0; i<class_type.size(); i++)
    {
      if (alert_conditions_class::class_set.count(class_type[i].alert_class) == 0)
	    {
	      error = "illegitimate alert class: " + class_type[i].alert_class;
	      return false;
	    }
    }

  return true;
}

bool alert_conditions_class::check_all_alert_conditions(std::string &error)
{
  std::string error_str = "";
  error = "";

  if (check_alert_conditions(conditions, error_str) == false)
    {
      error = "illegitimate conditions: " + error_str;
      return false;
    }
  if (check_alert_class(class_type, error_str) == false)
    {
      error = "illegitimate class_type: " + error_str;
      return false;
    }

  return true;
}

int apply_op_name_value(const op_name_value_class &op_name_value, const rwh_info &info)
{
  // Assess whether the rwh_info object satisfies the given individual alert condition in op_name_value
  double value;
  int ret = info.get_value(op_name_value.value_type, &value);
  //printf("applying op name value: %d %s %lf to value %lf\n", op_name_value.op, op_name_value.name.c_str(), op_name_value.value, value);
  if (ret == -1)
    {
      return -1;
    }
  else
    {
      switch (op_name_value.op)
	{
	case op_name_value_class::EQ:
	  if (fabs(op_name_value.value - value) < MACH_EPS)
	    {
	      return 1;
	    }
	  break;
	case op_name_value_class::NEQ:
	  return op_name_value.value != value;
	  break;
	case op_name_value_class::GT:
	  return op_name_value.value > value;
	  break;
	case op_name_value_class::GE:
	  return op_name_value.value >= value;
	  break;
	case op_name_value_class::LT:
	  return op_name_value.value < value;
	  break;
	case op_name_value_class::LE:
	  return op_name_value.value <= value;
	  break;
	}
    }

  return -1;
}

int apply_and_conditions(const std::vector<op_name_value_class> alert_and_conditions, const alert_class_type &class_type, const rwh_info &info, wx_time_alert &time_alert)
{
  // Assess whether the rwh_info object satisfies the given set of alert_and_conditions
  //printf("info time nominal: %lf %lf\n", info.time_nominal, time_alert.time);

  for (size_t i=0; i<alert_and_conditions.size(); i++)
    {
      const op_name_value_class &op_name_value = alert_and_conditions[i];
      int ret = apply_op_name_value(op_name_value, info);
      if (ret < 0)
	{
	  return -1;
	}
      else if (ret == 0)
	{
	  return 0;
	}
    }

  // And conditions have been satisfied so set time_alert
  // printf("precip type %d, precip intensity %d, pavement_condition %d, visibility %d, class alert code %d, class %s\n", info.precip_type, info.precip_intensity, info.pavement_condition, info.visibility, class_type.alert_code, class_type.alert_class.c_str());
  time_alert.time = info.time_nominal;
  if (time_alert.rdwx_alert_code < class_type.alert_code)
    {
      time_alert.rdwx_alert_code = class_type.alert_code;
      //printf("final alert code: %d\n", time_alert.alert_code);
    }

  if (class_type.alert_class == alert_conditions_class::precip_class_string)
    {
      time_alert.precip = class_type.alert_type;
      time_alert.maw_precip_condition = class_type.maw_condition;
      time_alert.maw_precip_condition_code = class_type.maw_condition_code;
      time_alert.maw_precip_action = class_type.maw_action;
      time_alert.maw_precip_action_code = class_type.maw_action_code;
      time_alert.maw_precip_priority = class_type.maw_priority;
      //printf("maw precip_priority = %d\n", time_alert.maw_precip_priority);
      //printf("final precip: %s\n", time_alert.precip.c_str());
    }
  else if (class_type.alert_class == alert_conditions_class::pavement_class_string)
    {
      time_alert.pavement = class_type.alert_type;
      time_alert.maw_pavement_condition = class_type.maw_condition;
      time_alert.maw_pavement_condition_code = class_type.maw_condition_code;
      time_alert.maw_pavement_action = class_type.maw_action;
      time_alert.maw_pavement_action_code = class_type.maw_action_code;
      time_alert.maw_pavement_priority = class_type.maw_priority;
      //printf("final pavement: %s\n", time_alert.pavement.c_str());
    }
  else if (class_type.alert_class == alert_conditions_class::visibility_class_string)
    {
      time_alert.visibility = class_type.alert_type;
      time_alert.maw_visibility_condition = class_type.maw_condition;
      time_alert.maw_visibility_condition_code = class_type.maw_condition_code;
      time_alert.maw_visibility_action = class_type.maw_action;
      time_alert.maw_visibility_action_code = class_type.maw_action_code;
      time_alert.maw_visibility_priority = class_type.maw_priority;
      //printf("final visibility: %s\n", time_alert.visibility.c_str());
    }

  return 1;
}

void alert_conditions_class::create_rwh_info_alerts(double obs_time, const rwh_info &info, wx_time_alert &time_alert) const
{
  // Iterate through or conditions to determine overall alert assessment for rwh info object
  // Note that the overall assessment is stored in time_alert
  time_alert.rdwx_alert_code = AC_NA;
  time_alert.time = info.time_nominal;

  time_alert.maw_pavement_priority = 0;
  time_alert.maw_pavement_action_code = 0;
  time_alert.maw_pavement_condition = "";
  time_alert.maw_pavement_action = "";

  time_alert.maw_precip_priority = 0;
  time_alert.maw_precip_condition_code = 0;
  time_alert.maw_precip_action_code = 0;
  time_alert.maw_precip_condition = "";
  time_alert.maw_precip_action = "";

  time_alert.maw_visibility_priority = 0;
  time_alert.maw_visibility_action_code = 0;



  time_alert.maw_visibility_condition = "";
  time_alert.maw_visibility_action = "";

  //printf("road segment id %d\n", info.road_segment_id);
  if (obs_time == info.time_nominal)
    {
      //printf("obs time is nominal, obs condition size %ld\n", obs_condition_indices.size());
      // use only obs conditions
      for (size_t i=0; i<obs_condition_indices.size(); i++)
	{
	  apply_and_conditions(conditions[obs_condition_indices[i]], class_type[obs_condition_indices[i]], info, time_alert);
	}
    }
  else
    {
      //printf("forecast time, fcst_condition size %ld\n", fcst_condition_indices.size());
      for (size_t i=0; i<fcst_condition_indices.size(); i++)
	{
	  apply_and_conditions(conditions[fcst_condition_indices[i]], class_type[fcst_condition_indices[i]], info, time_alert);
	}
    }
}

void alert_conditions_class::create_rwh_record_alerts(const rwh_record &record, std::vector<wx_time_alert> &alert_time_series) const
{
  if (record.info_vec.size() == 0)
    {
      return;
    }

  double obs_time = record.info_vec[0].time_nominal;

  alert_time_series.resize(record.info_vec.size());

  //printf("info_vec size %ld\n", record.info_vec.size());
  for (size_t i=0; i<record.info_vec.size(); i++)
    {
      create_rwh_info_alerts(obs_time, record.info_vec[i], alert_time_series[i]);
    }
}

