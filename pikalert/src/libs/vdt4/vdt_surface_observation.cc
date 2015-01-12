// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "vdt_surface_observation.hh"
#include "stat_tools.hh"

using namespace std;

const float vdt_surface_observation::MPH_TO_MS = 0.44704;
const float vdt_surface_observation::KNOT_TO_MS = 0.514444444;
const float vdt_surface_observation::MI_TO_KM = 1.609344;
const float vdt_surface_observation::IN_TO_MM = 25.4;
const float vdt_surface_observation::K_TO_C = 273.15;
const float vdt_surface_observation::PA_TO_MB = 0.01;

const float vdt_surface_observation::FILL_VALUE = -9999.0f;
const float vdt_surface_observation::MAX_MB = 1090.0;
const float vdt_surface_observation::MIN_MB = 580.0;
const float vdt_surface_observation::MAX_DEWP_C = 40.0;
const float vdt_surface_observation::MAX_AIR_C = 40.0;

vdt_surface_observation::vdt_surface_observation()
{
  station_name = "";
  time_obs = FILL_VALUE;
  air_temp = FILL_VALUE;
  surface_temp = FILL_VALUE;
  press = FILL_VALUE;
  slp = FILL_VALUE;
  dew_temp = FILL_VALUE;
  hourly_precip = FILL_VALUE;
  precip_24hr = FILL_VALUE;
  prevail_vis = FILL_VALUE;
  wind_dir = FILL_VALUE;
  wind_speed = FILL_VALUE;
  wind_gust = FILL_VALUE;
}

float vdt_surface_observation::get_avg_air_temp(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_air_temp;
  //printf("get_avg_air_temp\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_press(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_press;
  //printf("get_avg_press\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_vis(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_prevail_vis;
  //printf("get_avg_vis\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_dew_temp(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_dew_temp;
  //printf("get_avg_dew_temp\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_wind_speed(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_wind_speed;
  //printf("get_avg_wind_speed\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_wind_gust(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_wind_gust;
  //printf("get_avg_wind_gust\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_wind_dir(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_wind_dir;
  //printf("get_avg_wind_dir\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_hourly_precip(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_hourly_precip;
  //printf("get_avg_hourly_precip\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg(const vector<vdt_surface_observation>& obs, getter ptFn)
{
  vector<double> vals;

  if (obs.size() == 0)
    {
      //printf("obs.size() == 0\n");
      return FILL_VALUE;
    }
  for (size_t i = 0; i < obs.size(); i++)
    {
      float v = (obs[i].*ptFn)();
      //printf("v: %f\n", v);
      if (v != FILL_VALUE)
        {
          vals.push_back(v);
        }
    }
  if (vals.size() > 0)
    {
      //printf("about to compute mean\n");
      return ST_mean(vals);
    }
  return FILL_VALUE;
}

void vdt_surface_observation::get_points(const vector<vdt_surface_observation>& obs, vector<vdt_point> &points)
{
  for (size_t i=0; i<obs.size(); i++)
    {
      points.push_back(obs[i].get_point());
    }
}

float vdt_surface_observation::mph_to_ms(float v)
{
  return v * MPH_TO_MS;
}

float vdt_surface_observation::knot_to_ms(float v)
{
  return v * KNOT_TO_MS;
}

float vdt_surface_observation::mi_to_km(float v)
{
  return v * MI_TO_KM;
}

float vdt_surface_observation::in_to_mm(float v)
{
  return v * IN_TO_MM;
}

float vdt_surface_observation::k_to_c(float v)
{
  return v - K_TO_C;
}

float vdt_surface_observation::c_to_k(float v)
{
  return v + K_TO_C;
}

float vdt_surface_observation::pa_to_mb(float v)
{
  return v * PA_TO_MB;
}

void vdt_surface_observation::set_fill(int& v, int fill)
{
  if (v == fill)
    {
      v = (int)vdt_surface_observation::FILL_VALUE;
    }
}

void vdt_surface_observation::set_fill(float& v, float fill)
{
  if (v == fill)
    {
      v = vdt_surface_observation::FILL_VALUE;
    }
}
