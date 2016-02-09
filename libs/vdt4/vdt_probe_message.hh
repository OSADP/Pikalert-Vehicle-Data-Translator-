//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/03/28 21:25:34 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message.hh
 *
 * @class vdt_probe_message
 *
 *  vdt probe message class
 *
 */

#ifndef VDT_PROBE_MESSAGE_HH
#define VDT_PROBE_MESSAGE_HH

#include <string>

#include "vdt_point.hh"
#include "vdt_road_segment.hh"
#include "vdt_exception.hh"
#include "pika_message.hh"

/**
 * @class vdt_probe_message
 * Represents the standard probe message
 */
class vdt_probe_message : public pika_message
{
public:
  /** @brief bad or missing value netcdf name */
  static const short BAD_OR_MISSING_VALUE;  // = -9999;

  /** @brief unknown value netcdf name */
  static const string unknown_str;   		  // = "unknown";

  /** @brief not equipped netcdf attribute name */
  static const string notequipped_str;      // = "notequipped";

  /** @brief off netcdf atrritube name */
  static const string off_str;              // = "off";

  /** @brief on netcdf attribute name */
  static const string on_str;               // = "on";

  /** @brief engaged netcdf attribute name */
  static const string engaged_str;          // = "engaged";

  /** @brief intermittent netcdf attribut name */
  static const string intermittent_str;     // = "intermittent";

  /** @brief low netcdf attribute name */
  static const string low_str;              // = "low";

  /** @brief high netcdf attribute name */
  static const string high_str;             // = "high";

  /** @brief washer netcdf attribute name */
  static const string washer_str;           // = "washer";

  /** @brief automatic present netcdf attribute name */
  static const string automaticpresent_str; // = "automaticPresent";

  /** @brief parking lights netcdf attribute name */
  static const string parkingLights_str;        //  = "parkingLight";

  /** @brief fog light netcdf attribute name */
  static const string fogLight_str;             //  = "fogLight";

  /** @brief daytime running lights netcdf attribute name */
  static const string daytimeRunningLights_str; //  = "daytimeRunningLights";

  /** @brief automatic light control netcdf attribute name */
  static const string automaticLightControl_str;//  = "automaticLightControl";

  /** @brief right turn signal netcdf attribute name */
  static const string rightTurnSignal_str; 	  //  = "rightTurnSignal";

  /** @brief left turn signal netcdf attribute name */
  static const string leftTurnSignal_str;       //  = "leftTurnSignal";

  /** @brief high beam headlights netcdf attribute name */
  static const string highBeamHeadlights_str;   //  = "highBeamHeadlights";

  /** @brief low beam headlights netcdf attribute name */
  static const string lowBeamHeadlights_str;    //  = "lowBeamHeadlights";

  /** @brief hazard signal netcdf attribute name */
  static const string hazardSignal_str;         //  = "hazardSignal";

  /** @brief all off netcdf attribute name */
  static const string allOff_str;               //  = "off";

  /** @brief not equipped value */
  static const short NOT_EQUIPPED;          // = 0;

  /** @brief off value */
  static const short OFF;                   // = 1;

  /** @brief on value */
  static const short ON ;                   // = 2;

  /** @brief engaged value */
  static const short ENGAGED;               // = 3;

  /** @brief intermittent value */
  static const short INTERMITTENT;          // = 2;

  /** @brief low value */
  static const short LOW;                   // = 3;

  /** @brief high value */
  static const short HIGH;                  // = 4;

  /** @brief washer value */
  static const short WASHER;                // = 5;

  /** @brief automatic present value */
  static const short AUTOMATICPRESENT;      // = 255;


  /** @brief brakes all off  status value */
  static const short BRAKE_STATUS_ALL_OFF;

  /** @brief brakes right rear active status value */
  static const short BRAKE_STATUS_RR_ACTIVE;

  /** @brief brakes right front active status value */
  static const short BRAKE_STATUS_RF_ACTIVE;

  /** @brief brakes left rear active status value */
  static const short BRAKE_STATUS_LR_ACTIVE;

  /** @brief brakes left front active status value */
  static const short BRAKE_STATUS_LF_ACTIVE;

  /** @brief brakes all on  status value */
  static const short BRAKE_STATUS_ALL_ON;


  /** @brief not equipped wiper status value */
  static const short WIPER_STATUS_NOTEQUIPPED;

  /** @brief off wiper status value */
  static const short WIPER_STATUS_OFF;

  /** @brief intermittent wiper status value */
  static const short WIPER_STATUS_INTERMITTENT;

  /** @brief low wiper status value */
  static const short WIPER_STATUS_LOW;

  /** @brief high wiper status value */
  static const short WIPER_STATUS_HIGH;

  /** @brief washer wiper status value */
  static const short WIPER_STATUS_WASHER;

  /** @brief automatic present wiper status value */
  static const short WIPER_STATUS_AUTOMATICPRESENT;


  /** @brief traction not equipped status value */
  static const short TRAC_STATUS_NOTEQUIPPED;

  /** @brief traction off status value */
  static const short TRAC_STATUS_OFF;

  /** @brief traction on status value */
  static const short TRAC_STATUS_ON;

  /** @brief traction engaged status value */
  static const short TRAC_STATUS_ENGAGED;

  /** @brief stability not equipped status value */
  static const short STAB_STATUS_NOTEQUIPPED;

  /** @brief stability off status value */
  static const short STAB_STATUS_OFF;

  /** @brief stability on status value */
  static const short STAB_STATUS_ON;

  /** @brief stability engaged status value */
  static const short STAB_STATUS_ENGAGED;


  /** @brief anti lock brake not equipped status value */
  static const short ABS_STATUS_NOTEQUIPPED;

  /** @brief anti lock brake off status value */
  static const short ABS_STATUS_OFF;

  /** @brief anti lock brake on status value */
  static const short ABS_STATUS_ON;

  /** @brief anti lock brake engaged status value */
  static const short ABS_STATUS_ENGAGED;

  /** @brief parking lights on value */
  static const short LIGHTS_PARKING_LIGHTS_ON;

  /** @brief fog lights on value */
  static const short LIGHTS_FOG_LIGHTS_ON;

  /** @brief daytime running lights on value */
  static const short LIGHTS_DAYTIME_RUNNING_LIGHTS_ON;

  /** @brief automatic light control on value */
  static const short LIGHTS_AUTOMATIC_LIGHT_CONTROL_ON;

  /** @brief right turn signal on value */
  static const short LIGHTS_RIGHT_TURN_SIGNAL_ON;

  /** @brief left turn signal on value */
  static const short LIGHTS_LEFT_TURN_SIGNAL_ON;

  /** @brief high beam headlights on value */
  static const short LIGHTS_HIGH_BEAM_HEADLIGHTS_ON;

  /** @brief low beam headlights on value */
  static const short LIGHTS_LOW_BEAM_HEADLIGHTS_ON;

  /** @brief hazard signal on value */
  static const short LIGHTS_HAZARD_SIGNAL_ON;

  /** @brief all lights offvalue */
  static const short LIGHTS_ALL_LIGHTS_OFF;

private:
  void init_fields(const FIELD_NAME_TYPE_VEC& fields);
public:
  /** @brief constructor */
  vdt_probe_message(const FIELD_NAME_TYPE_VEC& fields);

  vdt_probe_message()
  {
    init();
  }

  /**
   * @brief  get observation time  value
   * @return observation time value
   */
  inline const double get_obs_time() const
  {
    return getattr<double>("obs_time");
  }

  /**
   * @brief  get  received time value
   * @return received time value
   */
  inline const double get_rec_time() const
  {
    return getattr<double>("rec_time");
  }

  /**
   * @brief  get vehicle id  value
   * @return vehicle id value
   */
  inline const string get_vehicle_id() const
  {
    return getattr<std::string>("vehicle_id");
  }

  /**
   * @brief  get elevation value
   * @return elevation value
   */
  const float get_elevation()
  {
    return static_cast<float>(getattr<double>("elevation"));
  }

  /**
   * @brief  get latitude value
   * @return latitude value
   */
  const double get_latitude()
  {
    return getattr<double>("latitude");
  }

  /**
   * @brief  get longitude value
   * @return longitude value
   */
  const double get_longitude()
  {
    return getattr<double>("longitude");
  }

  /**
   * @brief  get vdt point value (latitude, longitude, elevation)
   * @return vdt point value
   */
  inline const vdt_point get_point() const
  {
    return vdt_point(getattr<double>("latitude"), getattr<double>("longitude"), getattr<double>("elevation"));
  }

  /**
   * @brief  get heading value
   * @return heading value
   */
  inline const float get_heading() const
  {
    return static_cast<float>(getattr<double>("heading"));
  }

  /**
   * @brief  get speed value
   * @return speed value
   */
  inline const float get_speed() const
  {
    return static_cast<float>(getattr<double>("speed"));
  }

  /**
   * @brief  get brake status value
   * @return brake status value
   */
  inline const short get_brake_status() const
  {
    return static_cast<short>(getattr<int>("brake_status"));
  }

  /**
   * @brief  get yaw rate value
   * @return yaw rate value
   */
  inline const float get_yaw() const
  {
    return static_cast<float>(getattr<double>("yaw_rate"));
  }

  /**
   * @brief  get horizontal longitudinal acceleration value
   * @return horizontal longitudinal acceleration value
   */
  inline const float get_hoz_accel_long() const
  {
    return static_cast<float>(getattr<double>("hoz_accel_long"));
  }

  /**
   * @brief  get horizontal lateral acceleration value
   * @return horizontal lateral acceleration value
   */
  inline const float get_hoz_accel_lat() const
  {
    return static_cast<float>(getattr<double>("hoz_accel_lat"));
  }

  /**
   * @brief  get steering angle value
   * @return steering angle value
   */
  inline const float get_steering_angle() const
  {
    return static_cast<float>(getattr<double>("steering_angle"));
  }

  /**
   * @brief  get steering rate value
   * @return steering rate value
   */
  inline const short get_steering_rate() const
  {
    return static_cast<short>(getattr<int>("steering_rate"));
  }

  /**
   * @brief  get front wiper status value
   * @return front wiper status value
   */
  inline const short get_wipers_front_status() const
  {
    return static_cast<short>(getattr<int>("wiper_status"));
  }

  /**
   * @brief  get air temperature value
   * @return air temperature value
   */
  inline const short get_air_temp() const
  {
    return static_cast<short>(getattr<int>("air_temp"));
  }

  /**
   * @brief  get air temperature 2 value
   * @return air temperature 2 value
   */
  inline const float get_air_temp2() const
  {
    return static_cast<float>(getattr<double>("air_temp2"));
  }

  /**
   * @brief  get dew temperature value
   * @return dew temperature value
   */
  inline const float get_dew_temp() const
  {
    return static_cast<float>(getattr<double>("dew_temp"));
  }

  /**
   * @brief  get barometric pressure value
   * @return barometric pressure value
   */
  inline const short get_bar_press() const
  {
    return static_cast<short>(getattr<int>("bar_pressure"));
  }

  /**
   * @brief  get traction value
   * @return traction value
   */
  inline const short get_trac() const
  {
    return static_cast<short>(getattr<int>("trac"));
  }

  /**
   * @brief  get stability value
   * @return stability value
   */
  inline const short get_stab() const
  {
    return static_cast<short>(getattr<int>("stab"));
  }

  /**
   * @brief  get lights value
   * @return lights value
   */
  inline const short get_lights() const
  {
    return static_cast<short>(getattr<int>("lights"));
  }

  /**
   * @brief  get anti lock brake status value
   * @return anti lock brake status value
   */
  inline const short get_abs() const
  {
    return static_cast<short>(getattr<int>("abs"));
  }

  /**
   * @brief  get brake boost status value
   * @return brake boost status value
   */
  inline const short get_brake_boost() const
  {
    return static_cast<short>(getattr<int>("brake_boost"));
  }

  /**
   * @brief  get surface temperature value
   * @return surface temperature value
   */
  inline const float get_surface_temp() const
  {
    return static_cast<float>(getattr<double>("surface_temp"));
  }

  /**
   * @brief set obs time value
   * @param[in] value  obs time value
   */
  void set_obs_time(double value);

  /**
   * @brief set received time value
   * @param[in] value  received time value
   */
  void set_rec_time(double value);

  /**
   * @brief set vehicle id value
   * @param[in] value  vehicle id value
   */
  void set_vehicle_id(string value);

  /**
   * @brief set vdt_point value (latitude, longitude,elevation)
   * @param[in] value  vdt_point value
   */
  void set_point(vdt_point value);

  /**
   * @brief set speed value
   * @param[in] value  speed value
   */
  void set_speed(float value);

  /**
   * @brief set heading value
   * @param[in] value  heading value
   */
  void set_heading(float value);

  /**
   * @brief set yaw rate value
   * @param[in] value  yaw rate value
   */
  void set_yaw_rate(float value);

  /**
   * @brief set brake status value
   * @param[in] value  brake status value
   */
  void set_brake_status(short value);

  /**
   * @brief set yaw value
   * @param[in] value  yaw value
   */
  void set_yaw(float value);

  /**
   * @brief set horizontal longitudinal acceleration value
   * @param[in] value  horizontal longitudinal acceleration value
   */
  void set_hoz_accel_long(float value);

  /**
   * @brief set horizontal lateral acceleration value
   * @param[in] value  horizontal lateral acceleration value
   */
  void set_hoz_accel_lat(float value);

  /**
   * @brief set steering angle value
   * @param[in] value  steering angle value
   */
  void set_steering_angle(float value);

  /**
   * @brief set steering rate value
   * @param[in] value  steering rate value
   */
  void set_steering_rate(short value);

  /**
   * @brief set front wiper status value
   * @param[in] value  front wiper status value
   */
  void set_wipers_front_status(short value);

  /**
   * @brief set air temperature value
   * @param[in] value  air temperature value
   */
  void set_air_temp(short value);

  /**
   * @brief set air temperature 2 value
   * @param[in] value  air temperature 2 value
   */
  void set_air_temp2(float value);

  /**
   * @brief set surface temeperature value
   * @param[in] value  surface temperature value
   */
  void set_surface_temp(float value);

  /**
   * @brief set dew temperature value
   * @param[in] value dew temperature value
   */
  void set_dew_temp(float value);

  /**
   * @brief set barometric pressue value
   * @param[in] value  barometric pressure value
   */
  void set_bar_press(short value);

  /**
   * @brief set traction value
   * @param[in] value  traction value
   */
  void set_trac(short value);

  /**
   * @brief set stability value
   * @param[in] value  stability value
   */
  void set_stab(short value);

  /**
   * @brief set lights value
   * @param[in] value  lights value
   */
  void set_lights(short value);

  /**
   * @brief set anti lock brake status value
   * @param[in] value  anti lock brake status value
   */
  void set_abs(short value);

  /**
   * @brief set brake boost status value
   * @param[in] value brake boost status value
   */
  void set_brake_boost(short value);

  std::string probe_message_hdr() const
  {
    return message_hdr();
  }

  std::string probe_message_string() const
  {
    return message_string();
  }
private:
  void init();
};


#endif /* VDT_PROBE_MESSAGE_HH */
