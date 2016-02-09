/**
 *
 * @file   StringUtil.hh
 *
 * @note   String handling utilities. Taken from web site.
 *
 * @author Jim Cowie
 *
 * @date   11/06/2007
 *
 */


#include <string>
#include <vector>

using namespace std;


/**
 * @brief splitString
 * @param str - string which will be split into tokens
 * @param tokens - vector of strings returned to caller
 * @param delimeter - set of delimeters (default = " ")
 * @return void (substrings are returned in tokens)
 *
 */

void splitString(const string& str, vector<string>& tokens,
		 const string& delimiters);
