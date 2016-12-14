//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_dataset.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/08 16:57:43 $
//
//==============================================================================

/**
 *
 * @file vdt_dataset.hh
 *
 * @class vdt_dataset
 *
 *  vdt dataset class
 *
 */

#ifndef VDT_DATASET_HH
#define VDT_DATASET_HH

#include "vdt_const.hh"
#include <string>

/**
 * @class vdt_dataset
 */
class vdt_dataset
{

public:

  /** @brief vdt_dataset void constructor */
  vdt_dataset()
  {
    lat = vdt_const::FILL_VALUE;
    lon = vdt_const::FILL_VALUE;
    model_x = vdt_const::FILL_VALUE;
    model_y = vdt_const::FILL_VALUE;
    model_air_temp = vdt_const::FILL_VALUE;
    model_bar_press = vdt_const::FILL_VALUE;
    model_dewpoint = vdt_const::FILL_VALUE;
    model_wind_dir = vdt_const::FILL_VALUE;
    model_wind_speed = vdt_const::FILL_VALUE;
    radar_x = vdt_const::SHORT_FILL_VALUE;
    radar_y = vdt_const::SHORT_FILL_VALUE;
    radar_ref = vdt_const::FILL_VALUE;
    radar_cref = vdt_const::FILL_VALUE;
    radar_dual_pol_hc = vdt_const::FILL_VALUE;
    radar_dual_pol_hr = vdt_const::FILL_VALUE;        
    radar_precip_flag = vdt_const::SHORT_FILL_VALUE;
    radar_precip_type = vdt_const::SHORT_FILL_VALUE;
    cloud_x = vdt_const::SHORT_FILL_VALUE;
    cloud_y = vdt_const::SHORT_FILL_VALUE;
    cloud_mask = vdt_const::SHORT_FILL_VALUE;
    nss_air_temp_mean = vdt_const::FILL_VALUE;
    nss_surface_temp_mean = vdt_const::FILL_VALUE;
    nss_bar_press_mean = vdt_const::FILL_VALUE;
    nss_dew_temp_mean = vdt_const::FILL_VALUE;
    nss_wind_dir_mean = vdt_const::FILL_VALUE;
    nss_wind_speed_mean = vdt_const::FILL_VALUE;
    nss_hourly_precip_mean = vdt_const::FILL_VALUE;
    nss_prevail_vis_mean = vdt_const::FILL_VALUE;
    pres_wx = "";
    wx = "";
    road_state_1 = vdt_const::SHORT_FILL_VALUE;
  };

  /** @brief lat latitude */
  double lat;

  /** @brief lon longitude */
  double lon;

  /** @brief model_x  x coord of model */
  double model_x;

  /** @brief model_y y coord of model */
  double model_y;

  /** @brief  model_air_temp air temperature from the model data */
  float model_air_temp;

  /** @brief  model_bar_press barometric pressure from the model data */
  float model_bar_press;

  /** @brief  model_dewpoint dewpoint from the model data */
  float model_dewpoint;

  /** @brief  geopotential height from the model data */
  float model_geopotential_height;

  /** @brief  wind direction from the model data */
  float model_wind_dir;

  /** @brief  wind speed from the model data */
  float model_wind_speed;

  /** @brief  x coord of reflectivity from the radar grid */
  int radar_x;

  /** @brief  y coord of reflectivity from the radar grid */
  int radar_y;

  /** @brief  base reflectivity from the radar grid */
  float radar_ref;

  /** @brief  composite reflectivity from the radar grid */
  float radar_cref;

  /** @brief level 3 classification from the radar grid */
  float radar_dual_pol_hc;
  
  /** @brief level 3 reflectivity from the radar grid */
  float radar_dual_pol_hr;    

  /** @brief  precip from the radar grid */
  short radar_precip_flag;

  /** @brief  precip type from the radar grid */
  short radar_precip_type;

  /** @brief  x coord of cloud mask */
  int cloud_x;

  /** @brief  y coord of cloud mask */
  int cloud_y;

  /** @brief  cloud mask from the satellite grid */
  short cloud_mask;

  /** @brief present weather from madis */
  std::string pres_wx;

  /** @brief weather phenomena from metar */
  std::string wx;

  /** @brief road state 1 from madis */
  int road_state_1;
  
  /** @brief  mean air temperature for the nearest surface stations */
  float nss_air_temp_mean;

  /** @brief  mean surface temperature for the nearest surface stations */
  float nss_surface_temp_mean;

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

#endif /* VDT_DATASET_HH */
