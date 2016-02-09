//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_file_reader.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013-11-14 18:30:30 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_file_reader.hh
 *
 * @class vdt_road_segment_file_reader
 *
 *  vdt road segment file reader class
 *
 */

#ifndef VDT_ROAD_SEGMENT_FILE_READER_H
#define VDT_ROAD_SEGMENT_FILE_READER_H

#include <unordered_map>
#include <string>
#include <vector>

#include "vdt_road_segment.hh"
#include "vdt_point.hh"

using std::string;
using std::unordered_map;
using std::vector;

using namespace std;

/**
 * @class vdt_road_segment_file_reader
 * Class for reading road segment netcdf files
 */
class vdt_road_segment_file_reader
{
  std::string make_name_info_key(size_t index);
public:

  /** @brief latitude netcdf variable name */
  const static string LATITUDE;

  /** @brief longitude netcdf variable name */
  const static string LONGITUDE;

  /** @brief elevation netcdf variable name */
  const static string ELEVATION;

  /** @brief road segment name netcdf variable name */
  const static string SEG_NAME;

  /** @brief road segment id netcdf variable name */
  const static string SEG_ID;

  /** @brief corresponding road weather forecast system id netcdf variable name */
  const static string RWFS_ID;

  /** @brief corresponding auxillary road weather forecast system id netcdf variable name */
  const static string AUX_ID;
  
  /** @brief road type netcdf variable name */
  const static string ROAD_TYPE;

  /** @brief number of records netcdf dimension name */
  const static string REC_NUM;

  /** @brief road segment name length netcdf dimension name */
  const static string SEG_NAME_LEN;

  /** @brief rwfs id length netcdf dimension name */
  const static string RWFS_ID_LEN;
  
  /** @brief aux id length netcdf dimension name */
  const static string AUX_ID_LEN;
  
  /** @brief point type netcdf variable name */
  const static string POINT_TYPE;

  /** @brief road type netcdf variable name */
  const static string ROAD_TYPE_LEN;

  /** @brief speed limit netcdf variable name */
  const static string SPEED_LIMIT_MPH;

  string error() { return error_msg;}

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] logg   log file
   */
  vdt_road_segment_file_reader(const string &fpath);

  ~vdt_road_segment_file_reader();

  /**
   * @brief establish_road_segments()
   * Establishes road segment and rwis vectors
   */
  void establish_road_segments();

  /**
   * @brief  gets road segments
   * @return road segment vector
   */
  const vector<vdt_road_segment> & get_road_segments() const
  {
    return road_segments;
  }
  
  /**
   * @brief  gets rwis site
   * @return rwis site vector
   */
  const vector<vdt_road_segment> & get_rwis_sites() const
  {
    return rwis_sites;
  }

  /**
   * @brief  get road elevations
   * @return road elevations
   */
  const vector<float> & get_road_elevations() const
  {
    return road_elevation;
  }
  
  /**
   * @brief  get road latitudes
   * @return road latitudes
   */
  const vector<double> & get_road_latitudes() const
  {
    return road_latitude;
  }
  
  /**
   * @brief  get road longitudes
   * @return road longitudes
   */
  const vector<double> & get_road_longitudes() const
  {
    return road_longitude;
  }
  
  /**
   * @brief  get rwis latitudes
   * @return rwis latitudes
   */
  const vector<float> & get_rwis_elevations() const
  {
    return rwis_elevation;
  }
  
  /**
   * @brief  get rwis latitudes
   * @return rwis latitudes
   */
  const vector<double> & get_rwis_latitudes() const
  {
    return rwis_latitude;
  }
  
  /**
   * @brief  get rwis longitudes
   * @return rwis longitudes
   */
  const vector<double> & get_rwis_longitudes() const
  {
    return rwis_longitude;
  }
  
  /**
   * @brief  get latitude longitude point index to road segment index
   * @return point index to road segment index map
   */
  const unordered_map<size_t, size_t> & get_point_index_to_segment_index_map() const
  {
    return point_index_to_segment_index;
  }
  

private:
  string error_msg;
  vector<float> elevation;
  string elevation_units;
  float elevation_missing;
  vector<double> latitude;
  string latitude_units;
  double latitude_missing;
  vector<double> longitude;
  string longitude_units;
  double longitude_missing;
  vector<int> point_types;
  vector<string> segment_name;
  vector<int> segment_id;
  vector<string> rwfs_id;
  vector<string> aux_id;
  vector<float> speed_limit;
  string speed_limit_units;
  float speed_limit_missing;
  vector<string> road_type;
  vector<float> road_elevation;
  vector<double> road_latitude;
  vector<double> road_longitude;
  vector<float> rwis_elevation;
  vector<double> rwis_latitude;
  vector<double> rwis_longitude;
  vector<vdt_road_segment> road_segments;
  vector<vdt_road_segment> rwis_sites;
  unordered_map<size_t, size_t> point_index_to_segment_index;
};

#endif /* VDT_ROAD_SEGMENT_FILE_READER_H */
