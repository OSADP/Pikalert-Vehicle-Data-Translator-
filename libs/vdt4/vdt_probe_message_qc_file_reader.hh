//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_qc_file_reader.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2014/04/08 17:08:28 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_qc_file_reader.hh
 *
 * @class vdt_probe_message_qc_file_reader
 *
 *  vdt probe_message file reader class
 *
 */

#ifndef VDT_PROBE_MESSAGE_QC_FILE_READER_HH
#define VDT_PROBE_MESSAGE_QC_FILE_READER_HH

#include "vdt_nc_file_reader.hh"
#include "vdt_probe_message_qc.hh"
#include <vector>

using namespace std;

/**
 * @class vdt_probe_message_qc_file_reader
 */
class vdt_probe_message_qc_file_reader : public vdt_nc_file_reader
{

public:

  /** @brief number of records netcdf dimension name */
  const static char* REC_NUM;

  /** @brief vehicle id length netcdf dimension name */
  const static char* VEHICLE_ID_LEN;

  /** @brief anti lock brake status netcdf variable name */
  const static char* ABS;

  /** @brief anti lock brake status range test quality control flag netcdf variable name */
  const static char* ABS_RANGE_QC_PASSED;

  /** @brief brake boost netcdf variable name */
  const static char* BRAKE_BOOST;

  /** @brief brake boost range test quality control flag netcdf variable name */
  const static char* BRAKE_BOOST_RANGE_QC_PASSED;

  /** @brief brake status netcdf variable name */
  const static char* BRAKE_STATUS;

  /** @brief brake status range test quality control flag netcdf variable name */
  const static char* BRAKE_STATUS_RANGE_QC_PASSED;

  /** @brief cloud mask from satellite grid netcdf variable name */
  const static char* CLOUD_MASK;

  /** @brief elevation netcdf variable name */
  const static char* ELEVATION;

  /** @brief heading netcdf variable name */
  const static char* HEADING;

  /** @brief heading range test quality control flag netcdf variable name */
  const static char* HEADING_RANGE_QC_PASSED;

  /** @brief humidity netcdf variable name */
  const static char* HUMIDITY;

  /** @brief horizontal lateral acceleration netcdf variable name */
  const static char* LAT_ACCEL;

  /** @brief horizontal lateral acceleration range test quality control flag netcdf variable name */
  const static char* LAT_ACCEL_RANGE_QC_PASSED;

  /** @brief horizontal longitudinal acceleration netcdf variable name */
  const static char* LON_ACCEL;

  /** @brief horizontal longitudinal acceleration range test quality control flag netcdf variable name */
  const static char* LON_ACCEL_RANGE_QC_PASSED;

  /** @brief latitude netcdf variable name */
  const static char* LATITUDE;

  /** @brief latitude data filtering test quality control flag netcdf variable name */
  const static char* LATITUDE_DFT_PASSED;

  /** @brief longitude netcdf variable name */
  const static char* LONGITUDE;

  /** @brief longitude data filtering test quality control flag netcdf variable name */
  const static char* LONGITUDE_DFT_PASSED;

  /** @brief lights netcdf variable name */
  const static char* LIGHTS;

  /** @brief lights range test quality control flag netcdf variable name */
  const static char* LIGHTS_RANGE_QC_PASSED;

  /** @brief air temperature from the model data netcdf variable name */
  const static char* MODEL_AIR_TEMP;

  /** @brief dewpoint from the model data netcdf variable name */
  const static char* MODEL_DEWPOINT;

  /** @brief barometric pressure from the model data netcdf variable name */
  const static char* MODEL_BAR_PRESS;

  /** @brief mean air temperature for the nearest surface stations  netcdf variable name */
  const static char* NSS_AIR_TEMP_MEAN;

  /** @brief mean barometric pressure for the nearest surface stations netcdf variable name */
  const static char* NSS_BAR_PRESS_MEAN;

  /** @brief mean dew temperature for the nearest surface stations netcdf variable name */
  const static char* NSS_DEW_TEMP_MEAN;

  /** @brief mean hourly precipitation for the nearest surface stations netcdf variable name */
  const static char* NSS_HOURLY_PRECIP_MEAN;

  /** @brief mean prevailing visibility for the nearest surface stations netcdf variable name */
  const static char* NSS_PREVAIL_VIS_MEAN;

  /** @brief mean wind direction for the nearest surface stations netcdf variable name */
  const static char* NSS_WIND_DIR_MEAN;

  /** @brief mean wind speed for the nearest surface stations netcdf variable name */
  const static char* NSS_WIND_SPEED_MEAN;

  /** @brief observation time netcdf variable name */
  const static char* OBS_TIME;

  /** @brief reflectivity from radar grid netcdf variable name */
  const static char* RADAR_REF;

  /** @brief precip flag from radar grid netcdf variable name */
  const static char* RADAR_PRECIP_FLAG;

  /** @brief precip type from radar grid netcdf variable name */
  const static char* RADAR_PRECIP_TYPE;

  /** @brief received time netcdf variable name */
  const static char* REC_TIME;

  /** @brief road segment identifier netcdf variable name */
  const static char* ROAD_SEGMENT_ID;

  /** @brief speed netcdf variable name */
  const static char* SPEED;

  /** @brief speed range quality control flag netcdf variable name */
  const static char* SPEED_RANGE_QC_PASSED;

  /** @brief stability control netcdf variable name */
  const static char* STABILITY_CONTROL;

  /** @brief stability range test quality control flag netcdf variable name */
  const static char* STABILITY_CONTROL_RANGE_QC_PASSED;

  /** @brief steering angle netcdf variable name */
  const static char* STEERING_ANGLE;

  /** @brief steering angle range test quality control flag netcdf variable name */
  const static char* STEERING_ANGLE_RANGE_QC_PASSED;

  /** @brief steering rate netcdf variable name */
  const static char* STEERING_RATE;

  /** @brief steering rate range test quality control flag netcdf variable name */
  const static char* STEERING_RATE_RANGE_QC_PASSED;

  /** @brief traction control netcdf variable name */
  const static char* TRACTION_CONTROL;

  /** @brief traction range test quality control flag netcdf variable name */
  const static char* TRACTION_CONTROL_RANGE_QC_PASSED;

  /** @brief vehicle id netcdf variable name */
  const static char* VEHICLE_ID;

  /** @brief wiper status netcdf variable name */
  const static char* WIPER_STATUS;

  /** @brief wiper status range test quality control flag netcdf variable name */
  const static char* WIPER_STATUS_RANGE_QC_PASSED;

  /** @brief yaw rate netcdf variable name */
  const static char* YAW_RATE;

  /** @brief yaw rate range test quality control flag netcdf variable name */
  const static char* YAW_RATE_RANGE_QC_PASSED;

  /**
   * @brief  gets probe message
   * @param[out] msgs  vector of vdt probe messages
   * @return 0 on success, -1 on failure
   */
  int get_probe_messages(const FIELD_NAME_TYPE_VEC& qc_fields, vector<vdt_probe_message_qc>& msgs);

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   */
  vdt_probe_message_qc_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_probe_message_qc_file_reader();
};

#endif /* VDT_PROBE_MESSAGE_QC_FILE_READER_HH */
