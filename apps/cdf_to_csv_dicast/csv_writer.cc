//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: csv_writer.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/11/21 19:27:20 $
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
#include "csv_writer.hh"

using std::ofstream;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int csv_writer(const string &file_name, const vector<site *> &sites, unordered_map<int, int> &seg_site_map, const vector< std::pair<string, string> > &csv_field_selection, const rwx_vector_collection &vector_collection)
{
  // open output file
  ofstream outfile(file_name.c_str());
  if (outfile.is_open())
    {
      /*
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
      */
      
      // output values
      for (size_t i=0; i<vector_collection.size; i++)
	{
	  int output_flag = 0;
	  
	  for (size_t j=0; j<csv_field_selection.size(); j++)
	    {
	      string input_field = csv_field_selection[j].first;
	      string value = "";
	      int result = vector_collection.get_value(input_field, i, value);
	      
	      if (result != 0)
		{
		  outfile.close();
		  return -1;
		}
	      
	      // Conver unix time strings to readable date-time strings
	      if( strcmp(input_field.c_str(), "begin_time") == 0 || strcmp(input_field.c_str(), "end_time") == 0 )
		{
		  time_t time_value = (time_t)(atoi(value.c_str()));
		  char *date;
		  convert_utime_to_date(time_value, &date);
		  //printf("time_value: %d, date: %s\n", (int)time_value, date);
		  value = date;
		}
	      
	      // Get the associated "icao-id" from seg_site_map for sites in forecast site-list
	      if( strcmp(input_field.c_str(), "id") == 0 )
		{
		  int id = atoi(value.c_str());
		  
		  // Get the associated forecast site-list index for the give segment-id
		  int s;
		  if(seg_site_map.find(id) != seg_site_map.end())
		    {
		      s = seg_site_map[id];
		      //printf("i: %ld, seg-id: %d, sites[%d]->icao_id: %s\n", i, id, s, sites[s]->icao_id);
		      value = sites[s]->icao_id;
		      output_flag = 1;
		    }
		  else
		    {
		      s = -9999;
		      //printf("i: %ld, seg-id: %d, s: %d\n", i, id, s);
		      value = "-9999";
		      output_flag = 0;
		    }

		} // end if field is "id"
	      
	      // Ony ouput lines for the sites in the forecast site-list, 
	      // NOTE, this assumes the "id" is the first field to output
	      if(output_flag)
		{
	     
		  if (j != csv_field_selection.size() - 1) // output for all variables except last variable
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
		  else // output for last variable
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

		} // end if output_flag is true (1)

	    } // end loop over fields
	} // end loop over data collection
      
      outfile.close();
    }
  else
    {
      return -1;
    }

  return 0;
}


void convert_utime_to_date(time_t utime, char** date)
{
  *date = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;
  
  sprintf(*date, "%4d-%02d-%02d %02d:%02d:%02d", ct->tm_year, ct->tm_mon, ct->tm_mday, ct->tm_hour, ct->tm_min, ct->tm_sec);
}
