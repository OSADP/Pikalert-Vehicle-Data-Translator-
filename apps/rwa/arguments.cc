//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.cc,v $
//       Version: $Revision: 1.9 $  Dated: $Date: 2014/05/13 20:42:40 $
//
//==============================================================================

/**
 *
 * @file apps/probe_message_dataset_manager/arguments.cc
 *
 *   Implementation for simple class that parses command line arguments.
 *
 */

// Include files
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/format.hpp>
#include "arguments.hh"

using boost::format;

// Constant and macros

// Types, structures and classes

// Global variables

// Functions

void get_command_string(int argc, char **argv, string &command_string)
{
  for (int i=0; i<argc-1; i++)
    {
      command_string += string(argv[i]) + string(" ");
    }

  command_string += string(argv[argc-1]);
}

arguments::arguments(int argc, char **argv)
{
  int c = 0;
  char *ptr = 0;
  int errflg = 0;

  test = false;
  error = "";
  log_file = "";
  rwh_input_file = "";
  cfg_file = "";
  site_list_file = "";
  treatment_input_file = "";
  obs_stats_input_file = "";
  maw_output_file = "";

  // Parse command line options
  while ((c = getopt(argc, argv, "d:l:s:t:T")) != EOF)
    {
      switch (c)
        {
        case 'l':
          log_file = optarg;
          break;

        case 's':
          obs_stats_input_file = optarg;
          break;

        case 't':
          treatment_input_file = optarg;
          break;

	case 'T':
	  test = true;
	  break;

        case '?':
          errflg = 1;
	  break;
        }
    }

  if (errflg)
    {
      error = "bad command line option";
      return;
    }

  ptr = strrchr(argv[0], '/');
  if (ptr == NULL)
    ptr = argv[0];
  else
    ptr++;

  program_name = string(ptr);

  if ((argc - optind) < 3)
    {
      usage(argv[0]);
      exit(2);
    }

  date_time_string = string(argv[optind++]);
  cfg_file = string(argv[optind++]);
  site_list_file = string(argv[optind++]);
  rwh_input_file = string(argv[optind++]);
  alerts_output_file = string(argv[optind++]);
  plots_output_file = string(argv[optind++]);
  obs_stats_output_file = string(argv[optind++]);
  maw_output_file = string(argv[optind++]);

  get_command_string(argc, argv, command_string);
}

void arguments::usage(char *program_name)
{
  fprintf(stderr, "usage: %s [-l log_file] [-s observation statistics file (road segment statistics file)] [-t treatment input file] yyyymmddhhmm config_file ascii_site_list_file input_file alerts_output_file plots_output_file obs_stats_output_file maw_phone_app_output_file\n", program_name);
  fprintf(stderr, "-s observation statistics file: file contains road segment and rwis site statistics\n");
  fprintf(stderr, "-t treatment input file: treatment input netcdf file.\n");
  fprintf(stderr, "-l log_file: write log messages to log_file\n");
}
