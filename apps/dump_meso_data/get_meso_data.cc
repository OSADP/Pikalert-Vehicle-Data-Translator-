// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:20 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <string.h>
#include <fstream>
#include <cstdio>
#include <time.h>
#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include <time.h>
#include "data.h"

extern Log *logfile; 

int get_meso_data(int num_files, char *inputFileName, vector<char *> &variables, os *obs_data);

int get_meso_data(int num_files, char *inputFileName, vector<char *> &variables, os *obs_data){
  
  int ret;
  int f, i, j, k;
  int ind;
  NcVar *var;
  NcAtt *att;
  char varname[80];
  int minfld;
  char *cvar;
  
  minfld = 8; /* size of minimum field width */
  
  char *inputFile[num_files];  
  NcFile *nc_inf[num_files];
  long num_obs[num_files];
  int stnid_len[num_files];
  char *station_ids[num_files];
  double *obs_times[num_files];
  
  // Loop over files and get critical information
  for(f = 0; f < num_files; f++){
    
    inputFile[f] = inputFileName;
    logfile->write_time(2, "obs_input_file[%d] = %s\n", f, inputFile[f]);
    
    // Open input file
    nc_inf[f] = new NcFile(inputFile[f], NcFile::ReadOnly);
    if (!nc_inf[f]->is_valid())
      {
        logfile->write_time("Warning: Unable to open input file %s\n",inputFile[f]);
        continue;
      }
    
    // Get dimension for number of reports
    strcpy(varname,"recNum");
    NcDim *dim = nc_inf[f]->get_dim("recNum");
    if (!dim)
      {
        logfile->write_time("Warning: Unable to get ptr for dim %s,in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
	continue;
      }
    num_obs[f] = dim->size();
    
    
    // Get station IDs
    strcpy(varname,"maxStaIdLen");
    dim = nc_inf[f]->get_dim(varname);
    if (!dim)
      {
        logfile->write_time("Warning: Unable to get ptr for dim %s, in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
        continue;
      }
    stnid_len[f] = dim->size();
    
    
    strcpy(varname,"stationId");
    var = nc_inf[f]->get_var(varname);
    if (!var)
      {
        logfile->write_time("Warning: Unable to get ptr for %s, in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
        continue;
      }
    
    station_ids[f] = new char[num_obs[f]*stnid_len[f]];
    ret = var->get(station_ids[f], num_obs[f], stnid_len[f]);
    if (ret == 0)
      {
        logfile->write_time("Warning: Unable to get data for var %s, in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
        continue;
      }
    
    
    // Get report times
    strcpy(varname,"observationTime");
    var = nc_inf[f]->get_var(varname);
    if (!var)
      {
        logfile->write_time("Warning: Unable to get ptr for %s, in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
        continue;
      }
    
    obs_times[f] = new double[num_obs[f]];
    ret = var->get(obs_times[f], num_obs[f]);
    if (ret == 0)
      {
        logfile->write_time("Warning: Unable to get data for %s, in file %s\n",varname,inputFile[f]);
        obs_data[f].is_valid = 1;
        continue;
      }
  }
  
  
  // Allocate space for data and variable units
  float **var_ptrs = new float *[variables.size()*num_files];
  char **var_units = new char *[variables.size()*num_files];
  int *type = new int[variables.size()*num_files];
  int *width = new int[variables.size()*num_files];
  long *num_vals = new long[variables.size()*num_files];
  int *num_dim = new int[variables.size()*num_files];
  
  // Loop over requested variables and read data
  ind = 0;
  for (i=0; i<variables.size(); i++)
    {
      for(f = 0; f<num_files; f++)
        {
	  
	  if(nc_inf[f]->is_valid()){
	    
	    obs_data[f].is_valid = 0;
	    obs_data[ind].var_is_valid = 0;
	    
	    var = nc_inf[f]->get_var(variables[i]);
	    if (!var)
	      {
		if(strcmp(variables[i], "MissingVar") != 0 && strcmp(variables[i], "roadTemperature") != 0 && strcmp(variables[i], "bridgeTemperature") != 0 && strcmp(variables[i], "roadSubsurfaceTemp") != 0){ 
		  logfile->write_time("Info: Unable to get ptr for %s, in file %s\n",variables[i],inputFile[f]);
		}
		obs_data[ind].var_is_valid = 1;
		ind++;
                continue;
	      }
	    
	    // Get full size of variable and save
	    long *dim = var->edges();
	    num_dim[ind] = var->num_dims();
	    num_vals[ind] = var->num_vals();
	    var_ptrs[ind] = new float[num_vals[ind]];
	    
	    
	    // Save field width size
	    width[ind] = strlen(variables[i]);
	    if (width[ind] < minfld)
	      width[ind] = minfld;
	    
	    
	    // Get variable units
	    att = var->get_att( "units" );
	    if (att)
	      {
		var_units[ind] = new char[strlen(att->as_string(0))+1];
		strcpy(var_units[ind], att->as_string(0));
	      }
	    else
	      {
		var_units[ind] = new char[1];
		strcpy(var_units[ind], "");
	      }
	    
	    
	    // Get values
	    type[ind] = var->type();
	    if (type[ind] == ncFloat)
	      {
		ret = var->get(var_ptrs[ind], dim);
		if (ret == 0)
		  {
		    logfile->write_time("Warning Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		    obs_data[f].is_valid = 1;
		  }
	      }
	    else if (type[ind] == ncDouble)
	      {
		double *dvar = new double[num_vals[ind]];
		ret = var->get(dvar, dim);
		if (ret == 0)
		  {
		    logfile->write_time("Warning Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		    obs_data[f].is_valid = 1;
		  }
		for (j=0; j<num_vals[ind]; j++)
		  var_ptrs[ind][j] = (float)dvar[j];
	      }
	    else if (type[ind] == ncInt)
	      {
		int *svar = new int[num_vals[ind]];
		ret = var->get(svar, dim);
		if (ret == 0)
		  {
		    logfile->write_time("Warning Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		    obs_data[f].is_valid = 1;
		  }
		for (j=0; j<num_vals[ind]; j++)
		  var_ptrs[ind][j] = (float)svar[j];
	      }
	    else if (type[ind] == ncShort)
	      {
		short *svar = new short[num_vals[ind]];
		ret = var->get(svar, dim);
		if (ret == 0)
		  {
		    logfile->write_time("Warning Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		    obs_data[f].is_valid = 1;
		  }
		for (j=0; j<num_vals[ind]; j++)
		  var_ptrs[ind][j] = (float)svar[j];
	      }
	    else if (type[ind] == ncChar)
	      {
		cvar = new char[num_vals[ind]];
		ret = var->get(cvar, dim);
		if (ret == 0)
		  {
		    logfile->write_time("Warning Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		    obs_data[f].is_valid = 1;
		  }
		
		for (j=0; j<num_vals[ind]; j++)
		  var_ptrs[ind][j] = cvar[j];
		
		// Save string length for width if larger than variable name
		if(num_dim[ind] > 1){
		  if(dim[1] > width[ind]){
		    width[ind] = dim[1];
		  }
		}
	      }
	    else
	      {
		logfile->write_time("Warning %s, in file %s is not of type ncFloat, ncShort or ncDouble\n",variables[i],inputFile[f]);
		for (j=0; j<num_vals[ind]; j++){
                  var_ptrs[ind][j] = MISSING;
		}
	      }
	    
	    if(obs_data[ind].var_is_valid == 0){
	      logfile->write_time(2,"Read data for %s in file %s\n",variables[i],inputFile[f]);
	    }
	   
	  } else{
            obs_data[f].is_valid = 1;
	    obs_data[ind].var_is_valid = 1;
	  }
	  
	  ind++;
	}
    }
  
  
  // Put data into obs_data struct
  for(f = 0; f < num_files; f++){    
    if(obs_data[f].is_valid == 0){
      obs_data[f].num_obs = num_obs[f];
      obs_data[f].stnid_len = stnid_len[f];
      obs_data[f].station_ids = new char[num_obs[f]*stnid_len[f]];
      obs_data[f].obs_times = new double[num_obs[f]];
      for(j = 0; j < num_obs[f]*stnid_len[f]; j++){
	obs_data[f].station_ids[j] = station_ids[f][j];
      }
      for(j = 0; j < num_obs[f]; j++){
	obs_data[f].obs_times[j] = obs_times[f][j];
      }
    }
  }
  
  
  ind = 0;
  for (i=0; i<variables.size(); i++){
    for(f = 0; f < num_files; f++){
      if(obs_data[ind].var_is_valid == 0){
	obs_data[ind].units = &var_units[ind];
	obs_data[ind].var_ptrs = var_ptrs[ind];
      }
      ind++;
    }
  }
  
  
  return(0);
}

