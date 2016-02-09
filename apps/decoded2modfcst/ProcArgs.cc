#include <iostream>
#include <fstream>
#include <sstream>

#include <log/log.hh>

#include "Params.hh"
#include "ProcArgs.hh"

using namespace std;

/**************************************************************
 * Constructor
 */

ProcArgs::ProcArgs(int argc, char **argv, int min_required_args, int &num_args)
{
  int opt_args = 0;
  int debug_level = -1;
  int args_load = 0;
  int i = 0;
  int ret;
  
  L = new Log();
  P = new Params();
  
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
  
  tdrp_override_t override;
  TDRP_init_override(&override);
  char *params_path = (char *)"unknown";
  
  if (args_load)
    {
     if (P->loadFromArgs(argc, argv, override.list, &params_path))
        {
          cerr << "Error: problem with tdrp parameters\n";
          cerr << "Ending. Exit status = 2\n";
          exit (2);
        }
    }
  else
    {
      if (P->loadDefaults(0))
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
      if (strcmp(argv[i], "-l") == 0)
        {
          L = new Log(argv[i+1]);
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
  num_args = argc - opt_args;
  if (num_args < min_required_args)
    {
      cerr << "Error: Incorrect or insufficient command line arguments.\n";
      cerr << '\n';
      usage(argv[0]);
      P->usage(cerr);
      exit(2);
    }
  
  // Set up logfile
  if (L == 0)
    L = new Log("");
  
  L->set_debug(debug_level);
}

/**********************************************************************
 * Destructor
 */

ProcArgs::~ProcArgs()
{
}

/**************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************/

void ProcArgs::usage(char *prog_name)
{
  cerr << "usage: " << prog_name << " input_file site_list cdl_file output_file [-l logfile_base] [-d debug_level]" << '\n';
  cerr << "input_file: a decoded input file (a grib2site netcdf output file)\n";
  cerr << "site_list: a dicast site list containing the list of sites to output\n";
  cerr << "cdl_file: a mod_fcst netcdf configuration file used to setup the output file\n";
  cerr << "output_file: the full path (or filename) of the module forecast output file\n";
  cerr << "\n";
  cerr << "The following args are optional:\n";
  cerr << "\t[-d debug_level]\n";
  cerr << "\t[-l log_file]\n";
}
