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
#include <algorithm>

using std::unordered_map;

using namespace std;

#include "vdt_probe_message_qc.hh"
#include "vdt_const.hh"


size_t get_qc_msg_hash(const vdt_probe_message_qc& msg, const std::vector<std::string>& probe_msg_keys)
{
   std::string msg_str;
   std::vector<PIKA_TYPE> msg_values;
   msg.getvalues(probe_msg_keys, msg_values);

   for(const PIKA_TYPE& val : msg_values)
   {
     msg_str += pika_to_string(val);
   }

   return std::hash<std::string>()(msg_str);
}

void vdt_probe_message_qc::match_copy(const vector<vdt_probe_message_qc> &input_msg, vector<vdt_probe_message_qc> &output_msg)
{
  //vdt_probe_message_qc is a subclass of vdt_probe_message so get just the vdt_probe_message field names
  const vdt_probe_message probe_msg_model;
  std::vector<std::string> probe_msg_keys;
  probe_msg_model.getkeys(probe_msg_keys);

  //Map from the hash of the vdt_probe_message subset fields of output_msg records to the index in the vector.
  unordered_map<size_t,size_t> output_msg_index_map;
  for(size_t i = 0; i < output_msg.size(); ++i)
  {
    size_t hash_val = get_qc_msg_hash(output_msg[i], probe_msg_keys);
    output_msg_index_map[hash_val] = i;
  }

  //Compare hash of in to out and copy out matches
  for(const vdt_probe_message_qc& in_msg : input_msg)
  {
    size_t hash_val = get_qc_msg_hash(in_msg, probe_msg_keys);
    auto out_itr = output_msg_index_map.find(hash_val);
    if (out_itr != output_msg_index_map.end())
    {
      output_msg[out_itr->second] = in_msg;
    }
  }
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

  setattr("road_segment_id", static_cast<int>(vdt_const::FILL_VALUE));
  setattr("nss_air_temp_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_surface_temp_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("model_air_temp", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("model_dew_temp", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_bar_pressure_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("model_bar_pressure", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_ref", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_cref", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_dual_pol_hc", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("radar_dual_pol_hr", static_cast<double>(vdt_const::FILL_VALUE));    
  setattr("radar_precip_flag", static_cast<int>(vdt_const::FILL_VALUE));
  setattr("radar_precip_type", static_cast<int>(vdt_const::FILL_VALUE));
  setattr("cloud_mask", static_cast<int>(vdt_const::FILL_VALUE));
  setattr("nss_dew_temp_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_wind_dir_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_wind_speed_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_hourly_precip_mean", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("nss_prevail_vis_mean", static_cast<double>(vdt_const::FILL_VALUE));

  setattr("abs_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_nst_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_mat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_nst_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_mat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("air_temp2_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_nst_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_mat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("bar_pressure_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("brake_boost_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("brake_status_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_mat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_nst_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("dew_temp_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("elevation_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("heading_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("hoz_accel_long_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("hoz_accel_lat_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("latitude_dft_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("longitude_dft_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("lights_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("speed_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("stab_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("steering_angle_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("steering_rate_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("surface_temp_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("trac_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("wiper_status_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));
  setattr("yaw_rate_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE));

  setattr("air_temp_cat_confidence", static_cast<double>(0.0));
  setattr("air_temp2_cat_confidence", static_cast<double>(0.0));
  setattr("bar_pressure_cat_confidence", static_cast<double>(0.0));
  setattr("surface_temp_cat_confidence", static_cast<double>(0.0));
  setattr("dew_temp_cat_confidence", static_cast<double>(0.0));
}

void vdt_probe_message_qc::init_qc_fields(const FIELD_NAME_TYPE_VEC& qc_fields)
{
  std::map<std::string,int> initmap;

  for(const auto& elem : qc_fields)
  {
    const std::string& name = elem.first;
    initmap.insert(make_pair(name + "_range_qc_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_crt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_nvt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_cat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_sdt_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_persist_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_spatial_barnes_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_spatial_iqr_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_step_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_mat_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));
    initmap.insert(make_pair(name + "_nst_passed", static_cast<int>(vdt_probe_message_qc::NOT_APPLICABLE)));

    initmap.insert(make_pair(name + "_cat_confidence", static_cast<double>(0.0)));
  }

  setattr<int>(initmap,false);
}

vdt_probe_message_qc::vdt_probe_message_qc(const FIELD_NAME_TYPE_VEC& qc_fields) : vdt_probe_message(qc_fields)
{
  init();
  init_qc_fields(qc_fields);
}

vdt_probe_message_qc::vdt_probe_message_qc(const vdt_probe_message &msg, const FIELD_NAME_TYPE_VEC& qc_fields) : vdt_probe_message::vdt_probe_message(msg)
{
  init(); 
  init_qc_fields(qc_fields);
}

bool vdt_probe_message_qc::operator < (const vdt_probe_message_qc &probe_message_qc) const
{
  return (get_obs_time() < probe_message_qc.get_obs_time());
}
