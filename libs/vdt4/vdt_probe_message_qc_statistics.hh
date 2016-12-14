//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_qc_statistics.hh,v $
//       Version: $Revision: 1.12 $  Dated: $Date: 2014/04/11 20:32:55 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_qc_statistics.hh
 *
 * @class vdt_probe_message_qc_statistics
 *
 *  vdt probe_message_qc statistics class
 *
 */

#ifndef VDT_PROBE_MESSAGE_QC_STATISTICS_HH
#define VDT_PROBE_MESSAGE_QC_STATISTICS_HH

#include "vdt_dataset.hh"
#include "vdt_nearest_nbr.hh"
#include "vdt_probe_message_qc.hh"
#include "vdt_point.hh"

#include <unordered_map>

using std::unordered_map;
using namespace std;

/**
 * @class vdt_probe_message_qc_statistics 
 * calculates statistics for a collection of observations
 */
class vdt_probe_message_qc_statistics : public pika_message
{
public:

  /**
   * Augment statistics vector with associated dataset information 
   * @param[in] datasets input datasets
   * @param[in, out] seg_stats statistics to be augmented
   * @param[out] error if return < 0, error contains information as to the problem
   * @return 0 on success, -1 on failure. This procedure can fail if a seg_stat in seg_stats has an id that is not an index in for the datasets vector.
   */
  static int augment_statistics_vector_with_datasets(const vector<vdt_dataset> &datasets, vector<vdt_probe_message_qc_statistics> &seg_stats, string &error);

  /**
   * Augment statistics vector with associated dataset information 
   * @param[in] dataset input dataset for probe message qc statistic
   * Updates statistics object with dataset information.
   */
  void augment_statistics_with_dataset(const vdt_dataset &dataset);

private:
  /** @brief constructor initializes statistics variables */
  vdt_probe_message_qc_statistics(const std::vector<std::string>& stats_fields);

  /** @brief init initialization routine */
  void init();

  void init_stats_fields(const std::vector<std::string>& stats_fields);

  /**
   * @brief constructor constructs statistics using messages
   * @param[in] msgs probe message qc input vector used for calculating statistics
   */
  vdt_probe_message_qc_statistics(time_t begin_time_value, time_t end_time_value, const vector<vdt_probe_message_qc> &msgs, const std::vector<std::string>& stats_fields);
public:
  int& get_id()
  {
    return getattr<int>("id");
  }

  double& get_begin_time()
  {
    return getattr<double>("begin_time");
  }

  double& get_end_time()
  {
    return getattr<double>("end_time");
  }

  string& get_road_name()
  {
    return getattr<string>("road_name");
  }

  double& get_model_air_temp()
  {
    return getattr<double>("model_air_temp");
  }

  double& get_model_dewpoint()
  {
    return getattr<double>("model_dew_temp");
  }

  double& get_model_bar_press()
  {
    return getattr<double>("model_bar_pressure");
  }

  double& get_radar_ref()
  {
    return getattr<double>("radar_ref");
  }

  
  double& get_radar_cref()
  {
    return getattr<double>("radar_cref");
  }

  double& get_radar_dual_pol_hc()
  {
    return getattr<double>("radar_dual_pol_hc");
  }
  double& get_radar_dual_pol_hr()
  {
    return getattr<double>("radar_dual_pol_hr");
  }  
  
  int& get_radar_precip_flag()
  {
    return getattr<int>("radar_precip_flag");
  }

  int& get_radar_precip_type()
  {
    return getattr<int>("radar_precip_type");
  }

  int& get_cloud_mask()
  {
    return getattr<int>("cloud_mask");
  }

  double& get_nss_air_temp_mean()
  {
    return getattr<double>("nss_air_temp_mean");
  }

  double& get_nss_surface_temp_mean()
  {
    return getattr<double>("nss_surface_temp_mean");
  }

  double& get_nss_bar_press_mean()
  {
    return getattr<double>("nss_bar_pressure_mean");
  }

  double& get_nss_dew_temp_mean()
  {
    return getattr<double>("nss_dew_temp_mean");
  }

  double& get_nss_wind_dir_mean()
  {
    return getattr<double>("nss_wind_dir_mean");
  }

  double& get_nss_wind_speed_mean()
  {
    return getattr<double>("nss_wind_speed_mean");
  }

  double& get_nss_hourly_precip_mean()
  {
    return getattr<double>("nss_hourly_precip_mean");
  }

  double& get_nss_prevail_vis_mean()
  {
    return getattr<double>("nss_prevail_vis_mean");
  }

  int& get_num_msg()
  {
    return getattr<int>("total_num_msg");
  }

  int& get_num_msg_valid_air_temp()
  {
    return getattr<int>("num_msg_valid_air_temp");
  }

  double& get_air_temp_min()
  {
    return getattr<double>("air_temp_min");
  }

  double& get_air_temp_max()
  {
    return getattr<double>("air_temp_max");
  }

  double& get_air_temp_mean()
  {
    return getattr<double>("air_temp_mean");
  }

  double& get_air_temp_median()
  {
    return getattr<double>("air_temp_median");
  }

  double& get_air_temp_iqr25()
  {
    return getattr<double>("air_temp_iqr25");
  }

  double& get_air_temp_iqr75()
  {
    return getattr<double>("air_temp_iqr75");
  }

  double& get_air_temp_var()
  {
    return getattr<double>("air_temp_var");
  }

  double& get_air_temp_stdev()
  {
    return getattr<double>("air_temp_stdev");
  }

  int& get_num_msg_valid_air_temp2()
  {
    return getattr<int>("num_msg_valid_air_temp2");
  }

  double& get_air_temp2_min()
  {
    return getattr<double>("air_temp2_min");
  }

  double& get_air_temp2_max()
  {
    return getattr<double>("air_temp2_max");
  }

  double& get_air_temp2_mean()
  {
    return getattr<double>("air_temp2_mean");
  }

  double& get_air_temp2_median()
  {
    return getattr<double>("air_temp2_median");
  }

  double& get_air_temp2_iqr25()
  {
    return getattr<double>("air_temp2_iqr25");
  }

  double& get_air_temp2_iqr75()
  {
    return getattr<double>("air_temp2_iqr75");
  }

  double& get_air_temp2_var()
  {
    return getattr<double>("air_temp2_var");
  }

  double& get_air_temp2_stdev()
  {
    return getattr<double>("air_temp2_stdev");
  }

  int& get_num_msg_valid_bar_press()
  {
    return getattr<int>("num_msg_valid_bar_pressure");
  }

  double& get_bar_press_min()
  {
    return getattr<double>("bar_pressure_min");
  }

  double& get_bar_press_max()
  {
    return getattr<double>("bar_pressure_max");
  }

  double& get_bar_press_mean()
  {
    return getattr<double>("bar_pressure_mean");
  }

  double& get_bar_press_median()
  {
    return getattr<double>("bar_pressure_median");
  }

  double& get_bar_press_iqr25()
  {
    return getattr<double>("bar_pressure_iqr25");
  }

  double& get_bar_press_iqr75()
  {
    return getattr<double>("bar_pressure_iqr75");
  }

  double& get_bar_press_var()
  {
    return getattr<double>("bar_pressure_var");
  }

  double& get_bar_press_stdev()
  {
    return getattr<double>("bar_pressure_stdev");
  }

  int& get_num_msg_valid_dew_temp()
  {
    return getattr<int>("num_msg_valid_dew_temp");
  }

  double& get_dew_temp_min()
  {
    return getattr<double>("dew_temp_min");
  }

  double& get_dew_temp_max()
  {
    return getattr<double>("dew_temp_max");
  }

  double& get_dew_temp_mean()
  {
    return getattr<double>("dew_temp_mean");
  }

  double& get_dew_temp_median()
  {
    return getattr<double>("dew_temp_median");
  }

  double& get_dew_temp_iqr25()
  {
    return getattr<double>("dew_temp_iqr25");
  }

  double& get_dew_temp_iqr75()
  {
    return getattr<double>("dew_temp_iqr75");
  }

  double& get_dew_temp_var()
  {
    return getattr<double>("dew_temp_var");
  }

  double& get_dew_temp_stdev()
  {
    return getattr<double>("dew_temp_stdev");
  }

  int& get_num_msg_valid_heading()
  {
    return getattr<int>("num_msg_valid_heading");
  }

  double& get_heading_min()
  {
    return getattr<double>("heading_min");
  }

  double& get_heading_max()
  {
    return getattr<double>("heading_max");
  }

  double& get_heading_mean()
  {
    return getattr<double>("heading_mean");
  }

  double& get_heading_median()
  {
    return getattr<double>("heading_median");
  }

  double& get_heading_iqr25()
  {
    return getattr<double>("heading_iqr25");
  }

  double& get_heading_iqr75()
  {
    return getattr<double>("heading_iqr75");
  }

  double& get_heading_var()
  {
    return getattr<double>("heading_var");
  }

  double& get_heading_stdev()
  {
    return getattr<double>("heading_stdev");
  }

  int& get_num_msg_valid_lat_accel()
  {
    return getattr<int>("num_msg_valid_hoz_accel_lat");
  }

  double& get_lat_accel_min()
  {
    return getattr<double>("hoz_accel_lat_min");
  }

  double& get_lat_accel_max()
  {
    return getattr<double>("hoz_accel_lat_max");
  }

  double& get_lat_accel_mean()
  {
    return getattr<double>("hoz_accel_lat_mean");
  }

  double& get_lat_accel_median()
  {
    return getattr<double>("hoz_accel_lat_median");
  }

  double& get_lat_accel_iqr25()
  {
    return getattr<double>("hoz_accel_lat_iqr25");
  }

  double& get_lat_accel_iqr75()
  {
    return getattr<double>("hoz_accel_lat_iqr75");
  }

  double& get_lat_accel_var()
  {
    return getattr<double>("hoz_accel_lat_var");
  }

  double& get_lat_accel_stdev()
  {
    return getattr<double>("hoz_accel_lat_stdev");
  }

  int& get_num_msg_valid_lon_accel()
  {
    return getattr<int>("num_msg_valid_hoz_accel_lon");
  }

  double& get_lon_accel_min()
  {
    return getattr<double>("hoz_accel_lon_min");
  }

  double& get_lon_accel_max()
  {
    return getattr<double>("hoz_accel_lon_max");
  }

  double& get_lon_accel_mean()
  {
    return getattr<double>("hoz_accel_lon_mean");
  }

  double& get_lon_accel_median()
  {
    return getattr<double>("hoz_accel_lon_median");
  }

  double& get_lon_accel_iqr25()
  {
    return getattr<double>("hoz_accel_lon_iqr25");
  }

  double& get_lon_accel_iqr75()
  {
    return getattr<double>("hoz_accel_lon_iqr75");
  }

  double& get_lon_accel_var()
  {
    return getattr<double>("hoz_accel_lon_var");
  }

  double& get_lon_accel_stdev()
  {
    return getattr<double>("hoz_accel_lon_stdev");
  }

  int& get_num_msg_valid_yaw()
  {
    return getattr<int>("num_msg_valid_yaw_rate");
  }

  double& get_yaw_min()
  {
    return getattr<double>("yaw_rate_min");
  }

  double& get_yaw_max()
  {
    return getattr<double>("yaw_rate_max");
  }

  double& get_yaw_mean()
  {
    return getattr<double>("yaw_rate_mean");
  }

  double& get_yaw_median()
  {
    return getattr<double>("yaw_rate_median");
  }

  double& get_yaw_iqr25()
  {
    return getattr<double>("yaw_rate_iqr25");
  }

  double& get_yaw_iqr75()
  {
    return getattr<double>("yaw_rate_iqr75");
  }

  double& get_yaw_var()
  {
    return getattr<double>("yaw_rate_var");
  }

  double& get_yaw_stdev()
  {
    return getattr<double>("yaw_rate_stdev");
  }

  int& get_num_msg_valid_steering_angle()
  {
    return getattr<int>("num_msg_valid_steering_angle");
  }

  double& get_steering_angle_min()
  {
    return getattr<double>("steering_angle_min");
  }

  double& get_steering_angle_max()
  {
    return getattr<double>("steering_angle_max");
  }

  double& get_steering_angle_mean()
  {
    return getattr<double>("steering_angle_mean");
  }

  double& get_steering_angle_median()
  {
    return getattr<double>("steering_angle_median");
  }

  double& get_steering_angle_iqr25()
  {
    return getattr<double>("steering_angle_iqr25");
  }

  double& get_steering_angle_iqr75()
  {
    return getattr<double>("steering_angle_iqr75");
  }

  double& get_steering_angle_var()
  {
    return getattr<double>("steering_angle_var");
  }

  double& get_steering_angle_stdev()
  {
    return getattr<double>("steering_angle_stdev");
  }

  int& get_num_msg_valid_lights()
  {
    return getattr<int>("num_msg_valid_lights");
  }

  int& get_num_lights_off()
  {
    return getattr<int>("num_lights_off");
  }

  int& get_num_lights_hazard()
  {
    return getattr<int>("num_lights_hazard");
  }

  int& get_num_lights_parking()
  {
    return getattr<int>("num_lights_parking");
  }

  int& get_num_lights_fog()
  {
    return getattr<int>("num_lights_fog");
  }

  int& get_num_lights_drl()
  {
    return getattr<int>("num_lights_drl");
  }

  int& get_num_lights_automatic_control()
  {
    return getattr<int>("num_lights_automatic_control");
  }

  int& get_num_lights_right_turn()
  {
    return getattr<int>("num_lights_right_turn");
  }

  int& get_num_lights_left_turn()
  {
    return getattr<int>("num_lights_left_turn");
  }

  int& get_num_lights_high_beam()
  {
    return getattr<int>("num_lights_high_beam");
  }

  int& get_num_lights_low_beam()
  {
    return getattr<int>("num_lights_low_beam");
  }

  int& get_num_msg_valid_speed()
  {
    return getattr<int>("num_msg_valid_speed");
  }

  double& get_speed_ratio()
  {
    return getattr<double>("speed_ratio");
  }

  double& get_speed_min()
  {
    return getattr<double>("speed_min");
  }

  double& get_speed_max()
  {
    return getattr<double>("speed_max");
  }

  double& get_speed_mean()
  {
    return getattr<double>("speed_mean");
  }

  double& get_speed_median()
  {
    return getattr<double>("speed_median");
  }

  double& get_speed_iqr25()
  {
    return getattr<double>("speed_iqr25");
  }

  double& get_speed_iqr75()
  {
    return getattr<double>("speed_iqr75");
  }

  double& get_speed_var()
  {
    return getattr<double>("speed_var");
  }

  double& get_speed_stdev()
  {
    return getattr<double>("speed_stdev");
  }

  int& get_num_msg_valid_surface_temp()
  {
    return getattr<int>("num_msg_valid_surface_temp");
  }

  double& get_surface_temp_min()
  {
    return getattr<double>("surface_temp_min");
  }

  double& get_surface_temp_max()
  {
    return getattr<double>("surface_temp_max");
  }

  double& get_surface_temp_mean()
  {
    return getattr<double>("surface_temp_mean");
  }

  double& get_surface_temp_median()
  {
    return getattr<double>("surface_temp_median");
  }

  double& get_surface_temp_iqr25()
  {
    return getattr<double>("surface_temp_iqr25");
  }

  double& get_surface_temp_iqr75()
  {
    return getattr<double>("surface_temp_iqr75");
  }

  double& get_surface_temp_var()
  {
    return getattr<double>("surface_temp_var");
  }

  double& get_surface_temp_stdev()
  {
    return getattr<double>("surface_temp_stdev");
  }

  int& get_num_msg_valid_wipers()
  {
    return getattr<int>("num_msg_valid_wipers");
  }

  int& get_num_wipers_not_equipped()
  {
    return getattr<int>("num_wipers_not_equipped");
  }

  int& get_num_wipers_off()
  {
    return getattr<int>("num_wipers_off");
  }

  int& get_num_wipers_intermittent()
  {
    return getattr<int>("num_wipers_intermittent");
  }

  int& get_num_wipers_low()
  {
    return getattr<int>("num_wipers_low");
  }

  int& get_num_wipers_high()
  {
    return getattr<int>("num_wipers_high");
  }

  int& get_num_wipers_washer()
  {
    return getattr<int>("num_wipers_washer");
  }

  int& get_num_wipers_automatic()
  {
    return getattr<int>("num_wipers_automatic");
  }

  int& get_num_msg_valid_abs()
  {
    return getattr<int>("num_msg_valid_abs");
  }

  int& get_num_abs_not_equipped()
  {
    return getattr<int>("num_abs_not_equipped");
  }

  int& get_num_abs_off()
  {
    return getattr<int>("num_abs_off");
  }

  int& get_num_abs_on()
  {
    return getattr<int>("num_abs_on");
  }

  int& get_num_abs_engaged()
  {
    return getattr<int>("num_abs_engaged");
  }

  int& get_num_msg_valid_stab()
  {
    return getattr<int>("num_msg_valid_stab");
  }

  int& get_num_stab_not_equipped()
  {
    return getattr<int>("num_stab_not_equipped");
  }

  int& get_num_stab_off()
  {
    return getattr<int>("num_stab_off");
  }

  int& get_num_stab_on()
  {
    return getattr<int>("num_stab_on");
  }

  int& get_num_stab_engaged()
  {
    return getattr<int>("num_stab_engaged");
  }

  int& get_num_msg_valid_trac()
  {
    return getattr<int>("num_msg_valid_trac");
  }

  int& get_num_trac_not_equipped()
  {
    return getattr<int>("num_trac_not_equipped");
  }

  int& get_num_trac_off()
  {
    return getattr<int>("num_trac_off");
  }

  int& get_num_trac_on()
  {
    return getattr<int>("num_trac_on");
  }

  int& get_num_trac_engaged()
  {
    return getattr<int>("num_trac_engaged");
  }

  //data from madis
  string& get_pres_wx()
  {
    return getattr<string>("pres_wx");
  }

  //data from metar
  string& get_wx()
  {
    return getattr<string>("wx");
  }

  //data from madis
  int& get_road_state_1()
  {
    return getattr<int>("road_state_1");
  }

  //These appear to be used from the RWX but not the VDT
  //TODO: refactor these and their initializer to a subclass for the RWX
  double& get_fcst_air_temp()
  {
    return getattr<double>("fcst_air_temp");
  }

  double& get_fcst_wind_speed()
  {
    return getattr<double>("fcst_wind_speed");
  }

  double& get_fcst_precip_type()
  {
    return getattr<double>("fcst_precip_type");
  }

  double& get_fcst_precip_rate()
  {
    return getattr<double>("fcst_precip_rate");
  }

  double& get_fcst_rh()
  {
    return getattr<double>("fcst_rh");
  }

  double& get_fcst_visibility()
  {
    return getattr<double>("fcst_visibility");
  }

  double& get_fcst_prob_fog()
  {
    return getattr<double>("fcst_prob_fog");
  }

  double& get_fcst_road_water_phase_type()
  {
    return getattr<double>("fcst_road_water_phase_type");
  }

  double& get_fcst_road_temp()
  {
    return getattr<double>("fcst_road_temp");
  }

  std::map<std::string, std::vector<float>> field_value_map;

  vector<float> headings;
  vector<float> lat_accel;
  vector<float> lon_accel;
  vector<float> speeds;
  vector<float> steering_angle;
  vector<float> yaw;
  
  /**
   * Generate road segment statistics for input msgs and given road segments 
   * @param[in] begin_time only utilize probe messages whose obs_time >= begin_time
   * @param[in] end_time only utilize probe messages whose obs_time < end_time
   * @param[in] nn_index_map map that maps nn indices back to pair consisting
   * of vector of indices of source points and vector of distances of source
   * @param[in, out] qc_msgs  vector of vdt probe message qc msgs
   * @param[out] stats_vec  vector of vdt road segment statistics
   * @param[out] seg_index_offset_map maps segment index to offset in stats_vec
   */
  static void get_probe_message_qc_statistics(time_t begin_time, time_t end_time, const unordered_map<int, vdt_index_distance> &nn_index_map, const vector<vdt_probe_message_qc> &qc_msgs, const unordered_map<int, int> &seg_id_map, const std::vector<std::string>& stats_fields, vector<vdt_probe_message_qc_statistics> &stats_vec, unordered_map<int, int> &seg_index_offset_map);


  /**
   * Add vdt_probe_message_qc_statistics obs to stats_vec to account for ids that are not covered by probe messages. For example, this is useful to get weather coverage for road segments lacking probe messages.
   * @param[in] begin_time only utilize probe messages whose obs_time >= begin_time
   * @param[in] end_time only utilize probe messages whose obs_time < end_time
   * @param[in] stat_ids set of ids that should be added (note some of the ids may already be covered in stats_vec. These will not be added again.
   * @param[in, out] stats_vec  vector of vdt road segment statistics
   */
  static void add_non_associated_ids(time_t begin_time, time_t end_time, const vector<int> &stat_ids, const std::vector<std::string>& stats_fields, vector<vdt_probe_message_qc_statistics> &stats_vec);


  /**
   * Get min and max of vector. Sets min and max to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals  vector of values
   * @param[out] min  minimum value
   * @param[out] max  maximum value
   */
  static void get_min_max(const vector<float> &vals, float &min, float &max);

  static void get_min_max(const vector<float> &vals, double &min, double &max)
  {
    float fmin, fmax;
    get_min_max(vals, fmin, fmax);
    min = fmin;
    max = fmax;
  }

  /**
   * Get mean of vector. Sets mean to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals   vector of values
   * @param[out] mean  mean value
   */
  static void get_mean(const vector<float> &vals, float &mean);

  static void get_mean(const vector<float> &vals, double &mean)
  {
    float fmean;
    get_mean(vals, fmean);
    mean = fmean;
  }

  /**
   * Get median of vector. Sets median to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals     vector of values
   * @param[out] median  median value
   */
  static void get_median(const vector<float> &vals, float &median);

  static void get_median(const vector<float> &vals, double &median)
  {
    float fmedian;
    get_median(vals, fmedian);
    median = fmedian;
  }

  /**
   * Get variance of vector. Sets variance to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals  vector of values
   * @param[out] var  variance
   */
  static void get_var(const vector<float> &vals, float &var);

  static void get_var(const vector<float> &vals, double &var)
  {
    float fvar;
    get_var(vals, fvar);
    var = fvar;
  }

  /**
   * Get standard deviation of vector. Sets stdev to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals    vector of values
   * @param[out] stdev  standard deviation
   */
  static void get_stdev(const vector<float> &vals, float &stdev);

  static void get_stdev(const vector<float> &vals, double &stdev)
  {
    float fstdev;
    get_stdev(vals, fstdev);
    stdev = fstdev;
  }

 /** @brief probe message header used for testing */
  string probe_message_statistics_hdr() const
  {
    return message_hdr();
  }

  /** @brief probe message string used for testing */
  string probe_message_statistics_string() const
  {
    return message_string();
  }

protected:
  /**
   * Add value to vector if non-fill value
   * @param[in] val    value
   * @param[out] vals  vector of values
   */
  static void add_value(vector<float>& vals, float val);
};

#endif
