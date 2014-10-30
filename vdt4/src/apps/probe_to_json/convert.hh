//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: convert.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2014/04/04 18:39:53 $
//
//==============================================================================

/**
 * @file convert.hh
 *
 * @class Convert
 *
 *  <file/class description>
 *  
 * @date 4/4/14
 */

#ifndef CONVERT_HH
#define CONVERT_HH

#include <string>
#include <udunits2.h>

using std::string;

/**
 * @class Convert
 */
class Convert
{
public:
  /** Constructor */
  Convert();

  float convert(const string &input_units, const string &output_units, float value);

  double convert(const string &input_units, const string &output_units, double value);

  /** Destructor */
  virtual ~Convert();

  /** @brief pressure a variable containing the pressure */


private:
  ut_system *unitSystem;

  Convert & operator=(const Convert &convert);
  Convert(const Convert &convert);
};


#endif /* CONVERT_HH */
