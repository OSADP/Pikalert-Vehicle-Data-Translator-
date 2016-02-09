// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:33:35 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: get_sites.cc
 *
 * Author: Gerry Wiener
 *
 * Date:   6/18/99
 *
 * Description:
 *     Get sites from netcdf files. Adapted from Missy Petty's code.
 */

// Include files
#include "nxncf.hh"

// Constant, macro and type definitions 

// Global variables 

// Functions 


// This function allocates space for site_list. Use delete to free this space!
int NXNCF_get_sites(NcFile &ncf, int **site_ids)
{
  long num_sites = 0;
  NcVar *num_sites_var = 0;
  int site_list_size = 0;
  NcVar *st_list_var = 0;

  /* get site_list from netcdf file */
  st_list_var = ncf.get_var(SITE_LIST_NAME);
  if (st_list_var == 0)
    return(-1);

  num_sites_var = ncf.get_var(NUM_SITES_NAME);
  if(num_sites_var == 0)
    return(-1);

  if(!num_sites_var->get(&num_sites))
    return(-1);

  if (num_sites == NC_FILL_INT)
    return(-1);

  site_list_size = num_sites;
  *site_ids = new int[site_list_size];

  if (!st_list_var->get(*site_ids, num_sites))
    return(-1);
  else
    return(num_sites);
}
