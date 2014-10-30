//==============================================================================
//
//   (c) Copyright, 2013 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: rwx_types.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2014/02/19 16:38:10 $
//
//==============================================================================

/**
 * @file rwx_types.cc
 *
 * <file description>
 *
 * @date 12/13/13
 */

// Include files 
#include <netcdf>
#include "rwx_types.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int rwx::value_t::get_value(double *value_ptr)
{
  switch (v_type)
    {
    case NC_SHORT:
      *value_ptr = v_short;
      return NC_SHORT;
      break;

    case NC_INT:
      *value_ptr = v_int;
      return NC_INT;
      break;

    case NC_FLOAT:
      *value_ptr = v_float;
      return NC_FLOAT;
      break;

    case NC_DOUBLE:
      *value_ptr = v_double;
      return NC_DOUBLE;
      break;

    case NC_CHAR:
      return NC_CHAR;
    }

  return -1;
}
