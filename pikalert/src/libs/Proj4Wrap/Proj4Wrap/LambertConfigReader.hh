//==============================================================================
//
//   (c) Copyright, 2009 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: LambertConfigReader.hh,v $
//       Version: $Revision: 1.3 $  Dated: $Date: 2012/01/04 00:20:55 $
//
//==============================================================================

/**
 *
 * @file LambertConfigReader.hh
 *
 * @class LambertConfigReader
 *
 *  Configuration file reader class
 *  
 * @date 12/24/2009
 *
 */

#ifndef CONFIG_READER_HH
#define CONFIG_READER_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * @class LambertConfigReader
 */
class LambertConfigReader
{
public:

  /**
   * @brief LambertConfigReader constructor
   *
   * @param[in] fileName input configuration file
   * @note the variable error is set if the constructor fails
   */
  LambertConfigReader(const string &inputFilePathArg);

  /** @brief elevationDelta elevation delta in feet */
  float elevationDelta;

  /** @brief la1 first latitude */
  double la1;

  /** @brief lo1 first longitude */
  double lo1;

  /** @brief latin1 first intersection latitude */
  double latin1;

  /** @brief latin2 second intersection latitude */
  double latin2;

  /** @brief lov longitude of grid center */
  double lov;

  /** @brief dx x grid distance in meters */
  double dx;

  /** @brief dy y grid distance in meters */
  double dy;

  /** @brief nx number of x points */
  int nx;

  /** @brief ny number of y points */
  int ny;

  /** @brief projectionType projection type such as lambert */
  string projectionType;

  /** @brief inputFilePath input config file path */
  string inputFilePath;

  /** @brief error constructor error */
  string error;

private:
   LambertConfigReader & operator=(const LambertConfigReader &configReader);
   LambertConfigReader(LambertConfigReader  &configReader);
  
};


#endif /* CONFIG_READER_HH */
