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

#include "vdt_nc_file_dynamic_writer.hh"
#include "vdt_probe_message_qc.hh"
#include <vector>

//ED should be named vdt_qc_probe_message_file_writer and should inherit from vdt_probe_message_qc_file_writer which writes probe messages without qc information
//The vdt_probe_message_qc_file_writer module should inherit from vdt_nc_file_writer

using namespace std;

/**
 * @class vdt_probe_message_qc_file_writer
 * Represents the quality controlled probe message
 */
class vdt_probe_message_qc_file_writer : public vdt_nc_file_dynamic_writer<vdt_probe_message_qc>
{
public:

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl   cdl file
   */
  vdt_probe_message_qc_file_writer(const char* fpath, const char* cdl):vdt_nc_file_dynamic_writer(fpath,cdl)
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
