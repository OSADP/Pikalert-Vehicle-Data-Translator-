//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: csv_writer.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013/11/18 22:07:15 $
//
//==============================================================================

/**
 * @file csv_writer.hh
 *
 * @class csv_writer
 *
 *  <file/class description>
 *  
 * @date 7/30/13
 */

#ifndef CSV_WRITER_HH
#define CSV_WRITER_HH

#include <unordered_map>
#include <string>
#include <vector>

#include "rwx/rwx_vector_collection.hh"
#include "stat_utils.hh"

using std::string;
using std::unordered_map;
using std::vector;

int csv_writer(const string &file_name, const vector<site *> &sites, unordered_map<int, int> &seg_site_map, const vector< std::pair<string, string> > &csv_field_selection, const rwx_vector_collection &vector_collection);

void convert_utime_to_date(time_t utime, char** date);

#endif /* CSV_WRITER_HH */
