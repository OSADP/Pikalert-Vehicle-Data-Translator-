// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <cstring>
#include <string>
#include <log/log.hh>
#include "parse_traffic_layer_config.hh"

extern Log *proc_log;

//
//  Routine: parse_traffic_layer_config()
//  
//  Description: See parse_traffic_layer_config.hh for more info.
//
// 
int parse_traffic_layer_config( char *traffic_layer_filename, int *site_list,
				int num_sites, char *mat_default,
				list <SiteLayerTraffic*> &layer_traffic_list)  
{

  FILE *fptr;
  
  if ( (fptr = fopen(traffic_layer_filename,"r")) == NULL )
    {
      proc_log->write_time("Error: Unable to open file %s\n", traffic_layer_filename);
      int exit_status = 1;
      return(exit_status);
    }
   
  SiteLayerTraffic *site_layer_traffic = NULL;
  SiteLayerTraffic *default_site_layer_traffic = NULL;

  char line[MAX_LINE_LEN];

  while(!feof(fptr))
    {
      //
      // Get line form config file and discard if it starts
      // with white space or #.
      //
      fgets(line, MAX_LINE_LEN, fptr);
      if(line[0] == '#' || isspace(line [0]))
	continue;
      
      // replace newline with \0
      int len = strlen(line);
      if (len > 0)
	line[len-1] = '\0';
      
      char keyword[MAX_WORD_LEN];

      int num_times;

      //
      // Get keyword 
      //
      sscanf(line, "%s", keyword);  
      
      //
      // Get the number of times per day that a traffic volume 
      // indicator is recorded
      //
      if (strncmp (keyword,"NUM_TIMES", 9) == 0)
	{
	  sscanf(line, "%*10c %d", &num_times);	  
	}

      //
      // SITE_ID signifies the start of a new site block, save the
      // old one (if any) and start a new one
      //
      else if (strncmp (keyword,"SITE_ID", 7) == 0 )
	{

	  if (site_layer_traffic)
	    {
	      if (site_layer_traffic->isOK())
                {
		  // if this is a normal site entry, save in the list; if this
		  // is a default material, save as the default; else delete
		  if (site_layer_traffic->getSiteId() >= 0)
		    {
		      layer_traffic_list.push_back(site_layer_traffic);
		    }
		  else if (site_layer_traffic->getSiteId() == -9)
		    {
		      default_site_layer_traffic = site_layer_traffic;
		    }
		  else
		    {
		      delete site_layer_traffic;
		    }
                }
	      else
		{
		  proc_log->write_time("Error: Information for site %d is incomplete in site config file\n", site_layer_traffic->getSiteId());
		  return -1;
		}
	    }

	  //
	  // Create new SiteLayerTraffic object
	  //
	  site_layer_traffic = new SiteLayerTraffic();
	  
	  //
	  // SITE_ID: Look for 8-digit number or a default material name, which
	  // will be set to ID -9. Anything else is ignored and set to -9999.
	  //
	  long id;
	  int ret;
	  ret = sscanf(line, "%*8c %ld", &id);
	  if (ret == 1)
	    {
	      site_layer_traffic->setSiteId(id);
	    }
	  else if (mat_default && strcmp(mat_default, &line[9]) == 0)
	    {
	      site_layer_traffic->setSiteId(-9);
	    }
	  else
	    {
	      site_layer_traffic->setSiteId(-9999);
	    }

	  //
	  // record number of times per day that traffic volume is recorded
	  //
	  site_layer_traffic->setNumTrafficTimes(num_times);
 
	}
      
      else if (strncmp (keyword,"TZ_CODE", 7) == 0 )
	{
	  //
          // record the time zone code for this site
          //
          char tz_code[30];

          sscanf(line, "%*8c %s", tz_code);

          site_layer_traffic->setTzCode(tz_code);

        }

      else if (strncmp (keyword,"NUM_LAYERS", 10) == 0 )
	{
	  //
	  // record number of subsurface layers at this site
	  //
	  int num_layers;

	  sscanf(line, "%*11c %d", &num_layers);

	  site_layer_traffic->setNumLayers(num_layers);
  
	}

      else if (strncmp (keyword,"LAYER_MAT", 9) == 0 )
	{
	  //
	  // Get number of layers
	  //
	  int num_layers = site_layer_traffic->getNumLayers();
	  
	  //
	  // Set ptr past the keyword
	  //
	  char *substr = strchr(line,':');
	  
	  //
	  // We will break the remaining part of line
	  // into tokens separated by white space or a colon.
	  // Each token will be coverted to an int and 
	  // the layer material is set in the SiteLayerTraffic object.
	  //
	  substr = strtok(substr , " \f\n\r\t\v:");
	  
	  int material_code = atoi(substr);

	  site_layer_traffic->setLayerMat(0, material_code);

	  for (int i = 1; i < num_layers; i++)
	    {
	      substr = strtok( NULL, " \f\n\r\t\v:");

              material_code = atoi(substr);

	      site_layer_traffic->setLayerMat(i, material_code);
	    }
	} 

      else if (strncmp (keyword,"LAYER_THICKNESS", 15) == 0 )
        {
          //
          // Get number of layers
          //
          int num_layers = site_layer_traffic->getNumLayers();
 
          //
          // Set ptr past the keyword
          //
          char *substr = strchr(line,':');
 
          //
          // We will break the remaining part of line
          // into tokens separated by white space or a colon.
          // Each token will be coverted to an int and
          // the layer material is set in the SiteLayerTraffic object.
          //
          substr = strtok(substr , " \f\n\r\t\v:");
 
          float layer_thickness = atof(substr);
 
          site_layer_traffic->setLayerThickness(0, layer_thickness);

	  for (int i = 1; i < num_layers; i++)
	    {
	      substr = strtok( NULL, " \f\n\r\t\v:");

              layer_thickness = atof(substr);

	      site_layer_traffic->setLayerThickness(i, layer_thickness);
	    }
        } 

       else if (strncmp (keyword,"TRAFFIC", 7) == 0 )
        {
          //
          // Get number of layers
          //
          int num_times = site_layer_traffic->getNumTrafficTimes();
 
          //
          // Set ptr past the keyword
          //
          char *substr = strchr(line,':');
 
          //
          // We will break the remaining part of line
          // into tokens separated by white space or a colon.
          // Each token will be coverted to an int and
          // the layer material is set in the SiteLayerTraffic object.
          //
          substr = strtok(substr , " \f\n\r\t\v:");
 
          int traffic_vol = atoi(substr);
 
          site_layer_traffic->setTrafficVol(0, traffic_vol);
 
          for (int i = 1; i < num_times; i++)
            {
              substr = strtok( NULL, " \f\n\r\t\v:");
 
              traffic_vol = atoi(substr);
 
              site_layer_traffic->setTrafficVol(i, traffic_vol);
            }
        }

       else if (strncmp (keyword,"ROUTE_LENGTH", 12) == 0 )
	{
	  //
	  // record number of subsurface layers at this site
	  //
	  float route_length;

	  sscanf(line, "%*s %f", &route_length);

	  site_layer_traffic->setRouteLen(route_length);  
	}

       else if (strncmp (keyword,"ROUTE_LANES", 11) == 0 )
	{
	  //
	  // record number of lanes in this route
	  //
	  int route_lanes;

	  sscanf(line, "%*s %d", &route_lanes);

	  site_layer_traffic->setRouteLanes(route_lanes);
	}
       
       else if (strncmp (keyword,"ROUTE_TREATMENT_TIME", 20) == 0 )
	{
	  //
	  // record how long it takes to treat this route
	  //
	  int treat_time;

	  sscanf(line, "%*s %d", &treat_time);

	  site_layer_traffic->setRouteTreatmentTime(treat_time);
	}

       else if (strncmp (keyword, "TREATMENT_STRATEGY", strlen("TREATMENT_STRATEGY")) == 0 )
	{
	  //
	  // treatment strategy
	  //
	  int strategy;

	  sscanf(line, "%*s %d", &strategy);

	  site_layer_traffic->setTreatmentStrategy(strategy);
	}

       else if (strncmp (keyword, "SNOW_PLOW_THRESHOLD", strlen("SNOW_PLOW_THRESHOLD")) == 0 )
	{
	  //
	  // Snow amount plowing threshold
	  //
	  float threshold;

	  sscanf(line, "%*s %f", &threshold);

	  site_layer_traffic->setSnowPlowThreshold(threshold);
	}

       else if (strncmp (keyword, "CHEM_TYPE", strlen("CHEM_TYPE")) == 0 )
	{
	  //
	  // Default chemical type
	  //
	  int type;

	  sscanf(line, "%*s %d", &type);

	  site_layer_traffic->setChemType(type);
	}

       else if (strncmp (keyword, "CHEM_FORM", strlen("CHEM_FORM")) == 0 )
	{
	  //
	  // Default chemical form
	  //
	  int form;

	  sscanf(line, "%*s %d", &form);

	  site_layer_traffic->setChemForm(form);
	}

       else if (strncmp (keyword, "CHEM_MIN_APPLICATION_RATE", strlen("CHEM_MIN_APPLICATION_RATE")) == 0 )
	{
	  //
	  // Default chemical minimum application rate
	  //
	  float rate ;

	  sscanf(line, "%*s %f", &rate);

	  site_layer_traffic->setChemMinRate(rate);
	}

       else if (strncmp (keyword, "CHEM_MAX_APPLICATION_RATE", strlen("CHEM_MAX_APPLICATION_RATE")) == 0 )
	{
	  //
	  // Default chemical maximum application rate
	  //
	  float rate;

	  sscanf(line, "%*s %f", &rate);

	  site_layer_traffic->setChemMaxRate(rate);
	}

       else if (strncmp (keyword, "CHEM_RATE_INCREMENT", strlen("CHEM_RATE_INCREMENT")) == 0 )
	{
	  //
	  // Default chemical rate increment
	  //
	  float increment ;

	  sscanf(line, "%*s %f", &increment);

	  site_layer_traffic->setChemRateInc(increment);
	}

       else if (strncmp (keyword, "PRETREAT_TYPE", strlen("PRETREAT_TYPE")) == 0 )
	{
	  //
	  // Pretreatment chemical type
	  //
	  int type;

	  sscanf(line, "%*s %d", &type);

	  site_layer_traffic->setPretreatType(type);
	}

       else if (strncmp (keyword, "PRETREAT_FORM", strlen("PRETREAT_FORM")) == 0 )
	{
	  //
	  // Pretreatment chemical form
	  //
	  int form;

	  sscanf(line, "%*s %d", &form);

	  site_layer_traffic->setPretreatForm(form);
	}

       else if (strncmp (keyword, "PRETREAT_MIN_APPLICATION_RATE", strlen("PRETREAT_MIN_APPLICATION_RATE")) == 0 )
	{
	  //
	  // Default pretreatment minimum application rate
	  //
	  float rate ;

	  sscanf(line, "%*s %f", &rate);

	  site_layer_traffic->setPretreatMinRate(rate);
	}

       else if (strncmp (keyword, "PRETREAT_MAX_APPLICATION_RATE", strlen("PRETREAT_MAX_APPLICATION_RATE")) == 0 )
	{
	  //
	  // Default pretreatment maximum application rate
	  //
	  float rate;

	  sscanf(line, "%*s %f", &rate);

	  site_layer_traffic->setPretreatMaxRate(rate);
	}

       else if (strncmp (keyword, "PRETREAT_RATE_INCREMENT", strlen("PRETREAT_RATE_INCREMENT")) == 0 )
	{
	  //
	  // Default pretreatment rate increment
	  //
	  float increment ;

	  sscanf(line, "%*s %f", &increment);

	  site_layer_traffic->setPretreatRateInc(increment);
	}

       else
	{
	  proc_log->write_time("Error: Unknown keyword (%s) in site config file\n", keyword);
	  return -1;
	}

       //
       // Clear out old keyword.
       //
       keyword[0] = '\0';
    }

  //
  // Save last site info
  //
  if (site_layer_traffic->isOK())
    {

      // if this is a normal site entry, save in the list; if this is a default
      // configuration, save as the default; else, delete it

      if (site_layer_traffic->getSiteId() >= 0)
	{
	  layer_traffic_list.push_back(site_layer_traffic);
	}
      else if (site_layer_traffic->getSiteId() == -9)
	{
	  default_site_layer_traffic = site_layer_traffic;
	}
      else
	{
	  delete site_layer_traffic;
	}
    }
  else
    {
      proc_log->write_time("Error: Information for site %d is incomplete in site config file\n", site_layer_traffic->getSiteId());
      return -1;
    }

  //
  // Check layer_traffic_list against site_list and add default
  // entries for unknown sites, if requested
  //
  list< SiteLayerTraffic* > :: const_iterator site_config;

  for (int ns=0; ns<num_sites; ns++)
    {
      site_config = layer_traffic_list.begin();
      while ( site_config != layer_traffic_list.end() &&
	      (*site_config)->getSiteId() != site_list[ns] )
	{
	  site_config++;
	}

      // We found the site, go to next one
      if (site_config != layer_traffic_list.end())
	continue;

      // Site unknown, if a material default was specified, use it
      if (mat_default && default_site_layer_traffic)
	{
	  site_layer_traffic = new SiteLayerTraffic(*default_site_layer_traffic);

	  site_layer_traffic->setSiteId(site_list[ns]); 
	  
	  layer_traffic_list.push_back(site_layer_traffic);

	  proc_log->write_time(1, "Info: Using material default configuration '%s' for site %d\n", mat_default, site_list[ns]);
	}
      else if (mat_default != 0)
	{
	  proc_log->write_time("Error: Cound not find material default configuration '%s' in site_config_file\n", mat_default);
	  return -1;
	}
      else
	{
	  proc_log->write_time("Error: %d does not exist in site_config_file, and no material default was specified\n", site_list[ns]);
	  return -1;
	}
    }

  //
  // sort list according to site id and put in ascending order
  //
  layer_traffic_list.sort(site_comp);

  //
  // print list for debugging ingest
  //
  print_list(layer_traffic_list);

  fclose( fptr);

  if (default_site_layer_traffic)
    delete default_site_layer_traffic;

  return 0;
}


/////////////////////////////////////////////////////////////////////////////////
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
//  Outputs:
//    true............................If the siteId contained in the SiteLayerTraffic object pointed 
//                                    to by a_ptr precedes the siteId contained in the SiteLayerTraffic 
//                                    object pointed to by b_ptr.
//    
//    false...........................If the siteId contained in the SiteLayerTraffic object pointed 
//                                    to by a_ptr does not precede the siteId contained in the 
//                                    SiteLayerTraffic object pointed to by b_ptr.
//
bool site_comp(SiteLayerTraffic *a_ptr, SiteLayerTraffic *b_ptr )
{

   long Id_a = a_ptr->getSiteId();
   long Id_b = b_ptr->getSiteId();

   return(Id_a < Id_b );
}


/////////////////////////////////////////////////////////////////////////////////
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
void print_list( list <SiteLayerTraffic*> &traffic_layer_list)
{

  list< SiteLayerTraffic* > :: const_iterator site;

  for( site = traffic_layer_list.begin(); site != traffic_layer_list.end(); site++)
    {
      //
      // print time zone code
      //
      proc_log->write(9, "\n--------------------------------------------------------\n");
      //
      // print site_id
      //
      proc_log->write(9, "SITE_ID: %d\n", (*site)->getSiteId());

      //
      // print time zone code
      //
      proc_log->write(9, "TZ_CODE: %s\n", ((*site)->getTzCode()).c_str());

      //
      // print number of subsurface layers at site
      //
      int num_layers =  (*site)->getNumLayers();
      
      proc_log->write(9, "NUM_LAYERS: %d\n", num_layers );	


      //
      // print the material code for the layers
      //
      for (int i = 0; i < num_layers; i++)
	{
	  if (i == 0)
	    proc_log->write(9, "LAYER_MAT: ");
	  
	  proc_log->write(9, "%d ", (*site)->getLayerMat(i));
	  
	  if ( i == num_layers - 1)
	    proc_log->write(9, "\n");
	}

      //
      //  print number of times per that traffic volume will be recorded
      //
      int num_times = (*site)->getNumTrafficTimes();
      
      proc_log->write(9, "NUM_TIMES: %d\n", num_times);

      //
      // print the traffic vol indicators  
      //
      for (int i = 0; i < num_times; i++)
	{
	  if (i == 0)
                proc_log->write(9, "TRAFFIC: ");
	  
	  proc_log->write(9, "%d ", (*site)->getTrafficVol(i));
	  
	  if ( i == num_times - 1)
	    proc_log->write(9, "\n");
	}
      
      proc_log->write(9, "ROUTE_LEN: %f\n", (*site)->getRouteLen() );

      proc_log->write(9, "ROUTE_LANES: %d\n", (*site)->getRouteLanes() );

      proc_log->write(9, "ROUTE_TREATMENT_TIME: %d\n", (*site)->getRouteTreatmentTime() );

      proc_log->write(9, "TREATMENT_STRATEGY: %d\n", (*site)->getTreatmentStrategy() );

      proc_log->write(9, "SNOW_PLOW_THRESHOLD: %f\n", (*site)->getSnowPlowThreshold() );

      proc_log->write(9, "CHEM_TYPE: %d\n", (*site)->getChemType() );

      proc_log->write(9, "CHEM_MIN_APPLICATION_RATE: %f\n", (*site)->getChemMinRate() );

      proc_log->write(9, "CHEM_MAX_APPLICATION_RATE: %f\n", (*site)->getChemMaxRate() );

      proc_log->write(9, "CHEM_RATE_INCREMENT: %f\n", (*site)->getChemRateInc() );

      proc_log->write(9, "PRETREAT_TYPE: %d\n", (*site)->getPretreatType() );

      proc_log->write(9, "PRETREAT_MIN_APPLICATION_RATE: %f\n", (*site)->getPretreatMinRate() );

      proc_log->write(9, "PRETREAT_MAX_APPLICATION_RATE: %f\n", (*site)->getPretreatMaxRate() );

      proc_log->write(9, "PRETREAT_RATE_INCREMENT: %f\n", (*site)->getPretreatRateInc() );

    }
}

