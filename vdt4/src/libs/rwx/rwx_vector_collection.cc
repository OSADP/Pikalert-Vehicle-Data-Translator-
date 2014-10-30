//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_vector_collection.cc,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2013/10/28 20:12:39 $
//
//==============================================================================

/**
 * @file vector_collection.cc
 *
 * @date 8/1/13
 */

// Include files 
#include <boost/lexical_cast.hpp>
#include <netcdf>
#include "rwx_vector_collection.hh"

using boost::lexical_cast;

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

rwx_vector_collection::rwx_vector_collection(const unordered_map<string, std::pair<int, int> > &name_type_index_map_arg, const vector< vector<char> > &char_field_vector_arg, const vector<size_t> &char_field_size_arg, const vector< vector<short> > &short_field_vector_arg, const vector< vector<int> > &int_field_vector_arg, const vector<vector<float> > &float_field_vector_arg, const vector<vector<double> > &double_field_vector_arg) : name_type_index_map(name_type_index_map_arg), char_field_vector(char_field_vector_arg), char_field_size(char_field_size_arg), short_field_vector(short_field_vector_arg), int_field_vector(int_field_vector_arg), float_field_vector(float_field_vector_arg), double_field_vector(double_field_vector_arg)
{
  error = "";

  size = 0;
  if (short_field_vector.size() != 0)
    {
      size = short_field_vector[0].size();
    }
  else if (int_field_vector.size() != 0)
    {
      size = int_field_vector[0].size();
    }
  else if (float_field_vector.size() != 0)
    {
      size = float_field_vector[0].size();
    }
  else if (double_field_vector.size() != 0)
    {
      size = double_field_vector[0].size();
    }

#ifdef NOTNOW
  for (size_t i=0; i<char_field_vector.size(); i++)
    {
      if (char_field_vector[i].size() != size)
	{
	  error = string("inconsistent char field vector sizes");
	  return;
	}
    }
#endif
  for (size_t i=0; i<short_field_vector.size(); i++)
    {
      if (short_field_vector[i].size() != size)
	{
	  error = string("inconsistent short field vector sizes: ") + lexical_cast<string>(short_field_vector[i].size()) + " " + lexical_cast<string>(size);

	  return;
	}
    }

  for (size_t i=0; i<int_field_vector.size(); i++)
    {
      if (int_field_vector[i].size() != size)
	{
	  error = string("inconsistent int field vector sizes");
	  return;
	}
    }

  for (size_t i=0; i<float_field_vector.size(); i++)
    {
      if (float_field_vector[i].size() != size)
	{
	  error = string("inconsistent float field vector sizes");
	  return;
	}
    }

  for (size_t i=0; i<double_field_vector.size(); i++)
    {
      if (double_field_vector[i].size() != size)
	{
	  error = string("inconsistent double field vector sizes");
	  return;
	}
    }
}

int rwx_vector_collection::get_vector(const string &name, const vector<char> **field) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_CHAR)
    {
      return -1;
    }

  *field = &char_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection::get_vector(const string &name, const vector<short> **field) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_SHORT)
    {
      return -1;
    }

  *field = &short_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection::get_vector(const string &name, const vector<int> **field) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_INT)
    {
      return -1;
    }

  *field = &int_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection::get_vector(const string &name, const vector<float> **field) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_FLOAT)
    {
      return -1;
    }

  *field = &float_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection::get_vector(const string &name, const vector<double> **field) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end() || itr->second.first != NC_DOUBLE)
    {
      return -1;
    }

  *field = &double_field_vector[itr->second.second];
  return 0;
}  

int rwx_vector_collection::get_value(const string &name, size_t index, rwx::value_t &value) const
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

int rwx_vector_collection::get_value(const string &name, size_t index, string &value) const
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

int rwx_vector_collection::get_char_field_size(const string &name, size_t *string_size) const
{
  auto itr = name_type_index_map.find(name);
  if (itr == name_type_index_map.end())
    {
      return -1;
    }

  *string_size = char_field_size[itr->second.second];
  return 0;
}

void rwx_vector_collection::extract_indices(const vector<size_t> &latest_id_index, vector< vector<char> > &char_field_vector_arg, vector< vector<short> > &short_field_vector_arg, vector< vector<int> > &int_field_vector_arg, vector<vector<float> > &float_field_vector_arg, vector<vector<double> > &double_field_vector_arg)
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

rwx_vector_collection::~rwx_vector_collection()
{
}
				     
