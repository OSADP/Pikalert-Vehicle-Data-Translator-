//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: arguments.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 18:32:17 $
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

  error = "";
  log_file = "";
  cfg_file = "";
  time_delta = 3600;

  // Parse command line options
  while ((c = getopt(argc, argv, "d:l:t:")) != EOF)
    {
      switch (c)
        {
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

        case 't':
          time_delta = (int)strtol(optarg, &ptr, 10);
          if (time_delta == 0)
            {
              error = string("bad time_delta");
              return;
            }
          break;

        case '?':
          errflg = 1;
        }
    }

  if (argc < 4)
    {
      usage(argv[0]);
      exit(2);
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

  // Assign names to command line parameters
  cfg_file = string(argv[optind++]);

  for (int i=optind; i<argc-1; i++)
    {
      probe_message_file_list.push_back(argv[i]);
    }

  out_file = argv[argc-1];
  get_command_string(argc, argv, command_string);
}

void arguments::usage(char *program_name)
{
  fprintf(stderr, "usage: %s [-d debug] [-l log_file] config_file probe_message_file[s] out_file\n", program_name);
  fprintf(stderr, "-l log_file: write log messages to log_file\n");
}
