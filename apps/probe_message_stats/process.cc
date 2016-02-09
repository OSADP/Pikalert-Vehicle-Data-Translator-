//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/process.cc
 *
 */

// Include files
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include <boost/format.hpp>
#include <Eigen/Dense>
#include <unordered_map>

using namespace Eigen;

#include "process.hh"
#include "vdt/stat_tools.hh"
#include "vdt/vdt_probe_message_file_reader.hh"

using std::unordered_map;
using namespace std;
using namespace libconfig;
using boost::format;

extern Log *Logg;
extern int Debug_level;

const double FILL_VALUE = -9999.;

process::~process()
{
}

int print_header(FILE *fp_out)
{
  return(fprintf(fp_out, "num_invalid_location,num_valid_location,num_cells,area_cells,percent_cell_coverage,latitude_min,latitude_iqr_25,latitude_median,latitude_iqr_75,latitude_max,longitude_min,longitude_iqr_25,longitude_median,longitude_iqr_75,longitude_max,air_temp2_min,air_temp2_iqr_25,air_temp2_median,air_temp2_iqr_75,air_temp2_max,surface_temp_min,surface_temp_iqr_25,surface_temp_median,surface_temp_iqr_75,surface_temp_max\n"));
}

int print_field(FILE *fp_out, const vector<double> &field_values, const vector<size_t> &quartile_indices)
{
  if (field_values.size() == 0)
    {
      return(fprintf(fp_out, ",-9999.,-9999.,-9999.,-9999.,-9999."));
    }
  else
    {
      return(fprintf(fp_out, ",%lf,%lf,%lf,%lf,%lf", field_values[0],field_values[quartile_indices[0]],field_values[quartile_indices[1]],field_values[quartile_indices[2]],field_values[field_values.size()-1]));
    }
}

int print_field(FILE *fp_out, const vector<float> &field_values, const vector<size_t> &quartile_indices)
{
  if (field_values.size() == 0)
    {
      return(fprintf(fp_out, ",-9999.,-9999.,-9999.,-9999.,-9999."));
    }
  else
    {
      return(fprintf(fp_out, ",%f,%f,%f,%f,%f", field_values[0],field_values[quartile_indices[0]],field_values[quartile_indices[1]],field_values[quartile_indices[2]],field_values[field_values.size()-1]));
    }
}

int msg_stats(time_t nearest_delta_time, vector<vdt_probe_message> &msgs, FILE *fp_out)
{
  // Get rid of messages with 0 or missing latitude or longitude
  vector<double> obs_time;
  vector<double> latitude;
  vector<double> longitude;
  vector<float> air_temp2;
  vector<float> surface_temp;

  MatrixXi conus_lat_long = MatrixXi::Zero(90, 180);

  size_t invalid_location = 0;
  for (size_t i=0; i<msgs.size(); i++)
    {
      double lat = msgs[i].get_latitude();
      double lon = msgs[i].get_longitude();
      float air_temp2_value = msgs[i].get_air_temp2();
      float surface_temp_value = msgs[i].get_surface_temp();
      if (lat == 0.0 or lat == FILL_VALUE or lon == 0.0 or lon == FILL_VALUE)
	{
	  invalid_location++;
	}
      else
	{
	  int int_lat = static_cast<int> (fabs(floor(lat)));
	  int int_lon = static_cast<int> (fabs(floor(lon)));
	  conus_lat_long(int_lat, int_lon) = 1;
	  latitude.push_back(lat);
	  longitude.push_back(lon);

	  if (air_temp2_value != FILL_VALUE)
	    {
	      air_temp2.push_back(air_temp2_value);
	    }

	  if (surface_temp_value != FILL_VALUE)
	    {
	      surface_temp.push_back(surface_temp_value);
	    }
	}
    }

  if (latitude.size() == 0)
    {
      Logg->write_time_info("Error: no valid lat-long locations\n");
      return 1;
    }

  int number_cells = conus_lat_long.sum();
  Logg->write_time_info("number cells %d, number 0 messages = %ld, number messages with valid location %ld\n", number_cells, invalid_location, latitude.size());
  
  vector<size_t> lat_quartiles;  
  vector<size_t> lon_quartiles;  
  vector<size_t> air_temp2_quartiles;  
  vector<size_t> surface_temp_quartiles;  

  ST_quartiles(latitude, lat_quartiles);
  ST_quartiles(longitude, lon_quartiles);
  ST_quartiles(air_temp2, air_temp2_quartiles);
  ST_quartiles(surface_temp, surface_temp_quartiles);

  int area_cells = fabs((floor(latitude[latitude.size()-1]) - floor(latitude[0]) + 1) * (floor(longitude[longitude.size()-1]) - floor(longitude[0]) + 1));
  int percent_cell_coverage = static_cast<int>(100*static_cast<float>(number_cells)/area_cells);

  Logg->write_time_info("range cells %d, percent cells covered %f\n", area_cells, static_cast<float>(number_cells)/area_cells);


  // Write output statistics
  Logg->write_time_info("writing output statistics for probe message file[s]\n");

 
  char date_string[64];
  strftime(date_string, 64, "%Y%m%d:%H%M", gmtime(&nearest_delta_time));

  fprintf(fp_out, "%s,%ld,%ld,%d,%d,%d", date_string, invalid_location, longitude.size(), number_cells, area_cells, percent_cell_coverage);
	  
  if (print_field(fp_out, latitude, lat_quartiles) < 0)
    {
      Logg->write_time_error("failed writing latitude\n");
      return -1;
    }

  if (print_field(fp_out, longitude, lon_quartiles) < 0)
    {
      Logg->write_time_error("failed writing longitude\n");
      return -1;
    }

  if (print_field(fp_out, air_temp2, air_temp2_quartiles) < 0)
    {
      Logg->write_time_error("failed writing air_temp2\n");
      return -1;
    }

  if (print_field(fp_out, surface_temp, surface_temp_quartiles) < 0)
    {
      Logg->write_time_error("failed writing surface_temp\n");
      return -1;
    }
  fprintf(fp_out, "\n");
  return 0;
}

int process::run()
{
  // Read configuration file
  Logg->write_time("Info: reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      return -1;
    }

  // Read probe messages from each input file into vector 
  vector<vdt_probe_message> msgs;

  // NOTE: the reader should be updated to include file list reading capability
  unordered_map<time_t, vector<vdt_probe_message> > time_vector_map;
  unordered_map<time_t, vector<vdt_probe_message> >::iterator itr;
  vector<time_t> delta_times;
  vector<vdt_probe_message> file_msgs;

  for (size_t i=0; i<args.probe_message_file_list.size(); i++)
    {
      file_msgs.clear();

      vdt_probe_message_file_reader probe_rdr(args.probe_message_file_list[0].c_str());
      probe_rdr.get_probe_messages(file_msgs);

      Logg->write_time("Info: processing file %s having %ld messages\n", args.probe_message_file_list[i].c_str(), file_msgs.size());

      // append file_msgs to msgs
      for (size_t j=0; j<file_msgs.size(); j++)
	{
	  double msg_time = file_msgs[j].get_obs_time();
	  time_t nearest_delta_time = floor(msg_time/args.time_delta) * args.time_delta;
	  printf("nearest delta time %ld, time_delta %d\n", nearest_delta_time, args.time_delta);

	  itr = time_vector_map.find(nearest_delta_time);
	  if (itr == time_vector_map.end())
	    {
	      delta_times.push_back(nearest_delta_time);
	      time_vector_map[nearest_delta_time].push_back(file_msgs[j]);
	    }
	  else
	    {
	      itr->second.push_back(file_msgs[j]);
	    }
	  //time_vector_map[nearest_delta_time].push_back(file_msgs[j]);
	}
    }


  // Sort delta_times into ascending order
  sort(delta_times.begin(), delta_times.end());
  Logg->write_time("Info: number of delta times %ld\n", delta_times.size());

  FILE *fp_out = fopen(args.out_file.c_str(), "w");

  if (print_header(fp_out) < 0)
    {
      Logg->write_time_info("failed in print_header to %s\n");
      return -1;
    }

  for (size_t i=0; i<delta_times.size(); i++)
    {
      vector<vdt_probe_message> &msgs = time_vector_map[delta_times[i]];
      Logg->write_time("Info: delta_time, number of messages: %ld, %ld\n", delta_times[i], msgs.size());

      int ret = msg_stats(delta_times[i], msgs, fp_out);
      if (ret < 0)
	{
	  Logg->write_time("Error: msgs_stats failure\n");
	  return -1;
	}
    }

  fclose(fp_out);

  return 0;
}
