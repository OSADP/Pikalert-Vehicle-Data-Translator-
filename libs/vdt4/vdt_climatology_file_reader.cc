//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_climatology_file_reader.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_climatology_file_reader.cc
 *
 *  Read climatology data from a netcdf file
 */

// Include files
#include <iostream>
#include <math.h>
#include <sstream>
#include "vdt_climatology_file_reader.hh"
#include "vdt_const.hh"
#include <stdio.h>

using std::string;
using std::vector;

// Constant and macros

// Types, structures and classes

// Global variables

// Functions

int vdt_climatology_file_reader::read_int_var(netCDF::NcFile &data_file, const string &var_string, vector<int> &values, string &error_str)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file.getVar(var_string);
  if (var.isNull())
    {
      error_str = string("could not find variable: ") + var_string;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  values.resize(dims[0].getSize());
  var.getVar(&values[0]);

  return 0;
}

int vdt_climatology_file_reader::read_float_var(netCDF::NcFile &data_file, const string &var_string, vector<float> &values, string &error_str)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file.getVar(var_string);
  if (var.isNull())
    {
      error_str = string("could not find variable: ") + var_string;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();
  int vector_size = 1;

  for (unsigned int i = 0; i<dims.size(); i++)
    {
      vector_size = vector_size * (int)dims[i].getSize();
    }

  values.resize(vector_size);
  var.getVar(&values[0]);

  return 0;
}

int vdt_climatology_file_reader::read_scalar_var(netCDF::NcFile &data_file, const string &var_string, float *value_ptr, string &error_str)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file.getVar(var_string);
  if (var.isNull())
    {
      error_str = string("could not find variable: ") + var_string;
      return -1;
    }

  var.getVar(value_ptr);

  return 0;
}

void vdt_climatology_file_reader::get_grid_vector(vector<float> &values, Array<float, 3> &grid) const
{
  // Resize the grid
  grid.resize(month_size, latitude_size, longitude_size);
  for (int k = 0; k < month_size; k++)
    {
      for (int j = 0; j < latitude_size; j++)
        {
          for (int i = 0; i < longitude_size; i++)
            {
              grid(k,j,i) = values[((k * latitude_size) + j) * longitude_size + i];
            }
        }
    }
}

int vdt_climatology_file_reader::round_to_integer_coords(double dlat, double dlon, int *ilat, int *ilon) const
{
  // Ensure dlat, dlon is on grid
  if (dlat < 0)
    *ilat = 0;
  else if (dlat > (latitude_size - 1))
    {
      if (dlat < latitude_size)
	{
	  *ilat = latitude_size - 1;
	}
      else
	{
	  return -1;
	}
    }
  else
    *ilat = static_cast<int>(rint(dlat));

  if (dlon < 0)
    *ilon = 0;
  else if (dlon > (longitude_size - 1))
    {
      if (dlon < longitude_size)
	{
	  *ilon = longitude_size - 1;
	}
      else
	{
	  return -1;
	}
    }
  else
    *ilon = static_cast<int>(rint(dlon));

  return 0;
}


void vdt_climatology_file_reader::latlon2xy(double lat, double lon, double *xoffset, double *yoffset) const
{
  double xoff = (longitude_size - 1) * (lon - lower_left_longitude) / (upper_right_longitude - lower_left_longitude);
  double yoff = (latitude_size - 1) * (lat - lower_left_latitude) / (upper_right_latitude - lower_left_latitude);
  *xoffset = xoff;
  *yoffset = yoff;
}

void vdt_climatology_file_reader::get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, short *min_value, short *max_value) const
{
  if (((month_index >= month_size) || (month_index < 0)) ||
      ((lat_index >= latitude_size) || (lat_index < 0)) ||
      ((lon_index >= longitude_size) || (lon_index < 0)))
    {
      *min_value = vdt_const::SHORT_FILL_VALUE;
      *max_value = vdt_const::SHORT_FILL_VALUE;
    }
  else
    {
      *min_value = min_grid(month_index, lat_index, lon_index);
      *max_value = max_grid(month_index, lat_index, lon_index);
    }
}

void vdt_climatology_file_reader::get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, float *min_value, float *max_value) const
{
  if (((month_index >= month_size) || (month_index < 0)) ||
      ((lat_index >= latitude_size) || (lat_index < 0)) ||
      ((lon_index >= longitude_size) || (lon_index < 0)))
    {
      *min_value = vdt_const::FILL_VALUE;
      *max_value = vdt_const::FILL_VALUE;
    }
  else
    {
      *min_value = min_grid(month_index, lat_index, lon_index);
      *max_value = max_grid(month_index, lat_index, lon_index);
    }
}

vdt_climatology_file_reader::vdt_climatology_file_reader(const string &file_path)
{
  error_str = "";

  if (file_path == "")
    {
      error_str = "no file specified for vdt_climatology_file_reader";
      return;
    }
  try
    {
      // Open the file and check to make sure it's valid.
      netCDF::NcFile data_file(file_path, netCDF::NcFile::read);

      if (read_int_var(data_file, vdt_climatology_file_reader::month_string, month, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::latitude_string, latitude, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::longitude_string, longitude, error_str) < 0)
        {
          return;
        }

      if (read_scalar_var(data_file, vdt_climatology_file_reader::lower_left_latitude_string, &lower_left_latitude, error_str) < 0)
        {
          return;
        }

      if (read_scalar_var(data_file, vdt_climatology_file_reader::lower_left_longitude_string, &lower_left_longitude, error_str) < 0)
        {
          return;
        }

      if (read_scalar_var(data_file, vdt_climatology_file_reader::upper_right_latitude_string, &upper_right_latitude, error_str) < 0)
        {
          return;
        }

      if (read_scalar_var(data_file, vdt_climatology_file_reader::upper_right_longitude_string, &upper_right_longitude, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::min_air_temperature_string, min_air_temperature, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::max_air_temperature_string, max_air_temperature, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::min_atmospheric_pressure_string, min_atmospheric_pressure, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::max_atmospheric_pressure_string, max_atmospheric_pressure, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::min_dewpoint_temperature_string, min_dewpoint_temperature, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::max_dewpoint_temperature_string, max_dewpoint_temperature, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::min_surface_temperature_string, min_surface_temperature, error_str) < 0)
        {
          return;
        }

      if (read_float_var(data_file, vdt_climatology_file_reader::max_surface_temperature_string, max_surface_temperature, error_str) < 0)
        {
          return;
        }

      month_size = month.size();
      latitude_size = latitude.size();
      longitude_size = longitude.size();

      get_grid_vector(min_air_temperature, min_air_temperature_grid);
      
      get_grid_vector(max_air_temperature, max_air_temperature_grid);

      get_grid_vector(min_atmospheric_pressure, min_atmospheric_pressure_grid);

      get_grid_vector(max_atmospheric_pressure, max_atmospheric_pressure_grid);

      get_grid_vector(min_dewpoint_temperature, min_dewpoint_temperature_grid);

      get_grid_vector(max_dewpoint_temperature, max_dewpoint_temperature_grid);

      get_grid_vector(min_surface_temperature, min_surface_temperature_grid);

      get_grid_vector(max_surface_temperature, max_surface_temperature_grid);
    }
  catch (netCDF::exceptions::NcException e)
    {
      error_str = string("failed in climatology file reader for file ") + file_path;
      return;
    }
}

string vdt_climatology_file_reader::month_string = "month";
string vdt_climatology_file_reader::latitude_string = "latitude";
string vdt_climatology_file_reader::longitude_string = "longitude";
string vdt_climatology_file_reader::lower_left_latitude_string = "La1";
string vdt_climatology_file_reader::lower_left_longitude_string = "Lo1";
string vdt_climatology_file_reader::upper_right_latitude_string = "La2";
string vdt_climatology_file_reader::upper_right_longitude_string = "Lo2";
string vdt_climatology_file_reader::min_air_temperature_string = "minAirTemperature";
string vdt_climatology_file_reader::max_air_temperature_string = "maxAirTemperature";
string vdt_climatology_file_reader::min_atmospheric_pressure_string = "minAtmosphericPressure";
string vdt_climatology_file_reader::max_atmospheric_pressure_string = "maxAtmosphericPressure";
string vdt_climatology_file_reader::min_dewpoint_temperature_string = "minDewpointTemperature";
string vdt_climatology_file_reader::max_dewpoint_temperature_string = "maxDewpointTemperature";
string vdt_climatology_file_reader::min_surface_temperature_string = "minSurfaceTemperature";
string vdt_climatology_file_reader::max_surface_temperature_string = "maxSurfaceTemperature";
