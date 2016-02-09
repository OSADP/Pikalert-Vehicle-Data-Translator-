//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_metar_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_metar_file_reader.hh
 *
 * @class vdt_metar_file_reader
 *
 *  vdt metar file reader class
 *
 */

#ifndef VDT_METAR_FILE_READER_HH
#define VDT_METAR_FILE_READER_HH

#include "vdt_nc_file_reader.hh"
//ED why are vdt_probe_message.hh
#include "vdt_probe_message.hh"
#include "vdt_surface_observation.hh"
#include <vector>

using namespace std;

/**
 * @class vdt_metar_file_reader
 * Class for reading netcdf metar files
 */
class vdt_metar_file_reader : public vdt_nc_file_reader
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

  /** @brief sea level pressure netcdf variable name */
  const static char* PRESS;

  /** @brief dew point temperature netcdf variable name */
  const static char* DEW_TEMP;

  /** @brief hourly precipitation amount netcdf variable name */
  const static char* HOURLY_PRECIP;

  /** @brief 24 hour precipitation amount netcdf variable name */
  const static char* PRECIP_24HR;

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

  /** @brief value used to convert sea level pressure to barometric pressure  */
  const static float SLP_TO_PRESS;

  /** @brief  WX length dimension */
  const static char* WX_LEN;
  
  /** @brief weather phenomena dscription variable */
  const static char* WX;

  /**
   * @brief  gets metar observations
   * @param[out] obs  vector of vdt surface observations
   * @return 0 on success, -1 on failure
   */
  int get_metars(vector<vdt_surface_observation>& obs);

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   */
  vdt_metar_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_metar_file_reader();

};

#endif /* VDT_METAR_FILE_READER_HH */
