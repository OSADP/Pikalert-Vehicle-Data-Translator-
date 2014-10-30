//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: vdt_index_distance.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 * @file vdt_index_distance.cc
 *
 */

// Include files 
#include "vdt_index_distance.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


ostream& operator<<(ostream &ostr, const vdt_index_distance &index_distance)
{
  ostr << "index: " << index_distance.index << std::endl;
  ostr << "probe message indices: " << std::endl;

  for (size_t i=0; i<index_distance.indices.size(); i++)
    {
      ostr << "probe message index, distance: " << index_distance.indices[i] << ", " << index_distance.distance[i] << std::endl;
    }

  return ostr;
}
