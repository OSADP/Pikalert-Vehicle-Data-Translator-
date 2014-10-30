//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: json_writer.hh,v $
//       Version: $Revision: 1.4 $  Dated: $Date: 2013/11/26 17:38:53 $
//
//==============================================================================

/**
 * @file json_writer.hh
 *
 * @class json_writer
 *
 *  <file/class description>
 *  
 * @date 7/30/13
 */

#ifndef JSON_WRITER_HH
#define JSON_WRITER_HH

#include <unordered_map>
#include <string>
#include <vector>
#include "config_reader.hh"
#include "json_spirit.h"
#include "rwx/rwx_vector_collection_nc.hh"
#include "rwx/rwx_types.hh"

using std::string;
using std::unordered_map;
using std::vector;

double convert(const string &input_units, const string &output_units, double value);
float convert(const string &input_units, const string &output_units, float value);

int create_districts_obj(const string &date_time_string, const config_reader &cfg_reader, const rwx_vector_collection_nc &vector_collection, json_spirit::mValue &districts_obj);

int json_writer(const string &date_time_string, const config_reader &cfg_reader, const string &file_name, const rwx_vector_collection_nc &vector_collection);


#endif /* JSON_WRITER_HH */
