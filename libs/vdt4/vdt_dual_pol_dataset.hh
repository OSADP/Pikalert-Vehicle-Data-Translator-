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
 * @file vdt_dual_pol_dataset.hh
 *
 * @class vdt_dual_pol_dataset
 *
 *  VDT DUAL_POL (National Severe Storms Laboratory) radar dataset class
 *
 */

#ifndef VDT_DUAL_POL_DATASET_H
#define VDT_DUAL_POL_DATASET_H

#include <vector>
#include <map>
#include <netcdfcpp.h>
#include "log/log.hh"
#include "vdt_nssl_dataset.hh"

using namespace std;


//class vdt_dual_pol_tiled_radar_dataset : public vdt_radar_dataset
class vdt_dual_pol_tiled_radar_dataset
{

public:

  /** @brief dual_pol radar netcdf latitude grid spacing */
  static const float DUAL_POL_RADAR_LATITUDE_GRID_SPACING;

  /** @brief dual_pol radar netcdf longitude grid spacing */
  static const float DUAL_POL_RADAR_LONGITUDE_GRID_SPACING;  
  
  /** @brief dual_pol radar netcdf latitude attribute name */
  static const char* DUAL_POL_RADAR_NC_VAR_NAME_LATITUDE;

  /** @brief dual_pol radar netcdf longitude attribute name */
  static const char* DUAL_POL_RADAR_NC_VAR_NAME_LONGITUDE;

  /** @brief dual_pol radar netcdf latitude grid spacing attribute name */
  //static const char* DUAL_POL_RADAR_NC_ATTR_NAME_LATITUDE_GRID_SPACING;

  /** @brief dual_pol radar netcdf longitude grid spacing attribute name */
  //static const char* DUAL_POL_RADAR_NC_ATTR_NAME_LONGITUDE_GRID_SPACING;

  /** @brief dual_pol radar netcdf latitude dimension name */
  static const char* DUAL_POL_RADAR_NC_DIM_NAME_LATITUDE;

  /** @brief dual_pol radar netcdf longitude dimension name */
  static const char* DUAL_POL_RADAR_NC_DIM_NAME_LONGITUDE;

  /** @brief dual_pol radar name length dimension name */
  static const char* DUAL_POL_RADAR_NC_DIM_NAME_RADAR_NAME_LEN;
  
  /** @brief dual_pol number of radar dimension name */
  static const char* DUAL_POL_RADAR_NC_DIM_NAME_NUM_RADARS;  

  /** @brief dual_pol radar netcdf composite reflectivity variable name */
  static const char* DUAL_POL_RADAR_NC_VAR_NAME_HYDROMETEOR_CLASSIFICATION;

  /** @brief dual_pol radar netcdf base reflectivity variable name */
  static const char* DUAL_POL_RADAR_NC_VAR_NAME_DIGITAL_REFLECTIVITY;

  /** @brief dual_pol radar netcdf fill attribute name */
  static const char* DUAL_POL_RADAR_NC_VAR_ATTR_NAME_FILL;

    /** @brief dual_pol radar netcdf fill radar name */
  static const char* DUAL_POL_RADAR_NC_VAR_NAME_RADAR_NAME;

  /** @brief table to map priority of values */
  static map<int, int> DHR_priority_table;

  /**
   * @brief constructor
   * @param[in] filename  file name
   * @param[in] isClassif  says if it is a classification type radar
   * @param[in] radar_rad   the radius around the obs point to grab radar data
   * @param[in] logg   log file
   */
  vdt_dual_pol_tiled_radar_dataset(const char* filename, int isClassif, float radar_rad, Log* logg = NULL);

  /**
   * @brief constructor
   * @param[in] o  vdt dual_pol radar dateset object
   */
  vdt_dual_pol_tiled_radar_dataset(const vdt_dual_pol_tiled_radar_dataset& o);
  ~vdt_dual_pol_tiled_radar_dataset();

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
  int get_reflectivity(float lat, float lon, float& v, int& x, int& y);

  /**
   * @brief get x and y values based on a given latitude and longitude and a radius around a point
   * @param[in] lat  latitude
   * @param[in] lon  longitude
   * @param[in] radar_ind  radius around obs_point for which to accept radar inds
   * @param[out] x   x coordinate
   * @param[out] y   y coordinate
   * @return 0 on success, -1 on failure
   */
  vector<pair<int, int> > get_x_y_list(float lat, float lon, int& rad_ind);

  void corner_tests();

  float calc_mode(const vector<float> &v);

  float calc_mean(const vector<float> &v);
  
  
private:

  /** @brief netcdf file */
  NcFile* nc;

  /** @brief netcdf file */
  NcError* nc_error;

  /** @brief netcdf base reflectivity or composite reflectivity variable */
  NcVar* refl_var;

  /** @brief netcdf latitude variable */
  NcVar* lat_var;

  /** @brief netcdf longitude variable */
  NcVar* lon_var;

  /** @brief netcdf base reflectivity or composite reflectivity variable */
  NcVar* radar_name_var;

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

  /** @brief 1 if classification var, 0 otherwise */
  int isClassif;
  
  /** @brief radar radius value */
  float radar_radius;

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

  int radar_name_len;

  int num_radars;
  
  /**
   * @brief get attribute value
   * @param[in] attr_name attribute name
   * @param[out] value attribute value
   */
  //void get_attribute_value(const char* attr_name, float& value);

};


#endif /* VDT_DUAL_POL_DATASET_H */
