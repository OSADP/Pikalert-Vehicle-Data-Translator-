// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2015
// ** University Corporation for Atmospheric Research(UCAR)
// ** National Center for Atmospheric Research(NCAR)
// ** Research Applications Laboratory(RAL)
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
// ** See LICENCE.TXT if applicable for licence details
// ** 2015/00/12 23:54:47 UTC
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
////////////////////////////////////////////
// Params.cc
//
// TDRP C++ code file for class 'Params'.
//
// Code for program post_proc
//
// This file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Params.cc
 *
 * @class Params
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @note Source is automatically generated from
 *       paramdef file at compile time, do not modify
 *       since modifications will be overwritten.
 *
 *
 * @author Automatically generated
 *
 */
using namespace std;

#include "Params.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  Params::Params()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  Params::Params(const Params& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  Params::~Params()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void Params::operator=(const Params& other)

  {

    // sync the other object

    other.sync();

    // free up any existing memory

    freeAll();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // copy table

    tdrpCopyTable((TDRPtable *) other._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = other._exitDeferred;

  }

  ////////////////////////////////////////////
  // loadFromArgs()
  //
  // Loads up TDRP using the command line args.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   char **params_path_p:
  //     If this is non-NULL, it is set to point to the path
  //     of the params file used.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadFromArgs(int argc, char **argv,
                           char **override_list,
                           char **params_path_p,
                           bool defer_exit)
  {
    int exit_deferred;
    if (_tdrpLoadFromArgs(argc, argv,
                          _table, &_start_,
                          override_list, params_path_p,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadApplyArgs()
  //
  // Loads up TDRP using the params path passed in, and applies
  // the command line args for printing and checking.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   const char *param_file_path: the parameter file to be read in
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadApplyArgs(const char *params_path,
                            int argc, char **argv,
                            char **override_list,
                            bool defer_exit)
  {
    int exit_deferred;
    if (tdrpLoadApplyArgs(params_path, argc, argv,
                          _table, &_start_,
                          override_list,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  bool Params::isArgValid(const char *arg)
  {
    return (tdrpIsArgValid(arg));
  }

  ////////////////////////////////////////////
  // load()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to load
  // up more than one class for a single application. It is a
  // lower-level routine than loadFromArgs, and hence more
  // flexible, but the programmer must do more work.
  //
  //   const char *param_file_path: the parameter file to be read in.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::load(const char *param_file_path,
                   char **override_list,
                   int expand_env, int debug)
  {
    if (tdrpLoad(param_file_path,
                 _table, &_start_,
                 override_list,
                 expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadFromBuf()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to
  // load up more than one module for a single application,
  // using buffers which have been read from a specified source.
  //
  //   const char *param_source_str: a string which describes the
  //     source of the parameter information. It is used for
  //     error reporting only.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   const char *inbuf: the input buffer
  //
  //   int inlen: length of the input buffer
  //
  //   int start_line_num: the line number in the source which
  //     corresponds to the start of the buffer.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadFromBuf(const char *param_source_str,
                          char **override_list,
                          const char *inbuf, int inlen,
                          int start_line_num,
                          int expand_env, int debug)
  {
    if (tdrpLoadFromBuf(param_source_str,
                        _table, &_start_,
                        override_list,
                        inbuf, inlen, start_line_num,
                        expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadDefaults(int expand_env)
  {
    if (tdrpLoad(NULL,
                 _table, &_start_,
                 NULL, expand_env, FALSE)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void Params::sync(void) const
  {
    tdrpUser2Table(_table, (char *) &_start_);
  }

  ////////////////////////////////////////////
  // print()
  // 
  // Print params file
  //
  // The modes supported are:
  //
  //   PRINT_SHORT:   main comments only, no help or descriptions
  //                  structs and arrays on a single line
  //   PRINT_NORM:    short + descriptions and help
  //   PRINT_LONG:    norm  + arrays and structs expanded
  //   PRINT_VERBOSE: long  + private params included
  //

  void Params::print(FILE *out, tdrp_print_mode_t mode)
  {
    tdrpPrint(out, _table, _className, mode);
  }

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int Params::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int Params::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void Params::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void Params::usage(ostream &out)
  {
    out << "TDRP args: [options as below]\n"
        << "   [ -params/--params path ] specify params file path\n"
        << "   [ -check_params] check which params are not set\n"
        << "   [ -print_params [mode]] print parameters\n"
        << "     using following modes, default mode is 'norm'\n"
        << "       short:   main comments only, no help or descr\n"
        << "                structs and arrays on a single line\n"
        << "       norm:    short + descriptions and help\n"
        << "       long:    norm  + arrays and structs expanded\n"
        << "       verbose: long  + private params included\n"
        << "       short_expand:   short with env vars expanded\n"
        << "       norm_expand:    norm with env vars expanded\n"
        << "       long_expand:    long with env vars expanded\n"
        << "       verbose_expand: verbose with env vars expanded\n"
        << "   [ -tdrp_debug] debugging prints for tdrp\n"
        << "   [ -tdrp_usage] print this usage\n";
  }

  ////////////////////////////////////////////
  // arrayRealloc()
  //
  // Realloc 1D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Params::arrayRealloc(const char *param_name, int new_array_n)
  {
    if (tdrpArrayRealloc(_table, &_start_,
                         param_name, new_array_n)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // array2DRealloc()
  //
  // Realloc 2D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Params::array2DRealloc(const char *param_name,
                             int new_array_n1,
                             int new_array_n2)
  {
    if (tdrpArray2DRealloc(_table, &_start_, param_name,
                           new_array_n1, new_array_n2)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // _init()
  //
  // Class table initialization function.
  //
  //

  void Params::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'input_file_type'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("input_file_type");
    tt->descr = tdrpStrDup("Input file type");
    tt->help = tdrpStrDup("Either wx (dicast weather file), rc (rctm road file), or bc (rctm bridge file)");
    tt->val_offset = (char *) &input_file_type - &_start_;
    tt->single_val.s = tdrpStrDup("wx");
    tt++;
    
    // Parameter 'fec_vars'
    // ctype is '_fec_vars_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("fec_vars");
    tt->descr = tdrpStrDup("Array of structs containing the forecast variable names, the fec technique and the minimum and maximum number of correction hours allowed.");
    tt->help = tdrpStrDup("Only variables listed here will be fec'd. If the minimum is 6 hours the correction factors will be: 1.00, 0.83, 0.67, 0.50, 0.33, 0.17");
    tt->array_offset = (char *) &_fec_vars - &_start_;
    tt->array_n_offset = (char *) &fec_vars_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(fec_vars_t);
    tt->array_n = 15;
    tt->struct_def.name = tdrpStrDup("fec_vars_t");
    tt->struct_def.nfields = 4;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("string");
      tt->struct_def.fields[0].fname = tdrpStrDup("var_name");
      tt->struct_def.fields[0].ptype = STRING_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &_fec_vars->var_name - (char *) _fec_vars;
      tt->struct_def.fields[1].ftype = tdrpStrDup("fec_obs_type_t");
      tt->struct_def.fields[1].fname = tdrpStrDup("obs_gen_type");
      tt->struct_def.fields[1].ptype = ENUM_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &_fec_vars->obs_gen_type - (char *) _fec_vars;
        tt->struct_def.fields[1].enum_def.name = tdrpStrDup("fec_obs_type_t");
        tt->struct_def.fields[1].enum_def.nfields = 2;
        tt->struct_def.fields[1].enum_def.fields = (enum_field_t *) tdrpMalloc
          (tt->struct_def.fields[1].enum_def.nfields * sizeof(enum_field_t));
        tt->struct_def.fields[1].enum_def.fields[0].name = tdrpStrDup("POINT_OBS");
        tt->struct_def.fields[1].enum_def.fields[0].val = POINT_OBS;
        tt->struct_def.fields[1].enum_def.fields[1].name = tdrpStrDup("SPATIAL_OBS");
        tt->struct_def.fields[1].enum_def.fields[1].val = SPATIAL_OBS;
      tt->struct_def.fields[2].ftype = tdrpStrDup("int");
      tt->struct_def.fields[2].fname = tdrpStrDup("min_corr_hrs");
      tt->struct_def.fields[2].ptype = INT_TYPE;
      tt->struct_def.fields[2].rel_offset = 
        (char *) &_fec_vars->min_corr_hrs - (char *) _fec_vars;
      tt->struct_def.fields[3].ftype = tdrpStrDup("int");
      tt->struct_def.fields[3].fname = tdrpStrDup("max_corr_hrs");
      tt->struct_def.fields[3].ptype = INT_TYPE;
      tt->struct_def.fields[3].rel_offset = 
        (char *) &_fec_vars->max_corr_hrs - (char *) _fec_vars;
    tt->n_struct_vals = 60;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
      tt->struct_vals[0].s = tdrpStrDup("T");
      tt->struct_vals[1].e = POINT_OBS;
      tt->struct_vals[2].i = 6;
      tt->struct_vals[3].i = 10;
      tt->struct_vals[4].s = tdrpStrDup("dewpt");
      tt->struct_vals[5].e = POINT_OBS;
      tt->struct_vals[6].i = 6;
      tt->struct_vals[7].i = 10;
      tt->struct_vals[8].s = tdrpStrDup("wind_u");
      tt->struct_vals[9].e = POINT_OBS;
      tt->struct_vals[10].i = 6;
      tt->struct_vals[11].i = 10;
      tt->struct_vals[12].s = tdrpStrDup("wind_v");
      tt->struct_vals[13].e = POINT_OBS;
      tt->struct_vals[14].i = 6;
      tt->struct_vals[15].i = 10;
      tt->struct_vals[16].s = tdrpStrDup("wind_speed");
      tt->struct_vals[17].e = SPATIAL_OBS;
      tt->struct_vals[18].i = 6;
      tt->struct_vals[19].i = 10;
      tt->struct_vals[20].s = tdrpStrDup("cloud_cov");
      tt->struct_vals[21].e = SPATIAL_OBS;
      tt->struct_vals[22].i = 6;
      tt->struct_vals[23].i = 10;
      tt->struct_vals[24].s = tdrpStrDup("visibility");
      tt->struct_vals[25].e = POINT_OBS;
      tt->struct_vals[26].i = 6;
      tt->struct_vals[27].i = 10;
      tt->struct_vals[28].s = tdrpStrDup("prob_fog");
      tt->struct_vals[29].e = SPATIAL_OBS;
      tt->struct_vals[30].i = 6;
      tt->struct_vals[31].i = 10;
      tt->struct_vals[32].s = tdrpStrDup("prob_thunder");
      tt->struct_vals[33].e = SPATIAL_OBS;
      tt->struct_vals[34].i = 6;
      tt->struct_vals[35].i = 10;
      tt->struct_vals[36].s = tdrpStrDup("cprob_rain");
      tt->struct_vals[37].e = SPATIAL_OBS;
      tt->struct_vals[38].i = 6;
      tt->struct_vals[39].i = 10;
      tt->struct_vals[40].s = tdrpStrDup("cprob_snow");
      tt->struct_vals[41].e = SPATIAL_OBS;
      tt->struct_vals[42].i = 6;
      tt->struct_vals[43].i = 10;
      tt->struct_vals[44].s = tdrpStrDup("cprob_ice");
      tt->struct_vals[45].e = SPATIAL_OBS;
      tt->struct_vals[46].i = 6;
      tt->struct_vals[47].i = 10;
      tt->struct_vals[48].s = tdrpStrDup("prob_precip01");
      tt->struct_vals[49].e = SPATIAL_OBS;
      tt->struct_vals[50].i = 6;
      tt->struct_vals[51].i = 10;
      tt->struct_vals[52].s = tdrpStrDup("prob_precip03");
      tt->struct_vals[53].e = SPATIAL_OBS;
      tt->struct_vals[54].i = 6;
      tt->struct_vals[55].i = 10;
      tt->struct_vals[56].s = tdrpStrDup("prob_precip06");
      tt->struct_vals[57].e = SPATIAL_OBS;
      tt->struct_vals[58].i = 6;
      tt->struct_vals[59].i = 10;
    tt++;
    
    // Parameter 'other_var_names'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("other_var_names");
    tt->descr = tdrpStrDup("List of additional variables that will be copied to the output file");
    tt->help = tdrpStrDup("These variables will not be fec'd");
    tt->array_offset = (char *) &_other_var_names - &_start_;
    tt->array_n_offset = (char *) &other_var_names_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(char*);
    tt->array_n = 15;
    tt->array_vals = (tdrpVal_t *)
        tdrpMalloc(tt->array_n * sizeof(tdrpVal_t));
      tt->array_vals[0].s = tdrpStrDup("qpf01");
      tt->array_vals[1].s = tdrpStrDup("qpf03");
      tt->array_vals[2].s = tdrpStrDup("qpf06");
      tt->array_vals[3].s = tdrpStrDup("P_sfc");
      tt->array_vals[4].s = tdrpStrDup("P_msl");
      tt->array_vals[5].s = tdrpStrDup("T_bls");
      tt->array_vals[6].s = tdrpStrDup("T_lbls0");
      tt->array_vals[7].s = tdrpStrDup("T_lbls1");
      tt->array_vals[8].s = tdrpStrDup("T_lbls2");
      tt->array_vals[9].s = tdrpStrDup("T_lbls3");
      tt->array_vals[10].s = tdrpStrDup("cloud_low");
      tt->array_vals[11].s = tdrpStrDup("cloud_middle");
      tt->array_vals[12].s = tdrpStrDup("cloud_high");
      tt->array_vals[13].s = tdrpStrDup("dlwrf_sfc");
      tt->array_vals[14].s = tdrpStrDup("dswrf_sfc");
    tt++;
    
    // Parameter 'length_scale'
    // ctype is 'float'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = FLOAT_TYPE;
    tt->param_name = tdrpStrDup("length_scale");
    tt->descr = tdrpStrDup("length scale for averaging observations");
    tt->help = tdrpStrDup("distance at which obs has about 1/2 wgt of co-located obs");
    tt->val_offset = (char *) &length_scale - &_start_;
    tt->single_val.f = 25;
    tt++;
    
    // Parameter 'max_distance'
    // ctype is 'float'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = FLOAT_TYPE;
    tt->param_name = tdrpStrDup("max_distance");
    tt->descr = tdrpStrDup("maximum distance for bos inclusion in pseudo-obs");
    tt->help = tdrpStrDup("distance beyond which nbrs obs do not go into spatial average");
    tt->val_offset = (char *) &max_distance - &_start_;
    tt->single_val.f = 100;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }
