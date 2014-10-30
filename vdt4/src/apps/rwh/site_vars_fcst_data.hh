//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 *  @file site_vars_fcst_data.hh
 *
 *
 *  @class site_vars_fcst_data
 *
 *   Simple class for organizing forecast data by site
 */

#ifndef _SITE_VARS_FCST_DATA_HH
#define _SITE_VARS_FCST_DATA_HH

// Include files
#include <string>
#include <vector>
#include <unordered_map>
#include <log/log.hh>

#include "stat_utils.hh"
#include "ncfc_utils.hh"

using namespace std;


struct site_vars_fcst_data_struct
{
  string icao_id;
  vector<string> field_names;
  unordered_map<string, int> field_ind_map;
  vector< unordered_map<double, float> > data_maps;
};
typedef struct site_vars_fcst_data_struct site_vars_fcst;


class site_vars_fcst_data
{
public:
  
  /** Constructor */
  site_vars_fcst_data(Log *L);

  /** Destructor */
  ~site_vars_fcst_data();
  
  /** A vector that contains all the site fcst data */
  vector<site_vars_fcst *> site_vars_data;
  
  //int get_fcst_data(time_t begin_time, const vector<site *> &sites, const vector< vector<string> > &field_names, const vector<ncfc_io *> &data, int max_days, int max_lead_time, vector<site_fcst *> &site_fcst_data);
  
  int get_fcst_data(time_t begin_time, const vector<site *> &sites, const vector< vector<string> > &field_names, const vector<ncfc_io *> &data, int max_days, int max_lead_time);
  
  float get_value(double valid_time, string field_name, site_vars_fcst *site_vars_data);

private:
  
  //site_vars_fcst_data & operator=(const site_vars_fcst_data &site_vars_fcst_data);
  //site_vars_fcst_data(const site_vars_fcst_data &site_vars_fcst_data);
  
  Log *log;
  string errorString;
  
  NcfcUtils *ncfc_utils;

};
#endif /* SITE_VARS_FCST_DATA_HH */
