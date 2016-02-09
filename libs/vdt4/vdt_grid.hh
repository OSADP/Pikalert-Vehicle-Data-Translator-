//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_grid.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_grid.hh
 *
 * @class vdt_grid
 *
 *  vdt road segment class
 *
 */

#ifndef VDT_GRID_HH
#define VDT_GRID_HH


#include <string>
#include <iostream>
#include <vector>
#include "vdt_point.hh"

using std::string;
using std::vector;

/**
 * @class vdt_grid
 * Represents a road segment, defined by:
 *   two pairs of latitude, longitude - the begin and end points
 *   a unique identifier, name, highway symbol, etc.
 */
class vdt_grid
{
public:

  /** @brief constructor */
  vdt_grid(int latitude_size_arg, int longitude_size_arg, double lower_left_latitude_arg, double lower_left_longitude_arg, double upper_right_latitude_arg, double upper_right_longitude_arg) : latitude_size(latitude_size_arg), longitude_size(longitude_size_arg), lower_left_latitude(lower_left_latitude_arg), lower_left_longitude(lower_left_longitude_arg), upper_right_latitude(upper_right_latitude_arg), upper_right_longitude(upper_right_longitude_arg)
  {
  }


  /**
   * @brief get grid offset of grid point
   * @param[in] yoffset y offset of grid point
   * @param[in] xoffset x offset of grid point
   */
  inline size_t get_grid_offset(int yoffset, int xoffset) const
  {
    return yoffset * longitude_size + xoffset;
  }

  /**
   * @brief get grid size
   */
  inline int get_grid_size() const
  {
    return latitude_size * longitude_size;
  }

  /**
   * @brief get nearest indices to latitude longitude points
   * @param[in] latitude vector of latitude coordinates
   * @param[in] longitude vector of longitude coordinates
   * @param[out] grid_cell_offsets grid cell offsets when considered as single dimensional vector
   * @param[out] distances (in degrees using Euclidean metric) of points from grid cell center points
   * @return 0 on success, -1 on failure (this will occur if latitude.size() != longitude.sie())
   */
  int get_nearest_indices(const vector<double> &latitude, const vector<double> &longitude, vector<int> &grid_cell_offsets, vector<double> &distances) const;

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
   * @param[in] lat  latitude value
   * @param[in] lon  longitude value
   * @param[out] xoffset  longitude offset
   * @param[out] yoffset  latitude offset
   */
  void latlon2xy(double lat, double lon, double *xoffset, double *yoffset) const;

  /**
   * @brief get latitude and longitude for given xoffset and yoffset
   * @param[in] xoffset  longitude offset
   * @param[in] yoffset  latitude offset
   * @param[out] lat  latitude value
   * @param[out] lon  longitude value
   */
  void xy2latlon(double xoffset, double yoffset, double *lat, double *lon) const;

  /** @brief size of latitude vector */
  size_t latitude_size;

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


protected:

};

#endif /* VDT_GRID_HH */
