//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: qc_check.cc,v $
//       Version: $Revision: 1.13 $  Dated: $Date: 2014/06/15 20:54:36 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/qc_check.cc
 *
 *   Quality control check class implementation
 *
 *
 */

// Include files
#include <iostream>
#include <log/log.hh>
#include "qc_check.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt4/vdt_surface_observation.hh"
#include <stdio.h>
#include <time.h>
#include <math.h>

using boost::str;
using boost::format;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables
extern Log *Logg;
extern int Debug_level;

// Functions

int get_month_offset(double obstime)
{
  struct tm * timeinfo;
  const time_t obs_time = (time_t)obstime;

  timeinfo = localtime(&obs_time);

  return (timeinfo->tm_mon);

}

unsigned char range_check( double min, double max, double value)
{
  if ( min <= value && value <= max )
    {
      return vdt_probe_message_qc::PASSED;
    }

  if ( fabs( value - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS )
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  return vdt_probe_message_qc::FAILED;
}

unsigned char range_check(int min, int max, int value)
{
  if ( min <= value && value <= max )
    {
      return vdt_probe_message_qc::PASSED;
    }

  if ( value == vdt_const::SHORT_FILL_VALUE )
    {
      return vdt_probe_message_qc::NOT_APPLICABLE;
    }

  return vdt_probe_message_qc::FAILED;
}

unsigned char climate_range_check(double min, double max, double value)
{
  unsigned char range_check_result;

  if ( ( fabs( min - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS ) || ( fabs( max - vdt_const::FILL_VALUE ) < vdt_const::FILL_VALUE_EPS ) )
    {
      range_check_result = vdt_probe_message_qc::NOT_APPLICABLE;
    }
  else
    {
      range_check_result = range_check(min, max, value);
    }

  return range_check_result;
}

unsigned char climate_range_check(int min, int max, int value)
{
  unsigned char range_check_result;

  if ( ( min == vdt_const::SHORT_FILL_VALUE ) || ( max == vdt_const::SHORT_FILL_VALUE ) )
    {
      range_check_result = vdt_probe_message_qc::NOT_APPLICABLE;
    }
  else
    {
      range_check_result = range_check(min, max, value);
    }

  return range_check_result;
}

void iqr_spatial_check2::init()
{
  // construct air temperature iqr spatial test
  met_spatial_map.insert(std::make_pair("air_temp",vdt_spatial_check_iqr(cfg_reader.spatial_iqr_air_temp_tol, cfg_reader.spatial_iqr_multiplier)));
    
  // construct dew temperature iqr spatial test
  met_spatial_map.insert(std::make_pair("dew_temp", vdt_spatial_check_iqr(cfg_reader.spatial_iqr_dew_temp_tol, cfg_reader.spatial_iqr_multiplier)));
		       
  // construct pressure iqr spatial test
  met_spatial_map.insert(std::make_pair("bar_pressure", vdt_spatial_check_iqr(cfg_reader.spatial_iqr_pressure_tol, cfg_reader.spatial_iqr_multiplier)));

  // construct surface temp iqr spatial test
  met_spatial_map.insert(std::make_pair("surface_temp", vdt_spatial_check_iqr(cfg_reader.spatial_iqr_surface_temp_tol, cfg_reader.spatial_iqr_multiplier)));

  std::vector<std::string> met_vars;
  filter_vars_by_test(cfg_reader, var_test::MET_FIELD_KEY, met_vars);

  //Stash away the variable type and met_field name for each variable having a met_field
  for(const std::string& varname : met_vars)
  {
    const var_test& var = cfg_reader.vartests.at(varname);
    const STR_PIKA_TYPE_MAP& mf_params = var.getTestMap(var_test::MET_FIELD_KEY);
    const std::string& metfname = boost::get<std::string>(mf_params.at(var_test::NAME_KEY));
    var_type_map.insert(std::make_pair(varname, var.type));
    var_met_field_map.insert(std::make_pair(varname, metfname));
  }
}

void iqr_spatial_check2::operator()(vdt_probe_message& msg) const
{
  // get surface obs "close" to probe message location
  double obs_time = msg.getattr<double>("obs_time");
  vector<vdt_surface_observation> near;

  // get nearby surface stations
  datasets->get_near_surface_obs(msg.get_point(), obs_time, cfg_reader.spatial_range_search_km, cfg_reader.spatial_time_search_secs, near);

  // extract air temperature, dew temperature, surface temperature and pressure readings from near observations
  std::map<std::string, vector<float>> met_vec_map = {{"air_temp", vector<float>()}, 
                                                      {"dew_temp", vector<float>()}, 
                                                      {"surface_temp", vector<float>()}, 
                                                      {"bar_pressure", vector<float>()}};

  vector<float>& air_temp = met_vec_map.at("air_temp");
  vector<float>& dew_temp = met_vec_map.at("dew_temp");
  vector<float>& surface_temp = met_vec_map.at("surface_temp");
  vector<float>& pressure = met_vec_map.at("bar_pressure");

  //printf("#in iqr_spatial_check near.size() = %ld\n", near.size());
  // perform iqr spatial tests
  for (size_t i=0; i<near.size(); i++)
    {
      //printf("id, pressure, air temp, dew temp, surface temp values for spatial: %s, %f, %f, %f, %f\n", probe_message.vehicle_id.c_str(), near[i].get_press(), near[i].get_air_temp(), near[i].get_dew_temp(), near[i].get_surface_temp());
      pressure.push_back(near[i].get_press());
      air_temp.push_back(near[i].get_air_temp());
      dew_temp.push_back(near[i].get_dew_temp());
      surface_temp.push_back(near[i].get_surface_temp());
    }

  std::map<std::string, int> iqrmap;
  bool na = false;

  for(const auto& elem : var_type_map)
  {
    const std::string& varname = elem.first;
    const std::string& metfname = var_met_field_map.at(elem.first);
    const pika_type_id ptype = elem.second;

    if(ptype == INT_TYPE)
    {
      iqrmap[varname] = met_spatial_map.at(metfname).perform_iqr(met_vec_map.at(metfname), msg.getattr<int>(varname));
    }
    else
    {
      iqrmap[varname] = met_spatial_map.at(metfname).perform_iqr(met_vec_map.at(metfname), msg.getattr<double>(varname));
    }	

    msg.setattr(varname + "_spatial_iqr_passed",iqrmap[varname]);
    if(iqrmap[varname] ==  vdt_probe_message_qc::NOT_APPLICABLE)
    {
      na = true;
    }
  }
  
  // prepare for running barnes spatial checks
  if (na)
  {
    vector<vdt_point> surface_obs_points;
    vdt_surface_observation::get_points(near, surface_obs_points);
    vector<float> distances;

    vdt_spatial_check_barnes::calc_distances(msg.get_point(), surface_obs_points, distances);

    for(const auto& elem : var_type_map)
    {
      const std::string& varname = elem.first;
      const std::string& metfname = var_met_field_map.at(elem.first);
      const pika_type_id ptype = elem.second;

      if(iqrmap[varname] == vdt_probe_message_qc::NOT_APPLICABLE)
      {
	vdt_spatial_check_barnes spatial_check_barnes(cfg_reader.spatial_barnes_min_size, cfg_reader.spatial_barnes_max_sd);

	if(ptype == INT_TYPE)
	{
	  msg.setattr(varname + "_spatial_barnes_passed", 
		      spatial_check_barnes.perform_barnes(met_vec_map.at(metfname), distances, msg.getattr<int>(varname)));
	}
	else
	{
	  msg.setattr(varname + "_spatial_barnes_passed", 
		      spatial_check_barnes.perform_barnes(met_vec_map.at(metfname), distances, msg.getattr<double>(varname)));
	}
      } 
    }
  }
}

qc_check::qc_check(const config_reader &config_reader_arg, const vdt_climatology_file_reader *climate_data, vdt_probe_message_datasets *probe_message_dataset) : cfg_reader(config_reader_arg), climate_data(climate_data), datasets(probe_message_dataset)
{
}

void vdt_neighbor_check_iqr::operator()(int id, vdt_probe_message_qc &probe_message) const
{
  auto itr = seg_index_offset_map.find(id);
  if (itr == seg_index_offset_map.end())
    {
      return;
    }
  int offset = itr->second;

  for(const auto& elem : var_type_map)
  {
    const std::string& varname = elem.first;
    const std::string& metfname = var_met_field_map.at(elem.first);
    const pika_type_id ptype = elem.second;

    // get probe message iqr values on same segment as probe message
    double iqr25 = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<double>(varname + "_iqr25");
    double iqr75 = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<double>(varname + "_iqr75");
    double median = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<double>(varname + "_median");
    int num_values = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<int>("num_msg_valid_" + varname);

    int iqr_test;

    if(ptype == INT_TYPE)
    {
      iqr_test = met_spatial_map.at(metfname).perform_iqr(num_values, static_cast<float>(iqr25), static_cast<float>(iqr75), static_cast<float>(median), probe_message.getattr<int>(varname));
    }
    else
    {
      iqr_test = met_spatial_map.at(metfname).perform_iqr(num_values, static_cast<float>(iqr25), static_cast<float>(iqr75), static_cast<float>(median), static_cast<float>(probe_message.getattr<double>(varname)));
    }	
    
    probe_message.setattr(varname + "_nvt_passed", iqr_test);
  }
}

void vdt_model_check::init()
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& mat_paramap = test.second.getTestMap(var_test::MODEL_ANALYSIS_TEST_KEY);
      const STR_PIKA_TYPE_MAP& mf_paramap = test.second.getTestMap(var_test::MET_FIELD_KEY);

      if(!mat_paramap.empty() && !mf_paramap.empty())
      {
	const std::string& varname = test.second.name;
	const pika_type_id ptype = test.second.type;

	const PIKA_TYPE& pika_delta = mat_paramap.at(var_test::DELTA_KEY);
	if(pika_typeid(pika_delta) != DOUBLE_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::DELTA_KEY) + " must be a floating point value. Check config file.");
	}

	const double delta = boost::get<double>(pika_delta);
	var_delta_map[varname] = delta;
	
	var_type_map.insert(std::make_pair(varname, ptype));

	const std::string& metfname = boost::get<std::string>(mf_paramap.at(var_test::NAME_KEY));
	var_met_field_map.insert(std::make_pair(varname, metfname));
	model_set.insert(metfname);
      }
    }
  }
  catch(std::out_of_range)
  {
    throw VDT_EXCEPTION(str(format("Could not find %1% or %2%. Check all params in config file.") % var_test::DELTA_KEY % var_test::NAME_KEY));
  }
}

void vdt_model_check::operator()(int id, vdt_probe_message_qc &probe_message) const
{
  // future approach
  // get model values on same segment as probe message
  // float model_air_temp = road_seg_stats[offset].model_air_temp;
  // float model_dewpoint = road_seg_stats[offset].model_dewpoint;
  // float model_bar_press = road_seg_stats[offset].model_bar_press;

  std::map<std::string, double> met_model_value;
  for(const std::string& metfname : model_set)
  {
    met_model_value[metfname] = probe_message.getattr<double>("model_" + metfname);
  }

  for(const auto& elem : var_met_field_map)
  {
    const std::string& varname = elem.first;
    const std::string& metfname = elem.second;
    const double delta = var_delta_map.at(varname);
    const double model_value = met_model_value.at(metfname);
    const pika_type_id ptype = var_type_map.at(varname);

    double value;

    if(ptype == INT_TYPE)
    {
      value = probe_message.getattr<int>(varname);
    }
    else
    {
      value = probe_message.getattr<double>(varname);
    }

    if (model_value != vdt_const::FILL_VALUE)
    {
      if (value == vdt_const::FILL_VALUE)
      {
	probe_message.setattr(varname + "_mat_passed", vdt_probe_message_qc::NOT_APPLICABLE);
      }
      else if ((model_value - delta) <= value && value <= (model_value + delta))
      {
	probe_message.setattr(varname + "_mat_passed", vdt_probe_message_qc::PASSED);
      }
      else
      {
	probe_message.setattr(varname + "_mat_passed", vdt_probe_message_qc::FAILED);
      }
    }
  }
 }

void vdt_stdev_check::init()
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& paramap = test.second.getTestMap(var_test::STD_DEV_TEST_KEY);
      if(!paramap.empty())
      {
	const std::string& varname = test.second.name;
	const pika_type_id ptype = test.second.type;

	const PIKA_TYPE& pika_mult = paramap.at(var_test::MULTIPLIER_KEY);
	if(pika_typeid(pika_mult) != DOUBLE_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::MULTIPLIER_KEY) + " must be a floating point value. Check config file.");
	}
	double mult = boost::get<double>(pika_mult);
	var_mult_map[varname] = mult;
	
	var_type_map.insert(std::make_pair(varname, ptype));
      }
    }
  }
  catch(std::out_of_range)
  {
    throw VDT_EXCEPTION(str(format("Could not find %1%. Check all params in config file.") % var_test::MULTIPLIER_KEY));
  }
}

void vdt_stdev_check::operator()(int id, vdt_probe_message_qc &probe_message) const
{
  auto itr = seg_index_offset_map.find(id);
  if (itr == seg_index_offset_map.end())
    {
      return;
    }
  int offset = itr->second;

  for(const auto& elem : var_mult_map)
  {
    const std::string& varname = elem.first;
    const pika_type_id ptype = var_type_map.at(varname);

    double mult = elem.second;
    double mean = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<double>(varname + "_mean");
    double stdev = const_cast<vdt_probe_message_qc_statistics&>(road_seg_stats[offset]).getattr<double>(varname + "_stdev");
    double value;

    switch(ptype)
    {
    case INT_TYPE:
      value = static_cast<double>(probe_message.getattr<int>(varname));
      break;
    case DOUBLE_TYPE:
      value = probe_message.getattr<double>(varname);
      break;
    default:
      throw VDT_EXCEPTION(varname + " must be an int or float to run stdev tests on it.");
    }
    double delta = mult * stdev;

    if (value == vdt_const::FILL_VALUE)
    {
      probe_message.setattr(varname + "_sdt_passed", vdt_probe_message_qc::NOT_APPLICABLE);
    }
    else if ((mean - delta) <= value && value <= (mean + delta))
    {
      probe_message.setattr(varname + "_sdt_passed", vdt_probe_message_qc::PASSED);
    }
    else
    {
      probe_message.setattr(varname + "_sdt_passed", vdt_probe_message_qc::FAILED);
    }
  }
}

void vdt_combined_check::init()
{
  std::vector<std::string> met_vars;
  filter_vars_by_test(cfg_reader, var_test::MET_FIELD_KEY, met_vars);

  //Stash away the variable type and met_field name for each variable having a met_field
  for(const std::string& varname : met_vars)
  {
    const var_test& var = cfg_reader.vartests.at(varname);
    const STR_PIKA_TYPE_MAP& mf_params = var.getTestMap(var_test::MET_FIELD_KEY);
    const std::string& metfname = boost::get<std::string>(mf_params.at(var_test::NAME_KEY));
    var_met_field_map.insert(std::make_pair(varname, metfname));
  }
}

void vdt_combined_check::operator()(vdt_probe_message_qc &probe_message) const
{
  // Note that the combined algorithm test by default is set to
  // NOT_APPLICABLE in the vdt_probe_message_qc constructor
  //
  // if range_qc_passed = 0, then cat = 0
  //    else:
  // cat = 0.1*step+0.2*spatial+0.15*persistence+0.15*CRT 
  // 
  
  enum TESTS {STEP, SPATIAL, PERSIST, CRT, TEST_LEN};

  for(const auto& elem : var_met_field_map)
  {
    const std::string& varname = elem.first;

    probe_message.setattr(varname + "_cat_passed", vdt_probe_message_qc::FAILED);

    if (probe_message.getattr<int>(varname + "_range_qc_passed") != vdt_probe_message_qc::FAILED)
    {
      vector<float> weights = cfg_reader.combined_algorithm_weights;
      vector<float> tests(TEST_LEN, 0);
      float sum_weights;
      float sum_tests;
      float ratio;

      const int step_passed = probe_message.getattr<int>(varname + "_step_passed");
      if (step_passed == vdt_probe_message_qc::PASSED)
      {
	tests[STEP] = 1;
      }
      else if (step_passed == vdt_probe_message_qc::NOT_APPLICABLE)
      {
	weights[STEP] = 0;
      }

      const int spatial_passed = probe_message.getattr<int>(varname + "_spatial_iqr_passed");
      if (spatial_passed == vdt_probe_message_qc::PASSED)
      {
	tests[SPATIAL] = 1;
      }
      else if (spatial_passed == vdt_probe_message_qc::NOT_APPLICABLE)
      {
	if (probe_message.getattr<int>(varname + "_spatial_barnes_passed") == vdt_probe_message_qc::PASSED)
	{
	  tests[SPATIAL] = 1;
	}
	else
	{
	  weights[SPATIAL] = 0;
	}
      }

      const int persist_passed = probe_message.getattr<int>(varname + "_persist_passed");
      if (persist_passed == vdt_probe_message_qc::PASSED)
      {
	tests[PERSIST] = 1;
      }
      else if (persist_passed == vdt_probe_message_qc::NOT_APPLICABLE)
      {
	weights[PERSIST] = 0;
      }

      const int crt_passed = probe_message.getattr<int>(varname + "_crt_passed");
      if (crt_passed == vdt_probe_message_qc::PASSED)
      {
	tests[CRT] = 1;
      }
      else if (crt_passed == vdt_probe_message_qc::NOT_APPLICABLE)
      {
	weights[CRT] = 0;
      }

      sum_weights = 0;
      sum_tests = 0;
      
      for (size_t i=0; i<TEST_LEN; i++)
      {
	sum_weights += weights[i];
	sum_tests += weights[i] * tests[i];
      }
      if (sum_weights == 0)
      {
	probe_message.setattr(varname + "_cat_passed", vdt_probe_message_qc::NOT_APPLICABLE);
	probe_message.setattr(varname + "_cat_confidence", vdt_probe_message_qc::BAD_OR_MISSING_VALUE);
      }
      else
      {
	ratio = sum_tests/sum_weights;
	if (ratio >= cfg_reader.combined_algorithm_threshold)
	{
	  probe_message.setattr(varname + "_cat_passed", vdt_probe_message_qc::PASSED);
	}

	probe_message.setattr(varname + "_cat_confidence", ratio);
      }
    }
  }
}
