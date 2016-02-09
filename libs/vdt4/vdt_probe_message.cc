#include <string>
#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>

#include "vdt_probe_message.hh"
#include "vdt_const.hh"
#include "stat_tools.hh"

using namespace std;

const short vdt_probe_message::BAD_OR_MISSING_VALUE = -9999;

const string vdt_probe_message::unknown_str         = "unknown";
const string vdt_probe_message::notequipped_str     = "notequipped";
const string vdt_probe_message::off_str             = "off";
const string vdt_probe_message::on_str              = "on";
const string vdt_probe_message::engaged_str         = "engaged";
const string vdt_probe_message::intermittent_str    = "intermittent";
const string vdt_probe_message::low_str             = "low";
const string vdt_probe_message::high_str            = "high";
const string vdt_probe_message::washer_str          = "washer";
const string vdt_probe_message::automaticpresent_str= "automaticPresent";

const string vdt_probe_message::parkingLights_str 		  = "parking";
const string vdt_probe_message::fogLight_str 			  = "fog";
const string vdt_probe_message::daytimeRunningLights_str  = "daytimeRunning";
const string vdt_probe_message::automaticLightControl_str = "automatic";
const string vdt_probe_message::rightTurnSignal_str 	  = "right";
const string vdt_probe_message::leftTurnSignal_str        = "left";
const string vdt_probe_message::highBeamHeadlights_str    = "high";
const string vdt_probe_message::lowBeamHeadlights_str     = "low";
const string vdt_probe_message::hazardSignal_str          = "hazard";
const string vdt_probe_message::allOff_str                = "off";

const short vdt_probe_message::NOT_EQUIPPED         = 0;
const short vdt_probe_message::OFF                  = 1;
const short vdt_probe_message::ON                   = 2;
const short vdt_probe_message::ENGAGED              = 3;
const short vdt_probe_message::INTERMITTENT         = 2;
const short vdt_probe_message::LOW                  = 3;
const short vdt_probe_message::HIGH                 = 4;
const short vdt_probe_message::WASHER               = 5;
const short vdt_probe_message::AUTOMATICPRESENT     = 255;

const short vdt_probe_message::ABS_STATUS_NOTEQUIPPED      = 0;
const short vdt_probe_message::ABS_STATUS_OFF              = 1;
const short vdt_probe_message::ABS_STATUS_ON               = 2;
const short vdt_probe_message::ABS_STATUS_ENGAGED          = 3;

const short vdt_probe_message::BRAKE_STATUS_ALL_OFF   = 0;
const short vdt_probe_message::BRAKE_STATUS_RR_ACTIVE = 8;  //1000
const short vdt_probe_message::BRAKE_STATUS_RF_ACTIVE = 4;  //0100
const short vdt_probe_message::BRAKE_STATUS_LR_ACTIVE = 2;  //0010
const short vdt_probe_message::BRAKE_STATUS_LF_ACTIVE = 1;  //0001
const short vdt_probe_message::BRAKE_STATUS_ALL_ON    = 15; //1111

const short vdt_probe_message::STAB_STATUS_NOTEQUIPPED      = 0;
const short vdt_probe_message::STAB_STATUS_OFF              = 1;
const short vdt_probe_message::STAB_STATUS_ON               = 2;
const short vdt_probe_message::STAB_STATUS_ENGAGED          = 3;

const short vdt_probe_message::TRAC_STATUS_NOTEQUIPPED      = 0;
const short vdt_probe_message::TRAC_STATUS_OFF              = 1;
const short vdt_probe_message::TRAC_STATUS_ON               = 2;
const short vdt_probe_message::TRAC_STATUS_ENGAGED          = 3;

const short vdt_probe_message::WIPER_STATUS_NOTEQUIPPED      = 0;
const short vdt_probe_message::WIPER_STATUS_OFF              = 1;
const short vdt_probe_message::WIPER_STATUS_INTERMITTENT     = 2;
const short vdt_probe_message::WIPER_STATUS_LOW              = 3;
const short vdt_probe_message::WIPER_STATUS_HIGH             = 4;
const short vdt_probe_message::WIPER_STATUS_WASHER           = 5;
const short vdt_probe_message::WIPER_STATUS_AUTOMATICPRESENT = 255;

const short vdt_probe_message::LIGHTS_PARKING_LIGHTS_ON         = 128;
const short vdt_probe_message::LIGHTS_FOG_LIGHTS_ON              = 64;
const short vdt_probe_message::LIGHTS_DAYTIME_RUNNING_LIGHTS_ON  = 32;
const short vdt_probe_message::LIGHTS_AUTOMATIC_LIGHT_CONTROL_ON = 16;
const short vdt_probe_message::LIGHTS_RIGHT_TURN_SIGNAL_ON       = 8;
const short vdt_probe_message::LIGHTS_LEFT_TURN_SIGNAL_ON        = 4;
const short vdt_probe_message::LIGHTS_HIGH_BEAM_HEADLIGHTS_ON    = 2;
const short vdt_probe_message::LIGHTS_LOW_BEAM_HEADLIGHTS_ON     = 1;
const short vdt_probe_message::LIGHTS_HAZARD_SIGNAL_ON          = 24;
const short vdt_probe_message::LIGHTS_ALL_LIGHTS_OFF            = 0;

void vdt_probe_message::init()
{
  setattr("obs_time", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("rec_time", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("vehicle_id", "");
  setattr("longitude", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("latitude", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("elevation", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("heading", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("speed", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("brake_status", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("brake_boost", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("abs", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("stab", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("trac", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("lights", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("wiper_status", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("yaw_rate", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_long", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("hoz_accel_lat", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_angle", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("steering_rate", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("air_temp", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("bar_pressure", static_cast<int>(vdt_const::SHORT_FILL_VALUE));
  setattr("air_temp2", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("dew_temp", static_cast<double>(vdt_const::FILL_VALUE));
  setattr("surface_temp", static_cast<double>(vdt_const::FILL_VALUE));
}

void vdt_probe_message::init_fields(const FIELD_NAME_TYPE_VEC& fields)
{
  for(const auto& elem : fields)
  {
    if(elem.second == INT_TYPE && find(elem.first) == end())
    {
      //std::cout << "init_fields: " << elem.first << std::endl;
      setattr(elem.first, static_cast<int>(vdt_const::SHORT_FILL_VALUE));
    }
    else if(elem.second == DOUBLE_TYPE && find(elem.first) == end())
    {
      //std::cout << "init_fields: " << elem.first << std::endl;
      setattr(elem.first, static_cast<double>(vdt_const::FILL_VALUE));
    }
  }
}

vdt_probe_message::vdt_probe_message(const FIELD_NAME_TYPE_VEC& fields)
{
  init();
  init_fields(fields);
}

void vdt_probe_message::set_obs_time(double value)
{
  setattr("obs_time", value);
}

void vdt_probe_message::set_rec_time(double value)
{
  setattr("rec_time", value);
}

void vdt_probe_message::set_vehicle_id(string value)
{
  setattr("vehicle_id", value);
}

void vdt_probe_message::set_point(vdt_point value)
{
  setattr("longitude", value.get_longitude());
  setattr("latitude", value.get_latitude());
  setattr("elevation", value.get_elevation());
}

void vdt_probe_message::set_speed(float value)
{
  setattr("speed", static_cast<double>(value));
}

void vdt_probe_message::set_heading(float value)
{
  setattr("heading", static_cast<double>(value));
}

void vdt_probe_message::set_brake_status(short value)
{
  setattr("brake_status", static_cast<int>(value));
}

void vdt_probe_message::set_yaw(float value)
{
  setattr("yaw_rate", static_cast<double>(value));
}

void vdt_probe_message::set_yaw_rate(float value)
{
  setattr("yaw_rate", static_cast<double>(value));
}

void vdt_probe_message::set_hoz_accel_long(float value)
{
  setattr("hoz_accel_long", static_cast<double>(value));
}

void vdt_probe_message::set_hoz_accel_lat(float value)
{
  setattr("hoz_accel_lat", static_cast<double>(value));
}

void vdt_probe_message::set_steering_angle(float value)
{
  setattr("steering_angle", static_cast<double>(value));
}

void vdt_probe_message::set_steering_rate(short value)
{
  setattr("steering_rate", static_cast<int>(value));
}

void vdt_probe_message::set_wipers_front_status(short value)
{
  setattr("wiper_status", static_cast<int>(value));
}

void vdt_probe_message::set_air_temp(short value)
{
  setattr("air_temp", static_cast<int>(value));
}

void vdt_probe_message::set_air_temp2(float value)
{
  setattr("air_temp2", static_cast<double>(value));
}

void vdt_probe_message::set_dew_temp(float value)
{
  setattr("dew_temp", static_cast<double>(value));
}

void vdt_probe_message::set_surface_temp(float value)
{
  setattr("surface_temp", static_cast<double>(value));
}

void vdt_probe_message::set_bar_press(short value)
{
  setattr("bar_pressure", static_cast<int>(value));
}

void vdt_probe_message::set_trac(short value)
{
  setattr("trac", static_cast<int>(value));
}

void vdt_probe_message::set_stab(short value)
{
  setattr("stab", static_cast<int>(value));
}

void vdt_probe_message::set_lights(short value)
{
  setattr("lights", static_cast<int>(value));
}

void vdt_probe_message::set_abs(short value)
{
  setattr("abs", static_cast<int>(value));
}

void vdt_probe_message::set_brake_boost(short value)
{
  setattr("brake_boost", static_cast<int>(value));
}


