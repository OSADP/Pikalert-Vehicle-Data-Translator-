// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: create_index_array.cc
 *
 * Author: Melissa Petty
 *
 * Date:   6/21/99
 *
 * Description:
 *     
 */

/* System include files / Local include files */
#include <algorithm>
#include "nxncf/nxncf.hh"

long NXNCF_create_index_array(int *sl_arr, long num_sites, NcFile *ncfile, long **ind_array)
{
  long i;
  vector<int>::iterator fnd;
  long *new_arr;
  long num_ncf_sites;
  int *ncf_site_arr = 0;
  vector <int> ncf_list;

  num_ncf_sites = NXNCF_get_sites(*ncfile, &ncf_site_arr);
  if (num_ncf_sites < 0) 
    return(-1);

  for (i=0; i<num_ncf_sites; i++)
    ncf_list.push_back(ncf_site_arr[i]);

  new_arr = new long[num_sites];

  for (i=0; i<num_sites; i++) {
    fnd = find(ncf_list.begin(), ncf_list.end(), sl_arr[i]);
    if (fnd < ncf_list.end()) {
      if (*fnd == sl_arr[i])
	new_arr[i] = fnd - ncf_list.begin();
      else 
	new_arr[i] = -1;
    }
    else
      new_arr[i] = -1;
  }

  delete [] ncf_site_arr;
  *ind_array = new_arr;

  return (num_sites);
}





