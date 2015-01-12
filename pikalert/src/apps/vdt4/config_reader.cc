//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/05/27 20:14:58 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/config_reader.cc
 *
 *  Configuration file reader class implementation
 *
 */

// Include files
#include <stdlib.h>
#include <boost/format.hpp>

using boost::format;

#include "config_reader.hh"
#include "libconfig.h++"

using namespace libconfig;

// Constant and macros
static const int MAX_LINE = 1024;
static const float VALID_LAT_MIN = -90.0;
static const float VALID_LAT_MAX = 90.0;
static const float VALID_LON_MIN = -180.0;
static const float VALID_LON_MAX = 180.0;


// Types, structures and classes

// Global variables

// Functions

config_reader::config_reader()
{
}

config_reader::config_reader(const string &input_file_path_arg) : input_file_path(input_file_path_arg)
{
  Config cfg;

  error = string("");

  FILE *fp = fopen(input_file_path.c_str(), "r");

  if (fp == NULL)
    {
      error = string("could not load config file ") + input_file_path;
      return;
    }

  try
    {
      rtma_proj = 0;

      cfg.setAutoConvert(true);
      cfg.read(fp);
      fclose(fp);

      // Get probe message field names
      Setting &probe_message_field_names_setting = cfg.lookup("VDT.field_names.probe_message_field_names");
      for (int i=0; i<probe_message_field_names_setting.getLength(); i++)
        {
          string probe_message_field_name = probe_message_field_names_setting[i];
          probe_message_field_names.push_back(probe_message_field_name);
        }

      // CDL file paths

      const char *probe_message_qc_cdl_file_string = cfg.lookup("VDT.input_config.probe_message_qc_cdl");
      probe_message_qc_cdl_file = probe_message_qc_cdl_file_string;

      const char *probe_message_qc_statistics_cdl_file_string = cfg.lookup("VDT.input_config.statistics_cdl");
      probe_message_qc_statistics_cdl_file = probe_message_qc_statistics_cdl_file_string;

      const char *road_segment_assessment_cdl_file_string = cfg.lookup("VDT.input_config.assessment_cdl");
      road_segment_assessment_cdl_file = road_segment_assessment_cdl_file_string;

      // Lat/lon box
      south_lat = cfg.lookup("VDT.lat_long_box.s_lat");
      north_lat = cfg.lookup("VDT.lat_long_box.w_long");
      west_lon = cfg.lookup("VDT.lat_long_box.n_lat");
      east_lon = cfg.lookup("VDT.lat_long_box.e_long");

      // Combined algorithm test

      combined_algorithm_threshold = cfg.lookup("VDT.qc_config.combined_algorithm_threshold");

      // Combined algorithm weights for STEP, SPATIAL, SDT, PERSIST, NVT, CRT
      Setting &combined_algorithm_weights_setting = cfg.lookup("VDT.qc_config.combined_algorithm_weights");
      for (int i=0; i<combined_algorithm_weights_setting.getLength(); i++)
        {
          float weight = combined_algorithm_weights_setting[i];
          combined_algorithm_weights.push_back(weight);
        }

      // Sensor Range Test

      int srt_indicator = cfg.lookup("VDT.qc_config.sensor_range_test");
      sensor_range_test = srt_indicator;

      speed_range_min = cfg.lookup("VDT.qc_config.speed_range_min");
      speed_range_max = cfg.lookup("VDT.qc_config.speed_range_max");

      int temp_at_min = cfg.lookup("VDT.qc_config.air_temp_min");
      air_temp_min = temp_at_min;
      int temp_at_max = cfg.lookup("VDT.qc_config.air_temp_max");
      air_temp_max= temp_at_max;

      int temp_bp_min = cfg.lookup("VDT.qc_config.bar_press_min");
      bar_press_min = temp_bp_min;
      int temp_bp_max = cfg.lookup("VDT.qc_config.bar_press_max");
      bar_press_max = temp_bp_max;

      int temp_bs_min = cfg.lookup("VDT.qc_config.brake_status_min");
      brake_status_min = temp_bs_min;
      int temp_bs_max = cfg.lookup("VDT.qc_config.brake_status_max");
      brake_status_max = temp_bs_max;

      int temp_bb_min = cfg.lookup("VDT.qc_config.brake_boost_min");
      brake_boost_min = temp_bb_min;
      int temp_bb_max = cfg.lookup("VDT.qc_config.brake_boost_max");
      brake_boost_max = temp_bb_max;

      int temp_ws_min = cfg.lookup("VDT.qc_config.wiper_status_min");
      wiper_status_min = temp_ws_min;
      int temp_ws_max = cfg.lookup("VDT.qc_config.wiper_status_max");
      wiper_status_max = temp_ws_max;

      int temp_tc_min = cfg.lookup("VDT.qc_config.traction_control_min");
      traction_control_min = temp_tc_min;
      int temp_tc_max = cfg.lookup("VDT.qc_config.traction_control_max");
      traction_control_max = temp_tc_max;

      int temp_sc_min = cfg.lookup("VDT.qc_config.stability_control_min");
      stability_control_min = temp_sc_min;
      int temp_sc_max = cfg.lookup("VDT.qc_config.stability_control_max");
      stability_control_max = temp_sc_max;

      int temp_abs_min = cfg.lookup("VDT.qc_config.abs_min");
      abs_min = temp_abs_min;
      int temp_abs_max = cfg.lookup("VDT.qc_config.abs_max");
      abs_max = temp_abs_max;

      int temp_hm_min = cfg.lookup("VDT.qc_config.headlights_min");
      headlights_min = temp_hm_min;
      int temp_hm_max = cfg.lookup("VDT.qc_config.headlights_max");
      headlights_max = temp_hm_max;

      yaw_rate_min = cfg.lookup("VDT.qc_config.yaw_rate_min");
      yaw_rate_max = cfg.lookup("VDT.qc_config.yaw_rate_max");
      hoz_accel_lat_min = cfg.lookup("VDT.qc_config.hoz_accel_lat_min");
      hoz_accel_lat_max = cfg.lookup("VDT.qc_config.hoz_accel_lat_max");
      hoz_accel_lon_min = cfg.lookup("VDT.qc_config.hoz_accel_lon_min");
      hoz_accel_lon_max = cfg.lookup("VDT.qc_config.hoz_accel_lon_max");
      steering_wheel_angle_min = cfg.lookup("VDT.qc_config.steering_wheel_angle_min");
      steering_wheel_angle_max = cfg.lookup("VDT.qc_config.steering_wheel_angle_max");

      int temp_sr_min = cfg.lookup("VDT.qc_config.steering_rate_min");
      steering_rate_min = temp_sr_min;
      int temp_sr_max = cfg.lookup("VDT.qc_config.steering_rate_max");
      steering_rate_max =  temp_sr_max;

      heading_min = cfg.lookup("VDT.qc_config.heading_min");
      heading_max = cfg.lookup("VDT.qc_config.heading_max");

      max_dist_to_segment = cfg.lookup("VDT.qc_config.max_dist_to_segment");
      max_dist_to_cell = cfg.lookup("VDT.qc_config.max_dist_to_cell");


      // Standard Deviation Test

      int sdt_indicator = cfg.lookup("VDT.qc_config.standard_deviation_test");
      standard_deviation_test = sdt_indicator;

      air_temp_dev_mult = cfg.lookup("VDT.qc_config.air_temp_dev_mult");
      bar_press_dev_mult = cfg.lookup("VDT.qc_config.bar_press_dev_mult");

      // Neighboring Surface Station Test (NST)

      int sst_indicator = cfg.lookup("VDT.qc_config.surface_station_test");
      surface_station_test = sst_indicator;

      nst_search_radius = cfg.lookup("VDT.qc_config.nst_search_radius");

      int temp_ntr = cfg.lookup("VDT.qc_config.nst_time_range");
      nst_time_range = temp_ntr;

      int temp_nqat = cfg.lookup("VDT.qc_config.nst_qc_air_temp");
      nst_qc_air_temp = temp_nqat;

      nst_air_temp_delta = cfg.lookup("VDT.qc_config.nst_air_temp_delta");

      int temp_nqbp = cfg.lookup("VDT.qc_config.nst_qc_bar_press");
      nst_qc_bar_press = temp_nqbp;

      nst_bar_press_delta = cfg.lookup("VDT.qc_config.nst_bar_press_delta");

      // Model Analysis Test (MAT)

      int mat_indicator = cfg.lookup("VDT.qc_config.model_analysis_test");
      model_analysis_test = mat_indicator;

      mat_air_temp_delta = cfg.lookup("VDT.qc_config.mat_air_temp_delta");

      mat_dewpoint_delta = cfg.lookup("VDT.qc_config.mat_dewpoint_delta");

      mat_bar_press_delta = cfg.lookup("VDT.qc_config.mat_bar_press_delta");

      // Climatological Range Test (CRT)

      const char *climatology_file_string = cfg.lookup("VDT.input_config.climatology_file");
      climatology_file = climatology_file_string;

      int crt_indicator = cfg.lookup("VDT.qc_config.climatological_range_test");
      climatological_range_test = crt_indicator;

      // Neighboring Vehicle Test (NVT)

      int nvt_indicator = cfg.lookup("VDT.qc_config.neighboring_vehicle_test");
      neighboring_vehicle_test = nvt_indicator;

      // Remote Observation Test (ROT)

      int rot_indicator = cfg.lookup("VDT.qc_config.remote_observation_test");
      remote_observation_test  = rot_indicator;

      // Data Filtering Test (DFT)

      int dft_indicator = cfg.lookup("VDT.qc_config.data_filtering_test");
      data_filtering_test = dft_indicator;

      latitude_min = cfg.lookup("VDT.qc_config.latitude_min");
      latitude_max = cfg.lookup("VDT.qc_config.latitude_max");

      longitude_min = cfg.lookup("VDT.qc_config.longitude_min");
      longitude_max = cfg.lookup("VDT.qc_config.longitude_max");

      if (data_filtering_test)
        {
          if (latitude_min < VALID_LAT_MIN)
            {
              error = str(format(" - The latitude_min of %1% specified in %2% is less than the valid latitude minimum of %3%") % latitude_min % input_file_path.c_str() % VALID_LAT_MIN);
              return;
            }

          if (latitude_max > VALID_LAT_MAX)
            {
              error = str(format("- The latitude_max of %1% specified in %2% is greater than the valid latitude maximum of %3%") % latitude_max % input_file_path.c_str() % VALID_LAT_MAX);
              return;
            }
          if (longitude_min < VALID_LON_MIN)
            {
              error = str(format(" - The longitude_min of %1% specified in %2% is less than the valid longitude minimum of %3%") % longitude_min % input_file_path.c_str() % VALID_LON_MIN);
              return;
            }

          if (longitude_max > VALID_LON_MAX)
            {
              error = str(format("- The longitude_max of %1% specified in %2% is greater than the valid longitude maximum of %3%") % longitude_max % input_file_path.c_str() % VALID_LON_MAX);
              return;
            }
        }

      // Spatial Tests - Air Pressure (STP) and Air and Surface Temperature (STTa and STTp)
      int spatial_test_indicator = cfg.lookup("VDT.qc_config.spatial_tests");
      spatial_tests = spatial_test_indicator;

      spatial_iqr_air_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_air_temp_tol");
      spatial_iqr_dew_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_dew_temp_tol");
      spatial_iqr_surface_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_surface_temp_tol");
      spatial_iqr_pressure_tol = cfg.lookup("VDT.qc_config.spatial_iqr_pressure_tol");
      spatial_iqr_multiplier = cfg.lookup("VDT.qc_config.spatial_iqr_multiplier");

      spatial_range_search_km = cfg.lookup("VDT.qc_config.spatial_range_search_km");
      spatial_time_search_secs = cfg.lookup("VDT.qc_config.spatial_time_search_secs");

      spatial_barnes_min_size = cfg.lookup("VDT.qc_config.spatial_barnes_min_size");
      spatial_barnes_max_sd = cfg.lookup("VDT.qc_config.spatial_barnes_max_sd");

      // History Tests (HT)
      int ht_indicator = cfg.lookup("VDT.qc_config.history_tests");
      history_tests = ht_indicator;

      const char *history_path_string = cfg.lookup("VDT.qc_config.history_path");
      history_path = history_path_string;

      const char *history_basename_string = cfg.lookup("VDT.qc_config.history_basename");
      history_basename = history_basename_string;

      history_cutoff_time = cfg.lookup("VDT.qc_config.history_cutoff_time");

      // Persistence Test (PET)
      int pet_indicator = cfg.lookup("VDT.qc_config.persistence_test");
      persistence_test = pet_indicator;

      air_temp_persist_time = cfg.lookup("VDT.qc_config.air_temp_persist_time");
      air_temp2_persist_time = cfg.lookup("VDT.qc_config.air_temp2_persist_time");
      dew_temp_persist_time = cfg.lookup("VDT.qc_config.dew_temp_persist_time");
      surface_temp_persist_time = cfg.lookup("VDT.qc_config.surface_temp_persist_time");
      bar_press_persist_time = cfg.lookup("VDT.qc_config.bar_press_persist_time");

      // Air temp Time Step Test (TST)
      int tst_indicator = cfg.lookup("VDT.qc_config.time_step_test");
      time_step_test = tst_indicator;

      air_temp_step_magnitude = cfg.lookup("VDT.qc_config.air_temp_step_magnitude");
      air_temp_step_time = cfg.lookup("VDT.qc_config.air_temp_step_time");

      air_temp2_step_magnitude = cfg.lookup("VDT.qc_config.air_temp2_step_magnitude");
      air_temp2_step_time = cfg.lookup("VDT.qc_config.air_temp2_step_time");

      dew_temp_step_magnitude = cfg.lookup("VDT.qc_config.dew_temp_step_magnitude");
      dew_temp_step_time = cfg.lookup("VDT.qc_config.dew_temp_step_time");

      surface_temp_step_magnitude = cfg.lookup("VDT.qc_config.surface_temp_step_magnitude");
      surface_temp_step_time = cfg.lookup("VDT.qc_config.surface_temp_step_time");

      // Barometric pressure Time Step Test (TST)
      bar_press_step_magnitude = cfg.lookup("VDT.qc_config.bar_press_step_magnitude");
      bar_press_step_time = cfg.lookup("VDT.qc_config.bar_press_step_time");

      max_begin_time_delta = cfg.lookup("VDT.qc_config.max_begin_time_delta");
      max_end_time_delta = cfg.lookup("VDT.qc_config.max_end_time_delta");

      // Rtma projection string for proj4
      const char *rtma_proj4_projection_char_string = cfg.lookup("VDT.rtma_proj4_projection_string");
      rtma_proj4_projection_string = rtma_proj4_projection_char_string;

      // Rtma projection dx and dy for proj4
      rtma_dx = cfg.lookup("VDT.rtma_dx");      
      rtma_dy = cfg.lookup("VDT.rtma_dy");

      // Rtma projection la1 and lo1 for proj4
      rtma_la1 = cfg.lookup("VDT.rtma_la1");
      rtma_lo1 = cfg.lookup("VDT.rtma_lo1");      

      rtma_proj = new p4w::Proj4Wrap(rtma_proj4_projection_string, p4w::Proj4Wrap::LON_LAT_TYPE, rtma_lo1, rtma_la1, rtma_dx, rtma_dy);
    }
  catch (ParseException ce)
    {
      const int size = MAX_LINE;
      char line[size];
      snprintf(line, size, " on line %d", ce.getLine());
      error = string("Config reader parse exception occurred: ") + string(ce.getError()) + line;
    }
  catch (SettingException ce)
    {
      error = string("Config reader setting exception occurred on: ") + ce.getPath();
    }
  catch (FileIOException ce)
    {
      error = string("Config reader file exception occurred on: ") + input_file_path;
    }
  catch (ConfigException ce)
    {
      error = string("Config reader generic exception occurred");
    }
}

config_reader::~config_reader()
{
  delete rtma_proj;
}
