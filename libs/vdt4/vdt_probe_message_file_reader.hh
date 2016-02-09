//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_probe_message_file_reader.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
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

  /**
   * @brief  gets probe message
   * @param[out] msgs  vector of vdt probe messages
   * @return 0 on success, -1 on failure
   */
  int get_probe_messages(const FIELD_NAME_TYPE_VEC& name_id_pairs, vector<vdt_probe_message>& msgs);

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   */
  vdt_probe_message_file_reader(const char* fpath) : vdt_nc_file_reader(fpath) {};

  ~vdt_probe_message_file_reader();
};

#endif /* VDT_PROBE_MESSAGE_FILE_READER_HH */
