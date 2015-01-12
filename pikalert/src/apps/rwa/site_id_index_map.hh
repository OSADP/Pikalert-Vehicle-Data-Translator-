//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: site_id_index_map.hh,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2014/05/13 20:43:13 $
//
//==============================================================================

/**
 * @file site_id_index_map.hh
 *
 * @class Site_Id_Index_Map
 *
 *  <file/class description>
 *  
 * @date 10/16/13
 */

#ifndef SITE_ID_INDEX_MAP_HH
#define SITE_ID_INDEX_MAP_HH

#include <vector>
#include <unordered_map>

void create_site_id_index_map(const std::vector<int> &site_id, std::unordered_map<int, std::vector<std::size_t> > &site_id_index_map);

#endif /* SITE_ID_INDEX_MAP_HH */
