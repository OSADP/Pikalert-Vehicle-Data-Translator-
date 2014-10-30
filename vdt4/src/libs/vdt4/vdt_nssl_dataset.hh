//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_nssl_dataset.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/03/31 19:03:12 $
//
//==============================================================================

/**
 *
 * @file vdt_nssl_dataset.hh
 *
 * @class vdt_nssl_dataset
 *
 *  VDT NSSL (National Severe Storms Laboratory) radar dataset class
 *
 */

#ifndef VDT_NSSL_DATASET_H
#define VDT_NSSL_DATASET_H

#include <vector>
#include <netcdfcpp.h>
#include "log/log.hh"

using namespace std;

/**
 * @class vdt_radar_dataset
 * VDT radar dataset class
 */
class vdt_radar_dataset
{

public:

  /**
   * @brief get_composite_reflectivitiy
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @return composite relectivity value
   */
  virtual float get_reflectivity(float lat, float lon);
};

/**
 * @class vdt_nssl_radar_dataset
 * VDT NSSL (National Severe Storms Laboratory) radar dataset class
 */
class vdt_nssl_radar_dataset
{

public:

  /** @brief nssl radar netcdf latitude attribute name */
  static const char* NSSL_RADAR_NC_ATTR_NAME_LATITUDE;

  /** @brief nssl radar netcdf longitude attribute name */
  static const char* NSSL_RADAR_NC_ATTR_NAME_LONGITUDE;

  /** @brief nssl radar netcdf latitude grid spacing attribute name */
  static const char* NSSL_RADAR_NC_ATTR_NAME_LATITUDE_GRID_SPACING;

  /** @brief nssl radar netcdf longitude grid spacing attribute name */
  static const char* NSSL_RADAR_NC_ATTR_NAME_LONGITUDE_GRID_SPACING;

  /** @brief nssl radar netcdf latitude dimension name */
  static const char* NSSL_RADAR_NC_DIM_NAME_LATITUDE;

  /** @brief nssl radar netcdf longitude dimension name */
  static const char* NSSL_RADAR_NC_DIM_NAME_LONGITUDE;

  /** @brief nssl radar netcdf composite reflectivity variable name */
  static const char* NSSL_RADAR_NC_VAR_NAME_COMPOSITE_REFLECTIVITY;

  /** @brief nssl radar netcdf base reflectivity variable name */
  static const char* NSSL_RADAR_NC_VAR_NAME_BASE_REFLECTIVITY;

  /** @brief nssl radar netcdf scale attribute name */
  //static const char* NSSL_RADAR_NC_VAR_ATTR_NAME_SCALE;

  /** @brief nssl radar netcdf fill attribute name */
  static const char* NSSL_RADAR_NC_VAR_ATTR_NAME_FILL;

  //ED use string error instead of logg
  /**
   * @brief constructor
   * @param[in] filename  file name
   * @param[in] logg   log file
   */
  vdt_nssl_radar_dataset(const char* filename, Log* logg = NULL);

  //ED avoid single letter parameters

  /**
   * @brief constructor
   * @param[in] o  vdt nssl radar dateset object
   */
  vdt_nssl_radar_dataset(const vdt_nssl_radar_dataset& o);
  ~vdt_nssl_radar_dataset();

  /**
   * @brief  get upper left latitude
   * @return upper left latitude value
   */
  float get_upper_left_latitude()
  {
    return ul_lat;
  };

  /**
   * @brief  get upper left longitude
   * @return upper left longitude value
   */
  float get_upper_left_longitude()
  {
    return ul_lon;
  };

  /**
   * @brief  get latitude grid spacing
   * @return latitude grid spacing value
   */
  float get_latitude_grid_spacing()
  {
    return lat_space;
  };

  /**
   * @brief  get longitude grid spacing
   * @return longitude grid spacing value
   */
  float get_longitude_grid_spacing()
  {
    return lon_space;
  };

  /**
   * @brief  get longitudinal width
   * @return longitudinal width value
   */
  long get_width()
  {
    return width;
  };

  /**
   * @brief  get latitudinal height
   * @return latitudinal height value
   */
  long get_height()
  {
    return height;
  };

  /**
   * @brief get reflectivitiy value
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @param[out] v   composite reflectivitiy value
   * @return 0 on success, -1 on failure
   */
  int get_reflectivity(float lat, float lon, float& v);

  /**
   * @brief get x and y values based on a given latitude and longitude
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @param[out] x   x coordinate
   * @param[out] y   y coordinate
   * @return 0 on success, -1 on failure
   */
  int get_x_y(float lat, float lon, int& x, int& y);

private:

  /** @brief netcdf file */
  NcFile* nc;

  /** @brief file name */
  const char* filename;

  /** @brief upper left latitude */
  float ul_lat;

  /** @brief upper left longitude */
  float ul_lon;

  /** @brief upper right latitude */
  float ur_lat;

  /** @brief upper right longitude */
  float ur_lon;

  /** @brief lower right latitude */
  float lr_lat;

  /** @brief lower right longitude */
  float lr_lon;

  /** @brief lower left latitude */
  float ll_lat;

  /** @brief lower left longitude */
  float ll_lon;

  /** @brief latitude grid spacing */
  float lat_space;

  /** @brief longitude grid spacing */
  float lon_space;

  /** @brief longitudinal width */
  long width;

  /** @brief latitudinal height */
  long height;

  /** @brief reflectivity fill value */
  float ref_fill;

  /** @brief log file */
  Log* logg;

  /**
   * @brief get attribute value
   * @param[in] attr_name attribute name
   * @param[out] value attribute value
   */
  void get_attribute_value(const char* attr_name, float& value);
};

/**
 * @class vdt_nssl_tiled_radar_dataset
 * VDT NSSL (National Severe Storms Laboratory) tiled radar dataset class
 */
class vdt_nssl_tiled_radar_dataset
{

  /** @brief number of nssl tiles */
  static const int NUM_NSSL_TILES;

  /** @brief maximum file name length */
  static const int MAX_FILE_NAME_LEN;

public:

  /**
   * @brief constructor
   * @param[in] radar_file_pattern  radar file pattern
   * @param[in] logg                log file
   */
  vdt_nssl_tiled_radar_dataset(const char* radar_file_pattern, Log* logg = NULL);
  ~vdt_nssl_tiled_radar_dataset();

  /**
   * @brief get reflectivitiy value
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @param[out] v   composite reflectivitiy value
   * @return 0 on success, -1 on failure
   */
  int get_reflectivity(float lat, float lon, float& v);

private:

  /** @brief vector of vdt nssl radar dataset tiles */
  vector<vdt_nssl_radar_dataset*>* tiles;

  /** @brief log file */
  Log* logg;
};

#endif /* VDT_NSSL_DATASET_H */
