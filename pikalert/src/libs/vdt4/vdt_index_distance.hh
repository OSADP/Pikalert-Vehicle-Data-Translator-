//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: vdt_index_distance.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/04/10 17:43:27 $
//
//==============================================================================

/**
 * @file vdt_index_distance.hh
 *
 * @class vdt_index_distance
 *
 */

#ifndef VDT_INDEX_DISTANCE_HH
#define VDT_INDEX_DISTANCE_HH

#include <iostream>
#include <vector>

using std::ostream;
using std::vector;

/**
 * @class vdt_index_distance
 * supports the vdt_nearest_nbr class
 * maps the index of the road segment to the indices of the probe messages that fall on the segment and their distances
 */
class vdt_index_distance
{
public:

  /** @brief ind index of object (segment or grid cell) */
  int index;

  /** @brief probe message indices connected with source by location */
  vector<int> indices;

  /** @brief distances to probe message locations */
  vector<double> distance;

  /** @brief print index distance information */
  friend ostream & operator<<(ostream &ostr, const vdt_index_distance &index_distance);
};

#endif /* VDT_INDEX_DISTANCE_HH */
