//==============================================================================
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright �2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//==============================================================================

// Include files
#include <string>
#include <vector>
#include <stdio.h>

#include <boost/format.hpp>

#include <log/log.hh>

#include "vdt4/vdt_probe_message.hh"
#include "vdt4/vdt_probe_message_qc.hh"
#include "vdt4/vdt_probe_message_history.hh"
#include "vdt4/vdt_history_check.hh"
#include "vdt4/vdt_spatial_check.hh"
#include "vdt4/vdt_climatology_file_reader.hh"

#include "vdt4/stat_tools.hh"

#include "vdt_probe_data_qc.hh"
#include "config_reader.hh"
#include "qc_check.hh"

using namespace std;
using boost::format;
using boost::str;

// Global variables
extern Log *Logg;
extern int Debug_level;

const std::string EXCLUDE_PREFIX = "exclude";

vdt_probe_data_qc::vdt_probe_data_qc(const config_reader &cfg_reader_arg, const vdt_climatology_file_reader *climatology_data_ptr, vdt_probe_message_datasets *probe_message_datasets) : cfg_reader(cfg_reader_arg), climate_data(climatology_data_ptr), datasets(probe_message_datasets)
{
}

void vdt_probe_data_qc::build_persistence_tests(time_t begin_time, time_t end_time, vector<HISTORY_CHECK_PTR>& history_check)const
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& paramap = test.second.getTestMap(var_test::PERSISTENCE_TEST_KEY);
      if(!paramap.empty())
      {
	const PIKA_TYPE& time = paramap.at(var_test::TIME_KEY);
	if(pika_typeid(time) != INT_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::TIME_KEY) + " must be an integer value. Check config file.");
	}

	switch(test.second.type)
	{
	    case INT_TYPE:
	      history_check.push_back(HISTORY_CHECK_PTR(new vdt_persist_check<int>(test.second.name, begin_time, end_time, boost::get<int>(time))));
	      break;
	    case DOUBLE_TYPE:
	      history_check.push_back(HISTORY_CHECK_PTR(new vdt_persist_check<double>(test.second.name, begin_time, end_time, boost::get<int>(time))));
	      break;
	    default:
	      throw VDT_EXCEPTION(test.second.name + " must be an int or float to run persistence tests on it.");
	}
      }
    }
  }
  catch(std::out_of_range)
  {
    throw VDT_EXCEPTION(str(format("Could not find %1%. Check all params in config file.") % var_test::TIME_KEY));
  }
}

void vdt_probe_data_qc::build_step_tests(time_t begin_time, time_t end_time, vector<HISTORY_CHECK_PTR>& history_check)const
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& paramap = test.second.getTestMap(var_test::TIME_STEP_TEST_KEY);
      if(!paramap.empty())
      {
	const PIKA_TYPE& time = paramap.at(var_test::TIME_KEY);
	if(pika_typeid(time) != INT_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::TIME_KEY) + " must be an integer value. Check config file.");
	}

	const PIKA_TYPE& magnitude = paramap.at(var_test::MAGNITUDE_KEY);
	if(pika_typeid(magnitude) != DOUBLE_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::MAGNITUDE_KEY) + " must be a floating point value. Check config file.");
	}

	switch(test.second.type)
	{
	    case INT_TYPE:
	      history_check.push_back(HISTORY_CHECK_PTR(new vdt_step_check<int>(test.second.name, begin_time, end_time, boost::get<int>(time), boost::get<double>(magnitude))));
	      break;
	    case DOUBLE_TYPE:
	      history_check.push_back(HISTORY_CHECK_PTR(new vdt_step_check<double>(test.second.name, begin_time, end_time, boost::get<int>(time), boost::get<double>(magnitude))));
	      break;
	    default:
	      throw VDT_EXCEPTION(test.second.name + " must be an int or float to run persistence tests on it.");
	}
      }
    }
  }
  catch(std::out_of_range)
  {
      throw VDT_EXCEPTION(str(format("Could not find %1% or %2%. Check all params in config file.") % var_test::TIME_KEY % var_test::MAGNITUDE_KEY));
  }
}

void vdt_probe_data_qc::perform_history_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc> &history_msgs, vector<vdt_probe_message_qc> &messages)
{
  // Set up the history qc checks and store in the qc vector
  vector<HISTORY_CHECK_PTR> history_check;

  build_persistence_tests(begin_time, end_time, history_check);
  build_step_tests(begin_time, end_time, history_check);
  
  if (!history_check.empty())
  {
    unordered_map<string, vector<vdt_probe_message_qc> > probe_message_history_map;

    vdt_probe_message_history(history_msgs, probe_message_history_map);

    vector<vdt_probe_message_qc> out_probe_message;
    vdt_run_history_check(history_check, messages, probe_message_history_map, begin_time, end_time, out_probe_message);

    // Copy out_probe_message quality check information back to original set of messages based on recorded time
    vdt_probe_message_qc::match_copy(out_probe_message, messages);
  }
}

void vdt_probe_data_qc::build_range_tests(const std::string& test_key, vector<QC_CHECK_PTR>& tests)const
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& paramap = test.second.getTestMap(test_key);
      if(!paramap.empty())
      {
	const PIKA_TYPE& min = paramap.at(var_test::MIN_KEY);
	const PIKA_TYPE& max = paramap.at(var_test::MAX_KEY);

	if(pika_typeid(min) != pika_typeid(max))
	{
	  throw VDT_EXCEPTION("min and max value must have the same type.  Check field " + test.first);
	}

	if(pika_typeid(min) != test.second.type)
	{
	  throw VDT_EXCEPTION("Range check min and max must have the same type as the variable.  Check field " + test.first);
	}

	if(test.second.name == "wiper_status")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_range_check_wiper(test.second.name, boost::get<int>(min), boost::get<int>(max))));
	}
	else if(test.second.name == "brake_status")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_range_check_brake(test.second.name, boost::get<int>(min), boost::get<int>(max))));
	}
	else if(test.second.name == "lights")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_range_check_headlight(test.second.name, boost::get<int>(min), boost::get<int>(max))));
	}
	else if(test_key == var_test::DATA_FILTERING_TEST_KEY)
	{
	  tests.push_back(boost::apply_visitor(dft_range_check_visitor(test.first), min, max));
	}
	else if(test_key == var_test::SENSOR_RANGE_TEST_KEY)
	{
	  std::vector<PIKA_TYPE> exclude_vals;
	  for(const auto& kv : paramap)
	  {
	    const std::string& key = kv.first;
	    if(key.substr(0,EXCLUDE_PREFIX.size()) == EXCLUDE_PREFIX)
	    {
	      const PIKA_TYPE& exclude = kv.second;
	      exclude_vals.push_back(exclude);
	    }
 	  }

	  if(!exclude_vals.empty())
	  {
	    switch(test.second.type)
	    {
	      case INT_TYPE:
	      {
		std::vector<int> exclude_vals_int;
		for(const PIKA_TYPE& exc: exclude_vals)
		{
		  exclude_vals_int.push_back(boost::get<int>(exc));
		}
		tests.push_back(QC_CHECK_PTR(new qc_range_exclude_check<int>(test.first, boost::get<int>(min), boost::get<int>(max), exclude_vals_int, 0)));
		break;
	      }
	      case DOUBLE_TYPE:
	      {
	        std::vector<double> exclude_vals_double;
		for(const PIKA_TYPE& exc: exclude_vals)
		{
		   exclude_vals_double.push_back(boost::get<double>(exc));
		}
		tests.push_back(QC_CHECK_PTR(new qc_range_exclude_check<double>(test.first, boost::get<double>(min), boost::get<double>(max), exclude_vals_double, 0.001)));
		break;
	      }
	      default:
		throw VDT_EXCEPTION(test.second.name + " must be an int or float to run exclude range tests on it.");
	    }
	  }
	  else
	  {
	    tests.push_back(boost::apply_visitor(range_check_visitor(test.first), min, max));
	  }
	}
	else
	{
	  throw VDT_EXCEPTION(str(format("Unrecognized test %1%.") % test_key));
	}
      }
    }
  }
  catch(std::out_of_range)
  {
    throw VDT_EXCEPTION(str(format("Could not find matching %1% and %2%. Check all params in config file.") % var_test::MIN_KEY % var_test::MAX_KEY));
  }
}

void vdt_probe_data_qc::build_climate_tests(vector<QC_CHECK_PTR>& tests)const
{
  try
  {
    for(const auto& test : cfg_reader.vartests)
    {
      const STR_PIKA_TYPE_MAP& paramap = test.second.getTestMap(var_test::MET_FIELD_KEY);
      if(!paramap.empty())
      {
	const PIKA_TYPE& climate_var = paramap.at(var_test::NAME_KEY);
	if(pika_typeid(climate_var) != STRING_TYPE)
	{
	  throw VDT_EXCEPTION(std::string(var_test::NAME_KEY) + " must be a string. Check config file.");
	}
	const std::string climate_var_str = boost::get<std::string>(climate_var);

	if(climate_var_str == "air_temp")
	{
	  if(test.second.type == INT_TYPE)
	  {
	    tests.push_back(QC_CHECK_PTR(new qc_climate_range_check<int>(test.second.name, climate_data, climate_data->get_min_air_temperature_grid(), climate_data->get_max_air_temperature_grid())));
	  }
	  else
	  {
	    tests.push_back(QC_CHECK_PTR(new qc_climate_range_check<double>(test.second.name, climate_data, climate_data->get_min_air_temperature_grid(), climate_data->get_max_air_temperature_grid())));
	  }
	}
	else if(climate_var_str == "surface_temp")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_climate_range_check<double>(test.second.name, climate_data, climate_data->get_min_surface_temperature_grid(), climate_data->get_max_surface_temperature_grid())));
	}
	else if(climate_var_str == "dew_temp")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_climate_range_check<double>(test.second.name, climate_data, climate_data->get_min_dewpoint_temperature_grid(), climate_data->get_max_dewpoint_temperature_grid())));
	}
	else if(climate_var_str == "bar_pressure")
	{
	  tests.push_back(QC_CHECK_PTR(new qc_climate_range_check<int>(test.second.name, climate_data, climate_data->get_min_atmospheric_pressure_grid(), climate_data->get_max_atmospheric_pressure_grid())));
	}
	else
	{
	  throw VDT_EXCEPTION("Unrecognized grid variable " + climate_var_str);
	}
      }
    }
  }
  catch(std::out_of_range)
  {
    throw VDT_EXCEPTION(str(format("Could not find %1%. Check all params in config file.") % var_test::NAME_KEY));
  }
}

void vdt_probe_data_qc::perform_standard_checks(time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &messages)
{
  // Set up the qc tests and store in the qc vector
  vector<QC_CHECK_PTR> qc_vec;
  vector<QC_CHECK_PTR> climate_qc_vec;
  vector<QC_CHECK_PTR> data_filtering_qc_vec;
  vector<QC_CHECK_PTR> spatial_test_qc_vec;

  // Sensor Range Test (SRT)
  build_range_tests(var_test::SENSOR_RANGE_TEST_KEY, qc_vec);
  if(!qc_vec.empty())
  {
     Logg->write_time_info("running %ld sensor range tests\n", qc_vec.size());
  }

  
  // Climatological Range Test (CRT) checks
  build_climate_tests(climate_qc_vec);
  if (!climate_qc_vec.empty() && climate_data->error() == "")
  {
    Logg->write_time_info("running %ld climate range tests\n", climate_qc_vec.size());
  }

  // Data Filtering Test (DFT) checks
  build_range_tests(var_test::DATA_FILTERING_TEST_KEY, data_filtering_qc_vec);

  // Spatial checks
  QC_CHECK_PTR iqr_spatial_qc(new iqr_spatial_check2(cfg_reader, climate_data, datasets));

  if (cfg_reader.spatial_tests)
    {
      spatial_test_qc_vec.push_back(iqr_spatial_qc);

      Logg->write_time_info("running %ld spatial tests\n", spatial_test_qc_vec.size());
    }

  Logg->write_time_info("qc checks being performed on %ld messages\n", messages.size());
  for (unsigned int i=0; i<messages.size(); i++)
    {
      time_t obs_time = messages[i].get_obs_time();

      // Anticipated Range Test (ART) checks
      for (unsigned int j=0; j<qc_vec.size(); j++)
	{
	  (*qc_vec[j])(messages[i]);
	}
      // Climatological Range Test (CRT) checks
      for (unsigned int k=0; k<climate_qc_vec.size(); k++)
	{
	  (*climate_qc_vec[k])(messages[i]);
	}
      // Data Filtering Test (DFT) checks
      for (unsigned int m=0; m<data_filtering_qc_vec.size(); m++)
	{
	  (*data_filtering_qc_vec[m])(messages[i]);
	}
      // Spatial checks
      for (unsigned int n=0; n<spatial_test_qc_vec.size(); n++)
	{
	  // ensure message observation time falls in the time window used in the qc test
	  if (begin_time <= obs_time && obs_time < end_time)
	    {
	      (*spatial_test_qc_vec[n])(messages[i]);
	    }
	}

    }  // end for i
} // end check_all_ranges

void vdt_probe_data_qc::perform_concluding_checks(time_t begin_time, time_t end_time, const vector<vdt_probe_message_qc_statistics> &seg_stats, const vector<int> &seg_indices, unordered_map<int, int> &seg_index_offset, vector<vdt_probe_message_qc> &messages)
{
  // set up model analysis check
  vdt_model_check model_check(cfg_reader, climate_data, datasets);

  // set up standard deviation check
  vdt_stdev_check stdev_check(cfg_reader, climate_data, datasets, seg_stats, seg_indices, seg_index_offset);
  
  // set up neighboring vehicle check
  vdt_neighbor_check_iqr iqr_neighbor_qc(cfg_reader, climate_data, datasets, seg_stats, seg_indices, seg_index_offset);

  for (unsigned int i=0; i<messages.size(); i++)
    {
      time_t obs_time = messages[i].get_obs_time();

      // ensure message observation time falls in the time window used in the qc tests
      if (begin_time <= obs_time && obs_time < end_time)
	{
	  if (cfg_reader.model_analysis_test)
	    {
	      model_check(seg_indices[i], messages[i]);
	    }
	  if (cfg_reader.standard_deviation_test)
	    {
	      stdev_check(seg_indices[i], messages[i]);
	    }

	  if (cfg_reader.neighboring_vehicle_test)
	    {
	      iqr_neighbor_qc(seg_indices[i], messages[i]);
	    }
	}
    }
}
