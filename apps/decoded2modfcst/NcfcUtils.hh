/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2007
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
//   Module: NcfcUtils.hh
//
//   Author: Seth Linden
//
//   Date:   11/5/08
//
//   Description: 
//----------------------------------------------------------------------

#ifndef NCFCUTILS_HH
#define NCFCUTILS_HH

#include <netcdf.hh>
#include <string>
#include <vector>
#include <log/log.hh>

#include "Params.hh"

using namespace std;

struct ncfc_io_struct
{
  // Base ncfc vectors
  vector<string> var_names;
  vector<void *> var_ptrs;
  vector<void *> var_fill_ptrs;
  vector<nc_type> var_types;
  vector<int> var_ndims;
  vector<size_t *> var_dims;
  vector<long> var_sizes;
  vector<string> var_units;

  // Additional variables
  int is_valid;
  
};
typedef struct ncfc_io_struct ncfc_io;


struct out_var_info_struct
{
  char* var_name;
  char* var_type;
  long var_size;
  long num_days;
  long num_ftpd;
  long num_params;
};
typedef struct out_var_info_struct out_var_info;


class NcfcUtils
{

public:
  
  NcfcUtils(Log *L);
  ~NcfcUtils();
  
  int get_var_list(char *in_file, vector<string>* in_var_names, vector<nc_type>* in_var_types);
  int read_ncfile(char *in_file, vector<string> in_var_names, ncfc_io *in_data);
  int alloc_ncfc_io(vector <out_var_info *> all_out_vars, ncfc_io *out_data);
  int fill_ncfc_io(vector <out_var_info *> all_out_vars, ncfc_io *out_data);
  int copy_input2output(ncfc_io *in_data, ncfc_io *out_data);
  int write_ncfile(char *cdl_file, char *out_file, ncfc_io *out_data);
  int get_var_index(ncfc_io *data, string var_name);
  void print_ncfc_data(ncfc_io *data);
  void clean_ncfc_io(ncfc_io *data);
  
  bool error() {return (errorString != "");};
  //return 1 = error, 0 = no error
  
  string getErrorString() {return (errorString);};
  //return string containing error message
  
private:

  Log *log;
  string errorString;
  
};

#endif /* NCFCUTILS_HH */

