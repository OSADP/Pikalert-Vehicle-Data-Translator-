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
// Module: NcfcUtils.cc
//
// Author: Seth Linden
//
// Date:   11/5/08
//
// Description:
//     
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "log/log.hh"
#include "ncf/ncf.hh"
#include "ncfc/ncfc.hh"

#include "ncfc_utils.hh"


NcfcUtils::NcfcUtils(Log *L)
{
  log = L;
  errorString = "";
}

NcfcUtils::~NcfcUtils()
{
}


int NcfcUtils::get_var_list(char *in_file, vector<string>* in_var_names, vector<nc_type>* in_var_types)
{
  log->write_time(4,"Info: Getting variable list from netcdf file %s.\n", in_file);
  
  int in_ncid;
  int ret = 0;
  
  if (nc_open(in_file, NC_NOWRITE, &in_ncid) != NC_NOERR)
    {
      errorString = "Unable to open " + string(in_file) + " for reading.";
      return -1;
    }
  
  string ncfcError;
  
  ret = NCFC_get_var_list(in_ncid, *in_var_names, *in_var_types);
  if (ret != 0)
    {
      errorString = ncfcError;
      return -1;
    } else{
    log->write_time(4,"Info. Successfully read variables from %s.\n", in_file);
  }
  
  nc_close(in_ncid);
  
  return 0;
}


int NcfcUtils::read_ncfile(char *in_file, vector<string> in_var_names, ncfc_io *in_data)
{
  //log->write_time(2,"Info: Reading netcdf file %s.\n", in_file);

  int in_ncid;
  int ret = 0;

  vector<void *> in_var_ptrs;
  vector<void *> in_var_fill_ptrs;
  vector<nc_type> in_var_types;
  vector<int> in_var_ndims;
  vector<size_t *> in_var_dims;
  vector<long> in_var_sizes;
  vector<string> in_var_units;

  if (nc_open(in_file, NC_NOWRITE, &in_ncid) != NC_NOERR)
    {
      errorString = "Unable to open " + string(in_file) + " for reading.";
      return -1;
    }

  string ncfcError;
  
  ret = NCFC_get_variables(in_ncid, in_var_names, in_var_ptrs, in_var_fill_ptrs, in_var_types, in_var_units, in_var_ndims, in_var_dims, in_var_sizes, ncfcError);
  
  if (ret != 0)
    {
      log->write_time(2,"Warning. Unable to read netcdf data from %s.\n", in_file);
      errorString = ncfcError;
      return -1;
    } else{
      log->write_time(2,"Info. Successfully read netcdf data from %s.\n", in_file);
    }
  
  // Store the data in a ncfc_in struct
  in_data->var_ptrs = in_var_ptrs;
  in_data->var_fill_ptrs = in_var_fill_ptrs;
  in_data->var_types = in_var_types;
  in_data->var_ndims = in_var_ndims;
  in_data->var_dims = in_var_dims;
  in_data->var_sizes = in_var_sizes;
  in_data->var_units = in_var_units;

  nc_close(in_ncid);

  return 0;
}


int NcfcUtils::get_file_data(char *filename, ncfc_io *data)
{
  unsigned int i;
  int ret;
  
  // Get the variables (and types) from the file
  data->is_valid = 1;
  vector<string> var_names;
  vector<nc_type> var_types;
  ret = 0;
  ret = get_var_list(filename, &var_names, &var_types);
  if(ret != 0)
    {
      log->write_time("Error: %s\n", getErrorString().c_str());
      data->is_valid = 0;
      return(ret);
    }
  
  // Get the data for all the variables in the file 
  data->var_names = var_names;
  ret = 0;
  ret = read_ncfile(filename, var_names, data);
  if(ret != 0)
    {
      log->write_time("Error: %s\n", getErrorString().c_str());
      data->is_valid = 0;
      return(ret);
    }
  //print_ncfc_data(data);
  
  // Get the number of forecast days, times-per-day and parameters
  //
  int num_dims;
  int num_days;
  int num_times_per_day;
  double time_step;
  int num_params;
  
  for(i = 0; i < var_names.size(); i++)
    {
      num_dims = data->var_ndims[i];
      
      if(num_dims > 4)
	log->write_time("Warning: for variable %s, found unexpected number of dimensions: %d.\n", var_names[i].c_str(), num_dims);
      
      if(num_dims == 3 || num_dims == 4)
	{
	  num_days = data->var_dims[i][1];
	  num_times_per_day = data->var_dims[i][2];
	  
	  if(num_times_per_day == 1)
	    time_step = 3600 * 24;
	  else if(num_times_per_day == 4)
	    time_step = 3600 * 6;
	  else if(num_times_per_day == 8)
	    time_step = 3600 * 3;
	  else if(num_times_per_day == 24)
	    time_step = 3600;
	  else
	    {
	     log->write_time("Warning: for variable %s, found unexpected num_times_per_day: %d.\n", var_names[i].c_str(), num_times_per_day); 
	     time_step = MISSING;
	    }
	  
	  if(num_dims == 3)
	    num_params = 1;
	  else 
	    num_params = data->var_dims[i][3]; 
	}
      else
	{
	  num_days = MISSING;
	  num_times_per_day = MISSING;
	  time_step = MISSING;
	  num_params = MISSING;
	}
      
      //printf("var_names[%d]: %s, num_dims: %d, num_days: %d, num_times_per_day: %d, time_step: %d, num_params: %d\n", i, var_names[i].c_str(), num_dims, num_days, num_times_per_day, (int)time_step, num_params);
      
      // Store the values
      data->num_days.push_back(num_days);
      data->num_times_per_day.push_back(num_times_per_day);
      data->time_step.push_back(time_step);
      data->num_params.push_back(num_params);
      
    } // end loop over var-names
  
  return 0;
}


int NcfcUtils::alloc_ncfc_io(vector <out_var_info *> all_out_vars, ncfc_io *out_data)
{
  log->write_time(4,"Info: Allocating space for output data.\n");
  
  vector<string> out_var_names;
  vector<void *> out_var_ptrs;
  vector<long> out_var_sizes;
  vector<nc_type> out_var_types;
  vector<size_t *> out_var_dims;
  vector<void *> out_var_fill_ptrs;
  
  for(unsigned int i=0; i<all_out_vars.size(); i++){
    //cout << "all_out_vars[" << i << "]->var_name = " << all_out_vars[i]->var_name << endl;
    
    if(strcmp(all_out_vars[i]->var_type, "float") == 0){
      out_var_ptrs.push_back(new float[all_out_vars[i]->var_size]);
      out_var_types.push_back(NC_FLOAT);
    } else if(strcmp(all_out_vars[i]->var_type, "int") == 0){
      out_var_ptrs.push_back(new int[all_out_vars[i]->var_size]);
      out_var_types.push_back(NC_INT);
    } else if(strcmp(all_out_vars[i]->var_type, "short") == 0){
      out_var_ptrs.push_back(new short[all_out_vars[i]->var_size]);
      out_var_types.push_back(NC_SHORT);
    } else if(strcmp(all_out_vars[i]->var_type, "double") == 0){
      out_var_ptrs.push_back(new double[all_out_vars[i]->var_size]);
      out_var_types.push_back(NC_DOUBLE);
    } else if(strcmp(all_out_vars[i]->var_type, "string") == 0){
      out_var_ptrs.push_back(new char[all_out_vars[i]->var_size]);
      out_var_types.push_back(NC_CHAR);
    } else{
      errorString = "Unknown output variable type: " + string(all_out_vars[i]->var_type);
      return -1;
    }

    out_var_names.push_back(all_out_vars[i]->var_name);
    out_var_sizes.push_back(all_out_vars[i]->var_size);
    out_var_dims.push_back(NULL);
    out_var_fill_ptrs.push_back(NULL);
    
  } // end loop over all_out_vars
  
  // Store the data in a ncfc_out struct
  out_data->var_names = out_var_names;
  out_data->var_ptrs = out_var_ptrs;
  out_data->var_sizes = out_var_sizes;
  out_data->var_types = out_var_types;
  out_data->var_dims = out_var_dims;
  out_data->var_fill_ptrs = out_var_fill_ptrs;
  
  return 0;
}

int NcfcUtils::fill_ncfc_io(vector <out_var_info *> all_out_vars, ncfc_io *out_data)
{
  log->write_time(4,"Info: Copying fill values to output data.\n");
  
  for (unsigned int i=0; i<out_data->var_names.size(); i++)
    {
      //printf("var: %s, size: %d\n", out_data->var_names[i].c_str(), out_data->var_sizes[i]);
      for (int j=0; j<out_data->var_sizes[i]; j++)
	{
	  if(strcmp(all_out_vars[i]->var_type, "float") == 0)
	    ((float *)out_data->var_ptrs[i])[j] = NC_FILL_FLOAT;
	  else if(strcmp(all_out_vars[i]->var_type, "int") == 0)
	    ((int *)out_data->var_ptrs[i])[j] = NC_FILL_INT;
	  else if(strcmp(all_out_vars[i]->var_type, "short") == 0)
	    ((short *)out_data->var_ptrs[i])[j] = NC_FILL_SHORT;
	  else if(strcmp(all_out_vars[i]->var_type, "double") == 0)
	    ((double *)out_data->var_ptrs[i])[j] = NC_FILL_DOUBLE;
	  else if(strcmp(all_out_vars[i]->var_type, "string") == 0)
	    ((char *)out_data->var_ptrs[i])[j] = NC_FILL_CHAR;
	  else{
	    errorString = "Unknown output variable type: " + string(all_out_vars[i]->var_type);
	    return -1;
	  }
	} // end loop over var_size
    } // end loop over var_names 
  
  return 0;
}


int NcfcUtils::copy_input2output(ncfc_io *in_data, ncfc_io *out_data)
{
  log->write_time(4,"Info: Copying input data to output data.\n");
  
  for (unsigned int i=0; i<in_data->var_names.size(); i++)
    {
      //printf("var: %s, size: %d\n", in_data->var_names[i].c_str(), in_data->var_sizes[i]);
      
      if(out_data->var_sizes[i] < in_data->var_sizes[i]){
	log->write_time("Error: for var %s, out_data size: %d is less than in_data size %d, can't copy data to output\n",  in_data->var_names[i].c_str(), out_data->var_sizes[i], in_data->var_sizes[i]);
	return -1;
      }
      
      for (int j=0; j<in_data->var_sizes[i]; j++)
	{
	  if(in_data->var_types[i] == NC_FLOAT)
	    ((float *)out_data->var_ptrs[i])[j] = ((float *)in_data->var_ptrs[i])[j];
	  else if (in_data->var_types[i] == NC_INT)
	    ((int *)out_data->var_ptrs[i])[j] = ((int *)in_data->var_ptrs[i])[j];
	  else if (in_data->var_types[i] == NC_SHORT)
	    ((short *)out_data->var_ptrs[i])[j] = ((short *)in_data->var_ptrs[i])[j];
	  else if (in_data->var_types[i] == NC_DOUBLE)
	    ((double *)out_data->var_ptrs[i])[j] = ((double *)in_data->var_ptrs[i])[j];
	  else if (in_data->var_types[i] == NC_CHAR)
	    ((char *)out_data->var_ptrs[i])[j] = ((char *)in_data->var_ptrs[i])[j];
	  else{
	    log->write_time("Error: unknown input variable type, can't copy data to output\n");
	    return -1; 
	  }

	} // end loop over var_size
    } // end loop over var_names 
  
  return 0;
}


int NcfcUtils::write_ncfile(char *cdl_file, char *out_file, ncfc_io *out_data)
{
  log->write_time(4,"Info: Creating output file %s.\n", out_file);

  // Create output netcdf file
  if (NCF_cdl2nc(cdl_file, out_file) != 0)
    {
      errorString = "Unable to generate " + string(out_file) +
        " from cdl file " + string(cdl_file);
      return -1;
    }
  
  // Open file for writing
  int ncid;
  if (nc_open(out_file, NC_WRITE, &ncid) != NC_NOERR)
    {
      errorString = "Unable to open " + string(out_file) + " for writing.";
      return -1;
    }
  
  // Write data
  string ncfcError;
  int e = 0;
  e = NCFC_put_variables(ncid, out_data->var_names, out_data->var_ptrs, out_data->var_sizes, ncfcError);
  if (e != 0)
    {
      errorString = ncfcError;
      return -1;
    }
  
  log->write_time(4,"Info: Successfully wrote output data.\n");
  
  // Close the netcdf file
  //
  nc_close(ncid);

  return 0;
}


int NcfcUtils::get_var_index(ncfc_io *data, string var_name)
{
  int index = -1;
  for(unsigned int i=0; i<data->var_names.size(); i++){
    if(strcmp(data->var_names[i].c_str(), var_name.c_str()) == 0){
      index = i;
      break;
    }
  }
  return index;
}


void NcfcUtils::print_ncfc_data(ncfc_io *data)
{
  for (unsigned int i=0; i<data->var_names.size(); i++)
    {
      printf("var: %s, units: %s\n", data->var_names[i].c_str(), data->var_units[i].c_str());
      for (int j=0; j<data->var_sizes[i]; j++)
	{
	  if (data->var_types[i] == NC_FLOAT)
	    printf("%s[%d] = %f\n", data->var_names[i].c_str(), j, ((float *)data->var_ptrs[i])[j]);
	  else if (data->var_types[i] == NC_INT)
	    printf("%s[%d] = %d\n", data->var_names[i].c_str(), j, ((int *)data->var_ptrs[i])[j]);
	  else if (data->var_types[i] == NC_SHORT)
	    printf("%s[%d] = %d\n", data->var_names[i].c_str(), j, ((short *)data->var_ptrs[i])[j]);
	  else if (data->var_types[i] == NC_DOUBLE)
	    printf("%s[%d] = %lf\n", data->var_names[i].c_str(), j, ((double *)data->var_ptrs[i])[j]);
	  else if (data->var_types[i] == NC_CHAR)
	    printf("%s[%d] = %c\n", data->var_names[i].c_str(), j, ((char *)data->var_ptrs[i])[j]);
	  
	} // end loop over var_size
    } // end loop over var_names 
}


void NcfcUtils::clean_ncfc_io(ncfc_io *data)
{
  log->write_time(5,"Info: Cleaning ncfc_io data pointers\n");
  
  for (unsigned int i=0; i<data->var_names.size(); i++)
    {
      log->write_time(5,"Info: cleaning ncfc_io data pointers for var %s, size %d\n", data->var_names[i].c_str(), data->var_sizes[i]);
      
      if(data->var_dims[i] != NULL)
	delete [] (size_t *)data->var_dims[i];

      if (data->var_types[i] == NC_FLOAT)
        {
          delete [] (float *)data->var_ptrs[i];
	  if(data->var_fill_ptrs[i] != NULL)
	    delete [] (float *)data->var_fill_ptrs[i];
        }
      else if (data->var_types[i] == NC_INT)
        {
          delete [] (int *)data->var_ptrs[i];
	  if(data->var_fill_ptrs[i] != NULL)
	    delete [] (int *)data->var_fill_ptrs[i];
        }
      else if (data->var_types[i] == NC_SHORT)
        {
          delete [] (short *)data->var_ptrs[i];
	  if(data->var_fill_ptrs[i] != NULL)
	    delete [] (short *)data->var_fill_ptrs[i];
        }
      else if (data->var_types[i] == NC_DOUBLE)
        {
          delete [] (double *)data->var_ptrs[i];
	  if(data->var_fill_ptrs[i] != NULL)
	    delete [] (double *)data->var_fill_ptrs[i];
        }
      else if (data->var_types[i] == NC_CHAR)
        {
          delete [] (char *)data->var_ptrs[i];
	  if(data->var_fill_ptrs[i] != NULL)
	    delete [] (char *)data->var_fill_ptrs[i];
        }

    } // end loop over var_ptrs
}

