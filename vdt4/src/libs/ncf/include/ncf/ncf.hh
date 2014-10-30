// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 *   Module: ncf.hh
 *
 *   Author: Gerry Wiener
 *
 *   Date:   6/18/99
 *
 *   Description: Netcdf definitions
 */

#ifndef NCF_HH
#define NCF_HH

#ifdef SUNOS5_INTEL
#include <ieeefp.h>
#endif

#include <math.h>
#include <netcdf.hh>

#include "gtime.h"

namespace
{
  // constants
  const int SEC_PER_HOUR = 3600;
  const int SEC_PER_DAY = 24*SEC_PER_HOUR;
}

/*
  Function: Resets all non-finite array values to fill_value. 
  Returns: 1 if non-finite value is found, 0 otherwise.
*/
int NCF_finite(float *array, int len, float fill_value);

/*
  Function: Generates an output file from a cdl file. Original version 
  without the nc_version argument, and the new one which allows you to set
  the netcdf version, available in 3.6 and beyond.
  Returns: 0 if successful, -1 if not.
*/
int NCF_cdl2nc(const char *cdl_file, const char *ncfile);
int NCF_cdl2nc(const char *cdl_file, const char *ncfile, int nc_version);

/*
  Function: Finds 00Z of the day represented by current_time.
  Returns: Unix time of 00Z
*/
double NCF_find_day_start(double current_time);

/*
  Function: Converts time units used by Unidata's gribtonc to unix time
  Returns: Unix time 
*/
double NCF_cdf2unix_time(double input_time);

/*
  Function: Gets dimension size of dimension dim_name from infile.
  Returns: dim sixe if successful, -1 if not
*/
int NCF_get_dim_size(NcFile *infile, const char *dim_name);

/*
  Function: Converts attribute to a character string.
  Returns: Character array if successful, null pointer if not
  Notes: Memory of character array should be deleted by caller
  using delete[].
*/
char *NCF_att_as_string(NcAtt *att_ptr);

/*
  Function: Writes variable var_name to output file.
  Returns: 0 if successful, -1 if not.
*/
int NCF_write_var(NcFile *ncfp, const char *var_name, float *val);
int NCF_write_var(NcFile *ncfp, const char *var_name, double *val);
int NCF_write_var(NcFile *ncfp, const char *var_name, int *val);
int NCF_write_var(NcFile *ncfp, const char *var_name, char *val);
int NCF_write_var(NcFile *ncfp, const char *var_name, ncbyte *val);

/*
  Function: Gets fill value for particular variable. 
  Returns: 0 if successful, and assigns fill value to fill_val,
  otherwise returns -1 .
*/
int NCF_get_var_fill_val(NcVar *var_ptr, float *fill_val);
int NCF_get_var_fill_val(NcVar *var_ptr, int *fill_val);

// Get fill value given NcFile pointer and variable name
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, float *value);
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, int *value);
int NCF_get_fill_value(NcFile *ncfp, const char *var_name, unsigned char *value);

/*
  Function: Gets attribute value for a variable given NcFile, var_name,
            attribute name and index. Functions without index argument get
            scalar attributes.
  Returns: 0 if successful, otherwise -1. Assigns attribute value to value.
*/
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, float *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, float *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, int *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, double *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, double *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, short *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, short *value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int index, char **value);
int NCF_get_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, char **value);

int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, float value);
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, int value);
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, double value);
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, short value);
int NCF_put_var_attr(NcFile *ncfp, const char *var_name, const char *attr_name, const char *value);

/*
  Function: Gets value of variable var_name.
  Returns: 0 if successful, -1 if not
  Notes: Works only for single values (not arrays)
*/
int NCF_get_var(NcFile *ncfp, const char *var_name, float *val);
int NCF_get_var(NcFile *ncfp, const char *var_name, int *val);
int NCF_get_var(NcFile *ncfp, const char *var_name, double *val);
int NCF_get_var(NcFile *ncfp, const char *var_name, short *val);

/*
  Function: Gets variables and their dimensions. 
  Returns: 0 if successful, -1 if not.
  Notes: Remember to delete dim and val when no longer needed. 
  Memory is allocated using new [].
*/
int NCF_get_var(NcFile *ncfp, const char *var_name, float **val, long **dim, int *num_dim, long *size);
int NCF_get_var(NcFile *ncfp, const char *var_name, int **val, long **dim, int *num_dim, long *size);
int NCF_get_var(NcFile *ncfp, const char *var_name, double **val, long **dim, int *num_dim, long *size);
int NCF_get_var(NcFile *ncfp, const char *var_name, char **val, long **dim, int *num_dim, long *size);
int NCF_get_var(NcFile *ncfp, const char *var_name, ncbyte **val, long **dim, int *num_dim, long *size);
int NCF_get_var(NcFile *ncfp, const char *var_name, short **val, long **dim, int *num_dim, long *size);

/*
  Function: Writes variable val to output file ncfp.
  Returns: 0 if successful, -1 if not
*/
int NCF_put_var(NcFile *ncfp, const char *var_name, const float *val, const long *dim);
int NCF_put_var(NcFile *ncfp, const char *var_name, const int *val, const long *dim);
int NCF_put_var(NcFile *ncfp, const char *var_name, const double *val, const long *dim);
int NCF_put_var(NcFile *ncfp, const char *var_name, const char *val, const long *dim);
int NCF_put_var(NcFile *ncfp, const char *var_name, const short *val, const long *dim);

/*
  Function: Gets global attribute value for given NcFile and attribute name.
  Returns: 0 if successful, otherwise -1. Assigns attribute value to value.
*/
int NCF_get_attr(NcFile *ncfp, const char *attr_name, float *value);
int NCF_get_attr(NcFile *ncfp, const char *attr_name, int *value);
int NCF_get_attr(NcFile *ncfp, const char *attr_name, double *value);
int NCF_get_attr(NcFile *ncfp, const char *attr_name, short *value);
int NCF_get_attr(NcFile *ncfp, const char *attr_name, char **value);

int NCF_put_attr(NcFile *ncfp, const char *attr_name, float value);
int NCF_put_attr(NcFile *ncfp, const char *attr_name, int value);
int NCF_put_attr(NcFile *ncfp, const char *attr_name, double value);
int NCF_put_attr(NcFile *ncfp, const char *attr_name, short value);
int NCF_put_attr(NcFile *ncfp, const char *attr_name, const char *value);

inline int is_fill_float(float val) {return (val == NC_FILL_FLOAT);};

#endif /* NCF_HH */
