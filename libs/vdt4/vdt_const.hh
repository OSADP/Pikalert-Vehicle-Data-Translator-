//==============================================================================
//
//   (c) Copyright, 2012 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//
//       File: $RCSfile: vdt_const.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2013-04-10 17:43:27 $
//
//==============================================================================

/**
 *
 * @file vdt_const.hh
 *
 * @class vdt_const
 *
 *  vdt constant class
 *
 */
#ifndef VDT_CONST_HH
#define VDT_CONST_HH

/**
 * @class vdt_const
 * Class for vdt constant values
 */
class vdt_const
{

public:

  /** @brief upper bound on the relative error (epsilon fill value) */
  static const float FILL_VALUE_EPS;

  /** @brief floating point fill value */
  static const float FILL_VALUE;

  /** @brief short fill value */
  static const short SHORT_FILL_VALUE;
};

#endif /* VDT_CONST_HH */
