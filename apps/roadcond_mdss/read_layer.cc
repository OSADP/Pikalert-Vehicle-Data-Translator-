// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: read_layer.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 01/29/07
 *
 * Description: 
 *              This file contains one routine that reads in the subsurface 
 *              characteristics at each site and creates a layer structure 
 *              that describes that info. 
 *
 *       read_layer_info()...Main routine described above.
 *              
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>
#include <list>

#include <site_list/site_list.hh>
#include <log/log.hh>
#include "layer_info.hh"
#include "SiteLayerTraffic.hh"

extern Log *proc_log;


//  
//  Routine: read_layer_info()
//  
//  Description:
//     This routine reads in the subsurface characteristics at each site
//  from a list of SiteLayerTraffic objects including: material type
//  for each layer and layer thicknesses.
//  Note that the deepest layer is listed first. 
//  
//  Inputs: 
//    layer_traffic_list....list of SiteLayerTraffic objects from which layer info 
//                          is extracted.
//    output_site_array..An array containing the id's of the output forecast site list.
//    num_output_sites...The number of sites in the output forecast site list.
//  
//  Outputs:
//    site_layer.........A structure containing descriptions of the layer structure
//                       and materials for all sites.
//  
//  Returns:
//    0..................If no fatal errors were found.
//    1..................If a fatal processing condition was encountered.
//


int read_layer_info(list <SiteLayerTraffic*> &layer_traffic_list, 
		    int *output_site_array,int num_output_sites, LayerInfo *site_layer)
{
  int i, ns;

  //
  // Extract layer info for each site in which we are interested.
  // 
  for (ns = 0; ns < num_output_sites; ns++)
    {
      //
      // Make sure the site is in the SiteLayerTraffic list
      // and therefore has subsurface properties defined.
      //
      list< SiteLayerTraffic* > :: const_iterator site;

      site = layer_traffic_list.begin();

      while ( site != layer_traffic_list.end() &&  
	      (*site)->getSiteId() != output_site_array[ns] )
	{
	  site++;
	}

      if (site == layer_traffic_list.end() )
	{
	  //
	  // We didnt find it and we must have it!
	  //
	  proc_log->write_time("Error: No layer info specified for site %d\n", ns);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      //
      // Initialize all layer thicknesses to zero
      //
      for (i=0; i<MAX_LAYER; i++)
	  site_layer[ns].layer_thickness[i] = 0.;

      //
      // Set time zone for this site
      //
      site_layer[ns].tz_code = (*site)->getTzCode();

      //
      // Set number of layers at this site
      //
      site_layer[ns].num_layer = (*site)->getNumLayers();

      //
      // Specify data at each layer(layer material and thickness).
      //
      for (i=0; i<site_layer[ns].num_layer; i++)
	{
	  site_layer[ns].layer_mat[i] = (*site)->getLayerMat(i);
	
	  site_layer[ns].layer_thickness[i] = (*site)->getLayerThickness(i);
	}
    }


  return(0);
}

