// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: att_as_string.cc
 *
 * Author: Melissa Petty
 *
 * Date:   4/7/00
 *
 * Description: converts an attribute to a string
 *     
 */

#include <ncf/ncf.hh>
#include <cstring>
using namespace std;

/* Allocates memory which should be deleted by the calling function */

char *NCF_att_as_string(NcAtt *att_ptr)
{
  int att_len = 0;
  char *att_str = 0;

  if (!att_ptr->is_valid())
    return (0);

  att_len = (att_ptr->num_vals() + 1);
  att_str = new char[att_len];

  char *str_ptr = att_ptr->as_string(0);
  strncpy(att_str, str_ptr, att_len);

  att_str[att_len-1] = '\0';

  return(att_str);
}
