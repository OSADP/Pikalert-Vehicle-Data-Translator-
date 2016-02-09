//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: vdt_probe_message_history.cc,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2014/05/27 20:12:22 $
//
//==============================================================================

/**
 * @file vdt_probe_message_history.cc
 *
 * Implementation of vdt_probe_message_history()
 */

// Include files
#include <algorithm>
#include <functional>
#include "vdt_probe_message_history.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables

// Functions

void vdt_probe_message_history(const vector<vdt_probe_message_qc> &probe_message_input, unordered_map<string, vector<vdt_probe_message_qc> > &probe_message_history_map)
{
  typedef std::pair<double, size_t> pair_type_t;
  unordered_map<string, vector<pair_type_t> > probe_message_history_index_map;

  // Store probe messages having non-null string identifiers in history map
  //printf("\nStore probe messages having non-null string identifiers in history map\n");
  for (size_t i=0; i<probe_message_input.size(); i++)
    {
      const string &id = probe_message_input[i].get_vehicle_id();
      if (id != "")
        {
	  //printf("probe_message_input[%zu].obs_time: %lf\n", i, probe_message_input[i].obs_time);
          probe_message_history_index_map[id].push_back(pair_type_t(probe_message_input[i].get_obs_time(), i));
        }
    }

  // Iterate through probe_message_history_index_map sorting the probe
  // messages in probe_message_history_index_map in time descending order
  for (auto itr=probe_message_history_index_map.begin(); itr!=probe_message_history_index_map.end(); ++itr)
    {
      //printf("\nSorting for vehicle_id: %s\n", itr->first.c_str());
      sort(itr->second.begin(), itr->second.end(), std::greater<pair_type_t>());

      for (auto vec_itr=itr->second.begin(); vec_itr!=itr->second.end(); ++vec_itr)
        {
          probe_message_history_map[itr->first].push_back(probe_message_input[vec_itr->second]);
        }

      for (size_t i=0; i<probe_message_history_map[itr->first].size(); i++)
	{
	  //printf("history map, obs_time %s: %lf\n", itr->first.c_str(), probe_message_history_map[itr->first][i].obs_time);
	  //printf("history map: vehicle_id, obs_time: %s, %lf\n", itr->first.c_str(), probe_message_history_map[itr->first][i].obs_time);
	}
    }
}
