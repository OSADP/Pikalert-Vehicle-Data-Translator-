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

#include <iostream>
#include <string>
#include <sstream>
#include <vdt4/vdt_point.hh>
#include <vdt4/vdt_road_segment.hh>

using std::string;
using std::stringstream;

//Namespace to prevent name collisions with similar class in VDT 
namespace rwx
{

/**
 * @class vdt_probe_message
 * Represents the standard probe message
 */
class vdt_probe_message
{
public:
  /**
   * Observation time for the probe message in UNIX seconds
   * It is calculated from full_pos_utc_time_year, ...,
   * full_pos_utc_time_second.
   */
  double obs_time;

  /**
    * Reception time for probe message in UNIX seconds.
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

  /** @brief vehicle heading value */
  float heading;

  /** @brief vehicle speed value */
  float speed;

  /**
   * A combination of different bits being set:
   *
   *-- B&apos;0000  &quot;All Off&quot;
   *-- bit0 (0) -- B&apos;1000  Right Rear Active
   *-- bit1 (1) -- B&apos;0100  Right Front Active
   *-- bit2 (2) -- B&apos;0010  Left Rear Active
   *-- bit3 (3) -- B&apos;0001  Left Front Active
   *-- B&apos;1111  &quot;All On&quot;
   */
  short brake_status;

  /**
   * The value from the brake.boost element:
   * 0 - notEquipped
   * 1 - turned OFF
   * 2 - turned ON
   */
  short brake_boost;

  /** The value from the anti-lock breaking element:
   *  0 - notEquipped
   *  1 - turned OFF
   *  2 - turned ON
   *  3 - Active/Engaged
   */
  short abs;

  /**The value from the stability control element:
   *0 - notEquipped
   *1 - turned OFF
   *2 - turned ON
   *3 - Active/Engaged
   */
  short stab;

  /**The value from the traction control element:
   *0 - notEquipped
   *1 - turned OFF
   *2 - turned ON
   *3 - Active/Engaged
   */
  short trac;

  /**
   * A combination of the follwoing bit settings:
   *
   *bit0 (0) -- B&apos;1000-0000  parkingLightsOn
   *bit1 (1) -- B&apos;0100-0000  fogLightOn
   *bit2 (2) -- B&apos;0010-0000  daytimeRunningLightsOn
   *bit3 (3) -- B&apos;0001-0000  automaticLightControlOn
   *bit4 (4) -- B&apos;0000-1000  rightTurnSignalOn
   *bit5 (5) -- B&apos;0000-0100  leftTurnSignalOn
   *bit6 (6) -- B&apos;0000-0010  highBeamHeadlightsOn
   *bit7 (7) -- B&apos;0000-0001  lowBeamHeadlightsOn
   *-- B&apos;0001-1000  hazardSignalOn
   *-- B&apos;0000-0000  allLightsOff
   */
  short lights;

  /**
   * The value from the yaw element multiplied by 0.01 in degrees/second
   */
  float yaw;

  /**
   * The value from the horizontal longitudinal acceleration element multiplied by 0.01 in m/s^2
   */
  float hoz_accel_long;

  /**
   * The value from the horizontal lateral acceleration element multiplied by 0.01 in m/s^2
   */
  float hoz_accel_lat;

  /**
   * The value from the steering angle element multiplied by 0.02
   * in degrees.
   */
  float steering_angle;

  /**
    * The value from the steering rate element multiplied by 3
    * in degree/second
    */
  short steering_rate;

  /**
    * The value from front wipers status:
    * notEquipped (0)
    * off (1)
    * intermittent (2)
    * low (3)
    * high (4)
    * washer (5)
    * automaticPresent (255) -- Auto wipper equipped
    */
  short wipers_front_status;

  /**
   * The value from the air temperature element minus 40 in Celsius.
   */
  short air_temp;

  /**
   * The value from the barometric pressure element multiplied by 2 plus 580 in hPa:
   * Vehicle's Environmental Barometric Pressure
   * hPa=INT*2+580
   * Range of 580 to 1090 hPa
   */
  short bar_press;

  /**
   * The value from the second air temperature element minus 40 in Celsius.
   */
  float air_temp2;

  /** @brief dew temperature */
  float dew_temp;

  /** @brief surface temperature */
  float surface_temp;

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

  /** @brief constructor */
  vdt_probe_message();

  virtual ~vdt_probe_message();

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

  /**
   * @brief  get heading value
   * @return heading value
   */
  inline const float get_heading() const
  {
    return heading;
  }

  /**
   * @brief  get speed value
   * @return speed value
   */
  inline const float get_speed() const
  {
    return speed;
  }

  /**
   * @brief  get brake status value
   * @return brake status value
   */
  inline const short get_brake_status() const
  {
    return brake_status;
  }

  /**
   * @brief  get yaw rate value
   * @return yaw rate value
   */
  inline const float get_yaw() const
  {
    return yaw;
  }

  /**
   * @brief  get horizontal longitudinal acceleration value
   * @return horizontal longitudinal acceleration value
   */
  inline const float get_hoz_accel_long() const
  {
    return hoz_accel_long;
  }

  /**
   * @brief  get horizontal lateral acceleration value
   * @return horizontal lateral acceleration value
   */
  inline const float get_hoz_accel_lat() const
  {
    return hoz_accel_lat;
  }

  /**
   * @brief  get steering angle value
   * @return steering angle value
   */
  inline const float get_steering_angle() const
  {
    return steering_angle;
  }

  /**
   * @brief  get steering rate value
   * @return steering rate value
   */
  inline const short get_steering_rate() const
  {
    return steering_rate;
  }

  /**
   * @brief  get front wiper status value
   * @return front wiper status value
   */
  inline const short get_wipers_front_status() const
  {
    return wipers_front_status;
  }

  /**
   * @brief  get air temperature value
   * @return air temperature value
   */
  inline const short get_air_temp() const
  {
    return air_temp;
  }

  /**
   * @brief  get air temperature 2 value
   * @return air temperature 2 value
   */
  inline const float get_air_temp2() const
  {
    return air_temp2;
  }

  /**
   * @brief  get dew temperature value
   * @return dew temperature value
   */
  inline const float get_dew_temp() const
  {
    return dew_temp;
  }

  /**
   * @brief  get barometric pressure value
   * @return barometric pressure value
   */
  inline const short get_bar_press() const
  {
    return bar_press;
  }

  /**
   * @brief  get traction value
   * @return traction value
   */
  inline const short get_trac() const
  {
    return trac;
  }

  /**
   * @brief  get stability value
   * @return stability value
   */
  inline const short get_stab() const
  {
    return stab;
  }

  /**
   * @brief  get lights value
   * @return lights value
   */
  inline const short get_lights() const
  {
    return lights;
  }

  /**
   * @brief  get anti lock brake status value
   * @return anti lock brake status value
   */
  inline const short get_abs() const
  {
    return abs;
  }

  /**
   * @brief  get brake boost status value
   * @return brake boost status value
   */
  inline const short get_brake_boost() const
  {
    return brake_boost;
  }

  /**
   * @brief  get surface temperature value
   * @return surface temperature value
   */
  inline const float get_surface_temp() const
  {
    return surface_temp;
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

  /** @brief probe message header used for testing */
  static string probe_message_hdr()
  {
    return "obs_time,rec_time,vehicle_id,latitude,longitude,elevation,heading,speed,brake_status,brake_boost,abs,stab,trac,lights,yaw,hoz_accel_long,hoz_accel_lat,steering_angle,steering_rate,wipers_front_status,air_temp,bar_press,air_temp2,dew_temp,surface_temp";
  }

  /** @brief probe message string used for testing */
  string probe_message_string()
  {
    stringstream oss;
    oss << obs_time <<","<< rec_time <<","<< vehicle_id <<","<< point.get_latitude() <<","<< point.get_longitude() <<","<< point.get_elevation() <<","<< heading << ","<< speed <<","<< brake_status<<","<< brake_boost<<","<< abs<<","<< stab<<","<< trac<<","<< lights<<","<< yaw<<","<< hoz_accel_long<<","<< hoz_accel_lat<<","<< steering_angle<<","<< steering_rate<<","<< wipers_front_status<<","<< air_temp<<","<< bar_press<<","<< air_temp2<<","<< dew_temp<<","<< surface_temp;
    return oss.str();
  }

  /** @brief short probe message header used for testing */
  static string short_probe_message_hdr()
  {
    return "obs_time,rec_time,vehicle_id,latitude,longitude,elevation,heading,speed,air_temp,bar_press";
  }

  /** @brief short probe message string used for testing */
  string short_probe_message_string()
  {
    stringstream oss;
    oss << obs_time <<","<< rec_time <<","<< vehicle_id <<","<< point.get_latitude() <<","<< point.get_longitude() <<","<< point.get_elevation() <<","<< heading << ","<< speed <<","<< air_temp<<","<< bar_press;
    return oss.str();
  }


private:
  void init();
};

} //END namespace RWX
#endif /* VDT_PROBE_MESSAGE_HH */
