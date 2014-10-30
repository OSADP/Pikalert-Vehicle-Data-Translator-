//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_test_generator.cc,v $
//       Version: $Revision: 1.10 $  Dated: $Date: 2014/04/15 20:39:55 $
//
//==============================================================================

/**
 * @file vdt_test_generator.cc
 *
 *
 */

// Include files 
#include "vdt_test_generator.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

vdt_test_generator::vdt_test_generator(const config_reader &cfg_reader_arg) : cfg_reader(cfg_reader_arg) 
{
}

vdt_test_generator::~vdt_test_generator()
{
}

void vdt_test_generator::get_road_segments(vector<vdt_road_segment> &road_segments)
{
  vdt_road_segment road_segment;

  // create boulder segments
  vector<vdt_point> aspen_points;
  vector<vdt_point> boulder_north_points;
  vector<vdt_point> boulder_east_points;
  vector<vdt_point> denver_points;
  vector<vdt_point> estes_points;
  vector<vdt_point> glenwood_points;
  vector<vdt_point> steamboat_points;

  aspen_points.push_back(vdt_test_generator::aspen_point);
  boulder_north_points.push_back(vdt_test_generator::boulder_point);
  boulder_east_points.push_back(vdt_test_generator::boulder_point);
  denver_points.push_back(vdt_test_generator::denver_point);
  estes_points.push_back(vdt_test_generator::estes_point);
  glenwood_points.push_back(vdt_test_generator::glenwood_point);
  steamboat_points.push_back(vdt_test_generator::steamboat_point);
  vdt_point point;

  for (int i=1; i<10; i++)
    {
      point = vdt_point(aspen_points[i-1].get_latitude(), aspen_points[i-1].get_longitude() + 0.001, aspen_points[i-1].get_elevation());
      aspen_points.push_back(point);

      point = vdt_point(boulder_north_points[i-1].get_latitude() + 0.001, boulder_north_points[i-1].get_longitude(), boulder_north_points[i-1].get_elevation());
      boulder_north_points.push_back(point);

      point = vdt_point(boulder_east_points[i-1].get_latitude(), boulder_east_points[i-1].get_longitude() + 0.001, boulder_east_points[i-1].get_elevation());
      boulder_east_points.push_back(point);

      point = vdt_point(denver_points[i-1].get_latitude(), denver_points[i-1].get_longitude() + 0.001, denver_points[i-1].get_elevation());
      denver_points.push_back(point);

      point = vdt_point(estes_points[i-1].get_latitude(), estes_points[i-1].get_longitude() + 0.001, estes_points[i-1].get_elevation());
      estes_points.push_back(point);

      point = vdt_point(glenwood_points[i-1].get_latitude(), glenwood_points[i-1].get_longitude() + 0.001, glenwood_points[i-1].get_elevation());
      glenwood_points.push_back(point);

      point = vdt_point(steamboat_points[i-1].get_latitude(), steamboat_points[i-1].get_longitude() + 0.001, steamboat_points[i-1].get_elevation());
      steamboat_points.push_back(point);
    }

  printf("\nA list of points and their latitudes and longitudes used for creating road segments\n");
  printf("Point: lat, lon\n");
  for (size_t i=0; i<boulder_north_points.size(); i++)
    {
      printf("boulder_north[%ld]: %lf %lf\n", i, boulder_north_points[i].latitude, boulder_north_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<boulder_east_points.size(); i++)
    {
      printf("boulder_east[%ld]: %lf %lf\n", i, boulder_east_points[i].latitude, boulder_east_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<denver_points.size(); i++)
    {
      printf("denver[%ld]: %lf %lf\n", i, denver_points[i].latitude, denver_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<estes_points.size(); i++)
    {
      printf("estes[%ld]: %lf %lf\n", i, estes_points[i].latitude, estes_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<glenwood_points.size(); i++)
    {
      printf("glenwood[%ld]: %lf %lf\n", i, glenwood_points[i].latitude, glenwood_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<aspen_points.size(); i++)
    {
      printf("aspen[%ld]: %lf %lf\n", i, aspen_points[i].latitude, aspen_points[i].longitude);
    }

  printf("Point: lat, lon\n");
  for (size_t i=0; i<steamboat_points.size(); i++)
    {
      printf("steamboat[%ld]: %lf %lf\n", i, steamboat_points[i].latitude, steamboat_points[i].longitude);
    }

  int id = 0;

  printf("\nRoad segment information for this test case\n");
  int aspen_id = id;
  id += 1;
  road_segment.set_points(aspen_points);
  road_segment.set_id(aspen_id);
  road_segment.set_road_name("aspen");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int boulder_north_id = id;
  id += 1;
  road_segment.set_points(boulder_north_points);
  road_segment.set_id(boulder_north_id);
  road_segment.set_road_name("boulder_north");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int boulder_east_id = id;
  id += 1;
  road_segment.set_points(boulder_east_points);
  road_segment.set_id(boulder_east_id);
  road_segment.set_road_name("boulder_east");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int denver_id = id;
  id += 1;
  road_segment.set_points(denver_points);
  road_segment.set_id(denver_id);
  road_segment.set_road_name("denver");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int estes_id = id;
  id += 1;
  road_segment.set_points(estes_points);
  road_segment.set_id(estes_id);
  road_segment.set_road_name("estes");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int glenwood_id = id;
  id += 1;
  road_segment.set_points(glenwood_points);
  road_segment.set_id(glenwood_id);
  road_segment.set_road_name("glenwood");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  int steamboat_id = id;
  id += 1;
  road_segment.set_points(steamboat_points);
  road_segment.set_id(steamboat_id);
  road_segment.set_road_name("steamboat");
  road_segments.push_back(road_segment);
  printf("road name, id, lat, lon, elev: %s, %d, %lf, %lf, %lf\n", road_segment.get_road_name().c_str(), road_segment.get_id(), road_segment.get_point().latitude, road_segment.get_point().get_longitude(), road_segment.get_point().get_elevation());

  printf("\n");
}

void vdt_test_generator::get_history_probe_messages(vector<vdt_probe_message_qc> &messages)
{
  vdt_probe_message_qc aspen_message;
  vdt_probe_message_qc estes_message;
  vdt_probe_message_qc steamboat_message;

  // set min values and later set max values to fail step test
  double rec_time = vdt_test_generator::rec_time_past;
  steamboat_message.set_obs_time(vdt_test_generator::obs_time);  steamboat_message.set_rec_time(rec_time_past);
  steamboat_message.set_vehicle_id("steamboat_step_fail");
  steamboat_message.set_point(steamboat_point);
  steamboat_message.set_heading(cfg_reader.heading_min);
  steamboat_message.set_speed(cfg_reader.speed_range_min);
  steamboat_message.set_air_temp(cfg_reader.air_temp_min);
  steamboat_message.set_bar_press(cfg_reader.bar_press_min);
  steamboat_message.set_dew_temp(cfg_reader.air_temp_min);


  // test mid range
  aspen_message.set_obs_time(vdt_test_generator::obs_time);
  aspen_message.set_rec_time(rec_time_past);
  aspen_message.set_vehicle_id("aspen_persist_fail");
  aspen_message.set_point(aspen_point);
  aspen_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0);
  aspen_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0);
  aspen_message.set_air_temp((cfg_reader.air_temp_min+cfg_reader.air_temp_max)/2.0);
  aspen_message.set_bar_press((cfg_reader.bar_press_min+cfg_reader.bar_press_max)/2.0);

  // test mid range
  estes_message.set_obs_time(vdt_test_generator::obs_time);
  estes_message.set_rec_time(rec_time_past);
  estes_message.set_vehicle_id("estes_persist_pass");
  estes_message.set_point(estes_point);
  estes_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0);
  estes_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0);
  estes_message.set_air_temp((cfg_reader.air_temp_min+cfg_reader.air_temp_max)/2.0);
  estes_message.set_bar_press((cfg_reader.bar_press_min+cfg_reader.bar_press_max)/2.0);
  estes_message.set_dew_temp(estes_dew_temp - 1);

  for (int i=0; i<5; i++)
    {
      rec_time += 1;
      
      // store messages having different times but the same values
      aspen_message.set_obs_time(vdt_test_generator::obs_time - 3600 + i * 720);
      aspen_message.set_rec_time(rec_time);

      // store messages having different times but the same values
      estes_message.set_obs_time(vdt_test_generator::obs_time - 3600 + i * 720);
      estes_message.set_rec_time(rec_time);

      // store messages having different times but the same values
      steamboat_message.set_obs_time(vdt_test_generator::obs_time - 3600 + i * 720);
      steamboat_message.set_rec_time(rec_time);

      messages.push_back(aspen_message);
      messages.push_back(estes_message);
      messages.push_back(steamboat_message);
    }
}

void vdt_test_generator::get_probe_messages(vector<vdt_probe_message> &messages)
{
  vdt_probe_message null_test_message;
  vdt_probe_message test_too_low_message;
  vdt_probe_message test_low_message;
  vdt_probe_message test_mid_message;
  vdt_probe_message test_high_message;
  vdt_probe_message test_too_high_message;
  vdt_probe_message test_message;
  std::cout << "\nBelow is an empty probe message for testing.  Verify missing values. \n" << std::endl; 
  std::cout << test_message.probe_message_hdr() << std::endl;
  std::cout << test_message.probe_message_string() << std::endl;  

  double rec_time = vdt_test_generator::rec_time_present;
  test_message.set_obs_time(vdt_test_generator::obs_time);
  test_message.set_rec_time(rec_time);
  test_message.set_vehicle_id("test");
  test_message.set_point(boulder_point);
  rec_time += 1;

  // test below range minimum
  test_too_low_message.set_obs_time(vdt_test_generator::obs_time);
  test_too_low_message.set_rec_time(rec_time);
  test_too_low_message.set_vehicle_id("test_too_low");
  test_too_low_message.set_point(boulder_point);
  test_too_low_message.set_heading(cfg_reader.heading_min-1);
  test_too_low_message.set_speed(cfg_reader.speed_range_min-1);
  test_too_low_message.set_air_temp(cfg_reader.air_temp_min-1);
  test_too_low_message.set_bar_press(cfg_reader.bar_press_min-1);
  //test_low_message.set_brake_status(256);
  test_too_low_message.set_lights(256);
  test_too_low_message.set_wipers_front_status(256);
  test_too_low_message.set_abs(cfg_reader.abs_min-1);
  test_too_low_message.set_stab(cfg_reader.stability_control_min-1);
  test_too_low_message.set_trac(cfg_reader.traction_control_min-1);
  test_too_low_message.set_steering_angle(cfg_reader.steering_wheel_angle_min-1);
  test_too_low_message.set_yaw_rate(cfg_reader.yaw_rate_min-1);
  rec_time += 1;

  // test at range minimum
  test_low_message.set_obs_time(vdt_test_generator::obs_time);
  test_low_message.set_rec_time(rec_time);
  test_low_message.set_vehicle_id("test_low");
  test_low_message.set_point(boulder_point);
  test_low_message.set_heading(cfg_reader.heading_min);
  test_low_message.set_speed(cfg_reader.speed_range_min);
  test_low_message.set_air_temp(cfg_reader.air_temp_min);
  test_low_message.set_bar_press(cfg_reader.bar_press_min);
  //test_mid_message.set_brake_status(vdt_probe_message::BRAKE_STATUS_ALL_OFF);
  test_low_message.set_lights(vdt_probe_message::LIGHTS_ALL_LIGHTS_OFF);
  test_low_message.set_wipers_front_status(vdt_probe_message::WIPER_STATUS_NOTEQUIPPED);
  test_low_message.set_abs(cfg_reader.abs_min);
  test_low_message.set_stab(cfg_reader.stability_control_min);
  test_low_message.set_trac(cfg_reader.traction_control_min);
  test_low_message.set_steering_angle(cfg_reader.steering_wheel_angle_min);
  test_low_message.set_yaw_rate(cfg_reader.yaw_rate_min);
  
  rec_time += 1;

  // test mid range
  test_mid_message.set_obs_time(vdt_test_generator::obs_time);
  test_mid_message.set_rec_time(rec_time);
  test_mid_message.set_vehicle_id("test_mid");
  test_mid_message.set_point(boulder_point);
  test_mid_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0);
  test_mid_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0);
  test_mid_message.set_air_temp((cfg_reader.air_temp_min+cfg_reader.air_temp_max)/2.0);
  test_mid_message.set_bar_press((cfg_reader.bar_press_min+cfg_reader.bar_press_max)/2.0);
  test_mid_message.set_lights(vdt_probe_message::LIGHTS_RIGHT_TURN_SIGNAL_ON);
  test_mid_message.set_wipers_front_status(vdt_probe_message::WIPER_STATUS_LOW);
  test_mid_message.set_abs((cfg_reader.abs_min+cfg_reader.abs_max)/2.0);
  test_mid_message.set_stab((cfg_reader.stability_control_min+cfg_reader.stability_control_max)/2.0);
  test_mid_message.set_trac((cfg_reader.traction_control_min+cfg_reader.traction_control_max)/2.0);
  test_mid_message.set_steering_angle((cfg_reader.steering_wheel_angle_min+cfg_reader.steering_wheel_angle_max)/2.0);
  test_mid_message.set_yaw_rate((cfg_reader.yaw_rate_min+cfg_reader.yaw_rate_max)/2.0);
  
  rec_time += 1;

  // test range maximum
  test_high_message.set_obs_time(vdt_test_generator::obs_time);
  test_high_message.set_rec_time(rec_time);
  test_high_message.set_vehicle_id("test_high");
  test_high_message.set_point(boulder_point);
  test_high_message.set_heading(cfg_reader.heading_max);
  test_high_message.set_speed(cfg_reader.speed_range_max);
  test_high_message.set_air_temp(cfg_reader.air_temp_max);
  test_high_message.set_bar_press(cfg_reader.bar_press_max);
  // test_high_message.set_brake_status(vdt_probe_message::BRAKE_STATUS_ALL_ON);
  test_high_message.set_lights(vdt_probe_message::LIGHTS_PARKING_LIGHTS_ON);
  test_high_message.set_wipers_front_status(vdt_probe_message::WIPER_STATUS_AUTOMATICPRESENT);
  test_high_message.set_abs(cfg_reader.abs_max);
  test_high_message.set_stab(cfg_reader.stability_control_max);
  test_high_message.set_trac(cfg_reader.traction_control_max);
  test_high_message.set_steering_angle(cfg_reader.steering_wheel_angle_max);
  test_high_message.set_yaw_rate(cfg_reader.yaw_rate_max);
  

  rec_time += 1;

  // test above range maximum
  test_too_high_message.set_obs_time(vdt_test_generator::obs_time);
  test_too_high_message.set_rec_time(rec_time);
  test_too_high_message.set_vehicle_id("test_too_high");
  test_too_high_message.set_point(boulder_point);
  test_too_high_message.set_heading(cfg_reader.heading_max+1);
  test_too_high_message.set_speed(cfg_reader.speed_range_max+1);
  test_too_high_message.set_air_temp(cfg_reader.air_temp_max+1);
  test_too_high_message.set_bar_press(cfg_reader.bar_press_max+1);
  test_too_high_message.set_lights(vdt_probe_message::LIGHTS_PARKING_LIGHTS_ON + vdt_probe_message::LIGHTS_FOG_LIGHTS_ON + vdt_probe_message::LIGHTS_DAYTIME_RUNNING_LIGHTS_ON + vdt_probe_message::LIGHTS_AUTOMATIC_LIGHT_CONTROL_ON + vdt_probe_message::LIGHTS_RIGHT_TURN_SIGNAL_ON + vdt_probe_message::LIGHTS_LEFT_TURN_SIGNAL_ON + vdt_probe_message::LIGHTS_HIGH_BEAM_HEADLIGHTS_ON + vdt_probe_message::LIGHTS_LOW_BEAM_HEADLIGHTS_ON + vdt_probe_message::LIGHTS_HAZARD_SIGNAL_ON +vdt_probe_message::LIGHTS_ALL_LIGHTS_OFF);
  test_too_high_message.set_wipers_front_status(vdt_probe_message::WIPER_STATUS_AUTOMATICPRESENT + vdt_probe_message::WIPER_STATUS_WASHER + vdt_probe_message::WIPER_STATUS_HIGH + vdt_probe_message::WIPER_STATUS_LOW + vdt_probe_message::WIPER_STATUS_INTERMITTENT + vdt_probe_message::WIPER_STATUS_OFF + vdt_probe_message::WIPER_STATUS_NOTEQUIPPED );
  test_too_high_message.set_abs(cfg_reader.abs_max+1);
  test_too_high_message.set_stab(cfg_reader.stability_control_max+1);
  test_too_high_message.set_trac(cfg_reader.traction_control_max+1);
  test_too_high_message.set_steering_angle(cfg_reader.steering_wheel_angle_max+1);
  test_too_high_message.set_yaw_rate(cfg_reader.yaw_rate_max+1);

  rec_time += 1;

  // not available test
  null_test_message.set_vehicle_id("test_na"); 
  null_test_message.set_point(estes_point);
  null_test_message.set_obs_time(vdt_test_generator::obs_time);
  null_test_message.set_rec_time(rec_time);

  // messages for nvt test 
  vdt_probe_message east_boulder_message;
  east_boulder_message.set_obs_time(vdt_test_generator::obs_time);
  east_boulder_message.set_rec_time(rec_time);
  east_boulder_message.set_vehicle_id("east_boulder");
  east_boulder_message.set_point(east_boulder_point);
  east_boulder_message.set_heading(0.25 * cfg_reader.heading_min + 0.75 * cfg_reader.heading_max);
  east_boulder_message.set_speed(0.25 * cfg_reader.speed_range_min + 0.75 * cfg_reader.speed_range_max);
  east_boulder_message.set_air_temp(0.25 * cfg_reader.air_temp_min + 0.75 * cfg_reader.air_temp_max);
  east_boulder_message.set_bar_press(0.25 * cfg_reader.bar_press_min + 0.75 * cfg_reader.bar_press_max);
  rec_time += 1;

  // messages for nvt test
  vdt_probe_message east_boulder_message2;
  east_boulder_message2.set_obs_time(vdt_test_generator::obs_time);
  east_boulder_message2.set_rec_time(rec_time);
  east_boulder_message2.set_vehicle_id("east_boulder2");
  east_boulder_message2.set_point(east_boulder_point);
  east_boulder_message2.set_heading(0.75 * cfg_reader.heading_min + 0.25 * cfg_reader.heading_max);
  //east_boulder_message2.set_speed(0.75 * cfg_reader.speed_range_min + 0.25 * cfg_reader.speed_range_max);
  east_boulder_message2.set_speed(22.35);
  east_boulder_message2.set_air_temp(0.75 * cfg_reader.air_temp_min + 0.25 * cfg_reader.air_temp_max);
  east_boulder_message2.set_bar_press(0.75 * cfg_reader.bar_press_min + 0.25 * cfg_reader.bar_press_max);
  rec_time += 1;
  
  // messages for nvt test
  vdt_probe_message east_boulder_message3;
  east_boulder_message3.set_obs_time(vdt_test_generator::obs_time);
  east_boulder_message3.set_rec_time(rec_time);
  east_boulder_message3.set_vehicle_id("east_boulder3");
  east_boulder_message3.set_point(east_boulder_point);
  east_boulder_message3.set_heading(0.53 * cfg_reader.heading_min + 0.47 * cfg_reader.heading_max);
  //east_boulder_message3.set_speed(0.53 * cfg_reader.speed_range_min + 0.47 * cfg_reader.speed_range_max);
  east_boulder_message3.set_speed(22.35);
  east_boulder_message3.set_air_temp(0.53 * cfg_reader.air_temp_min + 0.47 * cfg_reader.air_temp_max);
  east_boulder_message3.set_bar_press(0.53 * cfg_reader.bar_press_min + 0.47 * cfg_reader.bar_press_max);
  rec_time += 1;
  
  // messages for nvt test
  vdt_probe_message east_boulder_message4;
  east_boulder_message4.set_obs_time(vdt_test_generator::obs_time);
  east_boulder_message4.set_rec_time(rec_time);
  east_boulder_message4.set_vehicle_id("east_boulder4");
  east_boulder_message4.set_point(east_boulder_point);
  east_boulder_message4.set_heading(0.54 * cfg_reader.heading_min + 0.46 * cfg_reader.heading_max);
  //east_boulder_message4.set_speed(0.54 * cfg_reader.speed_range_min + 0.46 * cfg_reader.speed_range_max);
  east_boulder_message4.set_speed(22.35);
  east_boulder_message4.set_air_temp(0.54 * cfg_reader.air_temp_min + 0.46 * cfg_reader.air_temp_max);
  east_boulder_message4.set_bar_press(0.54 * cfg_reader.bar_press_min + 0.46 * cfg_reader.bar_press_max);
  rec_time += 1;
  
  // current message for failing persistence
  vdt_probe_message aspen_message;
  aspen_message.set_obs_time(vdt_test_generator::obs_time);
  aspen_message.set_rec_time(rec_time);
  aspen_message.set_vehicle_id("aspen_persist_fail");
  aspen_message.set_point(aspen_point);
  aspen_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0);
  aspen_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0);
  aspen_message.set_air_temp((cfg_reader.air_temp_min+cfg_reader.air_temp_max)/2.0);
  aspen_message.set_bar_press((cfg_reader.bar_press_min+cfg_reader.bar_press_max)/2.0);
  rec_time += 1;

  // current message for passing persistence test
  vdt_probe_message estes_message;
  estes_message.set_obs_time(vdt_test_generator::obs_time);
  estes_message.set_rec_time(rec_time);
  estes_message.set_vehicle_id("estes_persist_pass");
  estes_message.set_point(estes_point);
  estes_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0 - 1.);
  estes_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0 - 1.);
  estes_message.set_air_temp((cfg_reader.air_temp_min+cfg_reader.air_temp_max)/2.0 - 1.);
  estes_message.set_dew_temp(estes_dew_temp);
  estes_message.set_bar_press((cfg_reader.bar_press_min+cfg_reader.bar_press_max)/2.0 - 1.);
  rec_time += 1;

  // current message for passing spatial iqr test
  vdt_probe_message glenwood_message;
  glenwood_message.set_obs_time(vdt_test_generator::obs_time);
  glenwood_message.set_rec_time(rec_time);
  glenwood_message.set_vehicle_id("glenwood_spatial");
  glenwood_message.set_point(glenwood_point);
  glenwood_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0 - 1.);
  glenwood_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0 - 1.);
  glenwood_message.set_air_temp(glenwood_spatial_temp);
  glenwood_message.set_air_temp2(glenwood_spatial_temp+4);
  glenwood_message.set_dew_temp(glenwood_spatial_dew_temp+3.4);
  glenwood_message.set_bar_press(vdt_test_generator::glenwood_spatial_bar_press-2);
  rec_time += 1;

  // current message for passing spatial iqr test
  vdt_probe_message boulder_barnes_message;
  boulder_barnes_message.set_obs_time(vdt_test_generator::obs_time);
  boulder_barnes_message.set_rec_time(rec_time);
  boulder_barnes_message.set_vehicle_id("boulder_barnes");
  boulder_barnes_message.set_point(boulder_point);
  boulder_barnes_message.set_heading((cfg_reader.heading_min+cfg_reader.heading_max)/2.0 - 1.);
  boulder_barnes_message.set_speed((cfg_reader.speed_range_min+cfg_reader.speed_range_max)/2.0 - 1.);
  boulder_barnes_message.set_air_temp(boulder_barnes_temp);
  boulder_barnes_message.set_air_temp2(boulder_barnes_temp+4);
  boulder_barnes_message.set_dew_temp(boulder_barnes_dew_temp+3.4);
  boulder_barnes_message.set_bar_press(vdt_test_generator::boulder_barnes_bar_press-2);
  rec_time += 1;

  // current message for failing step test
  vdt_probe_message steamboat_message;
  steamboat_message.set_obs_time(vdt_test_generator::obs_time);
  steamboat_message.set_rec_time(rec_time);
  steamboat_message.set_vehicle_id("steamboat_step_fail");
  steamboat_message.set_point(steamboat_point);
  steamboat_message.set_heading(cfg_reader.heading_max);
  steamboat_message.set_speed(cfg_reader.speed_range_max);
  steamboat_message.set_air_temp(cfg_reader.air_temp_max);
  steamboat_message.set_dew_temp(cfg_reader.air_temp_max);
  steamboat_message.set_bar_press(cfg_reader.bar_press_max);
  rec_time += 1;

  // For range check tests
  messages.push_back(null_test_message);
  messages.push_back(test_too_low_message);
  messages.push_back(test_low_message);
  messages.push_back(test_mid_message);
  messages.push_back(test_high_message);
  messages.push_back(test_too_high_message);

  // For nvt test
  messages.push_back(east_boulder_message);
  messages.push_back(east_boulder_message);
  messages.push_back(east_boulder_message);
  messages.push_back(east_boulder_message2);
  messages.push_back(east_boulder_message2);
  messages.push_back(east_boulder_message2);
  messages.push_back(east_boulder_message3);
  messages.push_back(east_boulder_message3);
  messages.push_back(east_boulder_message3);
  messages.push_back(east_boulder_message3);
  messages.push_back(east_boulder_message3);
  messages.push_back(east_boulder_message4);
  messages.push_back(east_boulder_message4);
  messages.push_back(east_boulder_message4);
  messages.push_back(east_boulder_message4);
  messages.push_back(east_boulder_message4);

  // Test for failing persistence
  messages.push_back(aspen_message);
  // Test for passing persistence test
  messages.push_back(estes_message);
  // Test for passing spatial iqr test
  messages.push_back(glenwood_message);
  // Test for failing time step test
  messages.push_back(steamboat_message);
}

double vdt_test_generator::boulder_lat = 40.0150;
double vdt_test_generator::boulder_lon = -105.2700;
double vdt_test_generator::boulder_elev = 1655;
vdt_point vdt_test_generator::boulder_point(boulder_lat, boulder_lon, boulder_elev);

double vdt_test_generator::north_boulder_lat = vdt_test_generator::boulder_lat + 0.002;
double vdt_test_generator::north_boulder_lon = vdt_test_generator::boulder_lon;
double vdt_test_generator::north_boulder_elev = boulder_elev;
vdt_point vdt_test_generator::north_boulder_point(north_boulder_lat, north_boulder_lon, north_boulder_elev);

double vdt_test_generator::east_boulder_lat = vdt_test_generator::boulder_lat;
double vdt_test_generator::east_boulder_lon = vdt_test_generator::boulder_lon + 0.002;
double vdt_test_generator::east_boulder_elev = boulder_elev;
vdt_point vdt_test_generator::east_boulder_point(east_boulder_lat, east_boulder_lon, east_boulder_elev);

double vdt_test_generator::denver_lat = 39.7392;
double vdt_test_generator::denver_lon = -104.9842;
double vdt_test_generator::denver_elev = 1609.344l;
vdt_point vdt_test_generator::denver_point(denver_lat, denver_lon, denver_elev);

double vdt_test_generator::aspen_lat = 39.1911;
double vdt_test_generator::aspen_lon = -106.8169;
double vdt_test_generator::aspen_elev = 3418;
vdt_point vdt_test_generator::aspen_point(aspen_lat, aspen_lon, aspen_elev);

double vdt_test_generator::estes_lat = 40.3772;
double vdt_test_generator::estes_lon = -105.5211;
double vdt_test_generator::estes_elev = 1609.344l;
float vdt_test_generator::estes_dew_temp = 35;
vdt_point vdt_test_generator::estes_point(estes_lat, estes_lon, estes_elev);

double vdt_test_generator::glenwood_lat = 39.5506;
double vdt_test_generator::glenwood_lon = -107.3242;
double vdt_test_generator::glenwood_elev = 1756;
vdt_point vdt_test_generator::glenwood_point(glenwood_lat, glenwood_lon, glenwood_elev);
float vdt_test_generator::glenwood_spatial_temp = 30;
float vdt_test_generator::glenwood_spatial_dew_temp = 35;
float vdt_test_generator::glenwood_spatial_bar_press = 1013;

float vdt_test_generator::boulder_barnes_temp = 30;
float vdt_test_generator::boulder_barnes_dew_temp = 35;
float vdt_test_generator::boulder_barnes_bar_press = 1013;

double vdt_test_generator::steamboat_lat = 40.4528;
double vdt_test_generator::steamboat_lon = -106.7731;
double vdt_test_generator::steamboat_elev = 2100;
vdt_point vdt_test_generator::steamboat_point(steamboat_lat, steamboat_lon, steamboat_elev);

string vdt_test_generator::begin_time_string = "100000";
time_t vdt_test_generator::begin_time = 100000;
time_t vdt_test_generator::end_time = 109000;
string vdt_test_generator::end_time_string = "109000";
double vdt_test_generator::obs_time = 100000;
double vdt_test_generator::rec_time_present = vdt_test_generator::begin_time;
double vdt_test_generator::rec_time_past = 1;

