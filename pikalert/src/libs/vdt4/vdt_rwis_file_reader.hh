//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_rwis_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_rwis_file_reader.hh
 *
 * @class vdt_rwis_file_reader
 *
 *  VDT RWIS (Road Weather Information System) class
 *
 */

#ifndef VDT_RWIS_FILE_READER_HH
#define VDT_RWIS_FILE_READER_HH

#include "vdt_nc_file_reader.hh"
//ED why is vdt_probe_message.hh needed here?
#include "vdt_probe_message.hh"
#include "vdt_surface_observation.hh"
#include <vector>

using namespace std;

class vdt_rwis_file_reader : public vdt_nc_file_reader
{
public:

  /** @brief number of records netcdf dimension name */
  const static char* REC_NUM;

  /** @brief station name length netcdf dimension name */
  const static char* STN_NAME_LEN;

  /** @brief station netcdf variable name */
  const static char* STATION_NAME;

  /** @brief latitude netcdf variable name */
  const static char* LATITUDE;

  /** @brief longitude netcdf variable name */
  const static char* LONGITUDE;

  /** @brief elevation netcdf variable name */
  const static char* ELEVATION;

  /** @brief observation time netcdf variable name */
  const static char* OBS_TIME;

  /** @brief air temperature netcdf variable name */
  const static char* AIR_TEMP;

  /** @brief surface temperature netcdf variable name */
  const static char* SURFACE_TEMP;

  /** @brief pressure netcdf variable name */
  const static char* PRESS;

  /** @brief dew point temperature netcdf variable name */
  const static char* DEW_TEMP;

  /** @brief hourly precipitation amount netcdf variable name */
  const static char* HOURLY_PRECIP;

  /** @brief prevailing visibility netcdf variable name */
  const static char* PREVAIL_VIS;

  /** @brief wind direction netcdf variable name */
  const static char* WIND_DIR;

  /** @brief wind speed netcdf variable name */
  const static char* WIND_SPEED;

  /** @brief wind gust netcdf variable name */
  const static char* WIND_GUST;

  /** @brief fill attribute netcdf variable name */
  const static char* FILL_ATTR;

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] logg   log file
   */
  vdt_rwis_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_rwis_file_reader();

  /**
   * @brief  gets surface  observations
   * @param[out] obs  vector of vdt surface observations
   * @return 0 on success, -1 on failure
   */
  int get_rwis(vector<vdt_surface_observation>& obs);
};

#endif /* VDT_RWIS_FILE_READER_HH */
