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
 * @file vdt_exception.hh
 *
 * General utility functions.
 *  
 * @date 12/23/14
 */

#ifndef VDT_EXCEPTION_HH
#define VDT_EXCEPTION_HH

#include <exception>
#include <string>
#include <boost/format.hpp>
#include <cstring>

#define MAX_ERROR_LEN 512
 
class vdt_exception : public std::exception
{
  char msg[MAX_ERROR_LEN];
public:
  vdt_exception(const std::string& msg, const std::string& funcname, const std::string& filename, unsigned int linum)
  {
      const std::string errorstr = str(boost::format("ERROR in %1% at %2%:%3%\nMessage: %4%") % funcname % filename % linum % msg);
      strncpy(this->msg, errorstr.c_str(), MAX_ERROR_LEN-1);
      this->msg[MAX_ERROR_LEN-1] = '\0';
  }


  virtual const char* what() const throw()
  {
      return msg;
  }
}; 

#define VDT_EXCEPTION(msg) \
	vdt_exception(msg, __PRETTY_FUNCTION__, __FILE__, __LINE__)
 
#endif /* VDT_EXCEPTION_HH */
