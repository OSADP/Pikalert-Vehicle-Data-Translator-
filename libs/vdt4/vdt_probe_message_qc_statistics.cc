//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_probe_message_qc_statistics.cc,v $
//       Version: $Revision: 1.16 $  Dated: $Date: 2014/05/27 20:12:22 $
//
//==============================================================================


#include <stat_tools.hh>

#include <unordered_set>
#include "vdt_probe_message_qc_statistics.hh"
#include "vdt_const.hh"
#include "vdt_spatial_check.hh"

/**
 * @file vdt_probe_message_qc_statistics.cc
 */

void vdt_probe_message_qc_statistics::init()
{
  setattr("id",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("begin_time",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("end_time",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("road_name","");
  setattr("model_air_temp",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("model_dew_temp",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("model_bar_pressure",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_ref",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_cref",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_dual_pol_hc",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_dual_pol_hr",static_cast<double>(vdt_const::FILL_VALUE));    
  setattr("radar_precip_flag",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("radar_precip_type",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("cloud_mask",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("nss_air_temp_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_surface_temp_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_bar_pressure_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_dew_temp_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_wind_dir_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_wind_speed_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_hourly_precip_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_prevail_vis_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("total_num_msg",static_cast<int>(0));
  setattr("num_msg_valid_heading",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("heading_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_hoz_accel_lat",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("hoz_accel_lat_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_hoz_accel_lon",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("hoz_accel_lon_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lon_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_yaw_rate",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("yaw_rate_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("yaw_rate_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_steering_angle",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("steering_angle_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_lights",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_off",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_hazard",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_parking",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_fog",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_drl",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_automatic_control",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_right_turn",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_left_turn",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_high_beam",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_lights_low_beam",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_msg_valid_speed",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("speed_ratio",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_min",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_max",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_mean",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_median",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_var",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("num_msg_valid_wipers",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_not_equipped",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_off",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_intermittent",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_low",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_high",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_washer",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_wipers_automatic",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_msg_valid_abs",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_abs_not_equipped",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_abs_off",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_abs_on",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_abs_engaged",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_msg_valid_stab",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_stab_not_equipped",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_stab_off",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_stab_on",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_stab_engaged",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_msg_valid_trac",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_trac_not_equipped",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_trac_off",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_trac_on",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("num_trac_engaged",static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("pres_wx","");
  setattr("wx","");
  setattr("road_state_1", static_cast<int>(vdt_const::SHORT_FILL_VALUE));

  //These appear to be used from the RWX but not the VDT
  //TODO: refactor these to a subclass for the RWX
  setattr("fcst_air_temp",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_wind_speed",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_precip_type",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_precip_rate",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_rh",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_visibility",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_prob_fog",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_road_water_phase_type",static_cast<double>(vdt_const::FILL_VALUE));
  setattr("fcst_road_temp",static_cast<double>(vdt_const::FILL_VALUE));
}

void vdt_probe_message_qc_statistics::init_stats_fields(const std::vector<std::string>& stats_fields)
{
  for(const std::string& name : stats_fields)
  {
     field_value_map.insert(std::make_pair(name, std::vector<float>()));

     setattr("num_msg_valid_" + name,static_cast<int>(vdt_const::SHORT_FILL_VALUE));
     setattr(name + "_min",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_max",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_mean",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_median",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_iqr25",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_iqr75",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_var",static_cast<double>(vdt_const::FILL_VALUE));
     setattr(name + "_stdev",static_cast<double>(vdt_const::FILL_VALUE));
  }
}

vdt_probe_message_qc_statistics::vdt_probe_message_qc_statistics(const std::vector<std::string>& stats_fields)
{
  init();
  init_stats_fields(stats_fields);
}

vdt_probe_message_qc_statistics::vdt_probe_message_qc_statistics(time_t begin_time_value, time_t end_time_value, const vector<vdt_probe_message_qc> &msgs,const std::vector<std::string>& stats_fields) 
{
  // Initialize
  init();
  init_stats_fields(stats_fields);

  get_begin_time() = begin_time_value;
  get_end_time() = end_time_value;

  int abs_ne = 0;
  int abs_off = 0;
  int abs_on = 0;
  int abs_engaged = 0;

  int lights_off = 0;
  int lights_hazard = 0;
  int lights_parking = 0;
  int lights_fog = 0;
  int lights_drl = 0;
  int lights_automatic_control = 0;
  int lights_right_turn = 0;
  int lights_left_turn = 0;
  int lights_high_beam = 0;
  int lights_low_beam = 0;

  int stab_ne = 0;
  int stab_off = 0;
  int stab_on = 0;
  int stab_engaged = 0;

  int trac_ne = 0;
  int trac_off = 0;
  int trac_on = 0;
  int trac_engaged = 0;

  int wipers_ne = 0;
  int wipers_off = 0;
  int wipers_int = 0;
  int wipers_low = 0;
  int wipers_high = 0;
  int wipers_washer = 0;
  int wipers_auto = 0;



  if (msgs.size() == 0)
    {
      return;
    }

  get_id() = msgs[0].get_road_segment_id();

  // Iterate through the probe messages associated with the current
  // segment or grid cell collecting those that have passed range qc
  // into vectors
  for (size_t i = 0; i < msgs.size(); i++)
    {
      const vdt_probe_message_qc &msg = msgs[i];

      for(const std::string& name : stats_fields)
      {
	if (msg.getattr<int>(name + "_cat_passed") == vdt_probe_message_qc::PASSED)
	{
	  pika_type_id type_id= boost::apply_visitor(PikaTypeVisitor(),msg.find(name)->second);
	  if(type_id == INT_TYPE)
	  {
	    add_value(field_value_map[name],static_cast<float>(msg.getattr<int>(name)));
	  }
	  else if(type_id == DOUBLE_TYPE)
	  {
	    add_value(field_value_map[name],static_cast<float>(msg.getattr<double>(name)));
	  }
	}
      }

      if (msg.get_heading_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(headings, msg.get_heading());
	}
      if (msg.get_hoz_accel_lat_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(lat_accel, msg.get_hoz_accel_lat());
	}
      if (msg.get_hoz_accel_long_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(lon_accel, msg.get_hoz_accel_long());
	}
      if (msg.get_speed_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(speeds, msg.get_speed());
	}
      if (msg.get_steering_angle_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(steering_angle, msg.get_steering_angle());
	}
      if (msg.get_yaw_range_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  add_value(yaw, msg.get_yaw());
	}

      short lights = msg.get_lights();

      if (lights == vdt_probe_message::LIGHTS_ALL_LIGHTS_OFF)
	{
	  lights_off ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_PARKING_LIGHTS_ON))
	{
	  lights_parking ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_FOG_LIGHTS_ON))
	{
	  lights_fog ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_DAYTIME_RUNNING_LIGHTS_ON))
	{
	  lights_drl ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_AUTOMATIC_LIGHT_CONTROL_ON))
	{
	  lights_automatic_control ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_RIGHT_TURN_SIGNAL_ON))
	{
	  lights_right_turn ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_LEFT_TURN_SIGNAL_ON))
	{
	  lights_left_turn ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_HIGH_BEAM_HEADLIGHTS_ON))
	{
	  lights_high_beam ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_LOW_BEAM_HEADLIGHTS_ON))
	{
	  lights_low_beam ++;
	}
      if (lights != vdt_const::FILL_VALUE && (lights & vdt_probe_message::LIGHTS_HAZARD_SIGNAL_ON))
	{
	  lights_hazard ++;
	}

      short abs = msg.get_abs();
      if (abs == vdt_probe_message::ABS_STATUS_NOTEQUIPPED)
	{
	  abs_ne ++;
	}
      else if (abs == vdt_probe_message::ABS_STATUS_OFF)
	{
	  abs_off ++;
	}
      else if (abs == vdt_probe_message::ABS_STATUS_ON)
	{
	  abs_on ++;
	}
      else if (abs == vdt_probe_message::ABS_STATUS_ENGAGED)
	{
	  abs_engaged ++;
	}

      short stab = msg.get_stab();
      if (stab == vdt_probe_message::STAB_STATUS_NOTEQUIPPED)
	{
	  stab_ne ++;
	}
      else if (stab == vdt_probe_message::STAB_STATUS_OFF)
	{
	  stab_off ++;
	}
      else if (stab == vdt_probe_message::STAB_STATUS_ON)
	{
	  stab_on ++;
	}
      else if (stab == vdt_probe_message::STAB_STATUS_ENGAGED)
	{
	  stab_engaged ++;
	}

      short trac = msg.get_trac();
      if (trac == vdt_probe_message::TRAC_STATUS_NOTEQUIPPED)
	{
	  trac_ne ++;
	}
      else if (trac == vdt_probe_message::TRAC_STATUS_OFF)
	{
	  trac_off ++;
	}
      else if (trac == vdt_probe_message::TRAC_STATUS_ON)
	{
	  trac_on ++;
	}
      else if (trac == vdt_probe_message::TRAC_STATUS_ENGAGED)
	{
	  trac_engaged ++;
	}

      short wipers = msg.get_wipers_front_status();
      if (wipers == vdt_probe_message::WIPER_STATUS_NOTEQUIPPED)
	{
	  wipers_ne ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_OFF)
	{
	  wipers_off ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_INTERMITTENT)
	{
	  wipers_int ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_LOW)
	{
	  wipers_low ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_HIGH)
	{
	  wipers_high ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_WASHER)
	{
	  wipers_washer ++;
	}
      else if (wipers == vdt_probe_message::WIPER_STATUS_AUTOMATICPRESENT)
	{
	  wipers_auto ++;
	}
    }

  // Calculate statistics
  get_num_msg() = static_cast<int>(msgs.size());

  for(const std::string& name : stats_fields)
  {
    std::vector<float>& vec = field_value_map[name];
    setattr("num_msg_valid_" + name, static_cast<int>(vec.size()));
    get_min_max(vec, getattr<double>(name + "_min"), getattr<double>(name + "_max"));
    get_mean(vec,  getattr<double>(name + "_mean"));
    vdt_spatial_check_iqr::iqr_median(vec, &getattr<double>(name + "_iqr25"), &getattr<double>(name + "_iqr75"), &getattr<double>(name + "_median"));
    get_var(vec, getattr<double>(name + "_var"));
    get_stdev(vec, getattr<double>(name + "_stdev"));
  }

  get_num_msg_valid_heading() = static_cast<int>(headings.size());
  get_min_max(headings, get_heading_min(), get_heading_max());
  get_mean(headings, get_heading_mean());
  vdt_spatial_check_iqr::iqr_median(headings, &get_heading_iqr25(), &get_heading_iqr75(), &get_heading_median());
  get_var(headings, get_heading_var());
  get_stdev(headings, get_heading_stdev());

  get_num_msg_valid_lat_accel() = static_cast<int>(lat_accel.size());
  get_min_max(lat_accel, get_lat_accel_min(), get_lat_accel_max());
  get_mean(lat_accel, get_lat_accel_mean());
  vdt_spatial_check_iqr::iqr_median(lat_accel, &get_lat_accel_iqr25(), &get_lat_accel_iqr75(), &get_lat_accel_median());
  get_var(lat_accel, get_lat_accel_var());
  get_stdev(lat_accel, get_lat_accel_stdev());

  get_num_msg_valid_lon_accel() = static_cast<int>(lon_accel.size());
  get_min_max(lon_accel, get_lon_accel_min(), get_lon_accel_max());
  get_mean(lon_accel, get_lon_accel_mean());
  vdt_spatial_check_iqr::iqr_median(lon_accel, &get_lon_accel_iqr25(), &get_lon_accel_iqr75(), &get_lon_accel_median());
  get_var(lon_accel, get_lon_accel_var());
  get_stdev(lon_accel, get_lon_accel_stdev());

  get_num_msg_valid_speed() = static_cast<int>(speeds.size());
  get_min_max(speeds, get_speed_min(), get_speed_max());
  get_mean(speeds, get_speed_mean());
  vdt_spatial_check_iqr::iqr_median(speeds, &get_speed_iqr25(), &get_speed_iqr75(), &get_speed_median());
  get_var(speeds, get_speed_var());
  get_stdev(speeds, get_speed_stdev());

  get_num_msg_valid_steering_angle() = static_cast<int>(steering_angle.size());
  get_min_max(steering_angle, get_steering_angle_min(), get_steering_angle_max());
  get_mean(steering_angle, get_steering_angle_mean());
  vdt_spatial_check_iqr::iqr_median(steering_angle, &get_steering_angle_iqr25(), &get_steering_angle_iqr75(), &get_steering_angle_median());
  get_var(steering_angle, get_steering_angle_var());
  get_stdev(steering_angle, get_steering_angle_stdev());

  get_num_msg_valid_yaw() = static_cast<int>(yaw.size());
  get_min_max(yaw, get_yaw_min(), get_yaw_max());
  get_mean(yaw, get_yaw_mean());
  vdt_spatial_check_iqr::iqr_median(yaw, &get_yaw_iqr25(), &get_yaw_iqr75(), &get_yaw_median());
  get_var(yaw, get_yaw_var());
  get_stdev(yaw, get_yaw_stdev());

  get_num_msg_valid_abs() = abs_ne + abs_off + abs_on + abs_engaged;
  if (get_num_msg_valid_abs() > 0)
    {
      get_num_abs_not_equipped() = abs_ne;
      get_num_abs_off() = abs_off;
      get_num_abs_on() = abs_on;
      get_num_abs_engaged() = abs_engaged;
    }

  get_num_msg_valid_lights() = lights_off + lights_hazard + lights_parking + lights_fog + lights_drl + lights_automatic_control + lights_right_turn + lights_left_turn + lights_low_beam + lights_high_beam;
  if (get_num_msg_valid_lights() > 0)
    {
      get_num_lights_off() = lights_off;
      get_num_lights_hazard() = lights_hazard;
      get_num_lights_parking() = lights_parking;
      get_num_lights_fog() = lights_fog;
      get_num_lights_drl() = lights_drl;
      get_num_lights_automatic_control() = lights_automatic_control;
      get_num_lights_right_turn() = lights_right_turn;
      get_num_lights_left_turn() = lights_left_turn;
      get_num_lights_low_beam() = lights_low_beam;
      get_num_lights_high_beam() = lights_high_beam;
    }

  get_num_msg_valid_stab() = stab_ne + stab_off + stab_on + stab_engaged;
  if (get_num_msg_valid_stab() > 0)
    {
      get_num_stab_not_equipped() = stab_ne;
      get_num_stab_off() = stab_off;
      get_num_stab_on() = stab_on;
      get_num_stab_engaged() = stab_engaged;
    }

  get_num_msg_valid_trac() = trac_ne + trac_off + trac_on + trac_engaged;
  if (get_num_msg_valid_trac() > 0)
    {
      get_num_trac_not_equipped() = trac_ne;
      get_num_trac_off() = trac_off;
      get_num_trac_on() = trac_on;
      get_num_trac_engaged() = trac_engaged;
    }

  get_num_msg_valid_wipers() = wipers_ne + wipers_off + wipers_int + wipers_low + wipers_high + wipers_washer + wipers_auto;
  if (get_num_msg_valid_wipers() > 0)
    {
      get_num_wipers_not_equipped() = wipers_ne;
      get_num_wipers_off() = wipers_off;
      get_num_wipers_intermittent() = wipers_int;
      get_num_wipers_low() = wipers_low;
      get_num_wipers_high() = wipers_high;
      get_num_wipers_washer() = wipers_washer;
      get_num_wipers_automatic() = wipers_auto;
    }
}

void vdt_probe_message_qc_statistics::get_min_max(const vector<float> &vals, float &min, float &max)
{
  if (vals.size() > 0)
    {
      ST_min_max(vals, &min, &max);
    }
  else
    {
      min = vdt_const::FILL_VALUE;
      max = vdt_const::FILL_VALUE;
    }
}

void vdt_probe_message_qc_statistics::get_mean(const vector<float> &vals, float &mean)
{
  if (vals.size() > 0)
    {
      mean = ST_mean(vals);
    }
  else
    {
      mean = vdt_const::FILL_VALUE;
    }
}

void vdt_probe_message_qc_statistics::get_median(const vector<float> &vals, float &median)
{
  if (vals.size() > 0)
    {
      median = ST_median(vals);
    }
  else
    {
      median = vdt_const::FILL_VALUE;
    }
}

void vdt_probe_message_qc_statistics::get_var(const vector<float> &vals, float &var)
{
  if (vals.size() > 0)
    {
      var = ST_variance(vals);
    }
  else
    {
      var = vdt_const::FILL_VALUE;
    }
}

void vdt_probe_message_qc_statistics::get_stdev(const vector<float> &vals, float &stdev)
{
  if (vals.size() > 0)
    {
      stdev = ST_standard_deviation(vals);
    }
  else
    {
      stdev = vdt_const::FILL_VALUE;
    }
}

int vdt_probe_message_qc_statistics::augment_statistics_vector_with_datasets(const vector<vdt_dataset> &datasets, vector<vdt_probe_message_qc_statistics> &seg_stats, string &error)
{
  for (size_t i=0; i<seg_stats.size(); i++)
    {
      if (seg_stats.size() != datasets.size())
	{
	  stringstream oss;
	  oss << "seg stats size: " << seg_stats.size() << ", datasets size: " << datasets.size();
	  error = oss.str();
	  return -1;
	}
      else
	{
	  seg_stats[i].augment_statistics_with_dataset(datasets[i]);
	}
    }

  return 0;
}
void vdt_probe_message_qc_statistics::augment_statistics_with_dataset(const vdt_dataset &dataset)
{
  const vdt_dataset &ds = dataset;
  get_model_air_temp() = ds.model_air_temp;
  get_model_dewpoint() = ds.model_dewpoint;
  get_model_bar_press() = ds.model_bar_press;
  get_radar_ref() = ds.radar_ref;
  get_radar_cref() = ds.radar_cref;
  get_radar_dual_pol_hc() = ds.radar_dual_pol_hc;
  get_radar_dual_pol_hr() = ds.radar_dual_pol_hr;    
  get_radar_precip_flag() = ds.radar_precip_flag;
  get_radar_precip_type() = ds.radar_precip_type;
  get_cloud_mask() = ds.cloud_mask;
  get_nss_air_temp_mean() = ds.nss_air_temp_mean;
  get_nss_surface_temp_mean() = ds.nss_surface_temp_mean;
  get_nss_bar_press_mean() = ds.nss_bar_press_mean;
  get_nss_dew_temp_mean() = ds.nss_dew_temp_mean;
  get_nss_wind_dir_mean() = ds.nss_wind_dir_mean;
  get_nss_wind_speed_mean() = ds.nss_wind_speed_mean;
  get_nss_hourly_precip_mean() = ds.nss_hourly_precip_mean;
  get_nss_prevail_vis_mean() = ds.nss_prevail_vis_mean;
  get_pres_wx() = ds.pres_wx;
  get_wx() = ds.wx;
  get_road_state_1() = ds.road_state_1;
}

void vdt_probe_message_qc_statistics::get_probe_message_qc_statistics(time_t begin_time, time_t end_time, const unordered_map<int, vdt_index_distance> &nn_index_map, const vector<vdt_probe_message_qc> &qc_msgs, const unordered_map<int, int> &seg_id_map, const std::vector<std::string>& stats_fields, vector<vdt_probe_message_qc_statistics> &stats_vec, unordered_map<int, int> &seg_index_offset_map)
{
  vector<vdt_probe_message_qc> msgs;

  int ct = 0;
  for (auto itr=nn_index_map.begin(); itr!=nn_index_map.end(); ++itr)
    {
      // collect probe messages for segment that meet time conditions
      // in the vector msgs
      msgs.clear();
      
      const vector<int> &indices = itr->second.indices;
      for (size_t i=0; i<indices.size(); i++)
	{
	  int offset = indices[i];
	  if (qc_msgs[offset].get_obs_time() >= begin_time && qc_msgs[offset].get_obs_time() < end_time)
	    {
	      msgs.push_back(qc_msgs[offset]);
	    }
	  else
	    {
	      ;
	    }
	}

      vdt_probe_message_qc_statistics stat(begin_time, end_time, msgs, stats_fields);
      auto seg_id_itr = seg_id_map.find(itr->first);
      if (seg_id_itr != seg_id_map.end())
	{
	  stat.get_id() = seg_id_itr->second;
	}
      else
	{
	  stat.get_id() = -1;
	}
      stats_vec.push_back(stat);
      seg_index_offset_map[stat.get_id()] = ct;
      ct += 1;
    }
}

void vdt_probe_message_qc_statistics::add_non_associated_ids(time_t begin_time, time_t end_time, const vector<int> &stat_ids, const std::vector<std::string>& stats_fields, vector<vdt_probe_message_qc_statistics> &stats_vec)
{
  // Note that the empty msgs vector is used only to get the vdt_probe_message_qc_statistics constructor to work properly
  vector<vdt_probe_message_qc> msgs;

  // First create set containing ids already in stats_vec
  unordered_set<int> id_set;
  for (size_t i=0; i<stats_vec.size(); i++)
    {
      id_set.insert(stats_vec[i].get_id());
    }

  for (size_t i=0; i<stat_ids.size(); i++)
    {
      auto itr = id_set.find(stat_ids[i]);
      if (itr == id_set.end())
	{
	  vdt_probe_message_qc_statistics stat(begin_time, end_time, msgs, stats_fields);
	  stat.get_id() = stat_ids[i];
	  stats_vec.push_back(stat);
	}
    }
}

void vdt_probe_message_qc_statistics::add_value(vector<float>& vals, float val)
{
  if (val != vdt_const::FILL_VALUE)
    {
      vals.push_back(val);
    }
}
