
//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_nc_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_nc_file_reader.hh
 *
 * @class vdt_nc_file_reader
 *
 *  vdt netcdf file reader class
 *
 */

#ifndef VDT_NC_FILE_READER_H
#define VDT_NC_FILE_READER_H

//ED should move to new cxx4 interface - old c++ interface only supports classic data model
#include <netcdfcpp.h>
#include <string>

using std::string;

/**
 * @class vdt_nc_file_reader
 */
class vdt_nc_file_reader
{

public:

  /**
   * @brief constructor
   * @param[in] fpath  file path
   */
  vdt_nc_file_reader(const char* fpath);
  ~vdt_nc_file_reader();

  /**
   * @brief get_error
   * @return error string
   */
  const string & get_error() const { return error; };

protected:
  /** @brief error string */
  string error;

  /** @brief file path */
  const char* fpath;

  /** @brief netcdf file */
  NcFile* nc;
};

#endif /* VDT_NC_FILE_READER_H */
