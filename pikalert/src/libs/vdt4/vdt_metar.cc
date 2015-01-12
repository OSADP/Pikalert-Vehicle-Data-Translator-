// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include "vdt_metar.hh"

using namespace std;

void vdt_metar::set_point(vdt_point value)
{
  point = value;
}

void vdt_metar::set_station_name(string value)
{
  station_name = value;
}

void vdt_metar::set_time_obs(double value)
{
  time_obs = value;
}

void vdt_metar::set_air_temp(float value)
{
  air_temp = value;
}

void vdt_metar::set_dew_temp(float value)
{
  dew_temp = value;
}

void vdt_metar::set_slp(float value)
{
  slp = value;
}

void vdt_metar::set_hourly_precip(float value)
{
  hourly_precip = value;
}

void vdt_metar::set_precip_24hr(float value)
{
  precip_24hr = value;
}

void vdt_metar::set_prevail_vis_mi(float value)
{
  prevail_vis_mi = value;
}

void vdt_metar::set_prevail_vis_km(float value)
{
  prevail_vis_km = value;
}

void vdt_metar::set_wind_dir(int value)
{
  wind_dir = value;
}

void vdt_metar::set_wind_speed(int value)
{
  wind_speed = value;
}

void vdt_metar::set_wind_gust(int value)
{
  wind_gust = value;
}
