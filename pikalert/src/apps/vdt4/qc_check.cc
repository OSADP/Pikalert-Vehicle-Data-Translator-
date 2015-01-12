//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: qc_check.cc,v $
//       Version: $Revision: 1.13 $  Dated: $Date: 2014/06/15 20:54:36 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/qc_check.cc
 *
 *   Quality control check class implementation
 *
 *
 */

// Include files
#include <iostream>
#include <log/log.hh>
#include "qc_check.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt4/vdt_surface_observation.hh"
#include <stdio.h>
#include <time.h>
#include <math.h>

// Constants and macros

// Types

// Structures, unions and classes

// Global variables
extern Log *Logg;
extern int Debug_level;

// Functions

int get_month_offset(double obstime)
{
  struct tm * timeinfo;
  const time_t obs_time = (time_t)obstime;

  timeinfo = localtime(&obs_time);

  return (timeinfo->tm_mon);

}

unsigned char range_check( double min, double max, double value)
{
  if ( min <= value && value <= max )
    {
      return vdt_probe_message_qc::PASSED;
    }

  if ( fabs( value - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS )
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  return vdt_probe_message_qc::FAILED;
}

unsigned char range_check( short min, short max, short value)
{
  if ( min <= value && value <= max )
    {
      return vdt_probe_message_qc::PASSED;
    }

  if ( value == vdt_const::SHORT_FILL_VALUE )
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  return vdt_probe_message_qc::FAILED;
}

unsigned char climate_range_check(double min, double max, double value)
{
  unsigned char range_check_result;

  if ( ( fabs( min - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS ) || ( fabs( max - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS ) )
    {
      range_check_result = vdt_probe_message_qc::NOT_APPLICABLE;
    }
  else
    {
      range_check_result = range_check(min, max, value);
    }

  return range_check_result;
}

unsigned char climate_range_check(short min, short max, short value)
{
  unsigned char range_check_result;

  if ( ( min == vdt_const::SHORT_FILL_VALUE ) || ( max == vdt_const::SHORT_FILL_VALUE ) )
    {
      range_check_result = vdt_probe_message_qc::NOT_APPLICABLE;
    }
  else
    {
      range_check_result = range_check(min, max, value);
    }

  return range_check_result;
}

qc_check::qc_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_dataset) : cfg_reader(config_reader_arg), climate_data(climate_data), datasets(probe_message_dataset)
{
}

void speed_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.speed_range_min, cfg_reader.speed_range_max, probe_message.get_speed());

  probe_message.set_speed_range_qc_passed(range_check_result);
}

void air_temp_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.air_temp_min, cfg_reader.air_temp_max, probe_message.get_air_temp());

  probe_message.set_air_temp_range_qc_passed(range_check_result);
}

void air_temp2_range_check::operator()(vdt_probe_message_qc &probe_message) const
{

  unsigned char range_check_result = range_check(float(cfg_reader.air_temp_min), float(cfg_reader.air_temp_max), probe_message.get_air_temp2());

  probe_message.set_air_temp2_range_qc_passed(range_check_result);
}

void dew_temp_range_check::operator()(vdt_probe_message_qc &probe_message) const
{

  unsigned char range_check_result = range_check(float(cfg_reader.air_temp_min), float(cfg_reader.air_temp_max), probe_message.get_dew_temp());

  probe_message.set_dew_temp_range_qc_passed(range_check_result);
}

void surface_temp_range_check::operator()(vdt_probe_message_qc &probe_message) const
{

  unsigned char range_check_result = range_check(float(cfg_reader.air_temp_min), float(cfg_reader.air_temp_max), probe_message.get_surface_temp());
  probe_message.set_surface_temp_range_qc_passed(range_check_result);
}

void bar_press_range_check::operator()(vdt_probe_message_qc &probe_message) const
{

  unsigned char range_check_result = range_check(cfg_reader.bar_press_min, cfg_reader.bar_press_max, probe_message.get_bar_press());

  probe_message.set_bar_press_range_qc_passed(range_check_result);
}

void brake_status_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  short status = probe_message.get_brake_status();

  unsigned char range_check_result = range_check(cfg_reader.brake_status_min, cfg_reader.brake_status_max, status);

  if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      if ( status == vdt_probe_message::BRAKE_STATUS_ALL_OFF ||
           status == vdt_probe_message::BRAKE_STATUS_LF_ACTIVE  ||
           status == vdt_probe_message::BRAKE_STATUS_LR_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_RF_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_RR_ACTIVE ||
           status == vdt_probe_message::BRAKE_STATUS_ALL_ON )
        {
          probe_message.set_brake_status_range_qc_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
          probe_message.set_brake_status_range_qc_passed(vdt_probe_message_qc::FAILED);
        }
    }
  else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      probe_message.set_brake_status_range_qc_passed(vdt_probe_message_qc::FAILED);
    }
  else
    {
      probe_message.set_brake_status_range_qc_passed(range_check_result);
    }
}

void brake_boost_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.brake_boost_min, cfg_reader.brake_boost_max, probe_message.get_brake_boost());

  probe_message.set_brake_boost_range_qc_passed(range_check_result);
}

void wiper_status_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  short status = probe_message.get_wipers_front_status();

  unsigned char range_check_result = range_check(cfg_reader.wiper_status_min, cfg_reader.wiper_status_max, status);

  if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      if ( status == vdt_probe_message::NOT_EQUIPPED ||
           status == vdt_probe_message::OFF  ||
           status == vdt_probe_message::INTERMITTENT ||
           status == vdt_probe_message::LOW ||
           status == vdt_probe_message::HIGH ||
           status == vdt_probe_message::WASHER ||
           status == vdt_probe_message::AUTOMATICPRESENT)

        {
          probe_message.set_wipers_front_status_range_qc_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
          probe_message.set_wipers_front_status_range_qc_passed(vdt_probe_message_qc::FAILED);
        }

    }
  else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      probe_message.set_wipers_front_status_range_qc_passed(vdt_probe_message_qc::FAILED);
    }
  else
    {
      probe_message.set_wipers_front_status_range_qc_passed(range_check_result);
    }
}

void traction_control_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.traction_control_min, cfg_reader.traction_control_max, probe_message.get_trac());

  probe_message.set_trac_range_qc_passed(range_check_result);
}

void stability_control_range_check::operator()(vdt_probe_message_qc &probe_message) const
{

  unsigned char range_check_result = range_check(cfg_reader.stability_control_min, cfg_reader.stability_control_max, probe_message.get_stab());

  probe_message.set_stab_range_qc_passed(range_check_result);
}

void abs_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.abs_min, cfg_reader.abs_max, probe_message.get_abs());

  probe_message.set_abs_range_qc_passed(range_check_result);
}

void headlights_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  short status =  probe_message.get_lights();

  unsigned char range_check_result = range_check(cfg_reader.headlights_min, cfg_reader.headlights_max, status);

  if (range_check_result == vdt_probe_message_qc::PASSED)
    {
      probe_message.set_lights_range_qc_passed(vdt_probe_message_qc::PASSED);
    }
  else if (range_check_result == vdt_probe_message_qc::FAILED)
    {
      probe_message.set_lights_range_qc_passed(vdt_probe_message_qc::FAILED);
    }
  else
    {
      probe_message.set_lights_range_qc_passed(range_check_result);
    }
}

void yaw_rate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.yaw_rate_min, cfg_reader.yaw_rate_max, probe_message.get_yaw());

  probe_message.set_yaw_range_qc_passed(range_check_result);
}

void hoz_accel_lat_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.hoz_accel_lat_min, cfg_reader.hoz_accel_lat_max, probe_message.get_hoz_accel_lat());

  probe_message.set_hoz_accel_lat_range_qc_passed(range_check_result);
}

void hoz_accel_lon_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.hoz_accel_lon_min, cfg_reader.hoz_accel_lon_max, probe_message.get_hoz_accel_long());

  probe_message.set_hoz_accel_long_range_qc_passed(range_check_result);
}

void steering_wheel_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.steering_wheel_angle_min, cfg_reader.steering_wheel_angle_max, probe_message.get_steering_angle());

  probe_message.set_steering_angle_range_qc_passed(range_check_result);
}

void steering_rate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.steering_rate_min, cfg_reader.steering_rate_max, probe_message.get_steering_rate());

  probe_message.set_steering_rate_range_qc_passed(range_check_result);
}

void heading_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.heading_min, cfg_reader.heading_max, probe_message.get_heading());

  probe_message.set_heading_range_qc_passed(range_check_result);
}

void air_temp_climate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Get month, latitude, and longitude offsets
  double lat_offset, lon_offset;
  short min_value, max_value;
  int month_offset = get_month_offset(probe_message.get_obs_time());
  climate_data->latlon2xy(probe_message.get_latitude(), probe_message.get_longitude(), &lon_offset, &lat_offset);

  // Get min/max values for given offsets
  int lat_vec_offset;
  int lon_vec_offset;
  if (climate_data->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, climate_data->get_min_air_temperature_grid(), climate_data->get_max_air_temperature_grid(), &min_value, &max_value);
    }

  unsigned char range_check_result = climate_range_check(min_value, max_value, probe_message.get_air_temp());

  probe_message.set_air_temp_crt_passed(range_check_result);
}

void air_temp2_climate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Get month, latitude, and longitude offsets
  double lat_offset, lon_offset;
  float min_value, max_value;
  int month_offset = get_month_offset(probe_message.get_obs_time());
  climate_data->latlon2xy(probe_message.get_latitude(), probe_message.get_longitude(), &lon_offset, &lat_offset);

  // Get min/max values for given offsets
  int lat_vec_offset;
  int lon_vec_offset;
  if (climate_data->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, climate_data->get_min_air_temperature_grid(), climate_data->get_max_air_temperature_grid(), &min_value, &max_value);
    }

  unsigned char range_check_result = climate_range_check(min_value, max_value, probe_message.get_air_temp2());

  probe_message.set_air_temp2_crt_passed(range_check_result);
}

void dew_temp_climate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Get month, latitude, and longitude offsets
  double lat_offset, lon_offset;
  float min_value, max_value;
  int month_offset = get_month_offset(probe_message.get_obs_time());
  climate_data->latlon2xy(probe_message.get_latitude(), probe_message.get_longitude(), &lon_offset, &lat_offset);

  // Get min/max values for given offsets
  int lat_vec_offset;
  int lon_vec_offset;
  if (climate_data->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, climate_data->get_min_dewpoint_temperature_grid(), climate_data->get_max_dewpoint_temperature_grid(), &min_value, &max_value);
    }

  unsigned char range_check_result = climate_range_check(min_value, max_value, probe_message.get_dew_temp());

  probe_message.set_dew_temp_crt_passed(range_check_result);
}

void surface_temp_climate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Get month, latitude, and longitude offsets
  double lat_offset, lon_offset;
  float min_value, max_value;
  int month_offset = get_month_offset(probe_message.get_obs_time());
  climate_data->latlon2xy(probe_message.get_latitude(), probe_message.get_longitude(), &lon_offset, &lat_offset);

  // Get min/max values for given offsets
  int lat_vec_offset;
  int lon_vec_offset;
  if (climate_data->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, climate_data->get_min_surface_temperature_grid(), climate_data->get_max_surface_temperature_grid(), &min_value, &max_value);
    }

  unsigned char range_check_result = climate_range_check(min_value, max_value, probe_message.get_surface_temp());

  probe_message.set_surface_temp_crt_passed(range_check_result);
}

void bar_press_climate_range_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Get month, latitude, and longitude offsets
  double lat_offset, lon_offset;
  short min_value, max_value;
  int month_offset = get_month_offset(probe_message.get_obs_time());
  climate_data->latlon2xy(probe_message.get_latitude(), probe_message.get_longitude(), &lon_offset, &lat_offset);

  // Get min/max values for given offsets
  int lat_vec_offset;
  int lon_vec_offset;
  if (climate_data->round_to_integer_coords(lat_offset, lon_offset, &lat_vec_offset, &lon_vec_offset) == 0)
    {
      climate_data->get_min_max_climate_values(month_offset, lat_vec_offset, lon_vec_offset, climate_data->get_min_atmospheric_pressure_grid(), climate_data->get_max_atmospheric_pressure_grid(), &min_value, &max_value);
    }

  unsigned char range_check_result = climate_range_check(min_value, max_value, probe_message.get_bar_press());

  probe_message.set_bar_press_crt_passed(range_check_result);
}

void latitude_data_filtering_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.latitude_min, cfg_reader.latitude_max, probe_message.get_latitude());

  probe_message.set_latitude_dft_passed(range_check_result);
}

void longitude_data_filtering_check::operator()(vdt_probe_message_qc &probe_message) const
{
  unsigned char range_check_result = range_check(cfg_reader.longitude_min, cfg_reader.longitude_max, probe_message.get_longitude());

  probe_message.set_longitude_dft_passed(range_check_result);
}

void iqr_spatial_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // get surface obs "close" to probe message location
  double obs_time = probe_message.get_obs_time();
  vector<vdt_surface_observation> near;

  // get nearby surface stations
  datasets->get_near_surface_obs(probe_message.get_point(), obs_time, cfg_reader.spatial_range_search_km, cfg_reader.spatial_time_search_secs, near);

  // extract air temperature, dew temperature, surface temperature and pressure readings from near observations
  vector<float> air_temp;
  vector<float> dew_temp;
  vector<float> surface_temp;
  vector<float> pressure;

  //printf("#in iqr_spatial_check near.size() = %ld\n", near.size());
  // perform iqr spatial tests
  for (size_t i=0; i<near.size(); i++)
    {
      //printf("id, pressure, air temp, dew temp, surface temp values for spatial: %s, %f, %f, %f, %f\n", probe_message.vehicle_id.c_str(), near[i].get_press(), near[i].get_air_temp(), near[i].get_dew_temp(), near[i].get_surface_temp());
      pressure.push_back(near[i].get_press());
      air_temp.push_back(near[i].get_air_temp());
      dew_temp.push_back(near[i].get_dew_temp());
      surface_temp.push_back(near[i].get_surface_temp());
    }

  //printf("#pressure iqr_spatial_check\n");
  int iqr_pressure_test = pressure_spatial_check.perform_iqr(pressure, probe_message.get_bar_press());
  //printf("#result: %d\n", iqr_pressure_test);
  //printf("#air temp iqr_spatial_check\n");
  int iqr_air_temp_test = air_temp_spatial_check.perform_iqr(air_temp, probe_message.get_air_temp());
  //printf("#result: %d\n", iqr_air_temp_test);
  //printf("#air temp2 iqr_spatial_check\n");
  int iqr_air_temp2_test = air_temp_spatial_check.perform_iqr(air_temp, probe_message.get_air_temp2());
  //printf("#result: %d\n", iqr_air_temp2_test);
  //printf("#dew temp iqr_spatial_check\n");
  int iqr_dew_temp_test = dew_temp_spatial_check.perform_iqr(dew_temp, probe_message.get_dew_temp());
  //printf("#result: %d\n", iqr_dew_temp_test);
  //printf("#surface temp iqr_spatial_check\n");
  int iqr_surface_temp_test = surface_temp_spatial_check.perform_iqr(surface_temp, probe_message.get_surface_temp());
  //printf("#result: %d\n", iqr_surface_temp_test);

  probe_message.set_bar_press_spatial_iqr_qc_passed(iqr_pressure_test);
  probe_message.set_air_temp_spatial_iqr_qc_passed(iqr_air_temp_test);
  probe_message.set_air_temp2_spatial_iqr_qc_passed(iqr_air_temp2_test);
  probe_message.set_dew_temp_spatial_iqr_qc_passed(iqr_dew_temp_test);
  probe_message.set_surface_temp_spatial_iqr_qc_passed(iqr_surface_temp_test);
  
  // prepare for running barnes spatial checks
  if (iqr_pressure_test == vdt_probe_message_qc::NOT_APPLICABLE || iqr_air_temp_test == vdt_probe_message_qc::NOT_APPLICABLE || iqr_air_temp2_test == vdt_probe_message_qc::NOT_APPLICABLE || iqr_dew_temp_test == vdt_probe_message_qc::NOT_APPLICABLE || iqr_surface_temp_test == vdt_probe_message_qc::NOT_APPLICABLE)
    {
      vector<vdt_point> surface_obs_points;
      vdt_surface_observation::get_points(near, surface_obs_points);
      vector<float> distances;

      vdt_spatial_check_barnes::calc_distances(probe_message.get_point(), surface_obs_points, distances);

      // perform Barnes pressure test if iqr pressure test cannot be applied
      if (iqr_pressure_test == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);
	  probe_message.set_bar_press_spatial_barnes_qc_passed(spatial_check_barnes.perform_barnes(pressure, distances, probe_message.get_bar_press()));
	}

      // perform Barnes air temp test if iqr air temp test cannot be applied
      if (iqr_air_temp_test == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);
	  probe_message.set_air_temp_spatial_barnes_qc_passed(spatial_check_barnes.perform_barnes(air_temp, distances, probe_message.get_air_temp()));
	}

      // perform Barnes air temp2 test if iqr air temp test cannot be applied
      if (iqr_air_temp2_test == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);
	  probe_message.set_air_temp2_spatial_barnes_qc_passed(spatial_check_barnes.perform_barnes(air_temp, distances, probe_message.get_air_temp2()));
	}

      // perform Barnes dew temp test if iqr dew temp test cannot be applied
      if (iqr_dew_temp_test == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);
	  probe_message.set_dew_temp_spatial_barnes_qc_passed(spatial_check_barnes.perform_barnes(dew_temp, distances, probe_message.get_dew_temp()));
	}

      // perform Barnes surface temp test if iqr surface temp test cannot be applied
      if (iqr_surface_temp_test == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);
	  probe_message.set_surface_temp_spatial_barnes_qc_passed(spatial_check_barnes.perform_barnes(surface_temp, distances, probe_message.get_surface_temp()));
	}
    }
}

void vdt_neighbor_check_iqr::operator()(int id, vdt_probe_message_qc &probe_message) const
{

  auto itr = seg_index_offset_map.find(id);
  if (itr == seg_index_offset_map.end())
    {
      return;
    }
  int offset = itr->second;

  // get probe message iqr values on same segment as probe message
  float air_temp_iqr25 = road_seg_stats[offset].air_temp_iqr25;
  float air_temp_iqr75 = road_seg_stats[offset].air_temp_iqr75;
  float air_temp_median = road_seg_stats[offset].air_temp_median;
  int num_air_temp_values = road_seg_stats[offset].num_msg_valid_air_temp;

  int iqr_air_temp_test = air_temp_spatial_check.perform_iqr(num_air_temp_values, air_temp_iqr25, air_temp_iqr75, air_temp_median, probe_message.get_air_temp());
  probe_message.set_air_temp_nvt_passed(iqr_air_temp_test);

  float air_temp2_iqr25 = road_seg_stats[offset].air_temp2_iqr25;
  float air_temp2_iqr75 = road_seg_stats[offset].air_temp2_iqr75;
  float air_temp2_median = road_seg_stats[offset].air_temp2_median;
  int num_air_temp2_values = road_seg_stats[offset].num_msg_valid_air_temp2;
  int iqr_air_temp2_test = air_temp_spatial_check.perform_iqr(num_air_temp2_values, air_temp2_iqr25, air_temp2_iqr75, air_temp2_median, probe_message.get_air_temp2());
  probe_message.set_air_temp2_nvt_passed(iqr_air_temp2_test);

  float dew_temp_iqr25 = road_seg_stats[offset].dew_temp_iqr25;
  float dew_temp_iqr75 = road_seg_stats[offset].dew_temp_iqr75;
  float dew_temp_median = road_seg_stats[offset].dew_temp_median;
  int num_dew_temp_values = road_seg_stats[offset].num_msg_valid_dew_temp;

  int iqr_dew_temp_test = dew_temp_spatial_check.perform_iqr(num_dew_temp_values, dew_temp_iqr25, dew_temp_iqr75, dew_temp_median, probe_message.get_dew_temp());
  probe_message.set_dew_temp_nvt_passed(iqr_dew_temp_test);

  float surface_temp_iqr25 = road_seg_stats[offset].surface_temp_iqr25;
  float surface_temp_iqr75 = road_seg_stats[offset].surface_temp_iqr75;
  float surface_temp_median = road_seg_stats[offset].surface_temp_median;
  int num_surface_temp_values = road_seg_stats[offset].num_msg_valid_surface_temp;

  int iqr_surface_temp_test = surface_temp_spatial_check.perform_iqr(num_surface_temp_values, surface_temp_iqr25, surface_temp_iqr75, surface_temp_median, probe_message.get_surface_temp());
  probe_message.set_surface_temp_nvt_passed(iqr_surface_temp_test);

  float bar_press_iqr25 = road_seg_stats[offset].bar_press_iqr25;
  float bar_press_iqr75 = road_seg_stats[offset].bar_press_iqr75;
  float bar_press_median = road_seg_stats[offset].bar_press_median;
  int num_bar_press_values = road_seg_stats[offset].num_msg_valid_bar_press;
  int iqr_bar_press_test = bar_press_spatial_check.perform_iqr(num_bar_press_values, bar_press_iqr25, bar_press_iqr75, bar_press_median, probe_message.get_bar_press());
  probe_message.set_bar_press_nvt_passed(iqr_bar_press_test);
}

void vdt_model_check::operator()(int id, vdt_probe_message_qc &probe_message) const
{
  // future approach
  // get model values on same segment as probe message
  // float model_air_temp = road_seg_stats[offset].model_air_temp;
  // float model_dewpoint = road_seg_stats[offset].model_dewpoint;
  // float model_bar_press = road_seg_stats[offset].model_bar_press;

  float model_dewpoint = probe_message.get_model_dewpoint();
  float model_air_temp = probe_message.get_model_air_temp();
  float model_bar_press = probe_message.get_model_bar_press();
  
  float bar_press = probe_message.get_bar_press();
  float air_temp = probe_message.get_air_temp();
  float air_temp2 = probe_message.get_air_temp2();
  float dew_temp = probe_message.get_dew_temp();  

  if (model_bar_press != vdt_const::FILL_VALUE)
    {
      if (bar_press == vdt_const::FILL_VALUE)
	{
          probe_message.set_bar_press_mat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	}
      else if ((model_bar_press - model_bar_press_delta) <= bar_press && bar_press <= (model_bar_press + model_bar_press_delta))
        {
          probe_message.set_bar_press_mat_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
          probe_message.set_bar_press_mat_passed(vdt_probe_message_qc::FAILED);
        }
    }

  if (model_air_temp != vdt_const::FILL_VALUE)
    {
      if (air_temp == vdt_const::FILL_VALUE)
	{
          probe_message.set_air_temp_mat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	}
      else if ((model_air_temp - model_air_temp_delta) <= air_temp && air_temp <= (model_air_temp + model_air_temp_delta))
        {
          probe_message.set_air_temp_mat_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
          probe_message.set_air_temp_mat_passed(vdt_probe_message_qc::FAILED);
        }
    }

  if (model_air_temp != vdt_const::FILL_VALUE)
    {
      if (air_temp2 == vdt_const::FILL_VALUE)
	{
          probe_message.set_air_temp2_mat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	}
      else if ((model_air_temp - model_air_temp_delta) <= air_temp2 && air_temp2 <= (model_air_temp + model_air_temp_delta))
        {
          probe_message.set_air_temp2_mat_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
          probe_message.set_air_temp2_mat_passed(vdt_probe_message_qc::FAILED);
        }
    }

  if (model_dewpoint != vdt_const::FILL_VALUE)
    {
      //printf("dew_temp, model_dewpoint: %f, %f\n", dew_temp, model_dewpoint);
      if (dew_temp == vdt_const::FILL_VALUE)
	{
	  //printf("dew_temp NA\n");
          probe_message.set_dew_temp_mat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	}
      else if ((model_dewpoint - model_dewpoint_delta) <= dew_temp && dew_temp <= (model_dewpoint + model_dewpoint_delta))
        {
	  //printf("dew_temp passed\n");
          probe_message.set_dew_temp_mat_passed(vdt_probe_message_qc::PASSED);
        }
      else
        {
	  //printf("dew_temp failed\n");
          probe_message.set_dew_temp_mat_passed(vdt_probe_message_qc::FAILED);
        }
    }

 }

void vdt_stdev_check::operator()(int id, vdt_probe_message_qc &probe_message) const
{
  auto itr = seg_index_offset_map.find(id);
  if (itr == seg_index_offset_map.end())
    {
      return;
    }
  int offset = itr->second;

  // get stdev values on same segment as probe message
  float air_temp_mean = road_seg_stats[offset].air_temp_mean;
  float air_temp_stdev = road_seg_stats[offset].air_temp_stdev;
  float air_temp2_mean = road_seg_stats[offset].air_temp2_mean;
  float air_temp2_stdev = road_seg_stats[offset].air_temp2_stdev;
  float dew_temp_mean = road_seg_stats[offset].dew_temp_mean;
  float dew_temp_stdev = road_seg_stats[offset].dew_temp_stdev;
  float surface_temp_mean = road_seg_stats[offset].surface_temp_mean;
  float surface_temp_stdev = road_seg_stats[offset].surface_temp_stdev;
  float bar_press_mean = road_seg_stats[offset].bar_press_mean;
  float bar_press_stdev = road_seg_stats[offset].bar_press_stdev;

  float bar_press = probe_message.get_bar_press();
  float air_temp = probe_message.get_air_temp();
  float air_temp2 = probe_message.get_air_temp2();
  float dew_temp = probe_message.get_dew_temp();
  float surface_temp = probe_message.get_surface_temp();

  float delta_temp = cfg_reader.air_temp_dev_mult * air_temp_stdev;
  float delta_temp2 = cfg_reader.air_temp_dev_mult * air_temp2_stdev;
  float delta_dew_temp = cfg_reader.dew_temp_dev_mult * dew_temp_stdev;
  float delta_surface_temp = cfg_reader.surface_temp_dev_mult * surface_temp_stdev;
  float delta_bar_press = cfg_reader.bar_press_dev_mult * bar_press_stdev;
  
  if (air_temp == vdt_const::FILL_VALUE)
    {
      probe_message.set_air_temp_sdt_passed(vdt_probe_message_qc::NOT_APPLICABLE);
    }
  else if ((air_temp_mean - delta_temp) <= air_temp && air_temp <= (air_temp_mean + delta_temp))
    {
      probe_message.set_air_temp_sdt_passed(vdt_probe_message_qc::PASSED);
    }
  else
    {
      probe_message.set_air_temp_sdt_passed(vdt_probe_message_qc::FAILED);
    }

  if (air_temp2 == vdt_const::FILL_VALUE)
    {
      probe_message.set_air_temp2_sdt_passed(vdt_probe_message_qc::NOT_APPLICABLE);
    }
  else if ((air_temp2_mean - delta_temp2) <= air_temp2 && air_temp2 <= (air_temp2_mean + delta_temp2))
    {
      probe_message.set_air_temp2_sdt_passed(vdt_probe_message_qc::PASSED);
    }
  else
    {
      probe_message.set_air_temp2_sdt_passed(vdt_probe_message_qc::FAILED);
    }

  if (dew_temp == vdt_const::FILL_VALUE)
    {
      probe_message.set_dew_temp_sdt_passed(vdt_probe_message_qc::NOT_APPLICABLE);
    }
  else if ((dew_temp_mean - delta_dew_temp) <= dew_temp && dew_temp <= (dew_temp_mean + delta_dew_temp))
    {
      probe_message.set_dew_temp_sdt_passed(vdt_probe_message_qc::PASSED);
    }
  else
    {
      probe_message.set_dew_temp_sdt_passed(vdt_probe_message_qc::FAILED);
    }

  if (surface_temp == vdt_const::FILL_VALUE)
    {
      probe_message.set_surface_temp_sdt_passed(vdt_probe_message_qc::NOT_APPLICABLE);
    }
  else if ((surface_temp_mean - delta_surface_temp) <= surface_temp && surface_temp <= (surface_temp_mean + delta_surface_temp))
    {
      probe_message.set_surface_temp_sdt_passed(vdt_probe_message_qc::PASSED);
    }
  else
    {
      probe_message.set_surface_temp_sdt_passed(vdt_probe_message_qc::FAILED);
    }

  if (bar_press == vdt_const::FILL_VALUE)
    {
      probe_message.set_bar_press_sdt_passed(vdt_probe_message_qc::NOT_APPLICABLE);
    }
  else if ((bar_press_mean - delta_bar_press) <= bar_press && bar_press <= (bar_press_mean + delta_bar_press))
    {
      probe_message.set_bar_press_sdt_passed(vdt_probe_message_qc::PASSED);
    }
  else
    {
      probe_message.set_bar_press_sdt_passed(vdt_probe_message_qc::FAILED);
    }
}

void vdt_combined_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Note that the combined algorithm test by default is set to
  // NOT_APPLICABLE in the vdt_probe_message_qc constructor
  //
  // For air_temp, air_temp2, bar_pressure, dew_temp:
  // if range_qc_passed = 0, then cat = 0
  //  else:
  // cat = 0.1*step+0.2*spatial+0.15*SDT+0.1*persistence+0.2*NVT+0.15*MAT+0.1*CRT
  //
  // For surface_temp:
  // if range_qc_passed = 0, then cat = 0
  //    else:
  // cat = 0.1*step+0.2*spatial+0.15*persistence+0.15*CRT 
  // 
  
  enum TESTS {STEP, SPATIAL, PERSIST, CRT, TEST_LEN};

  probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_air_temp2_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_air_temp2_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_air_temp2_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_air_temp2_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_air_temp2_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_air_temp2_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}

      if (probe_message.get_air_temp2_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_air_temp2_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_air_temp2_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_air_temp2_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.air_temp2_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.air_temp2_cat_confidence = ratio;
	}
    }

  probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_air_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_air_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_air_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_air_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_air_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_air_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}

      if (probe_message.get_air_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_air_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_air_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_air_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.air_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.air_temp_cat_confidence = ratio;
	}
    }

  probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_dew_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_dew_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_dew_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_dew_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_dew_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_dew_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}

      if (probe_message.get_dew_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_dew_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_dew_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_dew_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.dew_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.dew_temp_cat_confidence = ratio;
	}
    }

  probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_bar_press_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_bar_press_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_bar_press_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_bar_press_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_bar_press_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_bar_press_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}

      if (probe_message.get_bar_press_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_bar_press_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_bar_press_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_bar_press_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.bar_press_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.bar_press_cat_confidence = ratio;
	}
    }

  probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_surface_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_surface_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_surface_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_surface_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_surface_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_surface_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}

      if (probe_message.get_surface_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_surface_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_surface_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_surface_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.surface_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.surface_temp_cat_confidence = ratio;
	}
    }
}

#ifdef NOTNOW

// Original combined test  that uses data from segment statistics
// Need to move ahead of segment statistics test
void vdt_combined_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Note that the combined algorithm test by default is set to
  // NOT_APPLICABLE in the vdt_probe_message_qc constructor
  //
  // For air_temp, air_temp2, bar_pressure, dew_temp:
  // if range_qc_passed = 0, then cat = 0
  //  else:
  // cat = 0.1*step+0.2*spatial+0.15*SDT+0.1*persistence+0.2*NVT+0.15*MAT+0.1*CRT
  //
  // For surface_temp:
  // if range_qc_passed = 0, then cat = 0
  //    else:
  // cat = 0.1*step+0.2*spatial+0.2*SDT+0.15*persistence+0.2*NVT+0.15*CRT 
  // 
  
  enum TESTS {STEP, SPATIAL, SDT, PERSIST, NVT, CRT, TEST_LEN};

  probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_air_temp2_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_air_temp2_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_air_temp2_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_air_temp2_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_air_temp2_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_air_temp2_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}
      if (probe_message.get_air_temp2_sdt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SDT] = 1;
	}
      else if (probe_message.get_air_temp2_sdt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[SDT] = 0;
	}

      if (probe_message.get_air_temp2_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_air_temp2_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_air_temp2_nvt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[NVT] = 1;
	}
      else if (probe_message.get_air_temp2_nvt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[NVT] = 0;
	}

      if (probe_message.get_air_temp2_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_air_temp2_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.air_temp2_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_air_temp2_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.air_temp2_cat_confidence = ratio;
	}
    }

  probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_air_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_air_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_air_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_air_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_air_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_air_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}
      if (probe_message.get_air_temp_sdt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SDT] = 1;
	}
      else if (probe_message.get_air_temp_sdt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[SDT] = 0;
	}

      if (probe_message.get_air_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_air_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_air_temp_nvt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[NVT] = 1;
	}
      else if (probe_message.get_air_temp_nvt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[NVT] = 0;
	}

      if (probe_message.get_air_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_air_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.air_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_air_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.air_temp_cat_confidence = ratio;
	}
    }

  probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_dew_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_dew_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_dew_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_dew_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_dew_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_dew_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}
      if (probe_message.get_dew_temp_sdt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SDT] = 1;
	}
      else if (probe_message.get_dew_temp_sdt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[SDT] = 0;
	}

      if (probe_message.get_dew_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_dew_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_dew_temp_nvt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[NVT] = 1;
	}
      else if (probe_message.get_dew_temp_nvt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[NVT] = 0;
	}

      if (probe_message.get_dew_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_dew_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.dew_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_dew_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.dew_temp_cat_confidence = ratio;
	}
    }

  probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_bar_press_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_bar_press_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_bar_press_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_bar_press_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_bar_press_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_bar_press_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}
      if (probe_message.get_bar_press_sdt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SDT] = 1;
	}
      else if (probe_message.get_bar_press_sdt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[SDT] = 0;
	}

      if (probe_message.get_bar_press_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_bar_press_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_bar_press_nvt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[NVT] = 1;
	}
      else if (probe_message.get_bar_press_nvt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[NVT] = 0;
	}

      if (probe_message.get_bar_press_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_bar_press_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.bar_press_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_bar_press_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.bar_press_cat_confidence = ratio;
	}
    }

  probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::FAILED);

  if (probe_message.get_surface_temp_range_qc_passed() != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      if (probe_message.get_surface_temp_step_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[STEP] = 1;
	}
      else if (probe_message.get_surface_temp_step_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[STEP] = 0;
	}

      if (probe_message.get_surface_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
      else if (probe_message.get_surface_temp_spatial_iqr_qc_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  if (probe_message.get_surface_temp_spatial_barnes_qc_passed() == vdt_probe_message_qc::PASSED)
	    {
	      tests[SPATIAL] = 1;
	    }
	  else
	    {
	      weights[SPATIAL] = 0;
	    }
	}
      if (probe_message.get_surface_temp_sdt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[SDT] = 1;
	}
      else if (probe_message.get_surface_temp_sdt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[SDT] = 0;
	}

      if (probe_message.get_surface_temp_persist_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[PERSIST] = 1;
	}
      else if (probe_message.get_surface_temp_persist_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[PERSIST] = 0;
	}

      if (probe_message.get_surface_temp_nvt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[NVT] = 1;
	}
      else if (probe_message.get_surface_temp_nvt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[NVT] = 0;
	}

      if (probe_message.get_surface_temp_crt_passed() == vdt_probe_message_qc::PASSED)
	{
	  tests[CRT] = 1;
	}
      else if (probe_message.get_surface_temp_crt_passed() == vdt_probe_message_qc::NOT_APPLICABLE)
	{
	  weights[CRT] = 0;
	}

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
	{
	  sum_weights += weights[i];
	  sum_tests += weights[i] * tests[i];
	}
      if (sum_weights == 0)
	{
	  probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::NOT_APPLICABLE);
	  probe_message.surface_temp_cat_confidence = vdt_probe_message_qc::BAD_OR_MISSING_VALUE;
	}
      else
	{
	  ratio = sum_tests/sum_weights;
	  if (ratio >= cfg_reader.combined_algorithm_threshold)
	    {
	      probe_message.set_surface_temp_cat_passed(vdt_probe_message_qc::PASSED);
	    }

	  probe_message.surface_temp_cat_confidence = ratio;
	}
    }
}
#endif
#ifdef NOTNOW

      //if speed is less than 25 mph = 11.175 m/s, then set air_temp_qc_passed to false
      //Found out that qc-ing by speed, didn't make a difference, on the contrary it
      //reduced our sample size a lot.
      //float speed_qc = 11.175;

  float mat_bar_press_delta = qc_config.get_mat_bar_press_delta();

vector<vdt_probe_message_qc> vdt_probe_data_qc::bar_press_qc_model( vector<vdt_probe_message_qc> *messages)
{
  vector<vdt_probe_message_qc> temp;
  float bar_press, model_press;
  float mat_bar_press_delta = qc_config.get_mat_bar_press_delta();

  //vdt_probe_message_qc probe_message;

  for ( unsigned int i = 0; i < messages->size(); i++)
    {
      vdt_probe_message_qc probe_message = messages->at(i);
      bar_press = probe_message.get_bar_press();
      model_press = probe_message.get_model_bar_press();

      if (model_press == vdt_probe_message::BAD_OR_MISSING_VALUE)
        {
          continue;
        }

      if ( (model_press - mat_bar_press_delta) <= bar_press &&
           bar_press <= (model_press + mat_bar_press_delta) )
        {
          probe_message.set_bar_press_mat_passed (1);
        }
      else
        {
          probe_message.set_bar_press_mat_passed (0);
        }
      temp.push_back( probe_message );

    }
  return temp;
}

#endif
