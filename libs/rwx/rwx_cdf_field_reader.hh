//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: rwx_cdf_field_reader.hh,v $
//       Version: $Revision: 1.10 $  Dated: $Date: 2013/10/23 19:07:18 $
//
//==============================================================================

/**
 *
 * @file rwx_cdf_field_reader.hh
 *
 * @class rwx_cdf_field_reader
 *
 */

#ifndef RWX_CDF_FIELD_READER_HH
#define RWX_CDF_FIELD_READER_HH

#include <netcdf>
#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::unordered_map;
using std::vector;

namespace rwx
{
  /**
   * @class rwx_cdf_field_reader
   */
  class rwx_cdf_field_reader
  {
  public:
    /** Constructor */
    rwx_cdf_field_reader(const string &fileName);
    //rwx_cdf_field_reader(const vector <string> &fileList);
    ~rwx_cdf_field_reader();

    string error() { return error_;}

    const std::multimap<string, netCDF::NcDim> &get_dimension_map();
    const std::multimap<string, netCDF::NcVar> &get_var_map();

    int get_field(const string &field_name, vector<char> &field, size_t *char_field_size, string &error);
    int get_fields(const vector<string> &field_names, vector< vector<char> > &field_vector, vector<size_t> &char_field_size, string &error);

    int get_field(const string &field_name, vector<short> &field, string &units, short *missing, string &error);
    int get_fields(const vector<string> &field_names, vector< vector<short> > &field_vector, vector<string> &units, vector<short> &missing, string &error);
    int get_field(const string &field_name, vector<int> &field, string &units, int *missing, string &error);
    int get_fields(const vector<string> &field_names, vector< vector<int> > &field_vector, vector<string> &units, vector<int> &missing, string &error);
    int get_field(const string &field_name, vector<float> &field, string &units, float *missing, string &error);
    int get_fields(const vector<string> &field_names, vector< vector<float> > &field_vector, vector<string> &units, vector<float> &missing, string &error);
    int get_field(const string &field_name, vector<double> &field, string &units, double *missing, string &error);
    int get_fields(const vector<string> &field_names, vector< vector<double> > &field_vector, vector<string> &units, vector<double> &missing, string &error);

    int get_field_attr(const string &field_name, const string &attr_name, string &attr_values);
    int get_field_attr(const string &field_name, const string &attr_name, vector<short> &attr_values);
    int get_field_attr(const string &field_name, const string &attr_name, vector<int> &attr_values);
    int get_field_attr(const string &field_name, const string &attr_name, vector<float> &attr_values);
    int get_field_attr(const string &field_name, const string &attr_name, vector<double> &attr_values);

    inline void set_units_name(const string &units_string) {units_name_ = units_string;}
    inline void set_missing(const string &missing_string) {missing_name_ = missing_string;}



  private:
    string error_;
    string units_name_;
    string missing_name_;
    netCDF::NcFile *data_file_;
    std::multimap<string, netCDF::NcDim> dim_map_;
    std::multimap<string, netCDF::NcVar> var_map_;

    unordered_map<string, std::pair<int, int> > name_type_index_map_;
    vector< vector<char> > char_field_vector_;
    vector<size_t> char_field_size_;
    vector< vector<short> > short_field_vector_;
    vector< vector<int> > int_field_vector_;
    vector<vector<float> > float_field_vector_;
    vector<vector<double> > double_field_vector_;

    rwx_cdf_field_reader & operator=(const rwx_cdf_field_reader &field_reader);
    rwx_cdf_field_reader(const rwx_cdf_field_reader &field_reader);
  };

};

#endif /* RWX_CDF_FIELD_READER_HH */
