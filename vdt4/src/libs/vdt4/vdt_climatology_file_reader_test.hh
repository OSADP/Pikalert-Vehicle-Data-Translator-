//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_climatology_file_reader_test.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/04/22 22:09:42 $
//
//==============================================================================

/**
 *
 * @file vdt_climatology_file_reader_test.hh
 *
 * @class vdt_climatology_file_reader_test
 *
 *  vdt climatology file reader test class
 *
 */
#ifndef VDT_CLIMATOLOGY_FILE_READER_TEST_H
#define VDT_CLIMATOLOGY_FILE_READER_TEST_H

#include <netcdf>
#include <string>
#include <vector>
#include "vdt_climatology_file_reader.hh"
#include "log/log.hh"

using std::string;
using std::vector;

/**
 * @class vdt_climatology_file_reader_test
 * reads netcdf climatology file to support climate range QC test
 */
class vdt_climatology_file_reader_test : public vdt_climatology_file_reader
{
public:

  /**
   * @brief constructor
   */
  vdt_climatology_file_reader_test();

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
  inline const size_t get_month_size() const
  {
    return month.size();
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
  inline const size_t get_latitude_size() const
  {
    return latitude.size();
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
  inline const size_t get_longitude_size() const
  {
    return longitude.size();
  }

  /**
   * @brief get lower left latitude of grid
   * @return lower left latitude value
   */
  inline const float get_lower_left_latitude() const
  {
    return lower_left_latitude;
  }

  /**
   * @brief get lower left longitude of grid
   * @return lower left longitude value
   */
  inline const float get_lower_left_longitude() const
  {
    return lower_left_longitude;
  }

  /**
   * @brief get upper right latitude of grid
   * @return upper right latitude value
   */
  inline const float get_upper_right_latitude() const
  {
    return upper_right_latitude;
  }

  /**
   * @brief get upper right longitude of grid
   * @return upper right longitude value
   */
  inline const float get_upper_right_longitude() const
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
  inline const vector< vector< vector<float> > > & get_min_air_temperature_grid() const
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
  inline const vector< vector< vector<float> > > & get_max_air_temperature_grid() const
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
  inline const vector< vector< vector<float> > > & get_min_atmospheric_pressure_grid() const
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
  inline const vector< vector< vector<float> > > & get_max_atmospheric_pressure_grid() const
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
  inline const vector< vector< vector<float> > > & get_min_dewpoint_temperature_grid() const
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
  inline const vector< vector< vector<float> > > & get_max_dewpoint_temperature_grid() const
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
  inline const vector< vector< vector<float> > > & get_min_surface_temperature_grid() const
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
  inline const vector< vector< vector<float> > > & get_max_surface_temperature_grid() const
  {
    return max_surface_temperature_grid;
  }

  /**
   * @brief get xoffset and yoffset for given lat and lon
   * @param[in] nx  number of longitude points
   * @param[in] ny  number of latitude points
   * @param[in] latitude  latitude value
   * @param[in] longitude  longitude value
   * @param[out] xoffset  longitude offset
   * @param[out] yoffset  latitude offset
   */
  void latlon2xy(size_t nx, size_t ny, double latitude, double longitude, double *xoffset, double *yoffset) const;

  /**
   * @brief get min and max values for given min and max grids
   * @param[in] dim1  first dimension length
   * @param[in] dim2  second dimension length
   * @param[in] dim3  third dimension length
   * @param[in] dim1_offset  offset into first dimension
   * @param[in] dim2_offset  offset into second dimension
   * @param[in] dim3_offset  offset into third dimension
   * @param[in] min_grid  grid with minimum values
   * @param[in] max_grid  grid with maximum values
   * @param[out] min_value  minimum value for given offsets
   * @param[out] max_value  maximum value for given offsets
   */
  void get_min_max_climate_values(size_t dim1, size_t dim2, size_t dim3, size_t dim1_offset, size_t dim2_offset, size_t dim3_offset, const vector< vector< vector<float> > > &min_grid, const vector< vector< vector<float> > > &max_grid, short *min_value, short *max_value) const;

  void get_min_max_climate_values(size_t dim1, size_t dim2, size_t dim3, size_t dim1_offset, size_t dim2_offset, size_t dim3_offset, const vector< vector< vector<float> > > &min_grid, const vector< vector< vector<float> > > &max_grid, float *min_value, float *max_value) const;


protected:

  /** @brief error string */
  string error_str;

  /** @brief month vector */
  vector<int> month;

  /** @brief size of month vector */
  size_t month_size;

  /** @brief latitude vector */
  vector<float> latitude;

  /** @brief size of latitude vector */
  size_t latitude_size;

  /** @brief longitude vector */
  vector<float> longitude;

  /** @brief size of longitude vector */
  size_t longitude_size;

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
  vector< vector< vector<float> > > min_air_temperature_grid;

  /** @brief maximum air temperature vector */
  vector<float> max_air_temperature;

  /** @brief maximum air temperature grid vector */
  vector< vector< vector<float> > > max_air_temperature_grid;

  /** @brief minimum atmospheric pressure vector */
  vector<float> min_atmospheric_pressure;

  /** @brief minimum atmospheric pressure grid vector */
  vector< vector< vector<float> > > min_atmospheric_pressure_grid;

  /** @brief maximum atmospheric pressure vector */
  vector<float> max_atmospheric_pressure;

  /** @brief maximum atmospheric pressure grid vector */
  vector< vector< vector<float> > > max_atmospheric_pressure_grid;

  /** @brief minimum dewpoint temperature vector */
  vector<float> min_dewpoint_temperature;

  /** @brief minimum dewpoint temperature grid vector */
  vector< vector< vector<float> > > min_dewpoint_temperature_grid;

  /** @brief maximum dewpoint temperature vector */
  vector<float> max_dewpoint_temperature;

  /** @brief maximum dewpoint temperature grid vector */
  vector< vector< vector<float> > > max_dewpoint_temperature_grid;

  /** @brief minimum surface temperature vector */
  vector<float> min_surface_temperature;

  /** @brief minimum surface temperature grid vector */
  vector< vector< vector<float> > > min_surface_temperature_grid;

  /** @brief maximum surface temperature vector */
  vector<float> max_surface_temperature;

  /** @brief maximum surface temperature grid vector */
  vector< vector< vector<float> > > max_surface_temperature_grid;

  /** @brief assignment operator */
  vdt_climatology_file_reader_test & operator=(const vdt_climatology_file_reader_test &vdt_climatology_file_reader_test);

  /** @brief constructor */
  vdt_climatology_file_reader_test(const vdt_climatology_file_reader &vdt_climatology_file_reader_test);

};

#endif /* VDT_CLIMATOLOGY_FILE_READER_H */
