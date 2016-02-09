//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: obs_stats_record.cc,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file obs_stats_record.cc
 *
 * <file description>
 *
 * @date 10/16/13
 */

// Include files 
#include <algorithm>
#include "obs_stats_record.hh"

// Constants and macros
const int SECONDS_PER_DAY = 3600 * 24;

// Types

// Structures, unions and classes

// Global variables 

// Functions

const std::string obs_stats_record::id_string = "id";
const std::string obs_stats_record::begin_time_string = "begin_time";
const std::string obs_stats_record::end_time_string = "end_time";
const std::string obs_stats_record::model_air_temp_string = "model_air_temp";
const std::string obs_stats_record::model_bar_press_string = "model_bar_pressure";
const std::string obs_stats_record::nss_air_temp_mean_string = "nss_air_temp_mean";
const std::string obs_stats_record::nss_bar_press_mean_string = "nss_bar_pressure_mean";
const std::string obs_stats_record::radar_ref_string = "radar_ref";

#ifdef NOTNOW
int obs_stats_info::get_value(obs_stats_info_value_types value_type, double *value) const
{
  int ret = -1;
  switch (value_type)
    {
    case RI_TIME_NOMINAL:
      *value = time_nominal;
      ret = 0;
      break;

    case RI_ROAD_SEGMENT_ID:
      *value = road_segment_id;
      ret = 0;
      break;

    case RI_PRECIP_TYPE:
      //printf("getting precip_type\n");
      *value = precip_type;
      ret = 0;
      break;

    case RI_PRECIP_TYPE_CONFIDENCE:
      *value = precip_type_confidence;
      ret = 0;
      break;

    case RI_PRECIP_INTENSITY:
      //printf("getting precip_intesity\n");
      *value = precip_intensity;
      ret = 0;
      break;

    case RI_PRECIP_INTENSITY_CONFIDENCE:
      *value = precip_intensity_confidence;
      ret = 0;
      break;

    case RI_PAVEMENT_CONDITION:
      //printf("getting pavement cond\n");
      *value = pavement_condition;
      ret = 0;
      break;

    case RI_PAVEMENT_CONDITION_CONFIDENCE:
      *value = pavement_condition_confidence;
      ret = 0;
      break;

    case RI_VISIBILITY:
      //printf("getting visibility\n");
      *value = visibility;
      ret = 0;
      break;

    case RI_VISIBILITY_CONFIDENCE:
      *value = visibility_confidence;
      ret = 0;
      break;

    case RI_PAVEMENT_SLICKNESS:
      //printf("getting slickness\n");
      *value = pavement_slickness;
      ret = 0;
      break;
    }
  
  return ret;
}
#endif

bool time_compare(obs_stats_info info1, obs_stats_info info2)
{
  return (info1.time < info2.time);
}


inline float celsius_to_fahrenheit(float celsius)
{
  float fahrenheit = (180./100.) * celsius + 32.0;
  return fahrenheit;
}

// Create map mapping site numbers to their obs stats records
// Note that the times in obs_stats_record for a particular site id
// will be in increasing order
int create_obs_stats_record_map(int site_num_na, const rwx_vector_collection_nc *obs_stats_collection, std::unordered_map<int, obs_stats_record> &obs_stats_record_map, string &error)
{
  const vector<double> *begin_time;
  const vector<double> *end_time;
  const vector<int> *site_num;
  const vector<float> *model_air_temp;
  const vector<float> *model_bar_press;
  const vector<float> *nss_air_temp_mean;
  const vector<float> *nss_bar_press_mean;
  const vector<float> *radar_ref;


  int ret = 0;

  // Extract vectors from the vector collection

  ret = obs_stats_collection->get_vector(obs_stats_record::id_string, &site_num);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::id_string;
      return ret;
    }

  ret = obs_stats_collection->get_vector(obs_stats_record::begin_time_string, &begin_time);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::begin_time_string;
      return ret;
    }

  ret = obs_stats_collection->get_vector(obs_stats_record::model_air_temp_string, &model_air_temp);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::model_air_temp_string;
      return ret;
    }

  rwx::value_t model_air_temp_missing_value;
  int missing_ret = obs_stats_collection->get_missing(obs_stats_record::model_air_temp_string, model_air_temp_missing_value);
  if (missing_ret != 0)
    {
      error = string("failed getting missing value for variable: ") + obs_stats_record::model_air_temp_string;
      return ret;
    }

  float model_air_temp_missing = model_air_temp_missing_value.v_float;

  ret = obs_stats_collection->get_vector(obs_stats_record::model_bar_press_string, &model_bar_press);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::model_bar_press_string;
      return ret;
    }

  ret = obs_stats_collection->get_vector(obs_stats_record::nss_air_temp_mean_string, &nss_air_temp_mean);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::nss_air_temp_mean_string;
      return ret;
    }

  rwx::value_t nss_air_temp_mean_missing_value;
  missing_ret = obs_stats_collection->get_missing(obs_stats_record::nss_air_temp_mean_string, nss_air_temp_mean_missing_value);
  if (missing_ret != 0)
    {
      error = string("failed getting missing value for variable: ") + obs_stats_record::nss_air_temp_mean_string;
      return ret;
    }
  float nss_air_temp_mean_missing = nss_air_temp_mean_missing_value.v_float;

  ret = obs_stats_collection->get_vector(obs_stats_record::nss_bar_press_mean_string, &nss_bar_press_mean);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::nss_bar_press_mean_string;
      return ret;
    }

  ret = obs_stats_collection->get_vector(obs_stats_record::radar_ref_string, &radar_ref);
  if (ret != 0)
    {
      error = string("failed getting var: ") + obs_stats_record::radar_ref_string;
      return ret;
    }

  const std::unordered_map<string, size_t> &obs_stats_dim_map = obs_stats_collection->get_dim_map();
  

  rwx::value_t missing_site_num_value;

  ret = obs_stats_collection->get_missing(obs_stats_record::id_string, missing_site_num_value);

  for (size_t i=0; i<(*site_num).size(); i++)
    {
      int num = (*site_num)[i];
      if (num == site_num_na)
	{
	  continue;
	}

      obs_stats_record record;
      record.site_num = num;

      obs_stats_info info;
      info.time = static_cast<time_t>((*begin_time)[i]);
      info.model_air_temp = (*model_air_temp)[i];
      if (info.model_air_temp != model_air_temp_missing)
	{
	  info.model_air_temp = celsius_to_fahrenheit(info.model_air_temp);
	}

      info.model_bar_press = (*model_bar_press)[i];

      info.nss_air_temp_mean = (*nss_air_temp_mean)[i];
      if (info.nss_air_temp_mean != nss_air_temp_mean_missing)
	{
	  info.nss_air_temp_mean =  celsius_to_fahrenheit(info.nss_air_temp_mean);
	}

      info.nss_bar_press_mean = (*nss_bar_press_mean)[i];
      info.radar_ref = (*radar_ref)[i];

      // Check to see if the site number is in the obs stats record map
      auto itr = obs_stats_record_map.find(num);
      if (itr == obs_stats_record_map.end())
	{
	  record.info_vec.push_back(info);
	  obs_stats_record_map[num] = record;
	  itr = obs_stats_record_map.find(num);
	}

      // store the info structure in info_vec
      itr->second.info_vec.push_back(info);

    }

  // Sort the records for each site into time ascending order
  for (auto itr=obs_stats_record_map.begin(); itr!=obs_stats_record_map.end(); ++itr)
    {
      std::sort(itr->second.info_vec.begin(), itr->second.info_vec.end(), time_compare);
    }

  return 0;
}

#ifdef NOTNOW
// incorporate observation statistics in site alerts
void incorporate_obs_stats(int site_offset, const std::unordered_map<int, obs_stats_record> &obs_stats_record_map, vector<site_alert_class> &site_alerts)
{
  int site_number = site_alerts[site_offset].site_num;

  auto obs_stats_itr = obs_stats_record_map.find(site_number);
  //printf("looking for site number %d treatment record map\n", site_number);
  if (obs_stats_itr != obs_stats_record_map.end())
    {
      const vector<obs_stats_info> &info_vec = obs_stats_itr->second.info_vec;
      std::unordered_map<time_t, size_t> obs_stats_time_map;
      for (size_t i=0; i<info_vec.size(); i++)
	{
	  //printf("obs_stats time %ld\n", info_vec[i].time);
	  obs_stats_time_map[info_vec[i].time] = i;
	}


      vector<wx_time_alert> &alert_time_series = site_alerts[site_offset].alert_time_series;

      // iterate through alert time series matching up obs_stats times with alert times
      for (size_t i=0; i<alert_time_series.size(); i++)
	{
	  time_t alert_time = alert_time_series[i].time;
	  alert_time_series[i].obs_stats_alert_code = AC_NA;
	  alert_time_series[i].chemicals = "";
	  alert_time_series[i].plow = "";
	  alert_time_series[i].road_temperature = MISSING;

	  time_t rounded_alert_time = (alert_time/3600) * 3600;
	  //printf("trying to find hourly alert_time %ld in obs_statss\n", rounded_alert_time);
	  auto obs_stats_time_itr = obs_stats_time_map.find(rounded_alert_time);
	  if (obs_stats_time_itr != obs_stats_time_map.end())
	    {
	      size_t offset = obs_stats_time_itr->second;
	      const obs_stats_info &info = info_vec[offset];
	      //printf("obs_stats: matched rounded time at offset %ld\n", offset);
	      alert_time_series[i].plow = "none";
	      alert_time_series[i].obs_stats_alert_code = AC_CLEAR;
	      if (info.do_plowing)
		{
		  alert_time_series[i].plow = "plow";
		  alert_time_series[i].obs_stats_alert_code = AC_ALERT;
		}


	      alert_time_series[i].chemicals = "none";
	      if (info.apply_chemical)
		{
		  alert_time_series[i].chemicals = "apply chem";
		  alert_time_series[i].obs_stats_alert_code = AC_ALERT;
		}

	      if (info.road_temperature != NC_FILL_FLOAT)
		{
		  alert_time_series[i].road_temperature = rintf(info.road_temperature);
		}
	    }
	}
    }
}
#endif
