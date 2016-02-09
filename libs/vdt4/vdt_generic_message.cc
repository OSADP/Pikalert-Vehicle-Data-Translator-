//==============================================================================
//
//   (c) Copyright, 2015 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file vdt_generic_message.cc
 *
 * <file description>
 *
 * @date 2/3/15
 */

// Include files 
#include "vdt_const.hh"
#include "vdt_generic_message.hh"

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


void vdt_generic_message::init()
{
  obs_time = vdt_const::FILL_VALUE;
  rec_time = vdt_const::FILL_VALUE;
  vehicle_id = "";
  point.set_longitude(vdt_const::FILL_VALUE);
  point.set_latitude(vdt_const::FILL_VALUE);
  point.set_elevation(vdt_const::FILL_VALUE);
}

vdt_generic_message::vdt_generic_message()
{
  init();
}

vdt_generic_message::~vdt_generic_message() {}
