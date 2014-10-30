//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwx_vector_collection_nc.hh,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/03/04 19:47:33 $
//
//==============================================================================

/**
 * @file vector_collection_nc.hh
 *
 * @class vector_collection_nc
 *
 *  A vector collection_nc class 
 *  
 * @date 8/1/13
 */

#ifndef RWX_VECTOR_COLLECTION_NC_HH
#define RWX_VECTOR_COLLECTION_NC_HH

#include <string>
#include <unordered_map>
#include <vector>
#include <netcdf>
#include "rwx_types.hh"

//using namespace netCDF;                                                                                        //  using namespace netCDF::exceptions;    

using std::string;
//using std::unordered_map;
using std::vector;

template<typename T>
int get_field(const netCDF::NcFile &data_file, const string &field_name, vector<T> &field, const string &units_name, string &units, const string &missing_name, T *missing, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file.getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  size_t total_size = 1;

  for (size_t i=0; i<dims.size(); i++)
    {
      total_size *= dims[i].getSize();
    }

  if (dims.size() > 0)
    {
      field.resize(total_size);
      var.getVar(&field[0]);
    }
  else
    {
      field.resize(1);
      var.getVar(&field[0]);
    }

  // Get units if available
  netCDF::NcVarAtt att;

  try
    {
      att = var.getAtt(units_name);
      att.getValues(units);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  try
    {
      att = var.getAtt(missing_name);
      att.getValues(missing);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  return 0;
}

/**
 * @class vector_collection_nc
 */
class rwx_vector_collection_nc
{
public:
  /** Constructor */
  rwx_vector_collection_nc(const std::unordered_map<string, std::pair<int, int> > &name_type_index_map_arg, const vector< vector<char> > &char_field_vector_arg, const vector<size_t> &char_field_size_arg, const vector< vector<short> > &short_field_vector_arg, const vector< vector<int> > &int_field_vector_arg, const vector<vector<float> > &float_field_vector_arg, const vector<vector<double> > &double_field_vector_arg);

  /** Constructor */
  rwx_vector_collection_nc(const string &file_name, const vector<string> &field_names);

  /** @brief initialize vector collection from netcdf file given a vector of field names */
  int init_vector_collection_nc(const netCDF::NcFile &data_file, const vector<string> &field_names, string &error_string);

  /** @brief get name type index map */
  inline const std::unordered_map<string,  std::pair<int, int> > &get_name_type_index_map() const { return name_type_index_map; }
  
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

  const vector<size_t> & get_char_field_size_vector() const { return char_field_size; }

  /** @brief extract indices */
  void extract_indices(const vector<size_t> &latest_id_index, vector< vector<char> > &char_field_vector_arg, vector< vector<short> > &short_field_vector_arg, vector< vector<int> > &int_field_vector_arg, vector<vector<float> > &float_field_vector_arg, vector<vector<double> > &double_field_vector_arg);
  
  /** @brief destructor */
  virtual ~rwx_vector_collection_nc();

  /** @brief get missing value for field name */
  int get_missing(const string &name, rwx::value_t &value) const;

  /** @brief get units for field name */
  int get_units(const string &name, string &units) const;

  /** @brief get variable map mapping variable name to pair consisting of dimension name and dimension length */
  const std::unordered_map<string, vector<std::pair<string, size_t> > > & get_var_map() const { return var_map; };

  /** @brief get dimension map */
  const std::unordered_map<string, size_t> & get_dim_map() const { return dim_map; };

  /** @brief get nc type map */
  const std::unordered_map<string, int> & get_type_map() const { return type_map; };


  string error;

private:
  std::unordered_map<string, std::pair<int, int> > name_type_index_map;
  vector<vector<char> > char_field_vector;
  vector<size_t> char_field_size;
  vector<vector<short> > short_field_vector;
  vector<vector<int> > int_field_vector;
  vector<vector<float> > float_field_vector;
  vector<vector<double> > double_field_vector;

  std::unordered_map<string, size_t> dim_map;
  std::unordered_map<string, vector<std::pair<string, size_t> > > var_map;

  std::unordered_map<string, rwx::value_t> missing_map;
  std::unordered_map<string, int> type_map;
  std::unordered_map<string, string> units_map;
  
  string units_name;
  string missing_name;

  size_t size;
};

#endif /* RWX_VECTOR_COLLECTION_NC_HH */
