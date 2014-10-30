//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: json_writer.cc,v $
//       Version: $Revision: 1.14 $  Dated: $Date: 2014/05/28 21:46:17 $
//
//==============================================================================

/**
 * @file csv_writer.cc
 *
 * <file description>
 *
 * @date 7/30/13
 */

// Include files 
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <udunits2.h>
#include <stdlib.h>
#include "config_reader.hh"
#include "convert.hh"
#include "json_writer.hh"
#include <vdt4/vdt_const.hh>

#ifndef JSON_SPIRIT_VALUE_ENABLED
#error Please define JSON_SPIRIT_VALUE_ENABLED for the Value type to be enabled 
#endif

using std::ofstream;
using namespace json_spirit;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

#ifdef NOTNOW
// Create json spirit districts object from date time string and district vehicles map
void create_district_vehicle_index_map(const string &date_time_string, const vector<rwx::district_t> &districts, const rwx_vector_collection_nc &vector_collection, unordered_map<rwx::district_t, vector<size_t> > &district_vehicle_index_map)
{
  // Iterate through vector_collection of vehicle data attaching fields appropriate
  // districts based on lat-long coordinates
  const vector<double> *latitude_ptr;
  const vector<double> *longitude_ptr;
  vector_collection.get_vector("latitude", &latitude_ptr);
  vector_collection.get_vector("longitude", &longitude_ptr);

  const vector<double> &latitude = *latitude_ptr;
  const vector<double> &longitude = *longitude_ptr;

  for (size_t i=0; i<latitude.size(); i++)
    {
      for (size_t j=0; j<districts.size(); j++)
	{
	  if (districts[j].min_lat <= latitude[i] && latitude[i] <= district[j].max_lat)
	    {
	      if (districts[j].min_lon <= longitude[i] && longitude[i] <= district[j].max_lon)
		{
		  district_vehicle_index_map[districts[j]].push_back(i);
		}
	    }
	}
    }
}
#endif

// Create json spirit districts object from date time string and district vehicles map
int create_districts_obj(const string &date_time_string, const config_reader &cfg_reader, const rwx_vector_collection_nc &vector_collection, json_spirit::mObject& districts_obj)
{
  mArray district_vehicle_array;
  const vector<rwx::district_t> &districts = cfg_reader.districts;

  Convert convertObj;

  // Iterate through vector_collection of vehicle probe msg data
  // identifying indices appropriate to districts based on lat-long
  // coordinates
  const vector<double> *latitude_ptr;
  const vector<double> *longitude_ptr;
  vector_collection.get_vector("latitude", &latitude_ptr);
  vector_collection.get_vector("longitude", &longitude_ptr);

  const vector<double> &latitude = *latitude_ptr;
  const vector<double> &longitude = *longitude_ptr;
  unordered_map<size_t, vector<size_t> > district_vehicle_index_map;

  for (size_t i=0; i<latitude.size(); i++)
    {
      for (size_t j=0; j<districts.size(); j++)
	{
	  if (districts[j].min_lat <= latitude[i] && latitude[i] <= districts[j].max_lat)
	    {
	      if (districts[j].min_lon <= longitude[i] && longitude[i] <= districts[j].max_lon)
		{
		  district_vehicle_index_map[j].push_back(i);
		}
	    }
	}
    }

  // build district_vehicles object using district_vehicles_map
  for (auto itr=district_vehicle_index_map.begin(); itr!=district_vehicle_index_map.end(); ++itr)
    {
      mObject district_vehicles;
      district_vehicles["min_lat"] = districts[itr->first].min_lat;
      district_vehicles["max_lat"] = districts[itr->first].max_lat;
      district_vehicles["min_lon"] = districts[itr->first].min_lon;
      district_vehicles["max_lon"] = districts[itr->first].max_lon;
      district_vehicles["district_name"] = districts[itr->first].district_name;
      district_vehicles["display_name"] = districts[itr->first].display_name;

      mArray vehicles;
      for (size_t i=0; i<itr->second.size(); i++)
	{
	  mObject vehicle;

	  // iterate through the vehicle fields of interest
	  for (auto citr=cfg_reader.input_output_map.begin(); citr!=cfg_reader.input_output_map.end(); ++citr)
	    {
	      rwx::value_t value;
	      int ret = vector_collection.get_value(citr->first, itr->second[i], value);

	      if (ret == -1)
		{
		  return -1;
		}

	      switch (value.v_type)
	      {
		case NC_SHORT:
		{
		  if (citr->second.precision != "")
		    {
		      float float_conv_val = float(value.v_short);

		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_short != vdt_const::FILL_VALUE)
			    {
			      float_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, float(value.v_short));

			      // establish precision for the stored value using config information
			      char buf[16];
			      snprintf(buf, 16, citr->second.precision.c_str(), float_conv_val);
			      vehicle[citr->second.output_name] = buf;
			    }
			  else
			    {
			      char buf[16];
			      snprintf(buf, 16, citr->second.precision.c_str(), value.v_short);
			      vehicle[citr->second.output_name] = buf;
			    }
			}
		      else
			{
			  char buf[16];
			  snprintf(buf, 16, citr->second.precision.c_str(), value.v_short);
			  vehicle[citr->second.output_name] = buf;
			}
		    }
		  else
		    {
		      char buf[16];
		      snprintf(buf, 16, citr->second.precision.c_str(), value.v_short);
		      vehicle[citr->second.output_name] = buf;
		    }
		  break;
		}
		case NC_INT:
		{
		  if (citr->second.precision != "")
		    {
		      float float_conv_val = float(value.v_short);

		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_int != vdt_const::FILL_VALUE)
			    {
			      float_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, float(value.v_int));

			      // establish precision for the stored value using config information
			      char buf[16];
			      snprintf(buf, 16, citr->second.precision.c_str(), float_conv_val);
			      vehicle[citr->second.output_name] = buf;
			    }
			  else
			    {
			      char buf[16];
			      snprintf(buf, 16, citr->second.precision.c_str(), value.v_int);
			      vehicle[citr->second.output_name] = buf;
			    }
			}
		      else
			{
			  char buf[16];
			  snprintf(buf, 16, citr->second.precision.c_str(), value.v_int);
			  vehicle[citr->second.output_name] = buf;
			}
		    }
		  else
		    {
		      char buf[16];
		      snprintf(buf, 16, "%d", value.v_int);
		      vehicle[citr->second.output_name] = buf;
		    }
		  break;
		}
		case NC_FLOAT:
		{
		  float float_conv_val = float(value.v_float);
		  if (citr->second.precision != "")
		    {
		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_float != vdt_const::FILL_VALUE)
			    {
			      float_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, float(value.v_float));
			    }
			}
		      
		      // establish precision for the stored value using config information
		      char buf[16];
		      snprintf(buf, 16, citr->second.precision.c_str(), float_conv_val);
		      vehicle[citr->second.output_name] = buf;
		    }
		  else
		    {
		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_float != vdt_const::FILL_VALUE)
			    {
			      float_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, float(value.v_float));
			    }
			}

		      char buf[16];
		      snprintf(buf, 16, "%f", float_conv_val);
		      vehicle[citr->second.output_name] = buf;
		    }
		  break;
		}
		case NC_DOUBLE:
		{
		  double double_conv_val = double(value.v_double);
		  if (citr->second.precision != "")
		    {
		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_double != vdt_const::FILL_VALUE)
			    {
			      double_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, double(value.v_double));
			    }
			}
		      // establish precision for the stored value using config information
		      char buf[32];
		      snprintf(buf, 32, citr->second.precision.c_str(), double_conv_val);
		      vehicle[citr->second.output_name] = buf;
		    }
		  else
		    {
		      // convert the value if necessary
		      if (citr->second.input_units != "")
			{
			  if (value.v_double != vdt_const::FILL_VALUE)
			    {
			      double_conv_val = convertObj.convert(citr->second.input_units, citr->second.output_units, double(value.v_double));
			    }
			}
		      char buf[32];
		      snprintf(buf, 32, "%lf", double_conv_val);
		      vehicle[citr->second.output_name] = buf;
		    }
		  break;
		}
		case NC_CHAR:
		{
		  vehicle[citr->second.output_name] = value.v_string;
		  break;
		}		
	      }
	    }
	  vehicles.push_back(vehicle);
	}

      district_vehicles["vehicles"] = vehicles;
      district_vehicle_array.push_back(district_vehicles);
    }

  districts_obj["districts"] = district_vehicle_array;
  districts_obj["data_time"] = date_time_string;
  return 0;
}


int json_writer(const string &date_time_string, const config_reader &cfg_reader, const string &file_name, const rwx_vector_collection_nc &vector_collection)
{
  json_spirit::mObject districts_obj;

  int ret = create_districts_obj(date_time_string, cfg_reader, vector_collection, districts_obj);
  if (ret < 0)
    {
      return ret;
    }

  //json_spirit::mValue districts_value(districts_obj);

  ofstream out_stream(file_name.c_str());

  //write_formatted(v, out_stream);
  write(districts_obj, out_stream, remove_trailing_zeros);
  return 0;
}

#ifdef NOTNOW
int ascii_writer(const string &file_name, const vector< std::pair<string, string> > &json_field_selection, const rwx_vector_collection_nc &vector_collection)
{
  // open output file
  ofstream outfile(file_name.c_str());
  if (outfile.is_open())
    {
      // output header
      for (size_t j=0; j<json_field_selection.size(); j++)
	{
	  string output_string = json_field_selection[j].second;
	  if (j != json_field_selection.size() - 1)
	    {
	      outfile << output_string << ",";
	    }
	  else
	    {
	      outfile << output_string << std::endl;
	    }
	}
      
      // output values
      for (size_t i=0; i<vector_collection.size; i++)
	{
	  for (size_t j=0; j<json_field_selection.size(); j++)
	    {
	      string input_field = json_field_selection[j].first;
	      string value = "";
	      int result = vector_collection.get_value(input_field, i, value);

	      if (result != 0)
		{
		  outfile.close();
		  return -1;
		}

	      if (j != json_field_selection.size() - 1)
		{
		  if (value == "")
		    {
		      outfile << ",";
		    }
		  else
		    {
		      outfile << value << ",";
		    }
		}
	      else
		{
		  if (value == "")
		    {
		      outfile << std::endl;
		    }
		  else
		    {
		      outfile << value << std::endl;
		    }
		}
	    }
	}

      outfile.close();
    }
  else
    {
      return -1;
    }

  return 0;
}
#endif
