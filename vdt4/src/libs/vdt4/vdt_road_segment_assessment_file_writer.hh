//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_road_segment_assessment_file_writer.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/04/10 17:57:03 $
//
//==============================================================================

/**
 *
 * @file vdt_road_segment_assessment_file_writer.hh
 *
 * @class vdt_road_segment_assessment_file_writer
 *
 *  vdt road segment assessment file writer class
 *
 */


#ifndef VDT_ROAD_SEGMENT_ASSESSMENT_FILE_WRITER_HH
#define VDT_ROAD_SEGMENT_ASSESSMENT_FILE_WRITER_HH

#include "vdt_nc_file_writer.hh"
#include "vdt_road_segment_assessment.hh"

/**
 * @class vdt_road_segment_assessment_file_writer
 * Represents road segment assessment
 */
class vdt_road_segment_assessment_file_writer : public vdt_nc_file_writer
{

public:

  /** @brief all hazards flag netcdf variable name */
  static const char* ALL_HAZARDS;

  /** @brief derived pavement condition field netcdf variable name */
  static const char* PAVEMENT_CONDITION;

  /** @brief derived precipitation field netcdf variable name */
  static const char* PRECIPITATION;

  /** @brief road segment id netcdf variable name */
  static const char* ROAD_SEGMENT_ID;

  /** @brief derived visibility field netcdf variable name */
  static const char* VISIBILITY;

  /**
   * @brief constructor
   * @param[in] fpath  file path to netcdf file
   * @param[in] cdl   cdl file
   */
  vdt_road_segment_assessment_file_writer(const char* fpath, const char* cdl);
  ~vdt_road_segment_assessment_file_writer();

  /**
   * @brief  writes road segment assessments
   * @param[in] assessments  vector of vdt road segment assessments
   * @return 0 on success, -1 on failure
   */
  int write_road_segment_assessment(vector<vdt_road_segment_assessment>& assessments);
};

#endif /* VDT_ROAD_SEGMENT_ASSESSMENT_FILE_WRITER_HH */
