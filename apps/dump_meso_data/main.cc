// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:21:20 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <netcdf.hh>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <map>
#include <sys/types.h>
#include <ncf/ncf.hh>
#include <ncf/gtime.h>
#include <log/log.hh>
#include "data.h"

Log *logfile;

// External functions

extern int get_site_var_list(vector<ls *> &site_var_list, char *fname);
extern int get_site_ids(vector<site *> &sites, char *fname);
extern int get_meso_data(int num_files, char *inputFileName, vector<char *> &variables, os *obs_data);
extern int dump_meso_data(int num_files, vector<char *> &variables, vector<site *> &sites, vector <ls *> &site_var_list, os *obs_data, char *outputFileName);


void usage(char *prog_name)
{
  cerr << '\n';
  cerr << "usage: " << prog_name << " meso_input_file site_var_file site_list output_file [-l logfile_base] [-d debug_level]" << endl;
  cerr << "meso_input_file: file path of input mesonet nectdf file that contains the obs\n";
  cerr << "site_var_file: a file containing a list of dicast ids and the obs sensor variable name to use\n";
  cerr << "site_list: a dicast site list containing the sites to dump obs for\n";
  cerr << "output_file: the file path where the output will be written\n";
  cerr << "The following args are optional:\n";
  cerr << "\t[-m site_map_file]: a file containg a list of dicast ids and a mapped icao-id (from another site), obs will be output for the mapped icao-id\n";
  cerr << "\t[-d debug_level]\n";
  cerr << "\t[-l log_file]\n";
  exit(2);
}


int main(int argc, char **argv){
  
  int ret;
  int ind;
  int flag;
  int f, i, j, k, l, v;
  time_t start, end;
  double dif;
  int num_days;
  int num_files;
  int obs_num_files;
  int meso_num_files;
  int days;
  int fc_times_per_day;
  
  vector <site *> site_list_sites;
  vector <site *> sites;
  vector <char *> variables;
  vector <ls *> site_var_list;
  vector <ls *> site_map_list;
    
  NcError nc_err(NcError::silent_nonfatal);
  
  if (argc < 5)
    usage(argv[0]);
  
  char *meso_input_file = argv[1];
  char *site_var_file = argv[2];
  char *site_list = argv[3];
  char *output_file = argv[4];
  char *logfileBase = 0;
  int debug_level = -1;
  char *site_map_file = (char *)"";
  
  ind = 4;
  while (ind < argc)
    {
      if (strcmp(argv[ind], "-m") == 0)
        {
          ind++;
          site_map_file = argv[ind];
        }
      else if (strcmp(argv[ind], "-l") == 0)
        {
          ind++;
          logfileBase = argv[ind];
        }
      else if (strcmp(argv[ind], "-d") == 0)
        {
          ind++;
          debug_level = strtol(argv[ind], (char **)0, 10);
        }
 
      ind++;
    }
  
  ////////////////////////////////////////////////////////////////////
  // Set up logfile
  if (logfileBase)
    logfile = new Log(logfileBase);
  else
    logfile = new Log("");

  if (logfile->error())
    {
      cerr << "Error: Unable to open logfile\n";
      cerr << "Ending. Exit status = 1\n";
      exit (1);
    }
  
  
  logfile->set_debug(debug_level);
  logfile->write_time_starting( argv[0] );
  
  
  ////////////////////////////////////////////////////////////////////
  // Get site variable list from the site_var_file
  if (get_site_var_list(site_var_list, site_var_file))
    {
      logfile->write_time("Error: Unable to open/read variable list file %s\n",site_var_file);
      exit (1);
    }
  
  // Fill vars vector will all variables listed in the site_var_list
  for(i = 0; i < site_var_list.size(); i++){
    for(j = 0; j < site_var_list[i]->variables.size(); j++){
      flag = 0;
      for(k = 0; k < variables.size(); k++){
        if(strcmp(variables[k], site_var_list[i]->variables[j]) == 0){
          flag = 1;
        }
      }
      if(flag == 0){
        variables.push_back(site_var_list[i]->variables[j]);
      }
    }
  }
  
  
  ////////////////////////////////////////////////////////////////////
  // Get site_list_dicast_ids and site_list_icao_ids, from site list
  ret = get_site_ids(site_list_sites, site_list);
  if(ret){
    logfile->write_time("Error in get_site_ids.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  // Get dicast_ids and icoa_ids by cross checking site_var_file with site_list
  // Set "roadTemp" variable type: 0 for a road sensor 1 for a bridge sensor
  int rt_type;
  int found;
  for(i = 0; i < site_var_list.size(); i++){
    found = 0;
    site *site_data = new site;
    for(j = 0; j < site_list_sites.size(); j++){
      if(site_var_list[i]->site_id == site_list_sites[j]->dicast_id){
	found = 1;
	site_data->dicast_id = site_list_sites[j]->dicast_id;
	site_data->icao_id = site_list_sites[j]->icao_id;
	site_data->lat = site_list_sites[j]->lat;
	site_data->lon = site_list_sites[j]->lon;
	
	rt_type = 0;
	for(k = 0; k < site_var_list[i]->variables.size(); k++){
	  if(strncmp(site_var_list[i]->variables[k], "roadTemperature", 15) == 0){
	    for(l = 0; l < variables.size(); l++){
	      if(strcmp(variables[l], site_var_list[i]->variables[k]) == 0){
		site_data->roadTemp_type[l] = rt_type;
		//printf("site=%d, var=%s, site_data->roadTemp_type[%d]=%d\n", site_data->dicast_id, variables[l], l, site_data->roadTemp_type[l]);
	      }
	    } // end loop over variables
	    rt_type++;
	  }
	} // end loop over site_var->variables for each site
	
	sites.push_back(site_data);
      }
    } // end loop over site_list
    
    if(!found){
      logfile->write_time("Warning: site: %d is in the site_var_file but does not exist in the site list.\n", site_var_list[i]->site_id);
      site_data->dicast_id = MISSING;
      site_data->icao_id = (char *)"-9999";
      site_data->lat = MISSING;
      site_data->lon = MISSING;
      site_data->roadTemp_type[0] = MISSING;
      site_data->roadTemp_type[1] = MISSING;	
      sites.push_back(site_data);
    }
    
  } // end loop over site_var_list
  
  
  ////////////////////////////////////////////////////////////////////
  // Get site map list from the site_map_file (if the site_map_file was passed in)
  if(strlen(site_map_file) > 0){
    if (get_site_var_list(site_map_list, site_map_file))
      {
	logfile->write_time("Error: Unable to open/read variable list file %s\n",site_map_file);
	exit (1);
      }
    
    // For sites that are listed in the site_map_file replace the original icao-id with the mapped icao-id
    for(i = 0; i < sites.size(); i++){  
      for(k = 0; k < site_map_list.size(); k++){
	//printf("site_map_list[%d]->site_id = %d, site_map_list[%d]->variables[0] = %s\n", k, site_map_list[k]->site_id, k, site_map_list[k]->variables[0]);
	if(sites[i]->dicast_id == site_map_list[k]->site_id){
	  logfile->write_time("Info: for site %d;%s using obs from site %s\n",  sites[i]->dicast_id, sites[i]->icao_id, site_map_list[k]->variables[0]);
	  sites[i]->icao_id = site_map_list[k]->variables[0];
	}    
      } // end loop over site_map_list
    } // end loop over sites
  }

  
  ////////////////////////////////////////////////////////////////////
  // Get mesonet data
  
  logfile->write(1, "\n");
  logfile->write_time(1, "Getting mesonet data\n");
  
  num_files = 1;
  
  os *obs_data = new os[variables.size()*num_files];
  
  ret = get_meso_data(num_files, meso_input_file, variables, obs_data);
  if(ret){
    logfile->write_time("Error in get_meso_data.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  
  ////////////////////////////////////////////////////////////////////
  //Dump meso data
  logfile->write_time(1, "Dumping meso data.\n");
  ret = dump_meso_data(num_files, variables, sites, site_var_list, obs_data, output_file);
  if(ret){
    logfile->write_time("Error in dump_rctm_data.\n");
    logfile->write_time("Ending. Exit status = 1.\n");
    exit (1);
  }
  
  // Delete var_ptrs to free up memory
  ind = 0;
  for(v = 0; v < variables.size(); v++){
    for(f = 0; f < num_files; f++){
      if(obs_data[ind].var_is_valid == 0){
	delete [] obs_data[ind].var_ptrs;
      }
      ind++;
    }
  }
  
  logfile->write_time("Ending. Exit status = 0.\n");
  //return(0);
}
