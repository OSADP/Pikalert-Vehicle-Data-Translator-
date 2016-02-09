//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2013/09/27 16:47:01 $
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
  
  begin_time = 0;
  begin_time_string = "";
  end_time = 0;
  end_time_string = "";
  error = "";
  debug = 0;
  log_file = "";
  road_wx_fcst_file = "";
  road_cond_fcst_file = "";
  vdt_seg_stats_file = "";
  vdt_seg_assign_wx_file = "";
  prev_rwh_conf_file = "";
  test = false;
  
  // Parse command line options
  while ((c = getopt(argc, argv, "a:d:l:n:p:s:r:w:t")) != EOF)
    {
      switch (c)
        {
        case 'a':
          vdt_seg_assign_wx_file = optarg;
          break;
	  
        case 'd':
          debug = (int)strtol(optarg, &ptr, 10);
          if (debug == 0 && ptr == optarg)
            {
              error = string("bad debug level");
              return;
            }
          break;

	case 'l':
          log_file = optarg;
          break;

        case 's':
	  vdt_seg_stats_file = optarg;
	  break;
	  
        case 'r':
          road_cond_fcst_file = optarg;
          break;

        case 'w':
          road_wx_fcst_file = optarg;
          break;

        case 'p':
          prev_rwh_conf_file = optarg;
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
  else if (!test && (argc - optind) < 6)
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

      begin_time = datetime_2_utime((char *)(begin_time_string.c_str()) );
      end_time = datetime_2_utime((char *)(end_time_string.c_str()));
      
      fcst_road_seg_site_list_file = string(argv[optind++]);
      vdt_road_seg_file = string(argv[optind++]);
      output_file = string(argv[optind++]);
    }
  
  get_command_string(argc, argv, command_string);
}

void arguments::usage(char *program_name)
{
  fprintf(stderr, "usage: %s [-w road_wx_fcst_file] [-r road_cond_fcst_file] [-s vdt_seg_stats_file] [-a vdt_seg_assign_wx_file] [-p prev_rwh_conf_file] [-l log_file] [-d debug_level] [-t] config_file begin_time_string end_time_string fcst_road_seg_site_list_file vdt_road_seg_file rwh_output_file \n", program_name);
  fprintf(stderr, "config_file : configuration file for road weather hazzard application\n");
  fprintf(stderr, "begin_time_string : begin time string in form yyyymmddhhmm of time window of interest\n");
  fprintf(stderr, "end_time_string : end time string in form yyyymmddhhmm of time window of interest\n");
  fprintf(stderr, "fcst_road_seg_site_list_file : ascii file with road segment site list entries for retrieving forecast data\n");
  fprintf(stderr, "vdt_road_seg_file : netcdf file with road segments data\n");
  fprintf(stderr, "output_file : output file with road weather hazard data\n");
  fprintf(stderr, "-w road_wx_fcst_file : input file with road segment weather forecast data\n");
  fprintf(stderr, "-r road_cond_fcst_file : input file with road segment conditions forecast data\n");
  fprintf(stderr, "-s vdt_seg_stats_file : input file with VDT Stage II Segment Statistics data\n");
  fprintf(stderr, "-a vdt_seg_assign_wx_file : input file with VDT Stage II Road-Weather Assignment data\n");
  fprintf(stderr, "-p prev_rwh_conf_file : input file with a list of prevoius RWH files\n");
  fprintf(stderr, "-l log_file: write log messages to log_file\n");
  fprintf(stderr, "-d debug_level: set debug diagnostic level\n");
  fprintf(stderr, "-t: run tests dictated by configuration file. Note if -t is specified, the configuration file is the only other command line parameter needed.\n");
}

time_t arguments::datetime_2_utime(char *datetime)
{
  struct tm time_tm;
  int ret = sscanf(datetime, "%4d%02d%02d%02d%02d", &time_tm.tm_year, &time_tm.tm_mon, &time_tm.tm_mday, &time_tm.tm_hour, &time_tm.tm_min);
  if(ret != 5)
    {
      fprintf(stderr, "Error: invalid date: %s, format must be YYYYMMDDHHMM\n", datetime);
      return -1;
    }
  
  time_tm.tm_year -= 1900;
  time_tm.tm_mon -= 1;
  time_tm.tm_sec = 0;
  
  time_t utime = timegm(&time_tm);
  return utime;
}
