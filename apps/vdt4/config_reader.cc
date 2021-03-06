//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright �2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: config_reader.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/05/27 20:14:58 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/config_reader.cc
 *
 *  Configuration file reader class implementation
 *
 */

// Include files
#include <log/log.hh>
#include <stdlib.h>
#include <cstring>
#include <boost/format.hpp>
#include <sstream>

using boost::format;

#include "config_reader.hh"
#include "libconfig.h++"

using namespace libconfig;

// Constant and macros
static const int MAX_LINE = 1024;
static const float VALID_LAT_MIN = -90.0;
static const float VALID_LAT_MAX = 90.0;
static const float VALID_LON_MIN = -180.0;
static const float VALID_LON_MAX = 180.0;

const char var_test::SENSOR_RANGE_TEST_KEY[] = "sensor_range_test";
const char var_test::DATA_FILTERING_TEST_KEY[] = "data_filter_test";
const char var_test::STD_DEV_TEST_KEY[] = "std_dev_test";
const char var_test::MODEL_ANALYSIS_TEST_KEY[] = "model_analysis_test";
const char var_test::PERSISTENCE_TEST_KEY[] = "persistence_test";
const char var_test::TIME_STEP_TEST_KEY[] = "time_step_test";
const char var_test::MET_FIELD_KEY[] = "met_field";
const char var_test::MIN_KEY[] = "min";
const char var_test::MAX_KEY[] = "max";
const char var_test::NAME_KEY[] = "name";
const char var_test::TIME_KEY[] = "time";
const char var_test::MAGNITUDE_KEY[] = "magnitude";
const char var_test::MULTIPLIER_KEY[] = "mult";
const char var_test::DELTA_KEY[] = "delta";

// Types, structures and classes

// Global variables
extern Log *Logg;

// Functions

void config_reader::set_srt()
{
  // Sensor Range Test
   int srt_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::SENSOR_RANGE_TEST_KEY).empty();});
   sensor_range_test = srt_indicator;

   set_test_min_max("speed", var_test::SENSOR_RANGE_TEST_KEY, speed_range_min, speed_range_max);
   set_test_min_max("air_temp", var_test::SENSOR_RANGE_TEST_KEY, air_temp_min, air_temp_max);
   set_test_min_max("bar_pressure", var_test::SENSOR_RANGE_TEST_KEY, bar_press_min, bar_press_max);
   set_test_min_max("brake_status", var_test::SENSOR_RANGE_TEST_KEY, brake_status_min, brake_status_max);
   set_test_min_max("brake_boost", var_test::SENSOR_RANGE_TEST_KEY, brake_boost_min, brake_boost_max);
   set_test_min_max("wiper_status", var_test::SENSOR_RANGE_TEST_KEY, wiper_status_min, wiper_status_max);
   set_test_min_max("trac", var_test::SENSOR_RANGE_TEST_KEY, traction_control_min, traction_control_max);
   set_test_min_max("stab", var_test::SENSOR_RANGE_TEST_KEY, stability_control_min, stability_control_max);
   set_test_min_max("abs", var_test::SENSOR_RANGE_TEST_KEY, abs_min, abs_max);
   set_test_min_max("lights", var_test::SENSOR_RANGE_TEST_KEY, headlights_min, headlights_max);
   set_test_min_max("yaw_rate", var_test::SENSOR_RANGE_TEST_KEY, yaw_rate_min, yaw_rate_max);
   set_test_min_max("hoz_accel_lat", var_test::SENSOR_RANGE_TEST_KEY, hoz_accel_lat_min, hoz_accel_lat_max);
   set_test_min_max("hoz_accel_long", var_test::SENSOR_RANGE_TEST_KEY, hoz_accel_lon_min, hoz_accel_lon_max);
   set_test_min_max("steering_angle", var_test::SENSOR_RANGE_TEST_KEY, steering_wheel_angle_min, steering_wheel_angle_max);
   set_test_min_max("steering_rate", var_test::SENSOR_RANGE_TEST_KEY, steering_rate_min, steering_rate_max);
   set_test_min_max("heading", var_test::SENSOR_RANGE_TEST_KEY, heading_min, heading_max);
}

config_reader::config_reader()
{
}

config_reader::config_reader(const string &input_file_path_arg) : input_file_path(input_file_path_arg)
{
  Config cfg;

  error = string("");

  FILE *fp = fopen(input_file_path.c_str(), "r");

  if (fp == NULL)
    {
      error = string("could not load config file ") + input_file_path;
      return;
    }

  try
    {
      rtma_proj = 0;

      cfg.setAutoConvert(true);
      cfg.read(fp);
      fclose(fp);

      // CDL file paths

      const char *probe_message_qc_cdl_file_string = cfg.lookup("VDT.input_config.probe_message_qc_cdl");
      probe_message_qc_cdl_file = probe_message_qc_cdl_file_string;

      const char *probe_message_qc_statistics_cdl_file_string = cfg.lookup("VDT.input_config.statistics_cdl");
      probe_message_qc_statistics_cdl_file = probe_message_qc_statistics_cdl_file_string;

      const char *road_segment_assessment_cdl_file_string = cfg.lookup("VDT.input_config.assessment_cdl");
      road_segment_assessment_cdl_file = road_segment_assessment_cdl_file_string;

      // Lat/lon box
      grid_south_lat = cfg.lookup("VDT.lat_long_box.s_lat");
      grid_west_lon = cfg.lookup("VDT.lat_long_box.w_long");
      grid_north_lat = cfg.lookup("VDT.lat_long_box.n_lat");
      grid_east_lon = cfg.lookup("VDT.lat_long_box.e_long");
      grid_lat_size = cfg.lookup("VDT.lat_long_box.grid_lat_size");
      grid_lon_size = cfg.lookup("VDT.lat_long_box.grid_lon_size");
      if (fabs(grid_north_lat - grid_south_lat) < 0.1 || fabs(grid_east_lon - grid_west_lon) < 0.1)
	{
	  error = str(format("grid too small: %lg = grid_north_lat - %lg = grid_south_lat < 0.1 || %lg = grid_east_lon - %lg = grid_west_lon < 0.1") % grid_north_lat % grid_south_lat % grid_east_lon % grid_west_lon);
	  return;
	}
      
      // Combined algorithm test

      combined_algorithm_threshold = cfg.lookup("VDT.qc_config.combined_algorithm_threshold");

      // Combined algorithm weights for STEP, SPATIAL, SDT, PERSIST, NVT, CRT
      Setting &combined_algorithm_weights_setting = cfg.lookup("VDT.qc_config.combined_algorithm_weights");
      for (int i=0; i<combined_algorithm_weights_setting.getLength(); i++)
        {
          float weight = combined_algorithm_weights_setting[i];
          combined_algorithm_weights.push_back(weight);
        }

      max_nearest_neighbors = cfg.lookup("VDT.qc_config.max_nearest_neighbor");
      max_dist_to_segment = cfg.lookup("VDT.qc_config.max_dist_to_segment");
      max_dist_to_cell = cfg.lookup("VDT.qc_config.max_dist_to_cell");

      //DYNAMIC CONFIGURATION CAPABILITIES
      const Setting& var_settings = cfg.lookup("VDT.probe_message_field_params");
      //Loop over variables
      for(int i = 0; i < var_settings.getLength(); ++i)
      {
	const Setting& var_setting = var_settings[i];
	
	//First param of variable should be type = "double"|"float"|"int"|"short"|"string"
	if(var_setting.getLength() == 0 || std::string(var_setting[0].getName()) != std::string("type"))
	{
	  error = str(format(" - Missing the \"type\" parameter for variable %1%. This should be the first attribute for %1%.") % var_setting.getName());
          return;
	}
	var_test vartest(var_setting.getName(), var_setting[0]);
	probe_message_field_names.push_back(vartest.name);
	//Second param can be skip = true
	//TODO: review if any default values need to be set when we skip over a variable
	//if(var_setting.getLength() > 1 && strcmp(var_setting[1].getName(),"skip") == 0 && var_setting[1])
	//  continue;
	
	//Loop over tests
	for(int j = 1; j < var_setting.getLength(); ++j)
	{
	  const Setting& var_setting_test = var_setting[j];
	  STR_PIKA_TYPE_MAP& paramap =  vartest.getTestMap(var_setting_test.getName());

	  //Loop over test params
	  for(int k = 0; k < var_setting_test.getLength(); ++k)
	  {
	    const std::string paramname = var_setting_test[k].getName();
	    
	    //Extract as type to boost variant
	    switch(var_setting_test[k].getType())
	    {
	        case Setting::Type::TypeInt:
	        {
		  int val = var_setting_test[k];
		  paramap[paramname] = val;
		  break;
		}
	        case Setting::Type::TypeFloat:
		{
	          double val = var_setting_test[k];
		  paramap[paramname] = val;
		  break;
	        }
         	case Setting::Type::TypeString:
		{
		  std::string val = var_setting_test[k];
		  paramap[paramname] = val;
		  break;
		}
	        default:
		{
		  error = str(format(" - Unrecognized type %1% for %2%/%3%/%4%") % var_setting_test[k].getType() % vartest.name % var_setting_test.getName() % paramname);
		  return;
		}
	    }

	    std::stringstream ss;
	    ss << vartest.name << "/" << var_setting_test.getName() << "/" << paramname << " " << paramap[paramname] << std::endl;
	    Logg->write_time_info("%s", ss.str().c_str());
	  } //End for k
	} //End for j
	vartests.insert(std::make_pair(vartest.name, vartest));
      } //End for i
      //Dynamic configuration capabilities

      set_srt();

      // Standard Deviation Test
      int sdt_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::STD_DEV_TEST_KEY).empty();});
      
      standard_deviation_test = sdt_indicator;
      set_test_param("air_temp", var_test::STD_DEV_TEST_KEY, "mult", air_temp_dev_mult);
      set_test_param("dew_temp", var_test::STD_DEV_TEST_KEY, "mult", dew_temp_dev_mult);
      set_test_param("surface_temp", var_test::STD_DEV_TEST_KEY, "mult", surface_temp_dev_mult);
      set_test_param("bar_pressure", var_test::STD_DEV_TEST_KEY, "mult", bar_press_dev_mult);

      // Neighboring Surface Station Test (NST)

      int sst_indicator = cfg.lookup("VDT.qc_config.surface_station_test");
      surface_station_test = sst_indicator;

      nst_search_radius = cfg.lookup("VDT.qc_config.nst_search_radius");

      int temp_ntr = cfg.lookup("VDT.qc_config.nst_time_range");
      nst_time_range = temp_ntr;

      // Model Analysis Test (MAT)

      int mat_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::MODEL_ANALYSIS_TEST_KEY).empty();});
  
      model_analysis_test = mat_indicator;
      set_test_param("air_temp", var_test::MODEL_ANALYSIS_TEST_KEY, "delta", mat_air_temp_delta);
      set_test_param("dew_temp", var_test::MODEL_ANALYSIS_TEST_KEY, "delta", mat_dewpoint_delta);
      set_test_param("bar_pressure", var_test::MODEL_ANALYSIS_TEST_KEY, "delta", mat_bar_press_delta);

      // Climatological Range Test (CRT)

      const char *climatology_file_string = cfg.lookup("VDT.input_config.climatology_file");
      climatology_file = climatology_file_string;

      // Neighboring Vehicle Test (NVT)

      int nvt_indicator = cfg.lookup("VDT.qc_config.neighboring_vehicle_test");
      neighboring_vehicle_test = nvt_indicator;

      // Data Filtering Test (DFT)

      int dft_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::DATA_FILTERING_TEST_KEY).empty();});
      data_filtering_test = dft_indicator;

      set_test_min_max("latitude", var_test::DATA_FILTERING_TEST_KEY, latitude_min, latitude_max);
      set_test_min_max("longitude", var_test::DATA_FILTERING_TEST_KEY, longitude_min, longitude_max);

      if (data_filtering_test)
        {
          if (latitude_min < VALID_LAT_MIN)
            {
              error = str(format(" - The latitude_min of %1% specified in %2% is less than the valid latitude minimum of %3%") % latitude_min % input_file_path.c_str() % VALID_LAT_MIN);
              return;
            }

          if (latitude_max > VALID_LAT_MAX)
            {
              error = str(format("- The latitude_max of %1% specified in %2% is greater than the valid latitude maximum of %3%") % latitude_max % input_file_path.c_str() % VALID_LAT_MAX);
              return;
            }
          if (longitude_min < VALID_LON_MIN)
            {
              error = str(format(" - The longitude_min of %1% specified in %2% is less than the valid longitude minimum of %3%") % longitude_min % input_file_path.c_str() % VALID_LON_MIN);
              return;
            }

          if (longitude_max > VALID_LON_MAX)
            {
              error = str(format("- The longitude_max of %1% specified in %2% is greater than the valid longitude maximum of %3%") % longitude_max % input_file_path.c_str() % VALID_LON_MAX);
              return;
            }
        }

      // Spatial Tests - Air Pressure (STP) and Air and Surface Temperature (STTa and STTp)
      int spatial_test_indicator = cfg.lookup("VDT.qc_config.spatial_tests");
      spatial_tests = spatial_test_indicator;

      spatial_iqr_air_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_air_temp_tol");
      spatial_iqr_dew_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_dew_temp_tol");
      spatial_iqr_surface_temp_tol = cfg.lookup("VDT.qc_config.spatial_iqr_surface_temp_tol");
      spatial_iqr_pressure_tol = cfg.lookup("VDT.qc_config.spatial_iqr_pressure_tol");
      spatial_iqr_multiplier = cfg.lookup("VDT.qc_config.spatial_iqr_multiplier");

      spatial_range_search_km = cfg.lookup("VDT.qc_config.spatial_range_search_km");
      spatial_time_search_secs = cfg.lookup("VDT.qc_config.spatial_time_search_secs");

      spatial_barnes_min_size = cfg.lookup("VDT.qc_config.spatial_barnes_min_size");
      spatial_barnes_max_sd = cfg.lookup("VDT.qc_config.spatial_barnes_max_sd");

      // History Tests (HT)
      int ht_indicator = cfg.lookup("VDT.qc_config.history_tests");
      history_tests = ht_indicator;

      const char *history_path_string = cfg.lookup("VDT.qc_config.history_path");
      history_path = history_path_string;

      const char *history_basename_string = cfg.lookup("VDT.qc_config.history_basename");
      history_basename = history_basename_string;

      history_cutoff_time = cfg.lookup("VDT.qc_config.history_cutoff_time");

      // Persistence Test (PET)
      int pet_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::PERSISTENCE_TEST_KEY).empty();});
      persistence_test = pet_indicator;
      
      set_test_param("air_temp", var_test::PERSISTENCE_TEST_KEY, "time", air_temp_persist_time);
      set_test_param("air_temp2", var_test::PERSISTENCE_TEST_KEY, "time", air_temp2_persist_time);
      set_test_param("dew_temp", var_test::PERSISTENCE_TEST_KEY, "time", dew_temp_persist_time);
      set_test_param("surface_temp", var_test::PERSISTENCE_TEST_KEY, "time", surface_temp_persist_time);
      set_test_param("bar_pressure", var_test::PERSISTENCE_TEST_KEY, "time", bar_press_persist_time);

      // Air temp Time Step Test (TST)
       int tst_indicator = std::any_of(vartests.begin(), vartests.end(), 
				   [](const std::pair<std::string,var_test>& kv)
				   {return !kv.second.getTestMap(var_test::TIME_STEP_TEST_KEY).empty();});
      time_step_test = tst_indicator;

      set_test_param("air_temp", var_test::TIME_STEP_TEST_KEY, "magnitude", air_temp_step_magnitude);
      set_test_param("air_temp", var_test::TIME_STEP_TEST_KEY, "time", air_temp_step_time);

      set_test_param("air_temp2", var_test::TIME_STEP_TEST_KEY, "magnitude", air_temp2_step_magnitude);
      set_test_param("air_temp2", var_test::TIME_STEP_TEST_KEY, "time", air_temp2_step_time);

      set_test_param("dew_temp", var_test::TIME_STEP_TEST_KEY, "magnitude", dew_temp_step_magnitude);
      set_test_param("dew_temp", var_test::TIME_STEP_TEST_KEY, "time", dew_temp_step_time);

      set_test_param("surface_temp", var_test::TIME_STEP_TEST_KEY, "magnitude", surface_temp_step_magnitude);
      set_test_param("surface_temp", var_test::TIME_STEP_TEST_KEY, "time", surface_temp_step_time);

      // Barometric pressure Time Step Test (TST)
      set_test_param("bar_pressure", var_test::TIME_STEP_TEST_KEY, "magnitude", bar_press_step_magnitude);
      set_test_param("bar_pressure", var_test::TIME_STEP_TEST_KEY, "time", bar_press_step_time);

      max_begin_time_delta = cfg.lookup("VDT.qc_config.max_begin_time_delta");
      max_end_time_delta = cfg.lookup("VDT.qc_config.max_end_time_delta");

      // Rtma projection string for proj4
      const char *rtma_proj4_projection_char_string = cfg.lookup("VDT.rtma_proj4_projection_string");
      rtma_proj4_projection_string = rtma_proj4_projection_char_string;

      // Rtma projection dx and dy for proj4
      rtma_dx = cfg.lookup("VDT.rtma_dx");      
      rtma_dy = cfg.lookup("VDT.rtma_dy");

      // Rtma projection la1 and lo1 for proj4
      rtma_la1 = cfg.lookup("VDT.rtma_la1");
      rtma_lo1 = cfg.lookup("VDT.rtma_lo1");      

      rtma_proj = new p4w::Proj4Wrap(rtma_proj4_projection_string, p4w::Proj4Wrap::LON_LAT_TYPE, rtma_lo1, rtma_la1, rtma_dx, rtma_dy);

      // Alaska rtma projection information
      ak_rtma_proj_info.latin = cfg.lookup("VDT.ak_rtma_latin");
      ak_rtma_proj_info.lov = cfg.lookup("VDT.ak_rtma_lov");
      ak_rtma_proj_info.lo1 = cfg.lookup("VDT.ak_rtma_lo1");
      ak_rtma_proj_info.la1 = cfg.lookup("VDT.ak_rtma_la1");
      ak_rtma_proj_info.lo2 = cfg.lookup("VDT.ak_rtma_lo2");
      ak_rtma_proj_info.la2 = cfg.lookup("VDT.ak_rtma_la2");
      ak_rtma_proj_info.nx = cfg.lookup("VDT.ak_rtma_nx");
      ak_rtma_proj_info.ny = cfg.lookup("VDT.ak_rtma_ny");
      ak_rtma_proj_info.dx = cfg.lookup("VDT.ak_rtma_dx");
      ak_rtma_proj_info.dy = cfg.lookup("VDT.ak_rtma_dy");
      ak_rtma_proj_info.lad = cfg.lookup("VDT.ak_rtma_lad");
    }
  catch (ParseException ce)
    {
      const int size = MAX_LINE;
      char line[size];
      snprintf(line, size, " on line %d", ce.getLine());
      error = string("Config reader parse exception occurred: ") + string(ce.getError()) + line;
    }
  catch (SettingException ce)
    {
      error = string("Config reader setting exception occurred on: ") + ce.getPath();
    }
  catch (FileIOException ce)
    {
      error = string("Config reader file exception occurred on: ") + input_file_path;
    }
  catch (ConfigException ce)
    {
      error = string("Config reader generic exception occurred");
    }
  catch (std::out_of_range)
    {
      error = string("Configuration file missing required key value.");
    }
}

config_reader::~config_reader()
{
  delete rtma_proj;
}
