// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2010 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** 2010/10/7 23:12:33 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: cdltonc.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   6/18/99
 *
 * Description:
 *     Copied from Missy Petty's code.
 */

/* System include files / Local include files */
#include <cstdio>
#include <cstdlib>  
#include <strings.h>
#include <cstring>
#include <sys/stat.h>

/* Constant definitions / Macro definitions / Type definitions */

/* External global variables / Non-static global variables / Static globals */

/* External functions / Internal global functions / Internal static functions */

#include <ncf/ncf.hh>
using namespace std;

int check_parent_dir(const char *file_name)
{
  struct stat path_stat;
  char *slash_ptr = 0;
  int ret;

  slash_ptr = strrchr((char *)file_name, '/');
  if (!slash_ptr)
    /* no slash, parent_dir is working dir and therefore exists */
    return (0);
  
  int length = slash_ptr - file_name;


  char *pdirect_name = new char[length+1];
  strncpy(pdirect_name, file_name, length);
  pdirect_name[length] = '\0';
  ret = stat(pdirect_name, &path_stat);
  delete [] pdirect_name;

  return (ret);
}


int NCF_cdl2nc(const char *cdl_file, const char *ncfile, int nc_version)
{
  char *command_string = 0;
  const char *ncgen_cmd = "ncgen";
  char ver_str[6];
  int command_len = 0;
  int ret = 0;
  struct stat infile_stat;

  /* check to see if output directory exists */
  if ((ret=check_parent_dir(ncfile)) < 0)
    return (-1);

  if ((ret=stat(cdl_file, &infile_stat)) < 0)
    return (-1);

  if (nc_version > 1)
    sprintf(ver_str, "-k %d", nc_version);
  else
    ver_str[0] = '\0';

  command_len = strlen(cdl_file) + strlen(ncfile) + strlen(ncgen_cmd) + strlen(ver_str)
    + 20;

  command_string = new char[command_len];
  sprintf(command_string, "%s %s -b -o %s %s", ncgen_cmd, ver_str, ncfile, cdl_file);
  
  ret = system(command_string);
  delete [] command_string;

  if (ret != 0)
    return (-1);
  else
    return (0);
}

// Backward compatible version which does not have the ability to specify
// the kind of netcdf file format.

int NCF_cdl2nc(const char *cdl_file, const char *ncfile)
{
  return(NCF_cdl2nc(cdl_file, ncfile, 1));
}
