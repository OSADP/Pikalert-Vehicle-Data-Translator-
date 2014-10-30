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
 * Module: calc_road_temp.cc
 *
 * Author: Bill Myers
 *
 * Date:   01/12/07
 *
 * Modified by: Julie Prestopnik
 *
 * Date: 01/29/07
 *
 * Description: 
 *              This file contains a routine that prepares the files for the external 
 *              calculation of road temperature and snow depth time series by
 *              METRo, a land-surface model developed at Environemnt Canada. METRo's 
 *              output, a text file, is parsed to obtain the desired time series.
 *
 *    calc_road_temp().....Main routine described above.
 *
 */

/* System include files / Local include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <new>
#include <vector>
#include <string>
#include <time.h>
#include <fstream>
#include <iostream>

#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include <unistd.h> // for usleep test RGH
#include "WxFcst.hh"
#include "Obs.hh"
#include "RoadCond.hh"
#include "SiteLayerTraffic.hh"
#include "layer_info.hh"
#include "metro_type.hh"

/* External global variables / Non-static global variables / Static globals */

/* External functions / Internal global functions / Internal static functions */

extern int metro_direct(int bridge, float lat, float lon, LayerInfo *site_layer,
			int current_hour, double obs_start_time, ObsTS *obs_ts,
			double forc_time, int nt_start, int num_fcst,
			WxFcstTS *wx_fcst_ts,  int chem_failure_time,
			int treatment_option, int useSolarFluxForecast,
			int useIrFluxForecast, RoadConditions *roadcond);

extern void write_METRo_obs_input_xml_file(char * file_name, double forc_time, double obs_start_time, 
				       int site_id, int bridge, ObsTS *obs_ts, WxFcstTS *wx_fcst_ts,
				       RoadConditions *roadcond, int nt_start, int current_hour);
extern void write_METRo_forecast_input_xml_file(char *forecast_file_name, double forc_time, 
						int chem_failure_time,
						int current_hour, int num_fcst, int nt_start,					    
						int site_id, WxFcstTS *wx_fcst_ts, int treatment_option);
extern int write_METRo_station_input_xml_file(char *station_file_name, float lat, float lon,  int site_id,
					      LayerInfo *site_layer, int bridge, string layer_mat_string[]);
extern void parse_METRo_xml_output(char *roadcast_output_filename, int current_hour, double forc_time,
				   int num_fcst, RoadConditions *roadcond);

extern int write_METRo_obs_input_csv_file(char * file_name, double forc_time, double obs_start_time, 
					  int site_id, int bridge, ObsTS *obs_ts, WxFcstTS *wx_fcst_ts,
					  RoadConditions *roadcond, int nt_start, int current_hour);
extern int write_METRo_forecast_input_csv_file(char *forecast_file_name, double forc_time, 
						int chem_failure_time,
						int current_hour, int num_fcst, int nt_start,
						int site_id, WxFcstTS *wx_fcst_ts, int treatment_option);
extern int write_METRo_station_input_csv_file(char *station_file_name, float lat, float lon,  int site_id,
					      LayerInfo *site_layer, int bridge, string layer_mat_string[]);
extern void parse_METRo_csv_output(char *roadcast_output_filename, int current_hour, double forc_time,
				   int num_fcst, RoadConditions *roadcond);

extern Log *proc_log;


//  
//  Routine: calc_road_temp()
//  
//  Description:
//      Prepares data structures required to create the input files for METRo Then runs 
//  METRo using a system call. METRo's text output is parsed and the internal arrays
//  are filled with the extracted data.
//  
//  Inputs: 
//    treatment_num......The index of the current treatment. This is used in filename 
//                       creation only to keep sequential iterations input files all available.
//    ns.................The site index. Again this is used in file naming only.
//    lat................The latitude of the forecast location.
//    lon................The longitude of the forecast location.
//    site_id............The site id for the forecast site.
//    forc_time..........The Unix time of the start of the forecast.
//    current_hour.......The current hour offset from the foercast start
//    chem_failure_time..Hour when checmicals fail
//    treatment_option...Treatment option type
//    metro_run_type.....Option for how to run METRo
//    use_flux...........Option to generate or use forecast fluxes
//    num_fcst...........The last time step to be forecast.
//    nt_start...........The offset into the weather time series of the first forecast in 
//                       the weather data time series.
//    bridge.............An integar indicating if this site is a bridge site or a road site.
//    obs_start_time.....First time of the obs
//    wx_fcst_ts.........An array containing the pertinent weather forecast variables for this site.
//    obs_ts.............An array containing the pertinent observation variables for this site.
//    site_layer.........A structure containing the subsurface characteristics of the site
//                       and its parameterization.
//  
//  Outputs:
//    roadcond...........An array containing the road condition forecasts. The road condition 
//                       structure contains the updated surface/subsurface temperature and the 
//                       snow depth forecasts.
//  
//  Returns:
//    0..................Success
//    1..................METRo failed for various possible reasons
//
//  

int calc_road_temp(int treatment_num, int ns, float lat, float lon, 
		   int site_id, double forc_time, int current_hour,
		   int chem_failure_time, int treatment_option,
		   int metro_run_type, char *use_flux, int num_fcst,
		   int nt_start, int bridge, double obs_start_time,
		   WxFcstTS *wx_fcst_ts, ObsTS *obs_ts,
		   LayerInfo *site_layer, RoadConditions *roadcond)
{
  char forecast_file_name[30], obs_file_name[30], station_file_name[30];
  char roadcast_output_filename[80];
  string command;
  string layer_mat_string[MAX_LAYER];
  int ret;
  
  // If the current_hour is the last fcst do not run METRo
  if(current_hour >= num_fcst-1){
    proc_log->write_time("Info: Current_hour %d is greater than or equal to the last fcst index %d, not running METRo.\n", current_hour, num_fcst-1);
    return(0);
  }

  //
  // Parse the use_flux option string and set flags
  //
  int useSolarFluxForecast = 0;
  int useIrFluxForecast = 0;
  if (use_flux != 0 && (strcmp(use_flux, "solar") == 0 ||
			strcmp(use_flux, "both") == 0))
    useSolarFluxForecast = 1;

  if (use_flux != 0 && (strcmp(use_flux, "ir") == 0 ||
			strcmp(use_flux, "both") == 0))
    useIrFluxForecast = 1;

  // The choices to run METRo are: 
  //
  //   direct: send data directly to METRo road-temp model, macadam
  //      xml: create xml format files for METRo I/O
  //      csv: create csv files for METRo I/O
  //
  if (metro_run_type == METRO_DIRECT)
    {
      ret = metro_direct(bridge, lat, lon, site_layer, current_hour,
			 obs_start_time, obs_ts, forc_time, nt_start,
			 num_fcst, wx_fcst_ts, chem_failure_time,
			 treatment_option, useSolarFluxForecast,
			 useIrFluxForecast, roadcond);
      return(ret);
    }
  else if (metro_run_type == METRO_XML) 
    {
      sprintf(forecast_file_name, "%s.%d.%d.xml", "met", ns, treatment_num);
      sprintf(obs_file_name, "%s.%d.%d.xml", "obs", ns, treatment_num);
      sprintf(station_file_name, "%s.%d.%d.xml", "station", ns, treatment_num);
      sprintf(roadcast_output_filename, "roadcast.%03d.%d.xml", ns, treatment_num);

      // Write input XML files for METRo
      write_METRo_obs_input_xml_file(obs_file_name, forc_time, obs_start_time, site_id, bridge, 
				     obs_ts, wx_fcst_ts, roadcond,
				     nt_start, current_hour);
      write_METRo_forecast_input_xml_file(forecast_file_name, forc_time, chem_failure_time, 
					  current_hour, num_fcst, nt_start, 
					  site_id, wx_fcst_ts, treatment_option);
      
      if (0 != write_METRo_station_input_xml_file(station_file_name, lat, lon, site_id, 
						  site_layer, bridge, layer_mat_string))
	{
	  int exit_status = 1;
	  return(exit_status);
	}
    }
  else if (metro_run_type == METRO_CSV) 
    {
      sprintf(forecast_file_name, "%s.%d.%d.csv", "met", ns, treatment_num);
      sprintf(obs_file_name, "%s.%d.%d.csv", "obs", ns, treatment_num);
      sprintf(station_file_name, "%s.%d.%d.csv", "station", ns, treatment_num);
      sprintf(roadcast_output_filename, "roadcast.%03d.%d.csv", ns, treatment_num);

      // Write input CSV files for METRo
      if (0 != write_METRo_station_input_csv_file(station_file_name, lat, lon, site_id, 
						  site_layer, bridge, layer_mat_string))
	{
	  int exit_status = 1;
	  proc_log->write_time("Error: Write METRo CSV station file failed\n");
	  return(exit_status);
	}

      if (0 != write_METRo_obs_input_csv_file(obs_file_name, forc_time, obs_start_time, site_id, bridge, 
					      obs_ts, wx_fcst_ts, roadcond,
					      nt_start, current_hour))
	{
	  int exit_status = 1;
	  proc_log->write_time("Error: Write METRo CSV obs file failed\n");
	  return(exit_status);
	}

      if ( 0 != write_METRo_forecast_input_csv_file(forecast_file_name, forc_time, chem_failure_time, 
						    current_hour, num_fcst, nt_start, 
						    site_id, wx_fcst_ts, treatment_option))
	{
	  int exit_status = 1;
	  proc_log->write_time("Error: Write METRo CSV forecast file failed\n");
	  return(exit_status);
	}
    }
  else
    {
      int exit_status = 1;
      proc_log->write_time("Error: Bad METRo run type %d\n", metro_run_type);
      return(exit_status);
    }

  //--------------------------------------------------------------------------
  // Calculate Road Temp by starting METRo executable and waiting for it to be finished.
 
  command = "run_metro.py ";
  command += forecast_file_name; command += " "; command += station_file_name; command += " ";
  command += obs_file_name; command += " "; command += roadcast_output_filename;

  // Determine format type
  if (metro_run_type == METRO_XML)
    command += " xml";
  else
    command += " csv";


  // Add radiation flags
  if (useSolarFluxForecast)
    command += " 1";
  else
    command += " 0";

  if (useIrFluxForecast)
    command += " 1";
  else
    command += " 0";


  // Check for log file or stdout
  const char *log_path = proc_log->get_path();
  if (log_path)
    {
      string log_name = string(log_path);
      string asc_ext(".asc");
      size_t found;
      found=log_name.rfind(asc_ext);

      if (found!=string::npos)
	{
	  log_name.replace (found, asc_ext.length(), ".metro_log");
	  command += " " + log_name;
	}
      else
	command += " stdout";
    }
  else
    command += " stdout";


  // Run command
  ret = system(command.c_str());

  // Handle non-zero return status. 
  int exit_status = WEXITSTATUS(ret);
  if (exit_status == 1)
    {
      proc_log->write_time("Warning: Failure running METRo command: %s\n", command.c_str());
      return(exit_status);
    }
  else if (exit_status > 1)
    {
      proc_log->write_time("Error: METRo not found or usage error: %s\n", command.c_str());
      return(exit_status);
    }
  

  // Parse the METRo output file
  if (metro_run_type == METRO_XML)
    parse_METRo_xml_output(roadcast_output_filename, current_hour, forc_time, num_fcst, roadcond);
  else
    parse_METRo_csv_output(roadcast_output_filename, current_hour, forc_time, num_fcst, roadcond);

  return(0);
}

