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

using std::string;
using std::vector;

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

  /** @brief sensor_range_test perform sensor range test (SRT) 0=no, 1=yes */
  short sensor_range_test;

  /** @brief speed_range_min minimum speed range in m/s */
  float speed_range_min;

  /** @brief speed_range_max maximum speed range in m/s */
  float speed_range_max;

  /** @brief air_temp_min minimim air temperature in Celcius */
  short air_temp_min;

  /** @brief air_temp_max maximim air temperature in Celcius */
  short air_temp_max;

  /** @brief bar_press_min minimum barometric pressure in mb */
  short bar_press_min;

  /** @brief bar_press_max maximum barometric pressure in mb */
  short bar_press_max;

  /** @brief brake_status_min minimum brake status in bit values corresponding to
      all off, right rear active, right front active, left rear active, all on,
      or a combination of these */
  short brake_status_min;

  /** @brief brake_status_max maximum brake status in bit values corresponding to
      all off, right rear active, right front active, left rear active, all on,
      or a combination of these */
  short brake_status_max;

  /** @brief brake_boost_min minimum brake boost in bit values corresponding to
      not equipped, off, and on */
  short brake_boost_min;

  /** @brief brake_boost_max maximum brake boost in bit values corresponding to
      not equipped, off, and on */
  short brake_boost_max;

  /** @brief wiper_status_min minimum wiper status in bit values [0,5] and 255
             corresponding to not equipped, off, intermittent, low, high, washer, and
             automatic present */
  short wiper_status_min;

  /** @brief wiper_status_max maximum wiper status in bit values [0,5] and 255
      corresponding to not equipped, off, intermittent, low, high, washer, and
      automatic present */
  short wiper_status_max;

  /** @brief traction_control_min minimum traction control in bit values corresponding to
      not equipped, off, on, and engaged */
  short traction_control_min;

  /** @brief traction_control_max maximum traction control in bit values corresponding to
      not equipped, off, on, and engaged */
  short traction_control_max;

  /** @brief stability_control_min minimum stability control in bit values corresponding to
      not equipped, off, on, and engaged */
  short stability_control_min;

  /** @brief stability_control_max maximum stability control in bit values corresponding to
      not equipped, off, on, and engaged */
  short stability_control_max;

  /** @brief abs_min minimum abs in bit values corresponding to not equipped, off, on and
      engaged */
  short abs_min;

  /** @brief abs_max maximum abs in bit values corresponding to not equipped, off, on and
      engaged */
  short abs_max;

  /** @brief headlights_min minimum headlights in bit value corresponding to parking lights on,
      fog lights on, daytime running lights on, automatic light control on, right turn signal on,
      left turn signal on, high beam headlights on, low beam headlights on, hazard signal on,
      all lights off, or a combination of these. */
  short headlights_min;

  /** @brief headlights_max maximum headlights in bit value corresponding to parking lights on,
      fog lights on, daytime running lights on, automatic light control on, right turn signal on,
      left turn signal on, high beam headlights on, low beam headlights on, hazard signal on,
      all lights off, or a combination of these. */
  short headlights_max;

  /** @brief yaw_rate_min minimum yaw rate in degrees */
  float yaw_rate_min;

  /** @brief yaw_rate_max maximum yaw rate in degrees */
  float yaw_rate_max;

  /** @brief hoz_accel_lat_min minimum horizontal acceleration lateral value in m/s */
  float hoz_accel_lat_min;

  /** @brief horz_accel_lat_max maximum horizontal acceleration lateral value in m/s */
  float hoz_accel_lat_max;

  /** @brief horz_accel_lon_min minimum horizontal acceleration longitudinal value in m/s */
  float hoz_accel_lon_min;

  /** @brief horz_accel_lon_max maximum horizontal acceleration longitudinal value in m/s */
  float hoz_accel_lon_max;

  /** @brief steering_wheel_angle_min minimum steering wheel angle in degrees */
  float steering_wheel_angle_min;

  /** @brief steering_wheel_angle_max maximum steering wheel angle in degrees */
  float steering_wheel_angle_max;

  /** @brief steering_rate_min minimum steering rate in degrees */
  short steering_rate_min;

  /** @brief steering_rate_max maximum steering rate in degrees */
  short steering_rate_max;

  /** @brief heading_min minimum heading in degrees */
  float heading_min;

  /** @brief heading_max maximum heading in degrees */
  float heading_max;

  /** @brief standard_deviation_test perform standard deviation test 0=no, 1=yes */
  short standard_deviation_test;

  /** @brief air_temp_dev_mult air temperature standard deviation multiplier */
  float air_temp_dev_mult;

  /** @brief dew_temp_dev_mult dew temperature standard deviation multiplier */
  float dew_temp_dev_mult;

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
  short nst_time_range;

  /** @brief nst_qc_air_temp perform nst air temp check against metars */
  short nst_qc_air_temp;

  /** @brief nst_air_temp_delta allowed nst air temp variation for qc */
  float nst_air_temp_delta;

  /** @brief nst_qc_bar_press perform nst barometric pressure check against metars */
  short nst_qc_bar_press;

  /** @brief nst_bar_press_delta allowed nst barometric pressure variation for qc*/
  float nst_bar_press_delta;

  /** @brief model_analysis_test perform model analysis test (MAT) 0=no, 1=yes */
  short model_analysis_test;

  /** @brief mat_air_temp_delta allowed mat air temp variation for qc */
  float mat_air_temp_delta;

  /** @brief mat_dewpoint_temp_delta allowed mat dew temp variation for qc */
  float mat_dewpoint_delta;

  /** @brief mat_bar_press_delta allowed mat barometric pressure variation for qc */
  float mat_bar_press_delta;

  /** @brief climatological_range_test perform climatological range test (CRT) 0=no, 1=yes */
  short climatological_range_test;

  /** @brief combined_algorithm_test perform combined algorithm test (CAT) 0=no, 1=yes */
  short combined_algorithm_test;

  /** @brief combined_algorithm_threshold threshold for deciding outcome of combined_algorithm_test. Threshold should be between 0 and 1 where 0 means test almost always passes and 1 is more stringent */
  float combined_algorithm_threshold;

  /** @brief combined_algorithm_weights weights for STEP, SPATIAL, SDT, PERSIST, NVT, CRT */
  vector<float> combined_algorithm_weights;

  /** @brief neighboring_vehicle_test perform neighboring vehicle test (NVT) 0=no, 1=yes */
  short neighboring_vehicle_test;

  /** @brief remote_observation_test perform remote observation test (ROT) 0=no, 1=yes */
  short remote_observation_test;

  /** @brief data_filtering_test perform data filtering test (DFT) 0=no, 1=yes */
  short data_filtering_test;

  /** @brief latitude_min minimum latitude */
  double latitude_min;

  /** @brief latitude_max maximum latitude */
  double latitude_max;

  /** @brief longitude_min minimum longitude */
  double longitude_min;

  /** @brief longitude_max maximum longitude */
  double longitude_max;

  /** @brief spatial_tests perform all spatial tests (pressure, air temperature, surface temperature (STP, STTa, STTp) 0=no, 1=yes */
  short spatial_tests;

  /** @brief spatial_iqr_air_temperature_tol minimum tolerance bound for air temperature iqr spatial test */
  float spatial_iqr_air_temp_tol;

  /** @brief spatial_iqr_dew_temperature_tol minimum tolerance bound for dew temperature iqr spatial test */
  float spatial_iqr_dew_temp_tol;

  /** @brief spatial_iqr_surface_temperature_tol minimum tolerance bound for surface temperature iqr spatial test */
  float spatial_iqr_surface_temp_tol;

  /** @brief spatial_iqr_multiplier spatial test multiplier except for relative humidity */
  float spatial_iqr_multiplier;

  /** @brief spatial_iqr_pressure_tol minimum tolerance bound for pressure iqr spatial test */
  float spatial_iqr_pressure_tol;

  /** @brief spatial_barnes_min_size minimum number of observation sites needed for barnes spatial test */
  size_t spatial_barnes_min_size;

  /** @brief spatial_barnes_sd_max maximum allowable number of standard deviations from weighted mean to pass test */
  size_t spatial_barnes_max_sd;

  /** @brief spatial_range_search_km distance from target in kilometers to search for nearest stations */
  float spatial_range_search_km;

  /** @brief max_dist_to_segment maximum distance in degrees from probe message lat-long to nearest segment lat-long for inclusion in segment */
  float max_dist_to_segment;

  /** @brief max_dist_to_cell maximum distance in degrees from probe message lat-long to nearest grid cell lat-long for inclusion in grid cell */
  float max_dist_to_cell;

  /** @brief spatial_range_height_m elevation range from target in meters to search for nearest stations */
  float spatial_range_height_m;

  /** @brief spatial_time_search_secs time difference in seconds to search for nearest station observations from target observation */
  int spatial_time_search_secs;

  /** @brief history_tests perform history tests (includes PET and TST) 0=no, 1=yes */
  short history_tests;

  /** @brief history_path path to the history files */
  string history_path;

  /** @brief history_basename basename for the history files */
  string history_basename;

  /** @brief history_cutoff_time time period in seconds to cutoff old observations from addition to the history file */
  int history_cutoff_time;

  /** @brief persistence_test perform persistence test (PET) 0=no, 1=yes */
  short persistence_test;

  /** @brief persistence_sensor_minutes */
  unsigned int persistence_sensor_minutes;

  /** @brief air_temp_persist_time time period in seconds to detect whether air temp sensor readings remain constant */
  int air_temp_persist_time;

  /** @brief air2_temp_persist_time time period in seconds to detect whether air temp2 sensor readings remain constant */
  int air_temp2_persist_time;

  /** @brief dew_temp_persist_time time period in seconds to detect whether dew sensor readings remain constant */
  int dew_temp_persist_time;

  /** @brief surface_temp_persist_time time period in seconds to detect whether surface temp sensor readings remain constant */
  int surface_temp_persist_time;

  /** @brief bar_press_persist_time time period in seconds to detect whether barometric pressure sensor readings remain constant */
  int bar_press_persist_time;

  /** @brief time_step_test perform time step test (TST) 0=no, 1=yes */
  short time_step_test;

  /** @brief air_temp_step_time time period in seconds to look back for air temp time step test */
  int air_temp_step_time;

  /** @brief air_temp_step_magnitude degrees difference to check between the air temp current sensor value and prior sensor value */
  float air_temp_step_magnitude;

  /** @brief air_temp2_step_time time period in seconds to look back for air temp2 time step test */
  int air_temp2_step_time;

  /** @brief air_temp2_step_magnitude degrees difference to check between the air temp2 current sensor value and prior sensor value */
  float air_temp2_step_magnitude;

  /** @brief dew_temp_step_time time period in seconds to look back for dew temp time step test */
  int dew_temp_step_time;

  /** @brief dew_temp_step_magnitude degrees difference to check between the dew temp current sensor value and prior sensor value */
  float dew_temp_step_magnitude;

  /** @brief surface_temp_step_time time period in seconds to look back for air temp time step test */
  int surface_temp_step_time;

  /** @brief surface_temp_step_magnitude degrees difference to check between the air temp current sensor value and prior sensor value */
  float surface_temp_step_magnitude;

  /** @brief barometric pressure time period in seconds to look back for barometric pressure time step test */
  int bar_press_step_time;

  /** @brief barometric pressure difference in mb to check between the current barometric pressure sensor value and prior sensor value */
  float bar_press_step_magnitude;

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
