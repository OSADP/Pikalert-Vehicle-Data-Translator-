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
#include "arguments.hh"

using boost::format;

// Constant and macros

// Types, structures and classes

// Global variables

// Functions
void zeroTms(struct tm *ptm)
{
  ptm->tm_sec = 0;
  ptm->tm_min = 0;
  ptm->tm_hour = 0;
  ptm->tm_mday = 0;
  ptm->tm_mon = 0;
  ptm->tm_year = 0;
  ptm->tm_wday = 0;
  ptm->tm_yday = 0;
  ptm->tm_isdst = 0;
}

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

  add_non_associated_segments = false;
  begin_time = 0;
  begin_time_string = "";
  end_time = 0;
  end_time_string = "";
  error = "";
  log_file = "";
  metar_file = "";
  probe_message_qc_output = false;
  radar_file_pattern = "";
  rwis_file = "";
  cloud_class_file = "";
  rtma_file = "";
  cfg_file = "";
  debug = 0;
  grid_cell_statistics_out_file = "";
  road_seg_file = "";
  probe_message_file = "";
  probe_message_qc_output = true;
  probe_message_qc_out_file = "";
  probe_message_qc_statistics_out_file = "";
  test = false;
  old_radar = false;

  // Parse command line options
  while ((c = getopt(argc, argv, "B:d:g:H:l:m:nr:p:w:c:a:st")) != EOF)
    {
      switch (c)
        {
        case 'a':
          rtma_file = optarg;
          break;

        case 'c':
          cloud_class_file = optarg;
          break;

        case 'd':
          debug = (int)strtol(optarg, &ptr, 10);
          if (debug == 0 && ptr == optarg)
            {
              error = string("bad debug level");
              return;
            }
          break;

	case 'g':
	  grid_cell_statistics_out_file = optarg;
	  break;

        case 'H':
          history_probe_message_file = optarg;
          break;

        case 'l':
          log_file = optarg;
          break;

        case 'm':
          metar_file = optarg;
          break;

        case 'n':
          probe_message_qc_output = false;
	  break;

	case 'p':
          radar_file_pattern_cref = optarg;
          break;
	  
        case 'r':
          radar_file_pattern = optarg;
          break;

	case 's':
	  add_non_associated_segments = true;
	  break;

        case 'w':
          rwis_file = optarg;
          break;

        case 't':
          test = true;
	  break;

	//Backwards compatible radar
	case 'B':
          radar_file_pattern = optarg;
	  old_radar = true;
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
  else if (!test && (argc - optind) < 7)
    {
      usage(argv[0]);
      exit(2);
    }

  cfg_file = string(argv[optind++]);


  if (!test)
    {
      // Assign names to command line parameters
      begin_time_string = string(argv[optind++]);
      end_time_string = string(argv[optind++]);

      //printf("begin_time_string: %s\n", begin_time_string.c_str());
      //printf("end_time_string: %s\n", end_time_string.c_str());

      struct tm time_tm;
      zeroTms(&time_tm);
      char *ret_val = strptime(begin_time_string.c_str(), "%Y%m%d%H%M", &time_tm);
      if (ret_val == NULL)
	{
	  error = "bad begin time string";
	  return;
	}

      begin_time = timegm(&time_tm);
      zeroTms(&time_tm);
      ret_val = strptime(end_time_string.c_str(), "%Y%m%d%H%M", &time_tm);
      if (ret_val == NULL)
	{
	  error = "bad end time string";
	  return;
	}

      end_time = timegm(&time_tm);

      road_seg_file = string(argv[optind++]);
      probe_message_file = string(argv[optind++]);
      probe_message_qc_out_file = string(argv[optind++]);
      probe_message_qc_statistics_out_file = string(argv[optind++]);
    }

  get_command_string(argc, argv, command_string);
}

void arguments::usage(char *program_name)
{
  fprintf(stderr, "usage: %s [-a rtma_file] [-c cloud_class_file] [-d debug] [-g grid_cell_statistics_file] [-H history_probe_message_file] [-m metar_file] [-l log_file] [-r radar_file_pattern] [-t] [-w rwis_file] config_file begin_time_string end_time_string road_seg_file probe_message_file probe_message_qc_out_file probe_message_qc_statistics_out_file\n", program_name);
  fprintf(stderr, "-H history_probe_message_file: read history probe message file for performing persistence and step checks\n");
  fprintf(stderr, "-a rtma_file: input file with rtma model data\n");
  fprintf(stderr, "-c cloud_class_file: input file with cloud classification data\n");
  fprintf(stderr, "-d debug_level: set debug diagnostic level\n");
  fprintf(stderr, "-g grid_cell_statistics_file: write grid cell statistics to this file\n");
  fprintf(stderr, "-l log_file: write log messages to log_file\n");
  fprintf(stderr, "-m metar_file: input file with metar data\n");
  fprintf(stderr, "-n: do not write probe message qc file\n");
  fprintf(stderr, "-p radar_file_pattern: input file for cref radar data\n");
  fprintf(stderr, "-r radar_file_pattern: input file for bref radar data\n");
  fprintf(stderr, "-s: add non-associated road segments to road segment stats calculations (useful when limited or no probe messages)\n");
  fprintf(stderr, "-t: run tests dictated by configuration file. Note if -t is specified, the configuration file is the only other command line parameter needed.\n");
  fprintf(stderr, "-w rwis_file: input file with rwis data \n");
  fprintf(stderr, "Note: begin_time_string and end_time_string are of form yyyymmddhhmm\n");
}
