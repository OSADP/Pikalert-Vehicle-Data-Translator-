//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_statistics_file_writer.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/04/08 17:05:00 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_statistics_file_writer.hh
 *
 * @class vdt_road_segment_statistics_file_writer
 *
 *  vdt road segment statistics file writer class
 *
 */


#ifndef VDT_ROAD_SEGMENT_STATISTICS_FILE_WRITER_HH
#define VDT_ROAD_SEGMENT_STATISTICS_FILE_WRITER_HH

#include "vdt_nc_file_writer.hh"
#include "vdt_probe_message_qc_statistics.hh"

/**
 * @class vdt_road_segment_statistics_file_writer
 * Represents road segment statistics
 */
class vdt_road_segment_statistics_file_writer : public vdt_nc_file_writer
{

public:

  /** @brief road segment id netcdf variable name */
  static const char* ROAD_SEGMENT_ID;

  /** @brief total number of messages processed netcdf variable name */
  static const char* TOTAL_NUM_MSG;

  /** @brief air temperature from the model data netcdf variable name */
  static const char* MODEL_AIR_TEMP;

  /** @brief dewpoint from the model data netcdf variable name */
  static const char* MODEL_DEWPOINT;

  /** @brief barometric pressure from the model data netcdf variable name */
  static const char* MODEL_BAR_PRESS;

  /** @brief composite reflectivity from radar grid netcdf variable name */
  static const char* RADAR_REF;

  /** @brief precip from radar grid netcdf variable name */
  static const char* RADAR_PRECIP_FLAG;

  /** @brief precip type from radar grid netcdf variable name */
  static const char* RADAR_PRECIP_TYPE;

  /** @brief cloud mask from satellite grid netcdf variable name */
  static const char* CLOUD_MASK;

  /** @brief mean air temperature for the nearest surface stations netcdf variable name */
  static const char* NSS_AIR_TEMP_MEAN;

  /** @brief mean barometric pressure for the nearest surface stations netcdf variable name */
  static const char* NSS_BAR_PRESS_MEAN;

  /** @brief mean dew temperature for the nearest surface stations netcdf variable name */
  static const char* NSS_DEW_TEMP_MEAN;

  /** @brief mean wind direction for the nearest surface stations netcdf variable name */
  static const char* NSS_WIND_DIR_MEAN;

  /** @brief mean wind speed for the nearest surface stations netcdf variable name */
  static const char* NSS_WIND_SPEED_MEAN;

  /** @brief mean hourly precipitation for the nearest surface stations netcdf variable name */
  static const char* NSS_HOURLY_PRECIP_MEAN;

  /** @brief mean prevailing visiblity for the nearest surface stations netcdf variable name */
  static const char* NSS_PREVAIL_VIS_MEAN;

  /** @brief number of messages with valid air temperature values netcdf variable name */
  static const char* NUM_MSG_VALID_AIR_TEMP;

  /** @brief minumum air temperature netcdf variable name */
  static const char* AIR_TEMP_MIN;

  /** @brief maximum air temperature netcdf variable name */
  static const char* AIR_TEMP_MAX;

  /** @brief mean air temperature netcdf variable name */
  static const char* AIR_TEMP_MEAN;

  /** @brief median air temperature netcdf variable name */
  static const char* AIR_TEMP_MEDIAN;

  /** @brief air temperature variance netcdf variable name */
  static const char* AIR_TEMP_VAR;

  /** @brief air temperature standard deviation netcdf variable name */
  static const char* AIR_TEMP_STDEV;

  /** @brief number of messages with valid external air temperature values netcdf variable name */
  static const char* NUM_MSG_VALID_AIR_TEMP2;

  /** @brief minumum external air temperature netcdf variable name */
  static const char* AIR_TEMP2_MIN;

  /** @brief maximum external air temperature netcdf variable name */
  static const char* AIR_TEMP2_MAX;

  /** @brief mean external air temperature netcdf variable name */
  static const char* AIR_TEMP2_MEAN;

  /** @brief median external air temperature netcdf variable name */
  static const char* AIR_TEMP2_MEDIAN;

  /** @brief external air temperature variance netcdf variable name */
  static const char* AIR_TEMP2_VAR;

  /** @brief external air temperature standard deviation netcdf variable name */
  static const char* AIR_TEMP2_STDEV;

  /** @brief number of messages with valid barometric pressure values netcdf variable name */
  static const char* NUM_MSG_VALID_BAR_PRESS;

  /** @brief minumum barometric pressure netcdf variable name */
  static const char* BAR_PRESS_MIN;

  /** @brief maximum barometric pressure netcdf variable name */
  static const char* BAR_PRESS_MAX;

  /** @brief mean barometric pressure netcdf variable name */
  static const char* BAR_PRESS_MEAN;

  /** @brief median barometric pressure netcdf variable name */
  static const char* BAR_PRESS_MEDIAN;

  /** @brief vbarometric pressure ariance netcdf variable name */
  static const char* BAR_PRESS_VAR;

  /** @brief barometric pressure standard deviation netcdf variable name */
  static const char* BAR_PRESS_STDEV;

  /** @brief number of messages with valid dew temperature values netcdf variable name */
  static const char* NUM_MSG_VALID_DEW_TEMP;

  /** @brief minumum dew temperature netcdf variable name */
  static const char* DEW_TEMP_MIN;

  /** @brief maximum dew temperature netcdf variable name */
  static const char* DEW_TEMP_MAX;

  /** @brief mean dew temperature netcdf variable name */
  static const char* DEW_TEMP_MEAN;

  /** @brief median dew temperature netcdf variable name */
  static const char* DEW_TEMP_MEDIAN;

  /** @brief dew temperature variance netcdf variable name */
  static const char* DEW_TEMP_VAR;

  /** @brief dew temperature standard deviation netcdf variable name */
  static const char* DEW_TEMP_STDEV;

  /** @brief number of messages with valid heading values netcdf variable name */
  static const char* NUM_MSG_VALID_HEADING;

  /** @brief minumum heading netcdf variable name */
  static const char* HEADING_MIN;

  /** @brief maximum heading netcdf variable name */
  static const char* HEADING_MAX;

  /** @brief mean heading netcdf variable name */
  static const char* HEADING_MEAN;

  /** @brief median heading netcdf variable name */
  static const char* HEADING_MEDIAN;

  /** @brief heading variance netcdf variable name */
  static const char* HEADING_VAR;

  /** @brief heading standard deviation netcdf variable name */
  static const char* HEADING_STDEV;

  /** @brief number of messages with valid horizontal lateral acceleration values netcdf variable name */
  static const char* NUM_MSG_VALID_LAT_ACCEL;

  /** @brief minumum horizontal lateral acceleration netcdf variable name */
  static const char* LAT_ACCEL_MIN;

  /** @brief maximum horizontal lateral acceleration netcdf variable name */
  static const char* LAT_ACCEL_MAX;

  /** @brief mean horizontal lateral acceleration netcdf variable name */
  static const char* LAT_ACCEL_MEAN;

  /** @brief median horizontal lateral acceleration netcdf variable name */
  static const char* LAT_ACCEL_MEDIAN;

  /** @brief horizontal lateral acceleration variance netcdf variable name */
  static const char* LAT_ACCEL_VAR;

  /** @brief horizontal lateral acceleration standard deviation netcdf variable name */
  static const char* LAT_ACCEL_STDEV;

  /** @brief number of messages with valid horizontal longitudinal acceleration values netcdf variable name */
  static const char* NUM_MSG_VALID_LON_ACCEL;

  /** @brief minumum horizontal longitudinal acceleration netcdf variable name */
  static const char* LON_ACCEL_MIN;

  /** @brief maximum horizontal longitudinal acceleration netcdf variable name */
  static const char* LON_ACCEL_MAX;

  /** @brief mean horizontal longitudinal acceleration netcdf variable name */
  static const char* LON_ACCEL_MEAN;

  /** @brief median horizontal longitudinal acceleration netcdf variable name */
  static const char* LON_ACCEL_MEDIAN;

  /** @brief horizontal longitudinal acceleration variance netcdf variable name */
  static const char* LON_ACCEL_VAR;

  /** @brief horizontal longitudinal acceleration standard deviation netcdf variable name */
  static const char* LON_ACCEL_STDEV;

  /** @brief number of messages with valid yaw rate values netcdf variable name */
  static const char* NUM_MSG_VALID_YAW;

  /** @brief minumum yaw rate netcdf variable name */
  static const char* YAW_MIN;

  /** @brief maximum yaw rate netcdf variable name */
  static const char* YAW_MAX;

  /** @brief mean yaw rate netcdf variable name */
  static const char* YAW_MEAN;

  /** @brief median yaw rate netcdf variable name */
  static const char* YAW_MEDIAN;

  /** @brief yaw rate variance netcdf variable name */
  static const char* YAW_VAR;

  /** @brief yaw rate standard deviation netcdf variable name */
  static const char* YAW_STDEV;

  /** @brief number of messages with valid steering angle values netcdf variable name */
  static const char* NUM_MSG_VALID_STEERING_ANGLE;

  /** @brief minumum steering angle netcdf variable name */
  static const char* STEERING_ANGLE_MIN;

  /** @brief maximum steering angle netcdf variable name */
  static const char* STEERING_ANGLE_MAX;

  /** @brief mean steering angle netcdf variable name */
  static const char* STEERING_ANGLE_MEAN;

  /** @brief median steering angle netcdf variable name */
  static const char* STEERING_ANGLE_MEDIAN;

  /** @brief steering angle variance netcdf variable name */
  static const char* STEERING_ANGLE_VAR;

  /** @brief steering angle standard deviation netcdf variable name */
  static const char* STEERING_ANGLE_STDEV;

  /** @brief number of messages with valid light values netcdf variable name */
  static const char* NUM_MSG_VALID_LIGHTS;

  /** @brief number of messages with all lights off netcdf variable name */
  static const char* NUM_LIGHTS_OFF;

  /** @brief number of messages with hazard lights on netcdf variable name */
  static const char* NUM_LIGHTS_HAZARD;

  /** @brief number of messages with parking lights on netcdf variable name */
  static const char* NUM_LIGHTS_PARKING;

  /** @brief number of messages with fog lights on netcdf variable name */
  static const char* NUM_LIGHTS_FOG;

  /** @brief number of messages with daytime running lights on netcdf variable name */
  static const char* NUM_LIGHTS_DRL;

  /** @brief number of messages with automatic lights control netcdf variable name */
  static const char* NUM_LIGHTS_AUTOMATIC_CONTROL;

  /** @brief number of messages with right turn signal on netcdf variable name */
  static const char* NUM_LIGHTS_RIGHT_TURN;

  /** @brief number of messages with left turn signal on netcdf variable name */
  static const char* NUM_LIGHTS_LEFT_TURN;

  /** @brief number of messages with high beam on netcdf variable name */
  static const char* NUM_LIGHTS_HIGH_BEAM;

  /** @brief number of messages with low beam on netcdf variable name */
  static const char* NUM_LIGHTS_LOW_BEAM;

  /** @brief number of messages with valid speed values netcdf variable name */
  static const char* NUM_MSG_VALID_SPEED;

  /** @brief average speed to speed limit ratio netcdf variable name */
  static const char* SPEED_RATIO;

  /** @brief minumum speed netcdf variable name */
  static const char* SPEED_MIN;

  /** @brief maximum speed netcdf variable name */
  static const char* SPEED_MAX;

  /** @brief mean speed netcdf variable name */
  static const char* SPEED_MEAN;

  /** @brief median speed netcdf variable name */
  static const char* SPEED_MEDIAN;

  /** @brief speed variance netcdf variable name */
  static const char* SPEED_VAR;

  /** @brief speed standard deviation netcdf variable name */
  static const char* SPEED_STDEV;

  /** @brief number of messages with valid surface temperature values netcdf variable name */
  static const char* NUM_MSG_VALID_SURFACE_TEMP;

  /** @brief minumum surface temperature netcdf variable name */
  static const char* SURFACE_TEMP_MIN;

  /** @brief maximum surface temperature netcdf variable name */
  static const char* SURFACE_TEMP_MAX;

  /** @brief mean surface temperature netcdf variable name */
  static const char* SURFACE_TEMP_MEAN;

  /** @brief median surface temperature netcdf variable name */
  static const char* SURFACE_TEMP_MEDIAN;

  /** @brief surface temperature variance netcdf variable name */
  static const char* SURFACE_TEMP_VAR;

  /** @brief surface temperature standard deviation netcdf variable name */
  static const char* SURFACE_TEMP_STDEV;

  /** @brief number of messages with valid wiper values netcdf variable name */
  static const char* NUM_MSG_VALID_WIPERS;

  /** @brief number of messages with wiper sensor not equipped netcdf variable name */
  static const char* NUM_WIPERS_NOT_EQUIPPED;

  /** @brief number of messages with wipers off netcdf variable name */
  static const char* NUM_WIPERS_OFF;

  /** @brief number of messages with wipers intermittent netcdf variable name */
  static const char* NUM_WIPERS_INTERMITTENT;

  /** @brief number of messages with wipers low netcdf variable name */
  static const char* NUM_WIPERS_LOW;

  /** @brief number of messages with wipers high netcdf variable name */
  static const char* NUM_WIPERS_HIGH;

  /** @brief number of messages with wiper washer on netcdf variable name */
  static const char* NUM_WIPERS_WASHER;

  /** @brief number of messages with automatic wiper control on netcdf variable name */
  static const char* NUM_WIPERS_AUTOMATIC;

  /** @brief number of messages with valid anti lock brake status values netcdf variable name */
  static const char* NUM_MSG_VALID_ABS;

  /** @brief number of messages with abs not equipped netcdf variable name */
  static const char* NUM_ABS_NOT_EQUIPPED;

  /** @brief number of messages with abs off netcdf variable name */
  static const char* NUM_ABS_OFF;

  /** @brief number of messages with abs on netcdf variable name */
  static const char* NUM_ABS_ON;

  /** @brief number of messages with abs engaged netcdf variable name */
  static const char* NUM_ABS_ENGAGED;

  /** @brief number of messages with valid stability values netcdf variable name */
  static const char* NUM_MSG_VALID_STAB;

  /** @brief number of messages with stability control not equipped netcdf variable name */
  static const char* NUM_STAB_NOT_EQUIPPED;

  /** @brief number of messages with stability control off netcdf variable name */
  static const char* NUM_STAB_OFF;

  /** @brief number of messages with stability control on netcdf variable name */
  static const char* NUM_STAB_ON;

  /** @brief number of messages with stability control engaged netcdf variable name */
  static const char* NUM_STAB_ENGAGED;

  /** @brief number of messages with valid traction values netcdf variable name */
  static const char* NUM_MSG_VALID_TRAC;

  /** @brief number of messages with traction control not equipped netcdf variable name */
  static const char* NUM_TRAC_NOT_EQUIPPED;

  /** @brief number of messages with traction control off netcdf variable name */
  static const char* NUM_TRAC_OFF;

  /** @brief number of messages with traction control on netcdf variable name */
  static const char* NUM_TRAC_ON;

  /** @brief number of messages with traction control engaged netcdf variable name */
  static const char* NUM_TRAC_ENGAGED;

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl   cdl file
   */
  vdt_road_segment_statistics_file_writer(const char* fpath, const char* cdl);
  ~vdt_road_segment_statistics_file_writer();

  /**
   * @brief  writes road segment statistics
   * @param[in] stats  vector of vdt road segment statistics
   * @return 0 on success, -1 on failure
   */
  int write_road_segment_statistics(vector<vdt_probe_message_qc_statistics>& stats);

};

#endif /* VDT_ROAD_SEGMENT_STATISTICS_FILE_WRITER_HH */
