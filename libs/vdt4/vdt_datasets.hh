//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_datasets.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/08 16:57:43 $
//
//==============================================================================

/**
 *
 * @file vdt_datasets.hh
 *
 * @class vdt_datasets
 *
 *  vdt datasets class
 *
 */

#ifndef VDT_DATASETS_HH
#define VDT_DATASETS_HH

/**
 * @class vdt_datasets
 */
class vdt_datasets
{

public:

  /** @brief  air temperature from the model data */
  float model_air_temp;

  /** @brief  barometric pressure from the model data */
  float model_bar_press;

  /** @brief  dewpoint from the model data */
  float model_dewpoint;

  /** @brief  geopotential height from the model data */
  float model_geopotential_height;

  /** @brief  wind speed from the model data */
  float model_wind_speed;

  /** @brief  wind dir from the model data */
  float model_wind_dir;

  /** @brief  reflectivity from the radar grid */
  float radar_ref;

  /** @brief  precip from the radar grid */
  short radar_precip_flag;

  /** @brief  precip type from the radar grid */
  short radar_precip_type;

  /** @brief  cloud mask from the satellite grid */
  short cloud_mask;

  /** @brief  mean air temperature for the nearest surface stations */
  float nss_air_temp_mean;

  /** @brief  mean barometric pressure for the nearest surface stations */
  float nss_bar_press_mean;

  /** @brief  mean dew temperature for the nearest surface stations */
  float nss_dew_temp_mean;

  /** @brief  mean wind direction for the nearest surface stations */
  float nss_wind_dir_mean;

  /** @brief  mean wind speed for the nearest surface stations */
  float nss_wind_speed_mean;

  /** @brief  mean hourly precipitation for the nearest surface stations */
  float nss_hourly_precip_mean;

  /** @brief  mean prevailing visibility for the nearest surface stations */
  float nss_prevail_vis_mean;

};

#endif /* VDT_DATASETS_HH */
