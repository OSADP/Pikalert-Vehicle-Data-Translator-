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
 *   Module: parse_traffic_layer_config.hh
 *
 *   Author: Sue Dettling
 *
 *   Date:   12/9/02
 *
 *   Description: This module contains functions used to parse the
 *                site layer and traffic config file.  
 *                
 *   Changes: Julie Prestopnik 02/20/07 To add new variable tzCode
 *            Jim Cowie 8/8/05 To add new variables related to default
 *            chemical types and application rates.
 *
 */

#ifndef PARSE_TRAFFIC_LAYER_CONFIG_HH
#define PARSE_TRAFFIC_LAYER_CONFIG_HH

#include <list>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include "SiteLayerTraffic.hh"

#define MAX_LINE_LEN 300

#define MAX_WORD_LEN 50

//
//  Routine: parse_traffic_layer_config()
//  
//  Description:
//     This routine parses a site layer and traffic
//     config file.  Comment lines start with # or white space.
//     The first non comment line must contain the keyword NUM_TIMES,
//     followed by an int which represents the number of times in a
//     twenty four hour period that a traffic volume indicator will be recorded:
//     Then for each site it expects keywords and data in the following order:
//     SITE_ID: (long)
//     TZ_CODE: (string)
//     NUM_LAYERS: (int)
//     LAYER_MAT: (list of ints -- one for each layer)
//     NUM_NODES_PER_LAYER: (list of ints -- one for each layer. ) 
//     NODE_THICKNESS: (list of floats: one float for each node of each layer. 
//                      Order is deepest node of deepest layer -> top node of top layer.   
//     TRAFFIC: (int) 
//     ROUTE_LENGTH: (float)
//     ROUTE_LANES: (int)
//     ROUTE_TREATMENT_TIME: (int -- in minutes)
//     TREATMENT_STRATEGY: (int)
//     SNOW_PLOW_THRESHOLD (float)
//     CHEM_TYPE (int)
//     CHEM_MIN_APPLICATION_RATE (float)
//     CHEM_MAX_APPLICATION_RATE (float)
//     CHEM_RATE_INCREMENT (float)
//     PRETREAT_TYPE (int)
//     PRETREAT_MIN_APPLICATION_RATE (float)
//     PRETREAT_MAX_APPLICATION_RATE (float)
//     PRETREAT_RATE_INCREMENT (float)
//
//     Here is an example file with one site entry:
//     
//     #
//     # Number of times a day we will record traffic volume
//     #
//     NUM_TIMES 24
//
//     SITE_ID: 74449036
//     TZ_CODE: MST7MDT
//     NUM_LAYERS: 6
//     LAYER_MAT: 90 2 92 94 91 1
//     NUM_NODES_PER_LAYER: 3 3 2 5 0
//     NODE_THICKNESS: 3 2 2 2 1 0.5 0.5 0.25 0.25 0.2 0.2 0.12 0.04
//     TRAFFIC: 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 2 2 2 2 2
//     ROUTE_LENGTH: 9.77
//     ROUTE_LANES: 4
//     ROUTE_TREATMENT_TIME: 45
//     TREATMENT_STRATEGY: 0
//     SNOW_PLOW_THRESHOLD: 2.5
//     CHEM_TYPE 1
//     CHEM_MIN_APPLICATION_RATE 50
//     CHEM_MAX_APPLICATION_RATE 400
//     CHEM_RATE_INCREMENT 50
//     PRETREAT_TYPE 5
//     PRETREAT_MIN_APPLICATION_RATE 50
//     PRETREAT_MAX_APPLICATION_RATE 200
//     PRETREAT_RATE_INCREMENT 25
//   
//     For each site in the config file, a SiteLayerTraffic object is created to 
//     store all of the site information. Once we create this object, we store
//     it in a list which is passed to the function as an input argument
//   
//  Inputs: 
//    traffic_layer_filename..........Name of configuration file with site layer and 
//                                    traffic information.
//
//    site_list.......................Array containing site IDs
//
//    num_sites.......................Number of sites in site_list
//
//    mat_default.....................Default type of material 
//
//    layer_traffic_list..............A reference to a list of SiteLayerTraffic
//                                    objects. A SiteLayerTraffic object is created to 
//                                    store the information of each site and this object
//                                    is then stored in this list. 
//
//  Outputs: Entries are added to layer_traffic_list.
//
//  Returns:  0.......................If no fatal errors were found.
//
//            1.......................If a fatal processing condition was encountered.    
// 
int parse_traffic_layer_config( char *traffic_layer_filename, int *site_list,
				int num_sites, char *mat_default,
				list <SiteLayerTraffic*> &layer_traffic_list);

//
//  Routine: site_comp()
// 
//  Description: 
//    site_comp is a comparison function used to put SiteLayerTraffic
//    objects in order according to site id.
//
//  Inputs:
//    a_ptr...........................Pointer to a SiteLayerTraffic object.
//    
//    b_ptr...........................Pointer to a SiteLayerTraffic object.
//
//  Outputs: none
//  
//  Returns:
//    true............................If the siteId contained in the SiteLayerTraffic object pointed 
//                                    to by a_ptr precedes the siteId contained in the SiteLayerTraffic 
//                                    object pointed to by b_ptr.
//    
//    false...........................If the siteId contained in the SiteLayerTraffic object pointed 
//                                    to by a_ptr does not precede the siteId contained in the 
//                                    SiteLayerTraffic object pointed to by b_ptr.
//
bool site_comp(SiteLayerTraffic *a_ptr, SiteLayerTraffic *b_ptr );

//
// Routine: print_list()
// 
// Description:
//   Debug tool used to print contents of a list of SiteLayerTraffic pointers.
//
// Inputs: traffic_layer_list ........Reference to a list of pointers to SiteLayerTraffic objects.
// 
// Outputs: Log object handles writing messages to log file, stdout, or stderr.
//
// Returns: none
// 
void print_list( list <SiteLayerTraffic*> &traffic_layer_list);


#endif /* PARSE_TRAFFIC_LAYER_CONFIG_HH */









