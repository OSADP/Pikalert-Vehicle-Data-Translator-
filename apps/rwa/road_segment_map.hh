//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: road_segment_map.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file road_segment_map.hh
 *
 * @class Road_Segment_Map
 *
 *  <file/class description>
 *  
 * @date 10/21/13
 */

#ifndef ROAD_SEGMENT_MAP_HH
#define ROAD_SEGMENT_MAP_HH


#include <string>
#include "rwx/rwx_vector_collection_nc.hh"

class road_segment_info
{
public:
  int seg_id;
  double mid_point_latitude;
  double mid_point_longitude;
  std::string aux_id;
  std::string seg_name;
  int point_type;
};

int create_road_segment_map(int road_segment_na, const rwx_vector_collection_nc &road_segment_collection, std::unordered_map<int, road_segment_info> &road_segment_map, string &error);

#endif /* ROAD_SEGMENT_MAP_HH */
