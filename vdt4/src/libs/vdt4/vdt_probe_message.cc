// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
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
  obs_time = vdt_const::FILL_VALUE;
  rec_time = vdt_const::FILL_VALUE;
  vehicle_id = "";
  point.set_longitude(vdt_const::FILL_VALUE);
  point.set_latitude(vdt_const::FILL_VALUE);
  point.set_elevation(vdt_const::FILL_VALUE);
  heading = vdt_const::FILL_VALUE;
  speed = vdt_const::FILL_VALUE;
  brake_status = vdt_const::SHORT_FILL_VALUE;
  brake_boost = vdt_const::SHORT_FILL_VALUE;
  abs = vdt_const::SHORT_FILL_VALUE;
  stab = vdt_const::SHORT_FILL_VALUE;
  trac = vdt_const::SHORT_FILL_VALUE;
  lights = vdt_const::SHORT_FILL_VALUE;
  wipers_front_status = vdt_const::SHORT_FILL_VALUE;
  yaw = vdt_const::FILL_VALUE;
  hoz_accel_long = vdt_const::FILL_VALUE;
  hoz_accel_lat = vdt_const::FILL_VALUE;
  steering_angle = vdt_const::FILL_VALUE;
  steering_rate = vdt_const::SHORT_FILL_VALUE;
  air_temp = vdt_const::SHORT_FILL_VALUE;
  bar_press = vdt_const::SHORT_FILL_VALUE;
  air_temp2 = vdt_const::FILL_VALUE;
  dew_temp = vdt_const::FILL_VALUE;
  surface_temp = vdt_const::FILL_VALUE;
}

vdt_probe_message::vdt_probe_message()
{
  init();
}

vdt_probe_message::~vdt_probe_message() {}


void vdt_probe_message::set_obs_time(double value)
{
  obs_time = value;
}

void vdt_probe_message::set_rec_time(double value)
{
  rec_time = value;
}

void vdt_probe_message::set_vehicle_id(string value)
{
  vehicle_id = value;
}

void vdt_probe_message::set_point(vdt_point value)
{
  point = value;
}

void vdt_probe_message::set_speed(float value)
{
  speed = value;
}

void vdt_probe_message::set_heading(float value)
{
  heading = value;
}

void vdt_probe_message::set_brake_status(short value)
{
  brake_status = value;
}

void vdt_probe_message::set_yaw(float value)
{
  yaw = value;
}

void vdt_probe_message::set_yaw_rate(float value)
{
  yaw = value;
}

void vdt_probe_message::set_hoz_accel_long(float value)
{
  hoz_accel_long = value;
}

void vdt_probe_message::set_hoz_accel_lat(float value)
{
  hoz_accel_lat = value;
}

void vdt_probe_message::set_steering_angle(float value)
{
  steering_angle = value;
}

void vdt_probe_message::set_steering_rate(short value)
{
  steering_rate = value;
}

void vdt_probe_message::set_wipers_front_status(short value)
{
  wipers_front_status = value;
}

void vdt_probe_message::set_air_temp(short value)
{
  air_temp = value;
}

void vdt_probe_message::set_air_temp2(float value)
{
  air_temp2 = value;
}

void vdt_probe_message::set_dew_temp(float value)
{
  dew_temp = value;
}

void vdt_probe_message::set_surface_temp(float value)
{
  surface_temp = value;
}

void vdt_probe_message::set_bar_press(short value)
{
  bar_press = value;
}

void vdt_probe_message::set_trac(short value)
{
  trac = value;
}

void vdt_probe_message::set_stab(short value)
{
  stab = value;
}

void vdt_probe_message::set_lights(short value)
{
  lights = value;
}

void vdt_probe_message::set_abs(short value)
{
  abs = value;
}

void vdt_probe_message::set_brake_boost(short value)
{
  brake_boost = value;
}



