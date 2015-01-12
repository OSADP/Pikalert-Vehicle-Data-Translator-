//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwh_record.hh,v $
//       Version: $Revision: 1.7 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file rwh_record.hh
 *
 * @class Rwh_Record
 *
 *  <file/class description>
 *  
 * @date 10/16/13
 */

#ifndef RWH_RECORD_HH
#define RWH_RECORD_HH

#include "rwx/rwx_vector_collection_nc.hh"

#include <string>
#include <vector>
#include <unordered_map>

class rwh_info
{
public:
  enum rwh_info_value_types {RI_TIME_NOMINAL, RI_ROAD_SEGMENT_ID, RI_PRECIP_TYPE, RI_PRECIP_TYPE_CONFIDENCE, RI_PRECIP_INTENSITY, RI_PRECIP_INTENSITY_CONFIDENCE, RI_PAVEMENT_CONDITION, RI_PAVEMENT_CONDITION_CONFIDENCE, RI_VISIBILITY, RI_VISIBILITY_CONFIDENCE, RI_PAVEMENT_SLICKNESS};

  double time_nominal;
  int road_segment_id;
  short precip_type;
  float precip_type_confidence;
  short precip_intensity;
  float precip_intensity_confidence;
  short pavement_condition;
  float pavement_condition_confidence;
  short visibility;
  float visibility_confidence;
  short pavement_slickness;
  int get_value(rwh_info_value_types value_type, double *value) const;
};

typedef std::unordered_map<string, double> rwh_info_t;

/**
 * @class rwh_record
 */
class rwh_record
{
public:
  static const std::string time_nominal_string;
  static const std::string road_segment_id_string;
  static const std::string precip_type_string;
  static const std::string precip_type_confidence_string;
  static const std::string precip_intensity_string;
  static const std::string precip_intensity_confidence_string;
  static const std::string pavement_condition_string;
  static const std::string pavement_condition_confidence_string;
  static const std::string visibility_string;
  static const std::string visibility_confidence_string;
  static const std::string pavement_slickness_string;

  int site_num;
  vector<rwh_info> info_vec;
};

// create map mapping site ids to site information
int create_rwh_record_map(int road_segment_na, const rwx_vector_collection_nc &rwh_collection, std::unordered_map<int, rwh_record> &rwh_record_map, string &error);

#endif /* RWH_RECORD_HH */
