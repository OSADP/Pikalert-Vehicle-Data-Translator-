//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: csv_writer.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2013/12/13 22:33:33 $
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

using std::string;
using std::unordered_map;
using std::vector;


int csv_writer(const string &file_name, const vector< std::pair<string, string> > &csv_field_selection, const unordered_map<string, string> &time_string_format_map, const rwx_vector_collection &vector_collection);


#endif /* CSV_WRITER_HH */
