// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_rwis_file_reader.hh"

const char* vdt_rwis_file_reader::REC_NUM = "recNum";
const char* vdt_rwis_file_reader::STN_NAME_LEN = "maxNameLength";

const char* vdt_rwis_file_reader::STATION_NAME = "stationName";
const char* vdt_rwis_file_reader::LATITUDE = "latitude";
const char* vdt_rwis_file_reader::LONGITUDE = "longitude";
const char* vdt_rwis_file_reader::ELEVATION = "elevation";
const char* vdt_rwis_file_reader::OBS_TIME = "observationTime";
const char* vdt_rwis_file_reader::AIR_TEMP = "temperature";
const char* vdt_rwis_file_reader::SURFACE_TEMP = "roadTemperature1";
const char* vdt_rwis_file_reader::PRESS = "stationPressure";
const char* vdt_rwis_file_reader::DEW_TEMP = "dewpoint";
const char* vdt_rwis_file_reader::HOURLY_PRECIP = "precipAccum";
const char* vdt_rwis_file_reader::PREVAIL_VIS = "visibility";
const char* vdt_rwis_file_reader::WIND_DIR = "windDir";
const char* vdt_rwis_file_reader::WIND_SPEED = "windSpeed";
const char* vdt_rwis_file_reader::WIND_GUST = "windGust";

const char* vdt_rwis_file_reader::FILL_ATTR = "_FillValue";

vdt_rwis_file_reader::~vdt_rwis_file_reader()
{
}

int vdt_rwis_file_reader::get_rwis(vector<vdt_surface_observation>& obs)
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
  NcVar* obstime_var = nc->get_var(OBS_TIME);
  NcVar* temp_var = nc->get_var(AIR_TEMP);
  float temp_fill = temp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* surface_temp_var = nc->get_var(SURFACE_TEMP);
  float surface_temp_fill = surface_temp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* press_var = nc->get_var(PRESS);
  float press_fill = press_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dewp_var = nc->get_var(DEW_TEMP);
  float dewp_fill = dewp_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* precip_hr_var = nc->get_var(HOURLY_PRECIP);
  float precip_hr_fill = precip_hr_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* vis_var = nc->get_var(PREVAIL_VIS);
  float vis_fill = vis_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* dir_var = nc->get_var(WIND_DIR);
  float dir_fill = dir_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* spd_var = nc->get_var(WIND_SPEED);
  float spd_fill = spd_var->get_att(FILL_ATTR)->values()->as_float(0);
  NcVar* gust_var = nc->get_var(WIND_GUST);
  float gust_fill = gust_var->get_att(FILL_ATTR)->values()->as_float(0);

  char* stn_name = new char[stnlen];
  for (int i = 0; i < recnum; i++)
    {
      stn_name_var->set_cur(i);
      lat_var->set_cur(i);
      lon_var->set_cur(i);
      elev_var->set_cur(i);
      obstime_var->set_cur(i);
      temp_var->set_cur(i);
      surface_temp_var->set_cur(i);
      press_var->set_cur(i);
      dewp_var->set_cur(i);
      precip_hr_var->set_cur(i);
      vis_var->set_cur(i);
      dir_var->set_cur(i);
      spd_var->set_cur(i);
      gust_var->set_cur(i);

      double obstime;
      float lat,lon,elev;
      float temp,surface_temp,dewp,press;
      float precip_hr;
      float vis;
      float spd,dir,gust;

      stn_name_var->get(stn_name,1,stnlen);
      lat_var->get(&lat,1);
      lon_var->get(&lon,1);
      elev_var->get(&elev,1);
      obstime_var->get(&obstime,1);
      temp_var->get(&temp,1);
      surface_temp_var->get(&surface_temp,1);
      dewp_var->get(&dewp,1);
      press_var->get(&press,1);
      precip_hr_var->get(&precip_hr,1);
      vis_var->get(&vis,1);
      dir_var->get(&dir,1);
      spd_var->get(&spd,1);
      gust_var->get(&gust,1);

      vdt_surface_observation::set_fill(temp,temp_fill);
      vdt_surface_observation::set_fill(surface_temp,surface_temp_fill);
      vdt_surface_observation::set_fill(dewp,dewp_fill);
      vdt_surface_observation::set_fill(press,press_fill);
      vdt_surface_observation::set_fill(precip_hr,precip_hr_fill);
      vdt_surface_observation::set_fill(vis,vis_fill);
      vdt_surface_observation::set_fill(spd,spd_fill);
      vdt_surface_observation::set_fill(dir,dir_fill);
      vdt_surface_observation::set_fill(gust,gust_fill);

      if (vis != vdt_surface_observation::FILL_VALUE)
        {
          vis = vis / 1000.0; //convert from m to km
        }
      if (temp != vdt_surface_observation::FILL_VALUE)
        {
          temp = vdt_surface_observation::k_to_c(temp);
        }
      if (dewp != vdt_surface_observation::FILL_VALUE)
        {
          dewp = vdt_surface_observation::k_to_c(dewp);
        }
      if (surface_temp != vdt_surface_observation::FILL_VALUE)
        {
          surface_temp = vdt_surface_observation::k_to_c(surface_temp);
        }
      if (press != vdt_surface_observation::FILL_VALUE)
        {
          press = vdt_surface_observation::pa_to_mb(press);
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
      o.set_surface_temp(surface_temp);
      o.set_dew_temp(dewp);
      o.set_press(press);
      o.set_hourly_precip(precip_hr);
      o.set_prevail_vis(vis);
      o.set_wind_speed(spd);
      o.set_wind_dir(dir);
      o.set_wind_gust(gust);

      obs.push_back(o);
    }
  delete [] stn_name;
  return 0;
}
