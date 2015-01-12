
//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_nc_file_writer.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_nc_file_writer.hh
 *
 * @class vdt_nc_file_writer
 *
 *  vdt netcdf file writer class
 *
 */

#ifndef VDT_NC_FILE_WRITER_H
#define VDT_NC_FILE_WRITER_H

#include <netcdfcpp.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @class vdt_nc_file_writer
 */
class vdt_nc_file_writer
{

  //ED can remove
  /** @brief maximum command size */
  static const int MAX_CMD_SIZE;

public:

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl    cdl file
   */
  vdt_nc_file_writer(const char* fpath, const char* cdl);
  ~vdt_nc_file_writer();

protected:

  /** @brief netcdf file path */
  const char* fpath;

  /** @brief cdl file */
  const char* cdl;

  /** @brief netcdf file */
  NcFile* nc;

  //ED string error;
};

#endif /* VDT_NC_FILE_WRITER_H */
