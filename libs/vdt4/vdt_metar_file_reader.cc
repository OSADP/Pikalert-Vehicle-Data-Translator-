
#include <math.h>

#include "vdt_metar_file_reader.hh"
#include "vdt_point.hh"

const char* vdt_metar_file_reader::REC_NUM = "recNum";
const char* vdt_metar_file_reader::STN_NAME_LEN = "stn_name_len";

const char* vdt_metar_file_reader::STATION_NAME = "stn_name";
const char* vdt_metar_file_reader::LATITUDE = "lat";
const char* vdt_metar_file_reader::LONGITUDE = "lon";
const char* vdt_metar_file_reader::ELEVATION = "elev";
const char* vdt_metar_file_reader::OBS_TIME = "time_obs";
const char* vdt_metar_file_reader::AIR_TEMP = "T_tenths";
const char* vdt_metar_file_reader::PRESS = "SLP";
const char* vdt_metar_file_reader::DEW_TEMP = "TD_tenths";
const char* vdt_metar_file_reader::HOURLY_PRECIP = "PRECIP_hourly";
const char* vdt_metar_file_reader::PRECIP_24HR = "PRECIP_24_amt";
const char* vdt_metar_file_reader::PREVAIL_VIS = "prevail_VIS_SM";
const char* vdt_metar_file_reader::WIND_DIR = "DIR";
const char* vdt_metar_file_reader::WIND_SPEED = "SPD";
const char* vdt_metar_file_reader::WIND_GUST = "GUST";
const char* vdt_metar_file_reader::FILL_ATTR = "_FillValue";

const float vdt_metar_file_reader::SLP_TO_PRESS = 29.263;

const char* vdt_metar_file_reader::WX_LEN = "WX_len";
const char* vdt_metar_file_reader::WX = "WX";

vdt_metar_file_reader::~vdt_metar_file_reader()
{
}

int vdt_metar_file_reader::get_metars(vector<vdt_surface_observation>& obs)
{
  if (nc == NULL)
    {
      return 1;
    }

  int recnum = nc->get_dim(REC_NUM)->size();
  int stnlen = nc->get_dim(STN_NAME_LEN)->size();
  NcVar* stn_name_var = nc->get_var(STATION_NAME);
  NcVar* lat_var = nc->get_var(LATITUDE);
  NcVar* lon_var = nc->get_var(LONGITUDE);
  NcVar* elev_var = nc->get_var(ELEVATION);
  float elev_fill = elev_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* obstime_var = nc->get_var(OBS_TIME);
  NcVar* temp_var = nc->get_var(AIR_TEMP);
  float temp_fill = temp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* press_var = nc->get_var(PRESS);
  float press_fill = press_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dewp_var = nc->get_var(DEW_TEMP);
  float dewp_fill = dewp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* precip_hr_var = nc->get_var(HOURLY_PRECIP);
  float precip_hr_fill = precip_hr_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* precip_24_var = nc->get_var(PRECIP_24HR);
  float precip_24_fill = precip_24_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* vis_var = nc->get_var(PREVAIL_VIS);
  float vis_fill = vis_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dir_var = nc->get_var(WIND_DIR);
  int dir_fill = dir_var->get_att(FILL_ATTR)->values()->as_int(0);
  NcVar* spd_var = nc->get_var(WIND_SPEED);
  int spd_fill = spd_var->get_att(FILL_ATTR)->values()->as_int(0);
  NcVar* gust_var = nc->get_var(WIND_GUST);
  int gust_fill = gust_var->get_att(FILL_ATTR)->values()->as_int(0);
  int wxlen = nc->get_dim(WX_LEN)->size();
  NcVar* wx_var = nc->get_var(WX);

  char* stn_name = new char[stnlen];
  char* wx = new char[wxlen];
  for (int i = 0; i < recnum; i++)
    {
      stn_name_var->set_cur(i);
      lat_var->set_cur(i);
      lon_var->set_cur(i);
      elev_var->set_cur(i);
      obstime_var->set_cur(i);
      temp_var->set_cur(i);
      press_var->set_cur(i);
      dewp_var->set_cur(i);
      precip_hr_var->set_cur(i);
      precip_24_var->set_cur(i);
      vis_var->set_cur(i);
      dir_var->set_cur(i);
      spd_var->set_cur(i);
      gust_var->set_cur(i);
      wx_var->set_cur(i);

      int obstime;
      float lat,lon;
      int elev;
      float temp,dewp,press;
      float precip_hr,precip_24;
      float vis;
      int dir,spd,gust;

      stn_name_var->get(stn_name,1,stnlen);
      lat_var->get(&lat,1);
      lon_var->get(&lon,1);
      elev_var->get(&elev,1);
      obstime_var->get(&obstime,1);
      temp_var->get(&temp,1);
      dewp_var->get(&dewp,1);
      press_var->get(&press,1);
      precip_hr_var->get(&precip_hr,1);
      precip_24_var->get(&precip_24,1);
      vis_var->get(&vis,1);
      dir_var->get(&dir,1);
      spd_var->get(&spd,1);
      gust_var->get(&gust,1);
      wx_var->get(wx,1,wxlen);

      vdt_surface_observation::set_fill(elev,elev_fill);
      vdt_surface_observation::set_fill(temp,temp_fill);
      vdt_surface_observation::set_fill(dewp,dewp_fill);
      vdt_surface_observation::set_fill(press,press_fill);
      vdt_surface_observation::set_fill(precip_hr,precip_hr_fill);
      vdt_surface_observation::set_fill(precip_24,precip_24_fill);
      vdt_surface_observation::set_fill(vis,vis_fill);
      vdt_surface_observation::set_fill(spd,spd_fill);
      vdt_surface_observation::set_fill(dir,dir_fill);
      vdt_surface_observation::set_fill(gust,gust_fill);

      if (precip_hr != vdt_surface_observation::FILL_VALUE)
        {
          precip_hr = vdt_surface_observation::in_to_mm(precip_hr);
        }
      if (precip_24 != vdt_surface_observation::FILL_VALUE)
        {
          precip_24 = vdt_surface_observation::in_to_mm(precip_24);
        }
      if (vis != vdt_surface_observation::FILL_VALUE)
        {
          vis = vdt_surface_observation::mi_to_km(vis);
        }
      if (spd != vdt_surface_observation::FILL_VALUE)
        {
          spd = vdt_surface_observation::knot_to_ms(spd);
        }
      if (gust != vdt_surface_observation::FILL_VALUE)
        {
          gust = vdt_surface_observation::knot_to_ms(gust);
        }
      if (press != vdt_surface_observation::FILL_VALUE &&
          temp != vdt_surface_observation::FILL_VALUE &&
          elev != vdt_surface_observation::FILL_VALUE)
        {
          float ktemp = vdt_surface_observation::c_to_k(temp);
          press = press * exp(-elev / (ktemp * SLP_TO_PRESS));
        }
      else
        {
          press = vdt_surface_observation::FILL_VALUE;
        }

      vdt_surface_observation o;
      vdt_point pt;
      pt.set_latitude(lat);
      pt.set_longitude(lon);
      pt.set_elevation(elev);
      o.set_time_obs(obstime);
      o.set_point(pt);
      o.set_station_name(string(stn_name));
      o.set_air_temp(temp);
      o.set_dew_temp(dewp);
      o.set_press(press);
      o.set_hourly_precip(precip_hr);
      o.set_precip_24hr(precip_24);
      o.set_prevail_vis(vis);
      o.set_wind_speed(spd);
      o.set_wind_dir(dir);
      o.set_wind_gust(gust);
      o.set_wx(wx);

      obs.push_back(o);
    }
  delete [] stn_name;
  delete [] wx;
  return 0;
}
