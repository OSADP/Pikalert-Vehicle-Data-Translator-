// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:07 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <netcdf.hh>
#include <log/log.hh>
#include "Params.hh"
#include "data.h"

Log *logfile;
Params params;
NcError* ncErr = new NcError(NcError::silent_nonfatal);

// External functions
extern int get_site_ids(vector<site *> &sites, char *fname);
extern int get_fcst_data(int num_days, char *inputFileName, vector<char *> &variables, fs *fcst_data);
extern int dump_fcst_data(int num_files, vector<char *> &variables, vector<char *> &output_vars, vector<site *> &sites, fs *fcst_data, char *outputFileName, int fcst_hour);


void usage(char *prog_name)
{
  cerr << "usage: " << prog_name << " fcst_input_file fcst_creation_hour site_list output_file\n";
  cerr << "fcst_input_file: the forecast file to dump data from\n";
  cerr << "fcst_creation_hr: the hour (UTC) at which the forecast file was created (e.g. 00, 03, 12, ect)\n";
  cerr << "site_list: a dicast site list containing the sites to dump data for\n";
  cerr << "output_file: full path of output file name, data will be written to this file\n";
  cerr << "The following args are optional:\n";
  cerr << "\t[-s individual_sites_list]: individual site statistics will be calculated for these sites\n";
  cerr << "\t[-d debug_level]\n";
  cerr << "\t[-l log_file]\n";
}


int main(int argc, char **argv){

  int min_required_args = 5;
  int opt_args = 0;
  int debug_level = -1;
  char *logfileBase = 0;
  char *ind_sites_list = 0;
  int args_load = 0;
  int i = 0;
  
  int ret;
  int ind;
  int flag;
  int f, j, k, v;
  time_t start, end;
  double dif;
  int num_days;
  int num_files;
  int obs_num_files;
  int meso_num_files;
  int days;
  int fc_times_per_day;
  
  vector <char *> variables;
  vector <char *> output_vars;
  vector <site *> sites;
  
  /* first see if there are any tdrp opts */
  while (i<argc)
    {
      if ((strcmp(argv[i], "-params") == 0))
        {
          opt_args+=2;
          args_load = 1;
        }
      else if ((strcmp(argv[i], "-check_params") == 0) ||
               (strcmp(argv[i], "-tdrp_debug") == 0) ||
               (strcmp(argv[i], "-tdrp_usage") == 0))
        {
          opt_args++;
          args_load = 1;
        }
      else if (strcmp(argv[i], "-print_params") == 0)
        {
          opt_args++;
          args_load = 1;
          if (i+1 < argc && argv[i+1][0] != '-')
            opt_args++;
        }

      i++;
    }
  
  // Load tdrp params
  
  tdrp_override_t override;
  TDRP_init_override(&override);
  char *params_path = NULL;

  if (args_load)
    {
      if (params.loadFromArgs(argc, argv, override.list, &params_path))
        {
          cerr << "Error: problem with tdrp parameters\n";
          cerr << "Ending. Exit status = 2\n";
          exit (2);
        }
    }
  else
    {
      if (params.loadDefaults(0))
        {
          cerr << "Error: Unable to load default parameters\n";
          cerr << "Ending. Exit status = 2\n";
          exit(2);
        }
    }

  // Process non-TDRP args
  i = 0;
  while (i+1 < argc)
    {
      if (strcmp(argv[i], "-s") == 0)
        {
          ind_sites_list = argv[i+1];
          i++;
          opt_args+=2;
        }
      else if (strcmp(argv[i], "-l") == 0)
        {
          logfileBase = argv[i+1];
          i++;
          opt_args+=2;
        }
      else if (strcmp(argv[i], "-d") == 0)
        {
          debug_level = atoi(argv[i+1]);
          i++;
          opt_args+=2;
        }
      i++;
    }
  
  // Check on argument list count
  if ((argc-opt_args) < min_required_args)
    {
      cerr << "Error: Incorrect or insufficient command line arguments.\n";
      usage(argv[0]);
      params.usage(cerr);
      exit(2);
    }
 
  // Get command line args
  char *fcst_input_file = argv[1];
  int fcst_hour = atoi(argv[2]);
  char *site_list = argv[3];
  char *output_file = argv[4];
  
  ////////////////////////////////////////////////////////////////////
  // Set up logfile
  if (logfileBase)
    logfile = new Log(logfileBase);
  else
    logfile = new Log("");

  if (logfile->error())
    {
      cerr << "Error: Unable to open logfile\n";
      cerr << "Ending. Exit status = 1\n";
      exit (1);
    }
  
  
  logfile->set_debug(debug_level);
  logfile->write_time_starting( argv[0] );
  
  ////////////////////////////////////////////////////////////////////
  // Get forecast input and output variable names
  for(i = 0; i < params.fcst_var_names_n; i++){
    variables.push_back(params._fcst_var_names[i].in_var_name);
    output_vars.push_back(params._fcst_var_names[i].out_var_name);
  }
  
  if(output_vars.size() != variables.size()){
    logfile->write_time("Error: number of output variable names does not match number of forecasts variables.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  //for(i = 0; i < variables.size(); i++){
  //cout << variables[i] << ", " <<  output_vars[i] << endl;
  //}
  

  ////////////////////////////////////////////////////////////////////
  // Get site_list_dicast_ids and site_list_icao_ids, from site list
  ret = get_site_ids(sites, site_list);
  if(ret){
    logfile->write_time("Error in get_site_ids.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  //for(i = 0; i < sites.size(); i++){
  //cout << "sites[" << i << "]->dicast_id = " << sites[i]->dicast_id;
  //cout << " sites[" << i << "]->icao_id = " << sites[i]->icao_id;
  //cout << " sites[" << i << "]->lat = " << sites[i]->lat;
  //cout << " sites[" << i << "]->lon = " << sites[i]->lon << endl;
  //}
  //endl;
  
  
  ////////////////////////////////////////////////////////////////////  
  // Get dicast fcst data
    
  num_files = 1;
  fs *fcst_data = new fs[variables.size()*num_files];
  
  logfile->write_time(1, "Getting forecast data.\n");
  ret = get_fcst_data(num_files, fcst_input_file, variables, fcst_data);
  if(ret){
    logfile->write_time("Error in get_fcst_data.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  ////////////////////////////////////////////////////////////////////
  //Dump fcst data
  logfile->write_time(1, "Dumping forecast data.\n");
  ret = dump_fcst_data(num_files, variables, output_vars, sites, fcst_data, output_file, fcst_hour);
  if(ret){
    logfile->write_time("Error in dump_fcst_data.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Delete var_ptrs to free up memory
  ind = 0;
  for(v = 0; v < variables.size(); v++){
    for(f = 0; f < num_files; f++){
      if(fcst_data[ind].var_is_valid == 0){
	delete [] fcst_data[ind].var_ptrs;
      }
      ind++;
    }
  }
  
  
  logfile->write_time("Ending. Exit status = 0.\n");
  //return(0);
}

