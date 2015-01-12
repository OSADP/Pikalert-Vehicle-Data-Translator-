//==============================================================================
//
//   (c) Copyright, 2008 University Corporation for Atmospheric Research (UCAR).
//       All rights reserved.
//       Do not copy or distribute without authorization.
//
//       File: $RCSfile: stat_tools.cc,v $
//       Version: $Revision: 1.1 $  Dated: $Date: 2010/04/29 16:16:31 $
//
//==============================================================================

/**
 *
 * @file stat_tools.cc
 *
 * <file description>
 *
 * @note <brief note description> [optional]
 *
 * @date 5/23/08
 *
 * @todo [optional]
 *
 */

// Include files 
#include "../include/stat_tools/stat_tools.hh"

// Constant and macros

// Types, structures and classes

// Global variables 

char *ST_stat_list_cstring[] = {"ST_COUNT", "ST_MAX", "ST_MEAN", "ST_MEDIAN", "ST_MIN", "ST_STANDARD_DEVIATION", "ST_VARIANCE", "ST_HISTOGRAM", "ST_MAX_INDICES"};

map<string, int> ST_stat_map;

void ST_init_stat_map()
{
  int size = sizeof(ST_stat_list_cstring)/sizeof(char *);

  for (int i=0; i<size; i++)
    {
      ST_stat_map[string(ST_stat_list_cstring[i])] = i;
    }

}
