//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file process.hh
 *
 * @class process
 *
 * The purpose of this modules is to perform quality control checks
 */

#ifndef PROCESS_HH
#define PROCESS_HH

// Include files
#include <string>
#include <vector>
#include <unordered_map>

#include "rwx/vdt_probe_message_qc_statistics.hh"
#include "rwx/rwx_vector_collection.hh"

#include "arguments.hh"
#include "stat_utils.hh"
#include "ncfc_utils.hh"
#include "config_reader.hh"
#include "site_vars_fcst_data.hh"
#include "rwh_record.hh"

using std::string;
using std::vector;
using namespace rwx;

struct hazard_output_struct
{
  double valid_time;
  vector<int> seg_id;
  vector<int> num_probe_msg;
  vector<short> precip_type;
  vector<float> precip_type_conf;
  vector<short> precip_intensity;
  vector<float> precip_intensity_conf;
  vector<short> pavement_condition;
  vector<float> pavement_condition_conf;
  vector<short> pavement_slickness;
  vector<short> visibility;
  vector<float> visibility_conf;
};
typedef struct hazard_output_struct hazard_out;

struct prev_rwh_struct
{
  unordered_map<int, rwh_record> rwh_record_map;
  const vector<double> *rwh_time_field;
};
typedef struct prev_rwh_struct prev_rwh;

struct rwh_existing_hazards_struct
{
  int id;
  int prev_precip_type;
  int prev_precip_intensity;
  double prev_precip_time;
};
typedef struct rwh_existing_hazards_struct rwh_existing_hazards;

/**
 * @class process
 */
class process
{
public:
  /**
   *
   * Constructor
   *
   * @brief extract information from arguments and configuration
   * objects to prepare for processing
   *
   * @param[in] args_param command line arguments
   */
  process(const arguments &args_param) : args(args_param)
  {
    error = string("");
  };

  /** @brief arguments object */
  const arguments &args;

  /**
   * Destructor
   */
  ~process();

  /**
   * @brief run do processing
   * Processing steps:
   */
  int run();
  
  int determine_prev_rwh_hazard(double begin_time, const vector<int> &seg_ids, const vector<prev_rwh *> &prev_rwh_data, vector<rwh_existing_hazards *> &rwh_eh);
  
  int write_hazard_file(const vector<int> &seg_ids, unordered_map<int, int> &seg_coll_ind_map, unordered_map<int, int> &seg_site_map, const vector<hazard_out *> &vdt_hazard_data, const vector<hazard_out *> &fcst_hazard_data, string cdl_file, string output_file);
  
  int get_hazard_data(double begin_time, double valid_time, vector<vdt_probe_message_qc_statistics> &seg_stats, hazard_out *hazard_data);
  
  int augment_statistics_with_fcst(double valid_time, site_vars_fcst_data &site_data, const vector<int> &seg_ids, unordered_map<int, int> &seg_site_map, const vector<rwh_existing_hazards *> &rwh_eh, vector<vdt_probe_message_qc_statistics> &seg_stats);
  
  int augment_statistics_with_collection(const rwx_vector_collection &vector_collection, const vector<rwh_existing_hazards *> &rwh_eh, vector<vdt_probe_message_qc_statistics> &seg_stats);
  
  int get_fcst_site_indicies(ncfc_io *data, const vector<site *> &sites);
  
  int get_vdt_seg_indicies(const rwx_vector_collection &vector_collection, const vector<int> &seg_ids, vector<int> &seg_indicies);
  
  string get_collection_value(const rwx_vector_collection &vector_collection, string field_name, int field_index);

  int decode_station_wx(string wx_str, int &wx_precip_type, int &wx_precip_intensity, int &wx_visibility);
  
  /**
   * @brief StatUtils object
   */
  StatUtils *stat_utils;
  
  /**
   * @brief NcfcUtils object
   */
  NcfcUtils *ncfc_utils;
  
  /**
   * @brief construction error description
   */
  string error;
};


#endif /* PROCESS_HH */
