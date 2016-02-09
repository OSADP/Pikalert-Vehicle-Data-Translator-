//==============================================================================
//
//   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_probe_message_qc.cc,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/05/27 20:12:22 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_qc.cc
 *
 *
 */

// Include files
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

using std::unordered_map;

using namespace std;

#include "vdt_probe_message_qc.hh"
#include <vdt4/vdt_const.hh>

//Namespace to prevent name collisions with similar class in VDT 
namespace rwx
{

void vdt_probe_message_qc::copy_based_on_rec_time(const vector<vdt_probe_message_qc> &input_msg, vector<vdt_probe_message_qc> &output_msg)
{
  // set up map for recorded time in output_msg
  unordered_map<double, int> rec_time_index_map;
  for (size_t i=0; i<output_msg.size(); i++)
    {
      rec_time_index_map[output_msg[i].rec_time] = i;
    }

  printf("\n");
  for (auto in_itr=input_msg.begin(); in_itr!=input_msg.end(); ++in_itr)
    {
      auto out_itr = rec_time_index_map.find(in_itr->rec_time);
      if (out_itr == rec_time_index_map.end())
	{
	  continue;
	}
      else
	{
	  output_msg[out_itr->second] = *in_itr;
	  //printf("performing a copy for rec_time %d, %lf, passed %d\n", out_itr->second, in_itr->rec_time, output_msg[out_itr->second].air_temp_persist_passed);
	  //printf("copying quality check information back to original set of messages based on rec_time %d, %lf, passed %d\n", out_itr->second, in_itr->rec_time, output_msg[out_itr->second].air_temp_persist_passed);
	}
    }
  printf("\n");

}

void vdt_probe_message_qc::init()
{
  light_vector.push_back(vdt_probe_message::NOT_EQUIPPED);
  light_vector.push_back(vdt_probe_message::OFF);
  light_vector.push_back(vdt_probe_message::INTERMITTENT);
  light_vector.push_back(vdt_probe_message::LOW);
  light_vector.push_back(vdt_probe_message::HIGH);
  light_vector.push_back(vdt_probe_message::WASHER);
  light_vector.push_back(vdt_probe_message::AUTOMATICPRESENT);
  sort(light_vector.begin(), light_vector.end());

  road_segment_id = vdt_const::FILL_VALUE;
  nss_air_temp_mean = vdt_const::FILL_VALUE;
  nss_surface_temp_mean = vdt_const::FILL_VALUE;
  model_air_temp = vdt_const::FILL_VALUE;
  model_dewpoint = vdt_const::FILL_VALUE;
  nss_bar_press_mean = vdt_const::FILL_VALUE;
  model_bar_press = vdt_const::FILL_VALUE;
  radar_ref = vdt_const::FILL_VALUE;
  radar_precip_flag = vdt_const::FILL_VALUE;
  radar_precip_type = vdt_const::FILL_VALUE;
  cloud_mask = vdt_const::FILL_VALUE;
  nss_dew_temp_mean = vdt_const::FILL_VALUE;
  nss_wind_dir_mean = vdt_const::FILL_VALUE;
  nss_wind_speed_mean = vdt_const::FILL_VALUE;
  nss_hourly_precip_mean = vdt_const::FILL_VALUE;
  nss_prevail_vis_mean = vdt_const::FILL_VALUE;

  abs_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_range_qc_passed  = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_sdt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_nst_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_mat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_crt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_nvt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_cat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_persist_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_spatial_barnes_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_spatial_iqr_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp_step_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_sdt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_nst_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_mat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_crt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_nvt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_cat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_persist_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_spatial_barnes_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_spatial_iqr_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  air_temp2_step_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_range_qc_passed  = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_sdt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_nst_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_mat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_crt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_nvt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_cat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_persist_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_spatial_barnes_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_spatial_iqr_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  bar_press_step_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  brake_boost_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  brake_status_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_cat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_crt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_mat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_nst_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_nvt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_persist_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_sdt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_spatial_barnes_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_spatial_iqr_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  dew_temp_step_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  elevation_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  heading_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  hoz_accel_long_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  hoz_accel_lat_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  latitude_dft_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  longitude_dft_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  lights_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  speed_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  stab_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  steering_angle_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  steering_rate_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_crt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_nvt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_cat_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_sdt_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_persist_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_spatial_barnes_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_spatial_iqr_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  surface_temp_step_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  trac_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  wipers_front_status_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
  yaw_range_qc_passed = vdt_probe_message_qc::NOT_APPLICABLE;
}

vdt_probe_message_qc::vdt_probe_message_qc()
{
  init();
}

vdt_probe_message_qc::vdt_probe_message_qc(const vdt_probe_message &msg) : vdt_probe_message::vdt_probe_message(msg)
{
  init(); 
}

bool vdt_probe_message_qc::operator < (const vdt_probe_message_qc &probe_message_qc) const
{
  return (obs_time < probe_message_qc.obs_time);
}

} //END namespace RWX
