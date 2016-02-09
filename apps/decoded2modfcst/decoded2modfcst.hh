/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2011
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
//   Module: decoded2modfcst.hh
//
//   Author: Seth Linden
//
//   Date:   04/13/11
//
//   Description: 
//----------------------------------------------------------------------

#ifndef DECODED2MODFCST_HH
#define DECODED2MODFCST_HH

#include <log/log.hh>

#include "Params.hh"
#include "ProcArgs.hh"
#include "NcfcUtils.hh"
#include "StatUtils.hh"

using namespace std;


struct qc_struct
{
  string var_name;
  float lower_bound;
  float upper_bound;
  float max_abs_error;
};
typedef struct qc_struct qc;


struct output_info_struct
{
  vector<out_var_info *> out_vars;
  char *cdl_file;
  char *file_name;
};
typedef struct output_info_struct output_info;


class decoded2modfcst
{
  
public:

  // constructor
  decoded2modfcst(int argc, char **argv);

  // destructor
  ~decoded2modfcst();
  
  // run 
  int run(char **argv);
  
  // data members
  //int okay;
  
protected:
  
private:
  
  ProcArgs *args;
  int num_args;
  Params *params;
  Log *log;
  
  char *input_file;
  char *site_list;
  char *cdl_file;
  char *output_file;
  
  NcfcUtils *nc_utils;
  StatUtils *stat_utils;
  
  int processData();
  int copyData(ncfc_io *in_data, ncfc_io *out_data,  vector<site *> sites, vector <out_var_info *> out_vars);

};

#endif /* DECODED2MODFCST_HH */
