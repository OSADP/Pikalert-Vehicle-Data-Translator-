//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwh_record.cc,v $
//       Version: $Revision: 1.10 $  Dated: $Date: 2014/03/31 20:43:11 $
//
//==============================================================================

/**
 * @file rwh_record.cc
 *
 * <file description>
 *
 * @date 10/16/13
 */

// Include files 
#include <algorithm>
#include "rwh_record.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

const std::string rwh_record::time_nominal_string = "time_nominal";
const std::string rwh_record::road_segment_id_string = "road_segment_id";
const std::string rwh_record::precip_type_string = "precip_type";
const std::string rwh_record::precip_type_confidence_string = "precip_type_confidence";
const std::string rwh_record::precip_intensity_string = "precip_intensity";
const std::string rwh_record::precip_intensity_confidence_string = "precip_intensity_confidence";
const std::string rwh_record::pavement_condition_string = "pavement_condition";
const std::string rwh_record::pavement_condition_confidence_string = "pavement_condition_confidence";
const std::string rwh_record::visibility_string = "visibility";
const std::string rwh_record::visibility_confidence_string = "visibility_confidence";
const std::string rwh_record::pavement_slickness_string = "pavement_slickness";

int rwh_info::get_value(rwh_info_value_types value_type, double *value) const
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

bool time_compare(rwh_info info1, rwh_info info2)
{
  return (info1.time_nominal < info2.time_nominal);
}


// Note that the times in rwh_record for a particular site id will be in increasing order
int create_rwh_record_map(int road_segment_na, const rwx_vector_collection_nc &rwh_collection, std::unordered_map<int, rwh_record> &rwh_record_map, string &error)
{
  const vector<double> *time_nominal;
  const vector<int> *road_segment_id;
  const vector<short> *precip_type;
  const vector<float> *precip_type_confidence;
  const vector<short> *precip_intensity;
  const vector<float> *precip_intensity_confidence;
  const vector<short> *pavement_condition;
  const vector<float> *pavement_condition_confidence;
  const vector<short> *visibility;
  const vector<float> *visibility_confidence;
  const vector<short> *pavement_slickness;

  int ret = 0;

  // Extract vectors from the vector collection

  ret = rwh_collection.get_vector(rwh_record::time_nominal_string, &time_nominal);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::time_nominal_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::road_segment_id_string, &road_segment_id);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::road_segment_id_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::precip_type_string, &precip_type);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::precip_type_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::precip_type_confidence_string, &precip_type_confidence);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::precip_type_confidence_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::precip_intensity_string, &precip_intensity);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::precip_intensity_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::precip_intensity_confidence_string, &precip_intensity_confidence);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::precip_intensity_confidence_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::pavement_condition_string, &pavement_condition);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::pavement_condition_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::pavement_condition_confidence_string, &pavement_condition_confidence);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::pavement_condition_confidence_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::visibility_string, &visibility);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::visibility_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::visibility_confidence_string, &visibility_confidence);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::visibility_confidence_string;
      return ret;
    }
  ret = rwh_collection.get_vector(rwh_record::pavement_slickness_string, &pavement_slickness);
  if (ret != 0)
    {
      error = string("failed getting var: ") + rwh_record::pavement_slickness_string;
      return ret;
    }

  // Establish set of records for each site

  for (size_t i=0; i<(*time_nominal).size(); i++)
    {
      rwh_info info;

      info.time_nominal = (*time_nominal)[i];
      info.road_segment_id = (*road_segment_id)[i];
      info.precip_type = (*precip_type)[i];
      info.precip_type_confidence = (*precip_type_confidence)[i];
      info.precip_intensity = (*precip_intensity)[i];
      info.precip_intensity_confidence = (*precip_intensity_confidence)[i];
      info.pavement_condition = (*pavement_condition)[i];
      info.pavement_condition_confidence = (*pavement_condition_confidence)[i];
      info.visibility = (*visibility)[i];
      info.visibility_confidence = (*visibility_confidence)[i];
      info.pavement_slickness = (*pavement_slickness)[i];

      int id = info.road_segment_id;
      if (id == road_segment_na)
	{
	  continue;
	}
      auto itr = rwh_record_map.find(id);
      if (itr == rwh_record_map.end())
	{
	  rwh_record record;

	  record.site_num = id;
	  record.info_vec.push_back(info);
	  rwh_record_map[id] = record;
	}
      else
	{
	  itr->second.info_vec.push_back(info);
	}
    }

  // Sort the records for each site into time ascending order
  for (auto itr=rwh_record_map.begin(); itr!=rwh_record_map.end(); ++itr)
    {
      std::sort(itr->second.info_vec.begin(), itr->second.info_vec.end(), time_compare);
    }


  return 0;
}
