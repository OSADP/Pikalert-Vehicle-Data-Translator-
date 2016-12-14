//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
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

#ifndef VDT_PROBE_MESSAGE_DATASETS_H
#define VDT_PROBE_MESSAGE_DATASETS_H

#include <iostream>
#include <functional>
#include <libconfig.h++>
#include "config_reader.hh"
#include "vdt4/vdt_nssl_dataset.hh"
#include "vdt4/vdt_dual_pol_dataset.hh"
#include "rwx/rwx_rtma_dataset.hh"
#include "rwx/rwx_ak_rtma_dataset.hh"
#include "vdt4/vdt_cloud_mask_dataset.hh"
#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_surface_observation.hh"
#include "vdt4/vdt_nearest_nbr.hh"
#include "vdt4/vdt_point.hh"
#include "vdt4/vdt_dataset.hh"
#include "log/log.hh"
#include <string>

using namespace std;
using namespace libconfig;

/**
 * @class vdt_probe_message_datasets
 * handles the various datasets used with vdt probe messages including radar, model, cloud mask, nearest surface station fields, etc.
 */

class vdt_probe_message_datasets
{

public:

  vdt_probe_message_datasets()
  {
    cfg_reader = 0;
  }

  /**
   * @brief vdt_probe_message_dataset constructor
   *
   * @param[in] cfg config reader object holding configuration information
   * @param[in] radar_file_pattern  input file pattern for files with radar data
   * @param[in] radar_file_pattern_cref  input file pattern for files with composite radar data
   * @param[in] radar_file_pattern_dual_pol_hc  input file pattern for files with level 3 classification radar data
   * @param[in] radar_file_pattern_dual_pol_hr  input file pattern for files with level 3 reflectivity radar data
   * @param[in] metar_file  input file with metar data
   * @param[in] rwis_file  input file with rwis data
   * @param[in] rtma_file  input file wiith rtma data
   * @param[in] is_alaska true if state is alaska
   * @param[in] old_radar true if radar is old format
   * @param[in] cloud_class_file  input file with cloud classification data
   * @param[in] logg  file handle for log output
   */
  vdt_probe_message_datasets(const config_reader *cfg,
			     string radar_file_pattern,
			     string radar_file_pattern_cref,
			     string radar_file_pattern_dual_pol_hc,
			     string radar_file_pattern_dual_pol_hr,
			     float radar_rad,
                             string metar_file,
                             string rwis_file,
                             string rtma_file,
                             string cloud_class_file,
			     bool is_alaska,
			     bool old_radar,
                             Log* logg = NULL);
  /**
   * @brief vdt_probe_message_dataset destructor
   */
  virtual ~vdt_probe_message_datasets();

  /**
   * @brief  Sets up the vector of vdt_probe_message_qc objects with vdt dataset information,
   *         including radar, model, cloud mask, nearest surface station fields, etc.
   *
   * @param[in] cfg  config reader object holding configuration information
   * @param[in] in_msgs  vector of input probe messages
   * @param[out] out_msgs  vector of input probe messages
   */
  virtual void update_probe_messages(const config_reader &cfg, const vector<vdt_probe_message> &in_msgs, vector<vdt_probe_message_qc> &out_msgs);

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
   * @brief  Gets vdt dataset information, including radar, model, cloud mask, nearest surface
   *         station fields, etc. for a given lat, lon, and obstime
   *
   * @param[in] target  point containing lat, lon and height
   * @param[in] obstime  observation time
   * @return  vdt_datasets object
   */
  vdt_dataset get_vdt_dataset(double obstime, const vdt_point &target);

  //vdt_probe_message_datasets & operator=(const vdt_probe_message_datasets &vdt_probe_message_datasets);

  //vdt_probe_message_datasets(const vdt_probe_message_datasets &vdt_probe_message_datasets);


protected:

  /** @brief state is alaska? */
  bool alaska;

  /** @brief cfg_reader configuration reader object */
  const config_reader *cfg_reader;

  /** @brief surface_station_latitude surface station latitude */
  vector<double> surface_station_latitude;

  /** @brief surface_station_longitude surface station longitude */
  vector<double> surface_station_longitude;

  /** @brief surface_station_elevation surface station elevation */
  vector<double> surface_station_elevation;

  /** @brief radar  pointer to a bref vdt_nssl_radar_dataset object */
  vdt_radar_dataset  *radar;

  /** @brief radar  pointer to a cref vdt_nssl_radar_dataset object */
  vdt_radar_dataset  *radar_cref;

  /** @brief radar  pointer to a level 3 classification vdt_dual_pol_radar_dataset object */
  vdt_dual_pol_tiled_radar_dataset  *radar_dual_pol_hc;

  /** @brief radar  pointer to a level 3 classification vdt_dual_pol_radar_dataset object */
  vdt_dual_pol_tiled_radar_dataset  *radar_dual_pol_hr;  

  /** @brief rtma  pointer to an rwx_rtma_dataset object */
  rwx_rtma_dataset *rtma;

  /** @brief rtma  pointer to an rwx_ak_rtma_dataset object */
  rwx_ak_rtma_dataset *ak_rtma;

  /** @brief cloud_class  pointer to a vdt_cloud_mask_dataset object */
  vdt_cloud_mask_dataset *cloud_class;

  /** @brief obs  vector of vdt_surface_observation objects */
  vector<vdt_surface_observation> obs;

  /** @brief surface_obs_nearest_nbr surface observations nearest neighbor object */
  vdt_nearest_nbr *surface_obs_nearest_nbr;

  /** @brief logg  pointer to a log object */
  Log *logg;
};

#endif
