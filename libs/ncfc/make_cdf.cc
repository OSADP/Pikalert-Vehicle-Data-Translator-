//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: make_cdf.cc,v $
//       Version: $Revision: 1.8 $  Dated: $Date: 2014/10/20 20:37:08 $
//
//==============================================================================

/**
 *
 * @file make_cdf.cc
 *
 * @date 5/8/08
 *
 */

// Include files 
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include "../include/ncfc/ncfc.hh"

using std::string;
using std::stringstream;


// Constant and macros

// Types, structures and classes

// Global variables 

// Functions

int NCFC_make_cdf(const string &cdl_file, const string &out_file, string &error)
{
  // Make netcdf output file
  const int COMMAND_LEN = 8192;
  char command[COMMAND_LEN];

  int ret = snprintf(command, COMMAND_LEN, "ncgen -b %s -o %s", cdl_file.c_str(), out_file.c_str());

  if (ret == COMMAND_LEN)
    {
      stringstream sstream;
      sstream << "command line args to long, command buffer size = " << COMMAND_LEN;
      error = sstream.str();
      return 1;
    }

  ret = system(command);
  if (ret != 0)
    {
      stringstream sstream;
      sstream << "could not ncgen output file " << out_file << " using " << cdl_file;
      error = sstream.str();
      return 1;
    }

  return 0;
}

int NCFC_make_64bit_cdf(const string &cdl_file, const string &out_file, string &error)
{
  // Make netcdf output file
  const int COMMAND_LEN = 8192;
  char command[COMMAND_LEN];

  int ret = snprintf(command, COMMAND_LEN, "ncgen -k2 -b %s -o %s", cdl_file.c_str(), out_file.c_str());

  if (ret == COMMAND_LEN)
    {
      stringstream sstream;
      sstream << "command line args to long, command buffer size = " << COMMAND_LEN;
      error = sstream.str();
      return 1;
    }

  ret = system(command);
  if (ret != 0)
    {
      stringstream sstream;
      sstream << "could not ncgen output file " << out_file << " using " << cdl_file;
      error = sstream.str();
      return 1;
    }

  return 0;
}
