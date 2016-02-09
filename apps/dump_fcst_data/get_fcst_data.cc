// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:07 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <fstream>
#include <cstdio>
#include <ctime>
#include <vector>
#include <string.h>
#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include <ncf/ncf.hh>
#include "data.h"

extern Log *logfile; 

int get_fcst_data(int num_files, char *inputFileName, vector<char *> &variables, fs *fcst_data);

int get_fcst_data(int num_files, char *inputFileName, vector<char *> &variables, fs *fcst_data){
  
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
  
  int diff;

  int days;
  int fc_times_per_day;
  //int daily_time;
  
  double forc_time[num_files];
  double creation_time[num_files];
  int num_sites[num_files];
  int *site_list[num_files];
  
  long *sl_dim;
  int sl_num_dim;
  long sl_size;

  // Loop over files and get critical information
  for(f = 0; f < num_files; f++){
    
    fcst_data[f].is_valid = 0;
   
    inputFile[f] = inputFileName;
    logfile->write_time(2, "fcst_input_file[%d] = %s\n", f, inputFile[f]);
    
    // Open input file
    nc_inf[f] = new NcFile(inputFile[f], NcFile::ReadOnly);  
    
    if (!nc_inf[f]->is_valid())
      {
        logfile->write_time("Warning: Unable to open input file %s\n",inputFile[f]);
        fcst_data[f].is_valid = 1;
	continue;
      }

    
    // Get dimension for days
    strcpy(varname,"days");
    days = NCF_get_dim_size(nc_inf[f], varname);
    if(days == -1)
      {
	logfile->write_time("Warning: Unable to get ptr for dim %s,in file %s\n",varname,inputFile[f]);
	fcst_data[f].is_valid = 1;
	continue;
      }
    
    
    // Get dimension for fc_times_per_day
    strcpy(varname,"fc_times_per_day");
    fc_times_per_day = NCF_get_dim_size(nc_inf[f], varname);
    if(fc_times_per_day == -1)
      {
	logfile->write_time("Warning: Unable to get ptr for dim %s,in file %s\n",varname,inputFile[f]);
	fcst_data[f].is_valid = 1;
        continue;
      }
    
    
    // Get dimension for daily_time
    //strcpy(varname,"daily_time");
    //daily_time = NCF_get_dim_size(nc_inf[f], varname);
    //if(daily_time == -1)
    //{
    //logfile->write_time("Warning: Unable to get ptr for dim %s,in file %s\n",varname,inputFile[f]);
    //fcst_data[f].is_valid = 1;
    //continue;
    //}
    
    
    // Get variable creation_time
    strcpy(varname,"creation_time");
    ret = NCF_get_var(nc_inf[f], varname, &creation_time[f]);
    if(ret != 0)
      {
        logfile->write_time("Warning: Unable to get data for var %s, in file %s\n", varname, inputFile[f]);
	fcst_data[f].is_valid = 1;
        continue;
      }
    
    
    // Get variable forc_time
    strcpy(varname,"forc_time");
    ret = NCF_get_var(nc_inf[f], varname, &forc_time[f]);
    if(ret != 0)
      {
	logfile->write_time("Warning: Unable to get data for var %s, in file %s\n", varname, inputFile[f]);
	fcst_data[f].is_valid = 1;
        continue;
      }
    
    //cout << "creation_time[" << f << "]: " << (int)creation_time[f] << endl;
    //cout << "forc_time[" << f << "]: " << (int)forc_time[f] << endl;
    
    
    //diff = (int)creation_time[f] - (int)forc_time[f];
    //cout << "diff: " << diff << endl;
    //if(diff >= 86400){
    //  logfile->write_time("Warning: creation_time %d and forc_time %d are greater than 24 hours apart in file %s\n", (int)creation_time[f], (int)forc_time[f], inputFile[f]);
    // fcst_data[f].is_valid = 1;
    //}
    
    
    // Get variable num_sites
    strcpy(varname,"num_sites");
    ret = NCF_get_var(nc_inf[f], varname, &num_sites[f]);
    if(ret != 0)
      {
        logfile->write_time("Warning: Unable to get data for var %s, in file %s\n", varname, inputFile[f]);
	fcst_data[f].is_valid = 1;
        continue;
      }
    
    
    // Get variable site_list
    strcpy(varname,"site_list");
    ret = NCF_get_var(nc_inf[f], varname, &site_list[f], &sl_dim, &sl_num_dim, &sl_size);
    if(ret != 0)
      {
        logfile->write_time("Warning: Unable to get data for var %s, in file %s\n", varname, inputFile[f]);
	fcst_data[f].is_valid = 1;
        continue;
      }
  }
  
  
  // Allocate space for data and variable units
  float **var_ptrs = new float *[variables.size()*num_files];
  char **var_units = new char *[variables.size()*num_files];  
  long *dim[variables.size()*num_files];
  int num_dim[variables.size()*num_files];
  long size[variables.size()*num_files];
  
  // Loop over requested variables and read data
  ind = 0;
  for (i=0; i<variables.size(); i++)
    {
      for(f = 0; f<num_files; f++)
        {
	  
	  if(fcst_data[f].is_valid == 0){
	    fcst_data[ind].var_is_valid = 0;
	    
	    // Get variable units
	    var_units[ind] = new char[MAX_STRING_LEN];
	    strcpy(varname,"units");
	    ret = NCF_get_var_attr(nc_inf[f], variables[i], varname, &var_units[ind]);
	    if(ret != 0)
	      {
		//logfile->write_time("Warning: Unable to get data for attribute %s, in file %s\n", varname, inputFile[f]);
	      }
	    
	    // Get values
	    var_ptrs[ind] = new float[num_sites[f]];
	    ret = NCF_get_var(nc_inf[f], variables[i], &var_ptrs[ind], &dim[ind], &num_dim[ind], &size[ind]);
	    if (ret != 0)
	      {
		logfile->write_time("Warning: Unable to get data for %s, in file %s\n",variables[i],inputFile[f]);
		fcst_data[ind].var_is_valid = 1;
	      }
	    
	    if(fcst_data[ind].var_is_valid == 0){
	      logfile->write_time(2, "Read data for %s in file %s\n",variables[i],inputFile[f]);
	    }
	    
	  } else{
	    fcst_data[ind].var_is_valid = 1;
	  }
	  
	  ind++;
        }
    }
  
  
  // Put data into fcst_data struct
  for(f = 0; f < num_files; f++){
    if(fcst_data[f].is_valid == 0){
      fcst_data[f].days = days;
      fcst_data[f].fc_times_per_day = fc_times_per_day;
      //fcst_data[f].daily_time = daily_time;
      fcst_data[f].creation_time = creation_time[f];
      fcst_data[f].forc_time = forc_time[f];
      fcst_data[f].num_sites = num_sites[f];
      fcst_data[f].site_list = site_list[f];
    }
    nc_inf[f]->close();
  }
  
  ind = 0;
  for (i=0; i<variables.size(); i++){
    for(f = 0; f < num_files; f++){
      if(fcst_data[ind].var_is_valid == 0){
	fcst_data[ind].units = &var_units[ind];
	fcst_data[ind].num_dim = num_dim[ind];
	fcst_data[ind].dim = dim[ind];
	fcst_data[ind].var_ptrs = var_ptrs[ind];
      }
      ind++;
    }
  }
  
  
  return(0);
}

