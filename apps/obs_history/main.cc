// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:22:27 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include "obs_history.h"

Log *logfile;

// External functions

extern int get_site_info(vector<site *> &sites, char *fname);
extern int alloc_data(ds *data, vector<site *> sites, int num_out_times);
extern int get_data(double start_time, int num_out_times, vector<site *> sites, char *inputFileName, ds *data);
extern int qc_data(int num_out_times, ds *data);
extern int combine_data(int num_out_times, double start_time, double cur_time, ds *fcst_data, ds *meso_data, ds *out_data);
extern int check_data(int num_out_times, double start_time, double cur_time, ds *data);
extern int create_obs_history(int num_out_times, char *output_file_name, char *cdl_file_name, ds *data);


void usage(char *prog_name)
{
  cerr << endl;
  cerr << "usage: " << prog_name << " cur_time concat_meso_file dump_fcst_file site_list cdl_file output_file [-l logfile_base] [-d debug_level]" << endl;
  cerr << "cur_time:\t\tthe current unix time (seconds since 1970)" << endl;
  cerr << "concat_meso_file:\tan ascii file containing sorted obs data (sorted by site, then variable, then time) or specify \"none\" on the command line to use fcst data only" << endl;
  cerr << "dump_fcst_file:\t\tan ascii file containing sorted fcst data (sorted by site, then variable, then time) or specify \"none\" on the command line to use the obs data only" << endl;
  cerr << "site_list:\t\ta dicast site list containing the obs history sites\n";
  cerr << "cdl_file:\t\tthe cdl file that corresponds to the netcdf output file" << endl;
  cerr << "output_file:\t\tthe full path of the output file name" << endl;
  cerr << endl;
  cerr << "The following args are optional:" << endl;
  cerr << "\t[-d debug_level]" << endl;
  cerr << "\t[-l log_file]" << endl;
  cerr << endl;
  exit(2);
}


int main(int argc, char **argv){

  int min_required_args = 7;
  int opt_args = 0;
  int debug_level = -1;
  char *logfileBase = 0;
  
  int ret;
  int ind;
  int flag;
  int cnt;
  double cur_time, start_time, end_time;
  int i, j, k, f, s, t, v;
  vector <site *> sites;
  
  // Process args
  i = 0;
  while (i+1 < argc)
    {
      if (strcmp(argv[i], "-l") == 0)
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
  
  //printf("opt_args = %d\n", opt_args);
  //printf("logfileBase = %s\n", logfileBase);
  //printf("debug_level = %d\n", debug_level);
  
  
  // Check on argument list count
  if ((argc-opt_args) < min_required_args)
    {
      cerr << "Error: Incorrect or insufficient command line arguments.\n";
      usage(argv[0]);
      exit(2);
    }
  
  time_t curTime = (time_t)atof(argv[1]);
  double full_cur_time = atof(argv[1]);
  char *meso_file = argv[2];
  char *fcst_file = argv[3];
  char *site_list = argv[4];
  char *cdl_file = argv[5];
  char *output_file = argv[6];
  
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
  // Subtract minutes and seconds from cur_time so it can be used to 
  // get the expected time of the earliest and latest obs
  
  time_t init_time = 0;
  struct tm *ct;
  ct = gmtime(&init_time);
  ct = localtime(&curTime);
  ct->tm_year += 1900;
  ct->tm_mon += 1;

  char gen_min[MAX_STRING_LEN];
  char gen_sec[MAX_STRING_LEN];
  
  sprintf(gen_min, "%02d", ct->tm_min);
  sprintf(gen_sec, "%02d", ct->tm_sec);
  //cout << "gen_min = " << gen_min << endl;
  //cout << "gen_sec = " << gen_sec << endl;
  
  double d_gen_min = atof(gen_min);
  double d_gen_sec = atof(gen_sec);
  
  double total_sec = (d_gen_min * 60) + d_gen_sec;
  //cout << "total_sec = " << total_sec << endl;
  
  cur_time = full_cur_time - total_sec;
  //cout << "cur_time = " << (int)cur_time << endl;
  
  end_time = cur_time + (24*3600);
  //cout << "end_time = " << (int)end_time << endl;
  
  start_time = cur_time - (23*3600);
  //cout << "start_time = " << (int)start_time << endl;
  
  int num_out_times = (int)((end_time - start_time) / 3600) + 1;
  //cout << "num_out_times = " << num_out_times << endl;
  
  
  ////////////////////////////////////////////////////////////////////
  // Get site info
  if(get_site_info(sites, site_list)){
    logfile->write_time("Error in get_site_info.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  ////////////////////////////////////////////////////////////////////

  // Allocate space for fcst data struct
  ds *fcst_data = new ds;
  //logfile->write_time(1, "Allocating space for fcst_data.\n");
  ret = alloc_data(fcst_data, sites, num_out_times);
  if(ret){
    logfile->write_time("Error in alloc_data for fcst_data\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  if(strcmp(fcst_file, "none") != 0){
    // Get fcst data
    logfile->write_time(1, "Getting fcst data.\n");
    ret = get_data(start_time, num_out_times, sites, fcst_file, fcst_data);
    if(ret){
      logfile->write_time("Error in get_data for %s.\n", fcst_file);
      logfile->write_time("Ending. Exit status = 1.\n");
      exit (1);
    }
    // QC fcst data
    logfile->write_time(1, "QC'ing fcst data.\n");
    ret = qc_data(num_out_times, fcst_data);
    if(ret){
      logfile->write_time("Error in qc_data for fcst_data\n");
      logfile->write_time("Ending. Exit status = 1.\n");
      exit (1);
    }
    
  } else{
    fcst_data->is_valid = 0;
  }
  
  //if(fcst_data->is_valid){
  //cout << "fcst data is valid" << endl;
  //}
  
  
  // Allocate space for meso data struct
  ds *meso_data = new ds;
  //logfile->write_time(1, "Allocating space for meso_data\n");
  ret = alloc_data(meso_data, sites, num_out_times);
  if(ret){
    logfile->write_time("Error in alloc_data for meso_data\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  if(strcmp(meso_file, "none") != 0){
    // Get meso data
    logfile->write_time(1, "Getting meso data.\n");
    ret = get_data(start_time, num_out_times, sites, meso_file, meso_data);
    if(ret){
      logfile->write_time("Error in get_data for %s.\n", meso_file);
      logfile->write_time("Ending. Exit status = 1.\n");
      exit (1);
    }
    // QC meso data
    logfile->write_time(1, "QC'ing meso data.\n");
    ret = qc_data(num_out_times, meso_data);
    if(ret){
      logfile->write_time("Error in qc_data for meso_data\n");
      logfile->write_time("Ending. Exit status = 1.\n");
      exit (1);
    }
  } else{
    meso_data->is_valid = 0;
  }
  
  //if(meso_data->is_valid){
  //cout << "meso data is valid" << endl;
  //}
  
  
  ////////////////////////////////////////////////////////////////////
  // Combine data
  // Allocate space for output data struct
  ds *out_data = new ds;
  //logfile->write_time(1, "Allocating space for out_data\n");
  ret = alloc_data(out_data, sites, num_out_times);
  if(ret){
    logfile->write_time("Error in alloc_data for out_data\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  // Combine data
  logfile->write_time(1, "Combining data.\n");
  ret = combine_data(num_out_times, start_time, cur_time, fcst_data, meso_data, out_data);
  if(ret){
    logfile->write_time("Error in combine_data\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  // Checking output data
  logfile->write_time(1, "Checking output data.\n");
  ret = check_data(num_out_times, start_time, cur_time, out_data);
  if(ret){
    logfile->write_time("Error in check_data\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Output data
  logfile->write_time(1, "Writing output file.\n");
  ret = create_obs_history(num_out_times, output_file, cdl_file, out_data);
  if(ret){
    logfile->write_time("Error in create_obs_history\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  logfile->write_time("Ending. Exit status = 0.\n");
}
