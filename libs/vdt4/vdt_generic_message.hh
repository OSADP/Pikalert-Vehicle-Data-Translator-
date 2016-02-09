//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_generic_message.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/03/28 21:25:34 $
//
//==============================================================================

/**
 *
 * @file vdt_generic_message.hh
 *
 * @class vdt_generic_message
 *
 *  vdt generic message class
 *
 */

#ifndef VDT_GENERIC_MESSAGE_HH
#define VDT_GENERIC_MESSAGE_HH

#include <iostream>
#include <string>
#include <sstream>
#include "vdt_point.hh"
#include "vdt_road_segment.hh"

using std::string;
using std::stringstream;

/**
 * @class vdt_generic_message
 * Represents the standard generic message
 */
class vdt_generic_message
{
public:
  /**
   * Observation time for the generic message in UNIX seconds
   * It is calculated from full_pos_utc_time_year, ...,
   * full_pos_utc_time_second.
   */
  double obs_time;

  /**
    * Reception time for generic message in UNIX seconds.
    * It is calculated from directory/timestamped filename
    */
  double rec_time;

  /** @brief vehicle identifier string value */
  string vehicle_id;

  /**
   * Point location
   * lat, long, elevation for a vdt_point will be being extracted from the
   * fullPos.lat, fullPos.longitude and fullPos.elevation elements.
   * The following conversion formulas are used:
   * For lat/long: xml-values * 1/8,000,000 in degrees
   * For elevation: xml-vlaue * 0.1 - 1000
   */
  vdt_point point;

  /** @brief road data field names */
  vector<string> field_names;

  /** @brief road data field values */
  vector<float> field_values;

  /**
   * @brief get field value based on supplied field number
   * @return  field value
   */
  inline float get_field(int field_number) const
  {
    return field_values[field_number];
  }

  /**
   * @brief get field value based on supplied field number
   * @return  field value
   */
  inline void set_field(int field_number, float field_value)
  {
    field_values[field_number] = field_value;
  }

  /** @brief constructor */
  vdt_generic_message();

  virtual ~vdt_generic_message();

  /**
   * @brief  get observation time  value
   * @return observation time value
   */
  inline const double get_obs_time() const
  {
    return obs_time;
  }

  /**
   * @brief  get  received time value
   * @return received time value
   */
  inline const double get_rec_time() const
  {
    return rec_time;
  }

  /**
   * @brief  get vehicle id  value
   * @return vehicle id value
   */
  inline const string get_vehicle_id() const
  {
    return vehicle_id;
  }

  /**
   * @brief  get elevation value
   * @return elevation value
   */
  const float get_elevation()
  {
    return point.get_elevation();
  }

  /**
   * @brief  get latitude value
   * @return latitude value
   */
  const double get_latitude()
  {
    return point.get_latitude();
  }

  /**
   * @brief  get longitude value
   * @return longitude value
   */
  const double get_longitude()
  {
    return point.get_longitude();
  }

  /**
   * @brief  get vdt point value (latitude, longitude, elevation)
   * @return vdt point value
   */
  inline const vdt_point get_point() const
  {
    return point;
  }

  /** @brief generic message header used for testing */
  static string generic_message_hdr()
  {
    return "obs_time,rec_time,vehicle_id,latitude,longitude,elevation,heading,speed,brake_status,brake_boost,abs,stab,trac,lights,yaw,hoz_accel_long,hoz_accel_lat,steering_angle,steering_rate,wipers_front_status,air_temp,bar_press,air_temp2,dew_temp,surface_temp";
  }

  /** @brief generic message string used for testing */
  string generic_message_string()
  {
    stringstream oss;
#ifdef NOTNOW
    oss << obs_time <<","<< rec_time <<","<< vehicle_id <<","<< point.get_latitude() <<","<< point.get_longitude() <<","<< point.get_elevation() <<","<< heading << ","<< speed <<","<< brake_status<<","<< brake_boost<<","<< abs<<","<< stab<<","<< trac<<","<< lights<<","<< yaw<<","<< hoz_accel_long<<","<< hoz_accel_lat<<","<< steering_angle<<","<< steering_rate<<","<< wipers_front_status<<","<< air_temp<<","<< bar_press<<","<< air_temp2<<","<< dew_temp<<","<< surface_temp;
#endif
    return oss.str();
  }

  /** @brief short generic message header used for testing */
  static string short_generic_message_hdr()
  {
    return "obs_time,rec_time,vehicle_id,latitude,longitude,elevation,heading,speed,air_temp,bar_press";
  }

  /** @brief short generic message string used for testing */
  string short_generic_message_string()
  {
    stringstream oss;
#ifdef NOTNOW
    oss << obs_time <<","<< rec_time <<","<< vehicle_id <<","<< point.get_latitude() <<","<< point.get_longitude() <<","<< point.get_elevation() <<","<< heading << ","<< speed <<","<< air_temp<<","<< bar_press;
#endif
    return oss.str();
  }


private:
  void init();
};


#endif /* VDT_GENERIC_MESSAGE_HH */
