// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: fec.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/5/02
 *
 * Updated: 09/20/10 by Seth Linden
 * Updated to do road-T fec May/July 2011 by Seth Linden
 *
 * Description:
 *     
 */

// Include files 
#include <new>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <math.h>

#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "netcdf.h"
#include "Params.hh"

#include "concat_meso_data.h"

// Constant, macro and type definitions 
const float A = 4.;
const float B = 2.;
const float MAX_ALT_DIFF = 300.;

// Global variables 
extern Log *proc_log;
extern Params conf_params;


// Functions and objects
extern int read_fcst_info(NcFile *input_file, int *num_input_sites,
			  double *forc_time, long *num_input_times_per_day,
			  long *num_input_day, long *max_site_num);

extern int read_fcst_data(NcFile *input_file, char *var_name, 
			  long *times_per_day, float *data);

extern int read_nbr_info(NcFile *nbr_file, long *num_nbr, int *num_nbr_sites,
			 int **nbr_site_list, double **nbr_list);

extern int read_obs_info(NcFile *obs_file, long *num_todays_obs_times_per_day,
		  double **todays_obs_time, int *num_todays_obs_sites,
			 int **todays_obs_site_list);

extern int read_obs_data(NcFile *obs_file, char *var_name, long *times_per_day,
			 float *data);

extern int create_output_file(char *cdl_name, char * output_name);

extern int write_header(NcFile *output_file, double forc_time,
			int num_output_sites, int  *output_site_array, 
			long *max_output_sites, long *num_output_days,
			long *num_output_times_per_day);

extern int get_nbr_list(int site_num, double *nbr_list, int *nbr_site_list,
			int num_nbr_sites, int num_nbr, int *nbr, float *dist);

extern int get_forecast(int site_index, float *fcst_data,  int nday,
			int times_per_day, double forc_time, double corr_time, 
			float *fcst_val);

extern int find_nbr_obs(int num_obs_sites, int *obs_site_list,
			float *obs_data, int times_per_day,
			int nbr, double *obs_time_array,
			double obs_time, float *obs);
extern int write_mod_fcst(NcFile *output_file, char *var_name,
			 float *fcst_data);
extern float get_alt(int site_id, int *output_site_array, 
		     int num_output_sites, float *alt);

// Functions to set up new dynamic fec
extern int get_site_forecast_data(int site_index, float *fcst_data, int num_days, 
				  int num_times_per_day, float* &site_fcst_data);

extern int find_inflection_ind(int start_ind, int num_vals, float *data, int pos);

extern int get_site_obs_data(int dicast_id, int num_obs_sites, int *obs_site_list, 
			     float *obs_data, int num_times_per_day, float* &site_obs_data);

extern int concat_obs_data(float *todays_obs_data, float *yesterdays_obs_data, float* &all_obs_data, 
			   double *todays_obs_times, long todays_num_obs_times, double *yesterdays_obs_times, 
			   long yesterdays_num_obs_times, double* &all_obs_times);

extern float get_interp_obs_val(float *site_obs_data, int obs_start_ind, float *site_fcst_data, int fcst_start_ind, int do_interp);

extern int set_dynamic_forecast(int ns, int site_index, float * output_data,
				float *corr_factors, float num_corr_hrs, 
				float *fcst_data, int nday, int times_per_day, 
				double forc_time, double corr_time, float fcst_error);

// Function to read / write rctm fcst data
int read_rctm_fcst_info(NcFile *input_file, int *num_input_sites, int *type,
                        double *forc_time, long *num_input_times_per_day,
                        long *num_input_day, long *max_site_num, long *max_str_len,
                        long *total_num_times);

int write_rctm_header(NcFile *output_file, double forc_time, int num_output_sites, int output_type,
                      int  *output_site_array, long *max_output_sites, long *num_output_days,
                      long *num_output_times_per_day, long *output_max_str_len, long *output_total_num_times);

int read_char_fcst_data(NcFile *input_file, char *var_name,
			long *times_per_day, char *data);

int write_char_fcst_data(NcFile *output_file, char *var_name,
                         char *data);

// Functions to read in concat_meso data files for road-T fec
extern int get_site_info(vector<site *> &sites, char *fname);
extern int alloc_data(ds *data, vector<site *> sites, int num_out_times);
extern int get_data(double start_time, int num_out_times, vector<site *> sites, char *inputFileName, ds *data);
extern int get_site_info(vector<site *> &sites, char *fname);

// Local functions
int proc_meso_data(double start_time, int num_out_times, vector<site *> sites, char *meso_file, ds *meso_data);

int fec(double forc_corr_time, double forc_gen_time, char *site_list_file,
	char *nbr_file_name, char *cdl_file_name, char *todays_obs_file_name, 
	char *yesterdays_obs_file_name, char *concat_meso_file, char *input_file_name, char *output_file_name)
{
  int num_corr_hrs;
  int corr_hr;
  int num_input_sites, num_output_sites, num_todays_obs_sites, num_yesterdays_obs_sites;
  int  *output_site_array = 0;
  long *index_array = 0;
  float *alt;
  NcVar *input_var_ptr, *out_var_ptr, *obs_var_ptr;
  double *todays_obs_time, *yesterdays_obs_time;
  int *todays_obs_site_list, *yesterdays_obs_site_list;
  double forc_time;
  long num_input_times_per_day, num_input_day, max_input_sites;
  long num_output_times_per_day, num_output_days, max_output_sites;
  long num_todays_obs_times_per_day, num_yesterdays_obs_times_per_day;
  long times_per_day, todays_obs_times_per_day, yesterdays_obs_times_per_day;
  float *fcst_data, *todays_obs_data, *yesterdays_obs_data, *all_obs_data, fcst_val, obs_val;
  int num_nbr_sites;
  long num_nbr;
  double *nbr_list;
  int *nbr_site_list;
  float nbr_alt;
  float nbr_obs;
  float fcst_error;
  float *output_data = NULL;
  int output_data_size = 0;

  // For reading / writing rctm fcst data
  int nc;
  int input_type;
  long input_max_str_len;
  long input_total_num_times;
  char *char_fcst_data;

  long output_type;
  long output_max_str_len;
  long output_total_num_times;
  char *char_output_data;

  int i;
  int ret;
  int np, ns, nd, nt;
  struct tm *t;
  time_t clock; 
  int available_error_hrs;
  float L, D;

  // Set up behaviour of netCDF library
  NcError* ncErr = new NcError(NcError::silent_nonfatal);

  // Make sure times are at the top of the hour (for correction time)
  // and top o' the day for generation time
  forc_corr_time = SEC_PER_HOUR * (int)(forc_corr_time/SEC_PER_HOUR);
  forc_gen_time = SEC_PER_DAY * (int)(forc_gen_time/SEC_PER_DAY);

  available_error_hrs = 0;

  // UTC hour of correction time
  corr_hr = (int)(forc_corr_time - forc_gen_time);
  corr_hr /= SEC_PER_HOUR;

  while (corr_hr >= 24)
    corr_hr -= 24;

  while (corr_hr < 0)
    corr_hr += 24;
  
  // Open and read input  netcdf file
  NcFile input_file(input_file_name, NcFile::ReadOnly);
  if (!input_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open input file %s\n",input_file_name);
      return(1);
    }

  // Determine if we are doing wx, road, or bridge fec
  // by looking at the params: input_file_type
  int rctm_flag = 0;
  int bridge_flag = 0;
  if( strcmp(conf_params.input_file_type, "bc") == 0 )
    {
      rctm_flag = 1;
      bridge_flag = 1;
      proc_log->write_time(1, "Info: Working with rctm bridge data\n");
    }
  else if( strcmp(conf_params.input_file_type, "rc") == 0 )
    {
      rctm_flag = 1;
      proc_log->write_time(1, "Info: Working with rctm road data\n");
    }
  else
    {
      proc_log->write_time(1, "Info: Working with weather data\n");  
    }

 
  // Cross check some of the command line arguments
  if( (strcmp(todays_obs_file_name, "None") == 0 || strcmp(todays_obs_file_name, "none") == 0) &&  strlen(concat_meso_file) == 0 )
    {
      proc_log->write_time("Error: Can not fec data without todays int_obs file or concat_meso_file, fix the command line\n");
      return(1);
    }
  
  if( (strcmp(todays_obs_file_name, "None") != 0 && strcmp(todays_obs_file_name, "none") != 0) &&  strlen(concat_meso_file) != 0 )
    {
      proc_log->write_time("Error: Can not use both an int_obs file and concat_meso_file, weather or road fec?, fix the command line\n");
      return(1);
    }
  
  if(rctm_flag == 1)
    {
      if( strlen(concat_meso_file) == 0 )
	{
	  proc_log->write_time("Error: Can not fec rctm data without a concat_meso_file, fix the params file or fix the command line\n");
	  return(1); 
	}
    }
  else
    {
      if( (strcmp(todays_obs_file_name, "None") == 0 || strcmp(todays_obs_file_name, "none") == 0) )
	{
	  proc_log->write_time("Error: Can not fec weather data without todays int_obs_file, fix the params file or fix the command line\n");
	  return(1); 
	}
    }
  
  // Get input file header information (such as num_sites, site_list, ect)
  //
  if(rctm_flag == 0)
    {
      ret = read_fcst_info(&input_file, &num_input_sites, &forc_time,
			   &num_input_times_per_day, &num_input_day,
			   &max_input_sites);
    }
  else
    {
      ret = read_rctm_fcst_info(&input_file, &num_input_sites, &input_type,
				&forc_time, &num_input_times_per_day, 
				&num_input_day, &max_input_sites, &input_max_str_len,
				&input_total_num_times);
    }
  
  if (ret != 0)
    {
      proc_log->write_time("Error: Unable to read fcst file\n");
      return(1);
    }

  /* generate output netcdf file from cdl_file */
  ret = create_output_file(cdl_file_name, output_file_name);

  /* open netcdf file for writing */
  NcFile output_file(output_file_name, NcFile::Write);
  if (!output_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open output file %s\n",output_file_name);
      return(1);
    }

  /* create Site_list obj */
  Site_list output_sites(site_list_file);
  if (output_sites.error())
    {
      proc_log->write_time("Error: Unable to make output_sites object\n");
      proc_log->write_time(output_sites.error());
      return(1);
    }

  num_output_sites = output_sites.make_id_array(&output_site_array);
  if (num_output_sites < 0)
    {
      proc_log->write_time("Error: could not make_id_array\n");
      proc_log->write_time(output_sites.error());
      return(1);
    }

  /* create index array */
  ret = NXNCF_create_index_array(output_site_array, num_output_sites, 
			       &input_file, &index_array);
  if (ret < 0) {
    proc_log->write_time("Error: in call to NXNCF_create_index_array\n");
    return(1);
  }

  if (num_output_sites != output_sites.make_alt_array(&alt))
    {
      proc_log->write_time("Error: could not make_alt_array\n");
      proc_log->write_time(output_sites.error());
      return(1);
    }

  // Write output file header info
  //
  if(rctm_flag == 0)
    {
      ret = write_header(&output_file, forc_time, num_output_sites, 
			 output_site_array, &max_output_sites, &num_output_days,
			 &num_output_times_per_day);
    }
  else
    {
      output_type = input_type;
      ret = write_rctm_header(&output_file, forc_time, num_output_sites, output_type,  
			      output_site_array, &max_output_sites, &num_output_days,
			      &num_output_times_per_day, &output_max_str_len, &output_total_num_times);
    }
  
  if (ret < 0) {
    proc_log->write_time("Error: in writing header\n");
    return(1);
  }
  
  if (num_output_times_per_day != num_input_times_per_day)
    {
      proc_log->write_time("Error: num times per day must agree in cdl and input files\n");
      return(1);
    }

  if (num_output_sites > max_output_sites)
    {
      proc_log->write_time("Error: too many sites for output cdl\n");
      return(1);
    }

  // Copy over non-corrected vars into output file
  // Have to deal with char variables differently
  //
  int num_other_vars = conf_params.other_var_names_n;
  for (np = 0; np < num_other_vars; np++)
    {
      // Cross-check a few variables to make sure we aren't trying to 
      // read rctm variables from a weather file
      if(rctm_flag == 0 && 
	 (strcmp(conf_params._other_var_names[np], "application_rate") == 0 || 
	  strcmp(conf_params._other_var_names[np], "apply_chem") == 0))
	{
	  proc_log->write_time("Error: Can not read/write rctm variables from a weather input file\n");
	  return(1);
	}
      
      if(strcmp(conf_params._other_var_names[np], "treatment_explanation") == 0)
	{
	  //cout << "Working on char variable = " << conf_params._other_var_names[np] << endl;
	  
	  char_fcst_data = new char[max_input_sites*num_input_day*num_input_times_per_day*input_max_str_len];
	  char_output_data = new char[max_output_sites*num_output_days*num_output_times_per_day*output_max_str_len];
	  
	  read_char_fcst_data(&input_file, conf_params._other_var_names[np],
			      &times_per_day, char_fcst_data);
	} 
      else
	{
	  fcst_data = new float[max_input_sites*num_input_day*num_input_times_per_day];
	  output_data = new float[max_output_sites * num_output_days * num_output_times_per_day];
	  
	  read_fcst_data(&input_file, conf_params._other_var_names[np],
		     &times_per_day, fcst_data);
	}
      
      // Copy real data for output sites that exist in the input file
      //
      for (ns = 0; ns < num_output_sites; ns++)
	for (nd = 0; nd < num_output_days; nd++)
	  for (nt = 0; nt < times_per_day; nt++)
	    {
	      if(strcmp(conf_params._other_var_names[np], "treatment_explanation") == 0)
		{
		  for (nc = 0; nc < output_max_str_len; nc++)
		    {
		      int input_index = (index_array[ns] * num_input_day * times_per_day * input_max_str_len) + 
			(nd * times_per_day * input_max_str_len) + 
			(nt * input_max_str_len) + nc;
		      int output_index = (ns * num_output_days * times_per_day * output_max_str_len) + 
			(nd * times_per_day * output_max_str_len) + 
			(nt * output_max_str_len) + nc;
		      char_output_data[output_index] = char_fcst_data[input_index];
		    }
		}
	      else
		{
		  int input_index = index_array[ns] * num_input_day * times_per_day + nd * times_per_day + nt;
		  int output_index = ns * num_output_days * times_per_day + nd * times_per_day + nt;
		  output_data[output_index] = fcst_data[input_index];
		}
	    }
      
      // For  additional output sites, that are not in the input file
      // Copy fill data to output array
      for (ns = num_output_sites; ns < max_output_sites; ns++)
        for (nd = 0; nd < num_output_days; nd++)
          for (nt = 0; nt < times_per_day; nt++)
	    {
	      if(strcmp(conf_params._other_var_names[np], "treatment_explanation") == 0)
		{
		  for (nc = 0; nc < output_max_str_len; nc++)
		    {
		      int input_index = (index_array[ns] * num_input_day * times_per_day * input_max_str_len) + 
			(nd * times_per_day * input_max_str_len) + 
			(nt * input_max_str_len) + nc;
		      int output_index = (ns * num_output_days * times_per_day * output_max_str_len) + 
			(nd * times_per_day * output_max_str_len) + 
			(nt * output_max_str_len) + nc;
		      char_output_data[output_index] = NC_FILL_CHAR;
		    }
		}
	      else
		{
		  int output_index = ns * num_output_days * times_per_day + nd * times_per_day + nt;
		  output_data[output_index] = NC_FILL_FLOAT;
		}
	    }

      if(strcmp(conf_params._other_var_names[np], "treatment_explanation") == 0)
	write_char_fcst_data(&output_file, conf_params._other_var_names[np],
			     char_output_data);
      else
	write_mod_fcst(&output_file, conf_params._other_var_names[np],
		       output_data);
    } // end loop over variables

  
  // Read in nbr file
  NcFile nbr_file(nbr_file_name, NcFile::ReadOnly);
  if (!nbr_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open nbr file %s\n",
			   nbr_file_name);
      return(1);
    }

  ret = read_nbr_info(&nbr_file, &num_nbr, &num_nbr_sites, &nbr_site_list,
		      &nbr_list);
  int *nbr = new int[num_nbr];
  float *nbr_dist= new float[num_nbr];

  // Set Length Scale
  L = conf_params.length_scale / log(2.0);
  //printf("L = %f\n", L);

  // Set Max Distance for Nbr Obs Consideration
  D = conf_params.max_distance;

  
  // Open today's obs file
  NcFile todays_obs_file(todays_obs_file_name, NcFile::ReadOnly);
  if (!todays_obs_file.is_valid())
    {
      proc_log->write_time("Info: Unable to open todays int_obs file %s\n", todays_obs_file_name);
      //return(1);
    }
  else
    {
      // Get metadata from todays int_obs file
      read_obs_info(&todays_obs_file, &num_todays_obs_times_per_day,
		    &todays_obs_time, &num_todays_obs_sites, 
		    &todays_obs_site_list);
      
      // Make sure correction hour matchs to right time in obs file
      if (todays_obs_time[corr_hr] != forc_corr_time)
	{
	  proc_log->write_time("Error: Correction time %d does not match obs time\n",
			       (int)forc_corr_time, (int)todays_obs_time[corr_hr]);
	  return(1);
	}
      
      // Create space to read in todays obs data
      todays_obs_data = new float[num_todays_obs_sites*num_todays_obs_times_per_day];
    }
  
  
  // Open yesterdays int_obs file
  int yest_obs_flag = 1;
  NcFile yesterdays_obs_file(yesterdays_obs_file_name, NcFile::ReadOnly);
  if (!yesterdays_obs_file.is_valid())
    {
      proc_log->write_time("Info: Unable to open yesterdays int_obs file %s\n", yesterdays_obs_file_name);
      yest_obs_flag = 0;
    }
  else
    {
      // Get metadata from yesterdays int_obs file
      read_obs_info(&yesterdays_obs_file, &num_yesterdays_obs_times_per_day,
		    &yesterdays_obs_time, &num_yesterdays_obs_sites, 
		    &yesterdays_obs_site_list);
      
      // Create space to read in yesterdays obs data
      yesterdays_obs_data = new float[num_yesterdays_obs_sites*num_yesterdays_obs_times_per_day];
    }
  
  
  //
  // Code to get mesonet obs data from concat_meso file
  // Only if we are working with an rctm file
  //
  int s,m;
  ds *todays_meso_data = new ds;
  ds *yesterdays_meso_data = new ds;
  
  if(rctm_flag == 1)
    {
      // Get site info
      vector <site *> sites;
      if(get_site_info(sites, site_list_file)){
	proc_log->write_time("Error in get_site_info.\n");
	proc_log->write_time("Ending. Exit status = 1.\n");
	exit (1);
      }
  
      // Process today's meso data
      //
      double todays_start_time = forc_gen_time;
      double todays_end_time =  todays_start_time + (23 *3600);
      int todays_num_out_times = (int)((todays_end_time - todays_start_time) / 3600) + 1;
  
      proc_log->write_time(1, "Info: Processing today's meso data.\n");
      ret = proc_meso_data(todays_start_time, todays_num_out_times, sites, concat_meso_file, todays_meso_data);
      if(ret){
	proc_log->write_time("Error in proc_meso_data for today's meso data.\n");
	proc_log->write_time("Ending. Exit status = 1.\n");
	return(1);
      }
  
      // Process yesterday's meso data
      //
      yest_obs_flag = 1;
      double yesterdays_start_time = forc_gen_time - (24 * 3600);
      double yesterdays_end_time =  yesterdays_start_time + (23 *3600);
      int yesterdays_num_out_times = (int)((yesterdays_end_time - yesterdays_start_time) / 3600) + 1;
  
      proc_log->write_time(1, "Info: Processing yesterday's meso data.\n");
      ret = proc_meso_data(yesterdays_start_time, yesterdays_num_out_times, sites, concat_meso_file, yesterdays_meso_data);
      if(ret){
	proc_log->write_time("Error in proc_meso_data for yesterday's meso data.\n");
	proc_log->write_time("Ending. Exit status = 1.\n");
	return(1);
      }
    }
  
  // Find Julian Day
  clock = (time_t)(forc_time);
  t = gmtime(&clock);

  int num_fcst_vars = conf_params.fec_vars_n;

  // Now start main loop
  for (np = 0; np < num_fcst_vars; np++)
    {
      //printf("Working on variable index = %d, variable name = %s\n", np, conf_params._fec_vars[np].var_name);
      
      // Read input forecast
      read_fcst_data(&input_file, conf_params._fec_vars[np].var_name,
		     &times_per_day, fcst_data);
      
      // Read obs data
      //
      if(rctm_flag == 1)
	{
	  if(bridge_flag == 1 && strcmp(conf_params._fec_vars[np].var_name, "road_T") == 0){
	    proc_log->write_time(3, "Info: Using bridge_T meso data for todays_obs_data\n");
	    todays_obs_data = todays_meso_data->bridge_T;
	  } else if(strcmp(conf_params._fec_vars[np].var_name, "road_T") == 0){
	    proc_log->write_time(3, "Info: Using road_T meso data for todays_obs_data\n");
	    todays_obs_data = todays_meso_data->road_T;
	  } else if(strcmp(conf_params._fec_vars[np].var_name, "subsurface_T") == 0){
	    proc_log->write_time(3, "Info: Using subsurface_T meso data for todays_obs_data\n");
	    todays_obs_data = todays_meso_data->subsurface_T;
	  } else{
	    proc_log->write_time("Error: FEC variable: %s not recognized\n", conf_params._fec_vars[np].var_name);
	    return(1);
	  }
	  
	  todays_obs_times_per_day = 24;
	  todays_obs_time = todays_meso_data->time_nominal;
	  num_todays_obs_sites = todays_meso_data->num_sites;
	  todays_obs_site_list = todays_meso_data->dicast_id;
	}
      else
	{
	  proc_log->write_time(3, "Info: Using int_obs data for todays_obs_data\n");
	  read_obs_data(&todays_obs_file, conf_params._fec_vars[np].var_name,
			&todays_obs_times_per_day, todays_obs_data);
	}
      
      // Read yesterdays obs data
      if(rctm_flag == 1)
	{
	  if(bridge_flag == 1 && strcmp(conf_params._fec_vars[np].var_name, "road_T") == 0){
	    proc_log->write_time(3, "Info: Using bridge_T meso data for yesterdays_obs_data\n");
	    yesterdays_obs_data = yesterdays_meso_data->bridge_T;
	  } else if(strcmp(conf_params._fec_vars[np].var_name, "road_T") == 0){
	    proc_log->write_time(3, "Info: Using road_T meso data for yesterdays_obs_data\n");
	    yesterdays_obs_data = yesterdays_meso_data->road_T;
	  } else if(strcmp(conf_params._fec_vars[np].var_name, "subsurface_T") == 0){
	    proc_log->write_time(3, "Info: Using subsurface_T meso data for yesterdays_obs_data\n");
	    yesterdays_obs_data = yesterdays_meso_data->subsurface_T;
	  } else {
	    proc_log->write_time("Error: FEC variable: %s not recognized\n", conf_params._fec_vars[np].var_name);
	    return(1);
	  }
	
	  yesterdays_obs_times_per_day = 24;
	  yesterdays_obs_time = yesterdays_meso_data->time_nominal;
	  num_yesterdays_obs_sites = yesterdays_meso_data->num_sites;
	  yesterdays_obs_site_list = yesterdays_meso_data->dicast_id;
	}
      else
	{
	  if(yest_obs_flag)
	    {
	      proc_log->write_time(3, "Info: Using int_obs data for yesterdays_obs_data\n");
	      read_obs_data(&yesterdays_obs_file, conf_params._fec_vars[np].var_name,
			    &yesterdays_obs_times_per_day, yesterdays_obs_data);
	    }
	}
      
      for (i=0; i<max_output_sites*num_output_days*num_output_times_per_day; i++)
	output_data[i] = NC_FILL_FLOAT;

      for (ns = 0; ns < num_output_sites; ns++)
	{
	  proc_log->write_time(2, "Info: working on variable %s, site %d\n", conf_params._fec_vars[np].var_name, output_site_array[ns]); 
	  
	  // See if site is in input forecast file
	  if (index_array[ns] < 0)
	    continue;

	  if (times_per_day == 1)
	    {
	      // Don't correct daily vars
	      for (nd = 0; nd < num_output_days; nd++)
		{
		  int input_index = index_array[ns] * num_input_day + nd;
		  int output_index = ns * num_output_days + nd;
		  output_data[output_index] = fcst_data[input_index];
		}
	      continue;
	    }

	  // Read this sites nbrs
	  ret = get_nbr_list(output_site_array[ns], nbr_list, nbr_site_list,
			     num_nbr_sites, num_nbr, nbr, nbr_dist);
	  if (ret != 0)
	    {
	      // !found
	      continue;
	    }
	  
	  //
	  // Dynamically set the number of correction hours and correction factors (weights)
	  // based on the next inflection (max/min) in the forecast timeseries
	  //
	  
	  // Get fcst data for the current site
	  float *site_fcst_data = NULL;
	  ret =  get_site_forecast_data(index_array[ns], fcst_data, num_input_day, times_per_day, site_fcst_data);
	  
	  if(site_fcst_data != NULL){
	    for(i = 0; i < num_input_day*times_per_day; i++){
	      proc_log->write_time(4, "Info: site_fcst_data[%d] = %.2f\n", i, site_fcst_data[i]);
	    }
	  }
	  
	  // Find the closest inflection (max/min) point
	  int num_fcst_vals_per_site = num_input_day*times_per_day;
	  int fcst_inflc_ind = find_inflection_ind(corr_hr, num_fcst_vals_per_site, site_fcst_data, 0);
	  proc_log->write_time(3, "Info: fcst_inflc_ind = %d\n", fcst_inflc_ind);
	  
	  // Set the number of correction hours
	  int min_corr_hrs = conf_params._fec_vars[np].min_corr_hrs;
	  int max_corr_hrs = conf_params._fec_vars[np].max_corr_hrs;
	  int num_corr_hrs = (fcst_inflc_ind - corr_hr) + 1;
	  if(num_corr_hrs < min_corr_hrs){
	    num_corr_hrs = min_corr_hrs;
	  } else if(num_corr_hrs > max_corr_hrs){
	    num_corr_hrs = max_corr_hrs;
	  }
	  
	  // Limit the number of correction hours for the pop variables
	  if((strcmp(conf_params._fec_vars[np].var_name, "prob_precip01") == 0) ||
	     (strcmp(conf_params._fec_vars[np].var_name, "prob_precip03") == 0) ||
	     (strcmp(conf_params._fec_vars[np].var_name, "prob_precip06") == 0))
	    {
	      num_corr_hrs = min_corr_hrs;
	    }
	  proc_log->write_time(3, "Info: new_num_corr = %d\n", num_corr_hrs);
	  
	  // Set up correction factor array
	  int num_wgt_values = num_corr_hrs;
	  float wgt_incr = 1 / (float)num_wgt_values;
	  //cout << "wgt_incr = " << wgt_incr << endl;
	  
	  float *corr_factors = new float[num_wgt_values];
	  for(i = 0; i < num_wgt_values; i++){
	    float wgt_factor = (float)i * wgt_incr;
	    float wgt_value = 1 - wgt_factor;
	    corr_factors[i] = wgt_value;
	    proc_log->write_time(4, "Info: corr_factors[%d]: %4.2f\n", i, corr_factors[i]);
	  }
	  
	  // Set up correction factor array for no correction case
	  float *no_corr_factors =  new float[num_wgt_values];
	  for(i = 0; i < num_wgt_values; i++){
	    no_corr_factors[i] = 0.0;
	  }
	  
	  // Get obs data for the current site
	  float *todays_site_obs_data = NULL;
	  ret =  get_site_obs_data(output_site_array[ns], num_todays_obs_sites, todays_obs_site_list, 
				   todays_obs_data, todays_obs_times_per_day, todays_site_obs_data);
	  if(ret != 0){
	    proc_log->write_time(2, "Info: No obs found in todays obs file for site %d\n", output_site_array[ns]);
	  }
	  
	  // Get yesterdays obs data for the current site
	  float *yesterdays_site_obs_data = NULL;
	  if(yest_obs_flag){
	    ret =  get_site_obs_data(output_site_array[ns], num_yesterdays_obs_sites, yesterdays_obs_site_list, 
				     yesterdays_obs_data, yesterdays_obs_times_per_day, yesterdays_site_obs_data);
	    if(ret != 0){
	      proc_log->write_time(2, "Info: No obs found in yesterdays obs file for site %d\n", output_site_array[ns]);
	    }
	  }
	  
	  //
	  // Concatenate yesterdays and todays obs data
	  // or just use todays obs data if yesterdays is missing
	  //
	  float *site_all_obs_data = NULL;
	  double *all_obs_time = NULL;
	  int all_obs_times_per_day;
	  int obs_start;

	  proc_log->write_time(3, "Info: todays_obs_times_per_day = %d, todays_obs_time[0] = %d\n",  todays_obs_times_per_day, (int)todays_obs_time[0]); 
	  proc_log->write_time(3, "Info: yest_obs_flag = %d\n",  yest_obs_flag); 
	  if(yest_obs_flag == 1){
	    proc_log->write_time(3, "Info: yesterdays_obs_times_per_day = %d, yesterdays_obs_time[0] = %d\n",  yesterdays_obs_times_per_day, (int)yesterdays_obs_time[0]); 
	  }

	  if(todays_site_obs_data != NULL && yest_obs_flag == 1){
	    ret = concat_obs_data(todays_site_obs_data, yesterdays_site_obs_data, site_all_obs_data, todays_obs_time, todays_obs_times_per_day, yesterdays_obs_time, yesterdays_obs_times_per_day, all_obs_time);
	    all_obs_times_per_day =  yesterdays_obs_times_per_day+todays_obs_times_per_day;  
	  } else{
	    site_all_obs_data = todays_site_obs_data;
	    all_obs_time = todays_obs_time;
	    all_obs_times_per_day = todays_obs_times_per_day;
	  }
	    
	  // Find the index in the obs array assocaited with the forc_corr_time
	  if(site_all_obs_data != NULL){
	    for(i = 0; i < all_obs_times_per_day; i++){
	      proc_log->write_time(4, "Info: all_obs_time[%d] = %d, site_all_obs_data[%d] = %.2f\n", i, (int)all_obs_time[i], i,  site_all_obs_data[i]); 
	      if(all_obs_time[i] == forc_corr_time){
		obs_start = i;
	      }
	    }
	  }
	  proc_log->write_time(3, "Info: obs_start = %d\n", obs_start);
	  
	  // Determine whether to do point obs or spatial obs
	  int obs_type = (int)conf_params._fec_vars[np].obs_gen_type;
	  if(obs_type == (int)conf_params.POINT_OBS)
	    proc_log->write_time(2, "Info: initial obs_type is POINT_OBS\n");
	  else
	    proc_log->write_time(2, "Info: initial obs_type is SPATIAL_OBS\n");
	  
	  // Do not do any possible interpolation for irradiance variables
	  // For the rest of the variables we will possibly do interpolation so set flag to 1 initially
	  int do_interp = 1;
	  if((strcmp(conf_params._fec_vars[np].var_name, "av_dswrf_sfc") == 0) ||
	     (strcmp(conf_params._fec_vars[np].var_name, "av_PAI") == 0))
	    {
	      do_interp = 0;
	    }
	  proc_log->write_time(3, "Info: do_interp = %d\n", do_interp);
	  
	  //
	  // If the obs_type is POINT_OBS try and get an interpolated value
          // If there is an actual ob at the current hour (obs_start) the
	  // function will just return that value (not an interpolated value)
	  //  
	  float interp_obs_val = NC_FILL_FLOAT;
	  if(site_all_obs_data != NULL){  
	    if(obs_type == (int)conf_params.POINT_OBS){
	      interp_obs_val = get_interp_obs_val(site_all_obs_data, obs_start, site_fcst_data, corr_hr, do_interp);
	      site_all_obs_data[obs_start] = interp_obs_val;
	    }
	  }
	  
	  float error_sum = 0.;
	  float error_wgt_sum = 0;
	  float this_obs_sum = 0;
	  float this_obs_wgt_sum = 0;

	  // Get this hour's forecast
	  get_forecast(index_array[ns], fcst_data, num_input_day,
		       times_per_day, forc_time,
		       forc_corr_time, &fcst_val);
	  //printf("ns %d  ind %d   fcst_val %.3f\n", ns, index_array[ns], fcst_val);
	  
	  // If no obs at site, do spatial obs
	  // Here that means this site not in nbr list
	  // Only look for spatial obs if do_interp is set to 1
	  // (we don't want spatial obs for the irradiance variables)
	  if (obs_type == (int)conf_params.POINT_OBS && do_interp == 1)
	    {
	      find_nbr_obs(num_todays_obs_sites, todays_obs_site_list, 
			   todays_obs_data, todays_obs_times_per_day,
			   nbr[0], todays_obs_time,
			   forc_corr_time, &nbr_obs);
	      if (nbr[0] != output_site_array[ns] || nbr_obs == NC_FILL_FLOAT)
		{
		  //printf("nbr[0] = %d, site[%d] =  %d, nbr_obs = %f\n", nbr[0], ns, output_site_array[ns], nbr_obs);
		  obs_type = (int)conf_params.SPATIAL_OBS;
		}
	    }
	  
	  if(obs_type == (int)conf_params.POINT_OBS)
	    proc_log->write_time(2, "Info: updated obs_type is POINT_OBS\n");
	  else if(obs_type == (int)conf_params.SPATIAL_OBS && interp_obs_val != NC_FILL_FLOAT)
	    proc_log->write_time(2, "Info: updated obs_type is POINT_OBS\n");
	  else
	    proc_log->write_time(2, "Info: updated obs_type is SPATIAL_OBS\n");
	  
	  
	  int allowed_nbr = num_nbr;
	  if (obs_type == (int)conf_params.POINT_OBS)
	    allowed_nbr = 1;
	  
	  // Only look for neighbor obs if we are doing weather fec for standard variables (do_interp set to 1)
	  // Do not try to find nbr obs for rctm fec or irradiance fec (do_interp set to 0)
	  //
	  if(rctm_flag == 0 && do_interp == 1)
	    {
	      int nn = 0;
	      while (nn < allowed_nbr && nbr[nn] >= 0 && nbr_dist[nn] < D)
		{
		  // Find nbr's altitude
		  if (NC_FILL_FLOAT != (nbr_alt = get_alt(nbr[nn], output_site_array, num_output_sites, alt)))
		    {
		      if (fabs(nbr_alt - alt[ns]) < MAX_ALT_DIFF)
			{
			  // Read this sites nbr's obs
			  find_nbr_obs(num_todays_obs_sites, todays_obs_site_list, 
				       todays_obs_data, todays_obs_times_per_day,
				       nbr[nn], todays_obs_time,
				       forc_corr_time, &nbr_obs);
			  if (nbr_obs != NC_FILL_FLOAT)
			    {
			      float wgt = exp(-nbr_dist[nn]/L);
			      this_obs_sum += wgt * nbr_obs;
			      this_obs_wgt_sum += wgt;
			      /*
				printf("nn %d  nbr %d  dist %.1f  obs %.1f  obs_approx %.1f\n", 
				nn, nbr[nn], 
				nbr_dist[nn], nbr_obs, this_obs_sum/this_obs_wgt_sum);
			      */
			    }
			}
		    }
		  nn++;
		}
	    }

	  // Set the obs value and calculate the error sum
	  //
	  // Use the interpolated obs value if it is non-missing. 
          // This indicates that the obs type is POINT_OBS but 
	  // we were able to come up with an actual or interpolated value
	  //
	  if ( (this_obs_wgt_sum  > 0 || interp_obs_val != NC_FILL_FLOAT) && fcst_val != NC_FILL_FLOAT )
	    {
	      if(interp_obs_val != NC_FILL_FLOAT){
		obs_val = interp_obs_val;
		proc_log->write_time(2, "Info: for site %d, using actual or interpolated obs_val %.2f\n", output_site_array[ns], obs_val);
	      } else{
		obs_val = this_obs_sum /  this_obs_wgt_sum;
	      }
	      
	      error_sum += (obs_val - fcst_val);
	      error_wgt_sum ++;
	      
	      proc_log->write_time(2, "Info: fcst_val %.2f   obs_val %.2f   error_sum %.2f\n", fcst_val, obs_val, error_sum);     
	      
	    }
	  
	  if (error_wgt_sum > 0.)
	    fcst_error =  error_sum / error_wgt_sum;
	  else
	    fcst_error = NC_FILL_FLOAT;

	  proc_log->write_time(2, "Info: error_sum %.2f  error_wgt_sum %.2f  fcst_val %.2f\n", error_sum, error_wgt_sum, fcst_val);
	  proc_log->write_time(2, "Info: fcst_error = %.2f\n", fcst_error);
	  
	  // Only fec pop values if the correction (fcst_error) is postive
	  // This prevents minimizing any pop values
	  if((strcmp(conf_params._fec_vars[np].var_name, "prob_precip01") == 0) ||
	     (strcmp(conf_params._fec_vars[np].var_name, "prob_precip03") == 0) ||
	     (strcmp(conf_params._fec_vars[np].var_name, "prob_precip06") == 0))
	    {
	      if (fcst_error < 0.)
		fcst_error = NC_FILL_FLOAT;
	    }
	  	  
	  if (fcst_error != NC_FILL_FLOAT)
	    {
	      set_dynamic_forecast(ns, index_array[ns], output_data, 
				   corr_factors, num_corr_hrs, fcst_data, num_input_day,
				   times_per_day, forc_time, forc_corr_time, fcst_error);
	      
	    }
	  else     // Leave Forecast Unchanged
	    {
	      proc_log->write_time(2, "Info: leaving forecast unchanged\n");
	      
	      set_dynamic_forecast(ns, index_array[ns], output_data, 
				   no_corr_factors, num_corr_hrs, fcst_data, num_input_day,
				   times_per_day, forc_time, forc_corr_time, fcst_error);
	    }
	  
	  proc_log->write_time(2, "\n");
	  
	  // Clean up
	  delete [] site_fcst_data;
	  delete [] corr_factors;
	  delete [] no_corr_factors;
	  
	  if(todays_site_obs_data != NULL)
	    delete [] todays_site_obs_data;
	  
	  if(todays_site_obs_data != NULL && yesterdays_site_obs_data != NULL){
	    delete [] yesterdays_site_obs_data;
	    delete [] site_all_obs_data;
	    delete [] all_obs_time;
	  }
	  
	} // end loop over sites
      
      write_mod_fcst(&output_file, conf_params._fec_vars[np].var_name,
		     output_data);
      
    } // end loop over variables

  // Clean up
  delete [] index_array;
  delete [] alt;
  delete [] output_site_array;
  delete [] output_data;
  delete [] nbr;
  delete [] nbr_dist;
  delete [] nbr_list;
  delete [] nbr_site_list;
  
  delete [] fcst_data;
    
  delete [] todays_obs_data;
  delete [] todays_obs_time;
  delete [] todays_obs_site_list;
  
  if(yest_obs_flag){
    delete [] yesterdays_obs_data;
    delete [] yesterdays_obs_time;
    delete [] yesterdays_obs_site_list;
  }

  delete ncErr;

  return(0);
}

int proc_meso_data(double start_time, int num_out_times, vector<site *> sites, char *meso_file, ds *meso_data)
{
  int s, m;
  int ret;
  
  // Allocate space for meso data struct
  //proc_log->write_time(1, "Allocating space for meso_data\n");
  ret = alloc_data(meso_data, sites, num_out_times);
  if(ret){
    proc_log->write_time("Error in alloc_data for meso_data\n");
    proc_log->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  // Get meso data  
  //proc_log->write_time(1, "Getting meso data.\n");
  ret = get_data(start_time, num_out_times, sites, meso_file, meso_data);
  if(ret){
    proc_log->write_time("Error in get_data for %s.\n", meso_file);
    proc_log->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }

  // Set output time_nominal
  double utime = start_time;
  for(int m = 0; m < num_out_times; m++){
    meso_data->time_nominal[m] = utime;
    utime = utime + 3600;
  }
  
  // Set non-finite values (such as nan's) to missing
  //
  int size = meso_data->num_sites * num_out_times;

  NCF_finite(meso_data->road_T, size, MISSING);
  NCF_finite(meso_data->bridge_T, size, MISSING);
  NCF_finite(meso_data->subsurface_T, size, MISSING);

  return(0);
}
