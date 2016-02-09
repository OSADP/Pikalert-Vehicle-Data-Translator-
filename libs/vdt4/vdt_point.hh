//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_point.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_point.hh
 *
 * @class vdt_point
 *
 *  vdt point class
 *
 */

#ifndef VDT_POINT_HH
#define VDT_POINT_HH

#include <vector>
#include <iostream>
#include <string>

using std::ostream;
using std::string;
using std::vector;

/**
 * @class vdt_point
 * Contains latitude, longitude and elevation of a point of interest.
 * Inherits functionality of GDAL OGRPoint.
 */
//class vdt_point : public OGRPoint
class vdt_point
{

public:

  /** @brief constructor */
  vdt_point();

  /** @brief constructor */
  vdt_point(double latitude_value, double longitude_value, double elevation_value) : latitude(latitude_value), longitude(longitude_value), elevation(elevation_value)
  {
  }

  /** @brief get latitude
   * @return  latitude value
   */
  inline const double get_latitude() const
  {
    return latitude;
  }

  /** @brief get longitude
   * @return longitude value
   */
  inline const double get_longitude() const
  {
    return longitude;
  }

  /** @brief get elevation
   * @return elevation  value
   */
  inline const float get_elevation() const
  {
    return elevation;
  }

  /** @brief set latitude
   * @param[in] value  latitude value
   */
  inline void set_latitude(double value)
  {
    latitude = value;
  }

  /** @brief set longitude
   * @param[in] value  longitude value
   */
  inline void set_longitude(double value)
  {
    longitude = value;
  }

  /** @brief set elevation
   * @param[in] value  elevation value
   */
  void set_elevation(float value)
  {
    elevation = value;
  }
  
  /** @brief get_points_lle
   * gets the latitudes, longitudes, and elevations of the given object
   * @param[in] objects     vector of objects
   * @param[out] latitude   vector of latitudes for a given vdt point
   * @param[out] longitude  vector of longitudes for a given vdt point
   * @param[out] elevation  vector of elevations for a given vdt point
   */
  template<class T>
  static void get_points_lle(const vector<T> &objects, vector<double> &latitude, vector<double> &longitude, vector<double> &elevation)
  {
    for (size_t i=0; i<objects.size(); i++)
      {
	vdt_point point = objects[i].get_point();
	latitude.push_back(point.get_latitude());
	longitude.push_back(point.get_longitude());
	elevation.push_back(point.get_elevation());
	//These have valid values
	//printf("latitude, longitude: %f, %f\n", point.get_latitude(), point.get_longitude());
      }
  }
 
  /** @brief get_points
   * gets the latitude, longitude, and elevation for a  vdt point
   * @param[in] objects   vector of objects
   * @param[out] points   vector of vdt points
   */
  template<class T>
  static void get_points(const vector<T> &objects, vector<vdt_point> &points)
  {
    for (size_t i=0; i<objects.size(); i++)
      {
	vdt_point point = objects[i].get_point();
	points.push_back(point);
      }
  }

  /** @brief GRC_EARTH_RADIUS radius of the earth in km */
  static const double GRC_EARTH_RADIUS;

  /** @brief convert degrees to radians
   * @param degrees input degrees
   * @return radians
   */
  static double radians(double degrees);

  /** @brief great circle distance between two lat, long points
   * @param[in] lat1 latitude of first point
   * @param[in] lon1 longitude of first point
   * @param[in] lat2 latitude of second point
   * @param[in] lon2 longitude of second point
   * @return distance between two points
   */
  static double great_circle_distance(double lat1, double lon1, double lat2, double lon2);

  inline static bool check_lat_lon(double raw_lat, double raw_lon)
  {
    if (raw_lat > 0 && raw_lat < 90.)
      {
	if (raw_lon >= -180. && raw_lon <= 180. && raw_lon != 0)
	  {
	    return true;
	  }
      }
    return false;
  }
    

  /** @brief insertion operator */
  friend ostream& operator<<(ostream &ostr, const vdt_point &point);

  /** @brief supports vdt_point as an array */
  const double operator[](const int n);

  /** @brief equality operator */
  bool operator==(const vdt_point& o);

  /** @brief inequality operator */
  bool operator!=(const vdt_point& o);

  double latitude;
  double longitude;
  float elevation;
};


#endif
