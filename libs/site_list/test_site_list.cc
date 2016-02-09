// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:34:34 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: test_site_list.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   6/18/99
 *
 * Description:
 *     
 */

#include <stdio.h>
#include "site_list.hh"


int main(int argc, char **argv)
{
  const int ct = 10;
  int i, j;
  int *id_arr;
  float *latlon_arr, *alt_arr;
  long num_sites;
  int ret;
  struct site_loc site;
  int sites[ct];
  int site_store[ct];

  if (argc < 2) {
    cout << "usage: " << argv[0] << " input_site_file\n";
    exit(-1);
  }

  Site_list sl1(argv[1]);
  if (sl1.error()) {
    cerr << sl1.error() << "\n";
    exit(-1);
  }

  num_sites = sl1.make_id_array(&id_arr);

  /*
  num_sites = sl1.make_latlon_array(&latlon_arr);
  for (i=0; i<num_sites; i++) {
    cout << "lat = " << latlon_arr[i*2];
    cout << "    lon = " << latlon_arr[i*2+1] << "\n";
  }

  num_sites = sl1.make_alt_array(&alt_arr);
  for (i=0; i<num_sites; i++) 
    cout << "alt = " << alt_arr[i] << "\n";
  */
  string *ic_arr;
  num_sites = sl1.make_icao_array(&ic_arr);
  for (i=0; i<num_sites; i++) 
    cout << "icao = " << ic_arr[i] << "\n";



  delete[] id_arr;
  //  delete[] latlon_arr;
  delete[] alt_arr;

}
