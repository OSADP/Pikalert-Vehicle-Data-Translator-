/**
 *
 * @file   StringUtil.cc
 *
 * @note   String handling utilities. Taken from web site.
 *
 * @author Jim Cowie
 *
 * @date   11/06/2007
 *
 */


#include "StringUtil.hh"


// String splitting function. Takes string to split, str, delimeter
// (" " by default), and returns the vector tokens. This routine
// was taken off a website but modified to handle NULL tokens


void splitString(const string& str, vector<string>& tokens,
		 const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first delimiter.
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {

      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));

      // Set next starting position. This handles NULL tokens
      // (ie; adjacent delimeters).
      if (pos < str.size())
	lastPos = pos+1;
      else
	lastPos = str.find_first_not_of(delimiters, pos);

      // Find next delimiter
      pos = str.find_first_of(delimiters, lastPos);
    }

}
