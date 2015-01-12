//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwx_rtma_dataset.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/08 16:56:55 $
//
//==============================================================================

/**
 * @file rwx_rtma_dataset.hh
 *
 * @class rwx_rtma_dataset
 *
 * @date 10/24/13
 */


#ifndef RWX_RTMA_DATASET_HH
#define RWX_RTMA_DATASET_HH

#include <iostream>
#include <map>
#include <string>
#include "rwx_vector_collection_nc.hh"
#include "Proj4Wrap/Proj4Wrap.hh"

using namespace std;

/**
 * @class rwx_rtma_dataset
 * VDT Real-Time Mesoscale Analysis (RTMA) dataset class
 */
class rwx_rtma_dataset
{

public:

  /** @brief  pressure name */
  const static string PRESSURE;

  /** @brief  temperature name */
  const static string TEMPERATURE;

  /** @brief  dew point temperature name */
  const static string DEWPOINT;

  /** @brief  humidity name */
  const static string HUMIDITY;

  /** @brief  wind direction name */
  const static string WIND_DIR;

  /** @brief  wind speed name */
  const static string WIND_SPEED;

  /**
   * @brief constructor
   * @param[in] fname  file name
   * @param[in] logg   log file
   */
  rwx_rtma_dataset(const std::string &input_file_arg, const p4w::Proj4Wrap *rtma_proj_arg);
  ~rwx_rtma_dataset();


  /**
   * @brief  get dewpoint in celsius
   * @param[in] lat latitude
   * @param[in] lon longitude
   * @param[out] value value at grid point if in grid otherwise unspecified
   * @return 0 if lat, lon are in grid, 1 if not
   */
  int get_dewpoint_celsius(double lat, double lon, float &value);

  /**
   * @brief  get temperature in celsius
   * @param[in] lat latitude
   * @param[in] lon longitude
   * @param[out] value value at grid point if in grid otherwise unspecified
   * @return 0 if lat, lon are in grid, 1 if not
   */
  int get_temperature_celsius(double lat, double lon, float &value);

  /**
   * @brief  get pressure
   * @param[in] lat latitude
   * @param[in] lon longitude
   * @param[out] v  value
   * @return 0 on success, 1 on failure
   */
  int get_pressure_mb(double lat, double lon, float &value);

  /**
   * @brief checks whether coordinates are in grid
   * @param[in] x x coordinate
   * @param[in] y y coordinate
   * @return true, false 
   */
  inline bool in_grid(size_t x, size_t y)
  {
    if (0 <= x && x < xdim)
      {
	if (0 <= y && y < ydim)
	  {
	    return true;
	  }
      }

    return false;
  }

  /**
   * @brief gets linear offset of x, y coordinates in underlying grid vectors
   * @param[in] x x coordinate
   * @param[in] y y coordinate
   * @return linear offset
   */
  size_t get_xy_offset(size_t x, size_t y);

  string error;

private:
  bool yfirst;			// y dimension is first and x is second for gridded variables
  size_t xdim;
  size_t ydim;
  string input_file;
  const p4w::Proj4Wrap *rtma_proj;
  const vector<float> *pressure_field;
  const vector<float> *temperature_field;
  const vector<float> *dewpoint_field;
  rwx_vector_collection_nc *collection;

  /**
   * @brief  get value
   * @param[in] lat latitude
   * @param[in] lon longitude
   * @param[in] k key to band_index map
   * @param[out] v  value
   * @return 0 on success, 1 on failure
   */
  int get_val(double lat, double lon, float& v, string k);

};

#endif /* RWX_RTMA_DATASET_HH */
