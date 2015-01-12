//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: csv_writer.cc,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013/12/13 22:33:33 $
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
#include <iostream>
#include <fstream>
#include <netcdf>
#include "csv_writer.hh"

using std::ofstream;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 
const int MAX_TIME_LEN = 32;

// Functions

int csv_writer(const string &file_name, const vector< std::pair<string, string> > &csv_field_selection, const unordered_map<string, string> &time_string_format_map, const rwx_vector_collection &vector_collection)
{
  // open output file
  ofstream outfile(file_name.c_str());
  if (outfile.is_open())
    {
      // output header
      for (size_t j=0; j<csv_field_selection.size(); j++)
	{
	  string output_string = csv_field_selection[j].second;
	  if (j != csv_field_selection.size() - 1)
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
	  for (size_t j=0; j<csv_field_selection.size(); j++)
	    {
	      string input_field = csv_field_selection[j].first;
	      string value = "";
	      int result = -1;

	      auto itr = time_string_format_map.find(input_field);
	      if (itr != time_string_format_map.end())
		{
		  rwx::value_t value_type;
		  int ret = vector_collection.get_value(input_field, i, value_type);
		  if (ret == 0)
		    {
		      double double_time_value;
		      int value_ret = value_type.get_value(&double_time_value);
		      if (value_ret != -1 && value_ret != NC_CHAR)
			{
			  time_t time_value = double_time_value;
			  struct tm *tms = gmtime(&time_value);
			  char char_buf[MAX_TIME_LEN];
			  strftime(char_buf, MAX_TIME_LEN, itr->second.c_str(), tms);
			  result = 0;
			  value = char_buf;
			}
		    }
		}
	      else
		{
		  result = vector_collection.get_value(input_field, i, value);
		}

	      if (result != 0)
		{
		  outfile.close();
		  return -1;
		}

	      if (j != csv_field_selection.size() - 1)
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
