//==============================================================================
//
//   (c) Copyright, 2016 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_rtma_dataset.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/08 16:56:55 $
//
//==============================================================================

/**
 * @file rwx_ak_rtma_dataset.cc
 *
 * @date 06/16/2016
 */

// Include files 
#include <string>
#include <vector>
#include "rwx_conversions.hh"
#include "rwx_ak_rtma_dataset.hh"


// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


const std::string rwx_ak_rtma_dataset::GEOPOTENTIAL_HEIGHT = "HGT_P0_L1_GST0";
const std::string rwx_ak_rtma_dataset::PRESSURE = "PRES_P0_L1_GST0";
const std::string rwx_ak_rtma_dataset::TEMPERATURE = "TMP_P0_L103_GST0";
const std::string rwx_ak_rtma_dataset::DEWPOINT = "DPT_P0_L103_GST0";
const std::string rwx_ak_rtma_dataset::HUMIDITY = "SPFH_P0_L103_GST0";
const std::string rwx_ak_rtma_dataset::WIND_DIR = "WDIR_P0_L103_GST0";
const std::string rwx_ak_rtma_dataset::WIND_SPEED = "WIND_P0_L103_GST0";
const float rwx_ak_rtma_dataset::MISSING = -9999.;


rwx_ak_rtma_dataset::rwx_ak_rtma_dataset(const std::string &input_file_arg, const rwx_ak_rtma_proj_info_t &proj_info) : input_file(input_file_arg)
{
  std::vector<std::string> field_names;

  error = "";

  sobstr(&stcpm, proj_info.latin, proj_info.lov);
  stcm2p(&stcpm, 0., 0., proj_info.la1, proj_info.lo1, proj_info.nx-1, proj_info.ny-1, proj_info.la2, proj_info.lo2); 

  field_names.push_back(PRESSURE);
  field_names.push_back(TEMPERATURE);
  field_names.push_back(DEWPOINT);
  field_names.push_back(WIND_DIR);
  field_names.push_back(WIND_SPEED);
  field_names.push_back(GEOPOTENTIAL_HEIGHT);

  collection = new rwx_vector_collection_nc(input_file, field_names);
  if (collection->error != std::string(""))
    {
      error = string("ingest error for:") + input_file.c_str() + " " + collection->error.c_str();
      return;
    }

  int ret = collection->get_vector(DEWPOINT, &dewpoint_field);
  if (ret != 0)
    {
      error = string("cannot get dewpoint field");
      return;
    }

  ret = collection->get_vector(TEMPERATURE, &temperature_field);
  if (ret != 0)
    {
      error = string("cannot get temperature field");
      return;
    }

  ret = collection->get_vector(PRESSURE, &pressure_field);
  if (ret != 0)
    {
      error = string("cannot get pressure field");
      return;
    }

  ret = collection->get_vector(WIND_DIR, &wind_dir_field);
  if (ret != 0)
    {
      error = string("cannot get wind direction field");
      return;
    }

  ret = collection->get_vector(WIND_SPEED, &wind_speed_field);
  if (ret != 0)
    {
      error = string("cannot get wind speed field");
      return;
    }

  ret = collection->get_vector(GEOPOTENTIAL_HEIGHT, &geopotential_height_field);
  if (ret != 0)
    {
      error = string("cannot get geopotential height field");
      return;
    }

  const unordered_map<std::string, std::vector<std::pair<string, size_t> > > &var_map = collection->get_var_map();

  auto itr = var_map.find(TEMPERATURE);
  if (itr == var_map.end() || itr->second.size() != 2)
    {
      error = string("cannot get temperature field");
      return;
    }

  if (itr->second[0].first == string("ygrid_0"))
    {
      yfirst = true;
      ydim = itr->second[0].second;
      if (itr->second[1].first == string("xgrid_0"))
	{
	  xdim = itr->second[1].second;
	}
      else
	{
	  error = string("cannot resolve x and y dimensions for temperature");
	  return;
	}
    }
  else if (itr->second[1].first == string("ygrid_0"))
    {
      yfirst = false;
      ydim = itr->second[1].second;
      if (itr->second[0].first == string("xgrid_0"))
	{
	  xdim = itr->second[0].second;
	}
      else
	{
	  error = string("cannot resolve x and y dimensions for temperature");
	  return;
	}
    }

  
  rwx::value_t value;

  ret = collection->get_missing(DEWPOINT, value);
  if (ret != 0)
    {
      error = string("cannot get dewpoint missing");
      return;
    }

  dewpoint_missing = value.v_float;

  
  ret = collection->get_missing(TEMPERATURE, value);
  if (ret != 0)
    {
      error = string("cannot get temperature missing");
      return;
    }

  temperature_missing = value.v_float;

  ret = collection->get_missing(PRESSURE, value);
  if (ret != 0)
    {
      error = string("cannot get pressure missing");
      return;
    }

  pressure_missing = value.v_float;
  
  ret = collection->get_missing(WIND_DIR, value);
  if (ret != 0)
    {
      error = string("cannot get wind_dir missing");
      return;
    }

  wind_dir_missing = value.v_float;
  
  ret = collection->get_missing(WIND_SPEED, value);
  if (ret != 0)
    {
      error = string("cannot get wind_speed missing");
      return;
    }

  wind_speed_missing = value.v_float;
  
  ret = collection->get_missing(GEOPOTENTIAL_HEIGHT, value);
  if (ret != 0)
    {
      error = string("cannot get geopotential height missing");
      return;
    }

  geopotential_height_missing = value.v_float;
  
  return;
}

size_t rwx_ak_rtma_dataset::get_xy_offset(size_t x, size_t y)
{
  if (yfirst)
    {
      return (y * xdim + x);
    }
  else
    {
      return (x * ydim + y);
    }
}


int rwx_ak_rtma_dataset::get_offset(double lat, double lon, size_t *offset)
{
  double xc;
  double yc;

  int ret = get_coord(lat, lon, &xc, &yc);
  if (ret == -1)
    {
      return -1;
    }

  size_t xint = rint(xc);
  size_t yint = rint(yc);

  *offset = get_xy_offset(xint, yint);
  return 0;
}


rwx_ak_rtma_dataset::~rwx_ak_rtma_dataset()
{
  delete collection;
}

int rwx_ak_rtma_dataset::get_coord(double lat, double lon, double *xc, double *yc)
{
  double xcoord;
  double ycoord;
  
  cll2xy(&stcpm, lat, lon, &xcoord, &ycoord);
  
  if (!in_grid(xcoord, ycoord))
    {
      return -1;
    }

  *xc = xcoord;
  *yc = ycoord;
  return 0;
}

int rwx_ak_rtma_dataset::get_coord_offset(double lat, double lon, double *xc, double *yc, size_t *offset)
{
  double xcoord;
  double ycoord;
  
  cll2xy(&stcpm, lat, lon, &xcoord, &ycoord);
  
  if (!in_grid(xcoord, ycoord))
    {
      return -1;
    }

  *xc = xcoord;
  *yc = ycoord;

  size_t xint = rint(*xc);
  size_t yint = rint(*yc);

  *offset = get_xy_offset(xint, yint);

  return 0;
}

int rwx_ak_rtma_dataset::get_temperature_celsius(double lat, double lon, float &value)
{
  double xc;
  double yc;

  cll2xy(&stcpm, lat, lon, &xc, &yc);

  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);


  float temperature = (*temperature_field)[offset];
  if (temperature == temperature_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::k_to_c(temperature);
    }

  return 0;
}

void rwx_ak_rtma_dataset::get_temperature_celsius(size_t offset, float &value)
{
  float temperature = (*temperature_field)[offset];
  if (temperature == temperature_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::k_to_c(temperature);
    }
}

int rwx_ak_rtma_dataset::get_dewpoint_celsius(double lat, double lon, float &value)
{
  double xc;
  double yc;

  cll2xy(&stcpm, lat, lon, &xc, &yc);

  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);

  float dewpoint = (*dewpoint_field)[offset];
  if (dewpoint == dewpoint_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::k_to_c(dewpoint);
    }

  return 0;
}

void rwx_ak_rtma_dataset::get_dewpoint_celsius(size_t offset, float &value)
{
  float dewpoint = (*dewpoint_field)[offset];
  if (dewpoint == dewpoint_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::k_to_c(dewpoint);
    }
}

int rwx_ak_rtma_dataset::get_pressure_mb(double lat, double lon, float &value)
{
  double xc;
  double yc;

  cll2xy(&stcpm, lat, lon, &xc, &yc);

  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);

  float pressure = (*pressure_field)[offset];
  if (pressure == pressure_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::pa_to_mb(pressure);
    }

  return 0;
}

void rwx_ak_rtma_dataset::get_pressure_mb(size_t offset, float &value)
{
  float pressure = (*pressure_field)[offset];
  if (pressure == pressure_missing)
    {
      value = MISSING;
    }
  else
    {
      value = rwx_conversions::pa_to_mb(pressure);
    }
}

void rwx_ak_rtma_dataset::get_wind_dir_deg(size_t offset, float &value)
{
  float wind_dir = (*wind_dir_field)[offset];
  if (wind_dir == wind_dir_missing)
    {
      value = MISSING;
    }
  else
    {
      value = wind_dir;
    }
}

void rwx_ak_rtma_dataset::get_wind_speed_ms(size_t offset, float &value)
{
  float wind_speed = (*wind_speed_field)[offset];
  if (wind_speed == wind_speed_missing)
    {
      value = MISSING;
    }
  else
    {
      value = wind_speed;
    }
}

void rwx_ak_rtma_dataset::get_geopotential_height(size_t offset, float &value)
{
  float geopotential_height = (*geopotential_height_field)[offset];
  if (geopotential_height == geopotential_height_missing)
    {
      value = MISSING;
    }
  else
    {
      value = geopotential_height;
    }
}
