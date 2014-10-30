//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_file_reader.hh
 *
 * @class vdt_probe_message_file_reader
 *
 *  vdt probe_message file reader class
 *
 */

#ifndef VDT_PROBE_MESSAGE_FILE_READER_HH
#define VDT_PROBE_MESSAGE_FILE_READER_HH

#include "vdt_nc_file_reader.hh"
#include "vdt_probe_message.hh"
#include <vector>

using namespace std;

/**
 * @class vdt_probe_message_file_reader
 */
class vdt_probe_message_file_reader : public vdt_nc_file_reader
{

public:

  /** @brief number of records netcdf dimension name */
  const static char* REC_NUM;

  /** @brief vehicle id length netcdf dimension name */
  const static char* VEHICLE_ID_LEN;

  /** @brief latitude netcdf variable name */
  const static char* LATITUDE;

  /** @brief longitude netcdf variable name */
  const static char* LONGITUDE;

  /** @brief elevation netcdf variable name */
  const static char* ELEVATION;

  /** @brief observation time netcdf variable name */
  const static char* OBS_TIME;

  /** @brief received time netcdf variable name */
  const static char* REC_TIME;

  /** @brief vehicle id netcdf variable name */
  const static char* VEHICLE_ID;

  /** @brief speed netcdf variable name */
  const static char* SPEED;

  /** @brief brake status netcdf variable name */
  const static char* BRAKE_STATUS;

  /** @brief brake boost netcdf variable name */
  const static char* BRAKE_BOOST;

  /** @brief wiper status netcdf variable name */
  const static char* WIPER_STATUS;

  /** @brief air temperature netcdf variable name */
  const static char* AIR_TEMPERATURE;

  /** @brief barometric pressure netcdf variable name */
  const static char* BAR_PRESSURE;

  /** @brief traction control netcdf variable name */
  const static char* TRACTION_CONTROL;

  /** @brief stability control netcdf variable name */
  const static char* STABILITY_CONTROL;

  /** @brief anti lock brake system netcdf variable name */
  const static char* ABS;

  /** @brief lights netcdf variable name */
  const static char* LIGHTS;

  /** @brief probe segment netcdf variable name */
  const static char* PROBE_SEGMENT_NUMBER;

  /** @brief heading netcdf variable name */
  const static char* HEADING;

  /** @brief yaw rate netcdf variable name */
  const static char* YAW_RATE;

  /** @brief longitudinal acceleration netcdf variable name */
  const static char* LONGITUDINAL_ACCELERATION;

  /** @brief lateral acceleration netcdf variable name */
  const static char* LATERAL_ACCELERATION;

  /** @brief steering angle netcdf variable name */
  const static char* STEERING_ANGLE;

  /** @brief steering rate netcdf variable name */
  const static char* STEERING_RATE;

  /** @brief dew point temperature netcdf variable name */
  const static char* DEW_TEMPERATURE;

  /** @brief surface temperature netcdf variable name */
  const static char* SURFACE_TEMPERATURE;

  /** @brief external air temperature netcdf variable name */
  const static char* EXTERNAL_AIR_TEMPERATURE;

  /**
   * @brief  gets probe message
   * @param[out] msgs  vector of vdt probe messages
   * @return 0 on success, -1 on failure
   */
  int get_probe_messages(vector<vdt_probe_message>& msgs);

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   */
  vdt_probe_message_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_probe_message_file_reader();
};

#endif /* VDT_PROBE_MESSAGE_FILE_READER_HH */
