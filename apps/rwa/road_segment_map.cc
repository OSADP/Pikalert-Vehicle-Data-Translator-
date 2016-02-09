//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: road_segment_map.cc,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2013/11/23 00:48:29 $
//
//==============================================================================

/**
 * @file road_segment_map.cc
 *
 * <file description>
 *
 * @date 10/21/13
 */

// Include files 
#include <unordered_map>
#include <string>
#include <vector>
#include "road_segment_map.hh"
#include "rwx/rwx_vector_collection_nc.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int create_road_segment_map(int road_segment_na, const rwx_vector_collection_nc &road_segment_collection, std::unordered_map<int, road_segment_info> &road_segment_map, string &error)
{
  const vector<double> *mid_point_latitude_ptr;
  const vector<double> *mid_point_longitude_ptr;
  const vector<int> *seg_id_ptr;
  const vector<int> *point_type_ptr;
  const vector<char> *aux_id_ptr;

  int ret = road_segment_collection.get_vector("mid_point_latitude", &mid_point_latitude_ptr);
  if (ret != 0)
    {
      error = string("failed getting var: ") + "mid_point_latitude";
      return ret;
    }

  ret = road_segment_collection.get_vector("mid_point_longitude", &mid_point_longitude_ptr);
  if (ret != 0)
    {
      error = string("failed getting var: ") + "mid_point_longitude";
      return ret;
    }


  ret = road_segment_collection.get_vector("seg_id", &seg_id_ptr);
  if (ret != 0)
    {
      error = string("failed getting var: ") + "seg_id";
      return ret;
    }

  ret = road_segment_collection.get_vector("aux_id", &aux_id_ptr);
  if (ret != 0)
    {
      error = string("failed getting var: ") + "aux_id";
      return ret;
    }

  ret = road_segment_collection.get_vector("point_type", &point_type_ptr);
  if (ret != 0)
    {
      error = string("failed getting var: ") + "point_type";
      return ret;
    }

  const vector<double> &mid_point_latitude = *mid_point_latitude_ptr;
  const vector<double> &mid_point_longitude = *mid_point_longitude_ptr;
  const vector<int> &seg_id = *seg_id_ptr;
  const vector<char> &aux_id = *aux_id_ptr;
  const vector<int> &point_type = *point_type_ptr;

  const std::unordered_map<string, size_t> & dim_map = road_segment_collection.get_dim_map();

  auto itr = dim_map.find("aux_id_len");

  if (itr == dim_map.end())
    {
      error = string("failed finding var aux_id_len in road_segment_collection dimension map");
      return -1;
    }

  size_t aux_id_len = itr->second;
  for (size_t i=0; i<seg_id.size(); i++)
    {
      if (seg_id[i] == road_segment_na)
	{
	  continue;
	}
      auto itr = road_segment_map.find(seg_id[i]);
      if (itr == road_segment_map.end())
	{
	  road_segment_info segment_info;
	  segment_info.seg_id = seg_id[i];
	  segment_info.mid_point_latitude = mid_point_latitude[i];
	  segment_info.mid_point_longitude = mid_point_longitude[i];
	  segment_info.point_type = point_type[i];

	  // trim aux id string
	  string aux_id_string = string(&aux_id[aux_id_len * i], aux_id_len);
	  size_t aux_id_end = aux_id_string.find('\0');
	  if (aux_id_end != string::npos)
	    {
	      aux_id_string = aux_id_string.substr(0, aux_id_end);
	    }

	  segment_info.aux_id = aux_id_string;
	  road_segment_map[seg_id[i]] = segment_info;
	}
    }

  return 0;
}
