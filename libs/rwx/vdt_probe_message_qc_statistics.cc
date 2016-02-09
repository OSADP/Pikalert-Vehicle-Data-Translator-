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


#include <vdt4/stat_tools.hh>

#include <unordered_set>
#include "vdt_probe_message_qc_statistics.hh"
#include <vdt4/vdt_const.hh>
#include <vdt4/vdt_spatial_check.hh>


//Namespace to prevent name collisions with similar class in VDT 
namespace rwx
{

/**
 * @file vdt_probe_message_qc_statistics.cc
 */

void vdt_probe_message_qc_statistics::init()
{
  model_air_temp = vdt_const::FILL_VALUE;
  model_dewpoint = vdt_const::FILL_VALUE;
  model_bar_press = vdt_const::FILL_VALUE;
  radar_ref = vdt_const::FILL_VALUE;
  radar_cref = vdt_const::FILL_VALUE;
  radar_precip_flag = vdt_const::FILL_VALUE;
  radar_precip_type = vdt_const::FILL_VALUE;
  cloud_mask = vdt_const::SHORT_FILL_VALUE;

  nss_air_temp_mean = vdt_const::FILL_VALUE;
  nss_surface_temp_mean = vdt_const::FILL_VALUE;
  nss_bar_press_mean = vdt_const::FILL_VALUE;
  nss_dew_temp_mean = vdt_const::FILL_VALUE;
  nss_wind_dir_mean = vdt_const::FILL_VALUE;
  nss_wind_speed_mean = vdt_const::FILL_VALUE;
  nss_hourly_precip_mean = vdt_const::FILL_VALUE;
  nss_prevail_vis_mean = vdt_const::FILL_VALUE;

  num_msg = 0;
  num_msg_valid_air_temp = vdt_const::FILL_VALUE;
  air_temp_min = vdt_const::FILL_VALUE;
  air_temp_max = vdt_const::FILL_VALUE;
  air_temp_mean = vdt_const::FILL_VALUE;
  air_temp_median = vdt_const::FILL_VALUE;
  air_temp_var = vdt_const::FILL_VALUE;
  air_temp_stdev = vdt_const::FILL_VALUE;
  air_temp_iqr25 = vdt_const::FILL_VALUE;
  air_temp_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_air_temp2 = vdt_const::FILL_VALUE;
  air_temp2_min = vdt_const::FILL_VALUE;
  air_temp2_max = vdt_const::FILL_VALUE;
  air_temp2_mean = vdt_const::FILL_VALUE;
  air_temp2_median = vdt_const::FILL_VALUE;
  air_temp2_var = vdt_const::FILL_VALUE;
  air_temp2_stdev = vdt_const::FILL_VALUE;
  air_temp2_iqr25 = vdt_const::FILL_VALUE;
  air_temp2_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_bar_press = vdt_const::FILL_VALUE;
  bar_press_min = vdt_const::FILL_VALUE;
  bar_press_max = vdt_const::FILL_VALUE;
  bar_press_mean = vdt_const::FILL_VALUE;
  bar_press_median = vdt_const::FILL_VALUE;
  bar_press_var = vdt_const::FILL_VALUE;
  bar_press_stdev = vdt_const::FILL_VALUE;
  bar_press_iqr25 = vdt_const::FILL_VALUE;
  bar_press_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_dew_temp = vdt_const::FILL_VALUE;
  dew_temp_min = vdt_const::FILL_VALUE;
  dew_temp_max = vdt_const::FILL_VALUE;
  dew_temp_mean = vdt_const::FILL_VALUE;
  dew_temp_median = vdt_const::FILL_VALUE;
  dew_temp_var = vdt_const::FILL_VALUE;
  dew_temp_stdev = vdt_const::FILL_VALUE;
  dew_temp_iqr25 = vdt_const::FILL_VALUE;
  dew_temp_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_heading = vdt_const::FILL_VALUE;
  heading_min = vdt_const::FILL_VALUE;
  heading_max = vdt_const::FILL_VALUE;
  heading_mean = vdt_const::FILL_VALUE;
  heading_median = vdt_const::FILL_VALUE;
  heading_var = vdt_const::FILL_VALUE;
  heading_stdev = vdt_const::FILL_VALUE;
  heading_iqr25 = vdt_const::FILL_VALUE;
  heading_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_lat_accel = vdt_const::FILL_VALUE;
  lat_accel_min = vdt_const::FILL_VALUE;
  lat_accel_max = vdt_const::FILL_VALUE;
  lat_accel_mean = vdt_const::FILL_VALUE;
  lat_accel_median = vdt_const::FILL_VALUE;
  lat_accel_var = vdt_const::FILL_VALUE;
  lat_accel_stdev = vdt_const::FILL_VALUE;
  lat_accel_iqr25 = vdt_const::FILL_VALUE;
  lat_accel_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_lon_accel = vdt_const::FILL_VALUE;
  lon_accel_min = vdt_const::FILL_VALUE;
  lon_accel_max = vdt_const::FILL_VALUE;
  lon_accel_mean = vdt_const::FILL_VALUE;
  lon_accel_median = vdt_const::FILL_VALUE;
  lon_accel_var = vdt_const::FILL_VALUE;
  lon_accel_stdev = vdt_const::FILL_VALUE;
  lon_accel_iqr25 = vdt_const::FILL_VALUE;
  lon_accel_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_yaw = vdt_const::FILL_VALUE;
  yaw_min = vdt_const::FILL_VALUE;
  yaw_max = vdt_const::FILL_VALUE;
  yaw_mean = vdt_const::FILL_VALUE;
  yaw_median = vdt_const::FILL_VALUE;
  yaw_var = vdt_const::FILL_VALUE;
  yaw_stdev = vdt_const::FILL_VALUE;
  yaw_iqr25 = vdt_const::FILL_VALUE;
  yaw_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_steering_angle = vdt_const::FILL_VALUE;
  steering_angle_min = vdt_const::FILL_VALUE;
  steering_angle_max = vdt_const::FILL_VALUE;
  steering_angle_mean = vdt_const::FILL_VALUE;
  steering_angle_median = vdt_const::FILL_VALUE;
  steering_angle_var = vdt_const::FILL_VALUE;
  steering_angle_stdev = vdt_const::FILL_VALUE;
  steering_angle_iqr25 = vdt_const::FILL_VALUE;
  steering_angle_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_lights = vdt_const::FILL_VALUE;
  num_lights_off = vdt_const::FILL_VALUE;
  num_lights_hazard = vdt_const::FILL_VALUE;
  num_lights_parking = vdt_const::FILL_VALUE;
  num_lights_fog = vdt_const::FILL_VALUE;
  num_lights_drl = vdt_const::FILL_VALUE;
  num_lights_automatic_control = vdt_const::FILL_VALUE;
  num_lights_right_turn = vdt_const::FILL_VALUE;
  num_lights_left_turn = vdt_const::FILL_VALUE;
  num_lights_high_beam = vdt_const::FILL_VALUE;
  num_lights_low_beam = vdt_const::FILL_VALUE;

  num_msg_valid_speed = vdt_const::FILL_VALUE;
  speed_ratio = vdt_const::FILL_VALUE;
  speed_min = vdt_const::FILL_VALUE;
  speed_max = vdt_const::FILL_VALUE;
  speed_mean = vdt_const::FILL_VALUE;
  speed_median = vdt_const::FILL_VALUE;
  speed_var = vdt_const::FILL_VALUE;
  speed_stdev = vdt_const::FILL_VALUE;
  speed_iqr25 = vdt_const::FILL_VALUE;
  speed_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_surface_temp = vdt_const::FILL_VALUE;
  surface_temp_min = vdt_const::FILL_VALUE;
  surface_temp_max = vdt_const::FILL_VALUE;
  surface_temp_mean = vdt_const::FILL_VALUE;
  surface_temp_median = vdt_const::FILL_VALUE;
  surface_temp_var = vdt_const::FILL_VALUE;
  surface_temp_stdev = vdt_const::FILL_VALUE;
  surface_temp_iqr25 = vdt_const::FILL_VALUE;
  surface_temp_iqr75 = vdt_const::FILL_VALUE;

  num_msg_valid_wipers = vdt_const::FILL_VALUE;
  num_wipers_not_equipped = vdt_const::FILL_VALUE;
  num_wipers_off = vdt_const::FILL_VALUE;
  num_wipers_intermittent = vdt_const::FILL_VALUE;
  num_wipers_low = vdt_const::FILL_VALUE;
  num_wipers_high = vdt_const::FILL_VALUE;
  num_wipers_washer = vdt_const::FILL_VALUE;
  num_wipers_automatic = vdt_const::FILL_VALUE;

  num_msg_valid_abs = vdt_const::FILL_VALUE;
  num_abs_not_equipped = vdt_const::FILL_VALUE;
  num_abs_off = vdt_const::FILL_VALUE;
  num_abs_on = vdt_const::FILL_VALUE;
  num_abs_engaged = vdt_const::FILL_VALUE;

  num_msg_valid_stab = vdt_const::FILL_VALUE;
  num_stab_not_equipped = vdt_const::FILL_VALUE;
  num_stab_off = vdt_const::FILL_VALUE;
  num_stab_on = vdt_const::FILL_VALUE;
  num_stab_engaged = vdt_const::FILL_VALUE;

  num_msg_valid_trac = vdt_const::FILL_VALUE;;
  num_trac_not_equipped = vdt_const::FILL_VALUE;
  num_trac_off = vdt_const::FILL_VALUE;
  num_trac_on = vdt_const::FILL_VALUE;
  num_trac_engaged = vdt_const::FILL_VALUE;
  
  fcst_air_temp = vdt_const::FILL_VALUE;
  fcst_dew_temp = vdt_const::FILL_VALUE;
  fcst_wind_speed = vdt_const::FILL_VALUE;
  fcst_precip_type = vdt_const::FILL_VALUE;
  fcst_precip_rate = vdt_const::FILL_VALUE;
  fcst_rh = vdt_const::FILL_VALUE;
  fcst_visibility = vdt_const::FILL_VALUE;
  fcst_prob_fog = vdt_const::FILL_VALUE;
  fcst_road_temp = vdt_const::FILL_VALUE;
  fcst_road_water_phase_type = vdt_const::FILL_VALUE;

  wx_precip_type = vdt_const::FILL_VALUE;
  wx_precip_intensity = vdt_const::FILL_VALUE;
  wx_visibility = vdt_const::FILL_VALUE;
  
  prev_precip_type = vdt_const::FILL_VALUE;
  prev_precip_intensity = vdt_const::FILL_VALUE;
  prev_precip_time_gap = vdt_const::FILL_VALUE;
}

vdt_probe_message_qc_statistics::vdt_probe_message_qc_statistics()
{
  init();
}

vdt_probe_message_qc_statistics::vdt_probe_message_qc_statistics(time_t begin_time_value, time_t end_time_value, const vector<vdt_probe_message_qc> &msgs) : begin_time(begin_time_value), end_time(end_time_value)
{
  // Initialize
  init();

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

  id = msgs[0].get_road_segment_id();

  // Iterate through the probe messages associated with the current
  // segment or grid cell collecting those that have passed range qc
  // into vectors
  for (size_t i = 0; i < msgs.size(); i++)
    {
      const vdt_probe_message_qc &msg = msgs[i];

      // Only add fields that pass qc in evaluating statistics
      if (msg.get_air_temp_cat_passed() == vdt_probe_message_qc::PASSED)
	{
	  printf("temp_cat passed\n");
	  add_value(air_temps, msg.get_air_temp());
	}
      if (msg.get_air_temp2_cat_passed() == vdt_probe_message_qc::PASSED)
	{
	  printf("temp2_cat passed\n");
	  add_value(air_temps2, msg.get_air_temp2());
	}
      if (msg.get_bar_press_cat_passed() == vdt_probe_message_qc::PASSED)
	{
	  printf("bar_press_cat passed\n");
	  add_value(bar_press, msg.get_bar_press());
	}
      if (msg.get_dew_temp_cat_passed() == vdt_probe_message_qc::PASSED)
	{
	  printf("dew_temp_cat passed\n");
	  add_value(dew_temps, msg.get_dew_temp());
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
      if (msg.get_surface_temp_cat_passed() == vdt_probe_message_qc::PASSED)
	{
	  printf("surface_temp_cat passed\n");
	  add_value(surface_temps, msg.get_surface_temp());
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
  num_msg = msgs.size();

  num_msg_valid_air_temp = air_temps.size();
  get_min_max(air_temps, air_temp_min, air_temp_max);
  get_mean(air_temps, air_temp_mean);
  vdt_spatial_check_iqr::iqr_median(air_temps, &air_temp_iqr25, &air_temp_iqr75, &air_temp_median);
  get_var(air_temps, air_temp_var);
  get_stdev(air_temps, air_temp_stdev);

  num_msg_valid_air_temp2 = air_temps2.size();
  get_min_max(air_temps2, air_temp2_min, air_temp2_max);
  get_mean(air_temps2, air_temp2_mean);
  vdt_spatial_check_iqr::iqr_median(air_temps2, &air_temp2_iqr25, &air_temp2_iqr75, &air_temp2_median);
  get_var(air_temps2, air_temp2_var);
  get_stdev(air_temps2, air_temp2_stdev);

  num_msg_valid_bar_press = bar_press.size();
  get_min_max(bar_press, bar_press_min, bar_press_max);
  get_mean(bar_press, bar_press_mean);
  vdt_spatial_check_iqr::iqr_median(bar_press, &bar_press_iqr25, &bar_press_iqr75, &bar_press_median);
  get_var(bar_press, bar_press_var);
  get_stdev(bar_press, bar_press_stdev);

  num_msg_valid_dew_temp = dew_temps.size();
  get_min_max(dew_temps, dew_temp_min, dew_temp_max);
  get_mean(dew_temps, dew_temp_mean);
  vdt_spatial_check_iqr::iqr_median(dew_temps, &dew_temp_iqr25, &dew_temp_iqr75, &dew_temp_median);
  get_var(dew_temps, dew_temp_var);
  get_stdev(dew_temps, dew_temp_stdev);

  num_msg_valid_heading = headings.size();
  get_min_max(headings, heading_min, heading_max);
  get_mean(headings, heading_mean);
  vdt_spatial_check_iqr::iqr_median(headings, &heading_iqr25, &heading_iqr75, &heading_median);
  get_var(headings, heading_var);
  get_stdev(headings, heading_stdev);

  num_msg_valid_lat_accel = lat_accel.size();
  get_min_max(lat_accel, lat_accel_min, lat_accel_max);
  get_mean(lat_accel, lat_accel_mean);
  vdt_spatial_check_iqr::iqr_median(lat_accel, &lat_accel_iqr25, &lat_accel_iqr75, &lat_accel_median);
  get_var(lat_accel, lat_accel_var);
  get_stdev(lat_accel, lat_accel_stdev);

  num_msg_valid_lon_accel = lon_accel.size();
  get_min_max(lon_accel, lon_accel_min, lon_accel_max);
  get_mean(lon_accel, lon_accel_mean);
  vdt_spatial_check_iqr::iqr_median(lon_accel, &lon_accel_iqr25, &lon_accel_iqr75, &lon_accel_median);
  get_var(lon_accel, lon_accel_var);
  get_stdev(lon_accel, lon_accel_stdev);

  num_msg_valid_speed = speeds.size();
  get_min_max(speeds, speed_min, speed_max);
  get_mean(speeds, speed_mean);
  vdt_spatial_check_iqr::iqr_median(speeds, &speed_iqr25, &speed_iqr75, &speed_median);
  get_var(speeds, speed_var);
  get_stdev(speeds, speed_stdev);

  num_msg_valid_steering_angle = steering_angle.size();
  get_min_max(steering_angle, steering_angle_min, steering_angle_max);
  get_mean(steering_angle, steering_angle_mean);
  vdt_spatial_check_iqr::iqr_median(steering_angle, &steering_angle_iqr25, &steering_angle_iqr75, &steering_angle_median);
  get_var(steering_angle, steering_angle_var);
  get_stdev(steering_angle, steering_angle_stdev);

  num_msg_valid_surface_temp = surface_temps.size();
  get_min_max(surface_temps, surface_temp_min, surface_temp_max);
  get_mean(surface_temps, surface_temp_mean);
  vdt_spatial_check_iqr::iqr_median(surface_temps, &surface_temp_iqr25, &surface_temp_iqr75, &surface_temp_median);
  get_var(surface_temps, surface_temp_var);
  get_stdev(surface_temps, surface_temp_stdev);

  num_msg_valid_yaw = yaw.size();
  get_min_max(yaw, yaw_min, yaw_max);
  get_mean(yaw, yaw_mean);
  vdt_spatial_check_iqr::iqr_median(yaw, &yaw_iqr25, &yaw_iqr75, &yaw_median);
  get_var(yaw, yaw_var);
  get_stdev(yaw, yaw_stdev);

  num_msg_valid_abs = abs_ne + abs_off + abs_on + abs_engaged;
  if (num_msg_valid_abs > 0)
    {
      num_abs_not_equipped = abs_ne;
      num_abs_off = abs_off;
      num_abs_on = abs_on;
      num_abs_engaged = abs_engaged;
    }

  num_msg_valid_lights = lights_off + lights_hazard + lights_parking + lights_fog + lights_drl + lights_automatic_control + lights_right_turn + lights_left_turn + lights_low_beam + lights_high_beam;
  if (num_msg_valid_lights > 0)
    {
      num_lights_off = lights_off;
      num_lights_hazard = lights_hazard;
      num_lights_parking = lights_parking;
      num_lights_fog = lights_fog;
      num_lights_drl = lights_drl;
      num_lights_automatic_control = lights_automatic_control;
      num_lights_right_turn = lights_right_turn;
      num_lights_left_turn = lights_left_turn;
      num_lights_low_beam = lights_low_beam;
      num_lights_high_beam = lights_high_beam;
    }

  num_msg_valid_stab = stab_ne + stab_off + stab_on + stab_engaged;
  if (num_msg_valid_stab > 0)
    {
      num_stab_not_equipped = stab_ne;
      num_stab_off = stab_off;
      num_stab_on = stab_on;
      num_stab_engaged = stab_engaged;
    }

  num_msg_valid_trac = trac_ne + trac_off + trac_on + trac_engaged;
  if (num_msg_valid_trac > 0)
    {
      num_trac_not_equipped = trac_ne;
      num_trac_off = trac_off;
      num_trac_on = trac_on;
      num_trac_engaged = trac_engaged;
    }

  num_msg_valid_wipers = wipers_ne + wipers_off + wipers_int + wipers_low + wipers_high + wipers_washer + wipers_auto;
  if (num_msg_valid_wipers > 0)
    {
      num_wipers_not_equipped = wipers_ne;
      num_wipers_off = wipers_off;
      num_wipers_intermittent = wipers_int;
      num_wipers_low = wipers_low;
      num_wipers_high = wipers_high;
      num_wipers_washer = wipers_washer;
      num_wipers_automatic = wipers_auto;
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
  model_air_temp = ds.model_air_temp;
  model_dewpoint = ds.model_dewpoint;
  model_bar_press = ds.model_bar_press;
  radar_ref = ds.radar_ref;
  radar_precip_flag = ds.radar_precip_flag;
  radar_precip_type = ds.radar_precip_type;
  cloud_mask = ds.cloud_mask;
  nss_air_temp_mean = ds.nss_air_temp_mean;
  nss_surface_temp_mean = ds.nss_surface_temp_mean;
  nss_bar_press_mean = ds.nss_bar_press_mean;
  nss_dew_temp_mean = ds.nss_dew_temp_mean;
  nss_wind_dir_mean = ds.nss_wind_dir_mean;
  nss_wind_speed_mean = ds.nss_wind_speed_mean;
  nss_hourly_precip_mean = ds.nss_hourly_precip_mean;
  nss_prevail_vis_mean = ds.nss_prevail_vis_mean;
}

void vdt_probe_message_qc_statistics::get_probe_message_qc_statistics(time_t begin_time, time_t end_time, const unordered_map<int, vdt_index_distance> &nn_index_map, const vector<vdt_probe_message_qc> &qc_msgs, const unordered_map<int, int> &seg_id_map, vector<vdt_probe_message_qc_statistics> &stats_vec, unordered_map<int, int> &seg_index_offset_map)
{
  vector<vdt_probe_message_qc> msgs;

  int ct = 0;
  for (auto itr=nn_index_map.begin(); itr!=nn_index_map.end(); ++itr)
    {
      // collect probe messages for segment that meet time conditions
      msgs.clear();
      
      const vector<int> &indices = itr->second.indices;
      for (size_t i=0; i<indices.size(); i++)
	{
	  int offset = indices[i];
	  if (qc_msgs[offset].obs_time >= begin_time && qc_msgs[offset].obs_time < end_time)
	    {
	      msgs.push_back(qc_msgs[offset]);
	    }
	  else
	    {
	      ;
	      //printf("road segment time for msg on segment %d is not between begin_time and end_time %lf %ld %ld\n", qc_msgs[offset].road_segment_id, qc_msgs[offset].obs_time, begin_time, end_time);
	    }
	}

      vdt_probe_message_qc_statistics stat(begin_time, end_time, msgs);
      auto seg_id_itr = seg_id_map.find(itr->first);
      if (seg_id_itr != seg_id_map.end())
	{
	  stat.id = seg_id_itr->second;
	}
      else
	{
	  stat.id = -1;
	}
      stats_vec.push_back(stat);
      seg_index_offset_map[stat.id] = ct;
      ct += 1;
    }
}

void vdt_probe_message_qc_statistics::add_non_associated_ids(time_t begin_time, time_t end_time, const vector<int> &stat_ids, vector<vdt_probe_message_qc_statistics> &stats_vec)
{
  // Note that the empty msgs vector is used only to get the vdt_probe_message_qc_statistics constructor to work properly
  vector<vdt_probe_message_qc> msgs;

  // First create set containing ids already in stats_vec
  unordered_set<int> id_set;
  for (size_t i=0; i<stats_vec.size(); i++)
    {
      id_set.insert(stats_vec[i].id);
    }

  for (size_t i=0; i<stat_ids.size(); i++)
    {
      auto itr = id_set.find(stat_ids[i]);
      if (itr == id_set.end())
	{
	  vdt_probe_message_qc_statistics stat(begin_time, end_time, msgs);
	  stat.id = stat_ids[i];
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

} //END namespace RWX
