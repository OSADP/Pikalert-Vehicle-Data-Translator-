//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013-08-13 21:55:03 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment.hh
 *
 * @class vdt_road_segment
 *
 *  vdt road segment class
 *
 */

#ifndef VDT_ROAD_SEGMENT_HH
#define VDT_ROAD_SEGMENT_HH


#include <string>
#include <iostream>
#include <vector>
#include "vdt_point.hh"

using std::string;
using std::vector;

/**
 * @class vdt_road_segment
 * Represents a road segment, defined by:
 *   two pairs of latitude, longitude - the begin and end points
 *   a unique identifier, name, highway symbol, etc.
 */
class vdt_road_segment
{
public:

  /** @brief constructor */
  vdt_road_segment();


  /** @brief destructor */
  ~vdt_road_segment();

  /** @brief relational operator */
  bool operator<(const vdt_road_segment &road_segment) const
  {
    return (road_segment.id < id);
  }

  /** @brief equality operator */
  bool operator==(const vdt_road_segment &road_segment) const
  {
    return (road_segment.id == id);
  }

  /**
   * @brief  get vdt points
   * @return vdt points
   */
  inline const vector<vdt_point> & get_points() const
  {
    return points;
  }

  /**
   * @brief  get begin vdt point value
   * @return begin vdt point value
   */
  inline const vdt_point get_begin_point() const
  {
    return begin_point;
  }

  /**
   * @brief  get mid vdt point value
   * @return mid vdt point value
   */
  inline const vdt_point get_mid_point() const
  {
    return mid_point;
  }

  /**
   * @brief  get vdt point value (this is helpful for generic extraction of mid points using get_point_lle()
   * @return mid vdt point value
   */
  inline const vdt_point get_point() const
  {
    return mid_point;
  }

  /**
   * @brief  get end vdt point value
   * @return end vdt point value
   */
  inline const vdt_point get_end_point() const
  {
    return end_point;
  }
  
  /**
   * @brief  get road segment identifier value
   * @return road segment identifier value
   */
  inline const int get_id() const
  {
    return id;
  }
  
  /**
   * @brief  get rwfs id value
   * @return road segment corresponding rwfs id value
   */
  inline const string get_rwfs_id() const
  {
    return rwfs_id;
  }
  
  /**
   * @brief  get aux id value
   * @return road segment corresponding aux id value
   */
  inline const string get_aux_id() const
  {
    return aux_id;
  }
  
  /**
   * @brief  get road type value
   * @return road type value
   */
  inline const string get_road_type() const
  {
    return road_type;
  }
  
  /**
   * @brief  get road segment name value
   * @return road segment name value
   */
  inline const string get_road_name() const
  {
    return road_name;
  }

  /**
   * @brief  get class route value
   * @return class route value
   */
  inline const int get_route_class() const
  {
    return route_class;
  }

  /**
   * @brief  get highway type value
   * @return highway type value
   */
  inline const string get_hwy_type() const
  {
    return hwy_type;
  }

  /**
   * @brief  get highway symbol value
   * @return highway symbol value
   */
  inline const string get_hwy_symbol() const
  {
    return hwy_symbol;
  }

  /**
   * @brief  get speed limit value
   * @return speed limit value
   */
  inline const float get_speed_limit() const
  {
    return speed_limit;
  }

  /**
   * @brief  get road segment length value
   * @return road segment length value
   */
  inline const float get_length() const
  {
    return length;
  }

  /**
   * @brief set vdt points
   * @param[in] point_vector  vdt_points
   */
  void set_points(const vector<vdt_point> &point_vector);

  /**
   * @brief set begin vdt point value
   * @param[in] value begin vdt point value
   */
  inline void set_begin_point(const vdt_point &value)
  {
    begin_point = value;
  }

  /**
   * @brief set mid vdt point value
   * @param[in] value mid vdt point value
   */
  inline void set_mid_point(const vdt_point &value)
  {
    mid_point = value;
  }

  /**
   * @brief set end vdt point value
   * @param[in] value end vdt point value
   */
  inline void set_end_point(const vdt_point &value)
  {
    end_point = value;
  }

  /**
   * @brief set road segment identifier value
   * @param[in] value road segment identifier value
   */
  inline void set_id(int value)
  {
    id = value;
  }

  /**
   * @brief set rwfs id value
   * @param[in] value road segment corresponding rwfs id value
   */
  inline void set_rwfs_id(const string &value)
  {
    rwfs_id = value;
  }
  
  /**
   * @brief set aux id value
   * @param[in] value road segment corresponding aux id value
   */
  inline void set_aux_id(const string &value)
  {
    aux_id = value;
  }
  
  /**
   * @brief set road type value
   * @param[in] value road type value
   */
  inline void set_road_type(const string &value)
  {
    road_type = value;
  }

  /**
   * @brief set road segment name value
   * @param[in] value road segment name value
   */
  inline void set_road_name(const string &value)
  {
    road_name = value;
  }
  
  /**
   * @brief set class route value
   * @param[in] value class route value
   */
  inline void set_route_class(int value)
  {
    route_class = value;
  }

  /**
   * @brief set highway type value
   * @param[in] value highway type value
   */
  inline void set_hwy_type(const string &value)
  {
    hwy_type = value;
  }

  /**
   * @brief set highway symbol value
   * @param[in] value highway symbol value
   */
  inline void set_hwy_symbol(const string &value)
  {
    hwy_symbol = value;
  }

  /**
   * @brief set speed limit value
   * @param[in] value speed limit value
   */
  inline void set_speed_limit(float value)
  {
    speed_limit = value;
  }

  /**
   * @brief set road segment length value
   * @param[in] value road segment length value
   */
  inline void set_length(float value)
  {
    length = value;
  }

   /**
    * @brief set mid vdt point value
    */
   void set_mid_point();


  /** @brief stream insertion operator */
  friend ostream& operator<<(ostream &ostr, const vdt_road_segment &road_segment);

protected:

  /** @brief vector of vdt_points */
  vector<vdt_point> points;

  /** @brief vdt_point (latitude, longitude, and elevation) of the road segment begin point */
  vdt_point begin_point;

  /** @brief vdt_point (latitude, longitude, and elevation) of the road segment mid point */
  vdt_point mid_point;

  /** @brief vdt_point (latitude, longitude, and elevation) of the road segment end point*/
  vdt_point end_point;

  /** @brief road segment identifier */
  int id;
  
  /** @brief corresponding road-weather-forecast-system (rwfs) id  */
  string rwfs_id;
  
  /** @brief corresponding auxillary road-weather-forecast-system (rwfs) id  */
  string aux_id;
  
  /** @brief road type */
  string road_type;

  /** @brief road segment name */
  string road_name;

  /** @brief class route */
  int route_class;

  /** @brief highway type */
  string hwy_type;

  /** @brief highway symbol */
  string hwy_symbol;

  /** @brief speed limit */
  float speed_limit;

  /** @brief road segment length */
  float length;

};

#endif /* VDT_ROAD_SEGMENT_HH */
