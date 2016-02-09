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

#include "vdt_nc_file_dynamic_writer.hh"
#include "vdt_probe_message_qc_statistics.hh"

/**
 * @class vdt_road_segment_statistics_file_writer
 * Represents road segment statistics
 */
class vdt_road_segment_statistics_file_writer : public vdt_nc_file_dynamic_writer<vdt_probe_message_qc_statistics>
{
public:
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
