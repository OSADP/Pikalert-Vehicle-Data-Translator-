//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
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
#include <stdlib.h>
#include <boost/format.hpp>

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


// Types, structures and classes

// Global variables

// Functions

config_reader::config_reader()
{
}

config_reader::config_reader(const string &input_file_path_arg) : input_file_path(input_file_path_arg)
{
  Config cfg;

  rtma_proj = 0;
  wiper_status = 0;

  error = string("");

  FILE *fp = fopen(input_file_path.c_str(), "r");

  if (fp == NULL)
    {
      error = string("could not load config file ") + input_file_path;
      return;
    }

  try
    {
      cfg.setAutoConvert(true);
      cfg.read(fp);
      fclose(fp);

      // Get probe message field names
      Setting &probe_message_field_names_setting = cfg.lookup("Simulation.field_names.probe_message_field_names");
      for (int i=0; i<probe_message_field_names_setting.getLength(); i++)
        {
          string probe_message_field_name = probe_message_field_names_setting[i];
          probe_message_field_names.push_back(probe_message_field_name);
        }

      // CDL file paths

      const char *probe_message_qc_cdl_file_string = cfg.lookup("Simulation.input_config.probe_message_qc_cdl");
      probe_message_qc_cdl_file = probe_message_qc_cdl_file_string;

      const char *probe_message_qc_statistics_cdl_file_string = cfg.lookup("Simulation.input_config.statistics_cdl");
      probe_message_qc_statistics_cdl_file = probe_message_qc_statistics_cdl_file_string;

      const char *road_segment_assessment_cdl_file_string = cfg.lookup("Simulation.input_config.assessment_cdl");
      road_segment_assessment_cdl_file = road_segment_assessment_cdl_file_string;

      // Lat/lon box
      south_lat = cfg.lookup("Simulation.lat_long_box.s_lat");
      north_lat = cfg.lookup("Simulation.lat_long_box.w_long");
      west_lon = cfg.lookup("Simulation.lat_long_box.n_lat");
      east_lon = cfg.lookup("Simulation.lat_long_box.e_long");

      // Wiper status constants

      int off = cfg.lookup("Simulation.radar_wiper_table.off");
      int intermittent = cfg.lookup("Simulation.radar_wiper_table.intermittent");
      int low = cfg.lookup("Simulation.radar_wiper_table.low");
      int high = cfg.lookup("Simulation.radar_wiper_table.high");
      wiper_status = new Wiper_status(off, intermittent, low, high);

      // Hourly fractions for adjusting probe message count
      Setting &hourly_fractions_setting = cfg.lookup("Simulation.hourly_fractions");
      for (int i=0; i<hourly_fractions_setting.getLength(); i++)
        {
          float fraction = hourly_fractions_setting[i];
          hourly_fractions.push_back(fraction);
        }

      //max_dist_to_segment = cfg.lookup("Simulation.qc_config.max_dist_to_segment");
      //max_dist_to_cell = cfg.lookup("Simulation.qc_config.max_dist_to_cell");

      const char *climatology_file_string = cfg.lookup("Simulation.input_config.climatology_file");
      climatology_file = climatology_file_string;

#ifdef NOTNOW      
      longitude_min = cfg.lookup("Simulation.qc_config.longitude_min");
      longitude_max = cfg.lookup("Simulation.qc_config.longitude_max");

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
#endif
      // Rtma projection string for proj4
      const char *rtma_proj4_projection_char_string = cfg.lookup("Simulation.rtma_proj4_projection_string");
      rtma_proj4_projection_string = rtma_proj4_projection_char_string;

      // Rtma projection dx and dy for proj4
      rtma_dx = cfg.lookup("Simulation.rtma_dx");      
      rtma_dy = cfg.lookup("Simulation.rtma_dy");

      // Rtma projection la1 and lo1 for proj4
      rtma_la1 = cfg.lookup("Simulation.rtma_la1");
      rtma_lo1 = cfg.lookup("Simulation.rtma_lo1");      

      rtma_proj = new p4w::Proj4Wrap(rtma_proj4_projection_string, p4w::Proj4Wrap::LON_LAT_TYPE, rtma_lo1, rtma_la1, rtma_dx, rtma_dy);

      // Get nearest surface station search radius and time range
      nst_search_radius = cfg.lookup("Simulation.qc_config.nst_search_radius");
      nst_time_range = cfg.lookup("Simulation.qc_config.nst_time_range");

      // Get highway information
      interstate_highway_fraction = cfg.lookup("Simulation.highway_info.interstate_highway_fraction");
      us_highway_fraction = cfg.lookup("Simulation.highway_info.us_highway_fraction");
      state_highway_fraction = cfg.lookup("Simulation.highway_info.state_highway_fraction");
      default_less_dense_num_cars = cfg.lookup("Simulation.highway_info.default_less_dense_num_cars");
      default_more_dense_num_cars = cfg.lookup("Simulation.highway_info.default_more_dense_num_cars");

      const char *interstate_highway_cstring = cfg.lookup("Simulation.highway_info.interstate_highway_string");
      interstate_highway_type_string = interstate_highway_cstring;

      const char *us_highway_cstring = cfg.lookup("Simulation.highway_info.us_highway_string");
      us_highway_type_string = us_highway_cstring;

      const char *state_highway_cstring = cfg.lookup("Simulation.highway_info.state_highway_string");
      state_highway_type_string = state_highway_cstring;

      // Get city information
      const char *less_dense_file_string = cfg.lookup("Simulation.highway_info.less_dense_file");
      less_dense_file = less_dense_file_string;
      const char *more_dense_file_string = cfg.lookup("Simulation.highway_info.more_dense_file");
      more_dense_file = more_dense_file_string;

      // Get city, latitude, longitude, radius_km, less_dense_num_cars, more_dense_num_cars
      Setting &city_table_setting = cfg.lookup("Simulation.city_info.city_table");
      for (int i=0; i<city_table_setting.getLength(); i++)
        {
	  Setting &city_list = city_table_setting[i];
	  if (city_list.getLength() != 6)
	    {
	      error = str(format("city_list length = %1% != 6") % city_list.getLength());
	      return;
	    }
	  const char *city_string = city_list[0];
	  City_entry city_entry;

	  city_entry.city = city_string;
	  city_entry.latitude = city_list[1];
	  city_entry.longitude = city_list[2];
	  city_entry.radius_km = city_list[3];
	  city_entry.less_dense_num_cars = city_list[4];
	  city_entry.more_dense_num_cars = city_list[5];
	  city_table.push_back(city_entry);
        }
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
}

config_reader::~config_reader()
{
  delete rtma_proj;
  delete wiper_status;
}
