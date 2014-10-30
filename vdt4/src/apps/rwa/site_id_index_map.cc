//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: site_id_index_map.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/05/13 20:43:13 $
//
//==============================================================================

/**
 * @file site_id_index_map.cc
 *
 * <file description>
 *
 * @date 10/16/13
 */

// Include files 
#include "site_id_index_map.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

void create_site_id_index_map(const std::vector<int> &site_id, std::unordered_map<int, std::vector<std::size_t> > &site_id_index_map)
{
  for (std::size_t i=0; i<site_id.size(); i++)
    {
      site_id_index_map[site_id[i]].push_back(i);
    }
}
