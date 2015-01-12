//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_rtma_dataset.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/08 16:56:55 $
//
//==============================================================================

/**
 * @file rwx_rtma_dataset.cc
 *
 * @date 10/24/13
 */

// Include files 
#include <string>
#include <vector>
#include "Proj4Wrap/Proj4Wrap.hh"
#include "rwx_conversions.hh"
#include "rwx_rtma_dataset.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


const std::string rwx_rtma_dataset::PRESSURE = "PRES_P0_L1_GLC0";
const std::string rwx_rtma_dataset::TEMPERATURE = "TMP_P0_L103_GLC0";
const std::string rwx_rtma_dataset::DEWPOINT = "DPT_P0_L103_GLC0";
const std::string rwx_rtma_dataset::HUMIDITY = "SPFH_P0_L103_GLC0";
const std::string rwx_rtma_dataset::WIND_DIR = "WDIR_P0_L103_GLC0";
const std::string rwx_rtma_dataset::WIND_SPEED = "WIND_P0_L103_GLC0";



rwx_rtma_dataset::rwx_rtma_dataset(const std::string &input_file_arg, const p4w::Proj4Wrap *rtma_proj_arg) : input_file(input_file_arg), rtma_proj(rtma_proj_arg)
{
  std::vector<std::string> field_names;
  error = "";

  field_names.push_back(PRESSURE);
  field_names.push_back(TEMPERATURE);
  field_names.push_back(DEWPOINT);
  
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

  return;
}

size_t rwx_rtma_dataset::get_xy_offset(size_t x, size_t y)
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


rwx_rtma_dataset::~rwx_rtma_dataset()
{
  delete collection;
}

int rwx_rtma_dataset::get_temperature_celsius(double lat, double lon, float &value)
{
  double xc;
  double yc;

  rtma_proj->ll2xy(lon, lat, &xc, &yc);
  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);
  value = rwx_conversions::k_to_c((*temperature_field)[offset]);

  return 0;
}

int rwx_rtma_dataset::get_dewpoint_celsius(double lat, double lon, float &value)
{
  double xc;
  double yc;

  rtma_proj->ll2xy(lon, lat, &xc, &yc);
  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);
  value = rwx_conversions::k_to_c((*dewpoint_field)[offset]);

  return 0;
}

int rwx_rtma_dataset::get_pressure_mb(double lat, double lon, float &value)
{
  double xc;
  double yc;

  rtma_proj->ll2xy(lon, lat, &xc, &yc);
  if (!in_grid(xc, yc))
    {
      return -1;
    }

  size_t offset = get_xy_offset(xc, yc);
  value = rwx_conversions::pa_to_mb((*pressure_field)[offset]);

  return 0;
}
