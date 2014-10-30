//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//
//       National Center for Atmospheric Research(NCAR)
//       Research Applications Program(RAP)
//       P.O. Box 3000, Boulder, Colorado, 80307-3000, USA
//       All rights reserved. Licenced use only.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rename_fields.cc,v $
//       Version: $Revision: 1.6 $  Dated: $Date: 2012/03/02 17:32:34 $
//
//==============================================================================

/**
 *
 *  @file main.cc
 *
 *  @author Tiffany Tasset
 *
 *  Renames fields of netcdf file.
 *  
 *  @note 
 *
 *  @date 06/10/08
 */

// Include files 
#include "../include/ncfc/ncfc.hh"
#include <netcdf.h>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>


using namespace std;

// Constant and macros

// Types, structures and classes

// Global variables 

// Functions


/**
 *  @brief Copy data from input to output, recasting if necessary
 *
 *  @param [in] in_var_ptrs pointers to input data
 *  @param [in] in_var_types types of input data
 *  @param [in] in_var_sizes sizes of input data
 *  @param [in] in_var_dims dimensions of input data
 *  @param [out] out_var_ptrs pointers to output data
 *  @param [in] out_var_types types of output data
 *  @param [out] error error string
 */
int copy_data(vector<void *> in_var_ptrs, vector<nc_type> in_var_types, vector<long int> in_var_sizes, vector<size_t *> in_var_dims, vector<void *> &out_var_ptrs, vector<nc_type> out_var_types, string &error)
{
  int j = 0;
  for (int i=0; i<(int)in_var_types.size(); i++)
    {
      switch (out_var_types[i])
	{
	case NC_NAT:
	  error = "NC_NAT not supported";
	  return(1);

	case NC_FLOAT:
	  out_var_ptrs.push_back(new float[in_var_sizes[i]]);
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((float *)out_var_ptrs[i])[j] = ((float *)in_var_ptrs[i])[j];
	      break;
	    case NC_INT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((float *)out_var_ptrs[i])[j] = static_cast<float>(((int *)in_var_ptrs[i])[j]);
	      break;
	    case NC_SHORT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((float *)out_var_ptrs[i])[j] = static_cast<float>(((short *)in_var_ptrs[i])[j]);
	      break;
	    case NC_DOUBLE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((float *)out_var_ptrs[i])[j] = static_cast<float>(((double *)in_var_ptrs[i])[j]);
	      break;
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((float *)out_var_ptrs[i])[j] = static_cast<float>(((signed char *)in_var_ptrs[i])[j]);
	      break;
	    case NC_CHAR:
	      error = "Cannot cast char input as a float";
	      return(1);
	    default:
	      break;
	    }
	  break;
	
	case NC_INT:
	  out_var_ptrs.push_back(new int[in_var_sizes[i]]);
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((int *)out_var_ptrs[i])[j] = static_cast<int>(((float *)in_var_ptrs[i])[j]);
	      break;
	    case NC_INT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((int *)out_var_ptrs[i])[j] = static_cast<int>(((int *)in_var_ptrs[i])[j]);
	      break;
	    case NC_SHORT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((int *)out_var_ptrs[i])[j] = static_cast<int>(((short *)in_var_ptrs[i])[j]);
	      break;
	    case NC_DOUBLE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((int *)out_var_ptrs[i])[j] = static_cast<int>(((double *)in_var_ptrs[i])[j]);
	      break;
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((int *)out_var_ptrs[i])[j] = static_cast<int>(((signed char *)in_var_ptrs[i])[j]);
	      break;
	    case NC_CHAR:
	      error = "Cannot cast char input as a int";
	      return(1);
	    default:
	      break;
	    }
	  break;
	
	case NC_SHORT:
	  printf("IN_VAR_SIZES %ld\n", in_var_sizes[i]);
	  out_var_ptrs.push_back(new short[in_var_sizes[i]]);
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((short *)out_var_ptrs[i])[j] = static_cast<short>(((float *)in_var_ptrs[i])[j]);
	      break;
	    case NC_INT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((short *)out_var_ptrs[i])[j] = static_cast<short>(((int *)in_var_ptrs[i])[j]);
	      break;
	    case NC_SHORT:
	      for(j=0; j<in_var_sizes[i]; j++)
		{
		  short short_val = static_cast<short>(((short *)in_var_ptrs[i])[j]);
		  if (short_val <= -989)
		    ((short *)out_var_ptrs[i])[j] = -999;
		  else
		    ((short *)out_var_ptrs[i])[j] = static_cast<short>(((short *)in_var_ptrs[i])[j]);
		}
	      break;
	    case NC_DOUBLE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((short *)out_var_ptrs[i])[j] = static_cast<short>(((double *)in_var_ptrs[i])[j]);
	      break;
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((short *)out_var_ptrs[i])[j] = static_cast<short>(((signed char *)in_var_ptrs[i])[j]);
	      break;
	    case NC_CHAR:
	      error = "Cannot cast char input as a short";
	      return(1);
	    default:
	      break;
	    }
	  break;
	
	case NC_DOUBLE:
	  out_var_ptrs.push_back(new double[in_var_sizes[i]]);
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((double *)out_var_ptrs[i])[j] = static_cast<double>(((float *)in_var_ptrs[i])[j]);
	      break;
	    case NC_INT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((double *)out_var_ptrs[i])[j] = static_cast<double>(((int *)in_var_ptrs[i])[j]);
	      break;
	    case NC_SHORT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((double *)out_var_ptrs[i])[j] = static_cast<double>(((short *)in_var_ptrs[i])[j]);
	      break;
	    case NC_DOUBLE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((double *)out_var_ptrs[i])[j] = static_cast<double>(((double *)in_var_ptrs[i])[j]);
	      break;
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((double *)out_var_ptrs[i])[j] = static_cast<double>(((signed char *)in_var_ptrs[i])[j]);
	      break;
	    case NC_CHAR:
	      error = "Cannot cast char input as a double.";
	      return(1);
	    default:
	      break;
	    }
	  break;
	
	case NC_BYTE:
	  out_var_ptrs.push_back(new char[in_var_sizes[i]]);
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = static_cast<char>(((float *)in_var_ptrs[i])[j]);
	      break;
	    case NC_INT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = static_cast<char>(((int *)in_var_ptrs[i])[j]);
	      break;
	    case NC_SHORT:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = static_cast<char>(((short *)in_var_ptrs[i])[j]);
	      break;
	    case NC_DOUBLE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = static_cast<char>(((double *)in_var_ptrs[i])[j]);
	      break;
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = ((signed char *)in_var_ptrs[i])[j];
	      break;
	    case NC_CHAR:
	      error = "Cannot cast char input as a byte.";
	      return(1);
	    default:
	      break;
	    }
	  break;
	
	case NC_CHAR:
	  char *cptr = 0;
	  cptr = new char[in_var_sizes[i]];
	  out_var_ptrs.push_back(cptr);
	  memset(cptr, '\0', in_var_sizes[i]);
	  string char_ptr;
	  switch (in_var_types[i])
	    {
	    case NC_FLOAT:
	      error = "Cannot cast float input as a char.";
	      return(1);
	    case NC_INT:
	      error = "Cannot cast int input as a char.";
	      return(1);
	    case NC_SHORT:
	      error = "Cannot cast short input as a char.";
	      return(1);
	    case NC_DOUBLE:
	      error = "Cannot cast double input as a char.";
	      return(1);
	    case NC_BYTE:
	      for(j=0; j<in_var_sizes[i]; j++)
		((char *)out_var_ptrs[i])[j] = ((char *)in_var_ptrs[i])[j];
	      break;
	    case NC_CHAR:
	      for(j=0; j<(int)in_var_dims[i][0]; j++)
		{
		  char_ptr.clear();
		  for (int k=0; k<(int)in_var_dims[i][1]; k++)
		    char_ptr.push_back(((char *)in_var_ptrs[i])[j*(int)in_var_dims[i][1] + k]);
		  memcpy(&((char *)out_var_ptrs[i])[j*(int)in_var_dims[i][1]], char_ptr.c_str(), (int)in_var_dims[i][1]);
		}
	      
	    default:
	      break;
	    }
	  break;
	}
    }
  return(0);
}

// Split a string into tokens
void split(const string& str, vector<string>& tokens, const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));

      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);

      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

// Get split lines from a file and store in a vector. Ignore lines starting with #
void get_split_lines(const char *fname ,vector< vector<string> > &split_lines)
{
  vector<string> tokens;

  const string delim = " ";
  ifstream ins(fname);
  string line;
  while (getline(ins, line))
    {
      if (line[0] != '#') 
	{
	  tokens.clear();
	  split(line, tokens, delim);
	  split_lines.push_back(tokens);
	}
    }
}

/**
 *  @brief Get input data from netcdf file
 *
 *  @param [in] in_file name of input file
 *  @param [in] var_names names of variables to be retrieved from file
 *  @param [out] var_input object containing var_names data
 */
int get_nc_input(const char *in_file, vector<string> &var_names, Var_input **var_input)
{
  // Ingest netcdf input
  int in_ncid;
  int status = nc_open(in_file, NC_NOWRITE, &in_ncid);
  if (status != NC_NOERR)
    {
      return -1;
    }

  // Get input through constructor
  Var_input *var_input_ptr = new Var_input(in_ncid, var_names);
  if (var_input_ptr->error() != string(""))
    {
      return -1;
    }
  nc_close(in_ncid);

  *var_input = var_input_ptr;
  return 0;
}


/**
 *  @brief Puts data into netcdf file.
 *
 *  @param [in] cdl_file Name of cdl file
 *  @param [in] out_file Name of output netcdf file
 *  @param [in] var_names output netcdf file variable names
 *  @param [in] var_ptrs pointers to variable data
 *  @param [in] var_sizes sizes of variable data
 */
int put_nc_output( const char *cdl_file, const char *out_file, vector<string> var_names, vector<void *> var_ptrs, vector<long int> var_sizes)
{
  const int COMMAND_LEN = 2048;
  char command[COMMAND_LEN];

  if (cdl_file != 0)
    {
      // Make netcdf output file
      int ret = snprintf(command, COMMAND_LEN, "ncgen -b %s -o %s", cdl_file, out_file);
      if (ret == COMMAND_LEN)
	{
	  return -1;
	}

      ret = system(command);
      if (ret != 0)
	{
	  return -1;
	}
    }

  int out_ncid;
  int status = nc_open(out_file, NC_WRITE, &out_ncid);
  if (status != NC_NOERR)
    {
      return -1;
      printf("error opening file");
    }

  string error;
  int ret = NCFC_put_variables(out_ncid, var_names, var_ptrs, var_sizes, error);
  status = nc_close(out_ncid);

  if ((ret != 0) or (status != NC_NOERR))
    {
      return -1;
      printf("error writing file");
    }

  return 0;
}


/**
 *  @brief Reads config file.
 *
 *  @param [in] config_file name of config file
 *  @param [out] in_var_names input data variable names
 *  @param [out] out_var_names output data variable names
 *  @param [out] out_var_types out_put data types
 */
int read_config(const char *config_file, vector<string> &in_var_names, vector<string> &out_var_names, vector<nc_type> &out_var_types)
{
  vector< vector<string> > var_names;
  get_split_lines(config_file, var_names);
  for (int i=0; i<(int)var_names.size(); i++)
    {
      in_var_names.push_back(var_names[i][0]);
      out_var_names.push_back(var_names[i][1]);
      if (strcmp("float", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_FLOAT);
      else if (strcmp("double", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_DOUBLE);
      else if (strcmp("int", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_INT);
      else if (strcmp("short", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_SHORT);
      else if (strcmp("char", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_CHAR);
      else if (strcmp("byte", var_names[i][2].c_str()) == 0)
	out_var_types.push_back(NC_BYTE);
      else 
	{
	  printf("Invalid type in config file\n");
	  return(1);
	}
    }
  return(0);
}

void usage(char *prog_name)
{
  fprintf(stderr, "Usage: %s config_file cdl_file input_file output_file\n", prog_name);
  fprintf(stderr, "For example:\n\t%s rename_fields.conf generic_turb_obs.cdl $GTG_DATA/gtg_edr_hourly/edr_hourly.20071218.1800.nc $GTG_DATA/derived_data/generic_turb_obs/turb_obs_insitu.20071218.18.nc\n", prog_name );
}

int main(int argc, char **argv)
{
  char *cdl_file = 0;
  char *config_file = 0;
  char *in_file = 0;
  char *out_file = 0;
  int ret = 0;

  // Process command line arguments

  if (argc - optind < 4)
    {
      usage(argv[0]);
      exit(2);
    }

  char *prog_name = basename(argv[0]);
  config_file = argv[optind];
  cdl_file = argv[optind+1];
  in_file = argv[optind+2];
  out_file = argv[optind+3];

  
  printf("Starting %s.\n", prog_name);


  // Read config file containing field input and output names
  vector<string> in_var_names;
  vector<string> out_var_names;
  vector<nc_type> out_var_types;
  ret = read_config(config_file, in_var_names, out_var_names, out_var_types);
  if (ret != 0)
    {
      printf("Error: could not read config file %s\n", config_file);
      printf("Ending %s, return 1.\n", prog_name);
      return 1;
    }


  // Get needed data from input file
  Var_input *var_input = 0;
  ret = get_nc_input(in_file, in_var_names, &var_input);
  if (ret < 0)
    {
      printf("Error: could not get input from %s\n", in_file);
      printf("Ending %s, return 1.\n", prog_name);
      return 1;
    }

  const vector<long int> in_var_sizes = var_input->get_var_sizes();
  const vector<nc_type> in_var_types = var_input->get_var_types();
  const vector<void *> in_var_ptrs = var_input->get_var_ptrs();
  const vector<size_t *> in_var_dims = var_input->get_var_dims();
  

  // Copy data from input to output
  vector<void *> out_var_ptrs;
  string error;
  ret = copy_data(in_var_ptrs, in_var_types, in_var_sizes, in_var_dims, out_var_ptrs, out_var_types, error);
  if (ret != 0)
    {
      printf("Error: %s\n", error.c_str());
      printf("Ending %s, return 1.\n", prog_name);
      return 1;
    }

  
  // Output netcdf file
  ret = put_nc_output(cdl_file, out_file, out_var_names, out_var_ptrs, in_var_sizes);
  if (ret < 0)
    {
      printf("Error: output error occured when writing %s\n", out_file);
      printf("Ending %s, return 1.\n", prog_name);
      return 1;
    }


  // Release memory
  delete var_input;

  // Exit
  printf("Ending %s, return 0.\n", prog_name);
  return 0;
  
}
