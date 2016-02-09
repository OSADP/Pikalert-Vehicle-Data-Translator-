//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: config_reader.hh,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2014/05/27 18:15:14 $
//
//==============================================================================

/**
 *
 * @file config_reader.hh
 *
 * @class config_reader
 *
 *  Configuration file reader class
 *
 */

#ifndef CONFIG_READER_HH
#define CONFIG_READER_HH

#include <string>
#include <vector>
#include "Proj4Wrap/Proj4Wrap.hh"

#include "wiper_status.hh"

using std::string;
using std::vector;

struct City_entry
{
  string city;
  double latitude;
  double longitude;
  float radius_km;
  int less_dense_num_cars;
  int more_dense_num_cars;
};

/**
 * @class config_reader
 */
class config_reader
{
public:

  /**
   * @brief config_reader constructor
   *
   * @param[in] input_file_path_arg input configuration file
   * @note the variable error is set if the constructor fails
   */
  config_reader(const string &input_file_path_arg);

  /**
   * @brief config_reader constructor
   */
  config_reader();

  /**
   * @brief config_reader destructor
   */
  ~config_reader();


  vector<City_entry> city_table;

  vector<string> field_names;
  vector<float> standard_deviation_vec;

  /** @brief input_file_path input file path to config file */
  string input_file_path;

  /** @brief climatology_file path to climatology file supporting climatology range test  */
  string climatology_file;

  /** @brief probe_message_qc_cdl_file path to probe message qc cdl file  */
  string probe_message_qc_cdl_file;

  /** @brief probe_message_qc_statistics_cdl_file path to road segment statistics cdl file  */
  string probe_message_qc_statistics_cdl_file;

  /** @brief road_segment_assessment_cdl_file path to road segment assessment cdl file  */
  string road_segment_assessment_cdl_file;

  /** @brief south_lat south latitude of the lat/lon box */
  float south_lat;

  /** @brief north_lat north latitude of the lat/lon box */
  float north_lat;

  /** @brief west_lon west longitude of the lat/lon box */
  float west_lon;

  /** @brief east_lon east longitude of the lat/lon box */
  float east_lon;

  /** @brief wiper_status wiper status information like off, intermittent, low, high */
  Wiper_status *wiper_status;

  /** @brief hourly percentages */
  vector<float> hourly_fractions;

  /** @brief interstate_highway_string interstate highway type string */
  string interstate_highway_type_string;

  /** @brief us_highway_string us highway type string */
  string us_highway_type_string;

  /** @brief state_highway_type_string state highway type string */
  string state_highway_type_string;

  /** @brief interstate_highway_fraction fraction of vehicles on interstate highways */
  float interstate_highway_fraction;

  /** @brief us_highway_fraction fraction of vehicles on us highways */
  float us_highway_fraction;

  /** @brief state_highway_fraction fraction of vehicles on state highways */
  float state_highway_fraction;

  /** @brief default_less_dense_num_cars default number of less dense cars */
  int default_less_dense_num_cars;

  /** @brief default_more_dense_num_cars default number of more dense cars */
  int default_more_dense_num_cars;

  /** @brief less_dense_file less dense road segment file */
  string less_dense_file;

  /** @brief more_dense_file more dense road segment file */
  string more_dense_file;

  /** @brief surface_temp_dev_mult surface temperature standard deviation multiplier */
  float surface_temp_dev_mult;

  /** @brief bar_press_dev_mult barometric pressure standard deviation mulitplier */
  float bar_press_dev_mult;

  /** @brief surface_station_test perform surface station test (NST) 0=no, 1=yes */
  short surface_station_test;

  /** @brief nst_search_radius neighboring surface station radius in km */
  float nst_search_radius;

  /** @brief nst_elevation_range neighboring surface station elevation range in feet */
  short nst_elevation_range;

  /** @brief nst_time_range neighboring surface station time range in seconds */
  int nst_time_range;

  /** @brief nst_qc_air_temp perform nst air temp check against metars */
  short nst_qc_air_temp;

  /** @brief nst_air_temp_delta allowed nst air temp variation for qc */
  float nst_air_temp_delta;

  /** @brief nst_qc_bar_press perform nst barometric pressure check against metars */
  short nst_qc_bar_press;

  /** @brief nst_bar_press_delta allowed nst barometric pressure variation for qc*/
  float nst_bar_press_delta;

  /** @brief latitude_min minimum latitude */
  double latitude_min;

  /** @brief latitude_max maximum latitude */
  double latitude_max;

  /** @brief longitude_min minimum longitude */
  double longitude_min;

  /** @brief longitude_max maximum longitude */
  double longitude_max;

  /** @brief max_dist_to_segment maximum distance in degrees from probe message lat-long to nearest segment lat-long for inclusion in segment */
  float max_dist_to_segment;

  /** @brief max_dist_to_cell maximum distance in degrees from probe message lat-long to nearest grid cell lat-long for inclusion in grid cell */
  float max_dist_to_cell;

  /** @brief spatial_range_height_m elevation range from target in meters to search for nearest stations */
  float spatial_range_height_m;

  /** @brief spatial_time_search_secs time difference in seconds to search for nearest station observations from target observation */
  int spatial_time_search_secs;

  /** @brief maximum time distance prior to begin_time for message observation time */
  int max_begin_time_delta;

  /** @brief maximum time distance after end_time for message observation time */
  int max_end_time_delta;

  /** @brief rtma projection string used by proj4 */
  string rtma_proj4_projection_string;

  /** @brief rtma projection dx */
  double rtma_dx;

  /** @brief rtma projection dx */
  double rtma_dy;

  /** @brief rtma projection la1, the origin latitude point in the lower left corner */
  double rtma_la1;

  /** @brief rtma projection lo1, the origin longitude point in the lower left corner */
  double rtma_lo1;

  p4w::Proj4Wrap *rtma_proj;

  /** @brief error error string */
  string error;

  vector<string> probe_message_field_names;


private:
  config_reader & operator=(const config_reader &cfg_reader);
  config_reader(config_reader  &cfg_reader);

};


#endif /* CONFIG_READER_HH */
