//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file vdt_util.cc
 *
 * General utility functions.
 *
 * @date 12/23/14
 */

// Include files 
#include "vdt_util.hh"
#include <sys/stat.h>

// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions


bool fileExists(const std::string& filename)
{
    struct stat buf;
    return stat(filename.c_str(), &buf) != -1;
}
