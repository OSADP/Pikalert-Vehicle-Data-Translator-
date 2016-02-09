//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_climatology_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_climatology_file_reader.hh
 *
 * @class vdt_climatology_file_reader
 *
 *  vdt climatology file reader class
 *
 */
#ifndef VDT_CLIMATOLOGY_FILE_READER_H
#define VDT_CLIMATOLOGY_FILE_READER_H

#include <blitz/array.h>   
#include <netcdf>
#include <string>
#include <vector>
#include "log/log.hh"

using namespace blitz;
using std::string;
using std::vector;

/**
 * @class vdt_climatology_file_reader
 * Reads netcdf climatology file to support climate range QC test
 */
class vdt_climatology_file_reader
{
public:

  /**
   * @brief constructor
   * @param[in] file_path file path
   */
  vdt_climatology_file_reader(const string &file_path);

  /**
   * @brief void constructor
   */
  vdt_climatology_file_reader() {};

  /** @brief netcdf month variable name */
  static string month_string;

  /** @brief netcdf latitude variable name */
  static string latitude_string;

  /** @brief netcdf longiude variable name */
  static string longitude_string;

  /** @brief netcdf lower left latitude variable name */
  static string lower_left_latitude_string;

  /** @brief netcdf lower left longitude variable name */
  static string lower_left_longitude_string;

  /** @brief netcdf upper right latitude variable name */
  static string upper_right_latitude_string;

  /** @brief netcdf upper right longitude variable name */
  static string upper_right_longitude_string;

  /** @brief netcdf minimum air temperature variable name */
  static string min_air_temperature_string;

  /** @brief netcdf maximum air temperature variable name */
  static string max_air_temperature_string;

  /** @brief netcdf minimum atmospheric pressure variable name */
  static string min_atmospheric_pressure_string;

  /** @brief netcdf maximum atmospheric pressure variable name */
  static string max_atmospheric_pressure_string;

  /** @brief netcdf minimum dewpoint temperature variable name */
  static string min_dewpoint_temperature_string;

  /** @brief netcdf maximum dewpoint temperature variable name */
  static string max_dewpoint_temperature_string;

  /** @brief netcdf minimum surface temperature variable name */
  static string min_surface_temperature_string;

  /** @brief netcdf maximum surface temperature variable name */
  static string max_surface_temperature_string;

  /**
   * @brief constructor error
   * @return error string
   */
  inline string error() const
  {
    return error_str;
  }

  /**
   * @brief get vector of months for the underlying grid
   * @return month vector
   */
  inline const vector<int> & get_month() const
  {
    return month;
  }

  /**
   * @brief get month vector size
   * @return month vector size
   */
  inline size_t get_month_size() const
  {
    return month_size;
  }

  /**
   * @brief get latitude vector
   * @return vector of latitudes for the underlying grid
   */
  inline const vector<float> & get_latitude() const
  {
    return latitude;
  }

  /**
   * @brief get latitude vector size
   * @return size of the latitude vector
   */
  inline size_t get_latitude_size() const
  {
    return latitude_size;
  }

  /**
   * @brief get longitude vector
   * @return vector of longitudes for the underlying grid
   */
  inline const vector<float> & get_longitude() const
  {
    return longitude;
  }

  /**
   * @brief get longitude vector size
   * @return size of the longitude size
   */
  inline size_t get_longitude_size() const
  {
    return longitude_size;
  }

  /**
   * @brief get lower left latitude of grid
   * @return lower left latitude value
   */
  inline float get_lower_left_latitude() const
  {
    return lower_left_latitude;
  }

  /**
   * @brief get lower left longitude of grid
   * @return lower left longitude value
   */
  inline float get_lower_left_longitude() const
  {
    return lower_left_longitude;
  }

  /**
   * @brief get upper right latitude of grid
   * @return upper right latitude value
   */
  inline float get_upper_right_latitude() const
  {
    return upper_right_latitude;
  }

  /**
   * @brief get upper right longitude of grid
   * @return upper right longitude value
   */
  inline float get_upper_right_longitude() const
  {
    return upper_right_longitude;
  }

  /**
   * @brief get min air temperature vector
   * @return vector containing all minimum air temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_min_air_temperature() const
  {
    return min_air_temperature;
  }

  /**
   * @brief get min air temperature grid
   * @return minimum air temperature grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_min_air_temperature_grid() const
  {
    return min_air_temperature_grid;
  }

  /**
   * @brief get max air temperature vector
   * @return vector containing all maximum air temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_max_air_temperature() const
  {
    return max_air_temperature;
  }

  /**
   * @brief get max air temperature grid
   * @return maximum air temperature grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_max_air_temperature_grid() const
  {
    return max_air_temperature_grid;
  }

  /**
   * @brief get min atmospheric pressure vector
   * @return vector containing all minimum atmospheric pressures as stored in the netcdf file
   */
  inline const vector<float> & get_min_atmospheric_pressure() const
  {
    return min_atmospheric_pressure;
  }

  /**
   * @brief get min atmospheric pressure grid
   * @return minimum atmospheric pressure grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_min_atmospheric_pressure_grid() const
  {
    return min_atmospheric_pressure_grid;
  }

  /**
   * @brief get max atmospheric pressure vector
   * @return vector containing all maximum atmospheric pressures as stored in the netcdf file
   */
  inline const vector<float> & get_max_atmospheric_pressure() const
  {
    return max_atmospheric_pressure;
  }

  /**
   * @brief get max atmospheric pressure grid
   * @return maximum atmospheric pressure grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_max_atmospheric_pressure_grid() const
  {
    return max_atmospheric_pressure_grid;
  }

  /**
   * @brief get min dewpoint temperature vector
   * @return vector containing all minimum dewpoint temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_min_dewpoint_temperature() const
  {
    return min_dewpoint_temperature;
  }

  /**
   * @brief get min dewpoint temperature grid
   * @return minimum dewpoint temperatures grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_min_dewpoint_temperature_grid() const
  {
    return min_dewpoint_temperature_grid;
  }

  /**
   * @brief get max dewpoint temperature vector
   * @return vector containing all maximum dewpoint temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_max_dewpoint_temperature() const
  {
    return max_dewpoint_temperature;
  }

  /**
   * @brief get max dewpoint temperature grid
   * @return maximum dewpoint temperatures grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_max_dewpoint_temperature_grid() const
  {
    return max_dewpoint_temperature_grid;
  }

  /**
   * @brief get min surface temperature vector
   * @return vector containing all minimum surface temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_min_surface_temperature() const
  {
    return min_surface_temperature;
  }

  /**
   * @brief get min surface temperature grid
   * @return minimum surface temperature grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_min_surface_temperature_grid() const
  {
    return min_surface_temperature_grid;
  }

  /**
   * @brief get max surface temperature vector
   * @return vector containing all maximum surface temperatures as stored in the netcdf file
   */
  inline const vector<float> & get_max_surface_temperature() const
  {
    return max_surface_temperature;
  }

  /**
   * @brief get max surface temperature grid
   * @return maximum surface temperature grid vector with the dimensions month, latitude, and longitude
   */
  inline const Array<float, 3> & get_max_surface_temperature_grid() const
  {
    return max_surface_temperature_grid;
  }

  /**
   * @brief get integer coordinates for double lat, lon coordinates
   * @param[in] dlat double latitude value
   * @param[in] dlon double longitude value
   * @param[out] ilat integer latitude grid value
   * @param[out] ilon integer longitude grid value
   * @return 0 on success, -1 on failure
   */
  int round_to_integer_coords(double dlat, double dlon, int *ilat, int *ilon) const;

  /**
   * @brief get xoffset and yoffset for given lat and lon
   * @param[in] latitude  latitude value
   * @param[in] longitude  longitude value
   * @param[out] xoffset  longitude offset
   * @param[out] yoffset  latitude offset
   */
  void latlon2xy(double latitude, double longitude, double *xoffset, double *yoffset) const;

  /**
   * @brief get short min and max values for given min and max grids
   * @param[in] month_index  month index
   * @param[in] lat_index  latitude index
   * @param[in] lon_index  longitude index
   * @param[in] min_grid  grid with minimum values
   * @param[in] max_grid  grid with maximum values
   * @param[out] min_value  minimum value for given indexs
   * @param[out] max_value  maximum value for given indexs
   */
  void get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, short *min_value, short *max_value) const;

  void get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, int *min_value, int *max_value) const
  {
    short smin_value;
    short smax_value;
    get_min_max_climate_values(month_index, lat_index, lon_index, min_grid, max_grid, &smin_value, &smax_value);
    *min_value = smin_value;
    *max_value = smax_value;
  }

  /**
   * @brief get float min and max values for given min and max grids
   * @param[in] month_index  month index
   * @param[in] lat_index  latitude index
   * @param[in] lon_index  longitude index
   * @param[in] min_grid  grid with minimum values
   * @param[in] max_grid  grid with maximum values
   * @param[out] min_value  minimum value for given indexs
   * @param[out] max_value  maximum value for given indexs
   */
  void get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, float *min_value, float *max_value) const;

  void get_min_max_climate_values(int month_index, int lat_index, int lon_index, const Array<float, 3> &min_grid, const Array<float, 3> &max_grid, double *min_value, double *max_value) const
  {
    float fmin_value;
    float fmax_value;
    get_min_max_climate_values(month_index, lat_index, lon_index, min_grid, max_grid, &fmin_value, &fmax_value);
    *min_value = fmin_value;
    *max_value = fmax_value;
  }
protected:

  /**
   * @brief read vector of integer variables
   * @param[in] data_file  netcdf data file
   * @param[in] var_string  variable name string
   * @param[out] values  vector of values
   * @param[out] error_str  error string
   * @return 0 on success, -1 on failure
   */
  int read_int_var(netCDF::NcFile &data_file, const string &var_string, vector<int> &values, string &error_str);

  /**
   * @brief read vector of float variables
   * @param[in] data_file  netcdf data file
   * @param[in] var_string  variable name string
   * @param[out] values  vector of values
   * @param[out] error_str  error string
   * @return 0 on success, -1 on failure
   */
  int read_float_var(netCDF::NcFile &data_file, const string &var_string, vector<float> &values, string &error_str);

  /**
   * @brief read float variable
   * @param[in] data_file  netcdf data file
   * @param[in] var_string  variable name string
   * @param[out] value_ptr  value pointer
   * @param[out] error_str  error string
   * @return 0 on success, -1 on failure
   */
  int read_scalar_var(netCDF::NcFile &data_file, const string &var_string, float *value_ptr, string &error_str);

  /**
   * @brief stores vector of input values in grid implemented using combination of 3 vectors
   * @param[in] values  vector of values
   * @param[out] grid  grid structure
   */
  void get_grid_vector(vector<float> &values, Array<float, 3> &grid) const;

private:

  /** @brief error string */
  string error_str;

  /** @brief month vector */
  vector<int> month;

  /** @brief size of month vector */
  int month_size;

  /** @brief latitude vector */
  vector<float> latitude;

  /** @brief size of latitude vector */
  int latitude_size;

  /** @brief longitude vector */
  vector<float> longitude;

  /** @brief size of longitude vector */
  int longitude_size;

  /** @brief lower left latitude */
  float lower_left_latitude;

  /** @brief lower left longitude */
  float lower_left_longitude;

  /** @brief upper right latitude */
  float upper_right_latitude;

  /** @brief upper right longitude */
  float upper_right_longitude;

  /** @brief minimum air temperature vector */
  vector<float> min_air_temperature;

  /** @brief minimum air temperature grid vector */
  Array<float, 3> min_air_temperature_grid;

  /** @brief maximum air temperature vector */
  vector<float> max_air_temperature;

  /** @brief maximum air temperature grid vector */
  Array<float, 3>  max_air_temperature_grid;

  /** @brief minimum atmospheric pressure vector */
  vector<float> min_atmospheric_pressure;

  /** @brief minimum atmospheric pressure grid vector */
  Array<float, 3>  min_atmospheric_pressure_grid;

  /** @brief maximum atmospheric pressure vector */
  vector<float> max_atmospheric_pressure;

  /** @brief maximum atmospheric pressure grid vector */
  Array<float, 3>  max_atmospheric_pressure_grid;

  /** @brief minimum dewpoint temperature vector */
  vector<float> min_dewpoint_temperature;

  /** @brief minimum dewpoint temperature grid vector */
  Array<float, 3> min_dewpoint_temperature_grid;

  /** @brief maximum dewpoint temperature vector */
  vector<float> max_dewpoint_temperature;

  /** @brief maximum dewpoint temperature grid vector */
  Array<float, 3> max_dewpoint_temperature_grid;

  /** @brief minimum surface temperature vector */
  vector<float> min_surface_temperature;

  /** @brief minimum surface temperature grid vector */
  Array<float, 3> min_surface_temperature_grid;

  /** @brief maximum surface temperature vector */
  vector<float> max_surface_temperature;

  /** @brief maximum surface temperature grid vector */
  Array<float, 3>  max_surface_temperature_grid;

  /** @brief assignment operator */
  vdt_climatology_file_reader & operator=(const vdt_climatology_file_reader &vdt_climatology_file_reader);

  vdt_climatology_file_reader(const vdt_climatology_file_reader &vdt_climatology_file_reader);

};

#endif /* VDT_CLIMATOLOGY_FILE_READER_H */
