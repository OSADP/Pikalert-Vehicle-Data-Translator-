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
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include "csv_writer.hh"
#include "rwx/rwx_vector_collection.hh"
#include "rwx/rwx_cdf_field_reader.hh"

#include "process.hh"

using namespace rwx;
using namespace std;
using namespace libconfig;

extern Log *Logg;

process::~process()
{
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

  // Open input cdf file
  rwx_cdf_field_reader cdf_file(args.input_file);
  
  // Read double fields
  vector<string> double_field_names;
  vector< vector<double> > double_field_vector;
  vector<string> double_units;
  vector<double> double_missing;
  string error;

  Logg->write_time("Info: getting double fields\n");
  int ret = cdf_file.get_fields(cfg_reader.double_field_names, double_field_vector, double_units, double_missing, error);
  
  vector<string> float_field_names;
  vector< vector<float> > float_field_vector;
  vector<string> float_units;
  vector<float> float_missing;

  Logg->write_time("Info: getting float fields\n");
  ret = cdf_file.get_fields(cfg_reader.float_field_names, float_field_vector, float_units, float_missing, error);

  vector<string> int_field_names;
  vector< vector<int> > int_field_vector;
  vector<string> int_units;
  vector<int> int_missing;

  Logg->write_time("Info: getting int fields\n");
  ret = cdf_file.get_fields(cfg_reader.int_field_names, int_field_vector, int_units, int_missing, error);

  vector<string> short_field_names;
  vector< vector<short> > short_field_vector;
  vector<string> short_units;
  vector<short> short_missing;

  Logg->write_time("Info: getting short fields\n");
  ret = cdf_file.get_fields(cfg_reader.short_field_names, short_field_vector, short_units, short_missing, error);

  vector<string> char_field_names;
  vector< vector<char> > char_field_vector;
  vector<size_t> char_field_size;

  Logg->write_time("Info: getting character fields\n");
  ret = cdf_file.get_fields(cfg_reader.char_field_names, char_field_vector, char_field_size, error);

  Logg->write_time("Info: formulating collection\n");
  rwx_vector_collection collection(cfg_reader.input_type_index_map, char_field_vector, char_field_size, short_field_vector, int_field_vector, float_field_vector, double_field_vector);
  if (collection.error != "")
    {
      Logg->write_time("Error: collection error occurred\n");
      return 1;
    }

  Logg->write_time("Info: outputting csv file\n");
  csv_writer(args.output_file, cfg_reader.input_output_csv, cfg_reader.time_string_format_map, collection);
  return 0;
}


