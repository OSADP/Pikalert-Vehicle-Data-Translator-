//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_vector_collection_nc.cc,v $
//       Version: $Revision: 1.7 $  Dated: $Date: 2014/03/04 19:47:33 $
//
//==============================================================================

/**
 * @file vector_collection_nc.cc
 *
 * @date 8/1/13
 */

// Include files 
#include <boost/lexical_cast.hpp>
#include "rwx_vector_collection_nc.hh"

using boost::lexical_cast;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int get_field(const netCDF::NcFile &data_file, const string &field_name, vector<char> &field, size_t *char_field_size, string &error)
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
  if (dims.size() > 0)
    {
      *char_field_size = dims[dims.size()-1].getSize();
    }

  string units_name;
  string missing_name = "";
  char missing;
  string units;
  int ret = get_field<char>(data_file, field_name, field, units_name, units, missing_name, &missing, error);
  return ret;
}

int rwx_vector_collection_nc::init_vector_collection_nc(const netCDF::NcFile &data_file, const vector<string> &field_names, string &error_string)
{
  vector<netCDF::NcVar> var_vector;
  vector<netCDF::NcType::ncType> type_vector;
  int char_count = 0;
  int short_count = 0;
  int int_count = 0;
  int float_count = 0;
  int double_count = 0;

  for (uint i=0; i<field_names.size(); i++)
    {
      // Get the variable
      netCDF::NcVar var;
      var = data_file.getVar(field_names[i]);
      if (var.isNull())
	{
	  error_string = string("could not find variable: ") + field_names[i];
	  return -1;
	}

      var_vector.push_back(var);

      netCDF::NcType nc_type = var.getType();
      netCDF::NcType::ncType nc_type_class = nc_type.getTypeClass();
      type_vector.push_back(nc_type_class);
      type_map[field_names[i]] = nc_type_class;

      switch (nc_type_class)
	{
	case NC_CHAR:
	  char_count += 1;
	  break;

	case NC_INT:
	  int_count += 1;
	  break;

	case NC_SHORT:
	  short_count += 1;
	  break;

	case NC_FLOAT:
	  float_count += 1;
	  break;

	case NC_DOUBLE:
	  double_count += 1;
	  break;

	case NC_BYTE:
	case NC_UBYTE:
	case NC_USHORT:
	case NC_UINT:
	case NC_INT64:
	case NC_UINT64:
	case NC_STRING:
	case NC_COMPOUND:
	case NC_VLEN:
	case NC_OPAQUE:
	case NC_ENUM:
	  error_string = string("NC_BYTE, etc. not currently handled");
	  return -1;
	}
    }

  if (char_count > 0)
    {
      char_field_vector.resize(char_count);
      char_field_size.resize(char_count);
    }

  if (short_count > 0)
    {
      short_field_vector.resize(short_count);
    }

  if (int_count > 0)
    {
      int_field_vector.resize(int_count);
    }

  if (float_count > 0)
    {
      float_field_vector.resize(float_count);
    }

  if (double_count > 0)
    {
      double_field_vector.resize(double_count);
    }

  // count the various types
  
  int current_char_count = 0;
  int current_short_count = 0;
  int current_int_count = 0;
  int current_float_count = 0;
  int current_double_count = 0;

  for (uint i=0; i<field_names.size(); i++)
    {  
      switch (type_vector[i])
	{
	case NC_CHAR:
	  {
	    string error_occurred;
	    int error_return = 0;

	    error_return = get_field(data_file, field_names[i], char_field_vector[current_char_count], &char_field_size[current_char_count], error_occurred);

	    if (error_return != 0)
	      {
		error_string = error_occurred;
	      }

	    name_type_index_map[field_names[i]] = std::make_pair(NC_CHAR, current_char_count);

	    current_char_count += 1;
	    rwx::value_t value;
	    value.v_type = NC_CHAR;
	  }
	  break;

	case NC_SHORT:
	  {
	    short missing_value;
	    string units_string;
	    string error_occurred;
	    int error_return = 0;

	    error_return = get_field<short>(data_file, field_names[i], short_field_vector[current_short_count], units_name, units_string, missing_name, &missing_value, error_occurred);
	    if (error_return != 0)
	      {
		error_string = error_occurred;
	      }

	    name_type_index_map[field_names[i]] = std::make_pair(NC_SHORT, current_short_count);
	    current_short_count += 1;
	    rwx::value_t value;
	    value.v_type = NC_SHORT;
	    value.v_short = missing_value;
	    missing_map[field_names[i]] = value;
	    units_map[field_names[i]] = units_string;
	  }
	  break;

	case NC_INT:
	  {
	    int missing_value;
	    string units_string;
	    string error_occurred;
	    int error_return = 0;

	    error_return = get_field<int>(data_file, field_names[i], int_field_vector[current_int_count], units_name, units_string, missing_name, &missing_value, error_occurred);
	    if (error_return != 0)
	      {
		error_string = error_occurred;
	      }

	    name_type_index_map[field_names[i]] = std::make_pair(NC_INT, current_int_count);

	    current_int_count += 1;

	    rwx::value_t value;
	    value.v_type = NC_INT;
	    value.v_int = missing_value;
	    missing_map[field_names[i]] = value;
	    units_map[field_names[i]] = units_string;
	  }
	  break;

	case NC_FLOAT:
	  {
	    float missing_value;
	    string units_string;
	    string error_occurred;
	    int error_return = 0;

	    error_return = get_field<float>(data_file, field_names[i], float_field_vector[current_float_count], units_name, units_string, missing_name, &missing_value, error_occurred);
	    if (error_return != 0)
	      {
		error_string = error_occurred;
	      }

	    name_type_index_map[field_names[i]] = std::make_pair(NC_FLOAT, current_float_count);

	    current_float_count += 1;

	    rwx::value_t value;
	    value.v_type = NC_FLOAT;
	    value.v_float = missing_value;
	    missing_map[field_names[i]] = value;
	    units_map[field_names[i]] = units_string;
	  }
	  break;

	case NC_DOUBLE:
	  {
	    double missing_value;
	    string units_string;
	    string error_occurred;
	    int error_return = 0;

	    error_return = get_field<double>(data_file, field_names[i], double_field_vector[current_double_count], units_name, units_string, missing_name, &missing_value, error_occurred);
	    if (error_return != 0)
	      {
		error_string = error_occurred;
	      }

	    name_type_index_map[field_names[i]] = std::make_pair(NC_DOUBLE, current_double_count);

	    current_double_count += 1;
	    rwx::value_t value;
	    value.v_type = NC_DOUBLE;
	    value.v_double = missing_value;
	    missing_map[field_names[i]] = value;
	    units_map[field_names[i]] = units_string;
	  }
	  break;

	case NC_BYTE:
	case NC_UBYTE:
	case NC_USHORT:
	case NC_UINT:
	case NC_INT64:
	case NC_UINT64:
	case NC_STRING:
	case NC_COMPOUND:
	case NC_VLEN:
	case NC_OPAQUE:
	case NC_ENUM:
	  error_string = string("NC_BYTE, etc. not currently handled");
	  return -1;
	}
    }

  return 0;
}

rwx_vector_collection_nc::rwx_vector_collection_nc(const string &file_name, const vector<string> &field_names)
{
  error = "";
  units_name = "units";
  missing_name = "_FillValue";

  try
    {
      // Open the file and check to make sure it's valid.
      netCDF::NcFile data_file(file_name, netCDF::NcFile::read);
      std::multimap<string, netCDF::NcDim> nc_dim_map = data_file.getDims();
      std::multimap<string, netCDF::NcVar> nc_var_map = data_file.getVars();
      for (auto itr=nc_dim_map.begin(); itr!=nc_dim_map.end(); ++itr)
	{
	  dim_map[itr->first] = itr->second.getSize();
	}

      for (auto itr=nc_var_map.begin(); itr!=nc_var_map.end(); ++itr)
	{
	  vector<netCDF::NcDim> dims = itr->second.getDims();
	  for (size_t i=0; i<dims.size(); i++)
	    {
	      var_map[itr->first].push_back(std::make_pair(dims[i].getName(), dims[i].getSize()));
	    }
	}

      init_vector_collection_nc(data_file, field_names, error);
    }
  catch (netCDF::exceptions::NcException e)
    {
      // Capture exception in errorby redirecting cout
      error = e.what();
    }

}

rwx_vector_collection_nc::rwx_vector_collection_nc(const std::unordered_map<string, std::pair<int, int> > &name_type_index_map_arg, const vector< vector<char> > &char_field_vector_arg, const vector<size_t> &char_field_size_arg, const vector< vector<short> > &short_field_vector_arg, const vector< vector<int> > &int_field_vector_arg, const vector<vector<float> > &float_field_vector_arg, const vector<vector<double> > &double_field_vector_arg) : name_type_index_map(name_type_index_map_arg), char_field_vector(char_field_vector_arg), char_field_size(char_field_size_arg), short_field_vector(short_field_vector_arg), int_field_vector(int_field_vector_arg), float_field_vector(float_field_vector_arg), double_field_vector(double_field_vector_arg)
{
  error = "";
}

int rwx_vector_collection_nc::get_vector(const string &name, const vector<char> **field) const
{
  *field = 0;
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_CHAR)
    {
      return -1;
    }

  *field = &char_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection_nc::get_vector(const string &name, const vector<short> **field) const
{
  *field = 0;
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_SHORT)
    {
      return -1;
    }

  *field = &short_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection_nc::get_vector(const string &name, const vector<int> **field) const
{
  *field = 0;
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_INT)
    {
      return -1;
    }

  *field = &int_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection_nc::get_vector(const string &name, const vector<float> **field) const
{
  *field = 0;
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_FLOAT)
    {
      return -1;
    }

  *field = &float_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection_nc::get_vector(const string &name, const vector<double> **field) const
{
  *field = 0;

  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_DOUBLE)
    {
      return -1;
    }

  *field = &double_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection_nc::get_value(const string &name, size_t index, rwx::value_t &value) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end())
    {
      return -1;
    }

  switch (itr->second.first)
    {
    case NC_SHORT:
      value.v_short = short_field_vector[itr->second.second][index];
      value.v_type = NC_SHORT;
      break;
    case NC_INT:
      value.v_int = int_field_vector[itr->second.second][index];
      value.v_type = NC_INT;
      break;
    case NC_FLOAT:
      value.v_float = float_field_vector[itr->second.second][index];
      value.v_type = NC_FLOAT;
      break;

    case NC_DOUBLE:
      value.v_double = double_field_vector[itr->second.second][index];
      value.v_type = NC_DOUBLE;
      break;

    case NC_CHAR:
      size_t string_size = char_field_size[itr->second.second];
      auto begin = char_field_vector[itr->second.second].begin() + index * string_size;
      auto end = begin + string_size;
      value.v_string = string(begin, end);
      std::size_t pos = value.v_string.find('\0');
      if (pos != std::string::npos)
	{
	  value.v_string = value.v_string.substr(0, pos);
	}
      value.v_type = NC_CHAR;
      break;
    }

  return 0;
}

int rwx_vector_collection_nc::get_value(const string &name, size_t index, string &value) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end())
    {
      return -1;
    }

  value = "";
  switch (itr->second.first)
    {
    case NC_SHORT:
      value = lexical_cast<string>(short_field_vector[itr->second.second][index]);
      break;
    case NC_INT:
      value = lexical_cast<string>(int_field_vector[itr->second.second][index]);
      break;
    case NC_FLOAT:
      value = lexical_cast<string>(float_field_vector[itr->second.second][index]);
      break;

    case NC_DOUBLE:
      value = lexical_cast<string>(double_field_vector[itr->second.second][index]);
      break;

    case NC_CHAR:
      size_t string_size = char_field_size[itr->second.second];
      auto begin = char_field_vector[itr->second.second].begin() + index * string_size;
      auto end = begin + string_size;
      value = string(begin, end);
      value.erase(std::remove (value.begin(), value.end(), 0), value.end()); 
      break;
    }

  return 0;
}

int rwx_vector_collection_nc::get_char_field_size(const string &name, size_t *string_size) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end())
    {
      return -1;
    }

  *string_size = char_field_size[itr->second.second];
  return 0;
}

void rwx_vector_collection_nc::extract_indices(const vector<size_t> &latest_id_index, vector< vector<char> > &char_field_vector_arg, vector< vector<short> > &short_field_vector_arg, vector< vector<int> > &int_field_vector_arg, vector<vector<float> > &float_field_vector_arg, vector<vector<double> > &double_field_vector_arg)
{
  char_field_vector_arg.resize(char_field_vector.size());
  short_field_vector_arg.resize(short_field_vector.size());
  int_field_vector_arg.resize(int_field_vector.size());
  float_field_vector_arg.resize(float_field_vector.size());
  double_field_vector_arg.resize(double_field_vector.size());

  for (size_t i=0; i<latest_id_index.size(); i++)
    {
      size_t index = latest_id_index[i];
      for (size_t j=0; j<char_field_vector.size(); j++)
	{
	  for (size_t k=0; k<char_field_size[j]; k++)
	    {
	      char_field_vector_arg[j].push_back(char_field_vector[j][index*char_field_size[j] + k]);
	    }
	}

      for (size_t j=0; j<short_field_vector.size(); j++)
	{
	  short_field_vector_arg[j].push_back(short_field_vector[j][index]);
	}

      for (size_t j=0; j<int_field_vector.size(); j++)
	{
	  int_field_vector_arg[j].push_back(int_field_vector[j][index]);
	}

      for (size_t j=0; j<float_field_vector.size(); j++)
	{
	  float_field_vector_arg[j].push_back(float_field_vector[j][index]);
	}

      for (size_t j=0; j<double_field_vector.size(); j++)
	{
	  double_field_vector_arg[j].push_back(double_field_vector[j][index]);
	}
    }
}


int rwx_vector_collection_nc::get_missing(const string &name, rwx::value_t &value) const
{
  auto itr = missing_map.find(name);
  if (itr == missing_map.end())
    {
      return -1;
    }

  value = itr->second;
  return 0;
}

/** @brief get units for field name */
int rwx_vector_collection_nc::get_units(const string &name, string &units) const
{
  auto itr = units_map.find(name);
  if (itr == units_map.end())
    {
      return -1;
    }

  units = itr->second;
  return 0;
}


rwx_vector_collection_nc::~rwx_vector_collection_nc()
{
}
				     
