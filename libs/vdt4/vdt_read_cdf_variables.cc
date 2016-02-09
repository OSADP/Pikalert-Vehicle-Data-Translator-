//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file vdt_read_variables.cc
 *
 * <file description>
 *
 * @date 3/27/15
 */

// Include files 
#include "vdt_read_cdf_variables.hh"


// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int read_int_var(netCDF::NcFile &dataFile, const string &var_string, vector<int> &obs, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = dataFile.getVar(var_string);
  if (var.isNull())
    {
      error = string("could not find variable: ") + var_string;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  obs.resize(dims[0].getSize());
  var.getVar(&obs[0]);

  return 0;
}

int read_string_var(netCDF::NcFile &dataFile, const string &var_string, vector<string> &obs, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = dataFile.getVar(var_string);
  if (var.isNull())
    {
      error = string("could not find variable: ") + var_string;
      return -1;
    }

  vector<netCDF::NcDim> dims = var.getDims();

  vector<char> char_vector;
  char_vector.resize(dims[0].getSize() * dims[1].getSize());
  var.getVar(&char_vector[0]);
  for (unsigned int i=0; i<char_vector.size(); i+=dims[1].getSize())
    {
      string str(&char_vector[i], &char_vector[i+dims[1].getSize()]);
      obs.push_back(str.c_str());
    }

  return 0;
}

int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, int &missing_value, vector<int> &obs, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = dataFile.getVar(var_string);
  if (var.isNull())
    {
      error = string("could not find variable: ") + var_string;
      return -1;
    }

  // Get the units and scale factor attributes
  netCDF::NcVarAtt att;

  if (units_name != "")
    {
      att = var.getAtt(units_name);
      if (att.isNull())
	{
	  error = string("could not get variable units for: ") + var_string;
	  return -1;
	}

      att.getValues(units);
    }

  if (missing_value_string != "")
    {
      att = var.getAtt(missing_value_string);
      if (att.isNull())
	{
	  error = string("could not get missing value for variable: ") + var_string;
	  return -1;
	}

      att.getValues(&missing_value);
    }
   
  vector<netCDF::NcDim> dims = var.getDims();

  obs.resize(dims[0].getSize());
  var.getVar(&obs[0]);

  return 0;
}

int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, float &missing_value, vector<float> &obs, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = dataFile.getVar(var_string);
  if (var.isNull())
    {
      error = string("could not find variable: ") + var_string;
      return -1;
    }

  // Get the units and scale factor attributes
  netCDF::NcVarAtt att;

  if (units_name != "")
    {
      att = var.getAtt(units_name);
      if (att.isNull())
	{
	  error = string("could not get variable units for: ") + var_string;
	  return -1;
	}

      att.getValues(units);
    }

  if (missing_value_string != "")
    {
      att = var.getAtt(missing_value_string);
      if (att.isNull())
	{
	  error = string("could not get missing value for variable: ") + var_string;
	  return -1;
	}

      att.getValues(&missing_value);
    }
   
  vector<netCDF::NcDim> dims = var.getDims();

  obs.resize(dims[0].getSize());
  var.getVar(&obs[0]);

  return 0;
}

int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, double &missing_value, vector<double> &obs, string &error)
{
  // Get the variable
  netCDF::NcVar var;
  var = dataFile.getVar(var_string);
  if (var.isNull())
    {
      error = string("could not find variable: ") + var_string;
      return -1;
    }

  // Get the units and scale factor attributes
  netCDF::NcVarAtt att;

  if (units_name != "")
    {
      att = var.getAtt(units_name);
      if (att.isNull())
	{
	  error = string("could not get variable units for: ") + var_string;
	  return -1;
	}

      att.getValues(units);
    }

  if (missing_value_string != "")
    {
      att = var.getAtt(missing_value_string);
      if (att.isNull())
	{
	  error = string("could not get missing value for variable: ") + var_string;
	  return -1;
	}

      att.getValues(&missing_value);
    }
   
  vector<netCDF::NcDim> dims = var.getDims();

  obs.resize(dims[0].getSize());
  var.getVar(&obs[0]);

  return 0;
}

