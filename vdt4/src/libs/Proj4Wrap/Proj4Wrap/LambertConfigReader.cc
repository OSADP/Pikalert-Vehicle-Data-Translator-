//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: LambertConfigReader.cc,v $
//       Version: $Revision: 1.2 $  Dated: $Date: 2012/01/04 00:18:09 $
//
//==============================================================================

/**
 *
 * @file LambertConfigReader.cc
 *
 *  Configuration file reader class implementation
 *
 * @date 10/16/09
 */

// Include files 
#include <stdlib.h>
//#include <boost/foreach.hpp>
#include <boost/format.hpp>

// Constant and macros
//#define foreach         BOOST_FOREACH
//#define rforeach BOOST_REVERSE_FOREACH

using boost::format;

#include "LambertConfigReader.hh"
#include "libconfig.h++"

using namespace libconfig;

// Constant and macros
static const int MAX_LINE = 1024;

// Types, structures and classes

// Global variables 

// Functions

LambertConfigReader::LambertConfigReader(const string &inputFilePathArg) : inputFilePath(inputFilePathArg)
{
  Config cfg;

  error = string("");

  FILE *fp = fopen(inputFilePath.c_str(), "r");

  if (fp == NULL)
    {
      error = string("could not load config file ") + inputFilePath;
      return;
    }
  try
    {
      cfg.setAutoConvert(true);
      cfg.read(fp);
      fclose(fp);
      
      la1 = cfg.lookup("Projection.la1");
      lo1 = cfg.lookup("Projection.lo1");
      latin1 = cfg.lookup("Projection.latin1");
      latin2 = cfg.lookup("Projection.latin2");
      lov = cfg.lookup("Projection.lov");
      dx = cfg.lookup("Projection.dx");
      dy = cfg.lookup("Projection.dy");
      nx = cfg.lookup("Projection.nx");
      ny = cfg.lookup("Projection.ny");

      const char *projectionTypeString = cfg.lookup("Projection.projectionType");
      projectionType = projectionTypeString;
    }
  catch (ParseException ce)
    {
      const int size = MAX_LINE;
      char line[size];
      snprintf(line, size, " on line %d", ce.getLine());
      error = string("Config reader parse exception occurred: ") + string(ce.getError()) + line;
    }
  catch (SettingException ce)
    {
      error = string("Config reader setting exception occurred on: ") + ce.getPath();
    }
  catch (FileIOException ce)
    {
      error = string("Config reader file exception occurred on: ") + inputFilePath;
    }
  catch (ConfigException ce)
    {
      error = string("Config reader generic exception occurred");
    }
}

