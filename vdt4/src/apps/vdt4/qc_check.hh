//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: qc_check.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/04/08 17:10:42 $
//
//==============================================================================

/**
 *
 * @file qc_check.hh
 *
 * @class qc_check
 *
 *   Quality control check class
 *
 */

#ifndef QC_CHECK_HH
#define QC_CHECK_HH

#include "config_reader.hh"
#include "vdt4/vdt_const.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"
#include "vdt4/vdt_climatology_file_reader.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt_probe_message_datasets.hh"

class qc_check
{
public:
  /**
   * @brief qc_check
   * quality control check
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] ds probe message datasets object
   */
  qc_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *ds);
  
  /** @brief perform range check on given quality controlled probe message */
  virtual void operator()(vdt_probe_message_qc &probe_message) const {};

  virtual ~qc_check() {};

protected:
  /** @brief *configuration file parameters */
  const config_reader &cfg_reader;

  /** @brief vdt climatology file reader object */
  const vdt_climatology_file_reader *climate_data;

  /** @brief probe message datasets object */
  vdt_probe_message_datasets *datasets;
};

/**
 * @class speed_range_check
 * perform range check on speed values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class speed_range_check : public qc_check
{
public:
  /**
   * @brief speed range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  speed_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform speed range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class air_temp_range_check
 * perform range check on air temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class air_temp_range_check : public qc_check
{
public:
  /**
   * @brief air temperature range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  air_temp_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform air temperature range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class air_temp2_range_check
 * perform range check on external air temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class air_temp2_range_check : public qc_check
{
public:
  /**
   * @brief external air temperature range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  air_temp2_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform external air temperature range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class dew_temp_range_check
 * perform range check on dew temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class dew_temp_range_check : public qc_check
{
public:
  /**
   * @brief dew temperature range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  dew_temp_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform dew temperature range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class surface_temp_range_check
 * perform range check on surface temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class surface_temp_range_check : public qc_check
{
public:
  /**
   * @brief surface temperature range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  surface_temp_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform surface temeperature range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class bar_press_range_check
 * perform range check on barometric pressure values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class bar_press_range_check : public qc_check
{
public:
  /**
   * @brief barometric pressure range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  bar_press_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform barometric pressue range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class brake_status_range_check
 * perform range check on brake status values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * rr_active = 8  = '0000 1000B'
 * rf_active = 4  = '0000 0100B'
 * lr_active = 2  = '0000 0010B'
 * lf_active = 1  = '0000 0001B'
 * all_on    = 15 = '0000 1111B'
 * all_off   = 0  = '0000 0000B'
 */
class brake_status_range_check : public qc_check
{
public:
  /**
   * @brief brake status range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  brake_status_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform brake status range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class brake_boost_range_check
 * perform range check on brake boost values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * brake_boost:notEquipped = 0;
 * brake_boost:OFF = 1;
 * brake_boost:ON = 2;
 */
class brake_boost_range_check : public qc_check
{
public:
  /**
   * @brief brake boost range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  brake_boost_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform brake boost range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class wiper_status_range_check
 * perform range check on wiper status values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * wiper_status:notEquipped = 0;
 * wiper_status:off = 1;
 * wiper_status:intermittent = 2;
 * wiper_status:low = 3;
 * wiper_status:high = 4;
 * wiper_status:washer = 5;
 * wiper_status:automaticPresent = 255;
 */
class wiper_status_range_check : public qc_check
{
public:
  /**
   * @brief wiper status range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  wiper_status_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /*
   * @brief perform wiper status range check on given quality controlled probe message
   * @param[in] probe_message  input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class traction_control_range_check
 * perform range check on traction control values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * notEquipped = 0;
 * off = 1;
 * on = 2;
 * engaged = 3;
 */
class traction_control_range_check : public qc_check
{
public:
  /**
   * @brief traction control range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  traction_control_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}


  /**
   * @brief perform traction control range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class stability_control_range_check
 * perform range check on stability control values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * notEquipped = 0;
 * off = 1;
 * on = 2;
 * engaged = 3;
 */
class stability_control_range_check : public qc_check
{
public:
  /**
   * @brief stability control range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  stability_control_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform stability control range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class abs_range_check
 * perform range check on ABS values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * notEquipped = 0;
 * off = 1;
 * on = 2;
 * engaged = 3;
 */
class abs_range_check : public qc_check
{
public:
  /**
   * @brief ABS range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  abs_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform abs range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class headlights_range_check
 * performs range check on headlights values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 * lights:parkingLightsOn = 128;
 * lights:fogLightOn = 64;
 * lights:daytimeRunningLightsOn = 32;
 * lights:automaticLightControlOn = 16;
 * lights:rightTurnSignalOn = 8;
 * lights:leftTurnSignalOn = 4;
 * lights:highBeamHeadlightsOn = 2;
 * lights:lowBeamHeadlightsOn = 1;
 * lights:hazardSignalOn = 24;
 * lights:allLightsOff =  0;
 */
class headlights_range_check : public qc_check
{
public:
  /**
   * @brief headlights range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  headlights_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform headlights range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class yaw_rate_range_check
 * performs range check on yaw rate values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class yaw_rate_range_check : public qc_check
{
public:
  /**
   * @brief yaw rate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  yaw_rate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform yaw rate range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class hoz_accel_lat_range_check
 * perform range check on horizontal acceleration lateral values
 * and set corresponding qc flag in probe_message to true or false,
 * if the check passes or fails, respectively
 */
class hoz_accel_lat_range_check : public qc_check
{
public:
  /**
   * @brief horizontal latitudinal acceleration range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  hoz_accel_lat_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}


  /**
   * @brief perform horizontal acceleration lateral range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class hoz_accel_lon_range_check
 * perform range check on horizontal longitudinal acceleration values and set
 * corresponding qc flag in probe_message to true or false, if the check passes or fails,
 * respectively
 */
class hoz_accel_lon_range_check : public qc_check
{
public:
  /**
   * @brief horizontal longitudinal acceleration range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  hoz_accel_lon_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform horizontal longitudinal acceleration range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class steering_wheel_range_check
 * perform range check on steering wheel angle values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class steering_wheel_range_check : public qc_check
{
public:
  /**
   * @brief steering wheel range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  steering_wheel_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}


  /**
   * @brief perform steering wheel angle range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class steering_rate_range_check
 * perform range check on steering rate values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class steering_rate_range_check : public qc_check
{
public:
  /**
   * @brief steering rate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  steering_rate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform steering rate range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class heading_range_check
 * perform range check on heading values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class heading_range_check : public qc_check
{
public:
  /**
   * @brief heading range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  heading_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}


  /**
   * @brief perform heading range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class air_temp_climate_range_check
 * perform climate range check on air temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class air_temp_climate_range_check : public qc_check
{
public:
  /**
   * @brief air temperature climate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  air_temp_climate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}
																					      //{printf("in climate range constructor for air temp\n");}

  /**
   * @brief perform air temperature climate range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class air_temp2_climate_range_check
 * perform climate range check on external air temperature values and set corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class air_temp2_climate_range_check : public qc_check
{
public:
  /**
   * @brief external air temperature range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  air_temp2_climate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform external air temperature climate range check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class dew_temp_climate_range_check
 * performs climate range check on dew temperature values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class dew_temp_climate_range_check : public qc_check
{
public:
  /**
   * @brief dew point temperature climate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  dew_temp_climate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform dew temperature climate range check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class surface_temp_climate_range_check
 * performs climate range check on surface temperature values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class surface_temp_climate_range_check : public qc_check
{
public:
  /**
   * @brief surface temperature climate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  surface_temp_climate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform surface temperature climate range check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class bar_press_climate_range_check
 * performs climate range check on barometric pressure values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class bar_press_climate_range_check : public qc_check
{
public:
  /**
   * @brief barometric pressure climate range check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  bar_press_climate_range_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform barometric climate range check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class latitude_data_filtering_check
 * perform data filtering check on latitude values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class latitude_data_filtering_check : public qc_check
{
public:
  /**
   * @brief latitude data filtering check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  latitude_data_filtering_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform latitude data filtering check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class longitude_data_filtering_check
 * perform data filtering check on longitude values and sets corresponding qc flag in
 * probe_message to true or false, if the check passes or fails, respectively
 */
class longitude_data_filtering_check : public qc_check
{
public:
  /**
   * @brief longitude data filtering check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  longitude_data_filtering_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {}

  /**
   * @brief perform longitude data filtering check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};

/**
 * @class iqr_spatial_check
 * perform iqr spatial check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class iqr_spatial_check : public qc_check
{
public:
  /**
   * @brief iqr spatial check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  iqr_spatial_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {
    // construct air temperature iqr spatial test
    air_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_air_temp_tol, cfg_reader.spatial_iqr_multiplier);

    // construct dew temperature iqr spatial test
    dew_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_dew_temp_tol, cfg_reader.spatial_iqr_multiplier);

    // construct pressure iqr spatial test
    pressure_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_pressure_tol, cfg_reader.spatial_iqr_multiplier);

    // construct surface temp iqr spatial test
    surface_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_surface_temp_tol, cfg_reader.spatial_iqr_multiplier);
  }

  /**
   * @brief perform iqr spatial check on given quality controlled probe message
   * @param[in] probe_message probe message qc input
   */
  void operator()(vdt_probe_message_qc &probe_message) const;

  vdt_spatial_check_iqr air_temp_spatial_check;
  vdt_spatial_check_iqr dew_temp_spatial_check;
  vdt_spatial_check_iqr pressure_spatial_check;
  vdt_spatial_check_iqr surface_temp_spatial_check;
};

/**
 * @class vdt_neighbor_check_iqr
 * perform iqr neighboring vehicle check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_neighbor_check_iqr : public qc_check
{
public:
  /**
   * @brief vdt neighbor check iqr constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   * @param[in] seg_stats vector of road segment statistics
   * @param[in] seg_indices vector of road segment indices corresponding to probe message ids
   * @param[in] seg_index_offset maps a segment index into its offset into the statistics vector
   */
  vdt_neighbor_check_iqr(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, const unordered_map<int, int> &seg_index_offset) : qc_check(config_reader_arg, climate_data, probe_message_datasets), road_seg_stats(seg_stats), segment_indices(seg_indices), seg_index_offset_map(seg_index_offset)
  {
    // construct air temperature iqr spatial test
    air_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_air_temp_tol, cfg_reader.spatial_iqr_multiplier);

    // construct dew temperature iqr spatial test
    dew_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_dew_temp_tol, cfg_reader.spatial_iqr_multiplier);

    // construct pressure iqr spatial test
    bar_press_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_pressure_tol, cfg_reader.spatial_iqr_multiplier);

    // construct surface temp iqr spatial test
    surface_temp_spatial_check = vdt_spatial_check_iqr(cfg_reader.spatial_iqr_surface_temp_tol, cfg_reader.spatial_iqr_multiplier);
  }

  /**
   * @brief perform iqr neighboring vehicle check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message probe message qc input
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  vdt_spatial_check_iqr air_temp_spatial_check;
  vdt_spatial_check_iqr dew_temp_spatial_check;
  vdt_spatial_check_iqr bar_press_spatial_check;
  vdt_spatial_check_iqr surface_temp_spatial_check;
  const vector<vdt_probe_message_qc_statistics> &road_seg_stats;
  const vector<int> &segment_indices;
  const unordered_map<int, int> &seg_index_offset_map;
};


/**
 * @class vdt_model_check
 * perform model check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_model_check : public qc_check
{
public:
  /**
   * @brief vdt model check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  vdt_model_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {
    model_dewpoint_delta = cfg_reader.mat_dewpoint_delta;
    model_air_temp_delta = cfg_reader.mat_air_temp_delta;
    model_bar_press_delta = cfg_reader.mat_bar_press_delta;
  }

  /**
   * @brief perform model check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message input probe message
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  float model_dewpoint_delta;
  float model_air_temp_delta;
  float model_bar_press_delta;
};

/**
 * @class vdt_stdev_check
 * perform standard deviation check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_stdev_check : public qc_check
{
public:
  /**
   * @brief vdt stdev check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   * @param[in] seg_stats vector of road segment statistics
   * @param[in] seg_indices vector of road segment indices corresponding to probe message ids
   * @param[in] seg_index_offset maps a segment index into its offset into the statistics vector
   */
  vdt_stdev_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, unordered_map<int, int> &seg_index_offset) : qc_check(config_reader_arg, climate_data, probe_message_datasets), road_seg_stats(seg_stats), segment_indices(seg_indices), seg_index_offset_map(seg_index_offset)
  {
  }

  /**
   * @brief perform standard deviation check on given quality controlled probe message
   * @param[in] id road segment index 
   * @param[in] probe_message input probe message
   */
  void operator()(int id, vdt_probe_message_qc &probe_message) const;

  const vector<vdt_probe_message_qc_statistics> &road_seg_stats;
  const vector<int> &segment_indices;
  const unordered_map<int, int> &seg_index_offset_map;
};

/**
 * @class vdt_combined_check
 * perform combined qc check on fields such as air pressure, air temperature and sets corresponding qc flag in probe_message to true, false or non-applicable depending on the outcome
 *
 */
class vdt_combined_check : public qc_check
{
public:
  /**
   * @brief vdt combined check constructor
   * @param[in] config_reader_arg configuration file parameters
   * @param[in] climate_data  vdt climatology file reader object
   * @param[in] probe_message_datasets probe message datasets object
   */
  vdt_combined_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_datasets) : qc_check(config_reader_arg, climate_data, probe_message_datasets)
  {
  }

  /**
   * @brief perform combined algorithm check on given quality controlled probe message
   * @param[in] probe_message input probe message
   */
  void operator()(vdt_probe_message_qc &probe_message) const;
};


#endif /* QC_CHECK_HH */
