// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:22:18 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: merge_var.cc
 *
 * Author: Bill Myers
 * Date:   10/5/02
 *
 * Author: Jason Craig
 * Updated: 11/05
 *
 * Description:
 *  Program Takes two files and merges the variables into one single netCDF file.
 *  Variable input and output names are defined in the params file.
 *  Updated program to also blend variables based on weights defined in params.
 *  
 */

// Include files 
#include <new>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <log/log.hh>
#include <math.h>

#include <ncf/ncf.hh>
#include <nxncf/nxncf.hh>
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "netcdf.h"
#include "Params.hh"

// Constant, macro and type definitions 

// Global variables 
extern Log *proc_log;
extern Params conf_params;


// Functions and objects
extern int read_fcst_info(NcFile *input_file, int *num_input_sites,
			  double *forc_time, long *num_input_times_per_day,
			  long *num_input_day, long *max_site_num, long *max_str_len);
extern int read_fcst_data(NcVar *var_ptr,
			  long *times_per_day, int return_fm, float *data);
extern int read_fcst_data(NcVar *var_ptr,
			  long *times_per_day, int return_fm, char *data);
extern int create_output_file(char *cdl_name, char * output_name);
extern int write_header(NcFile *output_file, double forc_time,
			int num_output_sites, int *max_output_sites,
			long  *output_site_array, long *num_output_days,
			long *num_output_times_per_day);
extern int write_mod_fcst(NcFile *output_file, char *var_name,
			 float *fcst_data);
extern int write_mod_fcst(NcFile *output_file, char *var_name,
			 char *fcst_data);

extern int blend_mod_fcst(NcFile *output_file, char *var_name, 
			  float *blend_data, float *weights, int forc_time_diff_periods);

int merge_data(double forc_start_time, char *site_list_file,
	       char *cdl_name, char *input_file1_name, 
	       char *input_file2_name, char *output_name)
{
  int num_input_sites, num_output_sites, num_fcst_vars;
  long times_per_day, max_str_len;
  long num_input_times_per_day, num_input_days, max_input_sites;
  long num_output_times_per_day, num_output_days, max_output_sites;
  long input_index, output_index;

  int  *output_site_array = NULL;
  long *index_array = NULL;
  float *fcst_data = NULL;
  float *output_data = NULL;
  char *char_fcst_data = NULL;
  char *char_output_data = NULL;
  float *first_val;
  long *dim;

  char input_file_name[160];
  char var[160];
  char outvar[160];

  int output_data_size = 0;
  int num_dim, num_params, fm_format;
  int time_diff_secs, time_diff_periods, forc_time_diff_secs, forc_time_diff_periods;
  long size;
  int ret;
  int nf, np, ns, nd, nt, nv;
  double forc_time, forecast_time;

  NcVar *var_ptr;

  // Set up behaviour of netCDF library
  NcError* ncErr = new NcError(NcError::silent_nonfatal);

  forecast_time = forc_start_time;

  // Make sure times are at the top of the hour
  forc_start_time = SEC_PER_DAY * (int)(forc_start_time/SEC_PER_DAY);


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


  // Write output file header info
  ret = write_header(&output_file, forc_start_time, num_output_sites, 
		     output_site_array, &max_output_sites, &num_output_days,
		     &num_output_times_per_day);

  if (num_output_sites > max_output_sites)
    {
      proc_log->write_time("Error: too many sites for output cdl\n");
      return(1);
    }

  // This is the difference in the forecast time from command line and the
  // start of the output file. Used to start the weights at the forecast time.
  forc_time_diff_secs = (int)(forecast_time - forc_start_time);
  forc_time_diff_periods = 
    forc_time_diff_secs / (SEC_PER_DAY / num_output_times_per_day);

  // Check for FM format
  ret = NCF_get_var(&output_file,conf_params._file1_output_var_names[0],&first_val,&dim,&num_dim,&size);
  delete []first_val;
  if (num_dim == IND_FM_NUM_VAR_DIM && dim[IND_FM_PVALS_DIM] > 1)
  {
    fm_format = 1;
    num_params = dim[IND_FM_PVALS_DIM];
  }
  else
  {
    fm_format = 0;
    num_params = 1;
  }
  for (nf=0; nf < 3; nf++)
  {
    if (nf == 0)
      {
	strcpy(input_file_name, input_file1_name);
	num_fcst_vars = conf_params.file1_input_var_names_n;
	if(num_fcst_vars != conf_params.file1_output_var_names_n)
	{
	  proc_log->write_time("Error: file1 input and output var names do not match.\n");
	  return(1);
	}
      }
    else if(nf == 1)
      {
	strcpy(input_file_name, input_file2_name);
	num_fcst_vars = conf_params.file2_input_var_names_n;
	if(num_fcst_vars != conf_params.file2_output_var_names_n)
	{
	  proc_log->write_time("Error: file2 input and output var names do not match.\n");
	  return(1);
	}
      }
    else
      {
	strcpy(input_file_name, input_file2_name);
	num_fcst_vars = conf_params.file2_input_blend_var_names_n;
	if(num_fcst_vars > 0) {
	  if(num_fcst_vars != conf_params.file2_output_blend_var_names_n)
	  {
	    proc_log->write_time("Error: file2 input and output blend var names do not match.\n");
	    return(1);
	  }
	  if(conf_params.file2_blend_weights_n != num_fcst_vars*num_output_days*num_output_times_per_day)
	  {
	    proc_log->write_time("Error: file2 input and output blend weights do not match num_vars*num_days*num_times_per_day.\n");
	    return(1);
	  }
	}
      }

    if(num_fcst_vars > 0)
    {
      // Open and read input  netcdf file
      NcFile input_file(input_file_name, NcFile::ReadOnly);
      if (!input_file.is_valid())
	{
	  proc_log->write_time("Error: Unable to open input file %s\n",input_file_name);
	  return(1);
	}

      ret = read_fcst_info(&input_file, &num_input_sites, &forc_time,
			   &num_input_times_per_day, &num_input_days,
			   &max_input_sites, &max_str_len);
      if (ret != 0)
	{
	  proc_log->write_time("Error: Unable to read fcst file\n");
	  return(1);
	}

      /* create index array */
      ret = NXNCF_create_index_array(output_site_array, num_output_sites, 
				     &input_file, &index_array);
      if (ret < 0) {
	proc_log->write_time("Error: in call to NXNCF_create_index_array\n");
	return(1);
      }

      if (num_input_times_per_day != num_output_times_per_day)
      {
        proc_log->write_time("Error: num times per day must agree between input and output\n");
        return(1);
      }

      
      // Now start main loop
      for (np = 0; np < num_fcst_vars; np++)
	{
	  if (nf == 0)
	    {
	      strcpy(var, conf_params._file1_input_var_names[np]);
	      strcpy(outvar, conf_params._file1_output_var_names[np]);
	    } else if(nf == 1) 
	    {
	      strcpy(var, conf_params._file2_input_var_names[np]);
	      strcpy(outvar, conf_params._file2_output_var_names[np]);
	    } else {
	      strcpy(var, conf_params._file2_input_blend_var_names[np]);
	      strcpy(outvar, conf_params._file2_output_blend_var_names[np]);
	    }
	  // Get pointer to variable
	  var_ptr = input_file.get_var(var);
          if (!var_ptr)
            {
              proc_log->write_time("Error: Unable to get ptr for %s\n", var);
              return(1);
            }
	  
	  NcType type = var_ptr->type() ;
	  
          // Read in input wx data
          switch (type)
	    {
            case ncFloat:
	      if(fcst_data == NULL)
		fcst_data = new float[max_input_sites*num_input_days*num_input_times_per_day*num_params];
	      ret = read_fcst_data(var_ptr, &times_per_day, fm_format, fcst_data);
              break;
            case ncChar:
	      if(num_params > 1) {
		proc_log->write_time("Error: Unable to handle char type with more than 1 param.\n");
		return(1);
	      }
	      if(char_fcst_data == NULL)
		char_fcst_data = new char[max_input_sites*num_input_days*num_input_times_per_day*max_str_len];
	      ret = read_fcst_data(var_ptr, &times_per_day, fm_format, char_fcst_data);
              break;
            default:
              proc_log->write_time("Error: Cannot handle data of type %d\n", type);
              return(1);
            }

	  if (ret != 0)
	    {
	      if(ret == 2)
		proc_log->write_time("Error: Input file type does not match Cdl\n");
	      else
		proc_log->write_time("Error: Unable to read data from input file\n");
	      return(1);
	    }

	  // Allocate and initialize output array
	  switch (type)
	    {
	    case ncFloat:
	      if (output_data == NULL)
		output_data = new float[max_output_sites * num_output_days * num_output_times_per_day * num_params];
	      for (nt=0; nt < max_output_sites*num_output_days*num_output_times_per_day*num_params; nt++)
		output_data[nt] = NC_FILL_FLOAT;
	      break;
	    case ncChar:
	      if (char_output_data == NULL)
		char_output_data = new char[max_output_sites * num_output_days * num_output_times_per_day * max_str_len];
	      for (nt=0; nt < max_output_sites*num_output_days*num_output_times_per_day; nt++)
		char_output_data[nt] = NC_FILL_CHAR;
	      break;
	    }

	  // This is the difference in the input file and output file
	  time_diff_secs = (int)(forc_start_time - forc_time);
	  time_diff_periods = 
	    time_diff_secs / (SEC_PER_DAY / num_input_times_per_day);

	  for (ns = 0; ns < num_output_sites; ns++)
	    {
	      // See if site is in input forecast file
	      if (index_array[ns] < 0)
		continue;

	      for (nd=0; nd < num_input_days; nd++)
		for (nt=0; nt < num_output_times_per_day; nt++)
		  {
		    // Check to make sure this time plus the time difference is still in the output file
		    if (nd * num_input_times_per_day + nt + time_diff_periods >= 0 &&
			nd * num_input_times_per_day + nt + time_diff_periods < num_output_days*num_output_times_per_day)
		      {

			switch (type)
			  {
			  case ncFloat:

			    input_index = 
			      index_array[ns] * num_input_days*num_input_times_per_day * num_params + 
			      nd * num_input_times_per_day*num_params + nt*num_params + time_diff_periods;

			    output_index =
			      ns * num_output_days * num_output_times_per_day * num_params +
			      nd*num_output_times_per_day*num_params + nt*num_params;

			    for(nv=0; nv < num_params; nv++)
			    {
			      output_data[output_index+nv] = fcst_data[input_index+nv];
			    }
			    break;

			  case ncChar:

			    input_index = 
			      (index_array[ns] * num_input_days*num_input_times_per_day +
			       nd * num_input_times_per_day + nt + time_diff_periods) *
			      max_str_len;

			    output_index =
			      (ns * num_output_days * num_output_times_per_day +
			       nd*num_output_times_per_day + nt) * max_str_len;

			    for (int c=0; c<max_str_len; c++)
			      char_output_data[output_index+c] = char_fcst_data[input_index+c];
			    break;
			  }
			
		      }
		    
		  } // End loop over num_output_times_per_day
	        // End loop over num_input_days
	    } // End loop over num_output_sites

	  // Send the copied data to the write_output handler, where it will be written or blended.
	  if(nf < 2) {
	    switch (type)
	      {
	      case ncFloat:
		write_mod_fcst(&output_file, outvar, output_data);
		break;
	      case ncChar:
		write_mod_fcst(&output_file, outvar, char_output_data);
		break;
	      }
	  } else {
	    switch (type)
	      {
	      case ncFloat:
		blend_mod_fcst(&output_file, outvar, output_data,
			       &(conf_params._file2_blend_weights[np*num_output_days*num_output_times_per_day]),
			       forc_time_diff_periods);
		break;
	      case ncChar:
		proc_log->write_time("Error: Cannot blend data of type %d\n", type);
		return(1);
		break;
	      }
	  }
	} // END main loop over num_fcst_vars

      if (fcst_data != NULL) {
	delete []fcst_data;
	fcst_data = NULL;
      }
      if (char_fcst_data != NULL) {
	delete []char_fcst_data;
	char_fcst_data = NULL;
      }
      delete []index_array;
      input_file.close();
    }
  }  // END loop over num_files (3)

  if(output_data != NULL) {
    delete [] output_data;
    output_data = NULL;
  }
  if(char_output_data != NULL) {
    delete [] char_output_data;
    char_output_data = NULL;
  }
  
  delete []dim;
  delete []output_site_array;
  delete ncErr;

  return(0);
}

