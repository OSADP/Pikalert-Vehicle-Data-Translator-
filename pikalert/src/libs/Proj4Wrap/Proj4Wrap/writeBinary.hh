//==============================================================================
//
//   (c) Copyright, 2011 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: writeBinary.hh,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2012/01/04 00:34:21 $
//
//==============================================================================

/**
 * @file writeBinary.hh
 *
 * @class writeBinary
 *
 *  <file/class description>
 *  
 * @date 9/12/11
 */

#ifndef WRITE_BINARY_HH
#define WRITE_BINARY_HH

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * @brief writeBinary
 *
 * @param[in] field field to write to output file
 * @param[in] outFile output file name
 * @param[out] error error string
 * @return 0 on success, -1 on failure
 * 
 */
int writeBinary(vector<float> &field, const string &outFile, string &error);

#endif /* WRITE_BINARY_HH */
