// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:52 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: main.cc
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
#include <log/log.hh>
#include "Params.hh"

// Constant, macro and type definitions 

// Global variables 
Log *proc_log;
Params conf_params;

// Functions and objects
extern int fec(double forc_corr_time, double forc_gen_time, char *site_list_file,
	       char *nbr_file_name, char *cdl_file_name, char *todays_obs_file_name, 
	       char *yesterdays_obs_file_name, char *concat_meso_file, char *input_file_name, char *output_file_name);


void out_of_store()
{
  int exit_status = 1;

  proc_log->write_time("Error: out of store.\n");
  proc_log->write_time("Ending: exit status = %d.\n", exit_status);
  exit(exit_status);
}

void usage(char *prog_string)
{
  fprintf(stderr, "usage:  %s forecast_correction_time forecast_gen_time site_list nbr_file cdl_file todays_obs_file input_file output_file\n", prog_string);
  fprintf(stderr, "forecast_correction_time: unix time associated with the first time to correct\n");
  fprintf(stderr, "forecast_gen_time: unix time associated with the forecast generation time\n");
  fprintf(stderr, "site_list: dicast site list containing the list of sites to correct\n");
  fprintf(stderr, "nbr_file: dicast neighbor file containing neighbors to use for spatial correction\n");
  fprintf(stderr, "cdl_file: the netcdf configuration file used to setup the output file\n");
  fprintf(stderr, "todays_obs_file: todays integrated observation file containing obs used for correction\n");
  fprintf(stderr, "input_file: the input file containing the forecast data that will be corrected\n");
  fprintf(stderr, "output_file: the output file where the corrected output data will be written\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The following args are optional:\n");
  fprintf(stderr, "[-d debug_level]: an integer that changes the debug level\n");
  fprintf(stderr, "[-l log_file]: the log file where debug messages will be written, default is stdout\n");
  fprintf(stderr, "[-y yesterdays_obs_file]: yesterdays integrated observation file\n");
  fprintf(stderr, "[-c concat_meso_file]: a file containing the most recent concatenated mesonet data (for road fec)\n");
  fprintf(stderr, "    if this is specified, the int_obs file will be ignored but required for backward compatibility\n");
}

int main(int argc, char **argv)
{
  int debug_level = -1;
  char *yesterdays_obs_file = (char *)"";
  char *concat_meso_file = (char *)"";
  int exit_status = 0;
  int extra_args;
  double forc_corr_time;
  double forc_gen_time;
  int i;
  int ret;


  /* first see if there are any tdrp opts */
  i=0;
  int args_load = 0;
  extra_args = 0;
  while (i<argc)
    {
      if ((strcmp(argv[i], "-params") == 0) ||
	  (strcmp(argv[i], "-check_params") == 0) ||
	  (strcmp(argv[i], "-print_params") == 0) ||
	  (strcmp(argv[i], "-tdrp_debug") == 0) ||
	  (strcmp(argv[i], "-tdrp_usage") == 0))
	{
	  args_load = 1;
	  break;
	}
      i++;
    }

  tdrp_override_t override;
  TDRP_init_override(&override);
  char *params_path = NULL;

  if (args_load)
    {
      extra_args += 2;
      if (conf_params.loadFromArgs(argc, argv, override.list, &params_path))
	{
	  cerr << "Error: problem with tdrp parameters\n";
	  cerr << "Ending. Exit status = 2\n";
	  exit (2);
	}
    }
  else
    {
      if (conf_params.loadDefaults(0))
	{
	  cerr << "Error: Unable to load default parameters\n";
	  cerr << "Ending. Exit status = 2\n";
	  exit(2);
	}
    }

  for (i=1; i<argc; i++)
    {
      if (strcmp(argv[i], "-d") == 0)
	{
	  extra_args += 2;
	  debug_level = strtol(argv[i+1], (char **)0, 10);
	  i++;
	}
      else if (strcmp(argv[i], "-l") == 0)
	{
	  extra_args += 2;
	  proc_log = new Log(argv[i+1]);
	  i++;
	}
      else if (strcmp(argv[i], "-y") == 0)
	{
	  extra_args += 2;
	  yesterdays_obs_file = argv[i+1];
	  i++;
	}
      else if (strcmp(argv[i], "-c") == 0)
	{
	  extra_args += 2;
	  concat_meso_file = argv[i+1];
	  i++;
	}
    }

  argc -= extra_args;
  if (argc < 9 ){
    cerr << "Error: Incorrect or insufficient command line arguments.\n";
    cerr << endl;
    usage(argv[0]);
    cerr << endl;
    conf_params.usage(cerr);
    exit(2);
  }

  if (proc_log == 0)
    proc_log = new Log("");
  
  forc_corr_time = strtod(argv[1], (char **)0);
  forc_gen_time = strtod(argv[2], (char **)0);
  
  proc_log->set_debug(debug_level);
  proc_log->write_time_starting( argv[0] );

  set_new_handler(out_of_store);
  ret = fec(forc_corr_time, forc_gen_time, argv[3], argv[4], argv[5], argv[6], 
	    yesterdays_obs_file, concat_meso_file, argv[7], argv[8]);
  
  if (ret != 0)
    exit_status = 1;
  else
    exit_status = 0;
      
  proc_log->write_time("Ending: exit status = %d.\n", exit_status);
  delete proc_log;
  exit(exit_status);
}
