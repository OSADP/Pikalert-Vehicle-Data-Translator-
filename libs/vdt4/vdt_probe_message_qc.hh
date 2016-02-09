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
#include "vdt_road_segment.hh"

using std::string;
using std::vector;

/**
 * @class vdt_probe_message_qc
 * Probe message class with quality control information
 */
class vdt_probe_message_qc : public vdt_probe_message
{
  void init_qc_fields(const FIELD_NAME_TYPE_VEC& qc_fields);
public:
  /** @brief constructor */
  vdt_probe_message_qc(const FIELD_NAME_TYPE_VEC& qc_fields);

  /**
   * @brief constructor
   * @param[in] probe_message 
   */
  vdt_probe_message_qc(const vdt_probe_message &probe_message, const FIELD_NAME_TYPE_VEC& qc_fields);

  /** @brief relational operator */
  bool operator < (const vdt_probe_message_qc &probe_message_qc) const;

  /** @brief enum for the quality control flag values */
  enum Result { FAILED = 0, PASSED = 1, NOT_APPLICABLE = 255};

  /** @brief vector storing lights values*/
  vector<short> light_vector;

  /** @brief road segment */
  vdt_road_segment road_segment;


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
    return getattr<int>("road_segment_id");
  }

  /**
   * @brief get mean air temperature for the nearest surface stations value
   * @return  mean air temperature for the nearest surface stations value
   */
  inline float get_nss_air_temp_mean() const
  {
    return static_cast<float>(getattr<double>("nss_air_temp_mean"));
  }

  /**
   * @brief get mean surface temperature for the nearest surface stations value
   * @return  mean surface temperature for the nearest surface stations value
   */
  inline float get_nss_surface_temp_mean() const
  {
    return static_cast<float>(getattr<double>("nss_surface_temp_mean"));
  }

  /**
   * @brief get mean barometric pressure for the nearest surface stations value
   * @return mean barometric pressure for the nearest surface stations value
   */
  inline float get_nss_bar_press_mean() const
  {
    return static_cast<float>(getattr<double>("nss_bar_pressure_mean"));
  }

  /**
   * @brief get air temperature from the model data value
   * @return air temperature from the model data value
   */
  inline float get_model_air_temp() const
  {
    return static_cast<float>(getattr<double>("model_air_temp"));
  }

  /**
   * @brief get dewpoint from the model data value
   * @return dewpoint from the model data value
   */
  inline float get_model_dewpoint() const
  {
    return static_cast<float>(getattr<double>("model_dew_temp"));
  }

  /**
   * @brief get barometric pressure from the model data value
   * @return barometric pressure from the model data value
   */
  inline float get_model_bar_press() const
  {
    return static_cast<float>(getattr<double>("model_bar_pressure"));
  }

  /**
   * @brief get reflectivity from radar grid value
   * @return reflectivity from radar grid value
   */
  inline float get_radar_ref() const
  {
    return static_cast<float>(getattr<double>("radar_ref"));
  }

  /**
   * @brief get composite reflectivity from radar grid value
   * @return reflectivity from radar grid value
   */
  inline float get_radar_cref() const
  {
    return static_cast<float>(getattr<double>("radar_cref"));
  }

  /**
   * @brief get precip flag from radar grid value
   * @return precip flag from radar grid value
   */
  inline const short get_radar_precip_flag() const
  {
    return static_cast<short>(getattr<int>("radar_precip_flag"));
  }

  /**
   * @brief get precip type from radar grid value
   * @return precip type from radar grid value
   */
  inline const short get_radar_precip_type() const
  {
    return static_cast<short>(getattr<int>("radar_precip_type"));
  }

  /**
   * @brief get cloud mask from satellite grid value
   * @return cloud mask from satellite grid value
   */
  inline const short get_cloud_mask() const
  {
    return static_cast<short>(getattr<int>("cloud_mask"));
  }

  /**
   * @brief get mean dew temperature for the nearest surface stations value
   * @return mean dew temperature for the nearest surface stations value
   */
  inline float get_nss_dew_temp_mean() const
  {
    return static_cast<float>(getattr<double>("nss_dew_temp_mean"));
  }

  /**
   * @brief get mean wind direction for the nearest surface stations value
   * @return mean wind direction for the nearest surface stations value
   */
  inline float get_nss_wind_dir_mean() const
  {
    return static_cast<float>(getattr<double>("nss_wind_dir_mean"));
  }

  /**
   * @brief get mean wind speed for the nearest surface stations value
   * @return mean wind speed for the nearest surface stations value
   */
  inline float get_nss_wind_speed_mean() const
  {
    return static_cast<float>(getattr<double>("nss_wind_speed_mean"));
  }

  /**
   * @brief get mean hourly precipitation for the nearest surface stations value
   * @return mean hourly precipitation for the nearest surface stations value
   */
  inline float get_nss_hourly_precip_mean() const
  {
    return static_cast<float>(getattr<double>("nss_hourly_precip_mean"));
  }

  /**
   * @brief get mean prevailing visibility for the nearest surface stations value
   * @return mean prevailing visibility for the nearest surface stations value
   */
  inline float get_nss_prevail_vis_mean() const
  {
    return static_cast<float>(getattr<double>("nss_prevail_vis_mean"));
  }

  /**
   * @brief get quality control flag for heading value
   * @return quality control flag for heading value
   */
  inline unsigned char get_heading_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("heading_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for speed value
   * @return quality control flag for speed value
   */
  inline unsigned char get_speed_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("speed_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for brake status value
   * @return quality control flag for brake status value
   */
  inline unsigned char get_brake_status_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("brake_status_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for brake boost value
   * @return quality control flag for brake boost value
   */
  inline unsigned char get_brake_boost_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("brake_boost_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for anti lock brake status value
   * @return quality control flag for anti lock brake status value
   */
  inline unsigned char get_abs_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("abs_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for stability value
   * @return quality control flag for stability value
   */
  inline unsigned char get_stab_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("stab_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for traction value
   * @return quality control flag for traction value
   */
  inline unsigned char get_trac_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("trac_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for lights value
   * @return quality control flag for lights value
   */
  inline unsigned char get_lights_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("lights_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for yaw value
   * @return quality control flag for yaw value
   */
  inline unsigned char get_yaw_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("yaw_rate_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for horizontal longitudinal acceleration value
   * @return quality control flag for horizontal longitudinal acceleration value
   */
  inline unsigned char get_hoz_accel_long_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("hoz_accel_long_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for horizontal lateral acceleration value
   * @return quality control flag for horizontal lateral acceleration value
   */
  inline unsigned char get_hoz_accel_lat_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("hoz_accel_lat_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for elevation value
   * @return quality control flag for elevation value
   */
  inline unsigned char get_elevation_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("elevation_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for steering angle value
   * @return quality control flag for steering angle value
   */
  inline unsigned char get_steering_angle_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("steering_angle_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for for steering rate value
   * @return quality control flag for steering rate value
   */
  inline unsigned char get_steering_rate_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("steering_rate_range_qc_passed"));
  }

  /**
   * @brief get quality control flag for front wiper status value
   * @return  quality control flag for front wiper status value
   */
  inline unsigned char get_wipers_front_status_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("wiper_status_range_qc_passed"));
  }

  /**
   * @brief get air temperature sensor range test quality control flag value
   * @return  air temperature sensor range test quality control flag value
   */
  inline unsigned char get_air_temp_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_range_qc_passed"));
  }

  /**
   * @brief get air temperature standard deviation test quality control flag value
   * @return air temperature standard deviation test quality control flag value
   */
  inline unsigned char get_air_temp_sdt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_sdt_passed"));
  }

  /**
   * @brief get air temperature nearest surface station quality control flag value
   * @return air temperature nearest surface station quality control flag value
   */
  inline unsigned char get_air_temp_nst_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_nst_passed"));
  }

  /**
   * @brief get air temperature model analysis quality control flag value
   * @return air temperature model analysis quality control flag value
   */
  inline unsigned char get_air_temp_mat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_mat_passed"));
  }

  /**
   * @brief get air temperature climate range test quality control flag value
   * @return air temperature climate range test quality control flag value
   */
  inline unsigned char get_air_temp_crt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_crt_passed"));
  }

  /**
   * @brief get air temperature neighboring vehicle test quality control flag value
   * @return air temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_air_temp_nvt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_nvt_passed"));
  }

  /**
   * @brief get air temperature combined algorithm test quality control flag value
   * @return air temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_air_temp_cat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_cat_passed"));
  }

  /**
   * @brief get air temperature combined algorithm test confidence value
   * @return air temperature combined algorithm test confidence value
   */
  inline float get_air_temp_cat_confidence() const
  {
    return static_cast<float>(getattr<double>("air_temp_cat_confidence"));
  }

  /**
   * @brief get air temperature persistence quality control flag
   * @return air temperature persistence quality control flag value
   */
  inline unsigned char get_air_temp_persist_passed () const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_persist_passed"));
  }

  /**
   * @brief get air temperature time step quality control flag
   * @return air temperature time step quality control flag value
   */
  inline unsigned char get_air_temp_step_passed () const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_step_passed"));
  }

  /**
   * @brief get external air temperature sensor range test quality control flag  value
   * @return external air temperature sensor range test quality control flag value
   */
  inline unsigned char get_air_temp2_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_range_qc_passed"));
  }

  /**
   * @brief get external air temperature standard deviation test quality control flag value
   * @return  external air temperature standard deviation test quality control flag value
   */
  inline unsigned char get_air_temp2_sdt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_sdt_passed"));
  }

  /**
   * @brief get external air temperature nearest surface station quality control flag value
   * @return external air temperature nearest surface station quality control flag value
   */
  inline unsigned char get_air_temp2_nst_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_nst_passed"));
  }

  /**
   * @brief get external air temperature model analysis quality control flag value
   * @return external air temperature model analysis quality control flag value
   */
  inline unsigned char get_air_temp2_mat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_mat_passed"));
  }

  /**
   * @brief get external air temperature climate range quality control flag value
   * @return  external air temperature climate range test
   */
  inline unsigned char get_air_temp2_crt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_crt_passed"));
  }

  /**
   * @brief get external air temperature neighboring vehicle quality control flag value
   * @return  external air temperature neighboring vehicle test
   */
  inline unsigned char get_air_temp2_nvt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_nvt_passed"));
  }

  /**
   * @brief get external air temperature combined algorithm quality control flag value
   * @return  external air temperature combined algorithm test
   */
  inline unsigned char get_air_temp2_cat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_cat_passed"));
  }

  /**
   * @brief get external air temperature combined algorithm confidence value
   * @return  external air temperature combined algorithm confidence value
   */
  inline float get_air_temp2_cat_confidence() const
  {
    return static_cast<float>(getattr<double>("air_temp2_cat_confidence"));
  }

  /**
   * @brief get external air temperature persistence quality control flag value
   * @return  external air temperature persistence test 
   */
  inline unsigned char get_air_temp2_persist_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_persist_passed"));
  }

  /**
   * @brief get external air temperature time step quality control flag value
   * @return  external air temperature time step test 
   */
  inline unsigned char get_air_temp2_step_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_step_passed"));
  }

  /**
   * @brief get dew temperature sensor range test quality control flag value
   * @return dew temperature sensor range test quality control flag value
   */
  inline unsigned char get_dew_temp_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_range_qc_passed"));
  }

  /**
   * @brief get dew temperature combined algorithm test quality control flag value
   * @return dew temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_dew_temp_cat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_cat_passed"));
  }

  /**
   * @brief get dew temperature combined algorithm test confidence value
   * @return dew temperature combined algorithm test confidence value
   */
  inline float get_dew_temp_cat_confidence() const
  {
    return static_cast<float>(getattr<double>("dew_temp_cat_confidence"));
  }

  /**
   * @brief get dew temperature model analysis quality control flag value
   * @return dew temperature model analysis quality control flag value
   */
  inline unsigned char get_dew_temp_mat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_mat_passed"));
  }

  /**
   * @brief get dew temperature nearest surface station quality control flag value
   * @return dew temperature nearest surface station quality control flag value
   */
  inline unsigned char get_dew_temp_nst_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_nst_passed"));
  }

  /**
   * @brief get dew temperature spatial barnes quality control flag value
   * @return dew temperature spatial barnes quality control flag value
   */
  inline unsigned char get_dew_temp_spatial_barnes_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_spatial_barnes_passed"));
  }

  /**
   * @brief get dew temperature spatial iqr quality control flag value
   * @return dew temperature spatial iqr quality control flag value
   */
  inline unsigned char get_dew_temp_spatial_iqr_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_spatial_iqr_passed"));
  }

  /**
   * @brief get dew temperature standard deviation test quality control flag value
   * @return dew temperature standard deviation test quality control flag value
   */
  inline unsigned char get_dew_temp_sdt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_sdt_passed"));
  }

  /**
   * @brief get dew temperature neighboring vehicle test quality control flag value
   * @return dew temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_dew_temp_nvt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_nvt_passed"));
  }

  /**
   * @brief get dew temperature climate range test quality control flag value
   * @return dew temperature climate range test quality control flag value
   */
  inline unsigned char get_dew_temp_crt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_crt_passed"));
  }

  /**
   * @brief get dew temperature persistence quality control flag
   * @return dew temperature persistence quality control flag value
   */
  inline unsigned char get_dew_temp_persist_passed () const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_persist_passed"));
  }

  /**
   * @brief get dew temperature time step quality control flag
   * @return dew temperature time step quality control flag value
   */
  inline unsigned char get_dew_temp_step_passed () const
  {
    return static_cast<unsigned char>(getattr<int>("dew_temp_step_passed"));
  }

  /**
   * @brief get barometric pressure sensor range test  quality control flag value
   * @return barometric pressure sensor range test  quality control flag value
   */
  inline unsigned char get_bar_press_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_range_qc_passed"));
  }

  /**
   * @brief get barometric pressure standard deviation test quality control flag value
   * @return barometric pressure standard deviation test quality control flag value
   */
  inline unsigned char get_bar_press_sdt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_sdt_passed"));
  }

  /**
   * @brief get barometric pressure nearest surface station quality control flag value
   * @return barometric pressure nearest surface station quality control flag value
   */
  inline unsigned char get_bar_press_nst_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_nst_passed"));
  }

  /**
   * @brief get  barometric pressure model analysis quality control flag value
   * @return  barometric pressure model analysis quality control flag value
   */
  inline unsigned char get_bar_press_mat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_mat_passed"));
  }

  /**
   * @brief get barometric pressure climate range test quality control flag value
   * @return barometric pressure climate range test quality control flag value
   */
  inline unsigned char get_bar_press_crt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_crt_passed"));
  }

  /**
   * @brief get barometric pressure neighboring vehicle test quality control flag value
   * @return barometric pressure neighboring vehcile test quality control flag value
   */
  inline unsigned char get_bar_press_nvt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_nvt_passed"));
  }

  /**
   * @brief get barometric pressure combined algorithm test quality control flag value
   * @return barometric pressure combined algorithm test quality control flag value
   */
  inline unsigned char get_bar_press_cat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_cat_passed"));
  }

  /**
   * @brief get barometric pressure combined algorithm confidence value
   * @return barometric pressure combined algorithm test confidence value
   */
  inline float get_bar_press_cat_confidence() const
  {
    return static_cast<float>(getattr<double>("bar_pressure_cat_confidence"));
  }

  /**
   * @brief get barometric pressure persistence quality control flag value
   * @return barometric pressure persistence quality control flag  value
   */
  inline unsigned char get_bar_press_persist_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_persist_passed"));
  }

  /**
   * @brief get barometric pressure time step quality control flag value
   * @return barometric pressure time step quality control flag  value
   */
  inline unsigned char get_bar_press_step_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_step_passed"));
  }

  /**
   * @brief get barometric pressure spatial iqr quality control flag value
   * @return barometric pressure spatial iqr quality control flag  value
   */
  inline unsigned char get_bar_press_spatial_iqr_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_spatial_iqr_passed"));
  }

  /**
   * @brief get barometric pressure spatial barnes quality control flag value
   * @return barometric pressure spatial barnes quality control flag  value
   */
  inline unsigned char get_bar_press_spatial_barnes_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_spatial_barnes_passed"));
  }

  /**
   * @brief get surface temperature sensor range test quality control flag value
   * @return surface temperature sensor range test quality control flag value
   */
  inline unsigned char get_surface_temp_range_qc_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_range_qc_passed"));
  }

  /**
   * @brief get surface temperature climate range test quality control flag value
   * @return surface temperature climate range test quality control flag value
   */
  inline unsigned char get_surface_temp_crt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_crt_passed"));
  }

  /**
   * @brief get surface temperature neighboring vehicle test quality control flag value
   * @return surface temperature neighboring vehicle test quality control flag value
   */
  inline unsigned char get_surface_temp_nvt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_nvt_passed"));
  }

  /**
   * @brief get surface temperature combined algorithm test quality control flag value
   * @return surface temperature combined algorithm test quality control flag value
   */
  inline unsigned char get_surface_temp_cat_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_cat_passed"));
  }

  /**
   * @brief get surface temperature combined algorithm test confidence value
   * @return surface temperature combined algorithm test confidence value
   */
  inline float get_surface_temp_cat_confidence() const
  {
    return static_cast<float>(getattr<double>("surface_temp_cat_confidence"));
  }

  /**
   * @brief get surface temperature standard deviation test quality control flag value
   * @return surface temperature standard deviation test quality control flag value
   */
  inline unsigned char get_surface_temp_sdt_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_sdt_passed"));
  }

  /**
   * @brief get latitude data filtering test quality control flag value
   * @return latitude data filtering test quality control flag value
   */
  inline unsigned char get_latitude_dft_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("latitude_dft_passed"));
  }

  /**
   * @brief get longitude data filtering test quality control flag value
   * @return longitude data filtering test quality control flag value
   */
  inline unsigned char get_longitude_dft_passed() const
  {
    return static_cast<unsigned char>(getattr<int>("longitude_dft_passed"));
  }

  /**
   * @brief get air temperature spatial iqr quality control flag value
   * @return air temperature spatial iqr quality control flag value
   */
  inline unsigned char get_air_temp_spatial_iqr_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_spatial_iqr_passed"));
  }

  /**
   * @brief get external air temperature spatial iqr quality control flag value
   * @return external air spatial iqr temperature quality control flag value
   */
  inline unsigned char get_air_temp2_spatial_iqr_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_spatial_iqr_passed"));
  }

  /**
   * @brief get surface temperature spatial iqr quality control flag value
   * @return surface temperature spatial iqr quality control flag value
   */
  inline unsigned char get_surface_temp_spatial_iqr_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_spatial_iqr_passed"));
  }

  /**
   * @brief get barometric pressure spatial iqr quality control flag value
   * @return barometric pressure spatial iqr quality control flag value
   */
  inline unsigned char get_bar_press_spatial_iqr_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_spatial_iqr_passed"));
  }


  /**
   * @brief get air temperature spatial barnes quality control flag value
   * @return air temperature spatial barnes quality control flag value
   */
  inline unsigned char get_air_temp_spatial_barnes_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("air_temp_spatial_barnes_passed"));
  }

  /**
   * @brief get external air temperature spatial barnes quality control flag value
   * @return external air temperature spatial barnes quality control flag value
   */
  inline unsigned char get_air_temp2_spatial_barnes_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("air_temp2_spatial_barnes_passed"));
  }

  /**
   * @brief get surface temperature spatial barnes quality control flag value
   * @return surface temperature spatial barnes quality control flag value
   */
  inline unsigned char get_surface_temp_spatial_barnes_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_spatial_barnes_passed"));
  }

  /**
   * @brief get surface temperature persistence quality control flag value
   * @return surface temperature persistence quality control flag value
   */
  inline unsigned char get_surface_temp_persist_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_persist_passed"));
  }

  /**
   * @brief get surface temperature time step quality control flag value
   * @return surface temperature time step quality control flag value
   */
  inline unsigned char get_surface_temp_step_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("surface_temp_step_passed"));
  }

  /**
   * @brief get barometric pressure spatial barnes quality control flag value
   * @return barometric pressure spatial barnes quality control flag value
   */
  inline unsigned char get_bar_press_spatial_barnes_qc_passed ()
  {
    return static_cast<unsigned char>(getattr<int>("bar_pressure_spatial_barnes_passed"));
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
    setattr("road_segment_id", value);
  }

  /**
   * @brief set mean air temperature for the nearest surface stations value
   * @param[in] value mean air temperature for the nearest surface stations value
   */
  inline void set_nss_air_temp_mean(float value)
  {
    setattr("nss_air_temp_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean surface temperature for the nearest surface stations value
   * @param[in] value mean surface temperature for the nearest surface stations value
   */
  inline void set_nss_surface_temp_mean(float value)
  {
    setattr("nss_surface_temp_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean barometric pressure for the nearest surface stations value
   * @param[in] value mean barometric pressure for the nearest surface stations value
   */
  inline void set_nss_bar_press_mean(float value)
  {
    setattr("nss_bar_pressure_mean", static_cast<double>(value));
  }

  /**
   * @brief set air temperature from the model data value
   * @param[in] value air temperature from the model data value
   */
  inline void set_model_air_temp(float value)
  {
    setattr("model_air_temp", static_cast<double>(value));
  }

  /**
   * @brief set dewpoint from the model data value
   * @param[in] value dewpoint from the model data value
   */
  inline void set_model_dewpoint(float value)
  {
    setattr("model_dew_temp", static_cast<double>(value));
  }

  /**
   * @brief set barometric pressure from the model data value
   * @param[in] value barometric pressure from the model data value
   */
  inline void set_model_bar_press(float value)
  {
    setattr("model_bar_pressure", static_cast<double>(value));
  }

  /**
   * @brief set reflectivity from radar gridvalue
   * @param[in] value reflectivity from radar grid value
   */
  inline void set_radar_ref(float value)
  {
    setattr("radar_ref", static_cast<double>(value));
  }

  /**
   * @brief set composite reflectivity from radar gridvalue
   * @param[in] value reflectivity from radar grid value
   */
  inline void set_radar_cref(float value)
  {
    setattr("radar_cref", static_cast<double>(value));
  }

  /**
   * @brief set precip flag from radar grid value
   * @param[in] value precip flag from radar grid value
   */
  inline void set_radar_precip_flag(short value)
  {
    setattr("radar_precip_flag", static_cast<int>(value));
  }

  /**
   * @brief set precip type from radar grid value
   * @param[in] value precip type from radar grid value
   */
  inline void set_radar_precip_type(short value)
  {
    setattr("radar_precip_type", static_cast<int>(value));
  }

  /**
   * @brief set cloud mask from satellite grid value
   * @param[in] value cloud mask from satellite grid  value
   */
  inline void set_cloud_mask(short value)
  {
    setattr("cloud_mask", static_cast<int>(value));
  }

  /**
   * @brief set mean dew temperature for the nearest surface stations value
   * @param[in] value mean dew temperature for the nearest surface stations  value
   */
  inline void set_nss_dew_temp_mean(float value)
  {
    setattr("nss_dew_temp_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean wind direction for the nearest surface stations value
   * @param[in] value mean wind direction for the nearest surface stations  value
   */
  inline void set_nss_wind_dir_mean(float value)
  {
    setattr("nss_wind_dir_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean wind speed for the nearest surface stations value
   * @param[in] value  mean wind speed for the nearest surface stations value
   */
  inline void set_nss_wind_speed_mean(float value)
  {
    setattr("nss_wind_speed_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean hourly precipitation for the nearest surface stations value
   * @param[in] value  mean hourly precipitation for the nearest surface stations value
   */
  inline void set_nss_hourly_precip_mean(float value)
  {
    setattr("nss_hourly_precip_mean", static_cast<double>(value));
  }

  /**
   * @brief set mean prevailing visibility for the nearest surface stations value
   * @param[in] value  mean prevailing visibility for the nearest surface stations value
   */
  inline void set_nss_prevail_vis_mean(float value)
  {
    setattr("nss_prevail_vis_mean", static_cast<double>(value));
  }

  /**
   * @brief set quality control flag for elevation value
   * @param[in] value quality control flag for elevation  value
   */
  inline void set_elevation_range_qc_passed(unsigned char value)
  {
    setattr("elevation_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for heading value
   * @param[in] value  quality control flag for heading value
   */
  inline void set_heading_range_qc_passed(unsigned char value)
  {
    setattr("heading_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for speed value
   * @param[in] value  quality control flag for speed value
   */
  inline void set_speed_range_qc_passed(unsigned char value)
  {
    setattr("speed_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for brake status value
   * @param[in] value  quality control flag for brake status value
   */
  inline void set_brake_status_range_qc_passed(unsigned char value)
  {
    setattr("brake_status_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for brake boost value
   * @param[in] value  quality control flag for brake boost value
   */
  inline void set_brake_boost_range_qc_passed(unsigned char value)
  {
    setattr("brake_boost_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for anti lock brake status value
   * @param[in] value  quality control flag for anti lock brake status value
   */
  inline void set_abs_range_qc_passed(unsigned char value)
  {
    setattr("abs_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for stability value
   * @param[in] value quality control flag for stability value
   */
  inline void set_stab_range_qc_passed(unsigned char value)
  {
    setattr("stab_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for traction value
   * @param[in] value quality control flag for traction value
   */
  inline void set_trac_range_qc_passed(unsigned char value)
  {
    setattr("trac_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for lights value
   * @param[in] value quality control flag for lights value
   */
  inline void set_lights_range_qc_passed(unsigned char value)
  {
    setattr("lights_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for yaw value
   * @param[in] value quality control flag for yaw value
   */
  inline void set_yaw_range_qc_passed(unsigned char value)
  {
    setattr("yaw_rate_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for horizontal longitudinal acceleration value
   * @param[in] value  quality control flag for horizontal longitudinal acceleration value
   */
  inline void set_hoz_accel_long_range_qc_passed(unsigned char value)
  {
    setattr("hoz_accel_long_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for horizontal lateral acceleration value
   * @param[in] value  quality control flag for horizontal lateral acceleration value
   */
  inline void set_hoz_accel_lat_range_qc_passed(unsigned char value)
  {
    setattr("hoz_accel_lat_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for steering angle value
   * @param[in] value  quality control flag for steering angle value
   */
  inline void set_steering_angle_range_qc_passed(unsigned char value)
  {
    setattr("steering_angle_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for for steering rate value
   * @param[in] value  quality control flag for for steering rate value
   */
  inline void set_steering_rate_range_qc_passed(unsigned char value)
  {
    setattr("steering_rate_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set quality control flag for front wiper status value
   * @param[in] value  quality control flag for front wiper status value
   */
  inline void set_wipers_front_status_range_qc_passed(unsigned char value)
  {
    setattr("wipers_front_status_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature sensor range test quality control flag value
   * @param[in] value  set air temperature sensor range test quality control flag value
   */
  inline void set_air_temp_range_qc_passed(unsigned char value)
  {
    setattr("air_temp_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature nearest surface station quality control flag value
   * @param[in] value  air temperature nearest surface station quality control flag value
   */
  inline void set_air_temp_sdt_passed(unsigned char value)
  {
    setattr("air_temp_sdt_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature nearest surface station quality control flag value
   * @param[in] value  air temperature nearest surface station quality control flag value
   */
  inline void set_air_temp_nst_passed(unsigned char value)
  {
    setattr("air_temp_nst_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature model analysis quality control flag value
   * @param[in] value  air temperature model analysis quality control flag value
   */
  inline void set_air_temp_mat_passed(unsigned char value)
  {
    setattr("air_temp_mat_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature climate range test quality control flag value
   * @param[in] value  air temperature climate range test quality control flag value
   */
  inline void set_air_temp_crt_passed(unsigned char value)
  {
    setattr("air_temp_crt_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature neighboring vehicle test quality control flag value
   * @param[in] value  air temperature neighboring vehicle test quality control flag value
   */
  inline void set_air_temp_nvt_passed(unsigned char value)
  {
    setattr("air_temp_nvt_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature combined algorithm test quality control flag value
   * @param[in] value  air temperature combined algorithm test quality control flag value
   */
  inline void set_air_temp_cat_passed(unsigned char value)
  {
    setattr("air_temp_cat_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature persistence test quality control flag value
   * @param[in] value air temperature persistence test quality control flag  value
   */
  inline void set_air_temp_persist_passed(unsigned char value)
  {
    setattr("air_temp_persist_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature time step test quality control flag value
   * @param[in] value  air temperature time step test quality control flag value
   */
  inline void set_air_temp_step_passed(unsigned char value)
  {
    setattr("air_temp_step_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature sensor range test quality control flag value
   * @param[in] value external air temperature sensor range test quality control flag  value
   */
  inline void set_air_temp2_range_qc_passed(unsigned char value)
  {
    setattr("air_temp2_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature standard deviation test quality control flag value
   * @param[in] value  external air temperature standard deviation test quality control flag value
   */
  inline void set_air_temp2_sdt_passed(unsigned char value)
  {
    setattr("air_temp2_sdt_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature nearest surface station quality control flag nvalue
   * @param[in] value  external air temperature nearest surface station quality control flag nvalue
   */
  inline void set_air_temp2_nst_passed(unsigned char value)
  {
    setattr("air_temp2_nst_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature model analysis quality control flag value
   * @param[in] value external air temperature model analysis quality control flag value
   */
  inline void set_air_temp2_mat_passed(unsigned char value)
  {
    setattr("air_temp2_mat_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature climate range test quality control flag value
   * @param[in] value  external air temperature climate range test quality control flag value
   */
  inline void set_air_temp2_crt_passed(unsigned char value)
  {
    setattr("air_temp2_crt_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature neighboring vehicle test quality control flag value
   * @param[in] value  external air temperature neighboring vehicle test quality control flag value
   */
  inline void set_air_temp2_nvt_passed(unsigned char value)
  {
    setattr("air_temp2_nvt_passed", static_cast<int>(value));
  }

  /**
   * @brief set external air temperature combined algorithm test quality control flag value
   * @param[in] value  external air temperature combined algorithm test quality control flag value
   */
  inline void set_air_temp2_cat_passed(unsigned char value)
  {
    setattr("air_temp2_cat_passed", static_cast<int>(value));
  }

   /**
   * @brief set external air temperature persistence test quality control flag value
   * @param[in] value  external air temperature persistence test quality control flag value
   */
  inline void set_air_temp2_persist_passed(unsigned char value)
  {
    setattr("air_temp2_persist_passed", static_cast<int>(value));
  }

   /**
   * @brief set external air temperature time step test quality control flag value
   * @param[in] value  external air temperature time step test quality control flag value
   */
  inline void set_air_temp2_step_passed(unsigned char value)
  {
    setattr("air_temp2_step_passed", static_cast<int>(value));
  }

 /**
   * @brief set dew temperature sensor range test quality control flag value
   * @param[in] value dew temperature sensor range test quality control flag  value
   */
  inline void set_dew_temp_range_qc_passed(unsigned char value)
  {
    setattr("dew_temp_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature climate range test quality control flag value
   * @param[in] value  dew temperature climate range test quality control flag value
   */
  inline void set_dew_temp_nst_passed(unsigned char value)
  {
    setattr("dew_temp_nst_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature combined algorithm test quality control flag value
   * @param[in] value  dew temperature combined algorithm test quality control flag value
   */
  inline void set_dew_temp_cat_passed(unsigned char value)
  {
    setattr("dew_temp_cat_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature model analysis quality control flag value
   * @param[in] value external dew temperature model analysis quality control flag value
   */
  inline void set_dew_temp_mat_passed(unsigned char value)
  {
    setattr("dew_temp_mat_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature neighboring vehicle test quality control flag value
   * @param[in] value dew temperature neighboring vehicle test quality control flag value
   */
  inline void set_dew_temp_nvt_passed(unsigned char value)
  {
    setattr("dew_temp_nvt_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature nearest surface station quality control flag value
   * @param[in] value dew temperature nearest surface station quality control flag value
   */
  inline void set_dew_temp_sdt_passed(unsigned char value)
  {
    setattr("dew_temp_sdt_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature spatial iqr quality control flag value
   * @param[in] value dew temperature spatial iqr quality control flag value
   */
  inline void set_dew_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    setattr("dew_temp_spatial_iqr_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature spatial barnes quality control flag value
   * @param[in] value  dew temperature spatial barnes quality control flag value
   */
  inline void set_dew_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    setattr("dew_temp_spatial_barnes_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature climate range test quality control flag value
   * @param[in] value dew temperature climate range test quality control flag value
   */
  inline void set_dew_temp_crt_passed(unsigned char value)
  {
    setattr("dew_temp_crt_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature persistence test quality control flag value
   * @param[in] value dew temperature persistence test quality control flag  value
   */
  inline void set_dew_temp_persist_passed(unsigned char value)
  {
    setattr("dew_temp_persist_passed", static_cast<int>(value));
  }

  /**
   * @brief set dew temperature time step test quality control flag value
   * @param[in] value  dew temperature time step test quality control flag value
   */
  inline void set_dew_temp_step_passed(unsigned char value)
  {
    setattr("dew_temp_step_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure sensor range test  quality control flag value
   * @param[in] value  barometric pressure sensor range test  quality control flag value
   */
  inline void set_bar_press_range_qc_passed(unsigned char value)
  {
    setattr("bar_pressure_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure persistence test quality control flag value
   * @param[in] value barometric pressure persistence test quality control flag value
   */
  inline void set_bar_press_persist_passed(unsigned char value)
  {
    setattr("bar_pressure_persist_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure standard deviation test quality control flag value
   * @param[in] value  barometric pressure standard deviation test quality control flag value
   */
  inline void set_bar_press_sdt_passed(unsigned char value)
  {
    setattr("bar_pressure_sdt_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure nearest surface station quality control flag value
   * @param[in] value  barometric pressure nearest surface station quality control flag value
   */
  inline void set_bar_press_nst_passed(unsigned char value)
  {
    setattr("bar_pressure_nst_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure model analysis quality control flag value
   * @param[in] value  barometric pressure model analysis quality control flag value
   */
  inline void set_bar_press_mat_passed(unsigned char value)
  {
    setattr("bar_pressure_mat_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure climate range test quality control flag value
   * @param[in] value  barometric pressure climate range test quality control flag value
   */
  inline void set_bar_press_crt_passed(unsigned char value)
  {
    setattr("bar_pressure_crt_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure neighboring vehicle test quality control flag value
   * @param[in] value  barometric pressure neighboring vehicle test quality control flag value
   */
  inline void set_bar_press_nvt_passed(unsigned char value)
  {
    setattr("bar_pressure_nvt_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure combined algorithm test quality control flag value
   * @param[in] value  barometric pressure combined algorithm test quality control flag value
   */
  inline void set_bar_press_cat_passed(unsigned char value)
  {
    setattr("bar_pressure_cat_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure time step test quality control flag value
   * @param[in] value  barometric pressure time step test quality control flag value
   */
  inline void set_bar_press_step_passed(unsigned char value)
  {
    setattr("bar_pressure_step_passed", static_cast<int>(value));
  }

  /**
   * @brief set latitude data filtering test quality control flag value
   * @param[in] value  latitude data filtering test quality control flag value
   */
  inline void set_latitude_dft_passed(unsigned char value)
  {
    setattr("latitude_dft_passed", static_cast<int>(value));
  }

  /**
   * @brief set longitude data filtering test quality control flag value
   * @param[in] value  longitude data filtering test quality control flag value
   */
  inline void set_longitude_dft_passed(unsigned char value)
  {
    setattr("longitude_dft_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature climate range test quality control flag value
   * @param[in] value  surface temperature climate range test quality control flag value
   */
  inline void set_surface_temp_range_qc_passed(unsigned char value)
  {
    setattr("surface_temp_range_qc_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature nearest surface station quality control flag value
   * @param[in] value  surface temperature nearest surface station quality control flag value
   */
  inline void set_surface_temp_sdt_passed(unsigned char value)
  {
    setattr("surface_temp_sdt_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature climate range test quality control flag value
   * @param[in] value  surface temperature climate range test quality control flag value
   */
  inline void set_surface_temp_crt_passed(unsigned char value)
  {
    setattr("surface_temp_crt_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature neighboring vehicle test quality control flag value
   * @param[in] value  surface temperature neighboring vehicle test quality control flag value
   */
  inline void set_surface_temp_nvt_passed(unsigned char value)
  {
    setattr("surface_temp_nvt_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature combined algorithm test quality control flag value
   * @param[in] value  surface temperature combined algorithm test quality control flag value
   */
  inline void set_surface_temp_cat_passed(unsigned char value)
  {
    setattr("surface_temp_cat_passed", static_cast<int>(value));
  }

  /**
   * @brief set air temperature spatial iqr quality control flag value
   * @param[in] value  air temperature spatial iqr quality control flag value
   */
  inline void set_air_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    setattr("air_temp_spatial_iqr_passed", static_cast<int>(value));
  }

  /**
   * @brief set second air temperature spatial iqr quality control flag value
   * @param[in] value  second air temperature spatial iqr quality control flag value
   */
  inline void set_air_temp2_spatial_iqr_qc_passed (unsigned char value)
  {
    setattr("air_temp2_spatial_iqr_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature spatial iqr quality control flag value
   * @param[in] value  surface temperature spatial iqr quality control flag value
   */
  inline void set_surface_temp_spatial_iqr_qc_passed (unsigned char value)
  {
    setattr("surface_temp_spatial_iqr_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature persistence quality control flag value
   * @param[in] value  surface temperature persistence quality control flag value
   */
  inline void set_surface_temp_persist_passed (unsigned char value)
  {
    setattr("surface_temp_persist_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature time step quality control flag value
   * @param[in] value  surface temperature time step quality control flag value
   */
  inline void set_surface_temp_step_passed (unsigned char value)
  {
    setattr("surface_temp_step_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure spatial iqr quality control flag value
   * @param[in] value  barometric pressure spatial iqr quality control flag value
   */
  inline void set_bar_press_spatial_iqr_qc_passed (unsigned char value)
  {
    setattr("bar_pressure_spatial_iqr_passed", static_cast<int>(value));
  }


  /**
   * @brief set air temperature spatial barnes quality control flag value
   * @param[in] value  air temperature spatial barnes quality control flag value
   */
  inline void set_air_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    setattr("air_temp_spatial_barnes_passed", static_cast<int>(value));
  }

  /**
   * @brief set second air temperature spatial barnes quality control flag value
   * @param[in] value  second air temperature spatial barnes quality control flag value
   */
  inline void set_air_temp2_spatial_barnes_qc_passed (unsigned char value)
  {
    setattr("air_temp2_spatial_barnes_passed", static_cast<int>(value));
  }

  /**
   * @brief set surface temperature spatial barnes quality control flag value
   * @param[in] value  surface temperature spatial barnes quality control flag value
   */
  inline void set_surface_temp_spatial_barnes_qc_passed (unsigned char value)
  {
    setattr("surface_temp_spatial_barnes_passed", static_cast<int>(value));
  }

  /**
   * @brief set barometric pressure spatial barnes quality control flag value
   * @param[in] value  barometric pressure spatial barnes quality control flag value
   */
  inline void set_bar_press_spatial_barnes_qc_passed (unsigned char value)
  {
    setattr("bar_pressure_spatial_barnes_passed", static_cast<int>(value));
  }

  inline void set_air_temp_cat_confidence(float con) 
  {
    setattr("air_temp_cat_confidence", static_cast<double>(con));
  }

  inline void set_air_temp2_cat_confidence(float con) 
  {
    setattr("air_temp2_cat_confidence", static_cast<double>(con));
  }

  inline void set_bar_press_cat_confidence(float con) 
  {
    setattr("bar_pressure_cat_confidence", static_cast<double>(con));
  }

  inline void set_surface_temp_cat_confidence(float con) 
  {
    setattr("surface_temp_cat_confidence", static_cast<double>(con));
  }

  inline void set_dew_temp_cat_confidence(float con) 
  {
    setattr("dew_temp_cat_confidence", static_cast<double>(con));
  }

  /**
   * @brief  copy input messages to output messages based on match of rec_time and obs_time
   * @param[in] msg_to_copy source messages to copy to destination
   * @param[in,out] destination_msg contains the messages to replace with suitable messages from msg_to_copy
   */
  static void match_copy(const vector<vdt_probe_message_qc> &msg_to_copy, vector<vdt_probe_message_qc> &destination_msg);

private:
  void init();
};

#endif /* VDT_PROBE_MESSAGE_QC_HH */
