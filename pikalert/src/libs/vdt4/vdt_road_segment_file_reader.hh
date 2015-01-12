//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_file_reader.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013/11/14 18:30:30 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_file_reader.hh
 *
 * @class vdt_road_segment_file_reader
 *
 *  vdt road segment file reader class
 *
 */

#ifndef VDT_ROAD_SEGMENT_FILE_READER_H
#define VDT_ROAD_SEGMENT_FILE_READER_H

#include "vdt_nc_file_reader.hh"
#include "vdt_road_segment.hh"
#include "vdt_point.hh"
#include <map>

using namespace std;

/**
 * @class vdt_road_segment_file_reader
 * Class for reading road segment netcdf files
 */
class vdt_road_segment_file_reader : public vdt_nc_file_reader
{
public:

  /** @brief latitude netcdf variable name */
  const static char* LATITUDE;

  /** @brief longitude netcdf variable name */
  const static char* LONGITUDE;

  /** @brief elevation netcdf variable name */
  const static char* ELEVATION;

  /** @brief road segment name netcdf variable name */
  const static char* SEG_NAME;

  /** @brief road segment id netcdf variable name */
  const static char* SEG_ID;

  /** @brief corresponding road weather forecast system id netcdf variable name */
  const static char* RWFS_ID;

  /** @brief corresponding auxillary road weather forecast system id netcdf variable name */
  const static char* AUX_ID;
  
  /** @brief road type netcdf variable name */
  const static char* ROAD_TYPE;

  /** @brief number of records netcdf dimension name */
  const static char* REC_NUM;

  /** @brief road segment name length netcdf dimension name */
  const static char* SEG_NAME_LEN;

  /** @brief rwfs id length netcdf dimension name */
  const static char* RWFS_ID_LEN;
  
  /** @brief aux id length netcdf dimension name */
  const static char* AUX_ID_LEN;
  
  /** @brief point type netcdf variable name */
  const static char* POINT_TYPE;

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] logg   log file
   */
  vdt_road_segment_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_road_segment_file_reader();

  /**
   * @brief  gets road segments
   * @param[out] segs  vector of vdt road segments
   * @return 0 on success, -1 on failure
   */
  int get_road_segments(vector<vdt_road_segment>& segs);

  /**
   * @brief  gets rwis site
   * @param[out] rwis_segs  vector of vdt rwis sites
   * @return 0 on success, -1 on failure
   */
  int get_rwis_sites(vector<vdt_road_segment>& rwis_sites);
};

#endif /* VDT_ROAD_SEGMENT_FILE_READER_H */
