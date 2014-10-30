//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: treatment_record.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file treatment_record.cc
 *
 * <file description>
 *
 * @date 10/16/13
 */

// Include files 
#include <algorithm>
#include <math.h>
#include "treatment_record.hh"

// Constants and macros
const int SECONDS_PER_DAY = 3600 * 24;

// Types

// Structures, unions and classes

// Global variables 

// Functions

const std::string treatment_record::forecast_times_per_day = "fc_times_per_day";
const std::string treatment_record::num_days = "days";
const std::string treatment_record::forecast_time_string = "forc_time";
const std::string treatment_record::site_num_string = "site_list";
const std::string treatment_record::do_plowing_string = "do_plowing";
const std::string treatment_record::apply_chemical_string = "apply_chem";
const std::string treatment_record::road_temperature_string = "road_TempF";

#ifdef NOTNOW
int treatment_info::get_value(treatment_info_value_types value_type, double *value) const
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

bool time_compare(treatment_info info1, treatment_info info2)
{
  return (info1.time < info2.time);
}


// Note that the times in treatment_record for a particular site id
// will be in increasing order
int create_treatment_record_map(int site_num_na, const rwx_vector_collection_nc *treatment_collection, std::unordered_map<int, treatment_record> &treatment_record_map, string &error)
{
  const vector<double> *forecast_time_values;
  const vector<int> *site_num;
  const vector<float> *apply_chemical;
  const vector<float> *do_plowing;
  const vector<float> *road_temperature;

  int ret = 0;

  // Get forecast times per day and number days to generate time vector

  // Extract vectors from the vector collection

  ret = treatment_collection->get_vector(treatment_record::road_temperature_string, &road_temperature);
  if (ret != 0)
    {
      error = string("failed getting var: ") + treatment_record::road_temperature_string;
      return ret;
    }
  ret = treatment_collection->get_vector(treatment_record::forecast_time_string, &forecast_time_values);
  if (ret != 0)
    {
      error = string("failed getting var: ") + treatment_record::forecast_time_string;
      return ret;
    }
  ret = treatment_collection->get_vector(treatment_record::site_num_string, &site_num);
  if (ret != 0)
    {
      error = string("failed getting var: ") + treatment_record::site_num_string;
      return ret;
    }
  ret = treatment_collection->get_vector(treatment_record::apply_chemical_string, &apply_chemical);
  if (ret != 0)
    {
      error = string("failed getting var: ") + treatment_record::apply_chemical_string;
      return ret;
    }
  ret = treatment_collection->get_vector(treatment_record::do_plowing_string, &do_plowing);
  if (ret != 0)
    {
      error = string("failed getting var: ") + treatment_record::do_plowing_string;
      return ret;
    }

  const std::unordered_map<string, size_t> &treatment_dim_map = treatment_collection->get_dim_map();
  

  time_t forecast_start_time = static_cast<time_t>((*forecast_time_values)[0]);
  int forecast_times_per_day = 0;
  int num_days = 0;

  auto times_per_day_itr = treatment_dim_map.find(treatment_record::forecast_times_per_day);
  if (times_per_day_itr == treatment_dim_map.end())
    {
      error = string("failed getting forecast times per day dimension");
      return -1;
    }
  else
    {
      forecast_times_per_day = times_per_day_itr->second;
    }

  auto num_days_itr = treatment_dim_map.find(treatment_record::num_days);
  if (num_days_itr == treatment_dim_map.end())
    {
      error = string("failed getting number days dimension");
      return -1;
    }
  else
    {
      num_days = num_days_itr->second;
    }

  // Establish set of records for each site
  int time_delta = SECONDS_PER_DAY / forecast_times_per_day;
  
  for (size_t i=0; i<(*site_num).size(); i++)
    {
      int num = (*site_num)[i];
      if (num == site_num_na)
	{
	  continue;
	}

      treatment_record record;
      record.site_num = num;
      size_t offset = i * num_days * forecast_times_per_day;
      time_t time_value = forecast_start_time;

      for (int j=0; j<num_days; j++)
	{
	  for (int k=0; k<forecast_times_per_day; k++)
	    {
	      treatment_info info;
	      info.time = time_value;
	      info.apply_chemical = (*apply_chemical)[offset];
	      info.do_plowing = (*do_plowing)[offset];
	      info.road_temperature = (*road_temperature)[offset];
	      record.info_vec.push_back(info);
	      time_value += time_delta;
	      offset += 1;
	    }
	}

      // the treatment record is now complete so store in treatment record map
      treatment_record_map[num] = record;
    }

  // Sort the records for each site into time ascending order
  for (auto itr=treatment_record_map.begin(); itr!=treatment_record_map.end(); ++itr)
    {
      std::sort(itr->second.info_vec.begin(), itr->second.info_vec.end(), time_compare);
    }


  return 0;
}

// incorporate treatments in site alerts
void incorporate_treatments(int site_offset, const std::unordered_map<int, treatment_record> &treatment_record_map, vector<site_alert_class> &site_alerts)
{
  //printf("site offset %d\n", site_offset);

  int site_number = site_alerts[site_offset].site_num;

  auto treatment_itr = treatment_record_map.find(site_number);
  //printf("looking for site number %d treatment record map\n", site_number);
  if (treatment_itr != treatment_record_map.end())
    {
      const vector<treatment_info> &info_vec = treatment_itr->second.info_vec;
      std::unordered_map<time_t, size_t> treatment_time_map;
      for (size_t i=0; i<info_vec.size(); i++)
	{
	  //printf("treatment time %ld\n", info_vec[i].time);
	  treatment_time_map[info_vec[i].time] = i;
	}


      vector<wx_time_alert> &alert_time_series = site_alerts[site_offset].alert_time_series;

      // iterate through alert time series matching up treatment times with alert times
      for (size_t i=0; i<alert_time_series.size(); i++)
	{
	  time_t alert_time = alert_time_series[i].time;
	  alert_time_series[i].treatment_alert_code = AC_NA;
	  alert_time_series[i].chemicals = "";
	  alert_time_series[i].plow = "";
	  alert_time_series[i].road_temperature = MISSING;

	  time_t rounded_alert_time = (alert_time/3600) * 3600;
	  //printf("trying to find hourly alert_time %ld in treatments\n", rounded_alert_time);
	  auto treatment_time_itr = treatment_time_map.find(rounded_alert_time);
	  if (treatment_time_itr != treatment_time_map.end())
	    {
	      size_t offset = treatment_time_itr->second;
	      const treatment_info &info = info_vec[offset];
	      //printf("treatment: matched rounded time at offset %ld\n", offset);
	      alert_time_series[i].plow = "none";
	      alert_time_series[i].treatment_alert_code = AC_CLEAR;
	      if (info.do_plowing)
		{
		  alert_time_series[i].plow = "plow";
		  alert_time_series[i].treatment_alert_code = AC_ALERT;
		}


	      alert_time_series[i].chemicals = "none";
	      if (info.apply_chemical)
		{
		  alert_time_series[i].chemicals = "apply chem";
		  alert_time_series[i].treatment_alert_code = AC_ALERT;
		}

	      if (info.road_temperature != NC_FILL_FLOAT)
		{
		  alert_time_series[i].road_temperature = rintf(info.road_temperature);
		}
	    }
	}
    }
}
