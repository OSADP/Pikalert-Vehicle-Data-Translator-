//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: json_writer.cc,v $
//       Version: $Revision: 1.22 $  Dated: $Date: 2014/04/09 23:01:07 $
//
//==============================================================================

/**
 * @file csv_writer.cc
 *
 * <file description>
 *
 * @date 7/30/13
 */

// Include files 
#include <math.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "alert_conditions.hh"
#include "config_reader.hh"
#include "json_writer.hh"
#include "obs_stats_record.hh"

#ifndef JSON_SPIRIT_VALUE_ENABLED
#error Please define JSON_SPIRIT_VALUE_ENABLED for the Value type to be enabled 
#endif

using std::ofstream;
using namespace json_spirit;
using boost::lexical_cast;

// Constants and macros
const string MISSING_CODE = "missing";
const string CLEAR_CODE = "clear";
const string WARN_CODE = "warning";
const string ALERT_CODE = "alert";
const string ALERT_CODE_STRING = "alert_code";
const string TIME_STRING = "time";
const string TIME_SERIES = "time_series";

const string MISSING_LETTER = "m";
const string CLEAR_LETTER = "c";
const string WARN_LETTER = "w";
const string ALERT_LETTER = "a";

const string CHEMICAL_ALERT_STRING = "chemical";
const string ROAD_TEMPERATURE_ALERT_STRING = "road_temp";
const string DESC = "desc";
const string HR06_ALERT_CODE = "hr06_alert_code";
const string HR06_ALERT_IMAGE = "hr06_alert_image";
const string HR24_ALERT_CODE = "hr24_alert_code";
const string HR24_ALERT_IMAGE = "hr24_alert_image";
const string HR72_ALERT_CODE = "hr72_alert_code";
const string HR72_ALERT_IMAGE = "hr72_alert_image";
const string IS_ROAD_COND_SITE = "is_road_cond_site";
const string IS_RWIS_SITE = "is_rwis_site";
const string IS_WX_OBS_SITE = "is_wx_obs_site";
const string LATITUDE = "lat";
const string LONGITUDE = "lon";
const string OBS_ALERT_CODE = "obs_alert_code";
const string OBS_ALERT_IMAGE = "obs_alert_image";
const string PAVEMENT_ALERT_STRING = "pavement";
const string PLOW_ALERT_STRING = "plow";
const string PRECIP_ALERT_STRING = "precip";
const string SITE_ID = "site_id";
const string SITE_NUM = "site_num";
const string TREATMENT_ALERT_CODE = "treatment_alert_code";
const string TREATMENT_ALERT_IMAGE = "treatment_alert_image";
const string VISIBILITY_ALERT_STRING = "visibility";
 
// Types

// Structures, unions and classes

// Global variables 

// Functions

void alert_code_to_json_text(rdwx_alert_codes alert_code, string &json_alert_code)
{
  switch (alert_code)
    {
    case AC_NA:
    case AC_MISSING:
      json_alert_code = "missing";
      break;

    case AC_CLEAR:
      json_alert_code = "clear";
      break;

    case AC_WARNING:
      json_alert_code = "warning";
      break;

    case AC_ALERT:
      json_alert_code = "alert";
      break;
    }
}

int site_alert_to_json(const site_alert_class &site_alert, json_spirit::mArray &site_array, string &error)
{
  mObject site;
  vector<rdwx_alert_codes> alert_codes;
  string json_alert_code;
  string json_treatment_alert_code;

  site[DESC] = site_alert.description;

  alert_code_to_json_text(site_alert.obs_alert_code, json_alert_code);
  site[OBS_ALERT_CODE] = json_alert_code;

  alert_code_to_json_text(site_alert.hour06_alert_code, json_alert_code);
  site[HR06_ALERT_CODE] = json_alert_code;

  alert_code_to_json_text(site_alert.hour24_alert_code, json_alert_code);
  site[HR24_ALERT_CODE] = json_alert_code;

  alert_code_to_json_text(site_alert.hour72_alert_code, json_alert_code);
  site[HR72_ALERT_CODE] = json_alert_code;

  site[IS_ROAD_COND_SITE] = site_alert.is_road_cond_site;
  site[IS_RWIS_SITE] = site_alert.is_rwis_site;
  site[IS_WX_OBS_SITE] = site_alert.is_wx_obs_site;
  site[LATITUDE] = site_alert.lat;
  site[LONGITUDE] = site_alert.lon;

  site[SITE_ID] = site_alert.site_id.c_str();
  site[SITE_NUM] = site_alert.site_num;
  

  mArray alert_time_series;

  const size_t MAX_TIME_STRING = 13;
  size_t time_series_size = 24;
  if (site_alert.alert_time_series.size() < time_series_size)
    {
      time_series_size = site_alert.alert_time_series.size();
    }

  // printf("DA: site_num: %d, road_seg_num: %d, first two alert codes: %d, %d\n", site_alert.site_num, site_alert.road_segment_number, site_alert.alert_time_series[0].rdwx_alert_code, site_alert.alert_time_series[1].rdwx_alert_code);

  for (size_t i=0; i<time_series_size; i++)
    {
      mObject alert_time_slice;
      alert_code_to_json_text(site_alert.alert_time_series[i].rdwx_alert_code, json_alert_code);
      alert_time_slice[ALERT_CODE_STRING] = json_alert_code;
      alert_time_slice[PRECIP_ALERT_STRING] = site_alert.alert_time_series[i].precip.c_str();
      alert_time_slice[PAVEMENT_ALERT_STRING] = site_alert.alert_time_series[i].pavement.c_str();
      alert_time_slice[PRECIP_ALERT_STRING] = site_alert.alert_time_series[i].precip.c_str();
      alert_time_slice[VISIBILITY_ALERT_STRING] = site_alert.alert_time_series[i].visibility.c_str();

      alert_code_to_json_text(site_alert.alert_time_series[i].treatment_alert_code, json_alert_code);
      alert_time_slice[TREATMENT_ALERT_CODE] = json_alert_code;
      alert_time_slice[PLOW_ALERT_STRING] = site_alert.alert_time_series[i].plow;
      alert_time_slice[CHEMICAL_ALERT_STRING] = site_alert.alert_time_series[i].chemicals;
      alert_time_slice[ROAD_TEMPERATURE_ALERT_STRING] = site_alert.alert_time_series[i].road_temperature;
      time_t alert_time = site_alert.alert_time_series[i].time;
      struct tm *tms = gmtime(&alert_time);
      if (tms == NULL)
	{
	  error = "bad time string for: " + lexical_cast<string>(alert_time);
	  return -1;
	}
      char time_string[MAX_TIME_STRING];
      size_t ret = strftime(time_string, MAX_TIME_STRING, "%Y%m%d%H%M", tms);
      if (ret != MAX_TIME_STRING-1)
	{
	  error = "bad time string for: " + lexical_cast<string>(alert_time);
	  return -1;
	}

      alert_time_slice[TIME_STRING] = time_string;
      alert_time_series.push_back(alert_time_slice);
    }
  
  site[TIME_SERIES] = alert_time_series;
  site_array.push_back(site);
  return 0;
}

int site_alerts_to_json(const std::vector<site_alert_class> &site_alerts, const std::vector<size_t> &site_indices_in_district, json_spirit::mArray &site_array, string &error)
{
  for (size_t i=0; i<site_indices_in_district.size(); i++)
    {
      int ret = site_alert_to_json(site_alerts[site_indices_in_district[i]], site_array, error);
      if (ret < 0)
	{
	  return ret;
	}
    }

  return 0;
}

// Each district has a set of sites
// Each site has site information including alert summary information
// Each site also has time series information

// Create json spirit districts object from date time string and district vehicles map

int create_district_alerts_json(const string &date_time_string, const config_reader &cfg_reader, const vector<site_alert_class> &site_alerts, json_spirit::mObject& district_alerts_json, string &error)
{
  const vector<rwx::district_t> &districts = cfg_reader.districts;

  unordered_map<size_t, vector<size_t> > district_alert_index_map;

  // Find sites in district by looking at their lat-longs
  for (size_t i=0; i<districts.size(); i++)
    {
      for (size_t j=0; j<site_alerts.size(); j++)
	{
	  if (districts[i].min_lat <= site_alerts[j].lat && site_alerts[j].lat <= districts[i].max_lat)
	    {
	      if (districts[i].min_lon <= site_alerts[j].lon && site_alerts[j].lon <= districts[i].max_lon)
		{
		  district_alert_index_map[i].push_back(j);
		}
	    }
	}
    }

  json_spirit::mArray district_array;
  json_spirit::mArray site_array;
  mObject district_obj;
  unordered_map<int, string> hour_summary_map;

  // build district_vehicles object using district_vehicles_map
  for (auto itr=district_alert_index_map.begin(); itr!=district_alert_index_map.end(); ++itr)
    {
      district_obj.clear();
      site_array.clear();
      district_obj["min_lat"] = districts[itr->first].min_lat;
      district_obj["max_lat"] = districts[itr->first].max_lat;
      district_obj["min_lon"] = districts[itr->first].min_lon;
      district_obj["max_lon"] = districts[itr->first].max_lon;
      district_obj["district_name"] = districts[itr->first].district_name;

      const std::vector<size_t> &site_indices_in_district = itr->second;

      int ret = site_alerts_to_json(site_alerts, site_indices_in_district, site_array, error);
      if (ret != 0)
	{
	  return ret;
	}

      district_obj["sites"] = site_array;

      vector<site_alert_class> site_alerts_district;
      for (size_t j=0; j<site_indices_in_district.size(); j++)
	{
	  site_alerts_district.push_back(site_alerts[site_indices_in_district[j]]);
	}
      
      
      vector<rdwx_alert_codes> alert_codes;
      site_alert_class::get_max_summary_codes(site_alerts_district, alert_codes);
      
      string json_alert_code;
      string json_alert_image;

      alert_code_to_json_text(alert_codes[HR00], json_alert_code);
      district_obj["obs_alert_summary_code"] = json_alert_code;

      alert_code_to_json_text(alert_codes[HR06], json_alert_code);
      district_obj["hr06_alert_summary_code"] = json_alert_code;
      
      alert_code_to_json_text(alert_codes[HR24], json_alert_code);
      district_obj["hr24_alert_summary_code"] = json_alert_code;
      
      alert_code_to_json_text(alert_codes[HR72], json_alert_code);
      district_obj["hr72_alert_summary_code"] = json_alert_code;
      

      district_array.push_back(district_obj);
    }

  district_alerts_json["districts"] = district_array;
  district_alerts_json["data_time"] = date_time_string;

  return 0;
}

int create_plot_alerts_json(const string &date_time_string, const config_reader &cfg_reader, const vector<site_alert_class> &site_alerts, json_spirit::mObject& district_alerts_json, string &error)
{
  return 0;
}


// Create both road weather and treatment alerts for all sites.
// The output is used for plotting
void create_all_site_alerts_json(const string &date_time_string, const vector<site_alert_class> &site_alerts, json_spirit::mObject& all_site_alerts_json)
{
  json_spirit::mObject site_object;
  json_spirit::mObject rdwx_object;
  json_spirit::mObject treatment_object;

  // iterate through all site
  for (size_t i=0; i<site_alerts.size(); i++)
    {
      json_spirit::mArray rdwx_alert_array;
      json_spirit::mArray treatment_alert_array;

      // iterate through all alerts for this particular site
      for (size_t j=0; j<site_alerts[i].alert_time_series.size(); j++)
	{
	  rdwx_alert_array.push_back(site_alerts[i].alert_time_series[j].rdwx_alert_code);
	  treatment_alert_array.push_back(site_alerts[i].alert_time_series[j].treatment_alert_code);
	}
      rdwx_object[lexical_cast<string>(site_alerts[i].site_num)] = rdwx_alert_array;
      treatment_object[lexical_cast<string>(site_alerts[i].site_num)] = treatment_alert_array;

      //printf("PLOT: site_num: %d, road_seg_num: %d, first two alert codes: %d, %d\n", site_alerts[i].site_num, site_alerts[i].road_segment_number, site_alerts[i].alert_time_series[0].rdwx_alert_code, site_alerts[i].alert_time_series[1].rdwx_alert_code);

      rdwx_alert_array.clear();
      treatment_alert_array.clear();
    }

  all_site_alerts_json["rdwx_alerts"] = rdwx_object;
  all_site_alerts_json["tmt_alerts"] = treatment_object;
  return;
}

// Create json output file containing district alerts information
int alerts_json_writer(const string &date_time_string, const config_reader &cfg_reader, const string &file_name, const vector<site_alert_class> &site_alerts, string &error)
{
  json_spirit::mObject district_alerts_json;

  int ret = create_district_alerts_json(date_time_string, cfg_reader, site_alerts, district_alerts_json, error);
  if (ret < 0)
    {
      return ret;
    }

  /*
  json_spirit::mArray &district_array = district_alerts_json["districts"].get_array();
  json_spirit::mObject &district_obj = district_array[0].get_obj();
  district_obj["min_lat"] = -2345;
  */

  ofstream out_stream(file_name.c_str());
  if (out_stream)
    {
      write(district_alerts_json, out_stream, remove_trailing_zeros);
    }
  else
    {
      error = "could not open output file: " + file_name;
      return -1;
    }
  return 0;
}

// Create json output file containing district alerts information
int plots_json_writer(const string &date_time_string, const string &file_name, const vector<site_alert_class> &site_alerts, string &error)
{
  json_spirit::mObject all_alerts_json;

  create_all_site_alerts_json(date_time_string, site_alerts, all_alerts_json);
  /*
  json_spirit::mArray &district_array = district_alerts_json["districts"].get_array();
  json_spirit::mObject &district_obj = district_array[0].get_obj();
  district_obj["min_lat"] = -2345;
  */

  ofstream out_stream(file_name.c_str());
  if (out_stream)
    {
      write(all_alerts_json, out_stream, remove_trailing_zeros);
    }
  else
    {
      error = "could not open output file: " + file_name;
      return -1;
    }

  return 0;
}


// Create obs stats json output for roads.
// This output is used for presenting road segment observation data in the display
int create_obs_stats_json(const string &date_time_string, const unordered_map<int, obs_stats_record> &obs_stats_record_map, const unordered_map<int, road_segment_info> &road_segment_map, json_spirit::mObject &obs_stats_json, const site_info *site_info_ptr, string &error)
{
  json_spirit::mObject segment_obs_dict;
  const int MAX_STRING = 32;
  char buf[MAX_STRING];

  // iterate through all obs stats records
  for (auto itr = obs_stats_record_map.begin(); itr!=obs_stats_record_map.end(); ++itr)
    {
      // get last obs_stats_info record
      size_t info_vec_size = itr->second.info_vec.size();
      if (info_vec_size > 0)
	{
	  const obs_stats_info &latest_info = itr->second.info_vec[info_vec_size-1];
	  segment_obs_dict["time"] = latest_info.time;
	  if (latest_info.model_air_temp <= MISSING + 1)
	    {
	      segment_obs_dict["model_air_temp"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f deg F", latest_info.model_air_temp);
	      segment_obs_dict["model_air_temp"] = buf;
	    }

	  if (latest_info.model_bar_press <= MISSING + 1)
	    {
	      segment_obs_dict["model_bar_press"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f mb (%.2f inch Hg)", latest_info.model_bar_press, latest_info.model_bar_press * 0.02953);
	      segment_obs_dict["model_bar_press"] = buf;
	    }

	  if (latest_info.nss_air_temp_mean <= MISSING + 1)
	    {
	      segment_obs_dict["nss_air_temp_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f deg F", latest_info.nss_air_temp_mean);
	      segment_obs_dict["nss_air_temp_mean"] = buf;
	    }

	  if (latest_info.nss_bar_press_mean <= MISSING + 1)
	    {
	      segment_obs_dict["nss_bar_press_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f mb (%.2f inch Hg)", latest_info.nss_bar_press_mean, latest_info.nss_bar_press_mean * 0.02953);
	      segment_obs_dict["nss_bar_press_mean"] = buf;
	    }

	  if (latest_info.radar_ref <= MISSING + 1)
	    {
	      segment_obs_dict["radar_cref"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.2f dBZ", latest_info.radar_ref);
	      segment_obs_dict["radar_cref"] = buf;
	    }

	  if (latest_info.radar_hc <= MISSING + 1)
	    {
	      segment_obs_dict["radar_hydrometeor_classification"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.2f", latest_info.radar_hc);
	      segment_obs_dict["radar_hydrometeor_classification"] = buf;
	    }


	  if (latest_info.radar_hr <= MISSING + 1)
	    {
	      segment_obs_dict["radar_hybrid_reflectivity"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.2f dBZ", latest_info.radar_hr);
	      segment_obs_dict["radar_hybrid_reflectivity"] = buf;
	    }

	  if (latest_info.model_dew_temp <= MISSING + 1)
	    {
	      segment_obs_dict["model_dew_temp"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f deg F", latest_info.model_dew_temp);
	      segment_obs_dict["model_dew_temp"] = buf;
	    }

	  if (latest_info.speed_mean <= MISSING + 1)
	    {
	      segment_obs_dict["vehicle_speed_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f MpH", latest_info.speed_mean);
	      segment_obs_dict["vehicle_speed_mean"] = buf;
	    }
	  if (latest_info.num_wipers_off <= MISSING + 1)
	    {
	      segment_obs_dict["num_wipers_off"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%d", latest_info.num_wipers_off);
	      segment_obs_dict["num_wipers_off"] = buf;
	    }
	  // Sum up all intermittent, low, and high wiper counts
	  int num_wipers_on = 0;
	  if ((latest_info.num_wipers_intermittent <= MISSING + 1) && (latest_info.num_wipers_low <= MISSING + 1) && (latest_info.num_wipers_high <= MISSING + 1))
	    {
	      segment_obs_dict["num_wipers_on"] = "missing";
	    }
	  else{
	    if (latest_info.num_wipers_intermittent >= 0)
	      {
		num_wipers_on = num_wipers_on + latest_info.num_wipers_intermittent;
	      }
	    if (latest_info.num_wipers_low >= 0)
	      {
		num_wipers_on = num_wipers_on + latest_info.num_wipers_low;
	      }
	    if (latest_info.num_wipers_high >= 0)
	      {
		num_wipers_on = num_wipers_on + latest_info.num_wipers_high;
	      }
	    snprintf(buf, MAX_STRING, "%d", num_wipers_on);	    
	    segment_obs_dict["num_wipers_on"] = buf;
	  }
	  
	  if (latest_info.num_msg_valid_speed < 0)
	    {
	      segment_obs_dict["num_msg_valid_speed"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%d", latest_info.num_msg_valid_speed);
	      segment_obs_dict["num_msg_valid_speed"] = buf;
	    }

	  if (latest_info.veh_air_temp_mean <= MISSING + 1)
	    {
	      segment_obs_dict["vehicle_air_temp_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f deg F", latest_info.veh_air_temp_mean);
	      segment_obs_dict["vehicle_air_temp_mean"] = buf;
	    }
	  if (latest_info.veh_bar_pressure_mean <= MISSING + 1)
	    {
	      segment_obs_dict["vehicle_bar_pressure_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f mb (%.2f inch Hg)", latest_info.veh_bar_pressure_mean, latest_info.veh_bar_pressure_mean * 0.02953);
	      segment_obs_dict["vehicle_bar_pressure_mean"] = buf;
	    }
	  if (latest_info.veh_surface_temp_mean <= MISSING + 1)
	    {
	      segment_obs_dict["vehicle_surface_temp_mean"] = "missing";
	    }
	  else
	    {
	      snprintf(buf, MAX_STRING, "%.0f deg F", latest_info.veh_surface_temp_mean);
	      segment_obs_dict["vehicle_surface_temp_mean"] = buf;
	    }
	  
	  
#ifdef NOTNOW	  
	  segment_obs_dict["model_air_temp"] = latest_info.model_air_temp;
	  segment_obs_dict["model_bar_press"] = latest_info.model_bar_press;
	  segment_obs_dict["nss_air_temp_mean"] = latest_info.nss_air_temp_mean;
	  segment_obs_dict["nss_bar_press_mean"] = latest_info.nss_bar_press_mean;
	  segment_obs_dict["radar_ref"] = latest_info.radar_ref;
	  segment_obs_dict["radar_hydrometeor_classification"] = latest_info.radar_hc;
	  segment_obs_dict["radar_hybrid_reflectivity"] = latest_info.radar_hr;
	  segment_obs_dict["model_dew_temp"] = latest_info.model_dew_temp;
	  segment_obs_dict["vehicle_speed_mean"] = latest_info.speed_mean;
	  segment_obs_dict["num_wipers_off"] = latest_info.num_wipers_off;
	  segment_obs_dict["num_wipers_on"] = num_wipers_on;
	  segment_obs_dict["num_msg_valid_speed"] = latest_info.num_msg_valid_speed;
	  segment_obs_dict["vehicle_air_temp_mean"] = latest_info.veh_air_temp_mean;
	  segment_obs_dict["vehicle_bar_pressure_mean"] = latest_info.veh_bar_pressure_mean;	  	  	  
	  segment_obs_dict["vehicle_surface_temp_mean"] = latest_info.veh_surface_temp_mean;	  
#endif
	}

      // find auxiliary road segment id to use as output obs_stats_json key
      auto road_segment_itr = road_segment_map.find(itr->first);
      if (road_segment_itr != road_segment_map.end())
	{
	  // look for generic site id
	  auto generic_id_itr = site_info_ptr->fcst_site_map.find(road_segment_itr->second.aux_id);
	  if (generic_id_itr == site_info_ptr->fcst_site_map.end())
	    {
	      error = "could not find site id in fcst_site_map: " + road_segment_itr->second.aux_id;
	      return -1;
	    }

	  //printf("ids id, wmo_id %d %d\n", generic_id_itr->second.id, generic_id_itr->second.wmo_id);
	  obs_stats_json[lexical_cast<string>(generic_id_itr->second.id)] = segment_obs_dict;

	}
      else
	{
	  error = "could not find site in road segment map: " + itr->first;
	}
    }

  return 0;
}


// Create json output file containing observation stats information for roads
int obs_stats_json_writer(const string &date_time_string, const string &file_name, const unordered_map<int, obs_stats_record> &obs_stats_record_map, const unordered_map<int, road_segment_info> &road_segment_map, const site_info *site_info_ptr, string &error)
{
  json_spirit::mObject obs_stats_json;

  create_obs_stats_json(date_time_string, obs_stats_record_map, road_segment_map, obs_stats_json, site_info_ptr, error);

  ofstream out_stream(file_name.c_str());
  if (out_stream)
    {
      write(obs_stats_json, out_stream, remove_trailing_zeros);
    }
  else
    {
      error = "could not open output file: " + file_name;
      return -1;
    }

  return 0;
}


// Create both road weather and treatment alerts for all sites.
// The output is used for plotting
void create_maw_site_alerts_json(const string &date_time_string, const vector<site_alert_class> &site_alerts, json_spirit::mObject& maw_site_alerts_json)
{
  json_spirit::mObject maw_site_object;

  // iterate through all site
  for (size_t i=0; i<site_alerts.size(); i++)
    {
      json_spirit::mObject maw_site_object;

      if (site_alerts[i].alert_time_series.size() > 0)
	{
	  maw_site_object["maw_precip_action"] = site_alerts[i].alert_time_series[0].maw_precip_action;
	  maw_site_object["maw_precip_action_code"] = site_alerts[i].alert_time_series[0].maw_precip_action_code;
	  maw_site_object["maw_precip_condition"] = site_alerts[i].alert_time_series[0].maw_precip_condition;
	  maw_site_object["maw_precip_condition_code"] = site_alerts[i].alert_time_series[0].maw_precip_condition_code;
	  //printf("site num %d, maw precip_priority %d\n", site_alerts[i].site_num, site_alerts[i].alert_time_series[0].maw_precip_priority);
	  maw_site_object["maw_precip_priority"] = site_alerts[i].alert_time_series[0].maw_precip_priority;

	  maw_site_object["maw_pavement_action"] = site_alerts[i].alert_time_series[0].maw_pavement_action;
	  maw_site_object["maw_pavement_action_code"] = site_alerts[i].alert_time_series[0].maw_pavement_action_code;
	  maw_site_object["maw_pavement_condition"] = site_alerts[i].alert_time_series[0].maw_pavement_condition;
	  maw_site_object["maw_pavement_condition_code"] = site_alerts[i].alert_time_series[0].maw_pavement_condition_code;
	  maw_site_object["maw_pavement_priority"] = site_alerts[i].alert_time_series[0].maw_pavement_priority;

	  maw_site_object["maw_visibility_action"] = site_alerts[i].alert_time_series[0].maw_visibility_action;
	  maw_site_object["maw_visibility_condition"] = site_alerts[i].alert_time_series[0].maw_visibility_condition;
	  maw_site_object["maw_visibility_condition_code"] = site_alerts[i].alert_time_series[0].maw_visibility_condition_code;
	  maw_site_object["maw_visibility_priority"] = site_alerts[i].alert_time_series[0].maw_visibility_priority;
	  maw_site_object["maw_visibility_action_code"] = site_alerts[i].alert_time_series[0].maw_visibility_action_code;
	  maw_site_object["site_num"] = site_alerts[i].site_num;

	  maw_site_alerts_json[lexical_cast<string>(site_alerts[i].road_segment_number)] = maw_site_object;
	}
    }

  return;
}

// Create json output file containing maw phone app information
int maw_json_writer(const string &date_time_string, const string &file_name, const vector<site_alert_class> &site_alerts, string &error)
{
  json_spirit::mObject maw_alerts_json;

  create_maw_site_alerts_json(date_time_string, site_alerts, maw_alerts_json);

  ofstream out_stream(file_name.c_str());
  if (out_stream)
    {
      write(maw_alerts_json, out_stream, remove_trailing_zeros);
    }
  else
    {
      error = "could not open output file: " + file_name;
      return -1;
    }

  return 0;
}


