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
//   Module: ncfc.hh
//
//   Date:   5/22/07
//
//   Description: Netcdf convenience functions
//----------------------------------------------------------------------

#ifndef NCFC_HH
#define NCFC_HH

#include <map>
#include <netcdf.h>
#include <vector>
#include <string>
#include <netcdf.h>

using std::map;
using std::string;
using std::vector;

class Var_input
{
public:
  /**
   * @brief Constructor
   */
  Var_input();

  /**
   * @brief Constructor
   * @param[in] in_ncid input netcdf file id
   * @param[in] var_names vector of variable names of interest
   */
  Var_input(int in_ncid, const vector<string> &var_names);

  /**
   * @brief Constructor
   * @param[in] in_ncid input netcdf file id
   * @param[in] var_names vector of variable names of interest
   * @param[in] dim_names vector of dimension names of interest
   */
  Var_input(int in_ncid, const vector<string> &var_names, const vector<string> &dim_names);

  /**
   * @brief Constructor 
   * @param[in] in_file input netcdf file name
   * @param[in] var_names vector of variable names of interest
   */
  Var_input(const char *in_file, const vector<string> &var_names);

  /**
   * @brief Constructor 
   * @param[in] in_file input netcdf file name
   * @param[in] var_names vector of variable names of interest
   * @param[in] dim_names vector of dimension names of interest
   */
  Var_input(const char *in_file, const vector<string> &var_names, const vector<string> &dim_names);

  /**
   * @brief Destructor
   */
  ~Var_input();

  /**
   * @brief Initialize var input
   * @param[in] in_ncid netcdf file id
   * @param[in] var_names vector of variable names of interest
   */
  void init_var_input(int in_ncid, const vector<string> &var_names);

  /**
   * @brief Initialize var input
   * @param[in] in_ncid netcdf file id
   * @param[in] var_names vector of variable names of interest
   * @param[in] dim_names vector of dimension names of interest
   */
  void init_var_input(int in_ncid, const vector<string> &var_names, const vector<string> &dim_names);

  /**
   * @brief Initialize var input
   * @param[in] in_file netcdf file string
   * @param[in] var_names vector of variable names of interest
   */
  void init_var_input(const char *in_file, const vector<string> &var_names);

  /**
   * @brief Initialize var input
   * @param[in] in_file netcdf file string
   * @param[in] var_names vector of variable names of interest
   * @param[in] dim_names vector of dimension names of interest
   */
  void init_var_input(const char *in_file, const vector<string> &var_names, const vector<string> &dim_names);

  /**
   * @brief Get variable names
   */
  const vector<string> & get_var_names() const {return m_var_names;}

  /**
   * @brief Get variable data pointers
   */
  const vector<void *> & get_var_ptrs() const {return m_var_ptrs;}

  /**
   * @brief Get variable fill pointers
   */
  const vector<void *> & get_var_fill_ptrs() const {return m_var_fill_ptrs;}  

  /**
   * @brief Get variable scale factor pointers
   */
  const vector<void *> & get_var_scale_factor() const {return m_var_scale_factor;}  

  /**
   * @brief Get variable add offset pointers
   */
  const vector<void *> & get_var_add_offset() const {return m_var_add_offset;}  

  /**
   * @brief Get variable types
   */
  const vector<nc_type> & get_var_types() const {return m_var_types;}

  /**
   * @brief Get variable units
   */
  const vector<string> & get_var_units() const {return m_var_units;}

  /**
   * @brief Get variable number of dimensions
   */
  const vector<int> & get_var_ndims() const {return m_var_ndims;}

  /**
   * @brief Get variable dimensions
   */
  const vector<size_t *> & get_var_dims() const {return m_var_dims; }

  /**
   * @brief Get file dimension map
   */
  void get_file_dim_map(map<string, size_t> &file_dim_map) const;

  /**
   * @brief Get file dimension names
   */
  const vector<string> & get_file_dim_names() const {return m_file_dim_names; }

  /**
   * @brief Get file dimension lengths
   */
  const vector<size_t> & get_file_dim_len() const {return m_file_dim_len; }

  /**
   * @brief Get variable index map
   */
  void get_var_index_map(map<string, unsigned int> &var_index_map) const;

  /**
   * @brief Get variable sizes
   */
  const vector<long int> & get_var_sizes() const {return m_var_sizes; }

  int get_char_vec(const string &var_name, vector<char> &vec);
  int get_double_vec(const string &var_name, vector<double> &vec);
  int get_float_vec(const string &var_name, vector<float> &vec);
  int get_int_vec(const string &var_name, vector<int> &vec);
  int get_short_vec(const string &var_name, vector<short> &vec);

  /**
   * @brief Get integer error status information
   */
  int error_status() const {return m_error_status; }

  /**
   * @brief Get error description
   */
  const string & error() const {return m_error; }

protected:
  Var_input(const Var_input &var_input); // no support for copy constructor
  Var_input & operator=(const Var_input &var_input); // no support for assignment

  map<string, unsigned int> m_var_index_map;
  map<string, size_t> m_file_dim_map;
  int m_error_status;		// error status
  string m_error;		// error string
  vector<string> m_var_names;	// variable names of interest
  vector<void *> m_var_ptrs;	// storage for each variable (contents can be deleted by caller using delete [] var_ptrs[i])
  vector<void *> m_var_fill_ptrs; // pointer is 0 if no fill value for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] m_var_fill_ptrs[i])
  vector<void *> m_var_scale_factor; // pointer is 0 if no scale factor for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] m_var_scale_factor[i])
  vector<void *> m_var_add_offset; // pointer is 0 if no add offset for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] m_var_add_offset[i])
  vector<nc_type> m_var_types;	// type of variable using netcdf types NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT and NC_DOUBLE
  vector<string> m_var_units;	// variable units name strings
  vector<int> m_var_ndims;	// number of dimensions of variable 
  vector<size_t *> m_var_dims;	// actual dimensions of variable (contents can be deleted by caller using delete [] var_dims[i])
  vector<long int> m_var_sizes;	// size of variable 

  vector<string> m_file_dim_names;	// names of dimensions
  vector<size_t> m_file_dim_len; 	// lengths of dimensions
};

/**
 * @class Var_output
 */
class Var_output
{
public:
  /** Constructor */
  Var_output();

  /** Constructor */
  Var_output(const vector<string> &var_names_param, const vector<nc_type> &var_types_param, const vector<long int> &var_sizes_param);

  /** Destructor */
  virtual ~Var_output();

  /** @brief variable names container */
  vector<string> var_names;

  /** @brief pointers to space for each variable */
  vector<void *> var_ptrs;

  /** @brief netcdf integer type for each variable */
  vector<nc_type> var_types;

  /** @brief array size of each variable */
  vector<long int> var_sizes;

  /**
   * @brief Get variable names
   */
  const vector<string> & get_var_names() const {return var_names;}

  /**
   * @brief Get variable data pointers
   */
  const vector<void *> & get_var_ptrs() const {return var_ptrs;}

  /**
   * @brief Get variable data types
   */
  const vector<nc_type> & get_var_types() const {return var_types;}

  /**
   * @brief Get variable sizes
   */
  const vector<long int> & get_var_sizes() const {return var_sizes; }

  /** @brief error string */
  string error;

private:

  Var_output & operator=(const Var_output &var_output);
  Var_output(const Var_output &var_output);
};


/**
 * @brief get listing of variable names and types from netcdf file
 *
 * @param[in] ncid netcdf file handle
 * @param[out] var_name vector of variable names
 * @param[out] var_type vector of variable types
 * @return 0 on success, -1 on failure
 */
int NCFC_get_var_list(int ncid, vector<string> &var_name, vector<nc_type> &var_type);


/**
 * @brief Extract variables of interest from netcdf file. All of variables contents will be returned.
 * All variables are assumed to be of a single dimension.
 * Returns number of records or -1 on failure
 *
 * @param[in] in_ncid input netcdf file id
 * @param[in] var_names variable names of interest
 * @param[out] var_ptrs allocates storage for each variable (contents can be deleted by caller using delete [] var_ptrs[i])
 * @param[out] var_fill_ptrs pointer is 0 if no fill value for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] var_fill_ptrs[i])
 * @param[out] var_types type of variable using netcdf types NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT and NC_DOUBLE
 * @param[out] var_units variable units name strings
 * @param[out] var_ndims number of dimensions of variable 
 * @param[out] var_dims actual dimensions of variable (contents can be deleted by caller using delete [] var_dims[i])
 * @param[out] var_sizes size of variable 
 * @param[out] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_get_variables
(
 int in_ncid,
 const vector<string> &var_names,
 vector<void *> &var_ptrs,
 vector<void *> &var_fill_ptrs,
 vector<nc_type> &var_types,
 vector<string> &var_units,
 vector<int> &var_ndims,
 vector<size_t *> &var_dims,
 vector<long int> &var_sizes,
 string &error
 );

/**
 * @brief Append data to netcdf file
 *
 * @param[in] out_ncid output netcdf file id
 * @param[in] var_names variable names 
 * @param[in] var_ptrs contains pointers to variable data
 * @param[in] var_sizes contains size of each variable 
 * @param[in] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_append_variables
(
 int out_ncid,			// I - output netcdf file id
 const vector<string> &var_names,	// I - variable names 
 const vector<void *> &var_ptrs,	// I - will contain variable data
 const vector<long int> &var_sizes,	// I - size of variable 
 string &error			// O - error string
 );

/**
 * @brief Output data to netcdf file
 *
 * @param[in] out_ncid output netcdf file id
 * @param[in] var_names variable names 
 * @param[in] var_ptrs contains pointers to variable data
 * @param[in] var_sizes contains size of each variable 
 * @param[in] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_put_variables
(
 int out_ncid,
 const vector<string> &var_names,
 const vector<void *> &var_ptrs,
 const vector<long int> &var_sizes,
 string &error
 );

/**
 * @brief Get unlimited dimension from netcdf file
 * @param[in] in_ncid input netdf file id
 * @param[out] unlim_name name of unlimited dimension
 * @param[out] unlim_id netcdf id for unlimited dimension
 * @param[out] unlim_size size of unlimitied dimension
 * @return 0 on success, -1 on failure
 */
int NCFC_get_unlim(int in_ncid, string &unlim_name, int *unlim_id, int *unlim_size);



/**
 * @brief Get variable information. Information includes variable
 * type, dimensions, fill_values and units. Function is similar to
 * NCFC_get_var_info but does not get actual variable data.
 *
 * @param[in] in_ncid input netcdf file id
 * @param[in] var_names variable names of interest
 * @param[out] var_fill_ptrs pointer is 0 if no fill value for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] var_fill_ptrs[i])
 * @param[out] var_types type of variable using netcdf types NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT and NC_DOUBLE
 * @param[out] var_units variable units name strings
 * @param[out] var_ndims number of dimensions of variable 
 * @param[out] var_dims actual dimensions of variable (contents can be deleted by caller using delete [] var_dims[i])
 * @param[out] var_sizes size of variable 
 * @param[out] error descriptive error string
 * Returns 0 on success, -1 on failure
 */
int NCFC_get_var_info
(
 int in_ncid,			// I - input netcdf file id
 const vector<string> &var_names,	// I - variable names of interest
 vector<void *> &var_fill_ptrs,	// O - pointer is 0 if no fill value for variable, otherwise pointer points to value of variable (contents can be deleted by caller using delete [] var_fill_ptrs[i])
 vector<nc_type> &var_types,	// O - type of variable using netcdf types NC_CHAR, NC_SHORT, NC_INT, NC_FLOAT and NC_DOUBLE
 vector<string> &var_units,	// O - variable units name strings
 vector<int> &var_ndims,	// O - number of dimensions of variable 
 vector<size_t *> &var_dims,	// O - actual dimensions of variable (contents can be deleted by caller using delete [] var_dims[i])
 vector<long int> &var_sizes,	// O - size of variable 
 string &error			// O - error description
 );

/**
 * @brief Get dimension size information for specified dimension names
 *
 * @param[in] in_ncid input netcdf file id
 * @param[in] dim_names dimension names of interest
 * @param[out] dim_len dimension lengths
 * @param[out] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_get_dims
(
  int in_ncid,
  const vector<string> &dim_names,
  vector<size_t> &dim_len,
  string &error
  );

/**
 * @brief Get input from netcdf file and store in vectors
 *
 * @param[in] in_file input netcdf file name
 * @param[in] var_names variable names of interest
 * @param[in/out] var_input variable input container
 * @param[in] dim_names dimension names of interest
 * @param[out] dim_len dimension lengths
 * @param[out] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_cdfinput(const char *in_file, const vector<string> &var_names, Var_input **var_input, const vector<string> &dim_names, vector<size_t> &dim_len, string &error);

/**
 * @brief Same as NCFC_cdfinput()
 * @param[in] in_file input netcdf file name
 * @param[in] var_names variable names of interest
 * @param[in/out] var_input variable input container
 * @param[in] dim_names dimension names of interest
 * @param[out] dim_len dimension lengths
 * @param[out] error descriptive error string
 * @return 0 on success, -1 on failure
 */
int NCFC_var_dims_input(const char *in_file, const vector<string> &var_names, Var_input **var_input, const vector<string> &dim_names, vector<size_t> &dim_len, string &error);

/**
 * @brief Generate netcdf file using cdl file 
 * @param[in] cdl_file cdl file to use as template
 * @param[in] out_file output file name
 * @param[out] error output error string
 * @return 0 on success, -1 on failure
 */
int NCFC_make_cdf(const string &cdl_file, const string &out_file, string &error);

/**
 * @brief Generate 64 bit netcdf file using cdl file 
 * @param[in] cdl_file cdl file to use as template
 * @param[in] out_file output file name
 * @param[out] error output error string
 * @return 0 on success, -1 on failure
 */
int NCFC_make_64bit_cdf(const string &cdl_file, const string &out_file, string &error);


#endif /* NCFC_HH */
