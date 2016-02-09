//==============================================================================
//
//   (c) Copyright, 2014 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved. 
//
//       File: $RCSfile: fileheader,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/10/04 14:44:18 $
//
//==============================================================================

/**
 * @file get_time.hh
 *
 * @class Get_Time
 *
 *  <file/class description>
 *  
 * @date 12/15/14
 */

#ifndef GET_TIME_HH
#define GET_TIME_HH

#include <string>
#include <time.h>

using std::string;

/**
 * @brief Here is a brief description of get_time
 *
 * Here is an extended description.
 *
 * @param[in] time_string - input time string
 * @param[out] time_val - output time value
 * @return 0 on success, -1 on failure
 * 
 */
int get_time(const string &time_string, time_t *time_val);

/**
 * @brief Here is a brief description of zero_tms
 *
 * Here is an extended description.
 *
 * @param[in/out] ptm - pointer to time structure from time.h
 */
void zero_tms(struct tm *ptm);

#endif /* GET_TIME_HH */
