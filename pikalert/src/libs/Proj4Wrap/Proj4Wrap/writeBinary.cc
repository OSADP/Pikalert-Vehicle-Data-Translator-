//==============================================================================
//
//   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: writeBinary.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2012/01/04 00:34:20 $
//
//==============================================================================

/**
 * @file writeBinary.cc
 *
 * <file description>
 *
 * @date 9/12/11
 */

// Include files 
#include <iostream>
#include <fstream>
#include "writeBinary.hh"

using std::fstream;
using std::ofstream;
using std::ios;

  
// Constants and macros

// Types

// Structures, unions and classes

// Global variables 

// Functions

int writeBinary(vector<float> &field, const string &outFile, string &error)
{
  ofstream outStream;
  outStream.open(outFile.c_str(), ios::out | ios::binary | ios::trunc);
  if (!outStream)
    {
      error = string("failed to open output file: ") + outFile;
      return -1;
    }

  outStream.write((char *)&field[0], field.size() * sizeof(float));
  if (!outStream)
    {
      error = string("failed writing to output file: ") + outFile;
      return -1;
    }

  outStream.close();
  return 0;
}
