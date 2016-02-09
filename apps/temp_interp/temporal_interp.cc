/*
 * Module: temporal_interp.cc
 *
 * Author: Bill Myers
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
#include <vector>
#include <math.h>
#include <cassert>
#include <iterator>

#include "ncf/ncf.hh"
#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"

#include "log/log.hh"
#include "netcdf.h"
#include "Params.hh"

#include "calc_sun.hh"

// Constant, macro and type definitions 

// TWILIGHT_ZONE is the amount of time before and after sunset that
// we may still have light, in seconds. Twilight duration is really
// a function of latitude, time of year and extent of twilight but
// the value used here should be OK for most mid-latitude sites
// year-round.

//#define TWILIGHT_ZONE 3600
#define TWILIGHT_ZONE 0

// Global variables 
extern Log *proc_log;
extern Params conf_params;


// Functions and objects
extern int read_fcst_info(NcFile *input_file, int * num_input_sites, int * model_id, 
			  double *forc_time, long *num_input_times_per_day,
			  long *num_input_day, long *max_site_num,
			  long *num_param_vals);
extern int read_fcst_data(NcFile *input_file, char *var_name,
			  long *times_per_day, float *data);
extern int create_output_file(char *cdl_name, char * output_name);
extern int write_header(NcFile *output_file, double forc_time, int num_output_sites,
			int model_id, int  *output_site_array, long *max_output_sites,
			long *num_output_days, long *num_output_times_per_day,
			long *num_output_param_vals);
extern int write_mod_fcst(NcFile *output_file, char *var_name,
			 float *fcst_data);
extern void init_spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
extern double calc_splint(double *xa, double *ya, double *y2a, int n, double x,
                          double FILL_VALUE);


//A little function that can be used for testing array values.
//The output is Python compatible.
template <typename T>
void print_array(const T* array, size_t len)
{
  std::cout << "[";
  std::copy(array, array+len, std::ostream_iterator<T>(std::cout,","));
  std::cout << "]\n";
}

//Performs a simple binary search to find the two values in xs that bracket x.
//The distance between these values is then calculated and returned.
double bracket_dist(const double* xs, double x, size_t len)
{
  if (len <= 1 || x < xs[0] || x > xs[len-1])
  {
     return NC_FILL_DOUBLE;
  }

  size_t klo = 0;
  size_t khi = len-1;

  while (khi-klo > 1)
  {
    size_t k = ((khi+klo)/2);
    if (xs[k] > x)
        khi = k;
    else
        klo = k;
  }

  assert(xs[khi] >= xs[klo]);
  return xs[khi] - xs[klo];
}

int temporal_interp(double forc_start_time, char *site_list_file,
		    char *cdl_name, char *input_file_name, char *output_name)
{
  int num_input_sites, num_output_sites, model_id;
  int  *output_site_array = 0;
  long *index_array = 0; 
  int num_fcst_vars;
  long num_params;
  double forc_time;
  long num_input_times_per_day, num_input_day, max_input_sites;
  long num_output_times_per_day, num_output_days, max_output_sites;
  long times_per_day, num_input_param_vals, num_output_param_vals;
  float *fcst_data;
  float *output_data = NULL;
  float *latlon_array = 0;
  int output_data_size = 0;

  int ret;
  int np, ns, nd, nt, nv;
  double forecast_valid_time, time_diff;
  int input_time_step;
  float fp_time_step, interp;
  int input_nd, input_nt, next_nd, next_nt;
  int num_spline_vals;
  double *x, *y, *y2;
  double srise, sset;	      
  int doNightMask;

  int nsOutputMultiplier, ndOutputMultiplier, ntOutputMultiplier;
  int nsInputMultiplier, ndInputMultiplier, ntInputMultiplier;
  int nsOutputAdder, ndOutputAdder, ntOutputAdder;
  int nsInputAdder;

  // Set up behaviour of netCDF library
  NcError* ncErr = new NcError(NcError::silent_nonfatal);

  // Make sure times are at the beginning of the day
  forc_start_time = SEC_PER_DAY * (int)(forc_start_time/SEC_PER_DAY);

  // Open and read input  netcdf file
  NcFile input_file(input_file_name, NcFile::ReadOnly);
  if (!input_file.is_valid())
    {
      proc_log->write_time("Error: Unable to open integrator input file %s\n",input_file_name);
      return(1);
    }

  ret = read_fcst_info(&input_file, &num_input_sites, &model_id, &forc_time, 
		       &num_input_times_per_day, &num_input_day,
		       &max_input_sites, &num_input_param_vals);
  if (ret != 0)
    {
      proc_log->write_time("Error: Unable to read fcst file\n");
      return(1);
    }

  assert(num_input_param_vals >= 1 && "Invalid num_input_param_vals");
  
  // Create space to read in input wx data
  fcst_data = new float[max_input_sites*num_input_day*num_input_times_per_day*num_input_param_vals];

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

  if (output_sites.make_latlon_array(&latlon_array) < 0)
    {
      proc_log->write_time("Error: could not make_latlon_array\n");
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

  // Write output file header info
  ret = write_header(&output_file, forc_start_time, num_output_sites, 
		     model_id, output_site_array, &max_output_sites, &num_output_days,
		     &num_output_times_per_day, &num_output_param_vals);

  assert(num_output_param_vals >= 1 && "Invalid num_output_param_vals");

  if (num_output_sites > max_output_sites)
    {
      proc_log->write_time("Error: too many sites for output cdl\n");
      return(1);
    }

  if(num_output_param_vals > num_input_param_vals)
    {
      proc_log->write_time("Error: Number of output param_vals is greater than input.\n");
      return(1);
    }

  // Create output data array
  output_data = new float[max_output_sites * num_output_days *
			 num_output_times_per_day * num_output_param_vals];

  // Create arrays used by spline function. x represents the time variable,
  // y the forecast variable, and y2 the second derivative of y at x.
  x  = new double[num_input_day * num_input_times_per_day];
  y  = new double[num_input_day * num_input_times_per_day];
  y2 = new double[num_input_day * num_input_times_per_day];
  

  nsOutputMultiplier = num_output_days * num_output_times_per_day * num_output_param_vals;
  ndOutputMultiplier = num_output_times_per_day * num_output_param_vals;
  ntOutputMultiplier = num_output_param_vals;

  nsInputMultiplier = num_input_day * num_input_times_per_day * num_input_param_vals;
  ndInputMultiplier = num_input_times_per_day * num_input_param_vals;
  ntInputMultiplier = num_input_param_vals;


  char *var_name;
  char *interp_type;

  const double DIST_THRESHOLD = conf_params.dist_threshold;

  // Now start main loop
  num_fcst_vars = conf_params.var_list_n;
  for (np = 0; np < num_fcst_vars; np++)
    {

      var_name = conf_params._var_list[np].name;
      interp_type = conf_params._var_list[np].method;

      // Check that interpolation method is known
      if (strcmp(interp_type, "linear") != 0 &&
	  strcmp(interp_type, "cubic_spline") != 0 && 
	  strcmp(interp_type, "cubic_spline_nightmask") != 0 )
	{
	  proc_log->write_time("Error: Unknown interpolation method (%s) for %s\n", interp_type, var_name);
	  return(1);
	}

      // Read input forecast
      ret = read_fcst_data(&input_file, var_name,
			   &times_per_day, fcst_data);
      if (ret != 0)
	{
	  proc_log->write_time("Error: Unable to read data from fcst file\n");
	  return(1);
	}

      for (nt=0; nt<max_output_sites*num_output_days*num_output_times_per_day; nt++)
	for(nv = 0; nv<num_output_param_vals;nv++)
	  if(nv == 2 || nv == 3)
	    output_data[nt] = 0;
	  else
	    output_data[nt] = NC_FILL_FLOAT;

      proc_log->write_time(1, "Info: Var: %s, interp method: %s\n", var_name,
			   interp_type);


      // Start site loop
      for (ns = 0; ns < num_output_sites; ns++)
	{
	  nsOutputAdder = ns * nsOutputMultiplier;

	  // See if site is in input forecast file
	  if (index_array[ns] < 0)
	    continue;

	  nsInputAdder = index_array[ns] * nsInputMultiplier;

	  if (times_per_day == 1)
	    // Don't interpolate daily vars, simply copy them over
	    {
	      for (nd = 0; nd < num_output_days; nd++)
		for (nt = 0; nt < times_per_day; nt++)  // Yes this is always a single loop
		  for(nv = 0; nv < num_output_param_vals; nv++) {
		    output_data[ns * num_output_days * times_per_day * num_output_param_vals +
			       nd * times_per_day * num_output_param_vals +
			       nt * times_per_day + 
			       nv] =
		      fcst_data[index_array[ns] * num_input_day * times_per_day * num_input_param_vals +
			       nd * times_per_day * num_input_param_vals +
			       nt * times_per_day + 
			       nv];
		  }
	      continue;
	    }

	  // Do cubic spline interpolation (Any interp method starting with
	  // "cubic_spline" will be caught here.)
	  if (strncmp(interp_type, "cubic_spline", strlen("cubic_spline")) == 0)
	    {

	     //Because num_output_param_vals <= num_input_param_vals we use an index bounded by num_output_param_vals
	     //to index fcst_data and output_data
	     for(nv = 0; nv < num_output_param_vals; nv++)
	     {
		 // Move time and forecast data to array for spline function.
		 num_spline_vals = 0;
		 for (nd=0; nd < num_input_day; nd++)
		 {
		     for (nt=0; nt < num_input_times_per_day; nt++)
		     {
			 forecast_valid_time = forc_time + nd * SEC_PER_DAY + 
			     nt * SEC_PER_DAY / num_input_times_per_day;
		      
			 x[num_spline_vals] = forecast_valid_time;
			 y[num_spline_vals] = (double)fcst_data[nsInputAdder + 
								nd * ndInputMultiplier + 
								nt * ntInputMultiplier + nv];
		      
			 // Do not include fill values in the spline arrays
			 if (y[num_spline_vals] != (double)NC_FILL_FLOAT)
			     num_spline_vals++;
		     }
		 }
	      
		 // Call spline function to get second derivatives. Fill values
		 // at the end points will cause this to be a natural cubic spline.

		 init_spline(x, y, num_spline_vals, NC_FILL_DOUBLE, NC_FILL_DOUBLE, y2);

		 // Loop over output times, get value and move to output array
		 for (nd=0; nd < num_output_days; nd++)
		 {
		     for (nt=0; nt < num_output_times_per_day; nt++)
		     {
			 forecast_valid_time = forc_start_time + nd * SEC_PER_DAY + 
			     nt * SEC_PER_DAY / num_output_times_per_day;
		      
			 doNightMask = 0;

			 // Check for masking night-time values, a sub-case of the
			 // cubic spline method which forces values to 0 at night.
			 // This has been tested on sites around the world, but not
			 // for sites within the arctic cirles.

			 if (strcmp(interp_type, "cubic_spline_nightmask") == 0)
			 {
			     calc_sun2(latlon_array[index_array[ns]*2],
				       latlon_array[index_array[ns]*2+1],
				       forecast_valid_time,
				       srise, sset);
			  
			     srise -= TWILIGHT_ZONE;
			     sset  += TWILIGHT_ZONE;

			     if ((sset < srise &&
				  (forecast_valid_time > sset  && forecast_valid_time < srise)) ||
				 (sset > srise &&
				  (forecast_valid_time < srise || forecast_valid_time > sset)))
				 doNightMask = 1;
		      
			     //printf("lat, lon, ftime, srise, sset, night? = %.2f, %.2f, %.0f %.0f %.0f %d\n", latlon_array[index_array[ns]*2], latlon_array[index_array[ns]*2+1], forecast_valid_time, srise, sset, doNightMask);
			  
			 }

			 double dist = bracket_dist(x, forecast_valid_time, num_spline_vals);
			 float splined = NC_FILL_FLOAT;
			 
			 if(dist <=  DIST_THRESHOLD)
			 {
			   // Calculate splined value.
			   splined = (doNightMask == 1) ? 0. : (float) calc_splint(x, y, y2, num_spline_vals, forecast_valid_time, NC_FILL_DOUBLE);
			 }
			 else
			 {
 			     proc_log->write_time(1, "Info: spline interpolation skipped. Time delta %.4e > DIST_THRESHOLD at output (%d,%d,%d,%d)\n", dist, ns, nd, nt, nv);
			     //DEBUG STATEMENTS
			     /*std::cout << "--------------------------------" << std::endl;
			     std::cout << "Input Site: " << index_array[ns] << std::endl;
			     std::cout.precision(10);
			     std::cout << "Interp. Time: " << forecast_valid_time << std::endl;
			     std::cout << "--------------------------------" << std::endl;
			     std::cout << "X Vals: " << std::endl;
			     print_array(x, num_spline_vals);
			     std::cout << "--------------------------------" << std::endl;
			     std::cout.precision(6);
			     std::cout << "Y Vals: " << std::endl;
			     print_array(y, num_spline_vals);
			     std::cout << "--------------------------------" << std::endl;
			     std::cout << "Y2 Vals: " << std::endl;
			     print_array(y2, num_spline_vals);*/
			 }
			 	 
			 output_data[nsOutputAdder +
				     nd * ndOutputMultiplier + 
				     nt * ntOutputMultiplier + nv] = splined; 
	
			 // Copy over additional fields, if present
			 if(num_output_param_vals == 4)
			 {
			     output_data[nsOutputAdder +
					 nd * ndOutputMultiplier +
					 nt * ntOutputMultiplier + 1] = NC_FILL_FLOAT;
			     if(output_data[nsOutputAdder +
					    nd * ndOutputMultiplier + 
					    nt * ntOutputMultiplier] == NC_FILL_FLOAT)
				 output_data[nsOutputAdder +
					     nd * ndOutputMultiplier +
					     nt * ntOutputMultiplier + 2] = 0.0;
			     else
				 output_data[nsOutputAdder +
					     nd * ndOutputMultiplier +
					     nt * ntOutputMultiplier + 2] = 1.0;
			     output_data[nsOutputAdder +
					 nd * ndOutputMultiplier +
					 nt * ntOutputMultiplier + 3] = 0.0;
			  
			 }

		     }

		 }

		 //The original logic when num_output_param_vals == 4 is to just iterate over the first index of the last dimension
		 //and set the final three indices of this dimension at index+1, +2, +3 manually.  We don't want to do this 
		 //4 times so we'll break out of the nv loop after the first iteration.
		 if(num_output_param_vals == 4)
		 {
		     break;
		 }
	     }

	    }

	  // Linear interpolation is default


	  else

	    {
	      for (nd=0; nd < num_output_days; nd++)
		{
		  ndOutputAdder = nd * ndOutputMultiplier;
		  for (nt=0; nt < num_output_times_per_day; nt++)
		    {
		      ntOutputAdder = nt * ntOutputMultiplier;

		      forecast_valid_time = forc_start_time + nd * SEC_PER_DAY + 
			nt * SEC_PER_DAY / num_output_times_per_day;

		      // Find surrounding forecast times to use for interpolation
		      time_diff = forecast_valid_time - forc_time;
		      fp_time_step = time_diff / (SEC_PER_DAY / num_input_times_per_day);
		      input_time_step = (int)fp_time_step;
		      interp = fp_time_step - (int)fp_time_step;

		      input_nd = input_time_step / num_input_times_per_day;
		      input_nt = input_time_step - input_nd * num_input_times_per_day;

		      next_nd = (input_time_step + 1) / num_input_times_per_day;
		      next_nt = (input_time_step + 1) - next_nd * num_input_times_per_day;

		      if (input_nd < 0 || input_nd >= num_output_days)
			continue;

		      if (interp == 0. && 
			  fcst_data[nsInputAdder + 
				   input_nd * ndInputMultiplier + 
				   input_nt * ntInputMultiplier] == NC_FILL_FLOAT
			  && num_input_times_per_day == num_output_times_per_day) 
			{
			  bool found = false;
			  int pos1 = input_nd * ndInputMultiplier + input_nt * ntInputMultiplier;
			  int pos2 = input_nd * ndInputMultiplier + input_nt * ntInputMultiplier;
			  while(pos1 > 0 && !found) {
			    pos1 -= ntInputMultiplier;
			    if(fcst_data[nsInputAdder + pos1] != NC_FILL_FLOAT) {
			      found = true;
			    }
			  }
			  if(found) {
			    found = false;
			    while(pos2 < nsInputMultiplier - ntInputMultiplier && !found) {
			      pos2 += ntInputMultiplier;
			      if(fcst_data[nsInputAdder + pos2] != NC_FILL_FLOAT) {
				found = true;
			      }
			    }
			    if(found) {
			      interp = (float)(((nd * ndInputMultiplier + nt * ntInputMultiplier) 
					 - pos1) / ntInputMultiplier) / 
				(float)((pos2 - pos1) / ntInputMultiplier);

			      input_nd = pos1 / ndInputMultiplier;
			      input_nt = (pos1 - (input_nd * ndInputMultiplier)) / ntInputMultiplier;
			      next_nd = pos2 / ndInputMultiplier;
			      next_nt = (pos2 - (next_nd * ndInputMultiplier)) / ntInputMultiplier;
			    }
			  }

			}
		      
		      if (interp == 0.)
			{
			  for(nv = 0; nv < num_output_param_vals; nv++)
			    output_data[nsOutputAdder +
				        nd * ndOutputMultiplier +
				        nt * ntOutputMultiplier + nv] = 
			      fcst_data[nsInputAdder + 
				       input_nd * ndInputMultiplier +
				       input_nt * ntInputMultiplier + nv];
			}
		      else // Need to interpolate
			{
			  // is there a time afterwards to interpolate with?
			  if (next_nd >= num_input_day)
			    // Next time step beyond end of input data -- No interp
			    continue;

			  const int iter_count = (num_output_param_vals == 4) ? 1 : num_output_param_vals;
			  for(nv = 0; nv < iter_count; nv++)
			  {
			      if (fcst_data[nsInputAdder + 
					    input_nd * ndInputMultiplier +
					    input_nt * ntInputMultiplier + nv] == NC_FILL_FLOAT)
				  // Forecast "before" is MISSING  -- No interp
				  continue;
			  
			      if (fcst_data[nsInputAdder + 
					    next_nd * ndInputMultiplier +
					    next_nt * ntInputMultiplier + nv] == NC_FILL_FLOAT)
				  // Forecast "after" is MISSING  -- No interp
				  continue;
			  
			      output_data[nsOutputAdder +
					  nd * ndOutputMultiplier +
					  nt * ntOutputMultiplier + nv] = 
				  (1. - interp) * 
				  fcst_data[nsInputAdder + 
					    input_nd * ndInputMultiplier + 
					    input_nt * ntInputMultiplier + nv] +
				  interp * 
				  fcst_data[nsInputAdder + 
					    next_nd * ndInputMultiplier + 
					    next_nt * ntInputMultiplier + nv];

			      // Deal with the Stdev and confidence
			      if(num_output_param_vals == 4) {
				  // Take the greater of the two values stdev as the interpolateds
				  if(fcst_data[nsInputAdder + 
					       input_nd * ndInputMultiplier + 
					       input_nt * ntInputMultiplier + 1] >
				     fcst_data[nsInputAdder + 
					       next_nd * ndInputMultiplier + 
					       next_nt * ntInputMultiplier + 1])

				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 1] =
					  fcst_data[nsInputAdder + 
						    input_nd * ndInputMultiplier + 
						    input_nt * ntInputMultiplier + 1];
				  else
				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 1] =
					  fcst_data[nsInputAdder + 
						    next_nd * ndInputMultiplier + 
						    next_nt * ntInputMultiplier + 1];

				  // Take the lesser of the two values confidence1 as the interpolateds
				  if(fcst_data[nsInputAdder  + 
					       input_nd * ndInputMultiplier  + 
					       input_nt * ntInputMultiplier + 2] >
				     fcst_data[nsInputAdder  + 
					       next_nd * ndInputMultiplier  + 
					       next_nt * ntInputMultiplier + 2])

				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 2] =
					  fcst_data[nsInputAdder + 
						    input_nd * ndInputMultiplier + 
						    input_nt * ntInputMultiplier + 2];
				  else
				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 2] =
					  fcst_data[nsInputAdder  + 
						    next_nd * ndInputMultiplier  + 
						    next_nt * ntInputMultiplier + 2];

				  // Take the greater of the two values confidence2 as the interpolateds
				  if(fcst_data[nsInputAdder + 
					       input_nd * ndInputMultiplier + 
					       input_nt * ntInputMultiplier + 3] >
				     fcst_data[nsInputAdder + 
					       next_nd * ndInputMultiplier + 
					       next_nt * ntInputMultiplier + 3])

				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 3] =
					  fcst_data[nsInputAdder + 
						    input_nd * ndInputMultiplier + 
						    input_nt * ntInputMultiplier + 3];
				  else
				      output_data[nsOutputAdder +
						  nd * ndOutputMultiplier +
						  nt * ntOutputMultiplier + 3] =
					  fcst_data[nsInputAdder + 
						    next_nd * ndInputMultiplier + 
						    next_nt * ntInputMultiplier + 3];

			      }
			  }
			}
		    }
		}
	    }
	}

      write_mod_fcst(&output_file, var_name, output_data);
    }

  delete [] output_site_array;
  delete [] index_array;
  delete [] latlon_array;
  delete [] x;
  delete [] y;
  delete [] y2;
  delete [] fcst_data;
  delete [] output_data;
  delete ncErr;

  return(0);
}

