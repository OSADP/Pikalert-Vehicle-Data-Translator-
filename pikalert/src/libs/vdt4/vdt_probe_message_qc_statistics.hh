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
class vdt_probe_message_qc_statistics
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

  /** @brief constructor initializes statistics variables */
  vdt_probe_message_qc_statistics();

  /** @brief init initialization routine */
  void init();

  /**
   * @brief constructor constructs statistics using messages
   * @param[in] msgs probe message qc input vector used for calculating statistics
   */
  vdt_probe_message_qc_statistics(time_t begin_time_value, time_t end_time_value, const vector<vdt_probe_message_qc> &msgs);

  /** @brief identifier identifer for statistics (for example road segment id or grid cell id) */
  int id;

  /** @brief begin_time begin time of interval for statistics */
  time_t begin_time;

  /** @brief end_time begin time of interval for statistics */
  time_t end_time;

  /** @brief road name */
  string road_name;

  /** @brief model air temperature */
  float model_air_temp;

  /** @brief model dewpoint */
  float model_dewpoint;

  /** @brief model barometric pressure */
  float model_bar_press;

  /** @brief reflectivity from radar grid in dBZ */
  float radar_ref;

  /** @brief radar precipitation flag */
  short radar_precip_flag;

  /** @brief radar precipitation type having values no_radar_coverage, no_precipitation, liquid, frozen associated with -1, 0, 1, 3 */
  short radar_precip_type;

  /** @brief cloud_mask cloud mask value */
  short cloud_mask;

  /** @brief mean air temperature  in celsius for the nearest surface stations */
  float nss_air_temp_mean;

  /** @brief mean barometric pressure  in mb for the nearest surface stations */
  float nss_bar_press_mean;

  /** @brief mean dewpoint temperature  in celsius for the nearest surface stations */
  float nss_dew_temp_mean;

  /** @brief mean wind direction in degrees for the nearest surface stations*/
  float nss_wind_dir_mean;

  /** @brief mean wind speed in m/s for the nearest surface stations */
  float nss_wind_speed_mean;

  /** @brief mean hourly precipitation in cubic centimeters for the nearest surface stations */
  float nss_hourly_precip_mean;

  /** @brief mean prevailing visibility in km for the nearest surface stations */
  float nss_prevail_vis_mean;

  /** @brief number of messages in road segment */
  int num_msg;

  /** @brief number of messages having valid air temperature */
  int num_msg_valid_air_temp;

  /** @brief minimum air temperature */
  float air_temp_min;

  /** @brief maximum air temperature */
  float air_temp_max;

  /** @brief mean air temperature */
  float air_temp_mean;

  /** @brief median air temperature */
  float air_temp_median;

  /** @brief 25th percentile air temperature */
  float air_temp_iqr25;

  /** @brief 75th percentile air temperature */
  float air_temp_iqr75;

  /** @brief air temperature variance */
  float air_temp_var;

  /** @brief air temperature standard deviation */
  float air_temp_stdev;

  /** @brief number of messages having valid external air temperature */
  int num_msg_valid_air_temp2;

  /** @brief minumum external air temperature */
  float air_temp2_min;

  /** @brief maximum external air temperature */
  float air_temp2_max;

  /** @brief mean external air temperature */
  float air_temp2_mean;

  /** @brief median external air temperature */
  float air_temp2_median;

  /** @brief 25th percentile external air temperature */
  float air_temp2_iqr25;

  /** @brief 75th percentile external air temperature */
  float air_temp2_iqr75;

  /** @brief external air temperature variance */
  float air_temp2_var;

  /** @brief external air temperature standard deviation */
  float air_temp2_stdev;

  /** @brief number of messages having valid barometric pressure */
  int num_msg_valid_bar_press;

  /** @brief minumum barometric pressure */
  float bar_press_min;

  /** @brief maximum barometric pressure */
  float bar_press_max;

  /** @brief mean barometric pressure */
  float bar_press_mean;

  /** @brief median barometric pressure */
  float bar_press_median;

  /** @brief 25th percentile barometric pressure */
  float bar_press_iqr25;

  /** @brief 75th percentile air temperature */
  float bar_press_iqr75;

  /** @brief barometric pressure variance */
  float bar_press_var;

  /** @brief barometric pressure standard deviation */
  float bar_press_stdev;

  /** @brief number of messages having valid dewpoint temperature */
  int num_msg_valid_dew_temp;

  /** @brief maximum dew temperature */
  float dew_temp_min;

  /** @brief maximum dew temperature */
  float dew_temp_max;

  /** @brief mean dew temperature */
  float dew_temp_mean;

  /** @brief median dew temperature */
  float dew_temp_median;

  /** @brief 25th percentile dew temperature */
  float dew_temp_iqr25;

  /** @brief 75th percentile dew temperature */
  float dew_temp_iqr75;

  /** @brief dew temperature variance */
  float dew_temp_var;

  /** @brief dew temperature standard deviation */
  float dew_temp_stdev;

  /** @brief number of messages having valid heading */
  int num_msg_valid_heading;

  /** @brief minimum heading */
  float heading_min;

  /** @brief maximum heading */
  float heading_max;

  /** @brief mean heading */
  float heading_mean;

  /** @brief median heading */
  float heading_median;

  /** @brief 25th percentile heading */
  float heading_iqr25;

  /** @brief 75th percentile heading */
  float heading_iqr75;

  /** @brief heading variance */
  float heading_var;

  /** @brief heading standard deviation */
  float heading_stdev;

  /** @brief number of messages having valid lateral acceleration */
  int num_msg_valid_lat_accel;

  /** @brief minumum horizontal lateral acceleration */
  float lat_accel_min;

  /** @brief maximum horizontal lateral acceleration */
  float lat_accel_max;

  /** @brief mean horizontal lateral acceleration */
  float lat_accel_mean;

  /** @brief median horizontal lateral acceleration */
  float lat_accel_median;

  /** @brief 25th percentile lateral acceleration */
  float lat_accel_iqr25;

  /** @brief 75th percentile lateral acceleration */
  float lat_accel_iqr75;

  /** @brief horizontal lateral acceleration variance */
  float lat_accel_var;

  /** @brief horizontal lateral acceleration standard deviation */
  float lat_accel_stdev;

  /** @brief number of messages having valid longitudinal acceleration */
  int num_msg_valid_lon_accel;

  /** @brief minumum horizontal longitudinal acceleration */
  float lon_accel_min;

  /** @brief maximum horizontal longitudinal acceleration */
  float lon_accel_max;

  /** @brief mean horizontal longitudinal acceleration */
  float lon_accel_mean;

  /** @brief median horizontal longitudinal acceleration */
  float lon_accel_median;

  /** @brief 25th percentile longitudinal acceleration */
  float lon_accel_iqr25;

  /** @brief 75th percentile longitudinal acceleration */
  float lon_accel_iqr75;

  /** @brief horizontal longitudinal acceleration variance */
  float lon_accel_var;

  /** @brief horizontal longitudinal acceleration standard deviation */
  float lon_accel_stdev;

  /** @brief number of messages having valid yaw */
  int num_msg_valid_yaw;

  /** @brief minumum yaw rate */
  float yaw_min;

  /** @brief maximum yaw rate */
  float yaw_max;

  /** @brief mean yaw rate */
  float yaw_mean;

  /** @brief median yaw rate */
  float yaw_median;

  /** @brief 25th percentile yaw */
  float yaw_iqr25;

  /** @brief 75th percentile yaw */
  float yaw_iqr75;

  /** @brief yaw rate variance */
  float yaw_var;

  /** @brief yaw rate standard deviation */
  float yaw_stdev;

  /** @brief number of messages having valid steering angle */
  int num_msg_valid_steering_angle;

  /** @brief minumum steering angle */
  float steering_angle_min;

  /** @brief maximum steering angle */
  float steering_angle_max;

  /** @brief mean steering angle */
  float steering_angle_mean;

  /** @brief median steering angle */
  float steering_angle_median;

  /** @brief 25th percentile steering angle */
  float steering_angle_iqr25;

  /** @brief 75th percentile steering angle */
  float steering_angle_iqr75;

  /** @brief steering angle variance */
  float steering_angle_var;

  /** @brief steering angle standard deviation */
  float steering_angle_stdev;

  /** @brief number of messages having valid lights information */
  int num_msg_valid_lights;

  /** @brief number of messages with all lights off */
  int num_lights_off;

  /** @brief number of messages with hazard lights on */
  int num_lights_hazard;

  /** @brief number of messages with parking lights on */
  int num_lights_parking;

  /** @brief number of messages with fog lights on */
  int num_lights_fog;

  /** @brief number of messages with daytime running lights on */
  int num_lights_drl;

  /** @brief number of messages with automatic lights control */
  int num_lights_automatic_control;

  /** @brief number of messages with right turn signal on */
  int num_lights_right_turn;

  /** @brief number of messages with left turn signal on */
  int num_lights_left_turn;

  /** @brief number of messages with high beam on */
  int num_lights_high_beam;

  /** @brief number of messages with low beam on */
  int num_lights_low_beam;

  /** @brief number of messages having valid speed */
  int num_msg_valid_speed;

  /** @brief average speed to speed limit ratio */
  float speed_ratio;

  /** @brief minumum speed */
  float speed_min;

  /** @brief maximum speed */
  float speed_max;

  /** @brief mean speed */
  float speed_mean;

  /** @brief median speed */
  float speed_median;

  /** @brief 25th percentile speed */
  float speed_iqr25;

  /** @brief 75th percentile speed */
  float speed_iqr75;

  /** @brief speed variance */
  float speed_var;

  /** @brief speed standard deviation */
  float speed_stdev;

  /** @brief number of messages having valid surface temperature */
  int num_msg_valid_surface_temp;

  /** @brief minumum surface temperature */
  float surface_temp_min;

  /** @brief maximum surface temperature */
  float surface_temp_max;

  /** @brief mean surface temperature */
  float surface_temp_mean;

  /** @brief median surface temperature */
  float surface_temp_median;

  /** @brief 25th percentile surface_temp */
  float surface_temp_iqr25;

  /** @brief 75th percentile surface_temp */
  float surface_temp_iqr75;

  /** @brief surface temperature variance */
  float surface_temp_var;

  /** @brief surface temperature standard deviation */
  float surface_temp_stdev;

  /** @brief number of messages having valid wiper information */
  int num_msg_valid_wipers;

  /** @brief number of messages with wiper sensor not equipped */
  int num_wipers_not_equipped;

  /** @brief number of messages with wipers off */
  int num_wipers_off;

  /** @brief number of messages with wipers intermittent */
  int num_wipers_intermittent;

  /** @brief number of messages with wipers low */
  int num_wipers_low;

  /** @brief number of messages with wipers high */
  int num_wipers_high;

  /** @brief number of messages with wiper washer on */
  int num_wipers_washer;

  /** @brief number of messages with automatic wiper control on */
  int num_wipers_automatic;

  /** @brief number of messages having valid anti-lock breaking */
  int num_msg_valid_abs;

  /** @brief number of messages with abs not equipped */
  int num_abs_not_equipped;

  /** @brief number of messages with abs off */
  int num_abs_off;

  /** @brief number of messages with abs on */
  int num_abs_on;

  /** @brief number of messages with abs engaged */
  int num_abs_engaged;

  /** @brief number of messages having valid stability control */
  int num_msg_valid_stab;

  /** @brief number of messages with stability control not equipped */
  int num_stab_not_equipped;

  /** @brief number of messages with stability control off */
  int num_stab_off;

  /** @brief number of messages with stability control on */
  int num_stab_on;

  /** @brief number of messages with stability control engaged */
  int num_stab_engaged;

  /** @brief number of messages having valid traction */
  int num_msg_valid_trac;

  /** @brief number of messages with traction control not equipped */
  int num_trac_not_equipped;

  /** @brief number of messages with traction control off */
  int num_trac_off;

  /** @brief number of messages with traction control on */
  int num_trac_on;

  /** @brief number of messages with traction control engaged */
  int num_trac_engaged;

  vector<float> air_temps;
  vector<float> air_temps2;
  vector<float> bar_press;
  vector<float> dew_temps;
  vector<float> headings;
  vector<float> lat_accel;
  vector<float> lon_accel;
  vector<float> speeds;
  vector<float> steering_angle;
  vector<float> surface_temps;
  vector<float> yaw;

  /** Forecast variables */
  
  float fcst_air_temp;

  float fcst_wind_speed;
  
  float fcst_precip_type;
  
  float fcst_precip_rate;
  
  float fcst_rh;
  
  float fcst_visibility;
  
  float fcst_prob_fog;
  
  float fcst_road_water_phase_type;
  
  float fcst_road_temp;
  
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
  static void get_probe_message_qc_statistics(time_t begin_time, time_t end_time, const unordered_map<int, vdt_index_distance> &nn_index_map, const vector<vdt_probe_message_qc> &qc_msgs, const unordered_map<int, int> &seg_id_map, vector<vdt_probe_message_qc_statistics> &stats_vec, unordered_map<int, int> &seg_index_offset_map);


  /**
   * Add vdt_probe_message_qc_statistics obs to stats_vec to account for ids that are not covered by probe messages. For example, this is useful to get weather coverage for road segments lacking probe messages.
   * @param[in] begin_time only utilize probe messages whose obs_time >= begin_time
   * @param[in] end_time only utilize probe messages whose obs_time < end_time
   * @param[in] stat_ids set of ids that should be added (note some of the ids may already be covered in stats_vec. These will not be added again.
   * @param[in, out] stats_vec  vector of vdt road segment statistics
   */
  static void add_non_associated_ids(time_t begin_time, time_t end_time, const vector<int> &stat_ids, vector<vdt_probe_message_qc_statistics> &stats_vec);


  /**
   * Get min and max of vector. Sets min and max to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals  vector of values
   * @param[out] min  minimum value
   * @param[out] max  maximum value
   */
  static void get_min_max(const vector<float> &vals, float &min, float &max);

  /**
   * Get mean of vector. Sets mean to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals   vector of values
   * @param[out] mean  mean value
   */
  static void get_mean(const vector<float> &vals, float &mean);

  /**
   * Get median of vector. Sets median to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals     vector of values
   * @param[out] median  median value
   */
  static void get_median(const vector<float> &vals, float &median);

  /**
   * Get variance of vector. Sets variance to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals  vector of values
   * @param[out] var  variance
   */
  static void get_var(const vector<float> &vals, float &var);

  /**
   * Get standard deviation of vector. Sets stdev to vdt_const::FILL_VALUE if vals.size() == 0.
   * @param[in] vals    vector of values
   * @param[out] stdev  standard deviation
   */
  static void get_stdev(const vector<float> &vals, float &stdev);

  /** @brief probe message header used for testing */
  static string probe_message_statistics_hdr()
  {
//    return vdt_probe_message_qc_statistics::probe_message_statistics_hdr() + ",num_msg_valid_abs,num_abs_not_equipped,num_abs_off,num_abs_on,num_abs_engaged,model_air_temp,model_dewpoint,model_bar_press,nss_air_temp_mean,nss_bar_press_mean,nss_dew_temp_mean,nss_hourly_precip_mean,nss_prevail_vis_mean,nss_wind_dir_mean,nss_wind_speed_mean,radar_ref,radar_precip_flag,radar_precip_type,cloud_mask,num_msg_valid_air_temp,air_temp_min,air_temp_max,air_temp_mean,air_temp_median,air_temp_var,air_temp_stdev,air_temp_iqr25,air_temp_iqr75,num_msg_valid_air_temp2,air_temp2_min,air_temp2_max,air_temp2_mean,air_temp2_median,air_temp2_var,air_temp2_stdev,air_temp2_iqr25,air_temp2_iqr75,num_msg_valid_bar_press,bar_press_min,bar_press_max,bar_press_mean,bar_press_median,bar_press_var,bar_press_stdev,bar_press_iqr25,bar_press_iqr75,num_msg_valid_brakes,num_brakes_all_off,num_brakes_all_on,num_brakes_rr_active,num_brakes_rf_active,num_brakes_lr_active,num_brakes_lf_active,num_msg_valid_brakes_boost,num_brakes_boost_not_equipped,num_brakes_boost_off,num_brakes_boost_on,num_msg_valid_dew_temp,dew_temp_min,dew_temp_max,dew_temp_mean,dew_temp_median,dew_temp_var,dew_temp_stdev,dew_temp_iqr25,dew_temp_iqr75,num_msg_valid_heading,heading_min,heading_max,heading_mean,heading_median,heading_var,heading_stdev,heading_iqr25,heading_iqr75,num_msg_valid_hoz_accel_lat,hoz_accel_lat_min,hoz_accel_lat_max,hoz_accel_lat_mean,hoz_accel_lat_median,hoz_accel_lat_var,hoz_accel_lat_stdev,hoz_accel_lat_iqr25,hoz_accel_lat_iqr75,num_msg_valid_hoz_accel_lon,hoz_accel_lon_min,hoz_accel_lon_max,hoz_accel_lon_mean,hoz_accel_lon_median,hoz_accel_lon_var,hoz_accel_lon_stdev,hoz_accel_lon_iqr25,hoz_accel_lon_iqr75,num_msg_valid_lights,num_lights_off,num_lights_hazard,num_lights_parking,num_lights_fog,num_lights_drl,num_lights_automatic_control,num_lights_right_turn,num_lights_left_turn,num_lights_high_beam,num_lights_low_beam,num_msg_valid_speed,speed_min,speed_ratio,speed_max,speed_mean,speed_median,speed_var,speed_stdev,speed_iqr25,speed_iqr75,num_msg_valid_stab,num_stab_not_equipped,num_stab_off,num_stab_on,num_stab_engaged,num_msg_valid_steering_angle,steering_angle_min,steering_angle_max,steering_angle_mean,steering_angle_median,steering_angle_var,steering_angle_stdev,steering_angle_iqr25,steering_angle_iqr75,num_msg_valid_steering_rate,steering_rate_min,steering_rate_max,steering_rate_mean,steering_rate_median,steering_rate_var,steering_rate_stdev,num_msg_valid_surface_temp,surface_temp_min,surface_temp_max,surface_temp_mean,surface_temp_median,surface_temp_var,surface_temp_stdev,surface_temp_iqr25,surface_temp_iqr75,num_msg_valid_trac,num_trac_not_equipped,num_trac_off,num_trac_on,num_trac_engaged,num_msg_valid_wipers,num_wipers_not_equipped,num_wipers_off,num_wipers_intermittent,num_wipers_low,num_wipers_high,num_wipers_washer,num_wipers_automatic,num_msg_valid_yaw,yaw_min,yaw_max,yaw_mean,yaw_median,yaw_var,yaw_stdev,yaw_iqr25,yaw_iqr75";
    return  "num_msg_valid_abs,num_abs_not_equipped,num_abs_off,num_abs_on,num_abs_engaged,model_air_temp,model_dewpoint,model_bar_press,nss_air_temp_mean,nss_bar_press_mean,nss_dew_temp_mean,nss_hourly_precip_mean,nss_prevail_vis_mean,nss_wind_dir_mean,nss_wind_speed_mean,radar_ref,radar_precip_flag,radar_precip_type,cloud_mask,num_msg_valid_air_temp,air_temp_min,air_temp_max,air_temp_mean,air_temp_median,air_temp_var,air_temp_stdev,air_temp_iqr25,air_temp_iqr75,num_msg_valid_air_temp2,air_temp2_min,air_temp2_max,air_temp2_mean,air_temp2_median,air_temp2_var,air_temp2_stdev,air_temp2_iqr25,air_temp2_iqr75,num_msg_valid_bar_press,bar_press_min,bar_press_max,bar_press_mean,bar_press_median,bar_press_var,bar_press_stdev,bar_press_iqr25,bar_press_iqr75,num_msg_valid_dew_temp,dew_temp_min,dew_temp_max,dew_temp_mean,dew_temp_median,dew_temp_var,dew_temp_stdev,dew_temp_iqr25,dew_temp_iqr75,num_msg_valid_heading,heading_min,heading_max,heading_mean,heading_median,heading_var,heading_stdev,heading_iqr25,heading_iqr75,num_msg_valid_lights,num_lights_off,num_lights_hazard,num_lights_parking,num_lights_fog,num_lights_drl,num_lights_automatic_control,num_lights_right_turn,num_lights_left_turn,num_lights_high_beam,num_lights_low_beam,num_msg_valid_speed,speed_min,speed_ratio,speed_max,speed_mean,speed_median,speed_var,speed_stdev,speed_iqr25,speed_iqr75,num_msg_valid_stab,num_stab_not_equipped,num_stab_off,num_stab_on,num_stab_engaged,num_msg_valid_steering_angle,steering_angle_min,steering_angle_max,steering_angle_mean,steering_angle_median,steering_angle_var,steering_angle_stdev,steering_angle_iqr25,steering_angle_iqr75,num_msg_valid_surface_temp,surface_temp_min,surface_temp_max,surface_temp_mean,surface_temp_median,surface_temp_var,surface_temp_stdev,surface_temp_iqr25,surface_temp_iqr75,num_msg_valid_trac,num_trac_not_equipped,num_trac_off,num_trac_on,num_trac_engaged,num_msg_valid_wipers,num_wipers_not_equipped,num_wipers_off,num_wipers_intermittent,num_wipers_low,num_wipers_high,num_wipers_washer,num_wipers_automatic,num_msg_valid_yaw,yaw_min,yaw_max,yaw_mean,yaw_median,yaw_var,yaw_stdev,yaw_iqr25,yaw_iqr75";

  }
  /** @brief probe message string used for testing */
  string probe_message_statistics_string()
  {
    stringstream oss;
    
//    oss << vdt_probe_message_qc_statistics::probe_message_statistics_string() << "," << num_msg_valid_abs << "," << num_abs_not_equipped << "," << num_abs_off << "," << num_abs_on << "," << num_abs_engaged << "," << model_air_temp << "," << model_dewpoint << "," << model_bar_press << "," << nss_air_temp_mean << "," << nss_bar_press_mean << "," << nss_dew_temp_mean << "," << nss_hourly_precip_mean << "," << nss_prevail_vis_mean << "," << nss_wind_dir_mean << "," << nss_wind_speed_mean << "," << radar_ref << "," << static_cast<int>(radar_precip_flag) << "," << static_cast<int>(radar_precip_type) << "," << static_cast<int>(cloud_mask) << "," << num_msg_valid_air_temp << "," << air_temp_min << "," << air_temp_max << "," << air_temp_mean << "," << air_temp_median << "," << air_temp_var << "," << air_temp_stdev << "," << air_temp_iqr25 << "," << air_temp_iqr75 << "," << num_msg_valid_air_temp << "," << air_temp2_min << "," << air_temp2_max << "," << air_temp2_mean << "," << air_temp2_median << "," << air_temp2_var << "," << air_temp2_stdev << "," << air_temp2_iqr25 << "," << air_temp2_iqr75 << "," << num_msg_valid_bar_press << "," << bar_press_min << "," << bar_press_max << "," << bar_press_mean << "," << bar_press_median << "," << bar_press_var << "," << bar_press_stdev << "," << bar_press_iqr25 << "," << bar_press_iqr75 << "," << num_msg_valid_brakes << "," << num_brakes_all_off << "," << num_brakes_all_on << "," << num_brakes_rr_active << "," << num_brakes_rf_active << "," << num_brakes_lr_active << "," << num_brakes_lf_active << "," << num_msg_valid_brakes_boost << "," << num_brakes_boost_not_equipped << "," << num_brakes_boost_off << "," << num_brakes_boost_on << "," << num_msg_valid_dew_temp << "," << dew_temp_min << "," << dew_temp_max << "," << dew_temp_mean << "," << dew_temp_median << "," << dew_temp_var << "," << dew_temp_stdev << "," << dew_temp_iqr25 << "," << dew_temp_iqr75 << "," << num_msg_valid_heading << "," << heading_min << "," << heading_max << "," << heading_mean << "," << heading_median << "," << heading_var << "," << heading_stdev << "," << heading_iqr25 << "," << heading_iqr75 << "," << num_msg_valid_hoz_accel_lat << "," << hoz_accel_lon_min << "," << hoz_accel_lon_max << "," << hoz_accel_lat_mean << "," << hoz_accel_lon_median << "," << hoz_accel_lon_var << "," << hoz_accel_lon_stdev << "," << hoz_accel_lon_iqr25 << "," << hoz_accel_lat_iqr75 << "," << num_msg_valid_hoz_accel_lon << "," << hoz_accel_lat_min << "," << hoz_accel_lon_max << "," << hoz_accel_lon_mean << "," << hoz_accel_lon_median << "," << hoz_accel_lon_var << "," << hoz_accel_lon_stdev << "," << hoz_accel_lon_iqr25 << "," << hoz_accel_lon_iqr75 << "," << num_msg_valid_lights << "," << num_lights_off << "," << num_lights_hazard << "," << num_lights_parking << "," << num_lights_fog << "," << num_lights_drl << "," << num_lights_automatic_control << "," << num_lights_right_turn << "," << num_lights_left_turn << "," << num_lights_high_beam << "," << num_lights_low_beam << "," << num_msg_valid_speed << "," << speed_min << "," << speed_max << "," << speed_mean << "," << speed_median << "," << speed_median << "," << speed_var << "," << speed_stdev << "," << speed_iqr25 << "," << speed_iqr75 << "," << num_msg_valid_stab << "," << num_stab_not_equipped << "," << num_stab_off << "," << num_stab_on << "," << num_stab_engaged << "," << num_msg_valid_steering_angle << "," << steering_angle_min << "," << steering_angle_max << "," << <steering_angle_mean < "," << steering_angle_median << "," << steering_angle_var << "," << steering_angle_stdev << "," << steering_angle_iqr25 << "," << steering_angle_iqr75 << "," << num_msg_valid_steering_rate << "," << steering_rate_min << "," << steering_rate_max << "," << steering_rate_mean << "," << steering_rate_median << "," << steering_rate_var << "," << steering_rate_stdev << "," << num_msg_valid_surface_temp << "," << surface_temp_min << "," << surface_temp_max << "," << surface_temp_mean << "," << <surface_temp_median < "," << surface_temp_var << "," << surface_temp_stdev << "," << surface_temp_iqr25 << "," << surface_temp_iqr75 << "," << num_msg_valid_trac << "," << num_trac_not_equipped << "," << num_trac_off << "," << num_trac_on << "," << num_trac_engaged << "," << num_msg_valid_wipers << "," << num_wipers_not_equipped << "," << num_wipers_off << "," << num_wipers_intermittent << "," << num_wipers_low << "," << num_wipers_high << "," << num_wipers_washer << "," << num_wipers_automatic << "," << num_msg_valid_yaw << "," << yaw_min << "," << yaw_max << "," << yaw_mean << "," << yaw_median << "," << yaw_var << "," << yaw_stdev << "," << yaw_iqr25 << "," << yaw_iqr75 ;
    oss << num_msg_valid_abs << "," << num_abs_not_equipped << "," << num_abs_off << "," << num_abs_on << "," << num_abs_engaged << "," << model_air_temp << "," << model_dewpoint << "," << model_bar_press << "," << nss_air_temp_mean << "," << nss_bar_press_mean << "," << nss_dew_temp_mean << "," << nss_hourly_precip_mean << "," << nss_prevail_vis_mean << "," << nss_wind_dir_mean << "," << nss_wind_speed_mean << "," << radar_ref << "," << static_cast<int>(radar_precip_flag) << "," << static_cast<int>(radar_precip_type) << "," << static_cast<int>(cloud_mask) << "," << num_msg_valid_air_temp << "," << air_temp_min << "," << air_temp_max << "," << air_temp_mean << "," << air_temp_median << "," << air_temp_var << "," << air_temp_stdev << "," << air_temp_iqr25 << "," << air_temp_iqr75 << "," << num_msg_valid_air_temp << "," << air_temp2_min << "," << air_temp2_max << "," << air_temp2_mean << "," << air_temp2_median << "," << air_temp2_var << "," << air_temp2_stdev << "," << air_temp2_iqr25 << "," << air_temp2_iqr75 << "," << num_msg_valid_bar_press << "," << bar_press_min << "," << bar_press_max << "," << bar_press_mean << "," << bar_press_median << "," << bar_press_var << "," << bar_press_stdev << "," << bar_press_iqr25 << "," << bar_press_iqr75 << ","  << num_msg_valid_dew_temp << "," << dew_temp_min << "," << dew_temp_max << "," << dew_temp_mean << "," << dew_temp_median << "," << dew_temp_var << "," << dew_temp_stdev << "," << dew_temp_iqr25 << "," << dew_temp_iqr75 << "," << num_msg_valid_heading << "," << heading_min << "," << heading_max << "," << heading_mean << "," << heading_median << "," << heading_var << "," << heading_stdev << "," << heading_iqr25 << "," << heading_iqr75 << "," <<  num_msg_valid_lights << "," << num_lights_off << "," << num_lights_hazard << "," << num_lights_parking << "," << num_lights_fog << "," << num_lights_drl << "," << num_lights_automatic_control << "," << num_lights_right_turn << "," << num_lights_left_turn << "," << num_lights_high_beam << "," << num_lights_low_beam << "," << num_msg_valid_speed << "," << speed_min << "," << speed_max << "," << speed_mean << "," << speed_median << "," << speed_median << "," << speed_var << "," << speed_stdev << "," << speed_iqr25 << "," << speed_iqr75 << "," << num_msg_valid_stab << "," << num_stab_not_equipped << "," << num_stab_off << "," << num_stab_on << "," << num_stab_engaged << "," << num_msg_valid_steering_angle << "," << steering_angle_min << "," << steering_angle_max << "," << steering_angle_mean << "," << steering_angle_median << "," << steering_angle_var << "," << steering_angle_stdev << "," << steering_angle_iqr25 << "," << steering_angle_iqr75 << "," <<  num_msg_valid_surface_temp << "," << surface_temp_min << "," << surface_temp_max << "," << surface_temp_mean << "," << surface_temp_median << "," << surface_temp_var << "," << surface_temp_stdev << "," << surface_temp_iqr25 << "," << surface_temp_iqr75 << "," << num_msg_valid_trac << "," << num_trac_not_equipped << "," << num_trac_off << "," << num_trac_on << "," << num_trac_engaged << "," << num_msg_valid_wipers << "," << num_wipers_not_equipped << "," << num_wipers_off << "," << num_wipers_intermittent << "," << num_wipers_low << "," << num_wipers_high << "," << num_wipers_washer << "," << num_wipers_automatic << "," << num_msg_valid_yaw << "," << yaw_min << "," << yaw_max << "," << yaw_mean << "," << yaw_median << "," << yaw_var << "," << yaw_stdev << "," << yaw_iqr25 << "," << yaw_iqr75 ;
  
    return oss.str();
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
