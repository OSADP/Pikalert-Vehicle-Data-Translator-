//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_cdf_field_reader.cc,v $
//       Version: $Revision: 1.10 $  Dated: $Date: 2013/10/23 19:07:18 $
//
//==============================================================================

/**
 *
 * @file rwx_cdf_field_reader.cc
 *
 * Read raw wind farm observations from a netcdf file
 *
 */

// Include files 
#include <iostream>
#include <math.h>
#include <sstream>
#include <map>
#include "rwx_cdf_field_reader.hh"

using std::multimap;

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions

using namespace rwx;


const multimap< string, netCDF::NcDim >  &rwx_cdf_field_reader::get_dimension_map()
{
 return dim_map_;
}

const multimap< string, netCDF::NcVar >  &rwx_cdf_field_reader::get_var_map()
{
  return var_map_;
}

int rwx_cdf_field_reader::get_field(const string &field_name, vector<char> &field, size_t *char_field_size, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file_->getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  vector<char> charVector;
  size_t main_dim = dims[0].getSize();
  if (main_dim != 0)
    {
      field.resize(main_dim * dims[1].getSize());
      var.getVar(&field[0]);
    }

  *char_field_size = dims[1].getSize();

  return 0;
}

int rwx_cdf_field_reader::get_fields(const vector<string> &field_names, vector< vector<char> > &field_vector, vector<size_t> &char_field_size, string &error)
{
  string error_occurred;
  int error_return = 0;
  size_t field_size;

  field_vector.resize(field_names.size());
  char_field_size.resize(field_names.size());

  for (size_t i=0; i<field_names.size(); i++)
    {
      error_return = get_field(field_names[i], field_vector[i], &field_size, error_occurred);
      char_field_size[i] = field_size;
      if (error_return != 0)
	{
	  error = error_occurred;
	}
    }

  return error_return;
}

int rwx_cdf_field_reader::get_field(const string &field_name, vector<short> &field, string &units, short *missing, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file_->getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  size_t main_dim = dims[0].getSize();
  if (main_dim != 0)
    {
      field.resize(main_dim);
      var.getVar(&field[0]);
    }

  // Get units if available
  netCDF::NcVarAtt att;

  try
    {
      att = var.getAtt(units_name_);
      att.getValues(units);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  try
    {
      att = var.getAtt(missing_name_);
      att.getValues(missing);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  return 0;
}

int rwx_cdf_field_reader::get_fields(const vector<string> &field_names, vector< vector<short> > &field_vector, vector<string> &units, vector<short> &missing, string &error)
{
  string error_occurred;
  int error_return = 0;

  field_vector.resize(field_names.size());
  units.resize(field_names.size());
  missing.resize(field_names.size());

  for (size_t i=0; i<field_names.size(); i++)
    {
      short missing_value;
      string units_string;
      error_return = get_field(field_names[i], field_vector[i], units_string, &missing_value, error_occurred);
      missing[i] = missing_value;
      units[i] = units_string;
      if (error_return != 0)
	{
	  error = error_occurred;
	}
    }

  return error_return;
}

int rwx_cdf_field_reader::get_field(const string &field_name, vector<int> &field, string &units, int *missing, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file_->getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  size_t main_dim = dims[0].getSize();
  if (main_dim != 0)
    {
      field.resize(main_dim);
      var.getVar(&field[0]);
    }

  // Get units if available
  netCDF::NcVarAtt att;

  try
    {
      att = var.getAtt(units_name_);
      att.getValues(units);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  try
    {
      att = var.getAtt(missing_name_);
      att.getValues(missing);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  return 0;
}

int rwx_cdf_field_reader::get_fields(const vector<string> &field_names, vector< vector<int> > &field_vector, vector<string> &units, vector<int> &missing, string &error)
{
  string error_occurred;
  int error_return = 0;

  field_vector.resize(field_names.size());
  units.resize(field_names.size());
  missing.resize(field_names.size());

  for (size_t i=0; i<field_names.size(); i++)
    {
      int missing_value;
      string units_string;
      error_return = get_field(field_names[i], field_vector[i], units_string, &missing_value, error_occurred);
      missing[i] = missing_value;
      units[i] = units_string;
      if (error_return != 0)
	{
	  error = error_occurred;
	}
    }

  return error_return;
}

int rwx_cdf_field_reader::get_field(const string &field_name, vector<float> &field, string &units, float *missing, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file_->getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  size_t main_dim = dims[0].getSize();
  if (main_dim != 0)
    {
      field.resize(main_dim);
      var.getVar(&field[0]);
    }

  // Get units if available
  netCDF::NcVarAtt att;

  try
    {
      att = var.getAtt(units_name_);
      att.getValues(units);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  try
    {
      att = var.getAtt(missing_name_);
      att.getValues(missing);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  return 0;
}

int rwx_cdf_field_reader::get_fields(const vector<string> &field_names, vector< vector<float> > &field_vector, vector<string> &units, vector<float> &missing, string &error)
{
  string error_occurred;
  int error_return = 0;

  field_vector.resize(field_names.size());
  units.resize(field_names.size());
  missing.resize(field_names.size());

  for (size_t i=0; i<field_names.size(); i++)
    {
      float missing_value;
      string units_string;
      error_return = get_field(field_names[i], field_vector[i], units_string, &missing_value, error_occurred);
      missing[i] = missing_value;
      units[i] = units_string;
      if (error_return != 0)
	{
	  error = error_occurred;
	}
    }

  return error_return;
}

int rwx_cdf_field_reader::get_field_attr(const string &field_name, const string &attr_name, vector<short> &attr_values)
{
  // Get units if available
  netCDF::NcVarAtt att;

  auto itr = var_map_.find(field_name);
  if (itr == var_map_.end())
    {
      return -1;
    }

  netCDF::NcVar var = itr->second;

  try
    {
      att = var.getAtt(attr_name);
      att.getValues(&attr_values[0]);
    }
  catch (netCDF::exceptions::NcException e)
    {
      return -1;
    }

  return 0;
}

int rwx_cdf_field_reader::get_field_attr(const string &field_name, const string &attr_name, vector<int> &attr_values)
{
  // Get units if available
  netCDF::NcVarAtt att;

  auto itr = var_map_.find(field_name);
  if (itr == var_map_.end())
    {
      return -1;
    }

  netCDF::NcVar var = itr->second;

  try
    {
      att = var.getAtt(attr_name);
      att.getValues(&attr_values[0]);
    }
  catch (netCDF::exceptions::NcException e)
    {
      return -1;
    }

  return 0;
}

int rwx_cdf_field_reader::get_field_attr(const string &field_name, const string &attr_name, vector<float> &attr_values)
{
  // Get units if available
  netCDF::NcVarAtt att;

  auto itr = var_map_.find(field_name);
  if (itr == var_map_.end())
    {
      return -1;
    }

  netCDF::NcVar var = itr->second;

  try
    {
      att = var.getAtt(attr_name);
      att.getValues(&attr_values[0]);
    }
  catch (netCDF::exceptions::NcException e)
    {
      return -1;
    }

  return 0;
}

int rwx_cdf_field_reader::get_field_attr(const string &field_name, const string &attr_name, vector<double> &attr_values)
{
  // Get units if available
  netCDF::NcVarAtt att;

  auto itr = var_map_.find(field_name);
  if (itr == var_map_.end())
    {
      return -1;
    }

  netCDF::NcVar var = itr->second;

  try
    {
      att = var.getAtt(attr_name);
      att.getValues(&attr_values[0]);
    }
  catch (netCDF::exceptions::NcException e)
    {
      return -1;
    }

  return 0;
}

int rwx_cdf_field_reader::get_field_attr(const string &field_name, const string &attr_name, string &attr_values)
{
  // Get units if available
  netCDF::NcVarAtt att;

  auto itr = var_map_.find(field_name);
  if (itr == var_map_.end())
    {
      return -1;
    }

  netCDF::NcVar var = itr->second;

  try
    {
      att = var.getAtt(attr_name);
      att.getValues(&attr_values[0]);
    }
  catch (netCDF::exceptions::NcException e)
    {
      return -1;
    }

  return 0;
}
int rwx_cdf_field_reader::get_field(const string &field_name, vector<double> &field, string &units, double *missing, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = data_file_->getVar(field_name);
  if (var.isNull())
    {
      error = string("could not find variable: ") + field_name;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();
  size_t main_dim = dims[0].getSize();
  if (main_dim != 0)
    {
      field.resize(main_dim);
      var.getVar(&field[0]);
    }

  // Get units if available
  netCDF::NcVarAtt att;

  try
    {
      att = var.getAtt(units_name_);
      att.getValues(units);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  try
    {
      att = var.getAtt(missing_name_);
      att.getValues(missing);
    }
  catch (netCDF::exceptions::NcException e)
    {
    }

  return 0;
}

int rwx_cdf_field_reader::get_fields(const vector<string> &field_names, vector< vector<double> > &field_vector, vector<string> &units, vector<double> &missing, string &error)
{
  string error_occurred;
  int error_return = 0;

  field_vector.resize(field_names.size());
  units.resize(field_names.size());
  missing.resize(field_names.size());

  for (size_t i=0; i<field_names.size(); i++)
    {
      double missing_value;
      string units_string;
      error_return = get_field(field_names[i], field_vector[i], units_string, &missing_value, error_occurred);
      missing[i] = missing_value;
      units[i] = units_string;
      if (error_return != 0)
	{
	  error = error_occurred;
	}
    }

  return error_return;
}


rwx_cdf_field_reader::rwx_cdf_field_reader(const string &fileName)
{
  error_ = "";
  units_name_ = "units";
  missing_name_ = "_FillValue";
  data_file_ = 0;

  try
    {
      // Open the file and check to make sure it's valid.
      data_file_ = new netCDF::NcFile(fileName, netCDF::NcFile::read);
      dim_map_ = data_file_->getDims();
      var_map_ = data_file_->getVars();
    }
  catch (netCDF::exceptions::NcException e)
    {
      // Capture exception in error_ by redirecting cout
      error_ = e.what();
    }

  return;
}


rwx_cdf_field_reader::~rwx_cdf_field_reader()
{
  delete data_file_;
}
