//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file vdt_read_cdf_variables.hh
 *
 * @class Vdt_Read_Cdf_Variables
 *
 *  <file/class description>
 *  
 * @date 3/27/15
 */

#ifndef VDT_READ_CDF_VARIABLES_HH
#define VDT_READ_CDF_VARIABLES_HH

#include <netcdf>
#include <string>
#include <vector>

using std::string;
using std::vector;

int read_string_var(netCDF::NcFile &dataFile, const string &var_string, vector<string> &obs, string &error);
int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, int &missing_value, vector<int> &obs, string &error);
int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, short &missing_value, vector<short> &obs, string &error);
int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, float &missing_value, vector<float> &obs, string &error);
int read_variable(netCDF::NcFile &dataFile, const string &var_string, const string &units_name, const string &missing_value_string, string &units, double &missing_value, vector<double> &obs, string &error);




#endif /* VDT_READ_CDF_VARIABLES_HH */
