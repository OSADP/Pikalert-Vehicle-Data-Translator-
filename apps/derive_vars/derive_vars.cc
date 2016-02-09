// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: derive_vars.cc
 *
 * Author: Bill Myers, Jim Cowie, Seth Linden
 *
 * Date:   10/5/02
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

#include "netcdf.h"
#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"

#include "Params.hh"
#include "precip_type.hh"
#include "ConcatData.hh"
#include "SnowCalc.hh"
#include "ObsFile.hh"

// Constant, macro and type definitions 
#define MM_PER_INCH (25.3)
#define MPH_PER_MS (2.237)
const float SNOW_TO_SWE_RATIO = 10.; // "Idealized" snow
const int   MIN_PRINT_LEVEL   = 8;

const int VAISALA_PRECIP_NONE = 0;
const int VAISALA_PRECIP_LIGHT_RAIN = 1;
const int VAISALA_PRECIP_MEDIUM_RAIN = 2;
const int VAISALA_PRECIP_HEAVY_RAIN = 3;
const int VAISALA_PRECIP_LIGHT_SNOW = 4;
const int VAISALA_PRECIP_MEDIUM_SNOW = 5;
const int VAISALA_PRECIP_HEAVY_SNOW = 6;

const int DEF_MIN_HR = 5;  // 5am, this is the default min-T hour
const int DEF_MAX_HR = 16; // 4pm, this is the default max-T hour

const float DEFAULT_GUST_FACTOR = 2.0; // Default gust factor is it cannot be otherwise estimated
const float MAX_GUST_FACTOR = 4.0;     // Guess at what maximum gust factor might be

// Global variables 
extern Log *proc_log;
extern Params conf_params;


// Functions and objects
extern int read_fcst_info(NcFile *input_file, int *num_input_sites,
			  double *forc_time, long *num_input_times_per_day,
			  long *num_input_day, long *max_site_num);
extern int read_fcst_data(NcFile *input_file, const char *var_name,
			  long *times_per_day, float *data);
extern int create_output_file(char *cdl_name, char * output_name);
extern int write_header(NcFile *output_file, double forc_time,
			int num_output_sites, int *max_output_sites,
			long  *output_site_array, long *num_output_days,
			long *num_output_times_per_day);
extern int write_mod_fcst(NcFile *output_file, char *var_name,
			  float *fcst_data);
extern int find_precip_type(float T, float qpf, float prob_precip,
			    float cprob_rain, float cprob_snow, float cprob_ice);
extern float calc_snow_to_swe_ratio(float T, float wspd);
extern int calc_gust_ratio(float prev_wspd, float next_wspd, float max_gust, float *gust_factor);

extern void calc_dirspd(float U, float V, float *dir, float *spd);
extern float calc_rh(float temp, float dewpt);
extern float calc_mixr(float temp, float dewpt);
extern float calc_wet_bulb_temp(float press, float temp, float dewpt);
extern void init_spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
extern double calc_splint(double *xa, double *ya, double *y2a, int n, double x,
			  double FILL_VALUE);
extern float calc_pop_rap1(float conv_precip, float tot_precip, float mean_rh, int algo_hr);
extern float calc_pof_rap1_2(float rh_per);


// Local functions

// Used to bring in obs for Metx max-min calculation;
// and are separate but similar to the functions used
// to get the obs in the blowing snow algorithm
//
int readObsFiles(long maxHist, double forecastTime, char* obsConfigFile, int* siteIdArray, int numSites, vector< ObsFile* > &obsFiles);

float* fillInArray(float* fcstData, int numOutputSites, int fcstDays, int orig_fcstTimesPerDay, int &fcstTimesPerDay);

float* concatVarData(vector< ObsFile* > obsFiles, const string& obsVarName, int numOutputSites,  float *fcstData,  double forecastTime, int fcstDays, int fcstTimesPerDay, int& numVals,  int& numObsVals);
void utime2day(time_t utime, char* &day);
void utime2hour(time_t utime, char* &hour);

// Function to derive air density from moist air calculation
float calc_air_density(float T, float rh, float P_sfc);

inline float celsius2faren(float temp_cel) {return (32 + 1.8*temp_cel);};


int derive_vars(double forc_start_time, char *site_list_file,
		char *cdl_name, char *obs_config_file, 
                char *input_file_name, char *output_name,
                int debug_level)
{
  int num_input_sites, num_output_sites;
  int  *output_site_array = 0;
  long *index_array = 0; 
  int num_fcst_vars, num_derived_vars;
  double forc_time;
  char *fcst_hour_str;
  long num_input_times_per_day, num_input_day, max_input_sites;
  long num_output_times_per_day, num_output_days, max_output_sites;
  long times_per_day;
  float *fcst_data;
  float *output_data = NULL;
  int out_array_size = 0;
  double forc_start_hour;
  float *output_ll_array;

  int ret;
  int np, ns, nd, nt;

  // Set up behaviour of netCDF library
  NcError* ncErr = new NcError(NcError::silent_nonfatal);

  // Make sure times are at the top of the hour
  forc_start_hour = SEC_PER_HOUR * (int)(forc_start_time/SEC_PER_HOUR);
  forc_start_time = SEC_PER_DAY * (int)(forc_start_time/SEC_PER_DAY);

  // Open and read input  netcdf file
  NcFile input_file(input_file_name, NcFile::ReadOnly);
  if (!input_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open integrator input file %s\n",input_file_name);
      return(1);
    }

  ret = read_fcst_info(&input_file, &num_input_sites, &forc_time,
		       &num_input_times_per_day, &num_input_day,
		       &max_input_sites);
  if (ret != 0)
    {
      proc_log->write_time("Error: Unable to read fcst file\n");
      return(1);
    }
  int forc_start_index = (forc_start_hour - forc_time) / SEC_PER_HOUR;

  // Create space to read in input wx data
  fcst_data = new float[max_input_sites*num_input_day*num_input_times_per_day];

  /* generate output netcdf file from cdl_file */
  ret = create_output_file(cdl_name, output_name);

  /* open netcdf file for writing */
  NcFile output_file(output_name, NcFile::Write);
  if (!output_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open output file %s\n",output_name);
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

  // extract lat-lon array for sites
  ret = output_sites.make_latlon_array(&output_ll_array);

  // Write output file header info
  ret = write_header(&output_file, forc_start_time, num_output_sites, 
		     output_site_array, &max_output_sites, &num_output_days,
		     &num_output_times_per_day);

  if (num_output_sites > max_output_sites)
    {
      proc_log->write_time("Error: too many sites for output cdl\n");
      return(1);
    }

  output_data = new float[max_output_sites * num_output_days * num_output_times_per_day];
  
  if (num_input_times_per_day != num_output_times_per_day)
    {
      proc_log->write_time("Error: input and output file times-per_day must agree\n");
      return(1);
    }
  
  // Now start main loop
  num_fcst_vars = conf_params.fcst_var_names_n;
  for (np = 0; np < num_fcst_vars; np++)
    {
      // Read input forecast
      ret = read_fcst_data(&input_file, conf_params._fcst_var_names[np],
			   &times_per_day, fcst_data);
      if (ret != 0)
	{
	  proc_log->write_time("Error: Unable to read data from fcst file\n");
	  return(1);
	}
 
      for (nt=0; nt<max_output_sites*num_output_days*times_per_day; nt++)
	output_data[nt] = NC_FILL_FLOAT;

      // Do copy over data
      for (ns = 0; ns < num_output_sites; ns++)
	{
	  // See if site is in input forecast file
	  if (index_array[ns] < 0)
	    continue;

	  for (nd=0; nd < num_output_days; nd++)
	    for (nt=0; nt < times_per_day; nt++)
	      {
		int output_index = ns * num_output_days * times_per_day +
		  nd*times_per_day + nt;

		// num input and output times per day must be the same
		int time_diff_secs = (int)(forc_start_time - forc_time);
		int time_diff_periods = time_diff_secs / (SEC_PER_DAY / num_input_times_per_day);

		// copy over data with appropriate time offset
		if (nd * times_per_day + nt + time_diff_periods >= 0 &&
		    nd * times_per_day + nt + time_diff_periods < num_output_days*times_per_day)
		  {
		    int input_index = 
		      index_array[ns] * num_input_day*times_per_day + 
		      nd * times_per_day + nt + time_diff_periods;

		    output_data[output_index] = fcst_data[input_index];
		  }
	      }

	}

      write_mod_fcst(&output_file, conf_params._fcst_var_names[np],
		     output_data);
    }

  // Now derive vars
  proc_log->write_time(1,"Info: Processing derived variables...\n");
  num_derived_vars = conf_params.derived_var_names_n;

  // Calc hub hgt wind dir
  int found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "hub_hgt_wind_dir"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "hub_hgt_wind_u"))
	  found = 1;

      if (found)
	{
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "hub_hgt_wind_v"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst vars %s %s\n", 
			       "hub_hgt_wind_dir", "hub_hgt_wind_u", "hub_hgt_wind_v");
	}
      else
	{
	  // Allocate space for input vars
	  float *u = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *v = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *wind_dir = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float wind_speed;

	  ret = read_fcst_data(&output_file, "hub_hgt_wind_u", &times_per_day, u);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "hub_hgt_wind_u");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, "hub_hgt_wind_v", &times_per_day, v);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "hub_hgt_wind_v");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      wind_dir[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      if (u[i] != NC_FILL_FLOAT && v[i] != NC_FILL_FLOAT)
		{
		  calc_dirspd(u[i], v[i], &wind_dir[i], &wind_speed);
		}
	    }

	  write_mod_fcst(&output_file, (char *)"hub_hgt_wind_dir", wind_dir);
	  delete [] u;
	  delete [] v;
	  delete [] wind_dir;
	}
    }

  // Calc wind dir
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "wind_dir"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "wind_u"))
	  found = 1;

      if (found)
	{
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "wind_v"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst vars %s %s\n", 
			       "wind_dir", "wind_u", "wind_v");
	}
      else
	{
	  // Allocate space for input vars
	  float *u = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *v = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *wind_dir = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float wind_speed;

	  ret = read_fcst_data(&output_file, (char *)"wind_u", &times_per_day, u);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_u");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"wind_v", &times_per_day, v);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_v");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      wind_dir[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      if (u[i] != NC_FILL_FLOAT && v[i] != NC_FILL_FLOAT)
		{
		  calc_dirspd(u[i], v[i], &wind_dir[i], &wind_speed);
		}
	    }

	  write_mod_fcst(&output_file, (char *)"wind_dir", wind_dir);
	  delete [] u;
	  delete [] v;
	  delete [] wind_dir;
	}
    }

  // Calc wind speed
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "wind_speed"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "wind_u"))
	  found = 1;

      if (found)
	{
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "wind_v"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst vars %s %s\n", 
			       "wind_speed", "wind_u", "wind_v");
	}
      else
	{
	  // Allocate space for input vars
	  float *u = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *v = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *wind_speed = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float wind_dir;

	  ret = read_fcst_data(&output_file, (char *)"wind_u", &times_per_day, u);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_u");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"wind_v", &times_per_day, v);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_v");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      wind_speed[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      if (u[i] != NC_FILL_FLOAT && v[i] != NC_FILL_FLOAT)
		{
		  calc_dirspd(u[i], v[i], &wind_dir, &wind_speed[i]);
		}
	    }

	  write_mod_fcst(&output_file, (char *)"wind_speed", wind_speed);
	  delete [] u;
	  delete [] v;
	  delete [] wind_speed;
	}
    }

  // Calc wind gust
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "gust_spd"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "ecmwf_max_gust"))
	  found = 1;

      if (found)
	{
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "ecmwf_wspd"))
	      found = 1;
	}

      if (found)
	{
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "wind_speed"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst vars %s %s %s\n", 
			       "gust_spd", "ecmwf_max_gust", "ecmwf_wspd", "wind_speed");
	}
      else
	{
	  // Allocate space for input vars
	  float *max_gust = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *ref_wspd = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *wspd = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *gust_spd = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float gust_factor;

	  ret = read_fcst_data(&output_file, (char *)"ecmwf_max_gust", &times_per_day, max_gust);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "ecmwf_max_gust");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"ecmwf_wspd", &times_per_day, ref_wspd);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "ecmwf_wspd");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"wind_speed", &times_per_day, wspd);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_speed");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      gust_spd[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      int hr, prev_3hr, next_3hr, offset;
	      hr = i % (num_output_days*num_output_times_per_day);
	      if (hr % 3 == 0)
		{
		  next_3hr = hr;
		  prev_3hr = next_3hr - 3;
		}
	      else
		{
		  prev_3hr = hr - (hr % 3);
		  next_3hr = prev_3hr + 3;
		}
	      offset = hr - prev_3hr; 
	      gust_factor = DEFAULT_GUST_FACTOR;

	      if (prev_3hr >= 0 && next_3hr < num_output_days*num_output_times_per_day &&
		  max_gust[i - offset + 3] != NC_FILL_FLOAT &&
		  ref_wspd[i - offset] != NC_FILL_FLOAT && ref_wspd[i - offset + 3] != NC_FILL_FLOAT &&
		  wspd[i] != NC_FILL_FLOAT)
		{
		  ret = calc_gust_ratio(ref_wspd[i - offset], ref_wspd[i - offset + 3], max_gust[i - offset + 3], &gust_factor);
		  if (ret != 0 || gust_factor < 1. || ref_wspd[i - offset] == NC_FILL_FLOAT || 
		      ref_wspd[i - offset + 3] == NC_FILL_FLOAT || max_gust[i - offset + 3] == NC_FILL_FLOAT)
		    gust_factor = DEFAULT_GUST_FACTOR;
		  else if (wspd[i] > 10. && wspd[i] >= ref_wspd[i])
		    {
		      if (gust_factor > MAX_GUST_FACTOR)
			gust_factor = MAX_GUST_FACTOR;

		      /*
		      if ((int)(i/(num_output_days*num_output_times_per_day) == 1567))
			printf("%d  wspd %.2f  ref_wspd[i] %.2f  gust factor %.2f  %.2f\n", i, wspd[i], ref_wspd[i], gust_factor,  
			gust_factor + (1.5 - gust_factor) / (25. - ref_wspd[i]) * (wspd[i] - ref_wspd[i]) );
		      */

		      gust_factor = gust_factor + (1.5 - gust_factor) / (25. - ref_wspd[i]) * (wspd[i] - ref_wspd[i]) ;
		      //gust_factor = 1.5 - (gust_factor - 1.5) * (wspd[i] - 25.) / (ref_wspd[i] - 25.);
		    }
		  else if (gust_factor > MAX_GUST_FACTOR)
                    gust_factor = MAX_GUST_FACTOR;

		  else if (gust_factor > MAX_GUST_FACTOR)
		    gust_factor = MAX_GUST_FACTOR;
		  else if (wspd[i] > 25.)
		    gust_factor = 1.5;
		}
	      /*
	      if ((int)(i/(num_output_days*num_output_times_per_day) == 1567))
		printf("i %d  hr %d  prev %d  next %d   max_gust %.1f  ref_wspd %f %f  gust_factor %.2f\n", i, hr, prev_3hr, next_3hr,
		max_gust[i - offset + 3], ref_wspd[i - offset], ref_wspd[i - offset + 3], gust_factor);
	      */
	      if (wspd[i] != NC_FILL_FLOAT)
		gust_spd[i] = gust_factor * wspd[i];
	      else
		gust_spd[i] = NC_FILL_FLOAT;


	    }

	  write_mod_fcst(&output_file, (char *)"gust_spd", gust_spd);
	  delete [] max_gust;
	  delete [] ref_wspd;
	  delete [] wspd;
	  delete [] gust_spd;
	}
    }


  // Calc wind speed (MPH)
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "wind_speed_mph"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "wind_speed"))
	  found = 1;

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", "wind_speed_mph", "wind_speed");
	}
      else
	{
	  // Allocate space for input vars
	  float *wind_speed = new float[max_output_sites*num_output_days*num_output_times_per_day];
      
	  // Allocate space for output vars
	  float *wind_speed_mph = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"wind_speed", &times_per_day, wind_speed);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_speed");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      wind_speed_mph[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    if (wind_speed[i] != NC_FILL_FLOAT)
	      wind_speed_mph[i] = wind_speed[i] * MPH_PER_MS;

	  write_mod_fcst(&output_file, (char *)"wind_speed_mph", wind_speed_mph);
	  delete [] wind_speed;
	  delete [] wind_speed_mph;
	}
    }

  // Calc cloud cover (OKTA)
  found = 0;
  for (np=0; np<num_derived_vars; np++)
      if (0 == strcmp(conf_params._derived_var_names[np], "cloud_cov_okta"))
	found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "cloud_cov"))
	  found = 1;

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", "cloud_cov_okta", "cloud_cov");
	}
      else
	{
	  // Allocate space for input vars
	  float *cloud_cov = new float[max_output_sites*num_output_days*num_output_times_per_day];
      
	  // Allocate space for output vars
	  float *cloud_cov_okta = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"cloud_cov", &times_per_day, cloud_cov);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "cloud_cov");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      cloud_cov_okta[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    if (cloud_cov[i] != NC_FILL_FLOAT)
	      cloud_cov_okta[i] = (int)(cloud_cov[i] * 8 + 0.5);

	  write_mod_fcst(&output_file, (char *)"cloud_cov_okta", cloud_cov_okta);
	  delete [] cloud_cov;
	  delete [] cloud_cov_okta;
	}
    }

  // Calc cloud low (OKTA)
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "cloud_low_okta"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "cloud_low"))
	  found = 1;

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", "cloud_low_okta", "cloud_low");
	}
      else
	{
	  // Allocate space for input vars
	  float *cloud_low = new float[max_output_sites*num_output_days*num_output_times_per_day];
      
	  // Allocate space for output vars
	  float *cloud_low_okta = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"cloud_low", &times_per_day, cloud_low);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "cloud_low");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      cloud_low_okta[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    if (cloud_low[i] != NC_FILL_FLOAT)
	      cloud_low_okta[i] = (int)(cloud_low[i] * 8 + 0.5);

	  write_mod_fcst(&output_file, (char *)"cloud_low_okta", cloud_low_okta);
	  delete [] cloud_low;
	  delete [] cloud_low_okta;
	}
    }

  // Calc cloud middle (OKTA)
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "cloud_middle_okta"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "cloud_middle"))
	  found = 1;

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", "cloud_middle_okta", "cloud_middle");
	}
      else
	{
	  // Allocate space for input vars
	  float *cloud_middle = new float[max_output_sites*num_output_days*num_output_times_per_day];
      
	  // Allocate space for output vars
	  float *cloud_middle_okta = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"cloud_middle", &times_per_day, cloud_middle);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "cloud_middle");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      cloud_middle_okta[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    if (cloud_middle[i] != NC_FILL_FLOAT)
	      cloud_middle_okta[i] = (int)(cloud_middle[i] * 8 + 0.5);

	  write_mod_fcst(&output_file, (char *)"cloud_middle_okta", cloud_middle_okta);
	  delete [] cloud_middle;
	  delete [] cloud_middle_okta;
	}
    }

  // Calc cloud high (OKTA)
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "cloud_high_okta"))
      found = 1;

  if (found)
    {
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "cloud_high"))
	  found = 1;

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", "cloud_high_okta", "cloud_high");
	}
      else
	{
	  // Allocate space for input vars
	  float *cloud_high = new float[max_output_sites*num_output_days*num_output_times_per_day];
      
	  // Allocate space for output vars
	  float *cloud_high_okta = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"cloud_high", &times_per_day, cloud_high);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "cloud_high");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      cloud_high_okta[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    if (cloud_high[i] != NC_FILL_FLOAT)
	      cloud_high_okta[i] = (int)(cloud_high[i] * 8 + 0.5);

	  write_mod_fcst(&output_file, (char *)"cloud_high_okta", cloud_high_okta);
	  delete [] cloud_high;
	  delete [] cloud_high_okta;
	}
    }

  // Calc Metx max-min or local max-in (they are the same
  // Note that the variable names have Metx in them even though the data can
  // be written to either Metx max-min or local max-min
  //
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "Metx_max_T") ||
        0 == strcmp(conf_params._derived_var_names[np], "Metx_min_T") ||
	0 == strcmp(conf_params._derived_var_names[np], "local_max_T") ||
        0 == strcmp(conf_params._derived_var_names[np], "local_min_T"))
      found = 1;

  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      if (found)
	{
	  // See if required core vars are available
	  // check required core vars are here
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "max_T"))
	      found = 1;
	}

      if (found)
	{
	  // See if required core vars are available
	  // check required core vars are here
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "min_T"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s, %s or %s, %s without fcst vars %s, %s, and %s\n", 
			       "Metx_max_T", "local_max_T", "Metx_min_T", "local_min_T", "T", "max_T", "min_T");
	}
      else
	{
	  long int daily_times_per_day = 1;
	  
	  // Allocate space for input vars
	  float *T_orig = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *max_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *min_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  
	  // Allocate space for output vars
	  float *Metx_max_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *Metx_min_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *Metx_max_T_hr = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *Metx_min_T_hr = new float[max_output_sites*num_output_days*daily_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T_orig);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"max_T", &daily_times_per_day, max_T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "max_T");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"min_T", &daily_times_per_day, min_T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "min_T");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * daily_times_per_day;
	  
	  //
	  // Fill-in the fcst data if fc_times_per_day is less than 24
	  // use cubic spline to fill in missing values (this is primarily for LT data)
	  //
	  float *T_fcst = NULL;
	  if(times_per_day < 24){
	    proc_log->write_time(5, "Info: In Metx max/min: times_per_day: %d is less than 24, filling in T_fcst array\n", times_per_day);
	    int new_times_per_day;
	    T_fcst = fillInArray(T_orig, num_output_sites, num_output_days, times_per_day, new_times_per_day);
	    times_per_day = new_times_per_day;
	    num_output_times_per_day = new_times_per_day;
	  } else{
	    proc_log->write_time(5, "Info: In Metx max/min: times_per_day: %d is equal to 24, copying T_orig to T_fcst\n", times_per_day);
	    T_fcst = T_orig;
	  }
	  
	  //
	  //Read in all of the obs files from the obs_config_file
	  //
	  vector< ObsFile* > obsFiles;
	  ret =  readObsFiles(conf_params.max_history, forc_start_hour, obs_config_file, output_site_array, num_output_sites, obsFiles);
	  if(ret != 0){
	    proc_log->write_time("Warning: unable to read obs data from %s\n", obs_config_file);
	    //return(1);
	  }
	  
	  //
	  // Concatenate the T obs with the T fcst,
	  // only execute if readObsFile succeeded
	  //
	  const string obs_var_name = "T";
	  int numConcatValsPerSite = 0;
	  int numObsValsPerSite = 0;
	  float *T = NULL;
	  
	  if(ret == 0){
	    T = concatVarData(obsFiles, obs_var_name, num_output_sites, T_fcst, forc_start_hour, num_output_days, times_per_day, numConcatValsPerSite, numObsValsPerSite);
	  } 
	  
	  // If concatVarData returns NULL just use the T fcst data
	  int obs_offset;
	  if(T == NULL){
	    proc_log->write_time("Info: concatVarData returned NULL, copying T_fcst to T\n");
	    numConcatValsPerSite = num_output_days*num_output_times_per_day;
	    numObsValsPerSite = 0;
	    obs_offset = 0;
	    T = new float[max_output_sites*numConcatValsPerSite];
	    T = T_fcst;
	  } else{
	    obs_offset = 24;
	  }
	  //cout << "obs_offset = " << obs_offset << endl;
	  
	  //
	  // For debugging only
	  // output concatenated T array
	  //
	  proc_log->write_time(3, "In Metx max/min calc: OUTPUT T ARRAY\n");
	  for(int s = 0; s < num_output_sites; s++){
	    int data_ind = s * numConcatValsPerSite;
	    proc_log->write_time(3, "s = %d, data_ind = %d\n", s, data_ind);
	    for(int t = 0; t < numConcatValsPerSite; t++){
	      proc_log->write_time(3, "T[%d] = %f\n", data_ind, T[data_ind]);
	      data_ind++;
	    }
	  }
	  
	  // Initialize the Metx max/min arrays
	  for (nt=0; nt<out_array_size; nt++)
	    {
	      Metx_max_T[nt] = NC_FILL_FLOAT;
	      Metx_min_T[nt] = NC_FILL_FLOAT;
	      Metx_max_T_hr[nt] = NC_FILL_INT;
	      Metx_min_T_hr[nt] = NC_FILL_INT;
	    }
	  
	  
	  // Get the forecast hour, used for the day 0 max/min
	  utime2hour((time_t)forc_start_hour, fcst_hour_str);
	  int fcst_hour = atoi(fcst_hour_str);
	  //printf("fcst_hour = %d\n", fcst_hour);
	  
	  for (int i = 0; i < out_array_size; i++)
	    {
	      int ns = i / ( num_output_days * daily_times_per_day);
	      //cout << "i = " << i << " ns = " << ns << endl;
	      
	      if (ns < num_output_sites)
		{
		  // Calculate local time zone offset to GMT
		  float lat = output_ll_array[2*ns];
		  float lon = output_ll_array[2*ns+1];
		  if (lon > 180.)
		    lon = lon - 360.;
		  if (lon < -180.)
		    lon = lon + 360.;
		  
		  // Add check for jogs in the dateline. Currently only
		  // handles (crudely) the Aleutians.
		  if ((lat > 50. && lat < 55.) &&
		      (lon > 170.))
		    lon = lon - 360.;

		  int time_offset = (int)(-lon/360.*24. + 0.5);
		  proc_log->write_time(5, "time_offset = %d\n", time_offset);
		  int day = i % num_output_days;
		  //cout << "day = " << day << endl;
		  
		  // Set start and end indicies for finding min temperatures in time-series
		  int min_start = (day * 24 + time_offset + conf_params.local_min_start) + obs_offset;
		  int min_end = (day * 24 + time_offset + conf_params.local_min_end) + obs_offset;
	
		  //printf("for day = %d, original min_start = %d\n", day, min_start);
		  
		  // For day 0 re-set start index to current hour if it's later than the local-day start
		  if(day == 0 && (fcst_hour + obs_offset) >= min_start){
		    min_start = fcst_hour + obs_offset;
		    proc_log->write_time(5, "for site = %d, day = %d, using forc_start_hour, min_start = %d, min_end = %d, ", ns, day, min_start, min_end-1);
		  } else{
		    proc_log->write_time(5, "for site = %d, day = %d, using local-day hour,  min_start = %d, min_end = %d, ", ns, day, min_start, min_end-1);
		  }
		  
		  //
		  // Find lowest time series "night-time" min.
		  //
		  int num_min_timeseries_vals = 0;
		  float local_min = 9999999.;
		  int local_min_nt = -1;
		  for (nt = min_start; nt < min_end; nt++)
		    {
		      if (nt >= 0 && 
			  nt < numConcatValsPerSite)
			{
			  int index = ns * numConcatValsPerSite + nt;
			  if(T[index] != NC_FILL_FLOAT)
			    num_min_timeseries_vals++;
			  if (T[index] != NC_FILL_FLOAT && T[index] < local_min){
			    local_min = T[index];
			    local_min_nt = nt;
			  }
			}
		    }
		  
		  proc_log->write(5, "num_min_timeseries_vals: %d\n", num_min_timeseries_vals );
		  
		  // If we have less than 18 values in the time-series, default to UTC min by setting local-min to missing
		  if(day > 0 && num_min_timeseries_vals < 18)
		    {
		      local_min = 9999999.;
		      proc_log->write_time(5, "Not enough timeseries values to determine local min, setting to missing.\n" );
		    }
		  
		  // Choose correct min;
		  // default to utc min if local_min is missing
		  int Metx_min_time_ind = 9999999; 
		  if (local_min != 9999999.)
		    {
		      // Use time series if min is missing
		      if (min_T[i] == NC_FILL_FLOAT)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min is missing, using timeseries-min %6.2f, hour-index %d\n", i, local_min, local_min_nt);
			  Metx_min_T[i] = local_min;
			  Metx_min_T_hr[i] = local_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min warmer than time series, use time series
		      else if (min_T[i] > local_min)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min %6.2f is warmer than timeseries-min %6.2f, using timeseries-min %6.2f, hour-index %d\n", i, min_T[i], local_min, local_min, local_min_nt);
			  Metx_min_T[i] = local_min;
			  Metx_min_T_hr[i] = local_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min colder than allowed tolerance - set to tolerance
		      else if (local_min - min_T[i] > conf_params.min_ts_tolerance)
			{
			  proc_log->write_time(4, "Info: array index %d, timeseries-min %6.2f minus UTC-min %6.2f is greater than tolerance %6.2f, using timeseries-min minus tolerance %6.2f, hour-index %d\n", i, local_min, min_T[i], conf_params.min_ts_tolerance, (local_min - conf_params.min_ts_tolerance), local_min_nt);
			  Metx_min_T[i] = local_min - conf_params.min_ts_tolerance;
			  Metx_min_T_hr[i] = local_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min is between time series and tolerance, use as-is
		      else
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min %6.2f is colder than timeseries-min %6.2f but within tolerance, using UTC-min %6.2f, hour-index %d\n", i, min_T[i], local_min, min_T[i], local_min_nt);
			  Metx_min_T[i] = min_T[i];
			  Metx_min_T_hr[i] = local_min_nt - (day * 24) - obs_offset - time_offset;
			}
		    }
		  else
		    {
		      proc_log->write_time(4, "Info: array index %d, timeseries-min is missing, using UTC-min %6.2f, hour-index 'default'\n", i, min_T[i]);
		      Metx_min_T[i] = min_T[i]; 
		      Metx_min_T_hr[i] = DEF_MIN_HR;
		    }
		  
		  
		  // Now resolve Max
		  
		  // Set start and end indicies for finding max temperatures in time-series
		  int max_start = (day * 24 + time_offset + conf_params.local_max_start) + obs_offset;
		  int max_end = (day * 24 + time_offset + conf_params.local_max_end) + obs_offset;
		  
		  //printf("for day = %d, original max_start = %d\n", day, max_start);
		  
		  // For day 0 re-set start index to current hour if it's later than the local-day start
		  if(day == 0 && (fcst_hour + obs_offset) >= max_start){
		    max_start = fcst_hour + obs_offset;
		    proc_log->write_time(5, "for site = %d, day = %d, using forc_start_hour, max_start = %d, max_end = %d, ", ns, day, max_start, max_end-1);
		  } else{
		    proc_log->write_time(5, "for site = %d, day = %d, using local-day hour,  max_start = %d, max_end = %d, ", ns, day, max_start, max_end-1);
		  }
		  
		  //
		  // Find time-series "day-time" max and when that ocurred.
                  //
		  int num_max_timeseries_vals = 0;
		  float local_max = -9999999.;
		  int local_max_nt = -1;
		  for (nt = max_start; nt < max_end; nt++)
		    {
		      if (nt >= 0 && 
			  nt < numConcatValsPerSite)
			{
			  int index = ns * numConcatValsPerSite + nt;
			  if(T[index] != NC_FILL_FLOAT)
			    num_max_timeseries_vals++;
			  if (T[index] != NC_FILL_FLOAT && T[index] >= local_max)
			    {
			      local_max = T[index];
			      local_max_nt = nt;
			    }
			}
		    }
		  
		  proc_log->write(5, "num_max_timeseries_vals: %d\n", num_max_timeseries_vals);
		  
		  // If we have less than 18 values in the time-series, default to UTC max by setting local-max to missing
		  if(day > 0 && num_max_timeseries_vals < 18)
		    {
		      local_max = -9999999.;
		      proc_log->write_time(5, "Not enough timeseries values to determine local max, setting to missing.\n" );
		    }
		  
		  // Choose correct max
		  // default to utc max if local_max is missing
		  if (local_max != -9999999.)
		    {
		      // Use time series if max is missing
		      if (max_T[i] == NC_FILL_FLOAT)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max is missing, using timeseries-max %6.2f\n", i, local_max);
			  Metx_max_T[i] = local_max;
			  Metx_max_T_hr[i] = local_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max is at hour 0 of time series, use it (cold front
		      // case - special for max only)
		      else if (local_max_nt == max_start)
			{
			  proc_log->write_time(4, "Info: array index %d, timeseries-max %6.2f is at beginning of timeseries, using timeseries-max %6.2f (ref UTC-max %6.2f), hour-index %d\n", i, local_max, local_max, max_T[i], local_max_nt);
			  Metx_max_T[i] = local_max;
			  Metx_max_T_hr[i] = local_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max colder than time series, use time series
		      else if (max_T[i] < local_max)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f is colder than timeseries-max %6.2f, using timeseries-max %6.2f, hour-index %d\n", i, max_T[i], local_max, local_max, local_max_nt);
			  Metx_max_T[i] = local_max;
			  Metx_max_T_hr[i] = local_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max warmer than allowed tolerance
		      else if (max_T[i] - local_max > conf_params.max_ts_tolerance)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f minus timeseries-max %6.2f is greater than tolerance %6.2f,  using timeseries-max plus tolerance %6.2f, hour-index %d\n", i, max_T[i], local_max, conf_params.max_ts_tolerance, (local_max + conf_params.max_ts_tolerance), local_max_nt);
			  Metx_max_T[i] = local_max + conf_params.max_ts_tolerance;
			  Metx_max_T_hr[i] = local_max_nt - (day * 24) - obs_offset - time_offset;
	         
			}
		      // Max is between time series and tolerance, use as-is
		      else
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f is warmer than timeseries-max %6.2f but within tolerance, using UTC-max %6.2f, hour-index %d\n", i, max_T[i], local_max, max_T[i], local_max_nt);
			  Metx_max_T[i] = max_T[i];
			  Metx_max_T_hr[i] = local_max_nt - (day * 24) - obs_offset - time_offset;
			}
		    }
		  else
		    {
		      proc_log->write_time(4, "Info: array index %d, timeseries-max is missing, using UTC-max %6.2f, hour-index 'default'\n", i, max_T[i]);
		      Metx_max_T[i] = max_T[i];
		      Metx_max_T_hr[i] = DEF_MAX_HR;
		    }
		  
		} // end if ns is less than num_output_sites
	    } // end loop over out_array_size 
	  
	  //
	  // For debugging only
	  // output Metx max/min arrays
	  //
	  proc_log->write_time(3, "In Metx max/min: OUTPUT Metx max/min ARRAYS\n");
	  for(int s = 0; s < num_output_sites; s++){
	    int data_ind = s * num_output_days;  
	    proc_log->write_time(3, "s = %d, data_ind = %d\n", s, data_ind);
	    for(int t = 0; t < num_output_days; t++){
	      proc_log->write_time(3, "Metx_max_T[%d] = %6.2f, max_T_hr = %2.0f, Metx_min_T[%d] = %6.2f, min_T_hr = %2.0f\n", data_ind, Metx_max_T[data_ind], Metx_max_T_hr[data_ind], data_ind, Metx_min_T[data_ind], Metx_min_T_hr[data_ind]); 
	      data_ind++;
	    }
	  }
	  
	  //
	  // Only write out vars if they appear in derived var list
	  // Note, local max-min is exactly the same as Metx max-min
	  // thus we write the Metx max-min arrays to the local max-min output vars
	  //
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "Metx_max_T"))
	      write_mod_fcst(&output_file, (char *)"Metx_max_T", Metx_max_T);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "local_max_T"))
	      write_mod_fcst(&output_file, (char *)"local_max_T", Metx_max_T);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "Metx_min_T"))
	      write_mod_fcst(&output_file, (char *)"Metx_min_T", Metx_min_T);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "local_min_T"))
	      write_mod_fcst(&output_file, (char *)"local_min_T", Metx_min_T);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "Metx_max_T_hr"))
	      write_mod_fcst(&output_file, (char *)"Metx_max_T_hr", Metx_max_T_hr);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "local_max_T_hr"))
	      write_mod_fcst(&output_file, (char *)"local_max_T_hr", Metx_max_T_hr);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "Metx_min_T_hr"))
	      write_mod_fcst(&output_file, (char *)"Metx_min_T_hr", Metx_min_T_hr);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "local_min_T_hr"))
	      write_mod_fcst(&output_file, (char *)"local_min_T_hr", Metx_min_T_hr);
	  
	  delete [] T;
	  delete [] max_T;
	  delete [] min_T;
	  delete [] Metx_max_T;
	  delete [] Metx_min_T;
	  delete [] Metx_max_T_hr;
	  delete [] Metx_min_T_hr;
	}
    }

  // Calc Day/Night max-min 
  //
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "daytime_max_T") ||
        0 == strcmp(conf_params._derived_var_names[np], "nighttime_min_T"))
      found = 1;

  if (found)
    {
      // See if required core vars are available
      // check required core vars are here

      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      if (found)
	{
	  // See if required core vars are available
	  // check required core vars are here
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "max_T"))
	      found = 1;
	}

      if (found)
	{
	  // See if required core vars are available
	  // check required core vars are here
	  found = 0;
	  for (np=0; np<num_fcst_vars; np++)
	    if (0 == strcmp(conf_params._fcst_var_names[np], "min_T"))
	      found = 1;
	}

      if (!found)
	{
	  proc_log->write_time("Warning: Unable to calculate %s or %s without fcst vars %s, %s, and %s\n", 
			       "daytime_max_T",  "nighttime_min_T", "T", "max_T", "min_T");
	}
      else
	{
	  long int daily_times_per_day = 1;
	  
	  // Allocate space for input vars
	  float *T_orig = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *max_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *min_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  
	  // Allocate space for output vars
	  float *daytime_max_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *nighttime_min_T = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *daytime_max_T_hr = new float[max_output_sites*num_output_days*daily_times_per_day];
	  float *nighttime_min_T_hr = new float[max_output_sites*num_output_days*daily_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T_orig);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"max_T", &daily_times_per_day, max_T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "max_T");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"min_T", &daily_times_per_day, min_T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "min_T");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * daily_times_per_day;
	  
	  //
	  // Fill-in the fcst data if fc_times_per_day is less than 24
	  // use cubic spline to fill in missing values (this is primarily for LT data)
	  //
	  float *T_fcst = NULL;
	  if(times_per_day < 24){
	    proc_log->write_time(5, "Info: In Day/night max/min: times_per_day: %d is less than 24, filling in T_fcst array\n", 
				 times_per_day);
	    int new_times_per_day;
	    T_fcst = fillInArray(T_orig, num_output_sites, num_output_days, times_per_day, new_times_per_day);
	    times_per_day = new_times_per_day;
	    num_output_times_per_day = new_times_per_day;
	  } else{
	    proc_log->write_time(5, "Info: In Day/night max/min: times_per_day: %d is equal to 24, copying T_orig to T_fcst\n", times_per_day);
	    T_fcst = T_orig;
	  }
	  
	  //
	  //Read in all of the obs files from the obs_config_file
	  //
	  vector< ObsFile* > obsFiles;
	  ret =  readObsFiles(conf_params.max_history, forc_start_hour, obs_config_file, output_site_array, num_output_sites, obsFiles);
	  if(ret != 0){
	    proc_log->write_time("Warning: unable to read obs data from %s\n", obs_config_file);
	    //return(1);
	  }
	  
	  //
	  // Concatenate the T obs with the T fcst,
	  // only execute if readObsFile succeeded
	  //
	  const string obs_var_name = "T";
	  int numConcatValsPerSite = 0;
	  int numObsValsPerSite = 0;
	  float *T = NULL;
	  
	  if(ret == 0){
	    T = concatVarData(obsFiles, obs_var_name, num_output_sites, T_fcst, forc_start_hour, num_output_days, times_per_day, numConcatValsPerSite, numObsValsPerSite);
	  } 
	  
	  // If concatVarData returns NULL just use the T fcst data
	  int obs_offset;
	  if(T == NULL){
	    proc_log->write_time("Info: concatVarData returned NULL, copying T_fcst to T\n");
	    numConcatValsPerSite = num_output_days*num_output_times_per_day;
	    numObsValsPerSite = 0;
	    obs_offset = 0;
	    T = new float[max_output_sites*numConcatValsPerSite];
	    T = T_fcst;
	  } else{
	    obs_offset = 24;
	  }
	  //cout << "obs_offset = " << obs_offset << endl;
	  
	  //
	  // For debugging only
	  // output concatenated T array
	  //
	  proc_log->write_time(3, "In Day/night max/min calc: OUTPUT T ARRAY\n");
	  for(int s = 0; s < num_output_sites; s++){
	    int data_ind = s * numConcatValsPerSite;
	    proc_log->write_time(3, "s = %d, data_ind = %d\n", s, data_ind);
	    for(int t = 0; t < numConcatValsPerSite; t++){
	      proc_log->write_time(3, "T[%d] = %f\n", data_ind, T[data_ind]);
	      data_ind++;
	    }
	  }
	  
	  // Initialize the day/night max/min arrays
	  for (nt=0; nt<out_array_size; nt++)
	    {
	      daytime_max_T[nt] = NC_FILL_FLOAT;
	      nighttime_min_T[nt] = NC_FILL_FLOAT;
	      daytime_max_T_hr[nt] = NC_FILL_INT;
	      nighttime_min_T_hr[nt] = NC_FILL_INT;
	    }
	  
	  
	  // Get the forecast hour, used for the day 0 max/min
	  utime2hour((time_t)forc_start_hour, fcst_hour_str);
	  int fcst_hour = atoi(fcst_hour_str);
	  //printf("fcst_hour = %d\n", fcst_hour);
	  
	  for (int i = 0; i < out_array_size; i++)
	    {
	      int ns = i / ( num_output_days * daily_times_per_day);
	      //cout << "i = " << i << " ns = " << ns << endl;
	      
	      if (ns < num_output_sites)
		{
		  // Calculate local time zone offset to GMT
		  float lat = output_ll_array[2*ns];
		  float lon = output_ll_array[2*ns+1];
		  if (lon > 180.)
		    lon = lon - 360.;
		  if (lon < -180.)
		    lon = lon + 360.;
		  
		  // Add check for jogs in the dateline. Currently only
		  // handles (crudely) the Aleutians.
		  if ((lat > 50. && lat < 55.) &&
		      (lon > 170.))
		    lon = lon - 360.;

		  int time_offset = (int)(-lon/360.*24. + 0.5);
		  proc_log->write_time(5, "time_offset = %d\n", time_offset);
		  int day = i % num_output_days;
		  //cout << "day = " << day << endl;
		  
		  // Set start and end indicies for finding min temperatures in time-series
		  int min_start = (day * 24 + time_offset + conf_params.nighttime_min_start) + obs_offset;
		  int min_end = (day * 24 + time_offset + conf_params.nighttime_min_end) + obs_offset;
	
		  //printf("for day = %d, original min_start = %d\n", day, min_start);
		  
		  // For day 0 re-set start index to current hour if it's later than the nighttime start
		  if(day == 0 && (fcst_hour + obs_offset) >= min_start){
		    min_start = fcst_hour + obs_offset;
		    proc_log->write_time(5, "for site = %d, day = %d, using forc_start_hour, min_start = %d, min_end = %d\n", ns, day, min_start, min_end-1);
		  } else{
		    proc_log->write_time(5, "for site = %d, day = %d, using nighttime hour,  min_start = %d, min_end = %d\n", ns, day, min_start, min_end-1);
		  }
		  
		  //
		  // Find lowest time series "night-time" min.
		  //
		  float nighttime_min = 9999999.;
		  int nighttime_min_nt = -1;
		  for (nt = min_start; nt < min_end; nt++)
		    {
		      if (nt >= 0 && 
			  nt < numConcatValsPerSite)
			{
			  int index = ns * numConcatValsPerSite + nt;
			  if (T[index] != NC_FILL_FLOAT && T[index] < nighttime_min){
			    nighttime_min = T[index];
			    nighttime_min_nt = nt;
			  }
			}
		    }
		  
		  //printf("ns %d  nd %d   min %.2f\n", ns, day, nighttime_min);

		  // Choose correct min;
		  // default to utc min if nighttime_min is missing
		  int nighttime_min_time_ind = 9999999; 
		  if (nighttime_min != 9999999.)
		    {
		      // Use time series if min is missing
		      if (min_T[i] == NC_FILL_FLOAT)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min is missing, using timeseries-min %6.2f, hour-index %d\n", i, nighttime_min, nighttime_min_nt);
			  nighttime_min_T[i] = nighttime_min;
			  nighttime_min_T_hr[i] = nighttime_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min warmer than time series, use time series
		      else if (min_T[i] > nighttime_min)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min %6.2f is warmer than timeseries-min %6.2f, using timeseries-min %6.2f, hour-index %d\n", i, min_T[i], nighttime_min, nighttime_min, nighttime_min_nt);
			  nighttime_min_T[i] = nighttime_min;
			  nighttime_min_T_hr[i] = nighttime_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min colder than allowed tolerance - set to tolerance
		      else if (nighttime_min - min_T[i] > conf_params.nighttime_min_ts_tolerance)
			{
			  proc_log->write_time(4, "Info: array index %d, timeseries-min %6.2f minus UTC-min %6.2f is greater than tolerance %6.2f, using timeseries-min minus tolerance %6.2f, hour-index %d\n", i, nighttime_min, min_T[i], conf_params.nighttime_min_ts_tolerance, nighttime_min - conf_params.nighttime_min_ts_correction, nighttime_min_nt);
			  nighttime_min_T[i] = nighttime_min - conf_params.nighttime_min_ts_correction;
			  nighttime_min_T_hr[i] = nighttime_min_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Min is between time series and tolerance, use as-is
		      else
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-min %6.2f is colder than timeseries-min %6.2f but within tolerance, using UTC-min %6.2f, hour-index %d\n", i, min_T[i], nighttime_min, min_T[i], nighttime_min_nt);
			  nighttime_min_T[i] = min_T[i];
			  nighttime_min_T_hr[i] = nighttime_min_nt - (day * 24) - obs_offset - time_offset;
			}
		    }
		  else
		    {
		      proc_log->write_time(4, "Info: array index %d, timeseries-min is missing, using UTC-min %6.2f\n, hour-index 'default'", i, min_T[i]);
		      nighttime_min_T[i] = min_T[i]; 
		      nighttime_min_T_hr[i] = DEF_MIN_HR;
		    }
		  
		  // Now resolve Max
		  
		  // Set start and end indicies for finding max temperatures in time-series
		  int max_start = (day * 24 + time_offset + conf_params.daytime_max_start) + obs_offset;
		  int max_end = (day * 24 + time_offset + conf_params.daytime_max_end) + obs_offset;
		  
		  //printf("for day = %d, original max_start = %d\n", day, max_start);
		  
		  // For day 0 re-set start index to current hour if it's later than the daytime start
		  if(day == 0 && (fcst_hour + obs_offset) >= max_start){
		    max_start = fcst_hour + obs_offset;
		    proc_log->write_time(5, "for site = %d, day = %d, using forc_start_hour, max_start = %d, max_end = %d\n", ns, day, max_start, max_end-1);
		  } else{
		    proc_log->write_time(5, "for site = %d, day = %d, using daytime hour,  max_start = %d, max_end = %d\n", ns, day, max_start, max_end-1);
		  }
		  
		  //
		  // Find time-series "day-time" max and when that ocurred.
                  //
		  float daytime_max = -9999999.;
		  int daytime_max_nt = -1;
		  for (nt = max_start; nt < max_end; nt++)
		    {
		      if (nt >= 0 && 
			  nt < numConcatValsPerSite)
			{
			  int index = ns * numConcatValsPerSite + nt;
			  if (T[index] != NC_FILL_FLOAT && T[index] >= daytime_max)
			    {
			      daytime_max = T[index];
			      daytime_max_nt = nt;
			    }
			}
		    }
		  
		  // Choose correct max
		  // default to utc max if daytime_max is missing
		  if (daytime_max != -9999999.)
		    {
		      // Use time series if max is missing
		      if (max_T[i] == NC_FILL_FLOAT)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max is missing, using timeseries-max %6.2f\n", i, daytime_max);
			  daytime_max_T[i] = daytime_max;
			  daytime_max_T_hr[i] = daytime_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max is at hour 0 of time series, use it (cold front
		      // case - special for max only)
		      else if (daytime_max_nt == max_start)
			{
			  proc_log->write_time(4, "Info: array index %d, timeseries-max %6.2f is at beginning of timeseries, using timeseries-max %6.2f (ref UTC-max %6.2f), hour-index %d\n", i, daytime_max, daytime_max, max_T[i], daytime_max_nt);
			  daytime_max_T[i] = daytime_max;
			  daytime_max_T_hr[i] = daytime_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max colder than time series, use time series
		      else if (max_T[i] < daytime_max)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f is colder than timeseries-max %6.2f, using timeseries-max %6.2f, hour-index %d\n", i, max_T[i], daytime_max, daytime_max, daytime_max_nt);
			  daytime_max_T[i] = daytime_max;
			  daytime_max_T_hr[i] = daytime_max_nt - (day * 24) - obs_offset - time_offset;
			}
		      // Max warmer than allowed tolerance
		      else if (max_T[i] - daytime_max > conf_params.daytime_max_ts_tolerance)
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f minus timeseries-max %6.2f is greater than tolerance %6.2f,  using timeseries-max plus tolerance %6.2f, hour-index %d\n", i, max_T[i], daytime_max, conf_params.daytime_max_ts_tolerance, (daytime_max + conf_params.daytime_max_ts_correction), daytime_max_nt);
			  daytime_max_T[i] = daytime_max + conf_params.daytime_max_ts_correction;
			  daytime_max_T_hr[i] = daytime_max_nt - (day * 24) - obs_offset - time_offset;
	         
			}
		      // Max is between time series and tolerance, use as-is
		      else
			{
			  proc_log->write_time(4, "Info: array index %d, UTC-max %6.2f is warmer than timeseries-max %6.2f but within tolerance, using UTC-max %6.2f, hour-index %d\n", i, max_T[i], daytime_max, max_T[i], daytime_max_nt);
			  daytime_max_T[i] = max_T[i];
			  daytime_max_T_hr[i] = daytime_max_nt - (day * 24) - obs_offset - time_offset;
			}
		    }
		  else
		    {
		      proc_log->write_time(4, "Info: array index %d, timeseries-max is missing, using UTC-max %6.2f, hour-index 'default'\n", i, max_T[i]);
		      daytime_max_T[i] = max_T[i];
		      daytime_max_T_hr[i] = DEF_MAX_HR;
		    }
		  
		} // end if ns is less than num_output_sites
	    } // end loop over out_array_size 
	  
	  //
	  // For debugging only
	  // output day/night max/min arrays
	  //
	  proc_log->write_time(3, "In Day/night max/min: OUTPUT Day/Night max/min ARRAYS\n");
	  for(int s = 0; s < num_output_sites; s++){
	    int data_ind = s * num_output_days;  
	    proc_log->write_time(3, "s = %d, data_ind = %d\n", s, data_ind);
	    for(int t = 0; t < num_output_days; t++){
	      proc_log->write_time(3, "daytime_max_T[%d] = %6.2f, max_T_hr = %2.0f, nighttime_min_T[%d] = %6.2f, min_T_hr = %2.0f\n", data_ind, daytime_max_T[data_ind], daytime_max_T_hr[data_ind], data_ind, nighttime_min_T[data_ind], nighttime_min_T_hr[data_ind]); 
	      data_ind++;
	    }
	  }
	  
	  //
	  // Only write out vars if they appear in derived var list
	  //
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "daytime_max_T"))
	      write_mod_fcst(&output_file, (char *)"daytime_max_T", daytime_max_T);
	  
	  for (np=0; np<num_derived_vars; np++)
	    {
	      if (0 == strcmp(conf_params._derived_var_names[np], "nighttime_min_T"))
		{
		  write_mod_fcst(&output_file, (char *)"nighttime_min_T", nighttime_min_T);
		}
	    }

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "daytime_max_T_hr"))
	      write_mod_fcst(&output_file, (char *)"daytime_max_T_hr", daytime_max_T_hr);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "nighttime_min_T_hr"))
	      write_mod_fcst(&output_file, (char *)"nighttime_min_T_hr", nighttime_min_T_hr);
	  
	  delete [] T;
	  delete [] max_T;
	  delete [] min_T;
	  delete [] daytime_max_T;
	  delete [] nighttime_min_T;
	  delete [] daytime_max_T_hr;
	  delete [] nighttime_min_T_hr;
	}
    }



  // Calc RH, rh_pct, spec_hum, TempF, dewptF
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "rh") ||
	0 == strcmp(conf_params._derived_var_names[np], "rh_pct") ||
	0 == strcmp(conf_params._derived_var_names[np], "spec_hum") ||
	0 == strcmp(conf_params._derived_var_names[np], "TempF") ||
	0 == strcmp(conf_params._derived_var_names[np], "dewptF"))
      found = 1;

  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "dewpt"))
              found = 1;
        }

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "P_sfc"))
              found = 1;
        }

      if (!found)
        {
          proc_log->write_time("Warning: Unable to calculate %s, %s, %s or %s without fcst vars %s, %s and %s\n", 
			       "rh", "spec_hum", "TempF", "dewptF", "T", "dewpt", "P_sfc");
        }
      else
	{
	  // Allocate space for input vars
	  float *T = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *TD = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *P_sfc = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *rh = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *rh_pct = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *spec_hum = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *TempF = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *dewptF = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }
	
	  ret = read_fcst_data(&output_file, (char *)"dewpt", &times_per_day, TD);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "TD");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"P_sfc", &times_per_day, P_sfc);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "P_sfc");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      rh[nt] = NC_FILL_FLOAT;
	      rh_pct[nt] = NC_FILL_FLOAT;
	      spec_hum[nt] = NC_FILL_FLOAT;
	      TempF[nt] = NC_FILL_FLOAT;
	      dewptF[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      if (T[i] != NC_FILL_FLOAT && TD[i] != NC_FILL_FLOAT)
		{
		  rh[i] = calc_rh(T[i], TD[i]);
		  rh_pct[i] = 100. * rh[i];
		}

	      if (TD[i] != NC_FILL_FLOAT && P_sfc[i] != NC_FILL_FLOAT)
		{
		  float mixr = calc_mixr(TD[i], P_sfc[i])/1000.;
		  spec_hum[i] = mixr/(1+mixr);
		}
      
	      if (T[i] != NC_FILL_FLOAT)
		TempF[i] = celsius2faren(T[i]);

	      if (TD[i] != NC_FILL_FLOAT)
		dewptF[i] = celsius2faren(TD[i]);

	    }

	  // Only write out vars if they appear in derived var list
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "rh"))
	      write_mod_fcst(&output_file, (char *)"rh", rh);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "rh_pct"))
	      write_mod_fcst(&output_file, (char *)"rh_pct", rh_pct);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "spec_hum"))
	      write_mod_fcst(&output_file, (char *)"spec_hum", spec_hum);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "TempF"))
	      write_mod_fcst(&output_file, (char *)"TempF", TempF);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "TempF"))
	      write_mod_fcst(&output_file, (char *)"dewptF", dewptF);

	  delete [] T;
	  delete [] TD;
	  delete [] P_sfc;
	  delete [] rh;
	  delete [] rh_pct;
	  delete [] spec_hum;
	  delete [] TempF;
	  delete [] dewptF;
	}
    }


  // Calc prob_fog
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "prob_fog"))
      found = 1;
  
  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "rh"))
          found = 1;

      if (!found)
        {
          proc_log->write_time("Warning: Unable to calculate %s without fcst var %s\n", 
			       "prob_fog", "rh");
        }
      else
	{
	  // Allocate space for input vars
	  float *rh = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *prob_fog = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"rh", &times_per_day, rh);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "rh");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    prob_fog[nt] = NC_FILL_FLOAT;

	  for (int i = 0; i < out_array_size; i++)
	    {

	      // These are used for printing debug statments only
	      int ns = i / ( num_output_days * num_output_times_per_day);
	      int lead_ind = i - (ns *  num_output_days * num_output_times_per_day);
	      
	      if (rh[i] != NC_FILL_FLOAT)
		{
		  prob_fog[i] = calc_pof_rap1_2(rh[i]*100);
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, rh = %.2f, prob_fog = %.4f\n", ns, lead_ind, rh[i], prob_fog[i]);
		} 
	      else
		{
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, rh is MISSING, prob_fog = MISSING\n", ns, lead_ind);
		}
	      
	    }

	  write_mod_fcst(&output_file, (char *)"prob_fog", prob_fog);
	  
	  delete [] rh;
	  delete [] prob_fog;

	}
    }


  // Calc air-density
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "air_density"))
      found = 1;
  
  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "dewpt"))
              found = 1;
        }

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "P_sfc"))
              found = 1;
        }

      if (!found)
        {
          proc_log->write_time("Warning: Unable to calculate %s and %s without fcst vars %s, %s and %s\n", 
			       "rh", "air_density", "T", "dewpt", "P_sfc");
        }
      else
	{
	  // Allocate space for input vars
	  float *T = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *TD = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *P_sfc = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for temporary variables
	  float *rh = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  // Allocate space for output vars
	  float *air_density = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }
	
	  ret = read_fcst_data(&output_file, (char *)"dewpt", &times_per_day, TD);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "TD");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"P_sfc", &times_per_day, P_sfc);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "P_sfc");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      rh[nt] = NC_FILL_FLOAT;
	      air_density[nt] = NC_FILL_FLOAT;
	    }

	  for (int i = 0; i < out_array_size; i++)
	    {
	      // These are used for printing debug statments only
	      int ns = i / ( num_output_days * num_output_times_per_day);
	      int lead_ind = i - (ns *  num_output_days * num_output_times_per_day);
	      
	      // Calculate RH first, this is needed for the air-density calculation
	      if (T[i] != NC_FILL_FLOAT && TD[i] != NC_FILL_FLOAT)
		{
		  rh[i] = calc_rh(T[i], TD[i]);
		}
	      
	      // Use T, rh and P_sfc to calculate air-density (moist air calculation)
	      if (T[i] != NC_FILL_FLOAT && rh[i] != NC_FILL_FLOAT && P_sfc[i] != NC_FILL_FLOAT)
		{
		  air_density[i] = calc_air_density(T[i], rh[i], P_sfc[i]);
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, T = %.2f, rh =  %.2f, air_density = %.4f\n", ns, lead_ind, T[i], rh[i], air_density[i]);
		} 
	      else
		{
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, T = MISSING, rh =  MISSING, air_density = MISSING\n", ns, lead_ind);
		}
	      
	    }

	  // Only write out vars if they appear in derived var list
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "air_density"))
	      write_mod_fcst(&output_file, (char *)"air_density", air_density);
	  
	  delete [] T;
	  delete [] TD;
	  delete [] P_sfc;
	  delete [] rh;
	  delete [] air_density;
	}
    }

  // Calc wet-bulb temperature
  found = 0;
  for (np=0; np<num_derived_vars; np++)
    if (0 == strcmp(conf_params._derived_var_names[np], "wet_bulb_temp"))
      found = 1;
  
  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "dewpt"))
              found = 1;
        }

      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "P_sfc"))
              found = 1;
        }

      if (!found)
        {
          proc_log->write_time("Warning: Unable to calculate %s without fcst vars %s, %s and %s\n", 
			       "wet_bulb_temp", "T", "dewpt", "P_sfc");
        }
      else
	{
	  // Allocate space for input vars
	  float *T = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *TD = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *P_sfc = new float[max_output_sites*num_output_days*num_output_times_per_day];

	  // Allocate space for output vars
	  float *wet_bulb_temp = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }
	
	  ret = read_fcst_data(&output_file, (char *)"dewpt", &times_per_day, TD);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "TD");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"P_sfc", &times_per_day, P_sfc);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "P_sfc");
	      return(1);
	    }

	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    wet_bulb_temp[nt] = NC_FILL_FLOAT;

	  for (int i = 0; i < out_array_size; i++)
	    {
	      // These are used for printing debug statments only
	      int ns = i / ( num_output_days * num_output_times_per_day);
	      int lead_ind = i - (ns *  num_output_days * num_output_times_per_day);
	      
	      // Use T, TD and P_sfc to calculate wet bulb temp
	      if (T[i] != NC_FILL_FLOAT && TD[i] != NC_FILL_FLOAT && P_sfc[i] != NC_FILL_FLOAT)
		{
		  wet_bulb_temp[i] = calc_wet_bulb_temp(P_sfc[i], T[i], TD[i]);
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, T = %.2f, wet bulb temp = %.4f\n", ns, lead_ind, T[i], wet_bulb_temp[i]);
		} 
	      else
		{
		  proc_log->write_time(3, "Info: for site(ns) = %d, lead_ind = %d, one input is MISSING, wet bulb temp = MISSING\n", ns, lead_ind);
		}
	      
	    }

	  // Only write out vars if they appear in derived var list
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "wet_bulb_temp"))
	      write_mod_fcst(&output_file, (char *)"wet_bulb_temp", wet_bulb_temp);
	  
	  delete [] T;
	  delete [] TD;
	  delete [] P_sfc;
	  delete [] wet_bulb_temp;
	}
    }


  // Calc precip_rate, precip_type, precip accum, snow accum, 
  // snow_accum_total, snow_rate, prob_precip_pct, ect
  //
  //   If we are going to calculate blowing snow potential
  //   later, we need to make sure we have qpf01
  
  found = 0;
  int found_kuchera_snow_rate = 0;
  for (np=0; np<num_derived_vars; np++)
    {
      if (0 == strcmp(conf_params._derived_var_names[np], "precip_rate") ||
	  0 == strcmp(conf_params._derived_var_names[np], "precip_rate_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "precip_type") ||
	  0 == strcmp(conf_params._derived_var_names[np], "precip_accum") ||
	  0 == strcmp(conf_params._derived_var_names[np], "precip_accum_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "cprob_rain") ||
	  0 == strcmp(conf_params._derived_var_names[np], "cprob_snow") ||
	  0 == strcmp(conf_params._derived_var_names[np], "cprob_ice") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_rate") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_rate_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum_total") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum_total_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum_72hr_total") ||
	  0 == strcmp(conf_params._derived_var_names[np], "snow_accum_72hr_total_inches") ||
	  0 == strcmp(conf_params._derived_var_names[np], "ice_rate") ||
	  0 == strcmp(conf_params._derived_var_names[np], "ice_accum") ||
	  0 == strcmp(conf_params._derived_var_names[np], "ice_accum_total") ||
	  0 == strcmp(conf_params._derived_var_names[np], "kuchera_snow_rate") ||
	  0 == strcmp(conf_params._derived_var_names[np], "prob_precip01") ||
	  0 == strcmp(conf_params._derived_var_names[np], "prob_precip03") ||
	  0 == strcmp(conf_params._derived_var_names[np], "prob_precip06") ||
	  0 == strcmp(conf_params._derived_var_names[np], "prob_precip_prev24") ||
	  0 == strcmp(conf_params._derived_var_names[np], "qpf01") ||
	  0 == strcmp(conf_params._derived_var_names[np], "qpf03") ||
	  0 == strcmp(conf_params._derived_var_names[np], "qpf06") ||
	  0 == strcmp(conf_params._derived_var_names[np], "qpf24") ||
	  0 == strcmp(conf_params._derived_var_names[np], "pop03_pct") ||
	  0 == strcmp(conf_params._derived_var_names[np], "blowing_snow_potential") ||
	  0 == strcmp(conf_params._derived_var_names[np], "blowing_snow_pot_index") ||
	  0 == strcmp(conf_params._derived_var_names[np], "vaisala_precip_state"))
	found = 1;
      
      if(0 == strcmp(conf_params._derived_var_names[np], "kuchera_snow_rate"))
	found_kuchera_snow_rate = 1;
      
    } // end loop over derived vars
  

  if (found)
    {
      // See if required core vars are available
      // check required core vars are here
      found = 0;
      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "T"))
          found = 1;

      for (np=0; np<num_fcst_vars; np++)
        if (0 == strcmp(conf_params._fcst_var_names[np], "rh"))
          found = 1;
      
      
      int found_qpf06 = 0;
      if (found)
        {
          found = 0;
	  for (np=0; np<num_fcst_vars; np++){
            if (0 == strcmp(conf_params._fcst_var_names[np], "qpf01"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "qpf03"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "qpf06")){
	      found = 1;
	      found_qpf06 = 1;
	    }
	  } // end loop over fcst_vars
        }
      
      int found_cqpf06 = 0;
      if (found)
        {
          found = 0;
	  for (np=0; np<num_fcst_vars; np++){
            if (0 == strcmp(conf_params._fcst_var_names[np], "precip_cn1"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "precip_cn3"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "precip_cn6")){
	      found = 1;
	      found_cqpf06 = 1;
	    }
	  } // end loop over fcst_vars
        }
      
      int found_pop06 = 0;
      if (found)
        {
          found = 0;
          for (np=0; np<num_fcst_vars; np++){
            if (0 == strcmp(conf_params._fcst_var_names[np], "prob_precip01"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "prob_precip03"))
              found = 1;
	    if (0 == strcmp(conf_params._fcst_var_names[np], "prob_precip06")){
              found = 1;
	      found_pop06 = 1;
	    }
	  } // end loop over fcst_vars
        }
      
      // Check if we have kuchera_snow_ratio as input
      int found_kuchera_snow_ratio = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "kuchera_snow_ratio"))
	  found_kuchera_snow_ratio = 1;

      //printf("found_kuckera_snow_rate: %d, found_kuchera_snow_ratio: %d\n", found_kuchera_snow_rate, found_kuchera_snow_ratio);
      
      // Check if we have precip_type as input
      int found_precip_type = 0;
      for (np=0; np<num_fcst_vars; np++)
	if (0 == strcmp(conf_params._fcst_var_names[np], "precip_type"))
	  found_precip_type = 1;
      
      // Check for cprob_vars if we don't have precip_type
      //
      if(!found_precip_type)
	{
	  if (found)
	    {
	      found = 0;
	      for (np=0; np<num_fcst_vars; np++)
		if (0 == strcmp(conf_params._fcst_var_names[np], "cprob_snow"))
		  found = 1;
	    }
	  
	  if (found)
	    {
	      found = 0;
	      for (np=0; np<num_fcst_vars; np++)
		if (0 == strcmp(conf_params._fcst_var_names[np], "cprob_rain"))
		  found = 1;
	    }
	  
	  if (found)
	    {
	      found = 0;
	      for (np=0; np<num_fcst_vars; np++)
		if (0 == strcmp(conf_params._fcst_var_names[np], "cprob_ice"))
		  found = 1;
	    }
	}
      
      
      if (!found)
        {
          proc_log->write_time("Warning: Unable to calculate precip vars without fcst vars %s, %s, %s, %s, %s, and %s\n", 
			       "T", "qpf01, qpf03 or qpf06", "prob_precip01, prob_precip03 or prob_precip06", "cprob_snow", "cprob_rain", "cprob_ice");
        }
      else
	{
	  // Allocate space for input vars
	  float *T = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *rh = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *wspd = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *qpf03 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *pop03 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *qpf06 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *pop06 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cqpf03 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cqpf06 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cpos = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cpor = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cpoi = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *kuchera_snow_ratio = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  // Allocate space for output vars
	  float *precip_rate = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *precip_rate_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *precip_type = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *precip_accum = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *precip_accum_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cprob_rain = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cprob_snow = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cprob_ice = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum_total = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum_total_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum_72hr_total = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_accum_72hr_total_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_rate = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *snow_rate_inches = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *ice_rate = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *ice_accum = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *ice_accum_total = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *kuchera_snow_rate = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *pop03_pct = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *vaisala_precip_state = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *qpf01 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *cqpf01 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *pop01 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *der_pop03 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *der_pop06 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *der_pop24 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  float *qpf24 = new float[max_output_sites*num_output_days*num_output_times_per_day];
	  
	  ret = read_fcst_data(&output_file, (char *)"T", &times_per_day, T);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "T");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"rh", &times_per_day, rh);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "rh");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"precip_cn3", &times_per_day, cqpf03);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "precip_cn3");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"precip_cn3", &times_per_day, cqpf06);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "precip_cn6");
	      return(1);
	    }

	  ret = read_fcst_data(&output_file, (char *)"wind_speed", &times_per_day, wspd);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "wind_speed");
	      return(1);
	    }

	  
	  ret = read_fcst_data(&output_file, (char *)"qpf03", &times_per_day, qpf03);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "qpf03");
	      return(1);
	    }
	  
	  // Read in qpf06 only if it's listed in the fcst_var_names from the params file
	  if (found_qpf06){
	    ret = read_fcst_data(&output_file, (char *)"qpf06", &times_per_day, qpf06);
	    if (ret != 0)
	      {
		proc_log->write_time("Error: Unable to read fcst var %s\n", "qpf06");
		return(1);
	      }
	  }
	  
	  ret = read_fcst_data(&output_file, (char *)"prob_precip03", &times_per_day, pop03);
	  if (ret != 0)
	    {
	      proc_log->write_time("Error: Unable to read fcst var %s\n", "prob_precip03");
	      return(1);
	    }
	  
	  // Read in pop06 only if it's listed in the fcst_var_names from the params file
	  if (found_pop06){
	    ret = read_fcst_data(&output_file, (char *)"prob_precip06", &times_per_day, pop06);
	    if (ret != 0)
	      {
		proc_log->write_time("Error: Unable to read fcst var %s\n", "prob_precip06");
		return(1);
	      }
	  }
	  
	  // Read in kuchera_snow_ratio only if it's listed in the fcst_var_names from the params file
	  if (found_kuchera_snow_ratio){
	    ret = read_fcst_data(&output_file, (char *)"kuchera_snow_ratio", &times_per_day, kuchera_snow_ratio);
	    if (ret != 0)
	      {
		proc_log->write_time("Error: Unable to read fcst var %s\n", "kuchera_snow_ratio");
		return(1);
	      }
	  }
	  
	  // Read in precip_type only if it's listed in the fcst_var_names from the params file
	  // else read in the cprob vars so we can derive precip_type
	  if (found_precip_type)
	    {
	      ret = read_fcst_data(&output_file, (char *)"precip_type", &times_per_day, precip_type);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "precip_type");
		  return(1);
		}
	    }
	  else
	    {
	      ret = read_fcst_data(&output_file, (char *)"cprob_snow", &times_per_day, cpos);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "cprob_snow");
		  return(1);
		}

	      ret = read_fcst_data(&output_file, (char *)"cprob_rain", &times_per_day, cpor);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "cprob_rain");
		  return(1);
		}

	      ret = read_fcst_data(&output_file, (char *)"cprob_ice", &times_per_day, cpoi);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "cprob_rain");
		  return(1);
		}
	    }
	  
	  out_array_size = max_output_sites * num_output_days * times_per_day;

	  for (nt=0; nt<out_array_size; nt++)
	    {
	      precip_rate[nt] = NC_FILL_FLOAT;
	      precip_rate_inches[nt] = NC_FILL_FLOAT;
	      if(!found_precip_type)
		precip_type[nt] = NC_FILL_FLOAT;
	      precip_accum[nt] = NC_FILL_FLOAT;
	      precip_accum_inches[nt] = NC_FILL_FLOAT;
	      cprob_rain[nt] = NC_FILL_FLOAT;
	      cprob_snow[nt] = NC_FILL_FLOAT;
	      cprob_ice[nt] = NC_FILL_FLOAT;
	      snow_accum[nt] = NC_FILL_FLOAT;
	      snow_accum_inches[nt] = NC_FILL_FLOAT;
	      snow_accum_total[nt] = NC_FILL_FLOAT;
	      snow_accum_total_inches[nt] = NC_FILL_FLOAT;
	      snow_accum_72hr_total[nt] = NC_FILL_FLOAT;
	      snow_accum_72hr_total_inches[nt] = NC_FILL_FLOAT;
	      snow_rate[nt] = NC_FILL_FLOAT;
	      snow_rate_inches[nt] = NC_FILL_FLOAT;
	      ice_rate[nt] = NC_FILL_FLOAT;
	      ice_accum[nt] = NC_FILL_FLOAT;
	      ice_accum_total[nt] = NC_FILL_FLOAT;
	      kuchera_snow_rate[nt] = NC_FILL_FLOAT;
	      pop03_pct[nt] = NC_FILL_FLOAT;
	      der_pop03[nt] = NC_FILL_FLOAT;
	      der_pop06[nt] = NC_FILL_FLOAT;
	      der_pop24[nt] = NC_FILL_FLOAT;
	      qpf24[nt] = NC_FILL_FLOAT;
	    }


	  // if hourly pop and qpf data exist in the core vars list, use them
	  // Otherwise derive them from qpf03 and pop03
	  found = 0;
          for (np=0; np<num_fcst_vars; np++)
            if (0 == strcmp(conf_params._fcst_var_names[np], "prob_precip01"))
              found = 1;

	  if (found)
	    {
	      found = 0;
	      for (np=0; np<num_fcst_vars; np++)
		if (0 == strcmp(conf_params._fcst_var_names[np], "qpf01"))
		  found = 1;
	    }

	  if (found)
	    {
	      // read in hourly time series
	      ret = read_fcst_data(&output_file, (char *)"qpf01", &times_per_day, qpf01);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "qpf01");
		  return(1);
		}

	      ret = read_fcst_data(&output_file, (char *)"prob_precip01", &times_per_day, pop01);
	      if (ret != 0)
		{
		  proc_log->write_time("Error: Unable to read fcst var %s\n", "prob_precip01");
		  return(1);
		}	 
	      
	      // If some of the qpf01 and pop01 values are missing (st portion of the timeseries), 
	      // use the qpf03 and pop03 values (or the qpf06 and pop06 values) to fill in the missing values
	      for (int i = 0; i<out_array_size; i++)
		{
		  if(qpf01[i] == NC_FILL_FLOAT && qpf03[i] != NC_FILL_FLOAT){
		    qpf01[i] = qpf03[i] / 3.;
		    //printf("Info: using qpf03 to derive qpf01, i: %d, qpf03: %f, qpf01: %f\n", i, qpf03[i], qpf01[i]);
		  } else if(qpf01[i] == NC_FILL_FLOAT && qpf06[i] != NC_FILL_FLOAT){
		    qpf01[i] = qpf06[i] / 6.;
		    //printf("Info: using qpf06 to derive qpf01, i: %d, qpf06: %f, qpf01: %f\n", i, qpf06[i], qpf01[i]);
		  }
		  
		  if(pop01[i] == NC_FILL_FLOAT && pop03[i] != NC_FILL_FLOAT){
		    pop01[i] = pop03[i];
		    //printf("Info: using pop03 to derive pop01, i: %d, pop03: %f, pop01: %f\n", i, pop03[i], pop01[i]);
		  } else if(pop01[i] == NC_FILL_FLOAT && pop06[i] != NC_FILL_FLOAT){
		    pop01[i] = pop06[i];
		    //printf("Info: using pop06 to derive pop01, i: %d, pop06: %f, pop01: %f\n", i, pop06[i], pop01[i]);
		  }
		} // end loop over out_array_size
     
	    }
	  else
	    {
	      // Derive qpf01, precip_cn1 and pop01, using qpf03, precip_cn3 and pop03 (or qpf06 and pop06)
	      for (int i = 0; i<out_array_size; i++)
		{
		  if (qpf03[i] != NC_FILL_FLOAT){
		    qpf01[i] = qpf03[i] / 3.;
		    //printf("Info: using qpf03 to derive qpf01, i: %d, qpf03: %f, qpf01: %f\n", i, qpf03[i], qpf01[i]);
		  } else if (qpf06[i] != NC_FILL_FLOAT){
		    qpf01[i] = qpf06[i] / 6.;
		    //printf("Info: using qpf06 to derive qpf01, i: %d, qpf06: %f, qpf01: %f\n", i, qpf06[i], qpf01[i]);
		  } else {
		    qpf01[i] = NC_FILL_FLOAT;
		  }
		  
		  if (cqpf03[i] != NC_FILL_FLOAT){
		    cqpf01[i] = cqpf03[i] / 3.;
		    //printf("Info: using qpf03 to derive qpf01, i: %d, qpf03: %f, qpf01: %f\n", i, qpf03[i], qpf01[i]);
		  } else if (cqpf06[i] != NC_FILL_FLOAT){
		    cqpf01[i] = cqpf06[i] / 6.;
		    //printf("Info: using qpf06 to derive qpf01, i: %d, qpf06: %f, qpf01: %f\n", i, qpf06[i], qpf01[i]);
		  } else {
		    cqpf01[i] = NC_FILL_FLOAT;
		  }
		  
		  if (pop03[i] != NC_FILL_FLOAT){
                    pop01[i] = pop03[i];
		    //printf("Info: using pop03 to derive pop01, i: %d, pop03: %f, pop01: %f\n", i, pop03[i], pop01[i]);
                  } else if (pop06[i] != NC_FILL_FLOAT){
		    pop01[i] = pop06[i];
		    //printf("Info: using pop06 to derive pop01, i: %d, pop06: %f, pop01: %f\n", i, pop06[i], pop01[i]);
		  } else {
		    if (cqpf01[i] ==  NC_FILL_FLOAT || qpf01[i] == NC_FILL_FLOAT ||
			rh[i] ==  NC_FILL_FLOAT)
		      pop01[i] = NC_FILL_FLOAT;
		    else
		      pop01[i] = calc_pop_rap1(cqpf01[i],  qpf01[i], rh[i], 1);
		  }
		  
		} // end loop over out_array_size
	    }

	    
	  // Loop over output array and derive precip vars
	  //
	  // Initialize snow_total and ice_total
	  float snow_total = 0.;
	  float ice_total = 0.;
	  int warn_cnt = 0;
	  
	  for (int i = 0; i < out_array_size; i++)
	    {

	      int hr = i % (num_output_days*num_output_times_per_day);
	      if (hr == 0){
		snow_total = 0.;
		ice_total = 0;
	      }

	      //if(!found_precip_type)
	      //{
	      //  precip_type[i] = find_precip_type(T[i], qpf01[i],
	      //				    pop01[i], cpor[i],
	      //				    cpos[i], cpoi[i]);
	      //}
	      
	      // Calc simplistic cprob-rain, cprob-snow and cprob-ice using T
	      // This is to get values for Logicast MDSS
	      float snow_thresh = 30.0;
	      float rain_thresh = 36.0;
	      float range = 6.0;

	      if(T[i] != NC_FILL_FLOAT)
		{
		  float temp_f = celsius2faren(T[i]);
		  
		  if(temp_f <= 30)
		    {
		      cprob_rain[i] = 0.0;
		      cprob_snow[i] = 1.0;
		      cprob_ice[i] = 0.0;
		    }
		  else if(temp_f >= 36)
		    {
		      cprob_rain[i] = 1.0;
		      cprob_snow[i] = 0.0;
		      cprob_ice[i] = 0.0;
		    }
		  else
		    {
		      float T_diff = temp_f - snow_thresh;
		      cprob_snow[i] = 1 - (T_diff / range);
		      cprob_rain[i] = 1 - cprob_snow[i];
		      
		      if(temp_f >= 32 && temp_f <= 34)
			{
			  cprob_snow[i] = cprob_snow[i] - 0.05;
			  cprob_rain[i] = cprob_rain[i] - 0.05;
			  cprob_ice[i] = 0.10;
			}
		      else
			cprob_ice[i] = 0;
		    }
		  
		  //printf("i: %d, T: %f, temp_f: %f, cprob_rain: %f, cprob_snow: %f, cprob_ice: %f\n", i, T[i], temp_f, cprob_rain[i], cprob_snow[i], cprob_ice[i]);
	      
		}

	      // compute precip type uising the simple cprobs
	      if(!found_precip_type)
		{
		  precip_type[i] = find_precip_type(T[i], qpf01[i],
						    pop01[i], cprob_rain[i],
						    cprob_snow[i], cprob_ice[i]);
		}
	      
	      vaisala_precip_state[i] = VAISALA_PRECIP_NONE;
	      
	      // calc pop03_pct
	      if (pop03[i] != NC_FILL_FLOAT)
		pop03_pct[i] = 100. * pop03[i];	  
	      
	      if (qpf01[i] != NC_FILL_FLOAT)
		{
		  // precip_rate
		  if (precip_type[i] == C_PTYPE_NONE)
		    {
		      precip_rate[i] = 0.;
		      precip_rate_inches[i] = 0.;
		    }
		  else
		    {
		      precip_rate[i] = qpf01[i];
		      //printf("%d precip_rate %f  qpf01 %f  pop01 %.3f \n", i, precip_rate[i], qpf01[i], pop01[i]);
		      precip_rate_inches[i] = qpf01[i] / MM_PER_INCH;
		    }

		  // vaisala precip state
		  if (precip_type[i] == C_PTYPE_NONE)
		    vaisala_precip_state[i] = VAISALA_PRECIP_NONE;
		  else if (precip_type[i] == C_PTYPE_SNOW)
		    {
		      if (precip_rate[i] < 2.0)
			vaisala_precip_state[i] = VAISALA_PRECIP_LIGHT_SNOW;
		      else if (precip_rate[i] < 4.0)
			vaisala_precip_state[i] = VAISALA_PRECIP_MEDIUM_SNOW;
		      else
			vaisala_precip_state[i] = VAISALA_PRECIP_HEAVY_SNOW;
		    }
		  else
		    {
		      if (precip_rate[i] < 2.0)
			vaisala_precip_state[i] = VAISALA_PRECIP_LIGHT_RAIN;
		      else if (precip_rate[i] < 4.0)
			vaisala_precip_state[i] = VAISALA_PRECIP_MEDIUM_RAIN;
		      else
			vaisala_precip_state[i] = VAISALA_PRECIP_HEAVY_RAIN;
		    }

		  // initialize precip_accum
		  if ( precip_rate[i] == NC_FILL_FLOAT)
		    {
                      precip_accum[i] = NC_FILL_FLOAT;
                      precip_accum_inches[i] = NC_FILL_FLOAT;
                    }
		  else if (precip_type[i] == C_PTYPE_NONE)
		    {
		      precip_accum[i] = 0.;
		      precip_accum_inches[i] = 0.;
		    }
		  else
		    {
		      precip_accum[i] = precip_rate[i];
		      precip_accum_inches[i] = precip_rate[i] / MM_PER_INCH;
		    }

		  // add in precip accum over previous hours
		  for (int j=1; j < 3; j++)
		    {
		      if (hr - j >= 0)
			{
			  if (precip_accum[i] == NC_FILL_FLOAT ||
			      precip_rate[i] == NC_FILL_FLOAT ||
			      precip_rate[i-j] == NC_FILL_FLOAT)
			    {
			      // Missing data does not allow an hourly sum
			      precip_accum[i] = NC_FILL_FLOAT;
			      precip_accum_inches[i] = NC_FILL_FLOAT;
			    }
			  else
			    {
			      precip_accum[i] += precip_rate[i-j];
			      precip_accum_inches[i] += precip_rate_inches[i-j];

			    }
			}
		    }

		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  int fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on qpf03 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // add in qpf03 over previous 3 hours
		  int qpf1_cnt = 0;
		  float qpf03_sum = 0;
		  for (int j=0; j < 3; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (qpf01[i] != NC_FILL_FLOAT &&
			      qpf01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      qpf03_sum += qpf01[i-j];
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for qpf3 summing up qpf01[%d]: %f, qpf03_sum: %f\n", i, j, hr, i-j, qpf01[i-j], i, qpf03_sum);
			      if (hr - j > forc_start_index)
				qpf1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing qpf01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set qpf03 only if we have 3 qpf1 values
		  if(qpf1_cnt == 3)
		    qpf03[i] = qpf03_sum;
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, qpf1_cnt: %d,  qpf03_sum: %f, qpf03[%d]: %f\n", i, hr, fcst_hour, qpf1_cnt, qpf03_sum, i, qpf03[i]);


		  //-------------------------------------------------------------------


		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on pop03 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // work on pop03 over previous 3 hours
		  int pop1_cnt = 0;
		  float pop01_max = 0;
		  float no_pop01_indep = 1.;
		  for (int j=0; j < 3; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (pop01[i] != NC_FILL_FLOAT &&
			      pop01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      if (pop01[i-j] > pop01_max)
				pop01_max = pop01[i-j];

			      no_pop01_indep = no_pop01_indep * (1. - pop01[i-j]);
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for pop03 from pop01[%d]: %f, %f: %f\n", i, j, hr, i-j, pop01[i-j], pop01_max, no_pop01_indep);
			      if (hr - j > forc_start_index)
				pop1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing pop01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set pop03 only if we have 3 pop1 values
		  if(pop1_cnt == 3)
		    {
		      float pop01_indep = 1. - no_pop01_indep;
		      der_pop03[i] = 0.5 * (pop01_indep + pop01_max);
		    }
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, pop1_cnt: %d,  pop01_max: %f, no_pop01_indep: %f pop03[%d]: %f\n", i, hr, fcst_hour, pop1_cnt, pop01_max, no_pop01_indep, i, der_pop03[i]);

		  //-------------------------------------------------------------------

		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on pop06 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // work on pop06 over previous 6 hours
		  pop1_cnt = 0;
		  pop01_max = 0;
		  no_pop01_indep = 1.;
		  for (int j=0; j < 6; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (pop01[i] != NC_FILL_FLOAT &&
			      pop01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      if (pop01[i-j] > pop01_max)
				pop01_max = pop01[i-j];

			      no_pop01_indep = no_pop01_indep * (1. - pop01[i-j]);
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for pop06 from pop01[%d]: %f, %f: %f\n", i, j, hr, i-j, pop01[i-j], pop01_max, no_pop01_indep);
			      if (hr - j > forc_start_index)
				pop1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing pop01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set pop06 only if we have 6 pop1 values
		  if(pop1_cnt == 6)
		    {
		      float pop01_indep = 1. - no_pop01_indep;
		      der_pop06[i] = 0.5 * (pop01_indep + pop01_max);
		    }
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, pop1_cnt: %d,  pop01_max: %f, no_pop01_indep: %f pop06[%d]: %f\n", i, hr, fcst_hour, pop1_cnt, pop01_max, no_pop01_indep, i, der_pop06[i]);

		  //-------------------------------------------------------------------

		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on pop_prev24 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // work on pop24 over previous 24 hours
		  pop1_cnt = 0;
		  pop01_max = 0;
		  no_pop01_indep = 1.;
		  for (int j=0; j < 24; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (pop01[i] != NC_FILL_FLOAT &&
			      pop01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      if (pop01[i-j] > pop01_max)
				pop01_max = pop01[i-j];

			      no_pop01_indep = no_pop01_indep * (1. - pop01[i-j]);
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for pop_prev24 from pop01[%d]: %f, %f: %f\n", i, j, hr, i-j, pop01[i-j], pop01_max, no_pop01_indep);
			      if (hr - j > forc_start_index)
				pop1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing pop01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set pop24 only if we have 24 pop1 values
		  if(pop1_cnt == 24)
		    {
		      float pop01_indep = 1. - no_pop01_indep;
		      der_pop24[i] = 0.5 * (pop01_indep + pop01_max);
		    }
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, pop1_cnt: %d,  pop01_max: %f, no_pop01_indep: %f pop_prev24[%d]: %f\n", i, hr, fcst_hour, pop1_cnt, pop01_max, no_pop01_indep, i, der_pop24[i]);

		  //-------------------------------------------------------------------

		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on qpf06 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // add in qpf06 over previous 6 hours
		  qpf1_cnt = 0;
		  float qpf06_sum = 0;
		  for (int j=0; j < 6; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (qpf01[i] != NC_FILL_FLOAT &&
			      qpf01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      qpf06_sum += qpf01[i-j];
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for qpf06 summing up qpf01[%d]: %f, qpf06_sum: %f\n", i, j, hr, i-j, qpf01[i-j], i, qpf06_sum);
			      if (hr - j > forc_start_index)
				qpf1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing qpf01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set qpf06 only if we have 3 qpf1 values
		  if(qpf1_cnt == 6)
		    qpf06[i] = qpf06_sum;
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, qpf1_cnt: %d,  qpf06_sum: %f, qpf06[%d]: %f\n", i, hr, fcst_hour, qpf1_cnt, qpf06_sum, i, qpf06[i]);


		  // Get the forecast hour
		  utime2hour((time_t)forc_start_hour, fcst_hour_str);
		  fcst_hour = atoi(fcst_hour_str);
		  proc_log->write_time(3, "Working on qpf24 for hr: %d, fcst_hour: %d\n", hr, fcst_hour);
		  
		  // add in qpf24 over previous 24 hours
		  qpf1_cnt = 0;
		  float qpf24_sum = 0;
		  for (int j=0; j < 24; j++)
		    {
		      if (hr >= fcst_hour && hr - j >= 0)
			{ 
			  if (qpf01[i] != NC_FILL_FLOAT &&
			      qpf01[i-j] != NC_FILL_FLOAT &&
			      i -j > fcst_hour )
			    {
			      qpf24_sum += qpf01[i-j];
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, for qpf24 summing up qpf01[%d]: %f, qpf24_sum: %f\n", i, j, hr, i-j, qpf01[i-j], i, qpf24_sum);
			      if (hr - j > forc_start_index)
				qpf1_cnt++; 
			    } 
			  else
			    {
			      proc_log->write_time(3, "i: %d, j: %d, hr: %d, found missing qpf01 data, skipping\n ", i, j, hr);
			    }
			}
		    } // end loop over previous hours (j)
		  
		  // Set qpf24 only if we have 24 qpf1 values
		  if(qpf1_cnt == 24)
		    qpf24[i] = qpf24_sum;
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, qpf1_cnt: %d,  qpf24_sum: %f, qpf24[%d]: %f\n", i, hr, fcst_hour, qpf1_cnt, qpf24_sum, i, qpf24[i]);
		  
		  // Calc snow rate
		  if ( precip_rate[i] == NC_FILL_FLOAT)
		    {
		      snow_rate[i] = NC_FILL_FLOAT;
		      snow_rate_inches[i] = NC_FILL_FLOAT;
		    }
		  else if (precip_type[i] == C_PTYPE_SNOW)
		    {
		      float snow_ratio = calc_snow_to_swe_ratio(T[i], wspd[i]);
		      snow_rate[i] = precip_rate[i] * snow_ratio;
		      snow_rate_inches[i] = precip_rate_inches[i] * snow_ratio;
		      // printf("T %f  wspd %f   snow_ratio %f\n", T[i], wspd[i], snow_ratio);
		      /*
			snow_rate[i] = precip_rate[i] * SNOW_TO_SWE_RATIO;
			snow_rate_inches[i] = precip_rate_inches[i] * SNOW_TO_SWE_RATIO;
		      */
		    }
		  else
		    {
		      snow_rate[i] = 0.;
		      snow_rate_inches[i] = 0.;
		    }
		  
		  // Calc kuchera snow rate
		  // Use kuchera_snow_ratio from previous hour and current hour
		  if(found_kuchera_snow_rate && found_kuchera_snow_ratio)
		    { 
		      float avg_ratio;
		      if(i != 0 && kuchera_snow_ratio[i-1] != NC_FILL_FLOAT && kuchera_snow_ratio[i] != NC_FILL_FLOAT)
			{
			  //printf("  calculating avg_ratio using kuchera_snow_ratio[%d]: %f and kuchera_snow_ratio[%d]: %f\n", i-1, kuchera_snow_ratio[i-1], i, kuchera_snow_ratio[i]);
			  avg_ratio = (kuchera_snow_ratio[i-1] + kuchera_snow_ratio[i]) / 2;
			}
		      else if(kuchera_snow_ratio[i] != NC_FILL_FLOAT)
			{
			  //printf("  calculating avg_ratio using just kuchera_snow_ratio[%d]: %f\n", i, kuchera_snow_ratio[i]);
			  avg_ratio = kuchera_snow_ratio[i];
			}
		      else
			{
			  //printf("  could not calculate avg_ratio, setting to NC_FILL_FLOAT\n");
			  avg_ratio = NC_FILL_FLOAT;
			}
		      
		      // Set kuchera_snow_rate regardless of precip-type
		      if (precip_rate[i] == NC_FILL_FLOAT)
			kuchera_snow_rate[i] = NC_FILL_FLOAT;
		      else if(avg_ratio == NC_FILL_FLOAT)
			kuchera_snow_rate[i] = NC_FILL_FLOAT;
		      else if(avg_ratio < 3)
			kuchera_snow_rate[i] = 0;
		      else
			kuchera_snow_rate[i] = precip_rate[i] * avg_ratio;
		      
		      // If the rate is less than 1 mm /hr set to 0
		      if(kuchera_snow_rate[i] < 1)
			kuchera_snow_rate[i] = 0;
		      
		      // If the temperature is greater than 4 degC, set rate to 0
		      if(T[i] != NC_FILL_FLOAT && T[i] > 4)
			kuchera_snow_rate[i] = 0;
		      
		      proc_log->write_time(3, "i: %d, hr: %d, precip_rate: %f, avg_ratio: %f, kuchera_snow_rate: %f\n", i, hr, precip_rate[i], avg_ratio, kuchera_snow_rate[i]);
		    }
		  else if(found_kuchera_snow_rate && !found_kuchera_snow_ratio && warn_cnt == 0)
		    {
		      proc_log->write_time("Warning: Unable to calculate kuchera_snow_rate without kuchera_snow_ratio, values will be set to NC_FILL_FLOAT\n");
		      warn_cnt++;
		    }
		  
		  
		  // Calc ice rate (freezing rain or sleet rate)
		  if ( precip_rate[i] == NC_FILL_FLOAT)
		    {
		      ice_rate[i] = NC_FILL_FLOAT;
		    }
		  else if (precip_type[i] == C_PTYPE_FRZRAIN)
		    {
		      ice_rate[i] = precip_rate[i] * 1.09;
		    }
		  else
		    {
		      ice_rate[i] = 0.;
		    }
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, ice_rate: %f\n", i, hr, fcst_hour, ice_rate[i]);
		  
		  // Initialize snow_accum
		  if (snow_rate[i] != NC_FILL_FLOAT)
		    {
		      snow_accum[i] =  snow_rate[i];
		      snow_accum_inches[i] =  snow_rate_inches[i];
		    }
		  else
		    {
		      snow_accum[i] = NC_FILL_FLOAT;
		      snow_accum_inches[i] = NC_FILL_FLOAT;
		    }
                  
		  // Initialize ice_accum
		  if (ice_rate[i] != NC_FILL_FLOAT)
		    {
		      ice_accum[i] =  ice_rate[i];
		    }
		  else
		    {
		      ice_accum[i] = NC_FILL_FLOAT;
		    }
                  
		  // Calc snow accum and ice accum (currently over 3 hours)
		  for (int j=1; j<3; j++)
		    {
		      //printf("hr %d  j %d  snow_rate %f %f %f\n", hr, j,snow_rate[i], snow_rate[i-1], snow_rate[i-2]);
		      //printf("hr %d  j %d  ice_rate %f %f %f\n", hr, j,ice_rate[i], ice_rate[i-1], ice_rate[i-2]);
		      if (hr - j >= 0)
			{
			  // Do snow
			  if (snow_accum[i] == NC_FILL_FLOAT ||
			      snow_rate[i] == NC_FILL_FLOAT ||
			      snow_rate[i-j] == NC_FILL_FLOAT)
			    {
			      // Missing data does not allow an hourly sum
			      snow_accum[i] = NC_FILL_FLOAT;
			      snow_accum_inches[i] = NC_FILL_FLOAT;
			    }
			  else
			    {
			      //printf("%d %d  %f %f %f\n", i, j, snow_rate[i], snow_rate[i-j], snow_accum[i]);
			      snow_accum[i] += snow_rate[i-j];
			      snow_accum_inches[i] += snow_rate_inches[i-j];
			    }
			  
			  // Do ice
			  if (ice_accum[i] == NC_FILL_FLOAT ||
			      ice_rate[i] == NC_FILL_FLOAT ||
			      ice_rate[i-j] == NC_FILL_FLOAT)
			    {
			      // Missing data does not allow an hourly sum
			      ice_accum[i] = NC_FILL_FLOAT;
			    }
			  else
			    {
			      //printf("  %d %d  %f %f %f\n", i, j, ice_rate[i], ice_rate[i-j], ice_accum[i]);
			      ice_accum[i] += ice_rate[i-j];
			    }
			  
			} // end if hr >= 0
		    } // end loop over accumulation hours (j)
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, ice_accum: %f\n", i, hr, fcst_hour, ice_accum[i]);
		  
		  // calc snow_accum_total
		  if (hr < (int)((forc_start_hour - forc_start_time)/ SEC_PER_HOUR))
                    {
                      snow_accum_total[i] = NC_FILL_FLOAT;
                      snow_accum_total_inches[i] = NC_FILL_FLOAT;
		      snow_total = 0;
		    }
		  else if (hr == (int)((forc_start_hour - forc_start_time)/ SEC_PER_HOUR))
		    {
		      snow_accum_total[i] = 0.;
		      snow_accum_total_inches[i] = 0.;
		      snow_total = 0;
		    }
		  else if (snow_rate[i] != NC_FILL_FLOAT)
		    {
		      snow_accum_total[i] =  snow_total + snow_rate[i];
		      snow_accum_total_inches[i] =  snow_accum_total[i] / MM_PER_INCH;
		      snow_total = snow_accum_total[i];
		    }
		  else
		    {
		      snow_accum_total[i] = NC_FILL_FLOAT;
		      snow_accum_total_inches[i] = NC_FILL_FLOAT;
		      snow_total = 0.;
		    }
		  
		  // calc ice_accum_total
		  if (hr < (int)((forc_start_hour - forc_start_time)/ SEC_PER_HOUR))
                    {
                      ice_accum_total[i] = NC_FILL_FLOAT;
                      ice_total = 0;
		    }
		  else if (hr == (int)((forc_start_hour - forc_start_time)/ SEC_PER_HOUR))
		    {
		      ice_accum_total[i] = 0.;
		      ice_total = 0;
		    }
		  else if (ice_rate[i] != NC_FILL_FLOAT)
		    {
		      ice_accum_total[i] =  ice_total + ice_rate[i];
		      ice_total = ice_accum_total[i];
		    }
		  else
		    {
		      ice_accum_total[i] = NC_FILL_FLOAT;
		      ice_total = 0.;
		    }
		  
		  proc_log->write_time(3, "i: %d, hr: %d, fcst_hour: %d, ice_accum_total: %f\n", i, hr, fcst_hour, ice_accum_total[i]);
		  
		} // end if qpf01 is not missing
	    } // end loop over out_array values

	  // For snow_accum_72hr_total, use the snow_accum_total
	  // value at 72 hours at each time for each site.

	  for (int i=0; i<max_output_sites; i++)
	    {
	      int sn72_index = (i * num_output_days * num_output_times_per_day) + (int)(forc_start_hour + 72 * SEC_PER_HOUR - forc_start_time) / (SEC_PER_DAY / num_output_times_per_day);
	      int start = (int)(forc_start_hour - forc_start_time) / (SEC_PER_DAY / num_output_times_per_day);
	      for (int ii=start; ii<num_output_days*num_output_times_per_day; ii++)
		{
		  snow_accum_72hr_total[i * num_output_days * num_output_times_per_day + ii] = snow_accum_total[sn72_index];
		  snow_accum_72hr_total_inches[i * num_output_days * num_output_times_per_day + ii] = snow_accum_total_inches[sn72_index];
		}
	    }
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_rate"))
	      write_mod_fcst(&output_file, (char *)"precip_rate", precip_rate);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_rate_inches"))
	      write_mod_fcst(&output_file, (char *)"precip_rate_inches", precip_rate_inches);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_type"))
	      write_mod_fcst(&output_file, (char *)"precip_type", precip_type);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_accum"))
	      write_mod_fcst(&output_file, (char *)"precip_accum", precip_accum);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_accum_inches"))
	      write_mod_fcst(&output_file, (char *)"precip_accum_inches", precip_accum_inches);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "cprob_rain"))
	      write_mod_fcst(&output_file, (char *)"cprob_rain", cprob_rain);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "cprob_snow"))
	      write_mod_fcst(&output_file, (char *)"cprob_snow", cprob_snow);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "cprob_ice"))
	      write_mod_fcst(&output_file, (char *)"cprob_ice", cprob_ice);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum"))
	      write_mod_fcst(&output_file, (char *)"snow_accum", snow_accum);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum_inches"))
	      write_mod_fcst(&output_file, (char *)"snow_accum_inches", snow_accum_inches);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum_total"))
	      write_mod_fcst(&output_file, (char *)"snow_accum_total", snow_accum_total);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum_total_inches"))
	      write_mod_fcst(&output_file, (char *)"snow_accum_total_inches", snow_accum_total_inches);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum_72hr_total"))
	      write_mod_fcst(&output_file, (char *)"snow_accum_72hr_total", snow_accum_72hr_total);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_accum_72hr_total_inches"))
	      write_mod_fcst(&output_file, (char *)"snow_accum_72hr_total_inches", snow_accum_72hr_total_inches);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_rate"))
	      write_mod_fcst(&output_file, (char *)"snow_rate", snow_rate);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "snow_rate_inches"))
	      write_mod_fcst(&output_file, (char *)"snow_rate_inches", snow_rate_inches);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "ice_rate"))
	      write_mod_fcst(&output_file, (char *)"ice_rate", ice_rate);
	  
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "ice_accum"))
	      write_mod_fcst(&output_file, (char *)"ice_accum", ice_accum);
	  
	   for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "ice_accum_total"))
	      write_mod_fcst(&output_file, (char *)"ice_accum_total", ice_accum_total);

	   for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "kuchera_snow_rate"))
	      write_mod_fcst(&output_file, (char *)"kuchera_snow_rate", kuchera_snow_rate);
 
	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "prob_precip03_pct"))
	      write_mod_fcst(&output_file, (char *)"prob_precip03_pct", pop03_pct);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "prob_precip01"))
	      write_mod_fcst(&output_file, (char *)"prob_precip01", pop01);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "prob_precip03"))
	      write_mod_fcst(&output_file, (char *)"prob_precip03", der_pop03);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "prob_precip06"))
	      write_mod_fcst(&output_file, (char *)"prob_precip06", der_pop06);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "prob_precip_prev24"))
	      write_mod_fcst(&output_file, (char *)"prob_precip_prev24", der_pop24);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "qpf01"))
	      write_mod_fcst(&output_file, (char *)"qpf01", qpf01);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "qpf03"))
	      write_mod_fcst(&output_file, (char *)"qpf03", qpf03);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "qpf06"))
	      write_mod_fcst(&output_file, (char *)"qpf06", qpf06);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "qpf24"))
	      write_mod_fcst(&output_file, (char *)"qpf24", qpf24);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_cn1"))
	      write_mod_fcst(&output_file, (char *)"precip_cn1", cqpf01);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "precip_cn3"))
	      write_mod_fcst(&output_file, (char *)"precip_cn3", cqpf03);

	  for (np=0; np<num_derived_vars; np++)
	    if (0 == strcmp(conf_params._derived_var_names[np], "vaisala_precip_state"))
	      write_mod_fcst(&output_file, (char *)"vaisala_precip_state", vaisala_precip_state);


	  delete [] T;
	  delete [] wspd;
	  delete [] qpf03;
	  delete [] pop03;
	  delete [] cqpf03;
	  delete [] cqpf06;
	  delete [] cpos;
	  delete [] cpor;
	  delete [] cpoi;
	  delete [] kuchera_snow_ratio;
	  
	  delete [] precip_rate;
	  delete [] precip_rate_inches;
	  delete [] precip_type;
	  delete [] precip_accum;
	  delete [] precip_accum_inches;
	  delete [] cprob_rain;
	  delete [] cprob_snow;
	  delete [] cprob_ice;
	  delete [] snow_accum;
	  delete [] snow_accum_inches;
	  delete [] snow_accum_total;
	  delete [] snow_accum_total_inches;
	  delete [] snow_accum_72hr_total;
	  delete [] snow_accum_72hr_total_inches;
	  delete [] snow_rate;
	  delete [] snow_rate_inches;
	  delete [] ice_rate;
	  delete [] ice_accum;
	  delete [] ice_accum_total;
	  delete [] kuchera_snow_rate;
	  delete [] pop03_pct;
	  delete [] qpf01;
	  delete [] qpf24;
	  delete [] pop01;
	  delete [] vaisala_precip_state;
	}
    }

  // Stub in blowing snow derived var
  found = 0;
  for( np = 0; np < num_derived_vars; np++ ) {
    if ( 0 == strcmp( conf_params._derived_var_names[np], 
                     "blowing_snow_potential" ) ||
         0 == strcmp( conf_params._derived_var_names[np],
                     "blowing_snow_pot_index") ) {
       found = 1;
       break;
    }
  }

  if ( found ) {
       
     //
     // Set up objects
     //   max_history     = maximum amount of obs history in days
     //   forc_start_hour = forecast time entered by user to the 
     //                     nearest hour
     // 
     ConcatData concatData( conf_params.max_history, forc_start_hour, 
                            debug_level );
     SnowCalc   snowCalc( num_output_days, num_output_times_per_day,
                          max_output_sites );

     //
     // Initialize array and file
     //
     float *blowing_snow_potential = snowCalc.getBlowingSnowIndex();
     float *blowing_snow_pot_vals  = snowCalc.getBlowingSnowVals();
     write_mod_fcst( &output_file, (char *)"blowing_snow_potential", 
                     blowing_snow_potential );
     write_mod_fcst( &output_file, (char *)"blowing_snow_pot_vals", 
                     blowing_snow_pot_vals );
       
     //
     // Get ready to concatinate data
     //   obs_config_file   = name of obs config file
     //   input_file_name   = input forecast file name
     //   output_site_array = output site id array
     //   num_output_sites  = number of output sites in the id array
     //   num_output_days   = number of output forecast days
     //
     if( concatData.init( obs_config_file, input_file_name, 
                          output_site_array, num_output_sites,
                          num_output_days ) != 0 ) {
          
        proc_log->write_time_error( "Couldn't concatinate data for "
                                    "blowing snow calculation\n" );
        return( 1 );

     }
       
     //
     // Get input data
     //
     int     numValsPerSite = concatData.getNumValsPerSite();
     float  *wspd           = concatData.getWspd();
     float  *T              = concatData.getT();
     float  *snow           = concatData.getSnow();
     float  *rain           = concatData.getRain();
     float  *ice            = concatData.getIce();
     double *dataTime       = concatData.getDataTime();

     //
     // Set up the time offset
     //   forc_start_hour = user defined forecast time
     //   forc_time       = forecast time read from file
     //   
     //   If the time in the input forecast file begins at
     //   say 00:00Z and we are forecasting at say 18:00Z
     //   we need to skip over the first 18 hours worth of
     //   data in the forecast file.  Set an offset to 
     //   handle this.
     //   
     int timeDiffSecs    = (int) (forc_start_hour - forc_time);        
     int timeDiffPeriods = timeDiffSecs / 
       (SEC_PER_DAY / num_input_times_per_day);
       
     //
     // Calculate the blowing snow potential
     //
     if( snowCalc.calc( wspd, T, rain, snow, ice, dataTime,
                        num_output_sites, numValsPerSite,
                        timeDiffPeriods ) != 0 ) {
        proc_log->write_time_error( "Couldn't calculate blowing snow "
                                    "potential\n" );
        return( 1 );
     }
       
     //
     // Write values to file
     //
     blowing_snow_potential = snowCalc.getBlowingSnowIndex();
     blowing_snow_pot_vals  = snowCalc.getBlowingSnowVals();
     write_mod_fcst( &output_file, (char *)"blowing_snow_potential", 
                     blowing_snow_potential );
     write_mod_fcst( &output_file, (char *)"blowing_snow_pot_vals", 
                     blowing_snow_pot_vals );

     //
     // Debugging
     //
     if( debug_level >= MIN_PRINT_LEVEL ) {
        float* pop  = concatData.getPop();
        float* qpf  = concatData.getQpf();
        float* cpos = concatData.getCpos();
        float* cpor = concatData.getCpor();
        float* cpoi = concatData.getCpoi();
        
        snowCalc.printData( output_site_array, dataTime, T, pop,
                            qpf, cpos, cpor, cpoi, snow, rain, ice,
                            wspd, num_output_sites, numValsPerSite,
                            timeDiffPeriods );
     }
  }


  delete [] fcst_data;
  delete [] output_data;
  delete [] output_site_array;
  delete [] index_array;
  delete [] output_ll_array;

  delete ncErr;

  return(0);
}


int readObsFiles( long maxHist, double forecastTime, char* obsConfigFile, int* siteIdArray, int numSites, vector< ObsFile* > &obsFiles) 
{
  const int FILE_NAME_LEN = 120;
  
  //
  // Open the config file in read mode
  //
  FILE *fp = fopen( obsConfigFile, "r" );

  if ( fp == NULL ) {
    //proc_log->write_time("Error: Could not open %s\n", obsConfigFile );
    return( 1 );
  }
  
  //
  // Read the first maxHist lines from the config file
  // into the inputFile array.
  //   Note:  If we find a line that is less than 2 characters
  //   long, we assume we are done reading the file.
  //
  int i = 0;
  char tmpStr[FILE_NAME_LEN];
  while ( i < maxHist &&
	  NULL != fgets( tmpStr, FILE_NAME_LEN, fp ) && 
	  strlen( tmpStr ) > 2 ) {
    
    //
    // Take care of null termination
    //
    if( tmpStr[strlen( tmpStr ) - 1] == '\n' )
      tmpStr[ strlen( tmpStr ) - 1] = '\0';
      
    //
    // Get the file name
    //   Note that the memory associated with this string
    //   will be owned by the ObsFile object that it gets
    //   passed into
    //
    string *newString = new string( tmpStr );
      
    //
    // Set up the obs file object
    //
    ObsFile *newObsFile = new ObsFile( forecastTime );
      
    if( newObsFile->init( newString, siteIdArray, numSites ) != 0 ) {
      proc_log->write_time( 1, "Unable to use obs file %s\n", 
			    newString->c_str() );
    }
    else {
      
      //
      // Put it in the list
      //
      obsFiles.push_back( newObsFile );

      //
      // Only count this file toward the max history if
      // we can use it
      //
      i++;
    }
  }

  //
  // Close the file
  //
  fclose( fp );

  return( 0 );
}


float* fillInArray(float* fcstData, int numOutputSites, int fcstDays, int orig_fcstTimesPerDay, int &fcstTimesPerDay)
{
  // Set the new fcst_times_per_day
  // and figure out the original ftpd time resolution
  fcstTimesPerDay = 24;
  int ftpd_res = fcstTimesPerDay / orig_fcstTimesPerDay;
  
  //
  // First set of loops to fill in the 24 times_per_day array 
  // with missing values and real forecast values when available
  //
  float *dataArray = new float[numOutputSites*fcstDays*fcstTimesPerDay];

  for(int ns = 0; ns < numOutputSites; ns++ ){
    for(int nd = 0; nd < fcstDays; nd++){
      for(int nt = 0; nt < fcstTimesPerDay; nt++){
      
	int outputIndex = (ns * fcstDays * fcstTimesPerDay) + (nd * fcstTimesPerDay) + nt;
	dataArray[outputIndex] = NC_FILL_FLOAT;

	int nt_rem = nt % ftpd_res;
	if(nt_rem == 0){
	  int input_nt = nt / ftpd_res;
	  int inputIndex = (ns * fcstDays * orig_fcstTimesPerDay) + (nd * orig_fcstTimesPerDay) + input_nt;
	  dataArray[outputIndex] = fcstData[inputIndex];
	}
	
      } // end loop over fcstTimesPerDay
    } // end loop over fcstDays
  } // end loop over numOutputSites
  

  //
  // Second set of loops to perform cubic spline interpolation
  //
  int ts_size = fcstDays * fcstTimesPerDay;
  for(int i = 0; i < numOutputSites; i++ ) {
    
    //
    // Create arrays used by spline function. x represents time,
    // y the variable, and y2 the second derivative of y at x.
    //
    double *x  = new double[ts_size];
    double *y  = new double[ts_size];
    double *y2 = new double[ts_size];
    
    //
    // Fill arrays with time series data
    //
    int num_spl_vals = 0;
    proc_log->write_time(5, "Original values\n");
    for(int t = 0; t < ts_size; t++) {
      
      x[num_spl_vals] = (double)t;
      y[num_spl_vals] = (double)dataArray[i*ts_size + t];
      proc_log->write_time(5, "site index = %d, time index = %d, dataArray[%d] = %f\n", i, t, (int)(i*ts_size + t), dataArray[i*ts_size + t]);
      
      // Do not count fill values in the spline arrays
      if (y[num_spl_vals] != (double)NC_FILL_FLOAT)
	num_spl_vals++;
      
    } // end loop over fcst times (ts_size)
    
    //
    // Call spline function to get second derivatives. Fill values
    // at the end points will cause this to be a natural cubic spline.
    //
    init_spline(x, y, num_spl_vals, NC_FILL_DOUBLE, NC_FILL_DOUBLE, y2);
    
    //
    // Loop over output times again, get splined value, save in time series
    //
    proc_log->write_time(5, "Splined values\n");
    for (int t = 0; t < ts_size; t++){
      dataArray[i*ts_size + t] = (float)calc_splint(x, y, y2, num_spl_vals, (double)t, NC_FILL_DOUBLE);
      proc_log->write_time(5, "site index = %d, time index = %d, dataArray[%d] = %f\n", i, t, (int)(i*ts_size + t), dataArray[i*ts_size + t]);
    } // end loop over fcst times (ts_size)
    
  } // end loop over numOutputSites
  
  return ( dataArray );
}


float* concatVarData(vector< ObsFile* > obsFiles, const string& obsVarName, int numOutputSites, float *fcstData,  double forecastTime, int fcstDays, int fcstTimesPerDay, int& numVals, int& numObsVals) 
{
  //
  // Initialize numVals so that if we return with an
  // error, it makes sense
  //
  numVals = 0;
   
  //
  // Make sure we have obs data
  //
  if( obsFiles.size() < 1 ) {
    proc_log->write_time_error( "No obs files\n" );
    return( NULL );
  }
  
  //
  // Get obs data and sizes
  //   Put the obs data in these structures in reverse order
  //   so that we have the oldest file first.  Note that we
  //   assume that this value will be the same over all obs
  //   files
  //
  int obsTimesPerDay = obsFiles[0]->getNumTimesPerDay();
  
  vector< long >   obsSize;
  vector< float* > obsData;
  vector< double* > obsTime;
   
  int numObs = 0;
  
  for( int no = (int) obsFiles.size() - 1; no >= 0; no-- ) {
    long   currentObsSize = obsFiles[no]->getNumTimesUsed();
    float *currentObsData = obsFiles[no]->getData( obsVarName );
    double *currentObsTime = obsFiles[no]->getTime();
 
    if( !currentObsData ) {
      return( NULL );
    }
      
    obsSize.push_back( currentObsSize );
    obsData.push_back( currentObsData );
    obsTime.push_back( currentObsTime );
    
    numObs += currentObsSize;
  }
  //cout << "Initial numObs = " << numObs << endl;


  //
  // Make sure the number of observations per day match
  // the number of forecasts per day
  //
  if( fcstTimesPerDay != obsTimesPerDay ) {
    proc_log->write_time_error( "Number of times per day do not match between obs data and forecast data\n" );
    return( NULL );
  }
  
  //
  // Find the latest obs_time
  //
  double latest_obs_time = -9999999.;
  for( int no = 0; no < (int) obsData.size(); no++ ) {
    double *timeArray = obsTime[no];
    for( int nt = 0; nt < obsSize[no]; nt++ ) {
      if(timeArray[nt] > latest_obs_time){
	latest_obs_time = timeArray[nt] ;
      }
    }
  }
  //cout << "latest_obs_time = " << (int)latest_obs_time << endl;


  //
  // Set the forecast_start_time by adding 1-hour to the forecastTime
  // Get the first fcst_hour to determine the number of overlap values
  //
  time_t forecast_start_time = (time_t)forecastTime + 3600;
  //cout << "forecast_start_time = " << forecast_start_time << endl;
  
  char *obs_day_str;
  char *fcst_day_str;
  char *fcst_hour_str;
  
  utime2day((time_t)latest_obs_time, obs_day_str);
  utime2day(forecast_start_time, fcst_day_str);
  utime2hour(forecast_start_time, fcst_hour_str);
  
  int obs_day = atoi(obs_day_str);
  int fcst_day = atoi(fcst_day_str);
  int fcst_hour = atoi(fcst_hour_str);
  
  //
  // Set the number of overlap values
  // If fcst_hour is 0 we want to use all obs on day 0,
  // thus the number of overlap values = 24, otherwise
  // the number of overlap values = fcst_hour
  //
  int num_overlap_vals;
  if(fcst_hour == 0){
    num_overlap_vals = 24;
  } else{
    num_overlap_vals = fcst_hour;
  }
  
  //
  // Check the difference between the forecast_start_time and the latest_obs_time
  // Set the obsData start index (either 0 to start with the first file or 1 to start with the second file)
  // If we start with the second file subtract 24 from numObs to get the correct number of total concatenated values
  // and set the number of overlap values to 0
  //
  int no_start;
  time_t diff = forecast_start_time - (time_t)latest_obs_time;
  if(diff >= 86400){
    proc_log->write_time("Warning: the latest_obs_time %d is greater than or equal to 24 hours old, need more recent obs to concatenate\n", (int)latest_obs_time);
    return(NULL);
  } else if(diff > 3600){
    proc_log->write_time("Info: the latest_obs_time %d is greater than 1 hour old, assume obs file is from previous day\n", (int)latest_obs_time);
    numObs = numObs - 24;
    num_overlap_vals = 0;
    no_start = 1;
  } else{
    no_start = 0;
  }
  //cout << "numObs = " << numObs << endl;
  //cout << "num_overlap_vals = " << num_overlap_vals << endl;
  //cout << "no_start = " << no_start << endl;
  
  //
  // Initialize the dataArray
  //
  long fcstValsPerSite = fcstDays * fcstTimesPerDay;
  int numValsPerSite = (numObs + fcstValsPerSite) - num_overlap_vals;
  
  float *dataArray = new float[numValsPerSite*numOutputSites];

  for( int i = 0; i < numValsPerSite*numOutputSites; i++ ) {
    dataArray[i] = NC_FILL_FLOAT;
  }
   
  //
  // Copy obs data in 
  //
  int obsIndex = 0;
  for( int no = no_start; no < (int) obsData.size(); no++ ) {
    
    float *obsArray = obsData[no];

    //
    // For each site, calculate the indeces and copy
    // the data for the current file in
    //
    for( int ns = 0; ns < numOutputSites; ns++ ) {
      int outputIndex = ns * numValsPerSite + obsIndex;
      int inputIndex  = ns * obsSize[no];
	 
      //cout << "obsSize[" << no << "] = " << obsSize[no] << endl;
      
      for( int nt = 0; nt < obsSize[no]; nt++ ) {
	dataArray[outputIndex] = obsArray[inputIndex];
	outputIndex++;
	inputIndex++;
      } // end loop over times (obsSize)
    
    } // end loop over sites

    obsIndex += obsSize[no];
  } // end loop over obs files
  
  //
  // Copy forecast data in
  //
  //int start_hour = fcst_hour;
  int start_hour = num_overlap_vals;
  for( int ns = 0; ns < numOutputSites; ns++ ) {
    for( int nv = start_hour; nv < fcstValsPerSite; nv++ ) {
    
      int inputIndex = (ns * fcstValsPerSite) + nv;
      int outputIndex = (ns * numValsPerSite + numObs) + (nv - num_overlap_vals);
      
      dataArray[outputIndex] = fcstData[inputIndex];
      
    } // end loop over times (valsPerSite) 
  } // end loop over sites
  
  //
  // Clean up
  //
  obsData.erase( obsData.begin(), obsData.end() );
  obsSize.erase( obsSize.begin(), obsSize.end() );
  
  delete [] obs_day_str;
  delete [] fcst_day_str;
  delete [] fcst_hour_str;
  
  numVals = numValsPerSite;
  numObsVals = numObs;
  
  return( dataArray );
}

void utime2day(time_t utime, char* &day)
{
  const int MAX_STR_LEN = 120;
  
  day = new char[MAX_STR_LEN];
  
  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;

  sprintf(day, "%02d", ct->tm_mday);
}


void utime2hour(time_t utime, char* &hour)
{
  const int MAX_STR_LEN = 120;
  
  hour = new char[MAX_STR_LEN];

  struct tm *ct;
  ct = gmtime(&utime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;

  sprintf(hour, "%02d", ct->tm_hour);
}
