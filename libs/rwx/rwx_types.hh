//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwx_types.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/02/13 22:14:18 $
//
//==============================================================================

/**
 * @file rwx_types.hh
 *
 * @class Rwx_Types
 *
 *  <file/class description>
 *  
 * @date 9/16/13
 */

#ifndef RWX_TYPES_HH
#define RWX_TYPES_HH

#include <string>

namespace rwx
{

  class value_t
  {
  public:
    int v_type;			// one of NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT or NC_DOUBLE
    std::string v_string;
    short v_short;
    int v_int;
    float v_float;
    double v_double;

    int get_value(double *value_ptr);
  };

  struct vehicle_t
  {
    std::string id;
    double lat;
    double lon;
    int obs_time;
    int speed_mph;
    int heading_deg;
    std::string wipers;
    float temp_f;
    float road_temp_f;
    float wind_speed_ms;
  };

  struct district_t
  {
    float min_lat;
    float max_lat;
    float min_lon;
    float max_lon;
    std::string district_name;
    std::string display_name;
  };
};

#endif /* RWX_TYPES_HH */
