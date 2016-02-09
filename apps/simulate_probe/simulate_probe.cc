//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file simulate_probe.cc
 *
 * <file description>
 *
 * @date 12/16/14
 */

// Include files 
#include <unordered_map>
#include <random>
#include "vdt4/vdt_const.hh"
#include "vdt4/stat_tools.hh"
#include "simulate_probe.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

void normal_distribution(size_t number, float mean, float standard_deviation, vector<float> &values)
{
  std::random_device rd;

  std::mt19937 e2(rd());

  std::normal_distribution<> dist(mean, standard_deviation);

  for (size_t i=0; i<number; i++)
    {
      float value = dist(e2);
      values.push_back(value);
    }
}

void simulate_probe_road_segment_data(size_t number_probe_messages, size_t number_fields, const vector<float> &standard_deviation_vec, const vdt_road_segment_data &road_segment_data, vdt_road_segment_data &output_road_segment_data, vector<vdt_generic_message> &probe_messages)
{
  vector<vector <float> > field_distributions(number_fields);

  // Generate the distributions for each field
  // Then recalculate the mean of each distribution
  for (size_t i=0; i<number_fields; i++)
    {
      float field_mean = vdt_const::FILL_VALUE;
      if (road_segment_data.get_field(i) != vdt_const::FILL_VALUE)
	{
	  if (number_probe_messages > 1 && standard_deviation_vec[i] != 0)
	    {
	      normal_distribution(number_probe_messages, road_segment_data.get_field(i), standard_deviation_vec[i], field_distributions[i]);
	      field_mean = ST_mean(field_distributions[i]);
	    }
	  else
	    {
	      field_mean = road_segment_data.get_field(i);
	    }
	}

      output_road_segment_data.set_field(i, field_mean);
    }

  // Consolidate the field distributions into probe messages
}

