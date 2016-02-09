//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//==============================================================================

/**
 *
 * @file process.cc
 *
 */

// Include files
#include <algorithm>
#include <log/log.hh>
#include <libconfig.h++>
#include <stdio.h>
#include <stdlib.h>
#include "rwx/rwx_rtma_dataset.hh"
#include "process.hh"

using namespace std;

extern Log *Logg;

process::~process()
{
}

int process::run()
{
  Logg->write_time_info("reading input file %s\n", args.input_file.c_str());


  rwx_rtma_dataset rtma_dataset(args.input_file);
  if (rtma_dataset.error != string(""))
    {
      Logg->write_time("Error: dataset error for %s: %s\n", args.input_file.c_str(), rtma_dataset.error.c_str());
      return -1;
    }

  return 0;
}


