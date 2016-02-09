

#include "vdt_read_cdf_variables.hh"
#include "vdt_road_segment_file_reader.hh"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf>

using namespace netCDF;
using namespace netCDF::exceptions;


const string vdt_road_segment_file_reader::LATITUDE = "latitude";
const string vdt_road_segment_file_reader::LONGITUDE = "longitude";
const string vdt_road_segment_file_reader::ELEVATION = "elevation";
const string vdt_road_segment_file_reader::SPEED_LIMIT_MPH = "speed_mph";
const string vdt_road_segment_file_reader::SEG_NAME = "seg_name";
const string vdt_road_segment_file_reader::SEG_ID = "seg_id";
const string vdt_road_segment_file_reader::RWFS_ID = "rwfs_id";
const string vdt_road_segment_file_reader::AUX_ID = "aux_id";
const string vdt_road_segment_file_reader::ROAD_TYPE = "road_type";
const string vdt_road_segment_file_reader::POINT_TYPE = "point_type";
const string vdt_road_segment_file_reader::REC_NUM = "point_num";
const string vdt_road_segment_file_reader::SEG_NAME_LEN = "seg_name_len";
const string vdt_road_segment_file_reader::RWFS_ID_LEN = "rwfs_id_len";
const string vdt_road_segment_file_reader::AUX_ID_LEN = "aux_id_len";
const string vdt_road_segment_file_reader::ROAD_TYPE_LEN = "road_type_len";


vdt_road_segment_file_reader::vdt_road_segment_file_reader(const string &file_name)
{
  error_msg = "";

  try
    {
      // Open the file and check to make sure it's valid.
      NcFile data_file(file_name, NcFile::read);

      if (read_variable(data_file, vdt_road_segment_file_reader::ELEVATION, "units", "", elevation_units, elevation_missing, elevation, error_msg) < 0)
	{
	  return;
	}

      if (read_variable(data_file, vdt_road_segment_file_reader::LATITUDE, "units", "", latitude_units, latitude_missing, latitude, error_msg) < 0)
	{
	  return;
	}

      if (read_variable(data_file, vdt_road_segment_file_reader::LONGITUDE, "units", "", longitude_units, longitude_missing, longitude, error_msg) < 0)
	{
	  return;
	}

      if (read_string_var(data_file, vdt_road_segment_file_reader::SEG_NAME, segment_name, error_msg) < 0)
	{
	  return;
	}

      if (read_string_var(data_file, vdt_road_segment_file_reader::RWFS_ID, rwfs_id, error_msg) < 0)
	{
	  return;
	}

      if (read_string_var(data_file, vdt_road_segment_file_reader::AUX_ID, aux_id, error_msg) < 0)
	{
	  return;
	}

      string bogus_string;
      int bogus_int;
      if (read_variable(data_file, vdt_road_segment_file_reader::SEG_ID, "", "", bogus_string, bogus_int, segment_id, error_msg) < 0)
	{
	  return;
	}
      
      if (read_variable(data_file, vdt_road_segment_file_reader::POINT_TYPE, "", "", bogus_string, bogus_int, point_types, error_msg) < 0)
	{
	  return;
	}

      if (read_string_var(data_file, vdt_road_segment_file_reader::ROAD_TYPE, road_type, error_msg) < 0)
	{
	  return;
	}

      if (read_variable(data_file, vdt_road_segment_file_reader::SPEED_LIMIT_MPH, "units", "", speed_limit_units, speed_limit_missing, speed_limit, error_msg) < 0)
	{
	  return;
	}

      establish_road_segments();
    }
  catch (NcException e)
    {
      // Capture exception in error_msg by redirecting cout
      error_msg = e.what();
    }

  return;
}

struct Seg_info
{
  size_t index;
  int point_type;
  vector<vdt_point> points;
};

std::string vdt_road_segment_file_reader::make_name_info_key(size_t index)
{
  return segment_name[index] + std::string(" ") + std::to_string(segment_id[index]);
}

void vdt_road_segment_file_reader::establish_road_segments()
{
  unordered_map<string, Seg_info > seg_name_info_map;

  for (size_t i = 0; i < point_types.size(); i++)
    {
      // Road segments have a point type = 0 and rwis have a point type = 1
      if (point_types[i] == 0)
	{
	  road_elevation.push_back(elevation[i]);
	  road_latitude.push_back(latitude[i]);
	  road_longitude.push_back(longitude[i]);
	}
      else
	{
	  rwis_elevation.push_back(elevation[i]);
	  rwis_latitude.push_back(latitude[i]);
	  rwis_longitude.push_back(longitude[i]);
	}

      // Copy segment point
      vdt_point pt;
      pt.set_elevation(elevation[i]);
      pt.set_latitude(latitude[i]);
      pt.set_longitude(longitude[i]);

      string name_str = segment_name[i];
      auto it = seg_name_info_map.find(make_name_info_key(i));
      if (it == seg_name_info_map.end())
	{
	  // Establish new segment
	  // Store in road segments or rwis vector
	  // Record segment information in seg_name_info_map.
	  Seg_info seg_info;
	  seg_info.points.push_back(pt);

	  vdt_road_segment seg;

	  seg.set_road_name(name_str);
	  seg.set_rwfs_id(rwfs_id[i]);
	  seg.set_aux_id(aux_id[i]);
	  seg.set_id(segment_id[i]);
	  seg.set_road_type(road_type[i]);
	  seg.set_speed_limit(speed_limit[i]);

	  if (point_types[i] == 0)
	    {
	      seg_info.index = road_segments.size();
	      point_index_to_segment_index[i] = seg_info.index;
	      seg_info.point_type = point_types[i];
	      road_segments.push_back(seg);
	    }
	  else
	    {
	      seg_info.index = rwis_sites.size();
	      seg_info.point_type = point_types[i];
	      rwis_sites.push_back(seg);
	    }

	  seg_name_info_map[make_name_info_key(i)] = seg_info;
	}
      else
	{
	  // New segment has already been established
	  // Store new lat-long point in appropriate segment into structure
	  it->second.points.push_back(pt);
	  point_index_to_segment_index[i] = road_segments.size()-1;
	}
    }

  // Iterate through seg_name_info_map and set all the points for each road segment or rwis site
  for (auto itr=seg_name_info_map.begin(); itr!=seg_name_info_map.end(); ++itr)
    {
      if (itr->second.point_type == 0)
	{
	  road_segments[itr->second.index].set_points(itr->second.points);
	}
      else
	{
	  rwis_sites[itr->second.index].set_points(itr->second.points);
	}
    }

  return;
}

vdt_road_segment_file_reader::~vdt_road_segment_file_reader()
{
}
