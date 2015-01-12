// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_probe_message_file_reader.hh"

const char* vdt_probe_message_file_reader::REC_NUM = "rec_num";
const char* vdt_probe_message_file_reader::VEHICLE_ID_LEN = "vehicle_id_len";
const char* vdt_probe_message_file_reader::LATITUDE = "latitude";
const char* vdt_probe_message_file_reader::LONGITUDE = "longitude";
const char* vdt_probe_message_file_reader::ELEVATION = "elevation";
const char* vdt_probe_message_file_reader::OBS_TIME = "obs_time";
const char* vdt_probe_message_file_reader::REC_TIME = "rec_time";
const char* vdt_probe_message_file_reader::VEHICLE_ID = "vehicle_id";
const char* vdt_probe_message_file_reader::SPEED = "speed";
const char* vdt_probe_message_file_reader::BRAKE_STATUS = "brake_status";
const char* vdt_probe_message_file_reader::BRAKE_BOOST = "brake_boost";
const char* vdt_probe_message_file_reader::WIPER_STATUS = "wiper_status";
const char* vdt_probe_message_file_reader::AIR_TEMPERATURE = "air_temp";
const char* vdt_probe_message_file_reader::BAR_PRESSURE = "bar_pressure";
const char* vdt_probe_message_file_reader::TRACTION_CONTROL = "trac";
const char* vdt_probe_message_file_reader::STABILITY_CONTROL = "stab";
const char* vdt_probe_message_file_reader::ABS = "abs";
const char* vdt_probe_message_file_reader::LIGHTS = "lights";
const char* vdt_probe_message_file_reader::HEADING = "heading";
const char* vdt_probe_message_file_reader::YAW_RATE = "yaw_rate";
const char* vdt_probe_message_file_reader::LONGITUDINAL_ACCELERATION = "hoz_accel_long";
const char* vdt_probe_message_file_reader::LATERAL_ACCELERATION = "hoz_accel_lat";
const char* vdt_probe_message_file_reader::STEERING_ANGLE = "steering_angle";
const char* vdt_probe_message_file_reader::STEERING_RATE = "steering_rate";
const char* vdt_probe_message_file_reader::DEW_TEMPERATURE = "dew_temp";
const char* vdt_probe_message_file_reader::SURFACE_TEMPERATURE = "surface_temp";
const char* vdt_probe_message_file_reader::EXTERNAL_AIR_TEMPERATURE = "air_temp2";

vdt_probe_message_file_reader::~vdt_probe_message_file_reader()
{
}

int vdt_probe_message_file_reader::get_probe_messages(vector<vdt_probe_message>& msgs)
{
  int recnum = nc->get_dim(REC_NUM)->size();
  int vehicleidlen = nc->get_dim(VEHICLE_ID_LEN)->size();
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* elev_var = nc->get_var(ELEVATION);
  NcVar* obs_time_var = nc->get_var(OBS_TIME);
  NcVar* rec_time_var = nc->get_var(REC_TIME);
  NcVar* vehicle_id_var = nc->get_var(VEHICLE_ID);
  NcVar* speed_var = nc->get_var(SPEED);
  NcVar* brake_status_var = nc->get_var(BRAKE_STATUS);
  NcVar* brake_boost_var = nc->get_var(BRAKE_BOOST);
  NcVar* wiper_status_var = nc->get_var(WIPER_STATUS);
  NcVar* air_temp_var = nc->get_var(AIR_TEMPERATURE);
  NcVar* bar_press_var = nc->get_var(BAR_PRESSURE);
  NcVar* trac_var = nc->get_var(TRACTION_CONTROL);
  NcVar* stab_var = nc->get_var(STABILITY_CONTROL);
  NcVar* abs_var = nc->get_var(ABS);
  NcVar* lights_var = nc->get_var(LIGHTS);
  NcVar* head_var = nc->get_var(HEADING);
  NcVar* yaw_var = nc->get_var(YAW_RATE);
  NcVar* lon_accel_var = nc->get_var(LONGITUDINAL_ACCELERATION);
  NcVar* lat_accel_var = nc->get_var(LATERAL_ACCELERATION);
  NcVar* steer_angle_var = nc->get_var(STEERING_ANGLE);
  NcVar* steer_rate_var = nc->get_var(STEERING_RATE);
  NcVar* dew_var = nc->get_var(DEW_TEMPERATURE);
  NcVar* surface_var = nc->get_var(SURFACE_TEMPERATURE);
  NcVar* ext_air_temp_var = nc->get_var(EXTERNAL_AIR_TEMPERATURE);
  double lat;
  double lon;
  float elev;
  double obs_time;
  double rec_time;
  char* vehicle_id = new char[vehicleidlen];
  float speed;
  short brake_status;
  short brake_boost;
  short wiper_status;
  short air_temp;
  short bar_press;
  short trac;
  short stab;
  short abs;
  short lights;
  float heading;
  float yaw_rate;
  float lat_accel;
  float lon_accel;
  float steer_angle;
  short steer_rate;
  float ext_air_temp;
  float surface_temp;
  float dew_temp;
  for (int i = 0; i < recnum; i++)
    {
      lat_var->set_cur(i);
      lon_var->set_cur(i);
      elev_var->set_cur(i);
      obs_time_var->set_cur(i);
      rec_time_var->set_cur(i);
      vehicle_id_var->set_cur(i);
      speed_var->set_cur(i);
      brake_status_var->set_cur(i);
      brake_boost_var->set_cur(i);
      wiper_status_var->set_cur(i);
      air_temp_var->set_cur(i);
      bar_press_var->set_cur(i);
      trac_var->set_cur(i);
      stab_var->set_cur(i);
      abs_var->set_cur(i);
      lights_var->set_cur(i);
      head_var->set_cur(i);
      yaw_var->set_cur(i);
      lon_accel_var->set_cur(i);
      lat_accel_var->set_cur(i);
      steer_angle_var->set_cur(i);
      steer_rate_var->set_cur(i);
      dew_var->set_cur(i);
      surface_var->set_cur(i);
      ext_air_temp_var->set_cur(i);

      lat_var->get(&lat,1);
      lon_var->get(&lon,1);
      elev_var->get(&elev,1);
      obs_time_var->get(&obs_time,1);
      rec_time_var->get(&rec_time,1);
      vehicle_id_var->get(vehicle_id,1,vehicleidlen);
      speed_var->get(&speed,1);
      brake_boost_var->get(&brake_boost,1);
      brake_status_var->get(&brake_status,1);
      wiper_status_var->get(&wiper_status,1);
      air_temp_var->get(&air_temp,1);
      bar_press_var->get(&bar_press,1);
      trac_var->get(&trac,1);
      stab_var->get(&stab,1);
      abs_var->get(&abs,1);
      lights_var->get(&lights,1);
      head_var->get(&heading,1);
      yaw_var->get(&yaw_rate,1);
      lat_accel_var->get(&lat_accel,1);
      lon_accel_var->get(&lon_accel,1);
      steer_angle_var->get(&steer_angle,1);
      steer_rate_var->get(&steer_rate,1);
      ext_air_temp_var->get(&ext_air_temp,1);
      surface_var->get(&surface_temp,1);
      dew_var->get(&dew_temp,1);

      vdt_probe_message msg;
      vdt_point pt;
      pt.set_latitude(lat);
      pt.set_longitude(lon);
      pt.set_elevation(elev);
      msg.set_point(pt);
      msg.set_obs_time(obs_time);
      msg.set_rec_time(rec_time);
      string vid = string(vehicle_id);
      msg.set_vehicle_id(vid);
      msg.set_speed(speed);
      msg.set_brake_boost(brake_boost);
      msg.set_brake_status(brake_status);
      msg.set_wipers_front_status(wiper_status);
      msg.set_air_temp(air_temp);
      msg.set_bar_press(bar_press);
      msg.set_trac(trac);
      msg.set_stab(stab);
      msg.set_abs(abs);
      msg.set_lights(lights);
      msg.set_heading(heading);
      msg.set_yaw_rate(yaw_rate);
      msg.set_hoz_accel_long(lon_accel);
      msg.set_hoz_accel_lat(lat_accel);
      msg.set_steering_angle(steer_angle);
      msg.set_steering_rate(steer_rate);
      msg.set_air_temp2(ext_air_temp);
      msg.set_dew_temp(dew_temp);
      msg.set_surface_temp(surface_temp);
      msgs.push_back(msg);
    }
  delete [] vehicle_id;
  return 0;
}
