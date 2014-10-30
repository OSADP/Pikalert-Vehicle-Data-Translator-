//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: json_writer.hh,v $
//       Version: $Revision: 1.7 $  Dated: $Date: 2014/01/30 18:54:31 $
//
//==============================================================================

/**
 * @file json_writer.hh
 *
 * @class json_writer
 *
 *  <file/class description>
 *  
 * @date 7/30/13
 */

#ifndef JSON_WRITER_HH
#define JSON_WRITER_HH

#include <unordered_map>
#include <string>
#include <vector>
#include "config_reader.hh"
#include "json_spirit.h"
#include "obs_stats_record.hh"
#include "road_segment_map.hh"
#include "rwx/rwx_types.hh"
#include "site_alert.hh"
#include "site_info.hh"

using std::string;
using std::unordered_map;
using std::vector;

int site_alert_to_json(const std::vector<site_alert_class> &site_alerts, json_spirit::mValue &districts_json);

int create_district_alerts_json(const string &date_time_string, const config_reader &cfg_reader, const vector<site_alert_class> &site_alerts, json_spirit::mObject& districts_json, string &error);

int alerts_json_writer(const string &date_time_string, const config_reader &cfg_reader, const string &file_name, const vector<site_alert_class> &site_alerts, string &error);

void create_maw_site_alerts_json(const string &date_time_string, const vector<site_alert_class> &site_alerts, json_spirit::mObject& maw_site_alerts_json);

int maw_json_writer(const string &date_time_string, const string &file_name, const vector<site_alert_class> &site_alerts, string &error);

int plots_json_writer(const string &date_time_string, const string &file_name, const vector<site_alert_class> &site_alerts, string &error);

int obs_stats_json_writer(const string &date_time_string, const string &file_name, const unordered_map<int, obs_stats_record> &obs_stats_record_map, const unordered_map<int, road_segment_info> &road_segment_map, const site_info *site_info_ptr, string &error);

#endif /* JSON_WRITER_HH */
