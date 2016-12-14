
#include <math.h>
#include "vdt_surface_observation.hh"
#include "stat_tools.hh"

using namespace std;

static const float EPS = 10E-6;

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

inline void speed_dir_to_uv(float speed, float dir, float *u, float *v)
{
  float radians = (dir / 360.) * (2 * M_PI);
  *u = -1 * speed * sin(radians);
  *v = -1 * speed * cos(radians);
}

// Sets *mean_wind_speed_ptr and *mean_wind_dir_ptr to FILL_VALUE is averages can't be calculated
int vdt_surface_observation::get_avg_speed_dir(const vector<float> &speed, const vector<float> &dir, float *mean_wind_speed_ptr, float *mean_wind_dir_ptr)
{
  float RADIAN = 360.0 / (2 * M_PI);

  vector<float> u_vec;
  vector<float> v_vec;

  *mean_wind_speed_ptr = FILL_VALUE;
  *mean_wind_dir_ptr = FILL_VALUE;

  if ((speed.size() != dir.size()) || speed.size() == 0)
    {
      return -1;
    }

  for (size_t i=0; i<speed.size(); i++)
    {
      float u;
      float v;

      speed_dir_to_uv(speed[i], dir[i], &u, &v);
      u_vec.push_back(u);
      v_vec.push_back(v);
    }

  float mean_u = ST_mean(u_vec);
  float mean_v = ST_mean(v_vec);
  *mean_wind_speed_ptr = sqrt(mean_u * mean_u + mean_v * mean_v);

  float mean_wind_dir = 0;
  
  if (fabs(mean_u) < EPS)
    {
      mean_wind_dir = 0.0;
    }
  else
    {
      mean_wind_dir = atan(mean_v/mean_u);
      if (mean_u >= 0 && mean_v >= 0)
	{
	  mean_wind_dir = fmodf(270 - RADIAN * mean_wind_dir, 360.);
	}
      else if (mean_u >= 0 && mean_v <= 0)
	{
	  mean_wind_dir = fmodf(270 + RADIAN * fabs(mean_wind_dir), 360.);
	}
      else if (mean_u <= 0 && mean_v <= 0)
	{
	  mean_wind_dir = fmodf(360 + 90.0 - RADIAN * fabs(mean_wind_dir), 360.);
	}
      else if (mean_u <= 0 && mean_v >= 0)
	{
	  mean_wind_dir = fmodf(360 + 90 + RADIAN * fabs(mean_wind_dir), 360.);
	}
    }

  *mean_wind_dir_ptr = mean_wind_dir;

  return 0;
}


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
  pres_wx = "";
  road_state_1 = static_cast<int>(FILL_VALUE);
}

float vdt_surface_observation::get_avg_air_temp(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_air_temp;
  //printf("get_avg_air_temp\n");
  return get_avg(obs,ptFn);
}

float vdt_surface_observation::get_avg_surface_temp(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_surface_temp;
  //printf("get_avg_surface_temp\n");
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
  float speed;
  float dir;

  get_wind_speed_dir(obs, &speed, &dir);
  return dir;
}

float vdt_surface_observation::get_avg_hourly_precip(const vector<vdt_surface_observation>& obs)
{
  getter ptFn = &vdt_surface_observation::get_hourly_precip;
  //printf("get_avg_hourly_precip\n");
  return get_avg(obs,ptFn);
}

void vdt_surface_observation::get_wind_speed_dir(const vector<vdt_surface_observation>& obs, float *mean_speed, float *mean_dir)
{
  vector<double> vals;
  float speed;
  float dir;
  vector<float> speed_vec;
  vector<float> dir_vec;

  if (obs.size() == 0)
    {
      *mean_speed = FILL_VALUE;
      *mean_dir = FILL_VALUE;
      return;
    }

  for (size_t i = 0; i < obs.size(); i++)
    {
      speed = obs[i].get_wind_speed();
      dir =  obs[i].get_wind_dir();

      if (speed != FILL_VALUE && dir != FILL_VALUE)
	{
	  speed_vec.push_back(speed);
	  dir_vec.push_back(dir);
	}
    }
  if (speed_vec.size() > 0)
    {
      get_avg_speed_dir(speed_vec, dir_vec, mean_speed, mean_dir);
    }
  else
    {
      *mean_speed = FILL_VALUE;
      *mean_dir = FILL_VALUE;
    }
  
  return;
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
