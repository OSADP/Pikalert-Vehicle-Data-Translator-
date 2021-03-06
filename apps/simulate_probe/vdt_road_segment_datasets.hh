//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright �2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

/**
 *
 * @file vdt_probe_message_datasets.hh
 *
 * @class vdt_probe_message_datasets
 *
 * Class to handle met data
 *
 */

#ifndef VDT_ROAD_SEGMENT_DATASETS_H
#define VDT_ROAD_SEGMENT_DATASETS_H

#include <iostream>
#include <functional>
#include <libconfig.h++>
#include <log/log.hh>
#include "config_reader.hh"
#include "vdt4/vdt_nssl_dataset.hh"
#include "rwx/rwx_rtma_dataset.hh"
#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_surface_observation.hh"
#include "vdt4/vdt_nearest_nbr.hh"
#include "vdt4/vdt_point.hh"
#include "vdt4/vdt_road_segment.hh"
#include "vdt4/vdt_road_segment_data.hh"
#include "vdt4/vdt_dataset.hh"
#include <string>

using namespace std;
using namespace libconfig;

/**
 * @class road_segment_datasets
 * handles the various datasets used with the probe message simulator including radar, rtma model, nearest surface station fields, etc.
 */

class vdt_road_segment_datasets
{

public:

  /**
   * @brief vdt_road_segment_datasets constructor
   *
   * @param[in] cfg config reader object holding configuration information
   * @param[in] radar_file_pattern  input file pattern for files with radar data
   * @param[in] metar_file  input file with metar data
   * @param[in] rwis_file  input file with rwis data
   * @param[in] rtma_file  input file wiith rtma data
   */
  vdt_road_segment_datasets(const config_reader &cfg, const string &radar_file_pattern, const string &metar_file, const string &rwis_file, const string &rtma_file, Log *log_object);
  
  /**
   * @brief vdt_probe_message_dataset destructor
   */
  virtual ~vdt_road_segment_datasets();

  /**
   * @brief  Sets up the vector of vdt_probe_message_qc objects with vdt dataset information,
   *         including radar, model, nearest surface station fields, etc.
   *
   * @param[in] field_info_vec contains information about the fields using in out_segments
   * @param[in] field_name_map maps field names to offsets in out_segments
   * @param[in] in_segments  vector of input road segments
   * @param[out] out_segments  vector of output road segment data
   */
  virtual void update_road_segments(double obs_time, const Wiper_status *wiper_status, const field_info &field_info_obj, const vector<vdt_road_segment> &in_segments, vector<vdt_road_segment_data> &out_segments);
  
  /**
   * @brief  Gets the nearest surface station observations within the NSS_SEARCH_RANGE and NSS_TIME_RANGE
   *         for a given lat, lon, and obstime
   *
   * @param[in] target  point containing lat, lon and height
   * @param[in] obstime  observation time
   * @param[in] search_range  nearest surface station search range in degrees
   * @param[in] time_range  nearest surface station observations in seconds
   * @param[out] near  vector of vdt_surface_observation objects
   */
  virtual void get_near_surface_obs(const vdt_point &target, double obstime, float search_range, int time_range, vector<vdt_surface_observation> &near);

  /**
   * @brief  Gets road segment datasets for the vector of road segments for a given valid time
   *
   * @param[in] obs_time observation time
   * @param[in] vdt_points  vector of points (for example mid points of road segments or grid cells)
   * @param[out] datasets vector of datasets, one for each road segment
   */
  void get_point_datasets(double obs_time, const vector<vdt_point> &vdt_points, vector<vdt_dataset> &datasets);

  /**
   * @brief  Gets vdt dataset information, including radar, model, nearest surface
   *         station fields, etc. for a given lat, lon, and obstime
   *
   * @param[in] target  point containing lat, lon and height
   * @param[in] obstime  observation time
   * @return  vdt_datasets object
   */
  vdt_dataset get_vdt_dataset(double obstime, const vdt_point &target);

  string error;

protected:

  /** @brief cfg_reader configuration reader object */
  const config_reader &cfg_reader;

  /** @brief surface_station_latitude surface station latitude */
  vector<double> surface_station_latitude;

  /** @brief surface_station_longitude surface station longitude */
  vector<double> surface_station_longitude;

  /** @brief surface_station_elevation surface station elevation */
  vector<double> surface_station_elevation;

  /** @brief radar  pointer to a vdt_nssl_tiled_radar_dataset object */
  vdt_nssl_tiled_radar_dataset  *radar;

  /** @brief rtma  pointer to a vdt_rtma_dataset object */
  rwx_rtma_dataset *rtma;

  /** @brief obs  vector of vdt_surface_observation objects */
  vector<vdt_surface_observation> obs;

  /** @brief surface_obs_nearest_nbr surface observations nearest neighbor object */
  vdt_nearest_nbr *surface_obs_nearest_nbr;

  Log *logg;
};

#endif
