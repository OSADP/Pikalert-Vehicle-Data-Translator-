// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: main.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: 
 *              This file contains the entry point, main(), when this program is run.
 *              It handles command line argument interpretation and obvious errors.
 *              The arguments are translated into the proper data types for processing
 *              and passed on to the routine that does all the work.
 *
 *   main()...Main routine described above.
 *     
 */

/* System include files / Local include files */
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "metro_type.hh"

/* Constant definitions / Macro definitions / Type definitions */

//
// Make these global for ease of use.
//
Log *proc_log; 
int debug_level = -1;

/* External functions / Internal global functions / Internal static functions */

extern int road_cond(double forc_time, char *site_file_name, 
		     char *bridge_site_file_name, int treatment_option, 
		     char *treatment_def_file_name, char * last_rc_file, 
		     char *obs_file_name, char *wx_file_name, char *cdl_name,
		     int metro_run_type, char *use_flux, char *mat_default,
		     char *site_config_name, char *output_name);


void out_of_store()
{
  proc_log->write_time("Error: Out of store.\n");
  proc_log->write_time("Ending: exit status = 1.\n");
  exit(1);
}

void usage(char **argv)
{
  cerr << "\nusage: " << argv[0] << " forecast_time site_list_file bridge_site_list_file cdl_file site_config_file treatment_option treatment_def_file last_road_cond_file obs_history_file input_wx_file output_file [-d debug_level] [-l log_file] [-metro_format format] [-use_flux which] [-mat_default name]\n\n";
  cerr << "Required arguments:\n";
  cerr << "forecast_time\t\tunix time of the forecast\n";
  cerr << "site_list_file\t\tascii file containing list of forecast sites\n";
  cerr << "bridge_site_list_file\tascii file containing list of bridge forecast sites\n";
  cerr << "cdl_file\t\tcdl file containing format for the output file\n";
  cerr << "site_config_file\tascii file containing layer, traffic and default\n\t\t\ttreatment type data for each site\n";
  cerr << "treatment_option\tthe treatment option to use\n";
  cerr << "\t\t\t(0=no treatment, 1=recommended, 2=user-defined, 3=sufficient)\n";
  cerr << "treatment_def_file\tascii user-defined treatment file\n"; 
  cerr << "\t\t\t(ignored unless treatment_option=2)\n";
  cerr << "last_road_cond_file\tnetcdf road conditions file from previous run\n";
  cerr << "obs history file\tfile containing recent obs history\n";
  cerr << "input_wx_file\t\tnetcdf weather data file for calculating forecast\n";
  cerr << "output_file\t\tfile which will contain the forecast\n";
  cerr << "\nOptions:\n";
  cerr << "-d debug_level\t\tnumeric debug level\n";
  cerr << "-l log_file\t\tname of file to write log messages\n";
  cerr << "-metro_format format\tformat used by METRo ('direct' (default) 'xml' or 'csv')\n";
  cerr << "-use_flux which\t\tuse flux from the weather forecast instead of computing\n\t\t\tit from cloud cover ('solar', 'ir', or 'both')\n";
  cerr << "-mat_default name\tuse material default configuration in site_config_file\n\t\t\twith SITE_ID 'name' for unspecified sites\n";
  exit(2);
}


//  
//  Routine: main()
//  
//  Description:
//       Entry point for this program. Command line arguments are handled and passed to
//  the main driver routine.
//
//  Inputs:
//    argc........The number of command line arguments
//    argv........An array containing the command line arguments
//
//  Outputs:
//    None.
//
//  Returns:
//    0 if the program ran successfully.
//    1 if a failure condition was detected.
//

int 
main(int argc, char **argv)
{
  int exit_status = 0;
  double dforc_time = 0;
  long treatment_option = 0;
  char *treatment_def_file_name = 0;
  char *site_file_name = 0;
  char *bridge_site_file_name = 0;
  char *wx_file_name = 0;
  char *cdl_name = 0;
  char *last_rc_file_name = 0;
  char *obs_file_name = 0;
  char *site_config_name = 0;
  char *output_name = 0;
  char **str_ptr = 0; 
  int i;
  int metro_run_type = METRO_DIRECT;
  //int metro_run_type = METRO_CSV;
  char *metro_format = 0;
  char *use_flux = 0;
  char *mat_default = 0;

  if (argc < 12) {
    cerr << "Not enough command-line arguments\n";
    usage(argv);
  }

  // Use command line arguments to initialize internal vars
  dforc_time = strtod(argv[1], str_ptr);
  site_file_name = argv[2];
  bridge_site_file_name = argv[3];
  cdl_name = argv[4];
  site_config_name = argv[5];
  treatment_option = strtol(argv[6], str_ptr, 10);
  treatment_def_file_name = argv[7];
  last_rc_file_name = argv[8];
  obs_file_name = argv[9];
  wx_file_name = argv[10];
  output_name = argv[11];
  

  // Set up debug level
  i=1;
  while(i<argc) {
    if (strcmp(argv[i], "-d") == 0) {
      debug_level = strtol(argv[i+1], (char **)0, 10);
      i++;
    }
    else if (strcmp(argv[i], "-l") == 0) {
      i++;
      proc_log = new Log(argv[i]);
    }
    else if (strcmp(argv[i], "-metro_format") == 0) {
      metro_format = argv[i+1];
      if (0 == strcmp(metro_format, "xml"))
	metro_run_type = METRO_XML;
      else if (0 == strcmp(metro_format, "csv"))
	metro_run_type = METRO_CSV;
      else if (0 == strcmp(metro_format, "direct"))
	metro_run_type = METRO_DIRECT;
      else
	{
	  cerr << "Error: Invalid value for -metro_format: "
	       << metro_format << endl;
	  usage(argv);
	}
      i++;
    }
    else if (strcmp(argv[i], "-use_flux") == 0) {
      use_flux = argv[i+1];
      if (strcmp(use_flux, "solar") != 0 && strcmp(use_flux, "ir") != 0 &&
	  strcmp(use_flux, "both") != 0)
	{
	  cerr << "Error: Invalid value for -use_flux: "
	       << use_flux << endl;
	  usage(argv);
	}
      i++;
    }
    else if (strcmp(argv[i], "-mat_default") == 0) {
      mat_default = argv[i+1];
      i++;
    }
    else if (*argv[i] == '-')
      {
	cerr << "Error: Unknown option: " << argv[i] << endl;
	usage(argv);
      }
    i++;
  }

  // Set up logging
  if (proc_log == 0)
    proc_log = new Log("");


  if (proc_log->error()) {
    cerr << argv[0] << "Error: Unable to create log file " << endl;
    exit(1);
  }

  proc_log->set_debug(debug_level);

  set_new_handler(out_of_store);

  proc_log->write_time_starting( argv[0] );

  // Call the module which drives all the real work.

  exit_status = road_cond(dforc_time, site_file_name, bridge_site_file_name, 
			  (int)treatment_option, treatment_def_file_name, 
			  last_rc_file_name, obs_file_name,
			  wx_file_name, cdl_name, metro_run_type,
			  use_flux, mat_default,
			  site_config_name, output_name);

  proc_log->write_time("Ending: exit status = %d.\n", exit_status);

  delete proc_log;

  return (exit_status);
}
