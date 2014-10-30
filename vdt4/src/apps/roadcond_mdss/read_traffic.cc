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
 * Module: read_traffic.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: This file contains one routine which extracts
 *              forecast route segment information from SiteLayerList objects
 *              including: traffic levels at all times of the day, 
 *              the number of lanes, and the segment length. 
 *              These are stored in arrays.
 *     
 *       read_traffic()...Main routine described above.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>

#include <log/log.hh>
#include <list>
#include "SiteLayerTraffic.hh"

extern Log *proc_log;

//  
//  Routine: read_traffic()
//  
//  Description:
//      This module reads in descriptions of the route segments from a
//  SiteLayerTraffic object. It fills arrays with the traffic levels 
//  at all times of the day, the number of lanes, and the segment length
//  at each site. 
//  
//  Inputs: 
//    layer_traffic_list..list of SiteLayerTraffic objects from which traffic info
//                        is extracted.
//    output_site_array...An array containing the id's of the output forecast site list.
//    num_output_sites....The number of sites in the output forecast site list.
//  
//  Outputs:
//    traffic_level..........An array containing traffic levels at each hour of the day
//                           at every site.
//    route_length...........An array containing the length of the service route 
//                           at every site.
//    route_lanes............An array containing number of lanes on the road segments.
//    route_treatment_time...An array containing the nominal treatment for the road segments.
//  
//  Returns:
//    0..................If no fatal errors were found.
//    1..................If a fatal processing condition was encountered.
//


int read_traffic(list <SiteLayerTraffic*> &layer_traffic_list, 
		 int *output_site_array, long num_output_sites,
		 int **traffic_level, float **route_length, 
		 int **route_lanes, int **route_treatment_time)
{ 
  //
  // Get number of times traffic vol indicator is recorded
  // from any of the list elements
  //
  list< SiteLayerTraffic* > :: const_iterator site;
  site = layer_traffic_list.begin();
  int num_times = (*site)->getNumTrafficTimes();

  //
  // Allocate memory for traffic information.
  //
  *route_length = new float[num_output_sites];
  *route_lanes = new int[num_output_sites];
  *route_treatment_time = new int[num_output_sites];
  *traffic_level = new int[num_output_sites*num_times];

  //
  // Extract the traffic info specific to each site
  // 
  for (int i = 0; i < num_output_sites; i++)
    {
       list< SiteLayerTraffic* > :: const_iterator site;

       site = layer_traffic_list.begin();

       while ( site != layer_traffic_list.end() &&
	       (*site)->getSiteId() != output_site_array[i] )
	 {
	   site++;
	 }
       
       if (site == layer_traffic_list.end() )
	 {
	   //
	   // We didnt find it and we must have it!
	   //
	   proc_log->write_time("Error: No traffic info specified for site %d\n", i);
	   proc_log->write_time("Ending: exit status = 1.\n");
	   exit(1);
	 }

       (*route_length)[i] = (*site)->getRouteLen();
       (*route_lanes)[i] =  (*site)->getRouteLanes();
       (*route_treatment_time)[i] =  (*site)->getRouteTreatmentTime();
       for (int j = 0; j < num_times; j++)
	 (*traffic_level)[i*num_times+j] = (*site)->getTrafficVol(j);
    }

  return(0);
}
