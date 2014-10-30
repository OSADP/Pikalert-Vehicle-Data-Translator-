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
 * Module: nc_site.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   6/25/99
 *
 * Description:
 *     
 */

// Include files 
#include <stdio.h>
#include <ncf/ncf.hh>
#include <nxncf/nxncf.hh>

// Constant, macro and type definitions 

// Global variables 

// Functions and objects



NXNCF_site::NXNCF_site(NcFile *ncfp, int *sa, int sa_size) : site_index(sa_size)
{
  int i;
  int ind;
  maptype site_map;
  int *store;
  int sl_size;

  size = sa_size;
  sl_size = NXNCF_get_sites(*ncfp, &store);
  for (i=0; i<sl_size; i++)
    site_map[store[i]] = i;


  for (i=0; i<size; i++)
    {
      ind = site_map.count(sa[i]);

      if (ind == 0) 
	  site_index[i] = -1;
      else 
	site_index[i] = site_map[sa[i]];
    }

  delete [] store;
}

NXNCF_site::~NXNCF_site()
{
}

