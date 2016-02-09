//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_history_check.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014/04/11 20:33:20 $
//
//==============================================================================

/**
 * @file vdt_history_check.cc
 *
 */

// Include files

#include <math.h>

#include "vdt_probe_message_history.hh"
#include "vdt_probe_message_qc.hh"
#include "vdt_history_check.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables

// Functions

bool obs_time_comparison(const vdt_probe_message_qc &msg1, const vdt_probe_message_qc &msg2)
{
  return msg1.get_obs_time() > msg2.get_obs_time();
}

bool rec_time_comparison(const vdt_probe_message_qc &msg1, const vdt_probe_message_qc &msg2)
{
  return msg1.get_rec_time() > msg2.get_rec_time();
}

vdt_history_check::vdt_history_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg) : begin_time(begin_time_arg), end_time(end_time_arg), sensor_minutes(sensor_minutes_arg)
{
}

vdt_history_check::vdt_history_check(time_t begin_time_arg, time_t end_time_arg, unsigned int sensor_minutes_arg, float sensor_rate_magnitude_arg) : begin_time(begin_time_arg), end_time(end_time_arg), sensor_minutes(sensor_minutes_arg), sensor_rate_magnitude(sensor_rate_magnitude_arg)
{
}

vdt_history_check::~vdt_history_check()
{
}

bool vdt_history_check::operator()(vector<vdt_probe_message_qc> &probe_message, size_t start, size_t offset) const
{
  return true;
}

void history_check_element(const vector<HISTORY_CHECK_PTR> &history_check, size_t start, vector<vdt_probe_message_qc> &combination)
{
  for (size_t j=0; j<history_check.size(); j++)
    {
      for (size_t i=start+1; i<combination.size(); i++)
        {
          bool test_complete = (*history_check[j])(combination, start, i);

          if (test_complete)
            {
              break;
            }
        }
    }
}

void vdt_run_history_check(const vector<HISTORY_CHECK_PTR> &history_check, const vector<vdt_probe_message_qc> &current_probe_message, const unordered_map< string, vector<vdt_probe_message_qc> > &probe_message_history_map, time_t begin_time, time_t end_time, vector<vdt_probe_message_qc> &out_probe_message)
{
  unordered_map<string, vector<vdt_probe_message_qc> > current_probe_message_history_map;

  // Create probe message history map of current probe messages
  vdt_probe_message_history(current_probe_message, current_probe_message_history_map);

  // Iterate through vehicle ids in current_probe_message_history_map
  for (auto current_itr=current_probe_message_history_map.begin(); current_itr!=current_probe_message_history_map.end(); ++current_itr)
    {
      // Find current_itr->first in probe_message_history_map
      auto history_map_itr = probe_message_history_map.find(current_itr->first);
      vector<vdt_probe_message_qc> combination; // combines current and historic probe messages having the same vehicle id

      // Did not find current_itr->first in probe_message_history_map
      for (size_t i=0; i<current_itr->second.size(); i++)
	{
          if (begin_time <= current_itr->second[i].get_rec_time() && current_itr->second[i].get_rec_time() < end_time)
	    {
	      combination.push_back(current_itr->second[i]);
	    }
	}

      if (history_map_itr != probe_message_history_map.end())
        {
	  // copy historical probe messages with the same vehicle id
	  // to the end of the combination vector as long as they were
	  // received before the begin_time of this time period 
	  for (size_t i=0; i<history_map_itr->second.size(); i++)
	    {
	      if (history_map_itr->second[i].get_rec_time() < begin_time)
		{
		  combination.push_back(history_map_itr->second[i]);
		}
	    }
	 }	 

      if (combination.size() == 1)
	{
	  continue;
	}

      // sort the probe messages into obs time descending order
      sort(combination.begin(), combination.end(), obs_time_comparison);

      // Check that there are at least two different observation times
      // in the combination vector avoiding the situation that there
      // are multiple repeats of the same observation time for a
      // single vehicle
      bool different_obs_times = false;
      for (size_t i=1; i<combination.size(); i++)
	{
	  if (combination[i].get_obs_time() != combination[0].get_obs_time())
	    {
	      different_obs_times = true;
	    }
	}
      
      if (different_obs_times == false)
	{
	  continue;
	}

      // Now do history check on all probe messages in combination
      // vector for vehicle id current_itr->first
      for (size_t i=0; i<combination.size(); i++)
        {
          // Make sure that the message is a current message and this
          // means that the rec_time of the message is between
          // begin_time and end_time
          if (begin_time <= combination[i].get_rec_time() && combination[i].get_rec_time() < end_time)
            {
              // This is a current probe message so perform history check
              size_t start = i;
              history_check_element(history_check, start, combination);
            }
        }

      // Now copy current probe messages in combination vector to the output probe message vector
      for (size_t i=0; i<combination.size(); i++)
	{
          if (begin_time <= combination[i].get_rec_time() && combination[i].get_rec_time() < end_time)
	    {
	      out_probe_message.push_back(combination[i]);
	    }
	}

    }
}
