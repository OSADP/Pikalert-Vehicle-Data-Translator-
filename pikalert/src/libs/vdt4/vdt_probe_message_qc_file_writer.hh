//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_qc_file_writer.hh,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/05/29 17:54:47 $
//
//==============================================================================

/**
 *
 * @file vdt_probe_message_qc_file_writer.hh
 *
 * @class vdt_probe_message_qc_file_writer
 *
 *  vdt probe message qc file writer class
 *
 */

#ifndef VDT_PROBE_MESSAGE_QC_FILE_WRITER_H
#define VDT_PROBE_MESSAGE_QC_FILE_WRITER_H

#include "vdt_nc_file_writer.hh"
#include "vdt_probe_message_qc.hh"
#include <vector>

//ED should be named vdt_qc_probe_message_file_writer and should inherit from vdt_probe_message_qc_file_writer which writes probe messages without qc information
//The vdt_probe_message_qc_file_writer module should inherit from vdt_nc_file_writer

using namespace std;

/**
 * @class vdt_probe_message_qc_file_writer
 * Represents the quality controlled probe message
 */
class vdt_probe_message_qc_file_writer : public vdt_nc_file_writer
{

  /** @brief vehicle id length netcdf dimension name */
  static const char* VEHICLE_ID_LEN;

  /** @brief observation time netcdf variable name */
  static const char* OBS_TIME;

  /** @brief received time netcdf variable name */
  static const char* REC_TIME;

  /** @brief vehicle id netcdf variable name */
  static const char* VEHICLE_ID;

  /** @brief latitude netcdf variable name */
  static const char* LATITUDE;

  /** @brief latitude data filtering test quality control flag netcdf variable name */
  static const char* LATITUDE_DFT_PASSED;

  /** @brief longitude netcdf variable name */
  static const char* LONGITUDE;

  /** @brief longitude data filtering test quality control flag netcdf variable name */
  static const char* LONGITUDE_DFT_PASSED;

  /** @brief elevation netcdf variable name */
  static const char* ELEVATION;

  /** @brief road segment identifier netcdf variable name */
  static const char* ROAD_SEGMENT_ID;

  /** @brief speed netcdf variable name */
  static const char* SPEED;

  /** @brief speed range quality control flag netcdf variable name */
  static const char* SPEED_RANGE_QC_PASSED;

  /** @brief air temperature netcdf variable name */
  static const char* AIR_TEMP;

  /** @brief ait temperature sensor range test quality control flag netcdf variable name */
  static const char* AIR_TEMP_RANGE_QC_PASSED;

  /** @brief air temperature combined algorithm test quality control flag netcdf variable name */
  static const char* AIR_TEMP_CAT_PASSED;

  /** @brief air temperature combined algorithm test confidence netcdf variable name */
  static const char* AIR_TEMP_CAT_CONFIDENCE;

  /** @brief air temperature climate range test quality control flag netcdf variable name */
  static const char* AIR_TEMP_CRT_PASSED;

  /** @brief air temperature model analysis quality control flag netcdf variable name */
  static const char* AIR_TEMP_MAT_PASSED;

  /** @brief air temperature nearest surface station quality control flag netcdf variable name */
  static const char* AIR_TEMP_NST_PASSED;

  /** @brief air temperature neighboring vehicle test quality control flag netcdf variable name */
  static const char* AIR_TEMP_NVT_PASSED;

  /** @brief air temperature persistence quality control flag netcdf variable name */
  static const char* AIR_TEMP_PERSIST_PASSED;

  /** @brief air temperature standard deviation test quality control flag netcdf variable name */
  static const char* AIR_TEMP_SDT_PASSED;

  /** @brief air temperature spatial barnes quality control flag netcdf variable name */
  static const char* AIR_TEMP_SPATIAL_BARNES_PASSED;

  /** @brief air temperature spatial iqr quality control flag netcdf variable name */
  static const char* AIR_TEMP_SPATIAL_IQR_PASSED;

  /** @brief air temperature time step quality control flag netcdf variable name */
  static const char* AIR_TEMP_STEP_PASSED;

  /** @brief mean air temperature for the nearest surface stations  netcdf variable name */
  static const char* NSS_AIR_TEMP_MEAN;

  /** @brief air temperature from the model data netcdf variable name */
  static const char* MODEL_AIR_TEMP;

  /** @brief dewpoint from the model data netcdf variable name */
  static const char* MODEL_DEWPOINT;

  /** @brief barometric pressure netcdf variable name */
  static const char* BAR_PRESSURE;

  /** @brief barometric pressure sensor range test  quality control flag netcdf variable name */
  static const char* BAR_PRESS_RANGE_QC_PASSED;

  /** @brief barometric pressure combined algorithm test quality control flag netcdf variable name */
  static const char* BAR_PRESS_CAT_PASSED;

  /** @brief barometric pressure combined algorithm test confidence netcdf variable name */
  static const char* BAR_PRESS_CAT_CONFIDENCE;

  /** @brief barometric pressure climate range test quality control flag netcdf variable name */
  static const char* BAR_PRESS_CRT_PASSED;

  /** @brief barometric pressure model analysis quality control flag netcdf variable name */
  static const char* BAR_PRESS_MAT_PASSED;

  /** @brief barometric pressure nearest surface station quality control flag netcdf variable name */
  static const char* BAR_PRESS_NST_PASSED;

  /** @brief barometric pressure neighboring vehcile test quality control flag netcdf variable name */
  static const char* BAR_PRESS_NVT_PASSED;

  /** @brief barometric pressure persistence quality control flag netcdf variable name */
  static const char* BAR_PRESS_PERSIST_PASSED;

  /** @brief barometric pressure standard deviation test quality control flag netcdf variable name */
  static const char* BAR_PRESS_SDT_PASSED;

  /** @brief barometric pressure spatial barnes quality control flag netcdf variable name */
  static const char* BAR_PRESS_SPATIAL_BARNES_PASSED;

  /** @brief barometric pressure spatial iqr quality control flag netcdf variable name */
  static const char* BAR_PRESS_SPATIAL_IQR_PASSED;

  /** @brief barometric pressure time step quality control flag netcdf variable name */
  static const char* BAR_PRESS_STEP_PASSED;

  /** @brief mean barometric pressure for the nearest surface stations netcdf variable name */
  static const char* NSS_BAR_PRESS_MEAN;

  /** @brief barometric pressure from the model data netcdf variable name */
  static const char* MODEL_BAR_PRESS;

  /** @brief reflectivity from radar grid netcdf variable name */
  static const char* RADAR_REF;

  /** @brief precip flag from radar grid netcdf variable name */
  static const char* RADAR_PRECIP_FLAG;

  /** @brief precip type from radar grid netcdf variable name */
  static const char* RADAR_PRECIP_TYPE;

  /** @brief cloud mask from satellite grid netcdf variable name */
  static const char* CLOUD_MASK;

  /** @brief mean dew temperature for the nearest surface stations netcdf variable name */
  static const char* NSS_DEW_TEMP_MEAN;

  /** @brief mean wind direction for the nearest surface stations netcdf variable name */
  static const char* NSS_WIND_DIR_MEAN;

  /** @brief mean wind speed for the nearest surface stations netcdf variable name */
  static const char* NSS_WIND_SPEED_MEAN;

  /** @brief mean hourly precipitation for the nearest surface stations netcdf variable name */
  static const char* NSS_HOURLY_PRECIP_MEAN;

  /** @brief mean prevailing visibility for the nearest surface stations netcdf variable name */
  static const char* NSS_PREVAIL_VIS_MEAN;

  /** @brief brake status netcdf variable name */
  static const char* BRAKE_STATUS;

  /** @brief brake status range test quality control flag netcdf variable name */
  static const char* BRAKE_STATUS_RANGE_QC_PASSED;

  /** @brief brake boost netcdf variable name */
  static const char* BRAKE_BOOST;

  /** @brief brake boost range test quality control flag netcdf variable name */
  static const char* BRAKE_BOOST_RANGE_QC_PASSED;

  /** @brief wiper statues netcdf variable name */
  static const char* WIPER_STATUS;

  /** @brief wiper status range test quality control flag netcdf variable name */
  static const char* WIPER_STATUS_RANGE_QC_PASSED;

  /** @brief traction netcdf variable name */
  static const char* TRAC;

  /** @brief traction range test quality control flag netcdf variable name */
  static const char* TRAC_RANGE_QC_PASSED;

  /** @brief stability netcdf variable name */
  static const char* STAB;

  /** @brief stability range test quality control flag netcdf variable name */
  static const char* STAB_RANGE_QC_PASSED;

  /** @brief anti lock brake status netcdf variable name */
  static const char* ABS;

  /** @brief anti lock brake status range test quality control flag netcdf variable name */
  static const char* ABS_RANGE_QC_PASSED;

  /** @brief lights netcdf variable name */
  static const char* LIGHTS;

  /** @brief lights range test quality control flag netcdf variable name */
  static const char* LIGHTS_RANGE_QC_PASSED;

  /** @brief heading netcdf variable name */
  static const char* HEADING;

  /** @brief heading range test quality control flag netcdf variable name */
  static const char* HEADING_RANGE_QC_PASSED;

  /** @brief yaw rate netcdf variable name */
  static const char* YAW_RATE;

  /** @brief yaw rate range test quality control flag netcdf variable name */
  static const char* YAW_RATE_RANGE_QC_PASSED;

  /** @brief horizontal longitudinal acceleration netcdf variable name */
  static const char* LON_ACCEL;

  /** @brief horizontal longitudinal acceleration range test quality control flag netcdf variable name */
  static const char* LON_ACCEL_RANGE_QC_PASSED;

  /** @brief horizontal lateral acceleration netcdf variable name */
  static const char* LAT_ACCEL;

  /** @brief horizontal lateral acceleration range test quality control flag netcdf variable name */
  static const char* LAT_ACCEL_RANGE_QC_PASSED;

  /** @brief steering angle netcdf variable name */
  static const char* STEERING_ANGLE;

  /** @brief steering angle range test quality control flag netcdf variable name */
  static const char* STEERING_ANGLE_RANGE_QC_PASSED;

  /** @brief steering rate netcdf variable name */
  static const char* STEERING_RATE;

  /** @brief steering rate range test quality control flag netcdf variable name */
  static const char* STEERING_RATE_RANGE_QC_PASSED;

  /** @brief external air temperature netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP;

  /** @brief external air temperature sensor range test quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_RANGE_QC_PASSED;

  /** @brief external air temperature combined algorithm test quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_CAT_PASSED;

  /** @brief external air temperature combined algorithm test confidence netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_CAT_CONFIDENCE;

  /** @brief external air temperature climate range test quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_CRT_PASSED;

  /** @brief external air temperature model analysis quality control flag netcdf netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_MAT_PASSED;

  /** @brief external air temperature nearest surface station quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_NST_PASSED;

  /** @brief external air temperature neighboring vehcile test quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_NVT_PASSED;

  /** @brief external air temperature persistence quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_PERSIST_PASSED;

  /** @brief external air temperature standard deviation test quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_SDT_PASSED;

  /** @brief external air temperature spatial barnes quality control flag netcdf netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_SPATIAL_BARNES_PASSED;

  /** @brief external air temperature spatial iqr quality control flag netcdf netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_SPATIAL_IQR_PASSED;

  /** @brief external air temperature time step quality control flag netcdf variable name */
  static const char* EXTERNAL_AIR_TEMP_STEP_PASSED;

  /** @brief surface temperature netcdf variable name */
  static const char* SURFACE_TEMP;

  /** @brief surface temperature sensor range test quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_RANGE_QC_PASSED;

  /** @brief surface temperature combined algorithm test quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_CAT_PASSED;

  /** @brief surface temperature combined algorithm test quality control confidence variable name */
  static const char* SURFACE_TEMP_CAT_CONFIDENCE;

  /** @brief surface temperature climate range test quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_CRT_PASSED;

  /** @brief surface temperature model analysis quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_MAT_PASSED;

  /** @brief surface temperature neighboring vehicle test quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_NVT_PASSED;

  /** @brief surface temperature persistence quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_PERSIST_PASSED;

  /** @brief surface temperature standard deviation test quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_SDT_PASSED;

  /** @brief surface temperature spatial barnes quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_SPATIAL_BARNES_PASSED;

  /** @brief surface temperature spatial iqr quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_SPATIAL_IQR_PASSED;

  /** @brief surface temperature step quality control flag netcdf variable name */
  static const char* SURFACE_TEMP_STEP_PASSED;

  /** @brief dew temperature netcdf variable name */
  static const char* DEW_TEMP;

  /** @brief dew temperature sensor range test quality control flag netcdf variable name */
  static const char* DEW_TEMP_RANGE_QC_PASSED;

  /** @brief dew temperature combined algorithm test quality control flag netcdf variable name */
  static const char* DEW_TEMP_CAT_PASSED;

  /** @brief dew temperature combined algorithm test confidence netcdf variable name */
  static const char* DEW_TEMP_CAT_CONFIDENCE;

  /** @brief dew temperature climate range test quality control flag netcdf variable name */
  static const char* DEW_TEMP_CRT_PASSED;

  /** @brief dew temperature model analysis quality control flag netcdf variable name */
  static const char* DEW_TEMP_MAT_PASSED;

  /** @brief dew temperature nearest surface station quality control flag netcdf variable name */
  static const char* DEW_TEMP_NST_PASSED;

  /** @brief dew temperature neighboring vehicle test quality control flag netcdf variable name */
  static const char* DEW_TEMP_NVT_PASSED;

  /** @brief dew temperature persistence quality control flag netcdf variable name */
  static const char* DEW_TEMP_PERSIST_PASSED;

  /** @brief dew temperature standard deviation test quality control flag netcdf variable name */
  static const char* DEW_TEMP_SDT_PASSED;

  /** @brief dew temperature spatial barnes quality control flag netcdf variable name */
  static const char* DEW_TEMP_SPATIAL_BARNES_PASSED;

  /** @brief dew temperature spatial iqr quality control flag netcdf variable name */
  static const char* DEW_TEMP_SPATIAL_IQR_PASSED;

  /** @brief dew temperature time step quality control flag netcdf variable name */
  static const char* DEW_TEMP_STEP_PASSED;


public:

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl   cdl file
   */
  vdt_probe_message_qc_file_writer(const char* fpath, const char* cdl):vdt_nc_file_writer(fpath,cdl)
  {
  };
  ~vdt_probe_message_qc_file_writer();


  /**
   * @brief  writes probe message
   * @param[in] msgs  vector of quality controlled probe messages
   * @return 0 on success, -1 on failure
   */
  int write_probe_messages(const vector<vdt_probe_message_qc>& msgs);
};

#endif /* VDT_PROBE_MESSAGE_QC_FILE_WRITER_H */
