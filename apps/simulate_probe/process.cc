/**
 *
 * @file process.cc
 *
 */

// Include files
#include <math.h>
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "process.hh"
#include "vdt4/vdt_climatology_file_reader.hh"
#include "vdt4/vdt_grid.hh"
#include "vdt4/vdt_history_check.hh"
#include "vdt4/vdt_nearest_nbr.hh"
#include "vdt4/vdt_point.hh"
#include "vdt4/vdt_probe_message_file_reader.hh"
#include "vdt4/vdt_probe_message_qc_file_reader.hh"
#include "vdt4/vdt_probe_message_qc_statistics.hh"
#include "vdt4/vdt_probe_message_qc_file_writer.hh"
#include "vdt4/vdt_probe_message_qc_statistics_file_writer.hh"
#include "vdt4/vdt_road_segment.hh"
#include "vdt4/vdt_road_segment_data.hh"
#include "vdt4/vdt_road_segment_file_reader.hh"
#include "vdt_road_segment_datasets.hh"
#include "simulate_probe.hh"

using namespace std;
using namespace libconfig;

extern Log *Logg;
extern int Debug_level;

void ymdhms(time_t time_val, string &ymdhms)
{
  struct tm *tms = gmtime(&time_val);
  const int MAX_TIME = 16;
  char time_string[MAX_TIME];
  strftime(time_string, MAX_TIME, "%Y%m%d%H%M%S", tms);
  ymdhms = string(time_string);
}

int extract_file_list(const string &file_name, vector<string> &file_list)
{
  ifstream in_file(file_name.c_str());
  if (!in_file.is_open())
    {
      Logg->write_time_error("cannot open container file: %s\n", file_name.c_str());
      return -1;
    }

  string line;
  while (getline(in_file, line))
    {
      file_list.push_back(line);
    }

  return 0;
}

int get_file_lists(const arguments &args, vector<vector<string> > &file_lists)
{
  if (args.file_list_names.size() == 0)
    {
      return 0;
    }

  size_t file_list_size = 0;

  file_lists.resize(args.file_list_names.size());
  
  // File lists are either equal in size or 0 in size
  // File lists can contain the name None
  vector<size_t> file_list_sizes;

  Logg->write_time_info("extracting file lists\n");

  for (size_t i=0; i<args.file_list_names.size(); i++)
    {
      if (extract_file_list(args.file_list_names[i], file_lists[i]) < 0)
	{
	  return -1;
	}
      if (i == 0)
	{
	  file_list_size = file_lists[i].size();
	}
      else
	{
	  if (file_list_size != file_lists[i].size())
	    {
	      return -1;
	    }
	}
    }

  return 0;
}

// Determine the appropriate number of probe messages for the target lat, long for a road segment
int get_probe_message_count(int density, int utc_hour, const vdt_road_segment &road_segment, const config_reader &cfg_reader) 
{
  // Get road segment information include mid point and type
  vdt_point target = road_segment.get_mid_point();
  double target_lat = target.get_latitude();
  double target_lon = target.get_longitude();

  string road_type = road_segment.get_road_type();  

  // Derive road fraction based on road type
  float road_fraction = 0.0;
  if (road_type == cfg_reader.interstate_highway_type_string)
    {
      road_fraction = cfg_reader.interstate_highway_fraction;
    }
  else if (road_type == cfg_reader.us_highway_type_string)
    {
      road_fraction = cfg_reader.us_highway_fraction;
    }
  else if (density != arguments::LESS_DENSE && road_type == cfg_reader.state_highway_type_string)
    {
      road_fraction = cfg_reader.state_highway_fraction;
    }
  else
    {
      road_fraction = 0;
    }

  // Determine hour fraction
  float hour_fraction = cfg_reader.hourly_fractions[utc_hour];

  // Determine overall fraction
  float fraction = road_fraction * hour_fraction;

  // Find the closest city
  int count = -1;
  double min_dist = 1000000;
  int min_index = -1;

  // Debug print count
  static int debug_print_count = 0;

  for (unsigned int i=0; i<cfg_reader.city_table.size(); i++)
    {
      double dist = vdt_point::great_circle_distance(target_lat, target_lon, cfg_reader.city_table[i].latitude, cfg_reader.city_table[i].longitude);
      if (dist < min_dist)
	{
	  min_dist = dist;
	  min_index = i;
	}
    }

  if (min_index != -1)
    {
      if (min_dist <= cfg_reader.city_table[min_index].radius_km)
	{
	  if (density == arguments::LESS_DENSE)
	    {
	      count = rintf(fraction * cfg_reader.city_table[min_index].less_dense_num_cars);
	      printf("road_type, road_fraction, hour_fraction, total_fraction, table num cars: %s %f %f %f %d\n", road_type.c_str(), road_fraction, hour_fraction, fraction, cfg_reader.city_table[min_index].less_dense_num_cars);
	      printf("city %s %d, dist %lf, count %d, target, city lat lons %lf %lf %lf %lf\n", cfg_reader.city_table[min_index].city.c_str(), min_index, min_dist, count, target_lat, target_lon, cfg_reader.city_table[min_index].latitude, cfg_reader.city_table[min_index].longitude);
	    }
	  else
	    {
	      count = rintf(fraction * cfg_reader.city_table[min_index].more_dense_num_cars);
	    }
	}
      else
	{
	  if (density == arguments::LESS_DENSE)
	    {
	      count = rintf(fraction * cfg_reader.default_less_dense_num_cars);
	      
	      if (debug_print_count < 100)
		{
		  printf("NO CITY: road_type, road_fraction, hour_fraction, total_fraction, less dense number, count: %s %f %f %f %d %d\n", road_type.c_str(), road_fraction, hour_fraction, fraction, cfg_reader.default_less_dense_num_cars, count);
		  debug_print_count += 1;
		}

	    }
	  else
	    {
	      count = rintf(fraction * cfg_reader.default_more_dense_num_cars);
	    }
	}
    }
  
  return count;
}

int process::do_simulation(unsigned int ct, time_t current_time, const config_reader &cfg_reader, const vector<vector<string> > &file_lists, const vector<string> &field_names, const vector<string> &field_units, int wiper_status_field_number, const vector<float> &deviations, const vector<vdt_road_segment> &input_segments)
{

  field_info field_info_obj(field_names, field_units);

  int utc_hour = (current_time / 3600) % 24;

  // Read input files in vdt_road_segment_datasets()
  string radar_file = "";
  string metar_file = "";
  string rwis_file = "";
  string rtma_file = "";

  auto file_itr = args.file_list_position.find("RADAR");
  if (file_itr != args.file_list_position.end())
    {
      radar_file = file_lists[file_itr->second][ct];
    }

  file_itr = args.file_list_position.find("METAR");
  if (file_itr != args.file_list_position.end())
    {
      metar_file = file_lists[file_itr->second][ct];
    }

  file_itr = args.file_list_position.find("RWIS");
  if (file_itr != args.file_list_position.end())
    {
      rwis_file = file_lists[file_itr->second][ct];
    }

  file_itr = args.file_list_position.find("RTMA");
  if (file_itr != args.file_list_position.end())
    {
      rtma_file = file_lists[file_itr->second][ct];
    }


  Logg->write_time_info("constructing vdt_road_segment_datasets from input files for time %ld: %s, %s, %s, %s\n", current_time, radar_file.c_str(), metar_file.c_str(), rwis_file.c_str(), rtma_file.c_str());

  vdt_road_segment_datasets ds(cfg_reader, radar_file, metar_file, rwis_file, rtma_file, Logg);


  // Fill in known data for each road segment using road segments datasets
  Logg->write_time_info("updating road segments\n");
  vector<vdt_road_segment_data> road_segment_data_vec;
  ds.update_road_segments(current_time, cfg_reader.wiper_status, field_info_obj, input_segments, road_segment_data_vec);

  // Simulate probe messages based on the road segment data in out_segments

  Logg->write_time_info("simulating probe messages based on %ld road segments\n", road_segment_data_vec.size());
  vector<int> covered(road_segment_data_vec.size(), 0);
  vector<vdt_road_segment_data *> output_road_segment_data_vec(road_segment_data_vec.size(), 0);
  vector<vdt_generic_message> combined_probe_messages;


  for (size_t i=0; i<road_segment_data_vec.size(); i++)
    {
      //printf("radar x, y index %f %f\n", road_segment_data_vec[i].field_values[0], road_segment_data_vec[i].field_values[1]);

      // determine the number of probe messages to be used for this road segment
      int number_probe_messages = get_probe_message_count(args.car_density, utc_hour, road_segment_data_vec[i].road_segment, cfg_reader);
      if (number_probe_messages == -1)
	{
	  Logg->write_time_error("get_probe_message_count() returned -1\n");
	  return -1;
	}
      else if (number_probe_messages == 0)
	{
	  continue;
	}
      covered[i] = 1;

      // simulate probe messages for this segment and push onto probe_messages
      vector<vdt_generic_message> probe_messages;

      vdt_road_segment_data *output_road_segment_data = new vdt_road_segment_data(field_info_obj, road_segment_data_vec[i].obs_time, number_probe_messages, road_segment_data_vec[i].road_segment);

      simulate_probe_road_segment_data(number_probe_messages, field_info_obj.field_names.size(), deviations, road_segment_data_vec[i], *output_road_segment_data, probe_messages);
      std::copy (probe_messages.begin(), probe_messages.end(), combined_probe_messages.end());
      output_road_segment_data_vec[i] = output_road_segment_data;
    }

  // Output the road segment data vector in csv format if called for
  if (args.csv_out_file_base != "")
    {
      // Create output file name using date/time
      string date_string;
      ymdhms(current_time, date_string);
      string csv_out_file = args.csv_out_file_base + "." + date_string.substr(0, 8) + "." + date_string.substr(8, 4) + ".csv";

      Logg->write_time_info("writing data for %ld road segments to file: %s\n", output_road_segment_data_vec.size(), csv_out_file.c_str());

      FILE *fp = fopen(csv_out_file.c_str(), "w");
      if (fp == NULL)
	{
	  Logg->write_time_warning("cannot write csv output file %s\n", csv_out_file.c_str());
	}
      else
	{
	  bool printed_header = false;

	  for (size_t i=0; i<output_road_segment_data_vec.size(); i++)
	    {
	      if (covered[i] == 1)
		{

		  if (!printed_header)
		    {
		      output_road_segment_data_vec[i]->print_header(fp);
		      printed_header = true;
		    }

		  if (args.debug_segments)
		    {
		      road_segment_data_vec[i].print_values(fp, i);
		    }
			  
		  output_road_segment_data_vec[i]->print_values(fp, i);
		}
	    }

	  fclose(fp);
	}
    }
  else if (args.dicast_csv_out_file_base != "")
    {
      // Create output file name using date/time
      string date_string;
      ymdhms(current_time, date_string);
      string csv_out_file = args.dicast_csv_out_file_base + "." + date_string.substr(0, 8) + "." + date_string.substr(8, 4) + ".csv";

      Logg->write_time_info("writing wiper data for %ld road segments to file: %s\n", output_road_segment_data_vec.size(), csv_out_file.c_str());

      FILE *fp = fopen(csv_out_file.c_str(), "w");
      if (fp == NULL)
	{
	  Logg->write_time_warning("cannot write csv output file %s\n", csv_out_file.c_str());
	}

      else
	{
	  for (size_t i=0; i<output_road_segment_data_vec.size(); i++)
	    {
	      if (covered[i] == 1)
		{
		  output_road_segment_data_vec[i]->print_aux_id_field(fp, wiper_status_field_number);
		}
	    }

	  fclose(fp);
	}
    }

  // Output the simulated probe messages
  Logg->write_time_info("writing %ld probe messages\n", combined_probe_messages.size());
  for (unsigned int i=0; i<output_road_segment_data_vec.size(); i++)
    {
      delete output_road_segment_data_vec[i];
      output_road_segment_data_vec[i] = 0;
    }

  Logg->write_time_info("returning from do_simulation()\n");
  return 0;
}

process::~process()
{
}

int process::run()
{
  time_t begin_time = args.begin_time;
  string begin_time_string = args.begin_time_string;
  time_t end_time = args.end_time;
  string end_time_string = args.end_time_string;

  // Read configuration file
  Logg->write_time_info("reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time_error("configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error: ") + cfg_reader.error;
      return -1;
    }

  vdt_climatology_file_reader *climate_data = 0;
  vector<vdt_road_segment> input_segments;
  vector<vdt_road_segment> rwis_sites;
  vector<int> seg_ids;
  vector<int> rwis_site_ids;
 

  vector<string> field_names = {{"lat", "lon", "radar_x", "radar_y", "radar_ref", "wiper_status", "model_x", "model_y", "model_air_temp", "model_bar_press", "model_dewpoint", "model_geopotential_height", "model_wind_dir", "model_wind_speed", "nss_air_temp_mean", "nss_bar_press_mean", "nss_dew_temp_mean", "nss_hourly_precip_mean", "nss_prevail_vis_mean", "nss_wind_dir_mean", "nss_wind_speed_mean"}};

  int wiper_status_field_number = 5; // this value must correspond to the placement location of wiper_status in field
  vector<string> field_units = {{"degrees", "degrees", "grid", "grid", "dbZ", "none", "grid", "grid", "celsius", "hPa", "celsius", "m", "degrees", "m/s", "celsius", "hPa", "celsius", "in", "km", "degrees", "m/s"}};
  vector<float> deviations = {{0, 0, 0, 0, 0, 0, 0, 0, 0.5, 1, 0.5, 0, 10, 1, 0.5, 1, 0.5, 0, 0, 10, 1}};

  Logg->write_time_info("using the following variables, units and deviations\n");
  for (unsigned int i=0; i<field_names.size(); i++)
    {
      Logg->write_time_info("%s, %s, %f\n", field_names[i].c_str(), field_units[i].c_str(), deviations[i]);
    }

  Logg->write_time_info("calling vdt_climatology_file_reader on climatology file %s\n", cfg_reader.climatology_file.c_str());
  climate_data = new vdt_climatology_file_reader(cfg_reader.climatology_file);
  if (climate_data->error() != "")
    {
      Logg->write_time_warning("failed in vdt_climatology_file_reader constructor: %s\n", climate_data->error().c_str());
    }

  string road_seg_file;
  if (args.car_density == arguments::LESS_DENSE)
    {
      Logg->write_time_info("using less dense road segment file %s\n", cfg_reader.less_dense_file.c_str());
      road_seg_file = cfg_reader.less_dense_file;
    }
  else
    {
      Logg->write_time_info("using more dense road segment file %s\n", cfg_reader.more_dense_file.c_str());
      road_seg_file = cfg_reader.more_dense_file;
    }

  if (boost::filesystem::exists(road_seg_file))
    {
      Logg->write_time_info("calling vdt_road_segment_file_reader(%s)\n", road_seg_file.c_str());
      vdt_road_segment_file_reader seg_reader = vdt_road_segment_file_reader(road_seg_file.c_str());

      input_segments = seg_reader.get_road_segments();
      rwis_sites = seg_reader.get_rwis_sites();
    }
  else
    {
      Logg->write_time_error("cannot read road segment file %s\n", road_seg_file.c_str());
      return -1;
    }

  if (args.test)
    {
      Logg->write_time_info("initializing simulator test messages\n");
    }
  else
    {
      vector<vector<string> > file_lists;
      Logg->write_time_info("assembling input data file lists for simulation\n");
      int ret = get_file_lists(args, file_lists);
      if (ret < 0)
	{
	  Logg->write_time_error("failed in get_file_lists()\n");
	  return -1;
	}

      // Perform simulation
      Logg->write_time_info("performing simulation\n");

      unsigned int ct = 0;
      for (time_t current_time=begin_time; current_time<end_time; current_time+=args.time_delta)
	{
	  ret = do_simulation(ct, current_time, cfg_reader, file_lists, field_names, field_units, wiper_status_field_number, deviations, input_segments);
	  if (ret < 0)
	    {
	      Logg->write_time_error("failed in do_simulation()\n");
	      return -1;
	    }

	  ct += 1;
	}
    }

  return 0;
}
