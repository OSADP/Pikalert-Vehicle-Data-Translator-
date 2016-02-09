//==============================================================================
//
//   The source of this product, which was developed with funding from the
//   U.S. Department of Transportation under contract number
//   DTFH61-08-C-00004. Copyright ©2013, UCAR. This product includes quality
//   checking algorithms derived from code provided by Mixon-Hill, Inc. 
//
//       File: $RCSfile: arguments.cc,v $
//       Version: $Revision: 1.5 $  Dated: $Date: 2014-02-21 18:07:04 $
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
#include <boost/lexical_cast.hpp>
#include "arguments.hh"
#include "get_time.hh"

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

  begin_time = 0;
  begin_time_string = "";
  end_time = 0;
  end_time_string = "";
  error = "";
  log_file = "";

  cfg_file = "";
  probe_message_out_file_base = "";
  test = false;
  time_delta = 0;
  car_density = 0;
  debug_segments = false;
  csv_out_file_base = "";
  dicast_csv_out_file_base = "";
  do_wipers_only = false;

  // Parse command line options
  while ((c = getopt(argc, argv, "a:c:d:Dg:H:l:m:nr:R:tw:W")) != EOF)
    {
      switch (c)
        {
        case 'a':
	  file_list_position["RTMA"] = file_list_names.size();
          file_list_names.push_back(optarg);
          break;

        case 'c':
          car_density = (int)strtol(optarg, &ptr, 10);
          if (car_density == 0 && ptr == optarg)
            {
              error = string("bad car density");
              return;
            }
          break;

        case 'd':
          debug = (int)strtol(optarg, &ptr, 10);
          if (debug == 0 && ptr == optarg)
            {
              error = string("bad debug level");
              return;
            }
          break;

        case 'D':
          debug_segments = true;
          break;

        case 'l':
          log_file = optarg;
          break;

        case 'm':
	  file_list_position["METAR"] = file_list_names.size();
          file_list_names.push_back(optarg);
          break;

        case 'r':
	  file_list_position["RADAR"] = file_list_names.size();
          file_list_names.push_back(optarg);
          break;

        case 'w':
	  file_list_position["RWIS"] = file_list_names.size();
          file_list_names.push_back(optarg);
          break;

	case 'W':
	  do_wipers_only = true;
	  break;

        case 't':
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

  if (test && (argc - optind) < 1)
    {
      fprintf(stderr, "Error: when specifying the -t option, you also need to specify a configuration file\n");
      usage(argv[0]);
      exit(2);
    }
  else if (!test && (argc - optind) < 5)
    {
      usage(argv[0]);
      exit(2);
    }

  cfg_file = string(argv[optind++]);

  if (!test)
    {
      // Assign names to command line parameters
      time_delta = boost::lexical_cast<unsigned int>(argv[optind++]);
      begin_time_string = string(argv[optind++]);
      end_time_string = string(argv[optind++]);

      int ret = get_time(begin_time_string, &begin_time);
      if (ret < 0)
	{
	  error = "bad begin time string";
	  return;
	}

      ret = get_time(end_time_string, &end_time);
      if (ret < 0)
	{
	  error = "bad end time string";
	  return;
	}

      probe_message_out_file_base = string(argv[optind++]);

      if (do_wipers_only)
	{
	  dicast_csv_out_file_base = string(argv[optind++]);
	}
      else
	{
	  csv_out_file_base = string(argv[optind++]);
	}
    }

  get_command_string(argc, argv, command_string);
}

void arguments::usage(char *program_name)
{
  fprintf(stderr, "usage: %s [-a rtma_file_list_name] [-c car_density] [-d debug] [-m metar_file_list_name] [-l log_file] [-r radar_file_list_name] [-t] [-w rwis_file_list_name] [-W] config_file time_delta begin_time end_time probe_message_out_file_base csv_out_file_base\n", program_name);
  fprintf(stderr, "-a rtma_file_list_name: rtma model data input file list name\n");
  fprintf(stderr, "-c car_density: 0 - less dense, 1 - more dense. Note: defaults to less dense if -c is not used.\n");
  fprintf(stderr, "-d debug_level: set debug diagnostic level\n");
  fprintf(stderr, "-l log_file: write log messages to log_file\n");
  fprintf(stderr, "-m metar_file_list_name: metars data input file list name\n");
  fprintf(stderr, "-r radar_file_list_name: radar data input file list name\n");
  fprintf(stderr, "-t: run tests dictated by configuration file. Note if -t is specified, the configuration file is the only other command line parameter needed.\n");
  fprintf(stderr, "-w rwis_file_list_name: rwis data input file list name\n");
  fprintf(stderr, "-W: wipers only\n");
  fprintf(stderr, "Note: begin_time_string and end_time_string are of form yyyymmddhhmm\n");
}
