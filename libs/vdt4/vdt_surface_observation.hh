//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_surface_observation.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013-12-02 23:03:34 $
//
//==============================================================================

/**
 *
 * @file vdt_surface_observation.hh
 *
 * @class vdt_surface_observation
 *
 *  vdt surface observation class
 *
 */

#ifndef VDT_SURFACE_OBSERVATION_HH
#define VDT_SURFACE_OBSERVATION_HH

#include <iostream>
#include <string>
#include <vector>

#include "vdt_point.hh"

using namespace std;

/**
 * @class vdt_surface_observation
 * Represents a METAR: station name, vdt_point, time_obs, air_temp, mslp, etc.
 */
class vdt_surface_observation
{
public:

  /** @brief miles per hour to meters per second conversion value */
  const static float MPH_TO_MS;

  /** @brief knots to meters per second conversion value */
  const static float KNOT_TO_MS;

  /** @brief miles to kilometers conversion value */
  const static float MI_TO_KM;

  /** @brief inches to millimeters conversion value */
  const static float IN_TO_MM;

  /** @brief kelvin to celsius conversion value */
  const static float K_TO_C;

  /** @brief pascal to millbars conversion value */
  const static float PA_TO_MB;

  /** @brief fill value */
  const static float FILL_VALUE;

  /** @brief maximum millibars */
  const static float MAX_MB;

  /** @brief minimum millibars */
  const static float MIN_MB;

  /** @brief maximum dewpoint in Celsius */
  const static float MAX_DEWP_C;

  /** @brief maximum air temp in Celsius */
  const static float MAX_AIR_C;

  /**
   * @brief converts miles per hour to meters per second
   * @param[in] v value in miles per hour
   * @return value in meters per second
   */
  static float mph_to_ms(float v);

  /**
   * @brief converts knots to meters per second
   * @param[in] v value in knots
   * @return value in meters per second
   */
  static float knot_to_ms(float v);

  /**
   * @brief converts miles to kilometers
   * @param[in] v value in miles
   * @return value in kilometers
   */
  static float mi_to_km(float v);

  /**
   * @brief converts inches to millimeters
   * @param[in] v value in inches
   * @return value in millimeters
   */
  static float in_to_mm(float v);

  /**
   * @brief converts kelvin to celsius
   * @param[in] v value in kelvin
   * @return value in celsius
   */
  static float k_to_c(float v);

  /**
   * @brief converts celsius to kelvin
   * @param[in] v value celsius
   * @return value in kelvin
   */
  static float c_to_k(float v);

  /**
   * @brief converts pascal to millbars
   * @param[in] v value in pascal
   * @return value in millibars
   */
  static float pa_to_mb(float v);

  typedef float (vdt_surface_observation::*getter)() const;

  /** constructor */
  vdt_surface_observation();

  /**
   * @brief get points
   * @param[in] obs vector of surface observations
   * @param[out] points vector of surface observation points
   */
  static void get_points(const vector<vdt_surface_observation>& obs, vector<vdt_point> &points);

  /**
   * @brief get average air temperature
   * @param[in] obs vector of surface observations
   * @return average air temperature
   */
  static float get_avg_air_temp(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average surface temperature
   * @param[in] obs vector of surface observations
   * @return average surface temperature
   */
  static float get_avg_surface_temp(const vector<vdt_surface_observation>& obs);
  
  /**
   * @brief get average pressure
   * @param[in] obs vector of surface observations
   * @return average pressure
   */
  static float get_avg_press(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average visibility
   * @param[in] obs vector of surface observations
   * @return average visibility
   */
  static float get_avg_vis(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average dew temperature
   * @param[in] obs vector of surface observations
   * @return average dew temperature
   */
  static float get_avg_dew_temp(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average wind speed and direction utility function
   * @param[in] wind speed vector
   * @param[in] wind dir vector
   * @param[out] mean_wind_speed_ptr container for mean wind speed
   * @param[out] mean_wind_dir_ptr container for mean wind direction
   * @return 0 on success, -1 on failure
   */
  static int get_avg_speed_dir(const vector<float> &speed, const vector<float> &dir, float *mean_wind_speed_ptr, float *mean_wind_dir_ptr);

  /**
   * @brief get average wind speed
   * @param[in] obs vector of surface observations
   * @return average wind speed
   */
  static float get_avg_wind_speed(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average wind gust
   * @param[in] obs vector of surface observations
   * @return average wind gust
   */
  static float get_avg_wind_gust(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average wind direction
   * @param[in] obs vector of surface observations
   * @return average wind direction
   */
  static float get_avg_wind_dir(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average wind speed and direction
   * @param[in] obs vector of surface observations
   * @param[out] mean_wind_speed container for mean wind speed
   * @param[out] mean_wind_dir container for mean wind direction
   */
  static void get_wind_speed_dir(const vector<vdt_surface_observation>& obs, float *mean_speed, float *mean_dir);
    


  /**
   * @brief get average hourly precipitation
   * @param[in] obs vector of surface observations
   * @return average hourly precipitation
   */
  static float get_avg_hourly_precip(const vector<vdt_surface_observation>& obs);

  /**
   * @brief get average
   * @param[in] obs  vector of surface observations
   * @param[in] ptFn pointer function
   * @return average
   */
  static float get_avg(const vector<vdt_surface_observation>& obs, getter ptFn);

  /**
   * @brief  gets vdt_point
   * @return vdt_point value
   */
  inline const vdt_point &get_point() const
  {
    return point;
  }

  /**
   * @brief  sets vdt_point
   * @param[in] value vdt_point value
   */
  inline void set_point(vdt_point value)
  {
    point = value;
  }

  /**
   * @brief  gets station name
   * @return station name value
   */
  inline string get_station_name() const
  {
    return station_name;
  }


  /**
   * @brief  sets station name
   * @param[in] value  station name value
   */
  inline void set_station_name(string value)
  {
    station_name = value;
  }

  /**
   * @brief  gets observation time
   * @return observation time value
   */
  inline double get_time_obs() const
  {
    return time_obs;
  }

  /**
   * @brief  sets observation time
   * @param[in] value  observation time value
   */
  inline void set_time_obs(double value)
  {
    time_obs = value;
  }

  /**
   * @brief  gets air temperature
   * @return air temperature value
   */
  inline float get_air_temp() const
  {
    if (air_temp >= MAX_AIR_C)
    {
      return FILL_VALUE;
    }
    return air_temp;
  }

  /**
   * @brief  sets air temperature
   * @param[in] value air temperature value
   */
  inline void set_air_temp(float value)
  {
    air_temp = value;
  }

  /**
   * @brief  gets surface temperature
   * @return surface temperature value
   */
  inline float get_surface_temp() const
  {
    return surface_temp;
  }

  /**
   * @brief  sets surface temperature
   * @param[in] value  surface temperature value
   */
  inline void set_surface_temp(float value)
  {
    surface_temp = value;
  }

  /**
   * @brief  gets dew temperature
   * @return dew temperature value
   */
  inline float get_dew_temp() const
  {
    if (dew_temp >= MAX_DEWP_C)
    {
      return FILL_VALUE;
    }
    return dew_temp;
  }

  /**
   * @brief  sets dew temperature
   * @param[in] value dew temperature value
   */
  inline void set_dew_temp(float value)
  {
    dew_temp = value;
  }

  /**
   * @brief  gets pressure
   * @return pressure value
   */
  inline float get_press() const
  {
    if (press > MAX_MB || press < MIN_MB)
      {
	return FILL_VALUE;
      }
    return press;
  }

  /**
   * @brief  sets pressure
   * @param[in] value pressure value
   */
  inline void set_press(float value)
  {
    press = value;
  }


  /**
   * @brief  gets hourly precipitation
   * @return hourly precipitation value
   */
  inline float get_hourly_precip() const
  {
    return hourly_precip;
  }

  /**
   * @brief  sets hourly precipitation
   * @param[in] value  hourly precipitation value
   */
  inline void set_hourly_precip(float value)
  {
    hourly_precip = value;
  }

  /**
   * @brief  gets 24 hour precipitation
   * @return 24 hour precipitation value
   */
  inline float get_precip_24hr() const
  {
    return precip_24hr;
  }

  /**
   * @brief  sets 24 hour precipitation
   * @param[in] value  24 hour precipitation value
   */
  inline void set_precip_24hr(float value)
  {
    precip_24hr = value;
  }

  /**
   * @brief  gets prevailing visibility
   * @return prevailing visibility value
   */
  inline float get_prevail_vis() const
  {
    return prevail_vis;
  }

  /**
   * @brief  sets prevailing visibility
   * @param[in] value prevailing visibility value
   */
  inline void set_prevail_vis(float value)
  {
    prevail_vis = value;
  }

  /**
   * @brief  gets wind direction
   * @return wind direction value
   */
  inline float get_wind_dir() const
  {
    return wind_dir;
  }

  /**
   * @brief  sets wind direction
   * @param[in] value wind direction  value
   */
  inline void set_wind_dir(float value)
  {
    wind_dir = value;
  }

  /**
   * @brief  gets wind speed
   * @return wind speed value
   */
  inline float get_wind_speed() const
  {
    return wind_speed;
  }

  /**
   * @brief  sets wind speed
   * @param[in] value  wind speed value
   */
  inline void set_wind_speed(float value)
  {
    wind_speed = value;
  }

  /**
  * @brief  gets wind gust
  * @return wind gust value
  */
  inline float get_wind_gust() const
  {
    return wind_gust;
  }

  /**
   * @brief  sets wind gust
   * @param[in] value  wind gust value
   */
  inline void set_wind_gust(float value)
  {
    wind_gust = value;
  }

  /**
   * @brief  gets present weather
   * @return present weather value
   */
  inline string get_pres_wx() const
  {
    return pres_wx;
  }


  /**
   * @brief  sets present weather
   * @param[in] value  present weather value
   */
  inline void set_pres_wx(string value)
  {
    pres_wx = value;
  }

  /**
   * @brief  gets weather phenomena
   * @return weather phenomena value
   */
  inline string get_wx() const
  {
    return wx;
  }

  /**
   * @brief  sets weather phenomena
   * @param[in] value weather phenomena value
   */
  inline void set_wx(string value)
  {
    wx = value;
  }

  /**
   * @brief  sets integer fill value
   * @param[in] v  integer value
   * @param[in] fill  fill value
   */
  static void set_fill(int& v, int fill);

  /**
   * @brief  sets float fill value
   * @param[in] v  float value
   * @param[in] fill  fill value
   */
  static void set_fill(float& v, float fill);

protected:

  string station_name;

  vdt_point point;

  /** @brief obs time in seconds since epoch */
  double time_obs;

  /** @brief air temperature in celsius */
  float air_temp;

  /** @brief surface temperature in celsius */
  float surface_temp;

  /** @brief station pressure in pascal */
  float press;

  /** @brief sea level pressure in pascal */
  float slp;

  /** @brief dew point temperature in celsius */
  float dew_temp;

  /** @brief precipitation in mm */
  float hourly_precip;

  /** @brief precipitation in mm */
  float precip_24hr;

  /** @brief prevailing visibility in KM */
  float prevail_vis;

  /** @brief wind direction in m/s */
  float wind_dir;

  /** @brief wind speed in m/s */
  float wind_speed;

  /** @brief wind gust in m/s */
  float wind_gust;

  /** @brief present weather (origin Madis)*/
  std::string pres_wx;

  /** @brief weather pheonmena (origin Metar)*/
  std::string wx;
};


/**
 * @class vdt_compare_surface_observation
 * perform comparison on surface observations
 */
class vdt_compare_surface_observation
{
protected:

public:

  /** @brief compare surface observations */
  vdt_compare_surface_observation(const float (vdt_surface_observation::*getter)());
};

#endif /* VDT_SURFACE_OBSERVATION_HH */
