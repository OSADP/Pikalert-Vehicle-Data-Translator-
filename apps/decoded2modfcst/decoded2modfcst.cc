/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 ** Copyright UCAR (c) 1992 - 2010
 ** University Corporation for Atmospheric Research(UCAR)
 ** National Center for Atmospheric Research(NCAR)
 ** Research Applications Program(RAP)
 ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA
 ** All rights reserved. Licenced use only.
 ** Do not copy or distribute without authorization.
 *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

//----------------------------------------------------------------------
// Module: decoded2modfcst.cc
//
// Author: Seth Linden
//
// Date:   04/13/11
//
// Description:
//     
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>

#include <math.h>
#include <netcdf.h>
#include <vector>

#include "ncf/ncf.hh"


#include "ProcArgs.hh"
#include "StringUtil.hh"
#include "decoded2modfcst.hh"

using namespace std;


/**************************************************************
 * Constructor
 */

decoded2modfcst::decoded2modfcst(int argc, char **argv)
{
  int min_required_args = 5;
  
  args = new ProcArgs(argc, argv, min_required_args, num_args);
  
  params = args->P;
  log = args->L;
}

/**************************************************************
 * Destructor
 */

decoded2modfcst::~decoded2modfcst()
{
  // Call destructors
  delete params;
  delete args;
  delete log;
}


int decoded2modfcst::run(char **argv)
{
  log->write_time("Starting.\n");
 
  int i;
  int ret;
  
  // Get required args  
  input_file = argv[1];
  site_list = argv[2];
  cdl_file = argv[3];
  output_file = argv[4];
  
  ret = processData();
  if(ret != 0){
    //log->write_time("Error: in processData.\n");
    log->write_time(1, "Ending. Exit Status = %d.\n", ret);
    return(ret);
  }
  
  log->write_time("Ending. Exit Status = 0.\n");
  return 0;
}


int decoded2modfcst::processData()
{
  int i;
  int ret;
  
  // Initialize class object
  nc_utils = new NcfcUtils(log);
  stat_utils = new StatUtils(log);

  // Get the site information from the site_list file
  vector <site *> sites;
  ret = stat_utils->get_site_info(site_list, &sites);
  if(ret != 0){
    log->write_time("Error: in get_site_info for %s.\n", site_list);
    return(ret);
  }
  
  // Get the variables (and types) from the input file
  vector<string> in_var_names;
  vector<nc_type> in_var_types;
  ret = 0;
  ret = nc_utils->get_var_list(input_file, &in_var_names, &in_var_types);
  if(ret != 0){
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(ret);
  }
  /*
  for(i = 0; i < in_var_names.size(); i++){
    cout << "in_var_names[" << i << "] = " << in_var_names[i] << endl;
  }
  */

  // Get the data for all the variables
  ncfc_io *in_data = new ncfc_io;  
  in_data->is_valid = 0;
  in_data->var_names = in_var_names;
  ret = 0;
  ret = nc_utils->read_ncfile(input_file, in_var_names, in_data);
  if(ret != 0){
    in_data->is_valid = 1;
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(1);
  }
  //nc_utils->print_ncfc_data(in_data);
  

  // Create and then read output netcdf file to get required dimensions
  // to set the size of the output variables
  //
  if (NCF_cdl2nc(cdl_file, output_file) != 0)
    {
      log->write_time("Error: Unable to generate %s from cdl file %s.\n", output_file, cdl_file);
      return(1);
    }
  
  // Get the ouput file variable list
  vector<string> out_var_names;
  vector<nc_type> out_var_types;
  ret = 0;
  ret = nc_utils->get_var_list(output_file, &out_var_names, &out_var_types);
  if(ret != 0){
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(ret);
  }
  /*
  for(i = 0; i < out_var_names.size(); i++){
    cout << "out_var_names[" << i << "] = " << out_var_names[i] << endl;
  }
  */
  
  // Set up output attribute object (nc_in)
  // Read the output file to get dimensions and types
  ncfc_io *out_attr_data = new ncfc_io;
  out_attr_data->is_valid = 0;
  out_attr_data->var_names = out_var_names;
  ret = 0;
  ret = nc_utils->read_ncfile(output_file, out_var_names, out_attr_data);
  if(ret != 0){
    out_attr_data->is_valid = 1;
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(1);
  }
  
  // Set up the output info
  output_info *out_info = new output_info;
  out_info->cdl_file = cdl_file;
  out_info->file_name = output_file;
  
  // Loop over the output variables and set types and sizes
  for(i = 0; i < out_attr_data->var_names.size(); i++){
    //printf("var: %s, num-dimensions: %d\n", out_attr_data->var_names[i].c_str(), out_attr_data->var_ndims[i]);
    
    out_var_info *var_info = new out_var_info;
    var_info->var_name = strdup( out_attr_data->var_names[i].c_str() );
    
    // Set the type
    if (out_attr_data->var_types[i] == NC_FLOAT)
      var_info->var_type = (char *)"float";
    else if (out_attr_data->var_types[i] == NC_INT)
      var_info->var_type = (char *)"int";
    else if (out_attr_data->var_types[i] == NC_SHORT)
      var_info->var_type = (char *)"short";
    else if (out_attr_data->var_types[i] == NC_DOUBLE)
      var_info->var_type = (char *)"double";
    else if (out_attr_data->var_types[i] == NC_CHAR)
      var_info->var_type = (char *)"string";
    
    //printf("  var_info->var_type: %s\n", var_info->var_type);
    
    if(out_attr_data->var_ndims[i] == 0){
      var_info->num_days = 1;
      var_info->num_ftpd = 1;
      var_info->num_params = 1;
      log->write_time(2, "Info: for variable: %s, setting size to 1\n", out_attr_data->var_names[i].c_str());
      var_info->var_size = 1;
    } else if( strcmp(out_attr_data->var_names[i].c_str(), "site_list") == 0 ){
      var_info->num_days = 1;
      var_info->num_ftpd = 1;
      var_info->num_params = 1;
      log->write_time(2, "Info: for variable: %s, setting size to %d\n", out_attr_data->var_names[i].c_str(), sites.size());
      var_info->var_size = sites.size();
    } else if(out_attr_data->var_ndims[i] == 3){
      var_info->num_days = out_attr_data->var_dims[i][1];
      var_info->num_ftpd =  out_attr_data->var_dims[i][2];
      var_info->num_params = 1;
      log->write_time(2, "Info: for variable: %s, 3 dimensions, setting size to: %d * %d * %d\n", out_attr_data->var_names[i].c_str(), sites.size(), var_info->num_days, var_info->num_ftpd);
      var_info->var_size = sites.size() * var_info->num_days * var_info->num_ftpd;   
    } else if(out_attr_data->var_ndims[i] == 4){
      var_info->num_days = out_attr_data->var_dims[i][1];
      var_info->num_ftpd =  out_attr_data->var_dims[i][2];
      var_info->num_params = out_attr_data->var_dims[i][3];
      log->write_time(2, "Info: for variable: %s, 4 dimensions, setting size to: %d * %d * %d * %d\n", out_attr_data->var_names[i].c_str(), sites.size(), var_info->num_days, var_info->num_ftpd, var_info->num_params);
      var_info->var_size = sites.size() * var_info->num_days * var_info->num_ftpd * var_info->num_params;   
    } else{
      log->write_time("Error: for variable: %s, can't set size, unrecognized number of dimensions: %d\n", out_attr_data->var_names[i].c_str(), out_attr_data->var_ndims[i]);
      return(1);
    }
    
    out_info->out_vars.push_back(var_info);      
  }
  
  // Set up output data object (nc_out)
  // Allocate space for the output data
  ncfc_io *out_data = new ncfc_io;
  ret = nc_utils->alloc_ncfc_io(out_info->out_vars, out_data);
  if(ret != 0){
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(ret);
  }
  
  // Copy fill values to ouput arrays
  ret = nc_utils->fill_ncfc_io(out_info->out_vars, out_data);
  if(ret != 0){
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(ret);
  }
  
  // Copy data to the output arrays
  ret = copyData(in_data, out_data, sites, out_info->out_vars);
  if(ret != 0){
    log->write_time("Error: in copyData.\n");
    return(ret);
  }

  // Write the output data to a netcdf file
  ret = nc_utils->write_ncfile(out_info->cdl_file, out_info->file_name, out_data);
  if(ret != 0){
    log->write_time("Error: %s\n", nc_utils->getErrorString().c_str());
    return(ret);
  }

  // Clean up
  //nc_utils->clean_ncfc_io(in_data);
  //delete out_info;
  //delete out_data;

  return (0);
}


int decoded2modfcst::copyData(ncfc_io *in_data, ncfc_io *out_data, vector<site *> sites, vector <out_var_info *> out_vars)
{
  log->write_time(1,"Info. Copying input data to output data.\n");
  
  unsigned int i, j, k, t;
  unsigned int s, d, h, p;
  string in_var_name, out_var_name;
  int in_var_ind, out_var_ind;
  
  // Set time variables
  //
  double utime_1992 = 694224000;
  
  double creation_time = (double)(time(NULL));
  out_var_name = "creation_time";
  out_var_ind = nc_utils->get_var_index(out_data, out_var_name);
  ((double *)out_data->var_ptrs[out_var_ind])[0] = creation_time;
  
  in_var_name = "reftime";
  in_var_ind =  nc_utils->get_var_index(in_data, in_var_name);
  double forc_time =  (((double *)in_data->var_ptrs[in_var_ind])[0] * 3600) + utime_1992;  
  forc_time = ((int)forc_time / 86400) * 86400;
  log->write_time(3, "Info: forc_time = %d\n", (int)forc_time);
  out_var_name = "forc_time";
  out_var_ind = nc_utils->get_var_index(out_data, out_var_name);
  ((double *)out_data->var_ptrs[out_var_ind])[0] = forc_time;
  
  // Get input data valid_times
  //
  vector<double> valid_times;
  in_var_name = "valtime";
  in_var_ind =  nc_utils->get_var_index(in_data, in_var_name);
  for(t = 0; t < in_data->var_sizes[in_var_ind]; t++){
    double v_time = (((double *)in_data->var_ptrs[in_var_ind])[t] * 3600) + utime_1992;
    log->write_time(3, "Info: valid_times[%d] = %d\n", t, (int)v_time);
    valid_times.push_back(v_time);
  }
  
  // Loop over output variables
  // If a match is found in the params->mod_fcst_var_map
  // then copy input data to output
  for(i = 0; i < out_data->var_names.size(); i++){
    
    int num_sites = sites.size();
    int num_days = out_vars[i]->num_days;
    int num_ftpd = out_vars[i]->num_ftpd;
    int num_params = out_vars[i]->num_params;
    int time_step = 24 / num_ftpd;
    
    for(j = 0; j < params->mod_fcst_var_map_n; j++){
      if(strcmp(out_data->var_names[i].c_str(), params->_mod_fcst_var_map[j].out_name) == 0){
	break;
      }
    }
    
    if(j < params->mod_fcst_var_map_n){
      
      log->write_time(3, "Info: for var: %s, num_sites = %d, num_days = %d, num_ftpd = %d, num_params = %d, time_step = %d\n", out_data->var_names[i].c_str(), num_sites, num_days, num_ftpd, num_params, time_step);
    
      in_var_name = params->_mod_fcst_var_map[j].in_name;
      in_var_ind = nc_utils->get_var_index(in_data, in_var_name);

      if (in_var_ind < 0) {
	log->write_time("Error: %s does not exist in the input file\n", in_var_name.c_str());
	return(1);
      }
    
      in_var_name = params->_mod_fcst_var_map[j].in_name;
      out_var_name = out_data->var_names[i].c_str();
      out_var_ind = nc_utils->get_var_index(out_data, out_var_name);
      
      if(strcmp(out_data->var_names[i].c_str(), "num_sites") == 0 ||
	 strcmp(out_data->var_names[i].c_str(), "site_list") == 0){
	
	log->write_time(2, "Info: copying data for in_var_name: %s, out_var_name: %s\n", in_var_name.c_str(), out_var_name.c_str());
	out_data->var_ptrs[out_var_ind] = in_data->var_ptrs[in_var_ind];
	
      } else{
	
	log->write_time(2, "Info: copying data for in_var_name: %s, out_var_name: %s\n", in_var_name.c_str(), out_var_name.c_str());
	
	//
	// Loop over output dimensions (sites, days, ftpd)
	//
	for(s = 0; s < num_sites; s++){
	  
	  double output_loop_time = forc_time;
	  
	  for(d = 0; d < num_days; d++){
	    
	    for(h = 0; h < num_ftpd; h++){
	      
	      int out_data_ind = (s * num_days * num_ftpd * num_params) + (d * num_ftpd * num_params) + (h * num_params);
	      
	      for(t = 0; t < valid_times.size(); t++){
		
		if(output_loop_time == valid_times[t]){
		  
		  int in_site_ind = stat_utils->get_site_list_ind(in_data, sites[s]->dicast_id);
		  int in_data_ind = (t * num_sites) + in_site_ind;

		  // Copy input values to output array
		  if( ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind] == 9999. || 
		      ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind] == MISSING ||
		      ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind] == NC_FILL_FLOAT ) // Set missing values to fill values
		    ((float *)out_data->var_ptrs[out_var_ind])[out_data_ind] = NC_FILL_FLOAT;
		  else if( strcmp(in_data->var_units[in_var_ind].c_str(), "degK") == 0 ) // Convert Kelvin to Celsius
		    ((float *)out_data->var_ptrs[out_var_ind])[out_data_ind] = ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind] - 273.15;
		  else 
		    ((float *)out_data->var_ptrs[out_var_ind])[out_data_ind] = ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind];
		  
		  log->write_time(3, "Info: s: %4d, d: %2d ftpd: %2d, output_loop_time = %d, in_data[%4d] = %7.2f, out_data[%4d] = %7.2f\n", s, d, h, (int)output_loop_time, in_data_ind, out_data_ind, ((float *)in_data->var_ptrs[in_var_ind])[in_data_ind], ((float *)out_data->var_ptrs[out_var_ind])[out_data_ind]);
		}
		
	      } // end loop over input valid_times
	      
	      output_loop_time = output_loop_time + (time_step * 3600);
	    } // end loop over output times_per_day
	  } // end loop over output days
	} // end loop over output sites
	
      } // end else (different input / output variable names)
      
    } // end if match found in params variable list
    
  } // end loop over output variables
  
  return 0;
}

