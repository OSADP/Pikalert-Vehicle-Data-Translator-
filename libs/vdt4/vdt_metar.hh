//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_metar.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_metar.hh
 *
 * @class vdt_metar
 *
 *  vdt metar class
 *
 */
#ifndef VDT_METAR_HH
#define VDT_METAR_HH

#include <iostream>
#include <string>

#include "vdt_point.hh"

using namespace std;

/**
 * @class vdt_metar
 * Represents a METAR: station name, vdt_point, time_obs, air_temp, mslp, etc.
 */
class vdt_metar
{
protected:

  /** @brief station name */
  string station_name;

  /** @brief observation point consisting of a latitude, longitude, and elevation */
  vdt_point point;

  /** @brief observation time */
  double time_obs;

  /** @brief air temperature */
  float air_temp;

  /** @brief sea level pressure */
  float slp;

  /** @brief dew point temperature */
  float dew_temp;

  /** @brief hourly precipitation amount */
  float hourly_precip;

  /** @brief 24 hour precipitation amount */
  float precip_24hr;

  /** @brief prevailing visibility in miles */
  float prevail_vis_mi;

  /** @brief prevailing visibility in kilometers */
  float prevail_vis_km;

  /** @brief wind direction */
  int wind_dir;

  /** @brief wind speed */
  int wind_speed;

  /** @brief wind gust */
  int wind_gust;

public:

  /** @brief get vdt point
   * @return vdt_point consisting of latitude, longitude, and elevation
   */
  inline const vdt_point get_point() const;

  /** @brief set vdt point
   * @param[in] value  vdt_point consisting of latitude, longitude, and elevation
   */
  void set_point(vdt_point value);

  /** @brief get station name
   * @return station name string
   */
  inline const string get_station_name() const;

  /** @brief set station name
   * @param[in] value  station name string
   */
  void set_station_name(string value);

  /** @brief get observation time
   * @return  observation time value
   */
  inline const double get_time_obs() const;

  /** @brief set observation time
   * @param[in] value  observation time value
   */
  void set_time_obs(double value);

  /** @brief get air temperature
   * @return  air temperature value
   */
  inline const float get_air_temp() const;

  /** @brief set air temperature
   * @param[in] value  air temperature value
   */
  void set_air_temp(float value);

  /** @brief get dew point temperature
   * @return  dew point temperature value
   */
  inline const float get_dew_temp() const;

  /** @brief set dew point temeperature
  * @param[in] value  dew point temperature value
  */
  void set_dew_temp(float value);

  /** @brief get sea level pressure
   * @return  sea level pressure value
   */
  inline const float get_slp() const;

  /** @brief set sea level presssure
   * @param[in] value  sea level pressure value
   */
  void set_slp(float value);

  /** @brief get hourly precipitation amount
   * @return hourly precipitation amount
   */
  inline const float get_hourly_precip() const;

  /** @brief set hourly precipitation amount
   * @param[in] value  set hourly precipitation amount
   */
  void set_hourly_precip(float value);

  /** @brief get 24 hour precipitation amount
   * @return  24 hour precipitation amount
   */
  inline const float get_precip_24hr() const;

  /** @brief set 24 hour precipitation amount
   * @param[in] value  24 hour precipitation amount
   */
  void set_precip_24hr(float value);

  /** @brief get prevailing visibility in miles
   * @return prevailing visibility value in miles
   */
  inline const float get_prevail_vis_mi() const;

  /** @brief set prevailing visibility in miles
   * @param[in] value prevailing visibility value in miles
   */
  void set_prevail_vis_mi(float value);

  /** @brief get prevailing visibility in kilometers
   * @return prevailing visibility value in kilometers
   */
  inline const float get_prevail_vis_km() const;

  /** @brief set prevailing visibility in kilometers
   * @param[in] value prevailing visibility value in kilometers
   */
  void set_prevail_vis_km(float value);

  /** @brief get wind direction
   * @return wind direction value
   */
  inline const int get_wind_dir() const;

  /** @brief set wind direction
   * @param[in] value  wind direction value
   */
  void set_wind_dir(int value);

  /** @brief get wind speed
   * @return wind speed value
   */
  inline const int get_wind_speed() const;

  /** @brief set wind speed
   * @param[in] value wind speed value
   */
  void set_wind_speed(int value);

  /** @brief get wind gust
   * @return wind gust value
   */
  inline const int get_wind_gust() const;

  /** @brief set wind gust
   * @param[in] value  wind gust value
   */
  void set_wind_gust(int value);
};

inline const vdt_point vdt_metar::get_point() const
{
  return point;
}

inline const string vdt_metar::get_station_name() const
{
  return station_name;
}

inline const double vdt_metar::get_time_obs() const
{
  return time_obs;
}

inline const float vdt_metar::get_air_temp() const
{
  return air_temp;
}

inline const float vdt_metar::get_dew_temp() const
{
  return dew_temp;
}

inline const float vdt_metar::get_slp() const
{
  return slp;
}

inline const float vdt_metar::get_hourly_precip() const
{
  return hourly_precip;
}

inline const float vdt_metar::get_precip_24hr() const
{
  return precip_24hr;
}

inline const float vdt_metar::get_prevail_vis_mi() const
{
  return prevail_vis_mi;
}

inline const float vdt_metar::get_prevail_vis_km() const
{
  return prevail_vis_km;
}

inline const int vdt_metar::get_wind_dir() const
{
  return wind_dir;
}

inline const int vdt_metar::get_wind_speed() const
{
  return wind_speed;
}

inline const int vdt_metar::get_wind_gust() const
{
  return wind_gust;
}

#endif /* VDT_METAR_HH */
