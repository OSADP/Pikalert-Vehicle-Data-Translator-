//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwx_vector_collection.hh,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2013/10/28 20:12:39 $
//
//==============================================================================

/**
 * @file vector_collection.hh
 *
 * @class vector_collection
 *
 *  A vector collection class 
 *  
 * @date 8/1/13
 */

#ifndef RWX_VECTOR_COLLECTION_HH
#define RWX_VECTOR_COLLECTION_HH

#include <string>
#include <unordered_map>
#include <vector>
#include "rwx_types.hh"

using std::string;
using std::unordered_map;
using std::vector;

/**
 * @class vector_collection
 */
class rwx_vector_collection
{
public:
  /** Constructor */
  rwx_vector_collection(const unordered_map<string, std::pair<int, int> > &name_type_index_map_arg, const vector< vector<char> > &char_field_vector_arg, const vector<size_t> &char_field_size_arg, const vector< vector<short> > &short_field_vector_arg, const vector< vector<int> > &int_field_vector_arg, const vector<vector<float> > &float_field_vector_arg, const vector<vector<double> > &double_field_vector_arg);

  /** @brief get name type index map */
  inline const unordered_map<string,  std::pair<int, int> > &get_name_type_index_map() const { return name_type_index_map; }
  
  /** @brief get value for a field given its field name and index offset. Store in appropriate field in struct value_t */
  int get_value(const string &name, size_t index, rwx::value_t &value) const;

  /** @brief get a string value for a field given its field name and index offset */
  int get_value(const string &name, size_t index, string &value) const;

  /** @brief get vector corresponding to given name */
  int get_vector(const string &name, const vector<char> **field) const;

  /** @brief get vector corresponding to given name */
  int get_vector(const string &name, const vector<short> **field) const;

  /** @brief get vector corresponding to given name */
  int get_vector(const string &name, const vector<int> **field) const;

  /** @brief get vector corresponding to given name */
  int get_vector(const string &name, const vector<float> **field) const;

  /** @brief get vector corresponding to given name */
  int get_vector(const string &name, const vector<double> **field) const;

  /** @brief get vector char field size */
  int get_char_field_size(const string &name, size_t *field_size) const;

  /** @brief extract indices */
  void extract_indices(const vector<size_t> &latest_id_index, vector< vector<char> > &char_field_vector_arg, vector< vector<short> > &short_field_vector_arg, vector< vector<int> > &int_field_vector_arg, vector<vector<float> > &float_field_vector_arg, vector<vector<double> > &double_field_vector_arg);
  
  /** @brief destructor */
  virtual ~rwx_vector_collection();

  const unordered_map<string, std::pair<int, int> > &name_type_index_map;
  const vector< vector<char> > &char_field_vector;
  const vector<size_t> &char_field_size;
  const vector< vector<short> > &short_field_vector;
  const vector< vector<int> > &int_field_vector;
  const vector<vector<float> > &float_field_vector;
  const vector<vector<double> > &double_field_vector;

  size_t size;
  string error;


private:
  
  rwx_vector_collection & operator=(const rwx_vector_collection &vector_collection);
  rwx_vector_collection(const rwx_vector_collection &vector_collection);
};

#endif /* RWX_VECTOR_COLLECTION_HH */
