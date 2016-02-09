//==============================================================================
//
//   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_qc.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/05/29 17:54:47 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_qc.hh
 *
 * @class vdt_probe_message_qc
 *
 *  Extends vdt_probe_message class adding qc variables and methods
 */

#ifndef VDT_PROBE_MESSAGE_QC_HH
#define VDT_PROBE_MESSAGE_QC_HH

#include <algorithm>
#include <vector>
#include <string>

#include "vdt_probe_message.hh"
#include <vdt4/vdt_road_segment.hh>

using std::string;
using std::vector;

//Namespace to prevent name collisions with similar class in VDT 
namespace rwx
{

/**
 * @class vdt_probe_message_qc
 * Probe message class with quality control information
 */
class vdt_probe_message_qc : public vdt_probe_message
{
public:

  /** @brief constructor */
  vdt_probe_message_qc();

  /**
   * @brief constructor
   * @param[in] probe_message 
   */
  vdt_probe_message_qc(const vdt_probe_message &probe_message);

  /** @brief relational operator */
  bool operator < (const vdt_probe_message_qc &probe_message_qc) const;

  /** @brief enum for the quality control flag values */
  enum Result { FAILED = 0, PASSED = 1, NOT_APPLICABLE = 255};

  /** @brief vector storing lights values*/
  vector<short> light_vector;

  /** @brief road segment */
  vdt_road_segment road_segment;

  /** @brief road segment identifier */
  int road_segment_id;

  /** @brief mean air temperature for the nearest surface stations */
  float nss_air_temp_mean;

  /** @brief mean surface (pavement) temperature for the nearest surface stations */
  float nss_surface_temp_mean;
  
  /** @brief air temperature from the model data */
  float model_air_temp;

  /** @brief air temperature from the model data */
  float model_dewpoint;

  /** @brief mean barometric pressure for the nearest surface stations */
  float nss_bar_press_mean;

  /** @brief barometric pressure from the model data */
  float model_bar_press;

  /** @brief reflectivity from radar grid */
  float radar_ref;

  /** @brief precipitation flag  from radar grid */
  short radar_precip_flag;

  /** @brief precipitation type from radar grid  */
  short radar_precip_type;

  /** @brief cloud mask from satellite grid */
  short cloud_mask;

  /** @brief mean dew temperature for the nearest surface stations */
  float nss_dew_temp_mean;

  /** @brief mean wind direction for the nearest surface stations */
  float nss_wind_dir_mean;

  /** @brief  mean wind speed for the nearest surface stations */
  float nss_wind_speed_mean;

  /** @brief  mean hourly precipitation for the nearest surface stations */
  float nss_hourly_precip_mean;

  /** @brief  mean prevailing visibility for the nearest surface stations */
  float nss_prevail_vis_mean;

  /**
    * 1, if full_pos_heading value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char heading_range_qc_passed;

  /**
    * 1, if full_pos_speed value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char speed_range_qc_passed;

  /**
    * 1, if brake_status value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char brake_status_range_qc_passed;

  /**
    * 1, if brake_boost value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char brake_boost_range_qc_passed;

  /**
    * 1, if abs value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char abs_range_qc_passed;

  /**
    * 1, if elevation value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char elevation_range_qc_passed;

  /**
    * 1, if stab value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char stab_range_qc_passed;

  /**
    * 1, if trac value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char trac_range_qc_passed;

  /**
    * 1, if lights value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char lights_range_qc_passed;

  /**
    * 1, if yaw value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char yaw_range_qc_passed;

  /**
    * 1, if hoz_accel_long value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char hoz_accel_long_range_qc_passed;

  /**
    * 1, if hoz_accel_lat value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char hoz_accel_lat_range_qc_passed;

  /**
    * 1, if steering_angle value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char steering_angle_range_qc_passed;

  /**
    * 1, if steering_rate value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char steering_rate_range_qc_passed;

  /**
    * 1, if wipers_front_status value passed QC check
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char wipers_front_status_range_qc_passed;

  /**
    * 1, if air_temp2 value passed QC check for the sensor range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_range_qc_passed;

  /**
    * 1, if air_temp2 value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_sdt_passed;

  /**
    * 1, if air_temp2 value passed QC check for the nearest surface station test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_nst_passed;

  /**
    * 1, if air_temp2 value passed QC check for the model analysis test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_mat_passed;

  /**
    * 1, if air_temp2 value passed QC check for the climate range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_crt_passed;

  /**
    * 1, if air_temp2 value passed QC check for the neighboring vehicle test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_nvt_passed;

  /**
    * 1, if air_temp2 value passed QC check for the combined algorithm test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_cat_passed;

  /**
    * air_temp2_cat_confidence 
    * 0, no confidence
    * 1, completely confident
    * -9999 if not applicable
    */
  float air_temp2_cat_confidence;

  /**
    * 1, if air_temp2 value passed QC check for the persistence test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_persist_passed;

  /**
    * 1, if air_temp2 value passed QC check for the time step test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_step_passed;

  /**
    * 1, if dew_temp value passed QC check for the combined algorithm test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_cat_passed;

  /**
    * dew_temp_cat_confidence
    * 0, no confidence
    * 1, completely confident
    * -9999 if not applicable
    */
  float dew_temp_cat_confidence;

  /**
    * 1, if dew_temp value passed QC check for the model analysis test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_mat_passed;

  /**
    * 1, if dew_temp value passed QC check for the sensor range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_range_qc_passed;

  /**
    * 1, if dew_temp value passed QC check for the nearest surface station test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_nst_passed;

  /**
    * 1, if dew_temp value passed QC check for the neighboring vehicle test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_nvt_passed;

  /**
    * 1, if dew_temp value passed QC check for the climate range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_crt_passed;

  /**
    * 1, if dew_temp value passed QC check for the persistence test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_persist_passed;

  /**
    * 1, if dew_temp value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_sdt_passed;

  /**
    * 1, if dew_temp passed QC check for the barnes air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_spatial_barnes_passed;

  /**
    * 1, if dew_temp passed QC check for the IQR air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_spatial_iqr_passed;

  /**
    * 1, if dew_temp value passed QC check for the time step test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char dew_temp_step_passed;

  /**
    * 1, if surface_temp value passed QC check for the sensor range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_range_qc_passed;

  /**
    * 1, if surface_temp value passed QC check for the climate range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_crt_passed;

  /**
    * 1, if surface value passed QC check for the model analysis test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp2_mat_passed;

  /**
    * 1, if surface value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_sdt_passed;

  /**
    * 1, if surface_temp value passed QC check for the neighboring vehicle test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_nvt_passed;

  /**
    * 1, if surface_temp value passed QC check for the combined algorithm test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_cat_passed;

  /**
    * surface_temp_cat_confidence
    * 0, no confidence
    * 1, completely confident
    * -9999 if not applicable
    */
  float surface_temp_cat_confidence;

  /**
    * 1, if surface_temp value passed QC check for the persistence test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_persist_passed;

  /**
    * 1, if air_temp value passed QC check for the persistence test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_persist_passed;

  /**
    * 1, if bar_press value passed QC check for the persistence test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_persist_passed;

  /**
    * 1, if air_temp value passed QC check for the sensor range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_range_qc_passed;

  /**
    * 1, if air_temp value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_sdt_passed;

  /**
    * 1, if air_temp value passed QC check for the time step test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_step_passed;

  /**
    * 1, if surface_temp value passed QC check for the time step test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_step_passed;

  /**
    * 1, if surface_temp value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_sdt_passed;

  /**
    * 1, if bar_press value passed QC check for the time step test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_step_passed;

  /**
    * 1, if air_temp value passed QC check for the nearest surface station test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_nst_passed;

  /**
    * 1, if air_temp value passed QC check for the model analysis test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_mat_passed;

  /**
    * 1, if air_temp value passed QC check for the climate range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_crt_passed;

  /**
    * 1, if air_temp value passed QC check for the neighboring vehicle test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_nvt_passed;

  /**
    * 1, if air_temp value passed QC check for the combined algorithm test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_cat_passed;

  /**
    * air_temp_cat_confidence
    * 0, no confidence
    * 1, completely confident
    * -9999 if not applicable
    */
  float air_temp_cat_confidence;

  /**
    * 1, if bar_press value passed QC check for the sensor range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_range_qc_passed;

  /**
    * 1, if bar_press value passed QC check for the standard deviation test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_sdt_passed;

  /**
    * 1, if bar_press value passed QC check for the nearest surface station test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_nst_passed;

  /**
    * 1, if bar_press value passed QC check for the model analysis test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_mat_passed;

  /**
    * 1, if bar_press value passed QC check for the climate range test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_crt_passed;

  /**
    * 1, if bar_press value passed QC check for the neighboring vehicle test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_nvt_passed;

  /**
    * 1, if bar_press value passed QC check for the combined algorithm test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_cat_passed;

  /**
    * bar_press_cat_confidence
    * 0, no confidence
    * 1, completely confident
    * -9999 if not applicable
    */
  float bar_press_cat_confidence;

  /**
    * 1, if laitude value passed QC check for the data filtering test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char latitude_dft_passed;

  /**
    * 1, if longitude value passed QC check for the data filtering test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char longitude_dft_passed;

  /**
    * 1, if air temperature passed QC check for the IQR air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_spatial_iqr_passed;

  /**
    * 1, if air temperature passed QC check for the barnes air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp_spatial_barnes_passed;

  /**
    * 1, if second air temperature passed QC check for the IQR air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_spatial_iqr_passed;


  /**
    * 1, if second air temperature passed QC check for the barnes air temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char air_temp2_spatial_barnes_passed;

  /**
    * 1, if surface temperature passed QC check for the IQR surface temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_spatial_iqr_passed;


  /**
    * 1, if surface temperature passed QC check for the barnes surface temperature test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char surface_temp_spatial_barnes_passed;

  /**
    * 1, if barometric pressure passed QC check for the IQR barometric pressure test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_spatial_iqr_passed;


  /**
    * 1, if barometric pressure passed QC check for the spatial barnes barometric pressure test
    * 0, if failed QC check
    * 255, if not applicable (e.g. fill value)
    */
  unsigned char bar_press_spatial_barnes_passed;


  // Getters and setters

  /**
   * @brief get road_segment value
   * @return  road_segment value
   */
  inline const vdt_road_segment get_road_segment() const
  {
    return road_segment;
  }

  /**
   * @brief get road segment identifier value
   * @return road segment identifier value
   */
  inline int get_road_segment_id() const
  {
    return road_segment_id;
  }

  /**
   * @brief get mean air temperature for the nearest surface stations value
   * @return  mean air temperature for the nearest surface stations value
   */
  inline float get_nss_air_temp_mean() const
  {
    return nss_air_temp_mean;
  }

  /**
   * @brief get mean barometric pressure for the nearest surface stations value
   * @return mean barometric pressure for the nearest surface stations value
   */
  inline float get_nss_bar_press_mean() const
  {
    return nss_bar_press_mean;
  }

  /**
   * @brief get air temperature from the model data value
   * @return air temperature from the model data value
   */
  inline float get_model_air_temp() const
  {
    return model_air_temp;
  }

  /**
   * @brief get dewpoint from the model data value
   * @return dewpoint from the model data value
   */
  inline float get_model_dewpoint() const
  {
    return model_dewpoint;
  }

  /**
   * @brief get barometric pressure from the model data value
   * @return barometric pressure from the model data value
   */
  inline float get_model_bar_press() const
  {
    return model_bar_press;
  }

  /**
   * @brief get reflectivity from radar grid value
   * @return reflectivity from radar grid value
   */
  inline float get_radar_ref() const
  {
    return radar_ref;
  }

  /**
   * @brief get precip flag from radar grid value
   * @return precip flag from radar grid value
   */
  inline const short get_radar_precip_flag() const
  {
    return radar_precip_flag;
  }

  /**
   * @brief get precip type from radar grid value
   * @return precip type from radar grid value
   */
  inline const short get_radar_precip_type() const
  {
    return radar_precip_type;
  }

  /**
   * @brief get cloud mask from satellite grid value
   * @return cloud mask from satellite grid value
   */
  inline const short get_cloud_mask() const
  {
    return cloud_mask;
  }

  /**
   * @brief get mean dew temperature for the nearest surface stations value
   * @return mean dew temperature for the nearest surface stations value
   */
  inline float get_nss_dew_temp_mean() const
  {
    return nss_dew_temp_mean;
  }

  /**
   * @brief get mean wind direction for the nearest surface stations value
   * @return mean wind direction for the nearest surface stations value
   */
  inline float get_nss_wind_dir_mean() const
  {
    return nss_wind_dir_mean;
  }

  /**
   * @brief get mean wind speed for the nearest surface stations value
   * @return mean wind speed for the nearest surface stations value
   */
  inline float get_nss_wind_speed_mean() const
  {
    return nss_wind_speed_mean;
  }

  /**
   * @brief get mean hourly precipitation for the nearest surface stations value
   * @return mean hourly precipitation for the nearest surface stations value
   */
  inline float get_nss_hourly_precip_mean() const
  {
    return nss_hourly_precip_mean;
  }

  /**
   * @brief get mean prevailing visibility for the nearest surface stations value
   * @return mean prevailing visibility for the nearest surface stations value
   */
  inline float get_nss_prevail_vis_mean() const
  {
    return nss_prevail_vis_mean;
  }

  /**
   * @brief get quality control flag for heading value
   * @return quality control flag for heading value
   */
  inline unsigned char get_heading_range_qc_passed() const
  {
    return heading_range_qc_passed;
  }

  /**
   * @brief get quality control flag for speed value
   * @return quality control flag for speed value
   */
  inline unsigned char get_speed_range_qc_passed() const
  {
    return speed_range_qc_passed;
  }

  /**
   * @brief get quality control flag for brake status value
   * @return quality control flag for brake status value
   */
  inline unsigned char get_brake_status_range_qc_passed() const
  {
    return brake_status_range_qc_passed;
  }

  /**
   * @brief get quality control flag for brake boost value
   * @return quality control flag for brake boost value
   */
  inline unsigned char get_brake_boost_range_qc_passed() const
  {
    return brake_boost_range_qc_passed;
  }

  /**
   * @brief get quality control flag for anti lock brake status value
   * @return quality control flag for anti lock brake status value
   */
  inline unsigned char get_abs_range_qc_passed() const
  {
    return abs_range_qc_passed;
  }

  /**
   * @brief get quality control flag for stability value
   * @return quality control flag for stability value
   */
  inline unsigned char get_stab_range_qc_passed() const
  {
    return stab_range_qc_passed;
  }

  /**
   * @brief get quality control flag for traction value
   * @return quality control flag for traction value
   */
  inline unsigned char get_trac_range_qc_passed() const
  {
    return trac_range_qc_passed;
  }

  /**
   * @brief get quality control flag for lights value
   * @return quality control flag for lights value
   */
  inline unsigned char get_lights_range_qc_passed() const
  {
    return lights_range_qc_passed;
  }

  /**
   * @brief get quality control flag for yaw value
   * @return quality control flag for yaw value
   */
  inline unsigned char get_yaw_range_qc_passed() const
  {
    return yaw_range_qc_passed;
  }

  /**
   * @brief get quality control flag for horizontal longitudinal acceleration value
   * @return quality control flag for horizontal longitudinal acceleration value
   */
  inline unsigned char get_hoz_accel_long_range_qc_passed() const
  {
    return hoz_accel_long_range_qc_passed;
  }

  /**
   * @brief get quality control flag for horizontal lateral acceleration value
   * @return quality control flag for horizontal lateral acceleration value
   */
  inline unsigned char get_hoz_accel_lat_range_qc_passed() const
  {
    return hoz_accel_lat_range_qc_passed;
  }

  /**
   * @brief get quality control flag for elevation value
   * @return quality control flag for elevation value
   */
  inline unsigned char get_elevation_range_qc_passed() const
  {
    return elevation_range_qc_passed;
  }

  /**
   * @brief get quality control flag for steering angle value
   * @return quality control flag for steering angle value
   */
  inline unsigned char get_steering_angle_range_qc_passed() const
  {
    return steering_angle_range_qc_passed;
  }

  /**
   * @brief get quality control flag for for steering rate value
   * @return quality control flag for steering rate value
   */
  inline unsigned char get_steering_rate_range_qc_passed() const
  {
    return steering_rate_range_qc_passed;
  }

  /**
   * @brief get quality control flag for front wiper status value
   * @return  quality control flag for front wiper status value
   */
  inline unsigned char get_wipers_front_status_range_qc_passed() const
  {
    return wipers_front_status_range_qc_passed;
  }

  /**
   * @brief get air temperature sensor range test quality control flag value
   * @return  air temperature sensor range test quality control flag value
   */
  inline unsigned char get_air_temp_range_qc_passed() const
  {
    return air_temp_range_qc_passed;
  }

  /**
   * @brief get air temperature standard deviation test quality control flag value
   * @return air temperature standard deviation test quality control flag value
   */
  inline unsigned char get_air_temp_sdt_passed() const
  {
    return air_temp_sdt_passed;
  }

  /**
   * @brief get air temperature nearest surface station quality control flag value
   * @return air temperature nearest surface station quality control flag value
   */
  inline unsigned char get_air_temp_nst_passed() const
  {
    return air_temp_nst_passed;
  }

  /**
   * @brief get air temperature model analysis quality control flag value
   * @return air temperature model analysis quality control flag value
   */
  inline unsigned char get_air_temp_mat_passed() const
  {
    return air_temp_mat_passed;
  }

  /**
   * @brief get air temperature climate range test quality control flag value
   * @return air temperature climate range test quality control flag value
   */
  inline unsigned char get_air_temp_crt_passed() const
  {
    return air_temp_crt_passed;
  }

  /**
   * @brief get air temperature neighboring vehicle test quality control flag value
   * @return air temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_air_temp_nvt_passed() const
  {
    return air_temp_nvt_passed;
  }

  /**
   * @brief get air temperature combined algorithm test quality control flag value
   * @return air temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_air_temp_cat_passed() const
  {
    return air_temp_cat_passed;
  }

  /**
   * @brief get air temperature combined algorithm test confidence value
   * @return air temperature combined algorithm test confidence value
   */
  inline float get_air_temp_cat_confidence() const
  {
    return air_temp_cat_confidence;
  }

  /**
   * @brief get air temperature persistence quality control flag
   * @return air temperature persistence quality control flag value
   */
  inline unsigned char get_air_temp_persist_passed () const
  {
    return air_temp_persist_passed;
  }

  /**
   * @brief get air temperature time step quality control flag
   * @return air temperature time step quality control flag value
   */
  inline unsigned char get_air_temp_step_passed () const
  {
    return air_temp_step_passed;
  }

  /**
   * @brief get external air temperature sensor range test quality control flag  value
   * @return external air temperature sensor range test quality control flag value
   */
  inline unsigned char get_air_temp2_range_qc_passed() const
  {
    return air_temp2_range_qc_passed;
  }

  /**
   * @brief get external air temperature standard deviation test quality control flag value
   * @return  external air temperature standard deviation test quality control flag value
   */
  inline unsigned char get_air_temp2_sdt_passed() const
  {
    return air_temp2_sdt_passed;
  }

  /**
   * @brief get external air temperature nearest surface station quality control flag value
   * @return external air temperature nearest surface station quality control flag value
   */
  inline unsigned char get_air_temp2_nst_passed() const
  {
    return air_temp2_nst_passed;
  }

  /**
   * @brief get external air temperature model analysis quality control flag value
   * @return external air temperature model analysis quality control flag value
   */
  inline unsigned char get_air_temp2_mat_passed() const
  {
    return air_temp2_mat_passed;
  }

  /**
   * @brief get external air temperature climate range quality control flag value
   * @return  external air temperature climate range test
   */
  inline unsigned char get_air_temp2_crt_passed() const
  {
    return air_temp2_crt_passed;
  }

  /**
   * @brief get external air temperature neighboring vehicle quality control flag value
   * @return  external air temperature neighboring vehicle test
   */
  inline unsigned char get_air_temp2_nvt_passed() const
  {
    return air_temp2_nvt_passed;
  }

  /**
   * @brief get external air temperature combined algorithm quality control flag value
   * @return  external air temperature combined algorithm test
   */
  inline unsigned char get_air_temp2_cat_passed() const
  {
    return air_temp2_cat_passed;
  }

  /**
   * @brief get external air temperature combined algorithm confidence value
   * @return  external air temperature combined algorithm confidence value
   */
  inline float get_air_temp2_cat_confidence() const
  {
    return air_temp2_cat_confidence;
  }

  /**
   * @brief get external air temperature persistence quality control flag value
   * @return  external air temperature persistence test 
   */
  inline unsigned char get_air_temp2_persist_passed() const
  {
    return air_temp2_persist_passed;
  }

  /**
   * @brief get external air temperature time step quality control flag value
   * @return  external air temperature time step test 
   */
  inline unsigned char get_air_temp2_step_passed() const
  {
    return air_temp2_step_passed;
  }

  /**
   * @brief get dew temperature sensor range test quality control flag value
   * @return dew temperature sensor range test quality control flag value
   */
  inline unsigned char get_dew_temp_range_qc_passed() const
  {
    return dew_temp_range_qc_passed;
  }

  /**
   * @brief get dew temperature combined algorithm test quality control flag value
   * @return dew temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_dew_temp_cat_passed() const
  {
    return dew_temp_cat_passed;
  }

  /**
   * @brief get dew temperature combined algorithm test confidence value
   * @return dew temperature combined algorithm test confidence value
   */
  inline float get_dew_temp_cat_confidence() const
  {
    return dew_temp_cat_confidence;
  }

  /**
   * @brief get dew temperature model analysis quality control flag value
   * @return dew temperature model analysis quality control flag value
   */
  inline unsigned char get_dew_temp_mat_passed() const
  {
    return dew_temp_mat_passed;
  }

  /**
   * @brief get dew temperature nearest surface station quality control flag value
   * @return dew temperature nearest surface station quality control flag value
   */
  inline unsigned char get_dew_temp_nst_passed() const
  {
    return dew_temp_nst_passed;
  }

  /**
   * @brief get dew temperature spatial barnes quality control flag value
   * @return dew temperature spatial barnes quality control flag value
   */
  inline unsigned char get_dew_temp_spatial_barnes_qc_passed ()
  {
    return dew_temp_spatial_barnes_passed;
  }

  /**
   * @brief get dew temperature spatial iqr quality control flag value
   * @return dew temperature spatial iqr quality control flag value
   */
  inline unsigned char get_dew_temp_spatial_iqr_qc_passed ()
  {
    return dew_temp_spatial_iqr_passed;
  }

  /**
   * @brief get dew temperature standard deviation test quality control flag value
   * @return dew temperature standard deviation test quality control flag value
   */
  inline unsigned char get_dew_temp_sdt_passed() const
  {
    return dew_temp_sdt_passed;
  }

  /**
   * @brief get dew temperature neighboring vehicle test quality control flag value
   * @return dew temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_dew_temp_nvt_passed() const
  {
    return dew_temp_nvt_passed;
  }

  /**
   * @brief get dew temperature climate range test quality control flag value
   * @return dew temperature climate range test quality control flag value
   */
  inline unsigned char get_dew_temp_crt_passed() const
  {
    return dew_temp_crt_passed;
  }

  /**
   * @brief get dew temperature persistence quality control flag
   * @return dew temperature persistence quality control flag value
   */
  inline unsigned char get_dew_temp_persist_passed () const
  {
    return dew_temp_persist_passed;
  }

  /**
   * @brief get dew temperature time step quality control flag
   * @return dew temperature time step quality control flag value
   */
  inline unsigned char get_dew_temp_step_passed () const
  {
    return dew_temp_step_passed;
  }

  /**
   * @brief get barometric pressure sensor range test  quality control flag value
   * @return barometric pressure sensor range test  quality control flag value
   */
  inline unsigned char get_bar_press_range_qc_passed() const
  {
    return bar_press_range_qc_passed;
  }

  /**
   * @brief get barometric pressure standard deviation test quality control flag value
   * @return barometric pressure standard deviation test quality control flag value
   */
  inline unsigned char get_bar_press_sdt_passed() const
  {
    return bar_press_sdt_passed;
  }

  /**
   * @brief get barometric pressure nearest surface station quality control flag value
   * @return barometric pressure nearest surface station quality control flag value
   */
  inline unsigned char get_bar_press_nst_passed() const
  {
    return bar_press_nst_passed;
  }

  /**
   * @brief get  barometric pressure model analysis quality control flag value
   * @return  barometric pressure model analysis quality control flag value
   */
  inline unsigned char get_bar_press_mat_passed() const
  {
    return bar_press_mat_passed;
  }

  /**
   * @brief get barometric pressure climate range test quality control flag value
   * @return barometric pressure climate range test quality control flag value
   */
  inline unsigned char get_bar_press_crt_passed() const
  {
    return bar_press_crt_passed;
  }

  /**
   * @brief get barometric pressure neighboring vehicle test quality control flag value
   * @return barometric pressure neighboring vehcile test quality control flag value
   */
  inline unsigned char get_bar_press_nvt_passed() const
  {
    return bar_press_nvt_passed;
  }

  /**
   * @brief get barometric pressure combined algorithm test quality control flag value
   * @return barometric pressure combined algorithm test quality control flag value
   */
  inline unsigned char get_bar_press_cat_passed() const
  {
    return bar_press_cat_passed;
  }

  /**
   * @brief get barometric pressure combined algorithm confidence value
   * @return barometric pressure combined algorithm test confidence value
   */
  inline float get_bar_press_cat_confidence() const
  {
    return bar_press_cat_confidence;
  }

  /**
   * @brief get barometric pressure persistence quality control flag value
   * @return barometric pressure persistence quality control flag  value
   */
  inline unsigned char get_bar_press_persist_passed() const
  {
    return bar_press_persist_passed;
  }

  /**
   * @brief get barometric pressure time step quality control flag value
   * @return barometric pressure time step quality control flag  value
   */
  inline unsigned char get_bar_press_step_passed() const
  {
    return bar_press_step_passed;
  }

  /**
   * @brief get barometric pressure spatial iqr quality control flag value
   * @return barometric pressure spatial iqr quality control flag  value
   */
  inline unsigned char get_bar_press_spatial_iqr_qc_passed() const
  {
    return bar_press_spatial_iqr_passed;
  }

  /**
   * @brief get barometric pressure spatial barnes quality control flag value
   * @return barometric pressure spatial barnes quality control flag  value
   */
  inline unsigned char get_bar_press_spatial_barnes_passed() const
  {
    return bar_press_spatial_barnes_passed;
  }

  /**
   * @brief get surface temperature sensor range test quality control flag value
   * @return surface temperature sensor range test quality control flag value
   */
  inline unsigned char get_surface_temp_range_qc_passed() const
  {
    return surface_temp_range_qc_passed;
  }

  /**
   * @brief get surface temperature climate range test quality control flag value
   * @return surface temperature climate range test quality control flag value
   */
  inline unsigned char get_surface_temp_crt_passed() const
  {
    return surface_temp_crt_passed;
  }

  /**
   * @brief get surface temperature neighboring vehicle test quality control flag value
   * @return surface temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_surface_temp_nvt_passed() const
  {
    return surface_temp_nvt_passed;
  }

  /**
   * @brief get surface temperature combined algorithm test quality control flag value
   * @return surface temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_surface_temp_cat_passed() const
  {
    return surface_temp_cat_passed;
  }

  /**
   * @brief get surface temperature combined algorithm test confidence value
   * @return surface temperature combined algorithm test confidence value
   */
  inline float get_surface_temp_cat_confidence() const
  {
    return surface_temp_cat_confidence;
  }

  /**
   * @brief get surface temperature standard deviation test quality control flag value
   * @return surface temperature standard deviation test quality control flag value
   */
  inline unsigned char get_surface_temp_sdt_passed() const
  {
    return surface_temp_sdt_passed;
  }

  /**
   * @brief get latitude data filtering test quality control flag value
   * @return latitude data filtering test quality control flag value
   */
  inline unsigned char get_latitude_dft_passed() const
  {
    return latitude_dft_passed;
  }

  /**
   * @brief get longitude data filtering test quality control flag value
   * @return longitude data filtering test quality control flag value
   */
  inline unsigned char get_longitude_dft_passed() const
  {
    return longitude_dft_passed;
  }

  /**
   * @brief get air temperature spatial iqr quality control flag value
   * @return air temperature spatial iqr quality control flag value
   */
  inline unsigned char get_air_temp_spatial_iqr_qc_passed ()
  {
    return air_temp_spatial_iqr_passed;
  }

  /**
   * @brief get external air temperature spatial iqr quality control flag value
   * @return external air spatial iqr temperature quality control flag value
   */
  inline unsigned char get_air_temp2_spatial_iqr_qc_passed ()
  {
    return air_temp2_spatial_iqr_passed;
  }

  /**
   * @brief get surface temperature spatial iqr quality control flag value
   * @return surface temperature spatial iqr quality control flag value
   */
  inline unsigned char get_surface_temp_spatial_iqr_qc_passed ()
  {
    return surface_temp_spatial_iqr_passed;
  }

  /**
   * @brief get barometric pressure spatial iqr quality control flag value
   * @return barometric pressure spatial iqr quality control flag value
   */
  inline unsigned char get_bar_press_spatial_iqr_qc_passed ()
  {
    return bar_press_spatial_iqr_passed;
  }


  /**
   * @brief get air temperature spatial barnes quality control flag value
   * @return air temperature spatial barnes quality control flag value
   */
  inline unsigned char get_air_temp_spatial_barnes_qc_passed ()
  {
    return air_temp_spatial_barnes_passed;
  }

  /**
   * @brief get external air temperature spatial barnes quality control flag value
   * @return external air temperature spatial barnes quality control flag value
   */
  inline unsigned char get_air_temp2_spatial_barnes_qc_passed ()
  {
    return air_temp2_spatial_barnes_passed;
  }

  /**
   * @brief get surface temperature spatial barnes quality control flag value
   * @return surface temperature spatial barnes quality control flag value
   */
  inline unsigned char get_surface_temp_spatial_barnes_qc_passed ()
  {
    return surface_temp_spatial_barnes_passed;
  }

  /**
   * @brief get surface temperature persistence quality control flag value
   * @return surface temperature persistence quality control flag value
   */
  inline unsigned char get_surface_temp_persist_passed ()
  {
    return surface_temp_persist_passed;
  }

  /**
   * @brief get surface temperature time step quality control flag value
   * @return surface temperature time step quality control flag value
   */
  inline unsigned char get_surface_temp_step_passed ()
  {
    return surface_temp_step_passed;
  }

  /**
   * @brief get barometric pressure spatial barnes quality control flag value
   * @return barometric pressure spatial barnes quality control flag value
   */
  inline unsigned char get_bar_press_spatial_barnes_qc_passed ()
  {
    return bar_press_spatial_barnes_passed;
  }

  /**
   * @brief set road_segment value
   * @param[in] value  road_segment value
   */
  inline void set_road_segment(const vdt_road_segment& value)
  {
    road_segment = value;
  }

  /**
   * @brief set road_segment_id value
   * @param[in] value  road_segment_id value
   */
  inline void set_road_segment_id(int value)
  {
    road_segment_id = value;
  }

  /**
   * @brief set mean air temperature for the nearest surface stations value
   * @param[in] value mean air temperature for the nearest surface stations value
   */
  inline void set_nss_air_temp_mean(float value)
  {
    nss_air_temp_mean = value;
  }

  /**
   * @brief set mean barometric pressure for the nearest surface stations value
   * @param[in] value mean barometric pressure for the nearest surface stations value
   */
  inline void set_nss_bar_press_mean(float value)
  {
    nss_bar_press_mean = value;
  }

  /**
   * @brief set air temperature from the model data value
   * @param[in] value air temperature from the model data value
   */
  inline void set_model_air_temp(float value)
  {
    model_air_temp = value;
  }

  /**
   * @brief set dewpoint from the model data value
   * @param[in] value dewpoint from the model data value
   */
  inline void set_model_dewpoint(float value)
  {
    model_dewpoint = value;
  }

  /**
   * @brief set barometric pressure from the model data value
   * @param[in] value barometric pressure from the model data value
   */
  inline void set_model_bar_press(float value)
  {
    model_bar_press = value;
  }

  /**
   * @brief set reflectivity from radar gridvalue
   * @param[in] value reflectivity from radar grid value
   */
  inline void set_radar_ref(float value)
  {
    radar_ref = value;
  }

  /**
   * @brief set precip flag from radar grid value
   * @param[in] value precip flag from radar grid value
   */
  inline void set_radar_precip_flag(short value)
  {
    radar_precip_flag = value;
  }

  /**
   * @brief set precip type from radar grid value
   * @param[in] value precip type from radar grid value
   */
  inline void set_radar_precip_type(short value)
  {
    radar_precip_type = value;
  }

  /**
   * @brief set cloud mask from satellite grid value
   * @param[in] value cloud mask from satellite grid  value
   */
  inline void set_cloud_mask(short value)
  {
    cloud_mask = value;
  }

  /**
   * @brief set mean dew temperature for the nearest surface stations value
   * @param[in] value mean dew temperature for the nearest surface stations  value
   */
  inline void set_nss_dew_temp_mean(float value)
  {
    nss_dew_temp_mean = value;
  }

  /**
   * @brief set mean wind direction for the nearest surface stations value
   * @param[in] value mean wind direction for the nearest surface stations  value
   */
  inline void set_nss_wind_dir_mean(float value)
  {
    nss_wind_dir_mean = value;
  }

  /**
   * @brief set mean wind speed for the nearest surface stations value
   * @param[in] value  mean wind speed for the nearest surface stations value
   */
  inline void set_nss_wind_speed_mean(float value)
  {
    nss_wind_speed_mean = value;
  }

  /**
   * @brief set mean hourly precipitation for the nearest surface stations value
   * @param[in] value  mean hourly precipitation for the nearest surface stations value
   */
  inline void set_nss_hourly_precip_mean(float value)
  {
    nss_hourly_precip_mean = value;
  }

  /**
   * @brief set mean prevailing visibility for the nearest surface stations value
   * @param[in] value  mean prevailing visibility for the nearest surface stations value
   */
  inline void set_nss_prevail_vis_mean(float value)
  {
    nss_prevail_vis_mean = value;
  }

  /**
   * @brief set quality control flag for elevation value
   * @param[in] value quality control flag for elevation  value
   */
  inline void set_elevation_range_qc_passed(unsigned char value)
  {
    elevation_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for heading value
   * @param[in] value  quality control flag for heading value
   */
  inline void set_heading_range_qc_passed(unsigned char value)
  {
    heading_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for speed value
   * @param[in] value  quality control flag for speed value
   */
  inline void set_speed_range_qc_passed(unsigned char value)
  {
    speed_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for brake status value
   * @param[in] value  quality control flag for brake status value
   */
  inline void set_brake_status_range_qc_passed(unsigned char value)
  {
    brake_status_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for brake boost value
   * @param[in] value  quality control flag for brake boost value
   */
  inline void set_brake_boost_range_qc_passed(unsigned char value)
  {
    brake_boost_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for anti lock brake status value
   * @param[in] value  quality control flag for anti lock brake status value
   */
  inline void set_abs_range_qc_passed(unsigned char value)
  {
    abs_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for stability value
   * @param[in] value quality control flag for stability value
   */
  inline void set_stab_range_qc_passed(unsigned char value)
  {
    stab_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for traction value
   * @param[in] value quality control flag for traction value
   */
  inline void set_trac_range_qc_passed(unsigned char value)
  {
    trac_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for lights value
   * @param[in] value quality control flag for lights value
   */
  inline void set_lights_range_qc_passed(unsigned char value)
  {
    lights_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for yaw value
   * @param[in] value quality control flag for yaw value
   */
  inline void set_yaw_range_qc_passed(unsigned char value)
  {
    yaw_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for horizontal longitudinal acceleration value
   * @param[in] value  quality control flag for horizontal longitudinal acceleration value
   */
  inline void set_hoz_accel_long_range_qc_passed(unsigned char value)
  {
    hoz_accel_long_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for horizontal lateral acceleration value
   * @param[in] value  quality control flag for horizontal lateral acceleration value
   */
  inline void set_hoz_accel_lat_range_qc_passed(unsigned char value)
  {
    hoz_accel_lat_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for steering angle value
   * @param[in] value  quality control flag for steering angle value
   */
  inline void set_steering_angle_range_qc_passed(unsigned char value)
  {
    steering_angle_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for for steering rate value
   * @param[in] value  quality control flag for for steering rate value
   */
  inline void set_steering_rate_range_qc_passed(unsigned char value)
  {
    steering_rate_range_qc_passed = value;
  }

  /**
   * @brief set quality control flag for front wiper status value
   * @param[in] value  quality control flag for front wiper status value
   */
  inline void set_wipers_front_status_range_qc_passed(unsigned char value)
  {
    wipers_front_status_range_qc_passed = value;
  }

  /**
   * @brief set air temperature sensor range test quality control flag value
   * @param[in] value  set air temperature sensor range test quality control flag value
   */
  inline void set_air_temp_range_qc_passed(unsigned char value)
  {
    air_temp_range_qc_passed = value;
  }

  /**
   * @brief set air temperature nearest surface station quality control flag value
   * @param[in] value  air temperature nearest surface station quality control flag value
   */
  inline void set_air_temp_sdt_passed(unsigned char value)
  {
    air_temp_sdt_passed = value;
  }

  /**
   * @brief set air temperature nearest surface station quality control flag value
   * @param[in] value  air temperature nearest surface station quality control flag value
   */
  inline void set_air_temp_nst_passed(unsigned char value)
  {
    air_temp_nst_passed = value;
  }

  /**
   * @brief set air temperature model analysis quality control flag value
   * @param[in] value  air temperature model analysis quality control flag value
   */
  inline void set_air_temp_mat_passed(unsigned char value)
  {
    air_temp_mat_passed = value;
  }

  /**
   * @brief set air temperature climate range test quality control flag value
   * @param[in] value  air temperature climate range test quality control flag value
   */
  inline void set_air_temp_crt_passed(unsigned char value)
  {
    air_temp_crt_passed = value;
  }

  /**
   * @brief set air temperature neighboring vehicle test quality control flag value
   * @param[in] value  air temperature neighboring vehicle test quality control flag value
   */
  inline void set_air_temp_nvt_passed(unsigned char value)
  {
    air_temp_nvt_passed = value;
  }

  /**
   * @brief set air temperature combined algorithm test quality control flag value
   * @param[in] value  air temperature combined algorithm test quality control flag value
   */
  inline void set_air_temp_cat_passed(unsigned char value)
  {
    air_temp_cat_passed = value;
  }

  /**
   * @brief set air temperature persistence test quality control flag value
   * @param[in] value air temperature persistence test quality control flag  value
   */
  inline void set_air_temp_persist_passed(unsigned char value)
  {
    air_temp_persist_passed = value;
  }

  /**
   * @brief set air temperature time step test quality control flag value
   * @param[in] value  air temperature time step test quality control flag value
   */
  inline void set_air_temp_step_passed(unsigned char value)
  {
    air_temp_step_passed = value;
  }

  /**
   * @brief set external air temperature sensor range test quality control flag value
   * @param[in] value external air temperature sensor range test quality control flag  value
   */
  inline void set_air_temp2_range_qc_passed(unsigned char value)
  {
    air_temp2_range_qc_passed = value;
  }

  /**
   * @brief set external air temperature standard deviation test quality control flag value
   * @param[in] value  external air temperature standard deviation test quality control flag value
   */
  inline void set_air_temp2_sdt_passed(unsigned char value)
  {
    air_temp2_sdt_passed = value;
  }

  /**
   * @brief set external air temperature nearest surface station quality control flag nvalue
   * @param[in] value  external air temperature nearest surface station quality control flag nvalue
   */
  inline void set_air_temp2_nst_passed(unsigned char value)
  {
    air_temp2_nst_passed = value;
  }

  /**
   * @brief set external air temperature model analysis quality control flag value
   * @param[in] value external air temperature model analysis quality control flag value
   */
  inline void set_air_temp2_mat_passed(unsigned char value)
  {
    air_temp2_mat_passed = value;
  }

  /**
   * @brief set external air temperature climate range test quality control flag value
   * @param[in] value  external air temperature climate range test quality control flag value
   */
  inline void set_air_temp2_crt_passed(unsigned char value)
  {
    air_temp2_crt_passed = value;
  }

  /**
   * @brief set external air temperature neighboring vehicle test quality control flag value
   * @param[in] value  external air temperature neighboring vehicle test quality control flag value
   */
  inline void set_air_temp2_nvt_passed(unsigned char value)
  {
    air_temp2_nvt_passed = value;
  }

  /**
   * @brief set external air temperature combined algorithm test quality control flag value
   * @param[in] value  external air temperature combined algorithm test quality control flag value
   */
  inline void set_air_temp2_cat_passed(unsigned char value)
  {
    air_temp2_cat_passed = value;
  }

   /**
   * @brief set external air temperature persistence test quality control flag value
   * @param[in] value  external air temperature persistence test quality control flag value
   */
  inline void set_air_temp2_persist_passed(unsigned char value)
  {
    air_temp2_persist_passed = value;
  }

   /**
   * @brief set external air temperature time step test quality control flag value
   * @param[in] value  external air temperature time step test quality control flag value
   */
  inline void set_air_temp2_step_passed(unsigned char value)
  {
    air_temp2_step_passed = value;
  }

 /**
   * @brief set dew temperature sensor range test quality control flag value
   * @param[in] value dew temperature sensor range test quality control flag  value
   */
  inline void set_dew_temp_range_qc_passed(unsigned char value)
  {
    dew_temp_range_qc_passed = value;
  }

  /**
   * @brief set dew temperature climate range test quality control flag value
   * @param[in] value  dew temperature climate range test quality control flag value
   */
  inline void set_dew_temp_nst_passed(unsigned char value)
  {
    dew_temp_nst_passed = value;
  }

  /**
   * @brief set dew temperature combined algorithm test quality control flag value
   * @param[in] value  dew temperature combined algorithm test quality control flag value
   */
  inline void set_dew_temp_cat_passed(unsigned char value)
  {
    dew_temp_cat_passed = value;
  }

  /**
   * @brief set dew temperature model analysis quality control flag value
   * @param[in] value external dew temperature model analysis quality control flag value
   */
  inline void set_dew_temp_mat_passed(unsigned char value)
  {
    dew_temp_mat_passed = value;
  }

  /**
   * @brief set dew temperature neighboring vehicle test quality control flag value
   * @param[in] value dew temperature neighboring vehicle test quality control flag value
   */
  inline void set_dew_temp_nvt_passed(unsigned char value)
  {
    dew_temp_nvt_passed = value;
  }

  /**
   * @brief set dew temperature nearest surface station quality control flag value
   * @param[in] value dew temperature nearest surface station quality control flag value
   */
  inline void set_dew_temp_sdt_passed(unsigned char value)
  {
    dew_temp_sdt_passed = value;
  }

  /**
   * @brief set dew temperature spatial iqr quality control flag value
   * @param[in] value dew temperature spatial iqr quality control flag value
   */
  inline void set_dew_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    dew_temp_spatial_iqr_passed = value;
  }

  /**
   * @brief set dew temperature spatial barnes quality control flag value
   * @param[in] value  dew temperature spatial barnes quality control flag value
   */
  inline void set_dew_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    dew_temp_spatial_barnes_passed = value;
  }

  /**
   * @brief set dew temperature climate range test quality control flag value
   * @param[in] value dew temperature climate range test quality control flag value
   */
  inline void set_dew_temp_crt_passed(unsigned char value)
  {
    dew_temp_crt_passed = value;
  }

  /**
   * @brief set dew temperature persistence test quality control flag value
   * @param[in] value dew temperature persistence test quality control flag  value
   */
  inline void set_dew_temp_persist_passed(unsigned char value)
  {
    dew_temp_persist_passed = value;
  }

  /**
   * @brief set dew temperature time step test quality control flag value
   * @param[in] value  dew temperature time step test quality control flag value
   */
  inline void set_dew_temp_step_passed(unsigned char value)
  {
    dew_temp_step_passed = value;
  }

  /**
   * @brief set barometric pressure sensor range test  quality control flag value
   * @param[in] value  barometric pressure sensor range test  quality control flag value
   */
  inline void set_bar_press_range_qc_passed(unsigned char value)
  {
    bar_press_range_qc_passed = value;
  }

  /**
   * @brief set barometric pressure persistence test quality control flag value
   * @param[in] value barometric pressure persistence test quality control flag value
   */
  inline void set_bar_press_persist_passed(unsigned char value)
  {
    bar_press_persist_passed = value;
  }

  /**
   * @brief set barometric pressure standard deviation test quality control flag value
   * @param[in] value  barometric pressure standard deviation test quality control flag value
   */
  inline void set_bar_press_sdt_passed(unsigned char value)
  {
    bar_press_sdt_passed = value;
  }

  /**
   * @brief set barometric pressure nearest surface station quality control flag value
   * @param[in] value  barometric pressure nearest surface station quality control flag value
   */
  inline void set_bar_press_nst_passed(unsigned char value)
  {
    bar_press_nst_passed = value;
  }

  /**
   * @brief set barometric pressure model analysis quality control flag value
   * @param[in] value  barometric pressure model analysis quality control flag value
   */
  inline void set_bar_press_mat_passed(unsigned char value)
  {
    bar_press_mat_passed = value;
  }

  /**
   * @brief set barometric pressure climate range test quality control flag value
   * @param[in] value  barometric pressure climate range test quality control flag value
   */
  inline void set_bar_press_crt_passed(unsigned char value)
  {
    bar_press_crt_passed = value;
  }

  /**
   * @brief set barometric pressure neighboring vehicle test quality control flag value
   * @param[in] value  barometric pressure neighboring vehicle test quality control flag value
   */
  inline void set_bar_press_nvt_passed(unsigned char value)
  {
    bar_press_nvt_passed = value;
  }

  /**
   * @brief set barometric pressure combined algorithm test quality control flag value
   * @param[in] value  barometric pressure combined algorithm test quality control flag value
   */
  inline void set_bar_press_cat_passed(unsigned char value)
  {
    bar_press_cat_passed = value;
  }

  /**
   * @brief set barometric pressure time step test quality control flag value
   * @param[in] value  barometric pressure time step test quality control flag value
   */
  inline void set_bar_press_step_passed(unsigned char value)
  {
    bar_press_step_passed = value;
  }

  /**
   * @brief set latitude data filtering test quality control flag value
   * @param[in] value  latitude data filtering test quality control flag value
   */
  inline void set_latitude_dft_passed(unsigned char value)
  {
    latitude_dft_passed = value;
  }

  /**
   * @brief set longitude data filtering test quality control flag value
   * @param[in] value  longitude data filtering test quality control flag value
   */
  inline void set_longitude_dft_passed(unsigned char value)
  {
    longitude_dft_passed = value;
  }

  /**
   * @brief set surface temperature climate range test quality control flag value
   * @param[in] value  surface temperature climate range test quality control flag value
   */
  inline void set_surface_temp_range_qc_passed(unsigned char value)
  {
    surface_temp_range_qc_passed = value;
  }

  /**
   * @brief set surface temperature nearest surface station quality control flag value
   * @param[in] value  surface temperature nearest surface station quality control flag value
   */
  inline void set_surface_temp_sdt_passed(unsigned char value)
  {
    surface_temp_sdt_passed = value;
  }

  /**
   * @brief set surface temperature climate range test quality control flag value
   * @param[in] value  surface temperature climate range test quality control flag value
   */
  inline void set_surface_temp_crt_passed(unsigned char value)
  {
    surface_temp_crt_passed = value;
  }

  /**
   * @brief set surface temperature neighboring vehicle test quality control flag value
   * @param[in] value  surface temperature neighboring vehicle test quality control flag value
   */
  inline void set_surface_temp_nvt_passed(unsigned char value)
  {
    surface_temp_nvt_passed = value;
  }

  /**
   * @brief set surface temperature combined algorithm test quality control flag value
   * @param[in] value  surface temperature combined algorithm test quality control flag value
   */
  inline void set_surface_temp_cat_passed(unsigned char value)
  {
    surface_temp_cat_passed = value;
  }

  /**
   * @brief set air temperature spatial iqr quality control flag value
   * @param[in] value  air temperature spatial iqr quality control flag value
   */
  inline void set_air_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    air_temp_spatial_iqr_passed = value;
  }

  /**
   * @brief set second air temperature spatial iqr quality control flag value
   * @param[in] value  second air temperature spatial iqr quality control flag value
   */
  inline void set_air_temp2_spatial_iqr_qc_passed (unsigned char value)
  {
    air_temp2_spatial_iqr_passed = value;
  }

  /**
   * @brief set surface temperature spatial iqr quality control flag value
   * @param[in] value  surface temperature spatial iqr quality control flag value
   */
  inline void set_surface_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    surface_temp_spatial_iqr_passed = value;
  }

  /**
   * @brief set surface temperature persistence quality control flag value
   * @param[in] value  surface temperature persistence quality control flag value
   */
  inline void set_surface_temp_persist_passed (unsigned char value)
  {
    surface_temp_persist_passed = value;
  }

  /**
   * @brief set surface temperature time step quality control flag value
   * @param[in] value  surface temperature time step quality control flag value
   */
  inline void set_surface_temp_step_passed (unsigned char value)
  {
    surface_temp_step_passed = value;
  }

  /**
   * @brief set barometric pressure spatial iqr quality control flag value
   * @param[in] value  barometric pressure spatial iqr quality control flag value
   */
  inline void set_bar_press_spatial_iqr_qc_passed (unsigned char value)
  {
    bar_press_spatial_iqr_passed = value;
  }


  /**
   * @brief set air temperature spatial barnes quality control flag value
   * @param[in] value  air temperature spatial barnes quality control flag value
   */
  inline void set_air_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    air_temp_spatial_barnes_passed = value;
  }

  /**
   * @brief set second air temperature spatial barnes quality control flag value
   * @param[in] value  second air temperature spatial barnes quality control flag value
   */
  inline void set_air_temp2_spatial_barnes_qc_passed (unsigned char value)
  {
    air_temp2_spatial_barnes_passed = value;
  }

  /**
   * @brief set surface temperature spatial barnes quality control flag value
   * @param[in] value  surface temperature spatial barnes quality control flag value
   */
  inline void set_surface_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    surface_temp_spatial_barnes_passed = value;
  }

  /**
   * @brief set barometric pressure spatial barnes quality control flag value
   * @param[in] value  barometric pressure spatial barnes quality control flag value
   */
  inline void set_bar_press_spatial_barnes_qc_passed (unsigned char value)
  {
    bar_press_spatial_barnes_passed = value;
  }

  /**
   * @brief  copy input messages to output messages based on recorded time
   * @param[in] msg_to_copy source messages to copy to destination
   * @param[in,out] destination_msg contains the messages to replace with suitable messages from msg_to_copy
   */
  static void copy_based_on_rec_time(const vector<vdt_probe_message_qc> &msg_to_copy, vector<vdt_probe_message_qc> &destination_msg);
  
  /** @brief probe message header used for testing */
  static string probe_message_hdr()
  {
    return vdt_probe_message::probe_message_hdr() + ",road_segment_id,cloud_mask,model_air_temp,model_dewpoint,model_bar_press,nss_air_temp_mean,nss_bar_press_mean,nss_dew_temp_mean,nss_hourly_precip_mean,nss_prevail_vis_mean,nss_wind_dir_mean,nss_wind_speed_mean,radar_ref,radar_precip_flag,radar_precip_type,abs_range_qc_passed,air_temp_range_qc_passed,air_temp_sdt_passed,air_temp_nvt_passed,air_temp_mat_passed,air_temp_crt_passed,air_temp_persist_passed,air_temp_spatial_barnes_passed,air_temp_spatial_iqr_passed,air_temp_step_passed,air_temp2_range_qc_passed,air_temp2_sdt_passed,air_temp2_nvt_passed,air_temp2_mat_passed,air_temp2_crt_passed,air_temp2_spatial_barnes_passed,air_temp2_spatial_iqr_passed,air_temp2_persist_passed,air_temp2_step_passed,bar_press_range_qc_passed,bar_press_sdt_passed,bar_press_nvt_passed,bar_press_mat_passed,bar_press_crt_passed,bar_press_persist_passed,bar_press_spatial_barnes_passed,bar_press_spatial_iqr_passed,bar_press_step_passed,brake_boost_range_qc_passed,brake_status_range_qc_passed,dew_temp_range_qc_passed,dew_temp_cat_passed,dew_temp_crt_passed,dew_temp_mat_passed,dew_temp_nvt_passed,dew_temp_sdt_passed,dew_temp_spatial_barnes_passed,dew_temp_spatial_iqr_passed,dew_temp_persist_passed,dew_temp_step_test,elevation_range_qc_passed,heading_range_qc_passed,hoz_accel_long_range_qc_passed,hoz_accel_lat_range_qc_passed,latitude_dft_passed,longitude_dft_passed,lights_range_qc_passed,speed_range_qc_passed,stab_range_qc_passed,steering_angle_range_qc_passed,steering_rate_range_qc_passed,surface_temp_range_qc_passed,surface_temp_crt_passed,surface_temp_sdt_passed,surface_temp_persist_passed,surface_temp_spatial_barnes_passed,surface_temp_spatial_iqr_passed,surface_temp_step_passed,trac_range_qc_passed,wipers_front_status_range_qc_passed,yaw_range_qc_passed";
  }

  /** @brief probe message string used for testing */
  string probe_message_string()
  {
    stringstream oss;
    oss << vdt_probe_message::probe_message_string() <<","<< road_segment_id <<","<< cloud_mask <<","<< model_air_temp <<","<< model_dewpoint <<","<< model_bar_press <<","<< nss_air_temp_mean <<","<< nss_bar_press_mean <<","<< nss_dew_temp_mean <<","<< nss_hourly_precip_mean <<","<< nss_prevail_vis_mean <<","<< nss_wind_dir_mean <<","<< nss_wind_speed_mean <<","<< radar_ref <<","<< radar_precip_flag <<","<< radar_precip_type <<","<< static_cast<int>(abs_range_qc_passed) <<","<< static_cast<int>(air_temp_range_qc_passed) <<","<< static_cast<int>(air_temp_sdt_passed) <<","<< static_cast<int>(air_temp_nvt_passed) <<","<< static_cast<int>(air_temp_mat_passed) <<","<< static_cast<int>(air_temp_crt_passed) <<","<< static_cast<int>(air_temp_persist_passed) <<","<< static_cast<int>(air_temp_spatial_barnes_passed) <<","<< static_cast<int>(air_temp_spatial_iqr_passed) <<","<< static_cast<int>(air_temp_step_passed) <<","<< static_cast<int>(air_temp2_range_qc_passed) <<","<< static_cast<int>(air_temp2_sdt_passed) <<","<< static_cast<int>(air_temp2_nvt_passed) <<","<< static_cast<int>(air_temp2_mat_passed) <<","<< static_cast<int>(air_temp2_crt_passed) <<","<< static_cast<int>(air_temp2_spatial_barnes_passed) <<","<< static_cast<int>(air_temp2_spatial_iqr_passed) <<","<< static_cast<int>(air_temp2_persist_passed) <<","<< static_cast<int>(air_temp2_step_passed) <<","<< static_cast<int>(bar_press_range_qc_passed) <<","<< static_cast<int>(bar_press_sdt_passed) <<","<< static_cast<int>(bar_press_nvt_passed) <<","<< static_cast<int>(bar_press_mat_passed) <<","<< static_cast<int>(bar_press_crt_passed) <<","<< static_cast<int>(bar_press_persist_passed) <<","<< static_cast<int>(bar_press_spatial_barnes_passed) <<","<< static_cast<int>(bar_press_spatial_iqr_passed) <<","<< static_cast<int>(bar_press_step_passed) <<","<< static_cast<int>(brake_boost_range_qc_passed) <<","<< static_cast<int>(brake_status_range_qc_passed) <<","<< static_cast<int>(dew_temp_range_qc_passed) <<","<< static_cast<int>(dew_temp_cat_passed) <<","<< static_cast<int>(dew_temp_crt_passed) <<","<< static_cast<int>(dew_temp_mat_passed) <<","<< static_cast<int>(dew_temp_nvt_passed) <<","<< static_cast<int>(dew_temp_sdt_passed) <<","<< static_cast<int>(dew_temp_spatial_barnes_passed) <<","<< static_cast<int>(dew_temp_spatial_iqr_passed) <<","<< static_cast<int>(dew_temp_persist_passed) <<","<< static_cast<int>(dew_temp_step_passed) <<","<< static_cast<int>(elevation_range_qc_passed) <<","<< static_cast<int>(heading_range_qc_passed) <<","<< static_cast<int>(hoz_accel_long_range_qc_passed) <<","<< static_cast<int>(hoz_accel_lat_range_qc_passed) <<","<< static_cast<int>(latitude_dft_passed) <<","<< static_cast<int>(longitude_dft_passed) <<","<< static_cast<int>(lights_range_qc_passed) <<","<< static_cast<int>(speed_range_qc_passed) <<","<< static_cast<int>(stab_range_qc_passed) <<","<< static_cast<int>(steering_angle_range_qc_passed) <<","<< static_cast<int>(steering_rate_range_qc_passed) <<","<< static_cast<int>(surface_temp_range_qc_passed) <<","<< static_cast<int>(surface_temp_crt_passed) <<","<< static_cast<int>(surface_temp_sdt_passed) <<","<< static_cast<int>(surface_temp_persist_passed)  <<","<< static_cast<int>(surface_temp_spatial_barnes_passed) <<","<< static_cast<int>(surface_temp_spatial_iqr_passed) <<","<< static_cast<int>(surface_temp_step_passed) <<","<< static_cast<int>(trac_range_qc_passed) <<","<< static_cast<int>(wipers_front_status_range_qc_passed) <<","<< static_cast<int>(yaw_range_qc_passed) ;

    return oss.str();
  }

  /** @brief short probe message header used for testing */
  static string short_probe_message_hdr()
  {
    return vdt_probe_message::short_probe_message_hdr() + ",road_segment_id,air_temp_range_qc_passed,air_temp_sdt_passed,air_temp_mat_passed,air_temp_crt_passed,air_temp_persist_passed,air_temp_step_passed,air_temp_spatial_iqr_passed,air_temp_spatial_barnes_passed,air_temp_nvt_passed,bar_press_range_qc_passed,bar_press_sdt_passed,bar_press_mat_passed,bar_press_crt_passed,bar_press_persist_passed,bar_press_step_passed,bar_press_spatial_iqr_passed,bar_press_spatial_barnes_passed,bar_press_nvt_passed";
  }

  
  /** @brief short probe message string used for testing */
  string short_probe_message_string()
  {
    stringstream oss;
    oss << vdt_probe_message::short_probe_message_string() <<","<< road_segment_id <<","<<static_cast<int>(air_temp_range_qc_passed) <<","<< static_cast<int>(air_temp_sdt_passed) <<","<< static_cast<int>(air_temp_mat_passed) <<","<< static_cast<int>(air_temp_crt_passed) <<","<< static_cast<int>(air_temp_persist_passed)<<","<<static_cast<int>(air_temp_step_passed)<<","<<static_cast<int>(air_temp_spatial_iqr_passed)<<","<<static_cast<int>(air_temp_spatial_barnes_passed)<<","<<static_cast<int>(air_temp_nvt_passed)<<","<<static_cast<int>(bar_press_range_qc_passed) <<","<< static_cast<int>(bar_press_sdt_passed) <<","<< static_cast<int>(bar_press_mat_passed) <<","<< static_cast<int>(bar_press_crt_passed) <<","<< static_cast<int>(bar_press_persist_passed)<<","<<static_cast<int>(bar_press_step_passed)<<","<<static_cast<int>(bar_press_spatial_iqr_passed)<<","<<static_cast<int>(bar_press_spatial_barnes_passed)<<","<<static_cast<int>(bar_press_nvt_passed);

    return oss.str();
  }

private:
  void init();
};

} //END namespace RWX
#endif /* VDT_PROBE_MESSAGE_QC_HH */
