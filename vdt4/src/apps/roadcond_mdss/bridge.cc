// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/* System include files / Local include files */
#include <string.h>
#include <iostream>
#include <new>
#include <vector>
#include <math.h>

#include <nxncf/nxncf.hh>
#include <site_list/site_list.hh>
#include <log/log.hh>
#include "WxFcst.hh"
#include "RoadCond.hh"
#include "SiteLayerTraffic.hh"

extern Log *proc_log;

//inline float celsius2kelvin(float temp_cel) {return (temp_cel + 273.15);};
//inline float kelvin2celsius(float temp_kel) {return (temp_kel - 273.15);};

int init_bridge_sites(char *bridge_site_file_name, int *output_site_array,
		      long num_output_sites, int *bridge)
{
  int i, j;
  int *bridge_site_array = 0;
  long num_bridge_sites = 0;

  for (i=0; i<num_output_sites; i++)
    bridge[i] = 0;

  // No Bridge file
  if (strcmp(bridge_site_file_name, "None") == 0) {
    return(0);
  }

  // Create bridge site list object
  Site_list bridge_sites(bridge_site_file_name);
  if (bridge_sites.error()) {
    // Bridge file not valid -- assume zero bridges
    proc_log->write_time("Warning: Bridge site list file empty or could not be opened. Assume no bridges.\n");
    return(0);
  }

  num_bridge_sites = bridge_sites.make_id_array(&bridge_site_array);
  if (num_bridge_sites < 0) {
    proc_log->write_time("Error: failure in call to make_id_array.\n");
    proc_log->write_time("Error: %s\n", bridge_sites.error());
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }

  for (i=0; i<num_output_sites; i++)
    {
      bridge[i] = 0;
      
      for (j=0; j<num_bridge_sites; j++)
	if (bridge_site_array[j] == output_site_array[i])
	  bridge[i] = 1;
    }

  delete [] bridge_site_array;

  return(num_bridge_sites);
}
