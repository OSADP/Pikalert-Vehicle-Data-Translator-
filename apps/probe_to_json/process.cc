//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file process.cc
 *
 */

// Include files
#include <algorithm>
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include "json_writer.hh"
#include "rwx/rwx_vector_collection_nc.hh"

#include "process.hh"

using namespace rwx;
using namespace std;
using namespace libconfig;

extern Log *Logg;

process::~process()
{
}

// For each id, find pair containing containing latest time and corresponding index
// Skip ids having the give prefix
int get_id_latest_probe_indices(const string &ignore_prefix, unordered_map<string, vector<std::pair<double, size_t> > > &id_time_index_map, vector<double> &time_vec, vector<size_t> &index_vec)
{
  
  int ignore_count = 0;
  
  for (auto itr=id_time_index_map.begin(); itr!=id_time_index_map.end(); ++itr)
    {
      string vehicle_id = itr->first;
      if (ignore_prefix == vehicle_id.substr(0, ignore_prefix.size()))
	{
	  ignore_count += 1;
	  continue;
	}
	
      vector<std::pair<double, size_t>  > current_time_index = itr->second;

      double max_time = current_time_index[0].first;
      size_t current_index = current_time_index[0].second;
      for (size_t i=1; i<current_time_index.size(); i++)
	{
	  if (current_time_index[i].first > max_time)
	    {
	      max_time = current_time_index[i].first;
	      current_index = current_time_index[i].second;
	    }
	}

      time_vec.push_back(max_time);
      index_vec.push_back(current_index);
    }

  return ignore_count;
}

// 
void get_id_time_index_map(const string &id_name, const string &time_name, rwx_vector_collection_nc &collection, unordered_map<string, vector<std::pair<double, size_t> > > &id_time_index_map)
{
  const vector<char> *id;

  // Get id name vector
  collection.get_vector(id_name, &id);
  
  // Dereference pointer
  const vector<char> &id_vec = *id;

  size_t string_size;
  collection.get_char_field_size(id_name, &string_size);

  const vector<double> *obs_time;
  collection.get_vector(time_name, &obs_time);

  vector<std::pair<double, size_t> > time_index;

  for (size_t offset=0; offset<id_vec.size(); offset+=string_size)
    {
      auto begin = &id_vec[offset];
      auto end = begin + string_size;
      string value(begin, end);
      auto pos = value.find('\0');
      if (pos != std::string::npos)
	{
	  value = value.substr(0, pos);
	}

      int ind = offset/string_size;
      id_time_index_map[value].push_back(std::make_pair((*obs_time)[ind], ind));
    }
}

int process::run()
{
  // Read configuration file
  Logg->write_time("Info: reading config file %s\n", args.cfg_file.c_str());

  config_reader cfg_reader(args.cfg_file);
  if (cfg_reader.error != string(""))
    {
      Logg->write_time("Error: configuration file error %s\n", cfg_reader.error.c_str());
      error = string("configuration file error") + cfg_reader.error;
      return -1;
    }

  Logg->write_time_info("reading input file %s\n", args.input_file.c_str());

  rwx_vector_collection_nc collection(args.input_file, cfg_reader.field_names);
  const std::unordered_map<string, size_t> &dim_map = collection.get_dim_map();
  auto dim_map_itr = dim_map.find("rec_num");
  if (dim_map_itr->second == 0)
    {
      Logg->write_time("Info: no probe messages available\n");
      return 0;
    }

  if (collection.error != string(""))
    {
      Logg->write_time("Error: ingest error for %s: %s\n", args.input_file.c_str(), collection.error.c_str());
      return -1;
    }

  const vector<size_t> & char_field_size = collection.get_char_field_size_vector();

  Logg->write_time("Info: getting latest probe messages having vehicle ids\n");

  // map each probe message vehicle id to a struct containing
  // the probe message index, observation

  // formulate id_time_index_map which maps vehicle ids to a vector of time, index pairs
  unordered_map<string, vector<std::pair<double, size_t> > > id_time_index_map;
  get_id_time_index_map(cfg_reader.vehicle_id_name, cfg_reader.time_field_name, collection, id_time_index_map);

  // find latest index for each vehicle id
  vector<double> time_vec;
  vector<size_t> index_vec;

  int ignore_count = get_id_latest_probe_indices(cfg_reader.vehicle_id_ignore_prefix, id_time_index_map, time_vec, index_vec);
  Logg->write_time("Info: index vector has size %ld, ignored %d messages\n", index_vec.size(), ignore_count);


  vector<vector<char> > latest_char_field_vector;
  vector<vector<short> > latest_short_field_vector;
  vector<vector<int> > latest_int_field_vector;
  vector<vector<float> > latest_float_field_vector;
  vector<vector<double> > latest_double_field_vector;

  collection.extract_indices(index_vec, latest_char_field_vector, latest_short_field_vector, latest_int_field_vector, latest_float_field_vector, latest_double_field_vector);

  const unordered_map<string,  std::pair<int, int> > &name_type_index_map = collection.get_name_type_index_map();

  rwx_vector_collection_nc latest_collection(name_type_index_map, latest_char_field_vector, char_field_size, latest_short_field_vector, latest_int_field_vector, latest_float_field_vector, latest_double_field_vector);
  if (latest_collection.error != "")
    {
      Logg->write_time("Error: latest collection error occurred: %s\n", latest_collection.error.c_str());
      return 1;
    }

  // now modify collection extracting those items corresponding to the latest probe messages

  Logg->write_time("Info: calling json_writer for outputting json file\n");
  int ret = json_writer(args.date_time_string, cfg_reader, args.output_file, latest_collection);
  if (ret < 0)
    {
      Logg->write_time("Error: json file writer error\n");
    }

  return ret;
}


