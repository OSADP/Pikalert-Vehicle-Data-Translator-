// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 *   Module: nxncf.hh (broken off from ncf.hh)
 *
 *   Author: Gerry Wiener
 *
 *   Date:   6/18/99
 *
 *   Description: Netcdf definitions
 */

#ifndef NXNCF_HH
#define NXNCF_HH

#include <netcdf.hh>
//#include <site_list/site_list.hh>
#include <ncf/ncf.hh>
#include <vector>
#include <map>
#include <cstdio>
using namespace std;

namespace
{
  // dimension names 
  const char *DAILY_TIME_DIM_NAME = "daily_time";
  const char *DAYS_DIM_NAME = "days";
  const char *FC_TIMES_PER_DAY_NAME = "fc_times_per_day";
  const char *MAX_MODEL_NAME_LEN = "max_model_name_len";
  const char *MAX_MODEL_NUM_NAME = "max_model_num";
  const char *MAX_NUM_MOD_TIMES_NAME = "max_num_mod_times";
  const char *MAX_REG_NUM_NAME = "max_reg_num";
  const char *MAX_SITE_NUM_NAME = "max_site_num";
  const char *NUM_FC_TIMES_NAME = "num_fc_times";
  const char *PARAM_VALS_DIM_NAME = "param_vals";
  const char *QUART_TIMES_DIM_NAME = "quartile_times";
  const char *REG_VALS_NAME = "reg_vals";
  const char *TIMES_PER_DAY_NAME = "times_per_day";
  const char *NUM_OBS_PER_DAY_NAME = "num_obs_per_day";
  const char *WEIGHT_VALS_DIM_NAME = "weight_vals";

  // variable names 
  const char *CDL_TYPE_NAME = "type";
  const char *CREATION_TIME_NAME = "creation_time";
  const char *FORC_TIME_NAME = "forc_time";
  const char *MODEL_ID_NAME = "model_id";
  const char *MODEL_LIST_NAME = "model_list";
  const char *MODEL_NAMES_NAME = "model_names";
  const char *NBR_LIST_NAME = "nbr_list";
  const char *NUM_MOD_TIMES_NAME = "num_mod_times";
  const char *NUM_MODELS_NAME = "num_models";
  const char *NUM_REGS_NAME = "num_regs"; 	
  const char *NUM_SITES_NAME = "num_sites";
  const char *REG_IDS_NAME = "reg_ids";		
  const char *REG_VAR_NAME = "regressors";	
  const char *SITE_LIST_NAME = "site_list";
  const char *TIME_NOMINAL_NAME = "time_nominal";


  // dimension sizes 
  const int DAILY_TIME_DIM = 1;
  const int MONTHS_DIM = 12;
  const int MF_PARAM_VALS = 4;
  const int IE_WT_VALS = 6;
  const int QUART_TIMES_DIM = 4;

  // cdl types
  const int DMOS_TYPE = 0;
  const int INT_EMP_TYPE = 1;
  const int MOD_FCST_TYPE = 2;
  const int MOD_FCST_EMP_TYPE = 3;
  const int REG_TYPE = 4;
  const int SYN_DAILY_TYPE = 5;
  const int SYN_FIXED_TYPE = 6;

  // constants
  const float dmos_conf_lower_bound = 0.5;
  const float  fcst_emp_reg_sentinel = -1.0;
  const int REG_TIME_INTERVAL = 6; // Spacing in number of hours of regressor values
  const int FORECAST_OFFSET = 3*SEC_PER_HOUR;
  const int FORECAST_INTERVAL = 6*SEC_PER_HOUR;

  // forecast empirics indices
  enum fcst_emp_ind 
  {
    IND_FE_ID_MEAN = 0, IND_FE_TIME_MEAN, IND_FE_WEIGHT_MEAN, 
    IND_FE_ID_STD, IND_FE_TIME_STD, IND_FE_WEIGHT_STD, IND_FE_NUM_IND
  };

  // ensemble forecast empirics indices
  enum fcst_emp_ens_ind
  {
    IND_FE_ENS_ID_MEAN = 0, IND_FE_ENS_TIME_MEAN, IND_FE_ENS_WEIGHT_MEAN,
    IND_FE_ENS_R2, IND_FE_ENS_NUM_IND
  };

  const char *fcst_emp_string[] = 
  {
    "fe_reg_id_mean",
    "fe_time_mean",
    "fe_weight_mean",
    "fe_reg_id_std",
    "fe_time_std",
    "fe_weight_std"
  };

  // regressor variable name
  const char *Reg_var = "regressors";

  // forecast module parameter vals indices
  enum fcst_mod_ind
  {
    IND_FM_MEAN = 0, IND_FM_STD, IND_FM_MEAN_CONF, IND_FM_STD_CONF, IND_FM_NUM_IND
  };

  const int NUM_FM_STAT = IND_FM_NUM_IND / 2;

  // forecast module dimension indices 
  enum fcst_var_ind
  {
    IND_FM_SITE_DIM = 0, IND_FM_DAYS_DIM, IND_FM_TIME_DIM, IND_FM_PVALS_DIM, IND_FM_NUM_VAR_DIM
  };

  // regressor indices
  enum reg_ind
  {
    IND_REG_MEAN = 0, IND_REG_CONF, IND_REG_NUM_IND
  };

  // regressor dimension indices
  enum reg_var_ind
  {
    IND_REG_MOD_TM_DIM = 0, IND_REG_SITE_DIM, IND_REG_NUM_DIM, IND_REG_VAL_DIM, IND_REG_VAR_NUM_DIM
  };

  // integrator empirics indices
  enum int_emp_ind
  {
    IND_IE_MEAN = 0, IND_IE_STD, IND_IE_MEAN_CONF, IND_IE_STD_CONF, IND_IE_BIAS_MEAN, IND_IE_ERROR, IND_IE_NUM_IND
    //IND_IE_MEAN = 0, IND_IE_STD, IND_IE_MEAN_CONF, IND_IE_STD_CONF, IND_IE_BIAS_MEAN, IND_IE_BIAS_STD, IND_IE_NUM_IND
  };
  // For backward compatibility
  const int IND_IE_BIAS_STD = IND_IE_ERROR;

  enum dmos_var_ind
  {
    IND_DM_SITE_DIM, IND_DM_DAYS_DIM, IND_DM_FC_TPD_DIM, IND_DM_REG_NUM_DIM, IND_DM_REG_VALS_DIM, IND_DM_VAR_NUM_DIM
  };  
  
};

int NXNCF_get_sites(NcFile &ncf, int **site_ids);
long NXNCF_create_index_array(int *sl_arr, long num_sites, NcFile *ncfile, long **ind_array);
int NXNCF_write_creation_time(NcFile &ncfile);


typedef map<int, int, less<int> > maptype;

class NXNCF_site
{
public:
  NXNCF_site(NcFile *ncfp, int *sa, int sa_size);
  ~NXNCF_site();
  vector<int> site_index;	// Array of site indices
  int size;			// Size of site index array
  void print()
    {
      int i;

      for (i=0; i<size; i++)
	{
	  printf("site[%d] = %d\n", i, site_index[i]);
	}

    }
};

// This class proides a wrapper around writing output to a netcdf forecast file
// 
class NXNCF_fcst_output
{
public:
  NXNCF_fcst_output(char *cdl_file, int *site_list, int site_list_size, double forc_time, char *output_file);
  ~NXNCF_fcst_output();
  const char *error() {return error_str;}
  int set_var(const char *var_name); // set current variable to var_name
  int write_var(double *array);	// write entire array for current variable
  int write_var(float *array);	// write entire array for current variable
  int write_var(int *array);	// write entire array for current variable
  int write_var(char *array);	// write entire array for current variable
  int write_var_slice(double *array, long *sl_edges); // write slice of array for current variable
  int write_var_slice(float *array, long *sl_edges); // write slice of array for current variable
  int write_var_slice(int *array, long *sl_edges); // write slice of array for current variable
  int write_var_slice(char *array, long *sl_edges); // write slice of array for current variable
  int get_dim_size(const char *dim_name);
  
  int write_test_ramp
  (
   int dims_to_ramp			// Number of ending dimensions for the ramp (must be less than the number of dimensions for the variable
   );

  const long *get_edges() {return edges;}
  int get_num_dims() {return num_dims;}
  long get_num_vals() {return num_vals;}
private:
  NXNCF_fcst_output();		// no void constructor
  NXNCF_fcst_output(const NXNCF_fcst_output &);	/* no copy constructor */
  NXNCF_fcst_output & operator=(const NXNCF_fcst_output &); /* no assignment operator */
  NcVar *var_ptr;		// pointer to current output variable
  long *edges;			// edges for output variable
  int num_dims;			// number of dimensions of edges
  long num_vals;		// number of values of variable
  int max_site_num;             // number of sites, always first dimension, may
                                // be fixed or UNLIMITED
  const char *error_str;
  NcFile *ncfp;
};

// typedef map<int, int, less<int> > maptype;


#endif /* NXNCF_HH */




