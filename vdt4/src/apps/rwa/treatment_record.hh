//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: treatment_record.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file treatment_record.hh
 *
 * @class treatment_record
 *
 *  <file/class description>
 *  
 * @date 10/16/13
 */

#ifndef TREATMENT_RECORD_HH
#define TREATMENT_RECORD_HH

#include "rwx/rwx_vector_collection_nc.hh"
#include "site_alert.hh"

#include <string>
#include <vector>
#include <unordered_map>

class treatment_info
{
public:
  time_t time;
  int apply_chemical;
  int do_plowing;
  float road_temperature;
};

/**
 * @class treatment_record
 */
class treatment_record
{
public:
  static const std::string forecast_times_per_day;
  static const std::string num_days;
  static const std::string forecast_time_string;
  static const std::string site_num_string;
  static const std::string do_plowing_string;
  static const std::string apply_chemical_string;
  static const std::string road_temperature_string;

  int site_num;
  vector<treatment_info> info_vec;
};

// create map mapping treatment site ids to treatment information
int create_treatment_record_map(int road_segment_na, const rwx_vector_collection_nc *treatment_collection, std::unordered_map<int, treatment_record> &treatment_record_map, string &error);

// incorporate treatments into site alerts
void incorporate_treatments(int site_offset, const std::unordered_map<int, treatment_record> &treatment_record_map, vector<site_alert_class> &site_alerts);

#endif /* TREATMENT_RECORD_HH */
