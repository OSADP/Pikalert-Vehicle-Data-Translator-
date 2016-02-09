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
extern int temporal_interp(double forc_gen_time, char *site_list_file,
			   char *cdl_file, char *input_file, char *output_file);


void out_of_store()
{
  int exit_status = 1;

  proc_log->write_time("Error: out of store.\n");
  proc_log->write_time("Ending: exit status = %d.\n", exit_status);
  exit(exit_status);
}

void usage(char *prog_string)
{
  fprintf(stderr, "usage:  %s forecast_gen_time site_list cdl_file input_file output_file [-d debug_level] [-l log_file]\n", prog_string);
  exit(2);
}

int main(int argc, char **argv)
{
  int debug_level = -1;
  int exit_status = 0;
  int extra_args;
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
    }

  argc -= extra_args;
  if (argc < 6 )
    usage(argv[0]);

  if (proc_log == 0)
    proc_log = new Log("");
  
  forc_gen_time = strtod(argv[1], (char **)0);
  
  proc_log->set_debug(debug_level);
  proc_log->write_time_starting( argv[0] );

  set_new_handler(out_of_store);
  ret = temporal_interp(forc_gen_time, argv[2], argv[3], argv[4], argv[5]);

  if (ret != 0)
    exit_status = 1;
  else
    exit_status = 0;
      
  proc_log->write_time("Ending: exit status = %d.\n", exit_status);
  delete proc_log;
  exit(exit_status);
}
