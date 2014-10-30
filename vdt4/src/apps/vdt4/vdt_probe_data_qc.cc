//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

// Include files
#include <string>
#include <vector>
#include <stdio.h>

#include <log/log.hh>

#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_probe_message_history.hh"
#include "vdt4/vdt_history_check.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt4/vdt_climatology_file_reader.hh"

#include "vdt4/stat_tools.hh"

#include "vdt_probe_data_qc.hh"
#include "config_reader.hh"
#include "qc_check.hh"

using namespace std;

// Global variables
extern Log *Logg;
extern int Debug_level;

vdt_probe_data_qc::vdt_probe_data_qc(const config_reader &cfg_reader_arg, const vdt_climatology_file_reader *climatology_data_ptr, vdt_probe_message_datasets *probe_message_datasets) : cfg_reader(cfg_reader_arg), climate_data(climatology_data_ptr), datasets(probe_message_datasets)
{
}


void vdt_probe_data_qc::perform_history_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc> &history_msgs, vector<vdt_probe_message_qc> &messages)
{
  // Set up the history qc checks and store in the qc vector
  vector<vdt_history_check *> history_check;

  vdt_air_temp_persist_check air_temp_persist_check(begin_time, end_time, cfg_reader.air_temp_persist_time);
  vdt_air_temp2_persist_check air_temp2_persist_check(begin_time, end_time, cfg_reader.air_temp2_persist_time);
  vdt_dew_temp_persist_check dew_temp_persist_check(begin_time, end_time, cfg_reader.dew_temp_persist_time);

  vdt_surface_temp_persist_check surface_temp_persist_check(begin_time, end_time, cfg_reader.surface_temp_persist_time);
  vdt_bar_press_persist_check bar_press_persist_check(begin_time, end_time, cfg_reader.bar_press_persist_time);

  vdt_air_temp_step_check air_temp_step_check(begin_time, end_time, cfg_reader.air_temp_step_time, cfg_reader.air_temp_step_magnitude);
  vdt_air_temp2_step_check air_temp2_step_check(begin_time, end_time, cfg_reader.air_temp2_step_time, cfg_reader.air_temp2_step_magnitude);
  vdt_dew_temp_step_check dew_temp_step_check(begin_time, end_time, cfg_reader.dew_temp_step_time, cfg_reader.dew_temp_step_magnitude);
  vdt_surface_temp_step_check surface_temp_step_check(begin_time, end_time, cfg_reader.surface_temp_step_time, cfg_reader.surface_temp_step_magnitude);
  vdt_bar_press_step_check bar_press_step_check(begin_time, end_time, cfg_reader.bar_press_step_time, cfg_reader.bar_press_step_magnitude);
  
  if (cfg_reader.history_tests)
    {
      if (cfg_reader.persistence_test)
	{
	  history_check.push_back(&air_temp_persist_check);
	  history_check.push_back(&air_temp2_persist_check);
	  history_check.push_back(&dew_temp_persist_check);
	  history_check.push_back(&surface_temp_persist_check);
	  history_check.push_back(&bar_press_persist_check);
	}

      if (cfg_reader.time_step_test)
	{
	  history_check.push_back(&air_temp_step_check);
	  history_check.push_back(&air_temp2_step_check);
	  history_check.push_back(&dew_temp_step_check);
	  history_check.push_back(&surface_temp_step_check);
	  history_check.push_back(&bar_press_step_check);
	}

      unordered_map<string, vector<vdt_probe_message_qc> > probe_message_history_map;

      vdt_probe_message_history(history_msgs, probe_message_history_map);

      vector<vdt_probe_message_qc> out_probe_message;
      vdt_run_history_check(history_check, messages, probe_message_history_map, begin_time, end_time, out_probe_message);

      // Copy out_probe_message quality check information back to original set of messages based on recorded time
      vdt_probe_message_qc::copy_based_on_rec_time(out_probe_message, messages);
    }
}

void vdt_probe_data_qc::perform_standard_checks(time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &messages)
{

  // Set up the qc tests and store in the qc vector
  vector<qc_check *> qc_vec;
  vector<qc_check *> climate_qc_vec;
  vector<qc_check *> data_filtering_qc_vec;
  vector<qc_check *> spatial_test_qc_vec;

  // Anticipated Range Test (ART) checks
  speed_range_check speed_range_qc(cfg_reader, climate_data, datasets);
  air_temp_range_check air_temp_range_qc(cfg_reader, climate_data, datasets);
  air_temp2_range_check air_temp2_range_qc(cfg_reader, climate_data, datasets);
  dew_temp_range_check dew_temp_range_qc(cfg_reader, climate_data, datasets);
  surface_temp_range_check surface_temp_range_qc(cfg_reader, climate_data, datasets);
  bar_press_range_check bar_press_range_qc(cfg_reader, climate_data, datasets);
  brake_status_range_check brake_status_range_qc(cfg_reader, climate_data, datasets);
  brake_boost_range_check brake_boost_range_qc(cfg_reader, climate_data, datasets);
  wiper_status_range_check wiper_status_range_qc(cfg_reader, climate_data, datasets);
  traction_control_range_check traction_control_range_qc(cfg_reader, climate_data, datasets);
  stability_control_range_check stability_control_range_qc(cfg_reader, climate_data, datasets);
  abs_range_check abs_range_qc(cfg_reader, climate_data, datasets);
  headlights_range_check headlights_range_qc(cfg_reader, climate_data, datasets);
  yaw_rate_range_check yaw_rate_range_qc(cfg_reader, climate_data, datasets);
  hoz_accel_lat_range_check hoz_accel_lat_range_qc(cfg_reader, climate_data, datasets);
  hoz_accel_lon_range_check hoz_accel_lon_range_qc(cfg_reader, climate_data, datasets);
  steering_wheel_range_check steering_wheel_range_qc(cfg_reader, climate_data, datasets);
  steering_rate_range_check steering_rate_range_qc(cfg_reader, climate_data, datasets);
  heading_range_check heading_range_qc(cfg_reader, climate_data, datasets);

  if (cfg_reader.sensor_range_test)
    {
      qc_vec.push_back(&speed_range_qc);
      qc_vec.push_back(&air_temp_range_qc);
      qc_vec.push_back(&air_temp2_range_qc);
      qc_vec.push_back(&dew_temp_range_qc);
      qc_vec.push_back(&surface_temp_range_qc);
      qc_vec.push_back(&bar_press_range_qc);
      qc_vec.push_back(&brake_status_range_qc);
      qc_vec.push_back(&brake_boost_range_qc);
      qc_vec.push_back(&wiper_status_range_qc);
      qc_vec.push_back(&traction_control_range_qc);
      qc_vec.push_back(&stability_control_range_qc);
      qc_vec.push_back(&abs_range_qc);
      qc_vec.push_back(&headlights_range_qc);
      qc_vec.push_back(&yaw_rate_range_qc);
      qc_vec.push_back(&hoz_accel_lat_range_qc);
      qc_vec.push_back(&hoz_accel_lon_range_qc);
      qc_vec.push_back(&steering_wheel_range_qc);
      qc_vec.push_back(&steering_rate_range_qc);
      qc_vec.push_back(&heading_range_qc);
      Logg->write_time_info("running %ld sensor range tests\n", qc_vec.size());
    }

  // Climatological Range Test (CRT) checks
  air_temp_climate_range_check air_temp_climate_qc(cfg_reader, climate_data, datasets);
  air_temp2_climate_range_check air_temp2_climate_qc(cfg_reader, climate_data, datasets);
  dew_temp_climate_range_check dew_temp_climate_qc(cfg_reader, climate_data, datasets);
  surface_temp_climate_range_check surface_temp_climate_qc(cfg_reader, climate_data, datasets);
  bar_press_climate_range_check bar_press_climate_qc(cfg_reader, climate_data, datasets);

  if (cfg_reader.climatological_range_test && climate_data->error() == "")
    {
      climate_qc_vec.push_back(&air_temp_climate_qc);
      climate_qc_vec.push_back(&air_temp2_climate_qc);
      climate_qc_vec.push_back(&dew_temp_climate_qc);
      climate_qc_vec.push_back(&surface_temp_climate_qc);
      climate_qc_vec.push_back(&bar_press_climate_qc);

      Logg->write_time_info("running %ld climate range tests\n", climate_qc_vec.size());
    }

  // Data Filtering Test (DFT) checks
  latitude_data_filtering_check latitude_data_filtering_qc(cfg_reader, climate_data, datasets);
  longitude_data_filtering_check longitude_data_filtering_qc(cfg_reader, climate_data, datasets);

  if (cfg_reader.data_filtering_test)
    {
      data_filtering_qc_vec.push_back(&latitude_data_filtering_qc);
      data_filtering_qc_vec.push_back(&longitude_data_filtering_qc);
      Logg->write_time_info("running %ld data filtering tests\n", data_filtering_qc_vec.size());
    }

  // Spatial checks
  iqr_spatial_check iqr_spatial_qc(cfg_reader, climate_data, datasets);

  if (cfg_reader.spatial_tests)
    {
      spatial_test_qc_vec.push_back(&iqr_spatial_qc);

      Logg->write_time_info("running %ld spatial tests\n", spatial_test_qc_vec.size());
    }

  Logg->write_time_info("qc checks being performed on %ld messages\n", messages.size());
  for (unsigned int i=0; i<messages.size(); i++)
    {
      time_t obs_time = messages[i].get_obs_time();

      // Anticipated Range Test (ART) checks
      for (unsigned int j=0; j<qc_vec.size(); j++)
	{
	  (*qc_vec[j])(messages[i]);
	}
      // Climatological Range Test (CRT) checks
      for (unsigned int k=0; k<climate_qc_vec.size(); k++)
	{
	  (*climate_qc_vec[k])(messages[i]);
	}
      // Data Filtering Test (DFT) checks
      for (unsigned int m=0; m<data_filtering_qc_vec.size(); m++)
	{
	  (*data_filtering_qc_vec[m])(messages[i]);
	}
      // Spatial checks
      for (unsigned int n=0; n<spatial_test_qc_vec.size(); n++)
	{
	  // ensure message observation time falls in the time window used in the qc test
	  if (begin_time <= obs_time && obs_time < end_time)
	    {
	      (*spatial_test_qc_vec[n])(messages[i]);
	    }
	}

    }  // end for i

} // end check_all_ranges

void vdt_probe_data_qc::perform_concluding_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, unordered_map<int, int> &seg_index_offset, vector<vdt_probe_message_qc> &messages)
{
  // set up model analysis check
  vdt_model_check model_check(cfg_reader, climate_data, datasets);

  // set up standard deviation check
  vdt_stdev_check stdev_check(cfg_reader, climate_data, datasets, seg_stats, seg_indices, seg_index_offset);
  
  // set up neighboring vehicle check
  vdt_neighbor_check_iqr iqr_neighbor_qc(cfg_reader, climate_data, datasets, seg_stats, seg_indices, seg_index_offset);

  for (unsigned int i=0; i<messages.size(); i++)
    {
      time_t obs_time = messages[i].get_obs_time();

      // ensure message observation time falls in the time window used in the qc tests
      if (begin_time <= obs_time && obs_time < end_time)
	{
	  if (cfg_reader.model_analysis_test)
	    {
	      model_check(seg_indices[i], messages[i]);
	    }
	  if (cfg_reader.standard_deviation_test)
	    {
	      stdev_check(seg_indices[i], messages[i]);
	    }

	  if (cfg_reader.neighboring_vehicle_test)
	    {
	      iqr_neighbor_qc(seg_indices[i], messages[i]);
	    }
	}
    }
}
