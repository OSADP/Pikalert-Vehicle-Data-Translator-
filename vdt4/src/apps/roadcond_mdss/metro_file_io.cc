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
 * Module: metro_file_io.cc
 *
 * Author: Julie Prestopnik
 *
 * Date:   02/14/07
 *
 * Description: 
 *     This file contains several modules for creating the input
 *     XML files required by METRo and a module to parse the XML
 *     output for METRo.
 *
 *     write_METRo_forecast_input_file()....Writes the input forecast file in XML for METRo.
 * 
 *     write_METRo_obs_input_file().........Writes the input observation file in XML for METRo.
 * 
 *     write_METRo_station_input_file().....Writes the input station file in XML for METRo.
 *
 *     parse_METRo_output().................Parses the output roadcast XML file for METRo and
 *                                          puts the values in a RoadConditions structure.
 *
 *     init_road_cond().....................Initializes the values in the RoadConditions structure.
 *
 *     check_value()........................Check to see if the value is missing and returns an
 *                                          appropriate value.
 *     check_st_value().....................Check to see if the (sub)surface temperature value is 
 *                                          missing and returns an appropriate value within the
 *                                          valid METRo range (-40 to 80C).
 * 
 *     convert_layer_mat()..................Converts integer codes into text for road layers.
 *
 */

/* System include files / Local include files */
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <new>
#include <vector>
#include <string.h>
#include <time.h>
#include <fstream>

#include <log/log.hh>
#include <nxncf/nxncf.hh>
#include "Obs.hh"
#include "RulesOfPractice.hh"
#include "SiteLayerTraffic.hh"
#include "layer_info.hh"
#include "Utilities.hh"

/* External functions / Internal global functions / Internal static functions */
void write_METRo_forecast_input_file(char *forecast_file_name, double forc_time, int chem_effective_time,
				     int current_hour, int num_fcst, int nt_start, int site_id, WxFcstTS *wx_fcst_ts);

void write_METRo_obs_input_file(char * file_name, double forc_time, double obs_start_time, 
				int site_id, int bridge, ObsTS *obs_ts, WxFcstTS *wx_fcst_ts,
				int nt_start, int current_hour);

int write_METRo_station_input_file(char *station_file_name, float lat, float lon,  int site_id,
					  LayerInfo *site_layer, int bridge, string layer_mat_string[]);

void parse_METRo_output(char *pavement_T_output_filename, int current_hour, double forc_time,
			int num_fcst, RoadConditions *roadcond);

void init_road_cond(int num_fcst, RoadConditions *roadcond);

int check_value(int value);

float check_value(float value);

float check_st_value(float value);

int convert_layer_mat(int layer_mat[], string layer_mat_string[], int num_layers);

int convert_road_cond_metro_value_to_ssi_code(int road_cond_value);

extern Log *proc_log;

//  
//  Routine: write_METRo_forecast_input_file()
//  
//  Description:
//    Write the input forecast file in XML for METRo. This uses some site info
//    and weather info.  After the header lines, there is one line per forecast 
//    hour.
//  
//  Inputs: 
//    forc_time.............The Unix time of the start of the forecast.
//    chem_effective_time...The time in hours at which the chemicals on the road will fail.
//                          This should be "a negative number" if no chemicals have been put down.
//    num_fcst..............The last hour of the forecast.
//    nt_start..............The offset into the weather time series of the first forecast in 
//                          the weather data time series.
//    site_id...............The site id for the forecast site.
//    wx_fcst_ts............An array containing the pertinent weather forecast variables for this site.
//    treatment_option......The treatment option - only used to turn snow to rain in SUFF_TREATMENT
//  
//  Outputs:
//    forecast_file_name.........The name of the forecast input file to write to.
//  
//  Returns:
//    None.
//  

void write_METRo_forecast_input_xml_file(char *forecast_file_name, double forc_time, int chem_effective_time,
					 int current_hour, int num_fcst, int nt_start, int site_id,  
					 WxFcstTS *wx_fcst_ts, int treatment_option)
{
  int f_hr; 
  double ftime = forc_time + current_hour * SEC_PER_HOUR;
  float total_rain = 0;
  float total_snow = 0;
  int precip_type;
  tm *t;
  time_t clock;
  FILE *fp;
  UTIMstruct date;
  int start_hr;
  int chem_failure_hr;
  
  if (NULL == (fp = fopen(forecast_file_name, "w")))
    {
      proc_log->write_time("Error: Unable to open output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
 
  // Print the header information for the input forecast file
  if (0 > fprintf(fp, "%s\n", "<?xml version =\"1.0\" ?>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "<forecast>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "      <version>1.1</version>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  time(&clock);
  t = gmtime(&clock);

  //  if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n",
  //		  "      <production-date>", t->tm_year+1900,"-",
  //		  (t->tm_mon+1<10)?"0":"",t->tm_mon+1,"-",
  //		  (t->tm_mday<10)?"0":"",t->tm_mday,"T",
  //		  (t->tm_hour+1<10)?"0":"",t->tm_hour+1,":",
  //		  (t->tm_min+1<10)?"0":"",t->tm_min+1,"Z",
  //		  "</production-date>"))
  if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n",
		  "      <production-date>", t->tm_year+1900,"-",
		  (t->tm_mon+1<10)?"0":"",t->tm_mon+1,"-",
		  (t->tm_mday<10)?"0":"",t->tm_mday,"T",
		  (t->tm_hour<10)?"0":"",t->tm_hour,":",
		  (t->tm_min<10)?"0":"",t->tm_min,"Z",
		  "</production-date>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
 
  if (0 > fprintf(fp, "%s%i%s\n", "      <station-id>", site_id,"</station-id>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  </header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <prediction-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  start_hr = nt_start + current_hour;
  chem_failure_hr = nt_start + chem_effective_time;

  // Print the forecast for each hour
  //cout << "num_fcst = " << num_fcst << " current_hour = " << current_hour << endl;
  for (f_hr = 0; f_hr < num_fcst - current_hour; f_hr++)
    {
      if (0 > fprintf(fp, "%s\n", "      <prediction>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Forecast Date and Time
      UTIMunix_to_date((time_t)ftime, &date);

      //METRo requires a zero in front of every month, day, hour, min less than ten
      //Example: "01" is required instead of "1" for January
      if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n", 
		      "          <forecast-time>",date.year,"-",
		      (date.month<10)?"0":"",date.month,"-",
		      (date.day<10)?"0":"",date.day,"T",
		      (date.hour<10)?"0":"",date.hour, ":",
		      (date.min<10)?"0":"",date.min, "Z",
		      "</forecast-time>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Temperature
      if (0 > fprintf(fp, "%s%.2f%s\n", "          <at>", check_value(wx_fcst_ts->fcst[start_hr + f_hr].T), "</at>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Dew point
      // Make sure dewpoint depression is not too large if precip is occurring
      // As per Kevin Petty's request
      float dewpt = wx_fcst_ts->fcst[start_hr + f_hr].dewpt;
      if (dewpt != FCST_MISSING_VALUE && dewpt != NC_FILL_FLOAT &&
	  wx_fcst_ts->fcst[start_hr + f_hr].Ptype != C_PTYPE_NONE &&
	  wx_fcst_ts->fcst[start_hr + f_hr].T - dewpt > 1.0)
	dewpt = wx_fcst_ts->fcst[start_hr + f_hr].T - 1.0;

      if (0 > fprintf(fp, "%s%.2f%s\n", "          <td>", check_value(dewpt), "</td>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Precip since start of forecast
      precip_type = wx_fcst_ts->fcst[start_hr + f_hr].Ptype;
      if (wx_fcst_ts->fcst[start_hr + f_hr].qpf01 == FCST_MISSING_VALUE ||
	  wx_fcst_ts->fcst[start_hr + f_hr].qpf01 == NC_FILL_FLOAT)
	{
	  total_rain = FCST_MISSING_VALUE;
	  total_snow = FCST_MISSING_VALUE;
	}
      else if (treatment_option == SUFFICIENT_TREATMENT)
	{
	  total_rain = total_rain + wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  total_snow = 0.;
	}
      else if (start_hr + f_hr < chem_failure_hr && precip_type == C_PTYPE_SNOW)
	{
	  // Need to change rain to snow (Bad comment? Cowie 12/21/12)
	  total_rain = total_rain + wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  // total_snow unchanged
        }
      else if (wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_RAIN ||
	       wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_FRZRAIN)
	{
	  // Raining
	  total_rain += wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  // total_snow unchanged
	}
      else if (wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_SNOW)
	{
	  // Snowing
	  // total rain unchanged
	  total_snow += wx_fcst_ts->fcst[start_hr + f_hr].snow_accum * MM_TO_CM;
	}
      else
	{
	  // No precip
	  // total rain and snow unchanged
	}

      if (0 > fprintf(fp, "%s%.2f%s\n", "          <ra>", check_value(total_rain), "</ra>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%s%.2f%s\n", "          <sn>", check_value(total_snow), "</sn>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Wind Speed
      float wind_speed = wx_fcst_ts->fcst[start_hr + f_hr].wind_speed;
      if (wind_speed != FCST_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%s%.2f%s\n", "          <ws>", check_value(wind_speed), "</ws>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Surface Pressure
      if (0 > fprintf(fp, "%s%.2f%s\n", "          <ap>", check_value(wx_fcst_ts->fcst[start_hr + f_hr].P_sfc), "</ap>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Cloud Cover
      float cloud_cov = wx_fcst_ts->fcst[start_hr + f_hr].cloud_cov;
      if (cloud_cov != FCST_MISSING_VALUE && cloud_cov != NC_FILL_FLOAT)
	cloud_cov = cloud_cov * OCTAL + 0.5;

      if (0 > fprintf(fp, "%s%i%s\n", "          <cc>", (int)check_value(cloud_cov), "</cc>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Downward solar radiation
      if (0 > fprintf(fp, "%s%.2f%s\n", "          <sf>", check_value(wx_fcst_ts->fcst[start_hr + f_hr].dswrf_sfc), "</sf>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Downward infrared radiation
      if (0 > fprintf(fp, "%s%.2f%s\n", "          <ir>", check_value(wx_fcst_ts->fcst[start_hr + f_hr].dlwrf_sfc), "</ir>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Wind Direction
      if (0 > fprintf(fp, "%s%.2f%s\n", "          <wd>", check_value(wx_fcst_ts->fcst[start_hr + f_hr].wind_dir), "</wd>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
    
      if (0 > fprintf(fp, "%s\n", "      </prediction>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      ftime = ftime + SEC_PER_HOUR;
    
    }

  // Print footer information
  if (0 > fprintf(fp, "%s\n", "  </prediction-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "</forecast>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }


  fclose(fp);  
  return;
}

//  
//  Routine: write_METRo_obs_input_file()
//  
//  Description:
//    Write the input observation file in XML for METRo. This uses 12 hours
//    of obs info leading up to the forecast time.  After the header lines, 
//    there is one line per obs hour.
//  
//  Inputs: 
//    forc_time.............The Unix time of the start of the forecast.
//    num_fcst..............The last hour of the forecast.
//    obs_start_time........The Unix time of the start of the observations.
//    site_id...............The site id for the forecast site.
//    obs_ts................An array containing the pertinent observation variables for this site.
//    wx_fcst_ts............An array containing the weather forecast variables for this site.
//    nt_start..............Point representing current time in the weather times series.
//    current_hour..........Hours after current for this run of metro to start.
//  
//  Outputs:
//    obs_file_name.........The name of the observation input file to write to.
//  
//  Returns:
//    None.
//  

void write_METRo_obs_input_xml_file(char * obs_file_name, double forc_time, double obs_start_time,
				    int site_id, int bridge, ObsTS *obs_ts, WxFcstTS *wx_fcst_ts,
				    RoadConditions *roadcond, int nt_start, int current_hour)
{   
  int i;
  double otime;
  FILE *fp;
  UTIMstruct date;
  int start_hr;

  //Set up first observation time
  otime = (((forc_time - obs_start_time) / SEC_PER_HOUR) - NUM_OBS_HRS + 1) * SEC_PER_HOUR + 
    obs_start_time + current_hour * SEC_PER_HOUR;
  
  if (NULL == (fp = fopen(obs_file_name, "w")))
    {
      proc_log->write_time("Error: Unable to open output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
 
  // Print the header information for the input obs file
  if (0 > fprintf(fp, "%s\n", "<?xml version =\"1.0\" ?>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "<observation>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", " <header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <filetype>rwis-observation</filetype>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <version>1.0</version>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s%i%s\n", "  <road-station>", site_id,"</road-station>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", " </header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", " <measure-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Print the observations for each hour (before current time)
  for (i=0; i<NUM_OBS_HRS - current_hour; i++)
    {
      if (0 > fprintf(fp, "%s\n", "  <measure>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Obs Date and Time
      UTIMunix_to_date((time_t)otime, &date);

      if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n", 
		      "    <observation-time>",date.year,"-",
		      (date.month<10)?"0":"",date.month,"-",
		      (date.day<10)?"0":"",date.day,"T",
		      (date.hour<10)?"0":"",date.hour, ":",
		      (date.min<10)?"0":"",date.min, "Z",
		      "</observation-time>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Temperature
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <at>", check_value(obs_ts->obs[current_hour+i].T), "</at>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Dew point
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <td>", check_value(obs_ts->obs[current_hour+i].dewpt), "</td>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Precip
      int precip = obs_ts->obs[current_hour+i].Precip;
      if (precip == PRECIP_ROAD_COND_MISSING_VALUE)
	precip = OBS_MISSING_VALUE;

      if (0 > fprintf(fp, "%s%d%s\n", "    <pi>", check_value(precip), "</pi>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Wind Speed
      float wind_speed = obs_ts->obs[current_hour+i].wspd;
      if (wind_speed != OBS_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%s%.2f%s\n", "    <ws>", check_value(wind_speed), "</ws>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Condition
      int road_cond = 0;
      int rc_tmp = obs_ts->obs[current_hour+i].road_condition;
      if (rc_tmp == PRECIP_ROAD_COND_MISSING_VALUE ||
	  rc_tmp == OBS_MISSING_VALUE ||
	  rc_tmp == NC_FILL_INT)
	road_cond = OBS_MISSING_VALUE;
      else 
	road_cond = convert_road_cond_metro_value_to_ssi_code(rc_tmp);

      if (0 > fprintf(fp, "%s%d%s\n", "    <sc>", check_value(road_cond), "</sc>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Temperature
      // If the site is a bridge site, read in the bridge temp,
      // otherwise read in the road temp.
      if (bridge)
	{
	  if (0 > fprintf(fp, "%s%.2f%s\n", "    <st>", check_st_value(obs_ts->obs[current_hour+i].bridge_T), "</st>"))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      else
	{
	  if (0 > fprintf(fp, "%s%.2f%s\n", "    <st>", check_st_value(obs_ts->obs[current_hour+i].road_T),"</st>"))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }	  
	}
      
      // Subsurface Temperature
      // If the site is a bridge site, there is no subsurface temp,
      // so read in the air temperature, otherwise read in the subsurface temp.
      if (bridge)
	{
	  if (0 > fprintf(fp, "%s%.2f%s\n", "    <sst>", check_st_value(obs_ts->obs[current_hour+i].T), "</sst>"))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      else
	{
	  if (0 > fprintf(fp, "%s%.2f%s\n", "    <sst>", check_st_value(obs_ts->obs[current_hour+i].subsurface_T),"</sst>"))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      
      // End obs hour
      if (0 > fprintf(fp, "%s\n", "  </measure>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      otime = otime + SEC_PER_HOUR;

    }

  // Print the pseudo-observations for each hour ahead of current time (using forecasts)
  if (current_hour > NUM_OBS_HRS)
    start_hr = current_hour - NUM_OBS_HRS + 1;
  else
    start_hr = 1;

  for (i=start_hr; i<=current_hour; i++)
    {

      if (0 > fprintf(fp, "%s\n", "  <measure>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Obs Date and Time
      UTIMunix_to_date((time_t)otime, &date);

      if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n", 
		      "    <observation-time>",date.year,"-",
		      (date.month<10)?"0":"",date.month,"-",
		      (date.day<10)?"0":"",date.day,"T",
		      (date.hour<10)?"0":"",date.hour, ":",
		      (date.min<10)?"0":"",date.min, "Z",
		      "</observation-time>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Temperature
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <at>", check_value(wx_fcst_ts->fcst[nt_start+i].T) , "</at>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Dew point
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <td>", check_value(wx_fcst_ts->fcst[nt_start+i].dewpt), "</td>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Precip
      float precip = wx_fcst_ts->fcst[nt_start+i].qpf01;
      if (precip != FCST_MISSING_VALUE && precip != NC_FILL_FLOAT && precip > 0.)
	precip = 1.;

      if (0 > fprintf(fp, "%s%d%s\n", "    <pi>", check_value((int)precip), "</pi>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Wind Speed
      float wind_speed = wx_fcst_ts->fcst[nt_start+i].wind_speed;
      if (wind_speed != FCST_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%s%.2f%s\n", "    <ws>", check_value(wind_speed), "</ws>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Condition
      int road_cond;
      if (wx_fcst_ts->fcst[nt_start+i].qpf01 == FCST_MISSING_VALUE ||
	  wx_fcst_ts->fcst[nt_start+i].qpf01 == NC_FILL_FLOAT)
	road_cond = FCST_MISSING_VALUE;
      else if (wx_fcst_ts->fcst[nt_start+i].qpf01 > 0)
	road_cond = convert_road_cond_metro_value_to_ssi_code(2);
      else 
	road_cond = convert_road_cond_metro_value_to_ssi_code(1);
      
      if (0 > fprintf(fp, "%s%d%s\n", "    <sc>", check_value(road_cond), "</sc>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Temperature
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <st>", check_st_value(roadcond[i].road_T), "</st>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Subsurface Temperature
      if (0 > fprintf(fp, "%s%.2f%s\n", "    <sst>", check_st_value(roadcond[i].subsurface_T), "</sst>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // End obs hour
      if (0 > fprintf(fp, "%s\n", "  </measure>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      otime = otime + SEC_PER_HOUR;

    }

  // Print footer information
  if (0 > fprintf(fp, "%s\n", " </measure-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "</observation>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  fclose(fp);
  return;
}



//  
//  Routine: write_METRo_station_input_file()
//  
//  Description:
//    Write the input station file in XML for METRo. This uses static site
//    information.  
//  
//  Inputs: 
//    lat..................The latitude of the forecast location.
//    lon..................The longitude of the forecast location.
//    site_id..............The site id for the forecast site.
//    site_layer...........A structure containing the subsurface characteristics of the site
//                         and its parameterization.
//    bridge...............An integar indicating if this site is a bridge site or a road site.
//    layer_mat_string.....An array of road layer strings (e.g. asphalt, crushed rock, or cement)
//  
//  Outputs:
//    station_file_name....The name of the station input file to write to.
//  
//  Returns:  0.......................If no fatal errors were found.
//            1.......................If a fatal processing condition was encountered.    
//  

int write_METRo_station_input_xml_file(char *station_file_name, float lat, float lon,  int site_id,
				       LayerInfo *site_layer, int bridge, string layer_mat_string[])
{
  int i,j;
  int position = 0;
  float thickness;
  string station_type = "";
  struct tm *t;
  time_t clock;
  FILE *fp;
  int exit_status = 0;
  
  if (NULL == (fp = fopen(station_file_name, "w")))
    {
      proc_log->write_time("Error: Unable to open output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
 
  // Print header information for input station file
  if (0 > fprintf(fp, "%s\n", "<?xml version =\"1.0\" ?>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "<station>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "      <filetype>rwis-configuration</filetype>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "      <version>1.0</version>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s%i%s\n", "      <road-station>", site_id, "</road-station>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s%s%s\n", "      <time-zone>", site_layer->tz_code.c_str(), "</time-zone>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  time(&clock);
  t = gmtime(&clock);

  if (0 > fprintf(fp, "%s%i%s%s%i%s%s%i%s%s%i%s%s%i%s%s\n",
		  "      <production-date>", t->tm_year+1900,"-",
		  (t->tm_mon+1<10)?"0":"",t->tm_mon+1,"-",
		  (t->tm_mday<10)?"0":"",t->tm_mday,"T",
		  (t->tm_hour<10)?"0":"",t->tm_hour,":",
		  (t->tm_min<10)?"0":"",t->tm_min,"Z",
		  "</production-date>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "      <coordinate>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Latitude
  if (0 > fprintf(fp, "%s%.2f%s\n", "          <latitude>", lat, "</latitude>" ))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Longitude
  if (0 > fprintf(fp, "%s%.2f%s\n", "          <longitude>", lon, "</longitude>" ))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "      </coordinate>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  
  
  if (bridge)
    station_type = "bridge";
  else
    station_type = "road";
    
  if (0 > fprintf(fp, "%s%s%s\n", "      <station-type>",station_type.c_str(),"</station-type>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  </header>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "  <roadlayer-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 != convert_layer_mat(site_layer->layer_mat, layer_mat_string, site_layer->num_layer))
    {
      exit_status = 1;
      return(exit_status);
    }
  
  // Print road layer attributes
  for (i = MAX_LAYER-1; i >= 0 ; i--)
    {

      if (layer_mat_string[i] == "")
	continue;
      
      position++;

      if (0 > fprintf(fp, "%s\n", "      <roadlayer>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%s%i%s\n", "          <position>",position,"</position>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%s%s%s\n", "          <type>", layer_mat_string[i].c_str(),"</type>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // METRo workaround:
      // Cement conductivity is higher than for asphalt and needs a higher 
      // space resolution. For a thin layer of cement, METRo does not have
      // enough resolution.  To have a numerically stable solution, we 
      // need to have a thickness of at least MIN_CEMENT_THICKNESS  for cement.
      float thickness = site_layer->layer_thickness[i];
      //if ((layer_mat_string[i] == "cement") && (thickness < MIN_CEMENT_THICKNESS))
      //thickness = MIN_CEMENT_THICKNESS;

      if (0 > fprintf(fp, "%s%.4f%s\n", "          <thickness>",thickness,"</thickness>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%s\n", "      </roadlayer>"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
    }

  if (0 > fprintf(fp, "%s\n", "  </roadlayer-list>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  if (0 > fprintf(fp, "%s\n", "</station>"))
    {
      proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  
  fclose(fp);
  return 0;
}

//  
//  Routine: parse_METRo_output()
//  
//  Description:
//    Parse the output roadcast XML file for METRo and puts the values in a 
//    RoadConditions structure.
//  
//  Inputs: 
//    roadcast_output_filname....The name of the output roadcast file to read from.
//    forc_time..................The Unix time of the start of the forecast.
//    num_fcst...................The last hour of the forecast.
//  
//  Outputs:
//    roadcond...................The structure that holds the values from the output roadcast
//  
//  Returns:
//    None.
// 

void parse_METRo_xml_output(char *roadcast_output_filename, int current_hour, double forc_time, 
			    int num_fcst, RoadConditions *roadcond)
{
  ifstream in;
  string str;
  TaXml output;
  UTIMstruct utim_date;
  time_t roadcast_time;
  double hours_offset_from_forecast;
  int count;

  const string roadcast_time_tag = "roadcast-time";
  vector<string> time;
  vector<string>::iterator ix_time;

  const string road_temp_tag = "st";
  vector<string> road_temp;
  vector<string>::iterator ix_road_temp;

  const string road_subsurf_temp_tag = "sst";
  vector<string> road_subsurf_temp;
  vector<string>::iterator ix_road_subsurf_temp;

  const string road_condition_tag = "rc";
  vector<string> road_condition;
  vector<string>::iterator ix_road_condition;

  const string q_snow_ice_road_tag = "sn";
  vector<string> q_snow_ice_road;
  vector<string>::iterator ix_q_snow_ice_road;
 
  // Initialize the values for this site in the road condition structure
  if (current_hour == 0)
    init_road_cond(num_fcst, roadcond);

  in.open(roadcast_output_filename, ifstream::in);
  if ( ! in ) {
    proc_log->write_time("Error: Unable to read from output file %s.\n", roadcast_output_filename);
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }
  
  // Get length of file
  in.seekg(0,ios::end);

  int length = in.tellg();  
  if (length == -1)
    {
      proc_log->write_time("Error: Unable to get length of output file %s.\n", roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  in.seekg(0,ios::beg);

  // Allocate memory
  char *buffer = new char[length+1];

  // Read data as a block
  in.read(buffer, length);
  
  buffer[length] = '\0';

  in.close();
  
  str.assign(buffer);

  // Get forecast time
  if ( 0 > output.readStringArray(str, roadcast_time_tag, time))
    {
      proc_log->write_time("Error: Unable to read %s from output file %s.\n", roadcast_time_tag.c_str(), roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Get road temperature values
  if ( 0 > output.readStringArray(str, road_temp_tag, road_temp))
    {
      proc_log->write_time("Error: Unable to read %s from output file %s.\n", road_temp_tag.c_str(), roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  ix_road_temp = road_temp.begin();

  // Get road subsurface temperature values
  if ( 0 > output.readStringArray(str, road_subsurf_temp_tag, road_subsurf_temp))
    {
      proc_log->write_time("Error: Unable to read %s from output file %s.\n", road_subsurf_temp_tag.c_str(), roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  ix_road_subsurf_temp = road_subsurf_temp.begin();

  // Get road condition values
  if ( 0 > output.readStringArray(str, road_condition_tag, road_condition))
    {
      proc_log->write_time("Error: Unable to read %s from output file %s.\n", road_condition_tag.c_str(), roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  ix_road_condition = road_condition.begin();
  
  // Get quantity of snow or ice on the road values
  if ( 0 > output.readStringArray(str, q_snow_ice_road_tag, q_snow_ice_road))
    {
      proc_log->write_time("Error: Unable to read %s from output file %s.\n", q_snow_ice_road_tag.c_str(), roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }
  ix_q_snow_ice_road = q_snow_ice_road.begin();

  // Exception: First roadcast starts 20 minutes after the forecast time, but we want 
  // these value to be our first roadcast
  ix_time = time.begin();

  string date = *ix_time;
  string month = date.substr(5,2);
  string day  = date.substr(8,2);
  string year = date.substr(0,4);
  string hour = date.substr(11,2);
  // METRo's first roadcast starts 20 minutes after the forecast hour
  // We want to take this first roadcast and consider it as the roadcast 
  // for the forecast hour.
  // Therefore, we'll replace the minutes with "00" in this code because the forecast
  // hour will always be on the hour
  string minute = "00";
  string second = "00";
  
  // Get the ISO 8601 time of the first roadcast we want
  // Put the time info for the first roadcast into a UTIMstruct
  utim_date.year = atoi(year.c_str());
  utim_date.month = atoi(month.c_str());
  utim_date.day = atoi(day.c_str());
  utim_date.hour = atoi(hour.c_str());
  utim_date.min = atoi(minute.c_str());      
  utim_date.sec = atoi(second.c_str());
  // Get the unix time of the first roadcast we want
  roadcast_time = UTIMdate_to_unix(&utim_date);
  
  // Roadcast time should be the same as the forecast time, but grab the offset if there is one
  hours_offset_from_forecast = (roadcast_time - forc_time) / SEC_PER_HOUR;
  proc_log->write_time(1, "Info: The roadcast starts %d hour(s) after the forecast time.\n", int(hours_offset_from_forecast));
  
  // Want to start filling roadcast struct at hours_offset_from_forecast
  count = int(hours_offset_from_forecast);
  
  roadcond[count].road_T = atof(ix_road_temp->c_str());
  
  roadcond[count].subsurface_T = atof(ix_road_subsurf_temp->c_str());
  
  roadcond[count].road_condition = atoi(ix_road_condition->c_str());
  
  roadcond[count].PsnowOnRoad = atof(ix_q_snow_ice_road->c_str());
  //Convert from centimeters to mm
  roadcond[count].PsnowOnRoad *= CM_TO_MM;
  
  count++;	  
   
  // METRo give forecasts every 20 minutes, but we only want the 
  // roadcast on the hour
  for(; ix_time != time.end(); ix_time++)
    {
      string date = *ix_time;
      string minute = date.substr(14,2);
      string on_the_hour = "00";

      if (minute == on_the_hour)
        {
          if (count >= num_fcst)
            {
              proc_log->write_time("Error: The number of forecast hours has exceeded %d in %s.\n", num_fcst, roadcast_output_filename);
              proc_log->write_time("Ending: exit status = 1.\n");
              exit(1);
            }

	  roadcond[count].road_T = atof(ix_road_temp->c_str());
	  
	  roadcond[count].subsurface_T = atof(ix_road_subsurf_temp->c_str());

	  roadcond[count].road_condition = atoi(ix_road_condition->c_str());
	  
	  roadcond[count].PsnowOnRoad = atof(ix_q_snow_ice_road->c_str());
	  //Convert from centimeters to mm
	  roadcond[count].PsnowOnRoad *= CM_TO_MM;
	  
	  count++;	  
	}

      ix_road_temp++;
      ix_road_subsurf_temp++;
      ix_road_condition++;
      ix_q_snow_ice_road++;
    }
  
  delete[] buffer;
 
  return;
}

//  Routine: check_value()
//
//  Description:
//    These modules check to see if the value (int or float) 
//    is set to missing. If the value is missing, it sets 
//    the value to METRo's value for missing data, otherwise
//    it returns the value originally passed to it.
//
//  Input:
//    value...........The value to check.
//
//
//  Returns:
//    int or float...........Missing value or value passed in.
//

int check_value(int value)
{
  if ((value == OBS_MISSING_VALUE) || (value == FCST_MISSING_VALUE) || value == NC_FILL_INT)
    {
      value = METRO_MISSING_VALUE;
      return value;
    } 
  
  else 
      return value;
}

float check_value(float value)
{
  if ((fabs(value-OBS_MISSING_VALUE) < .01) || (fabs(value-FCST_MISSING_VALUE) < .01) || (fabs(value-NC_FILL_FLOAT) < .01))
    {
      value = METRO_MISSING_VALUE;
      return value;
    } 
  else 
      return value;
}

float check_st_value(float value)
{
  if ((fabs(value-OBS_MISSING_VALUE) < .01) || (fabs(value-FCST_MISSING_VALUE) < .01) || (fabs(value-NC_FILL_FLOAT) < .01))
    {
      value = METRO_MISSING_VALUE;
      return value;
    } 
  else if (value > METRO_MAX_SURFACE_TEMP)
      return METRO_MAX_SURFACE_TEMP;
  else if (value < METRO_MIN_SURFACE_TEMP)
      return METRO_MIN_SURFACE_TEMP;
  else
      return value;
}


//
//  Routine: convert_layer_mat()
//
//  Description:
//    This module converts integer codes into text for road layers.
//
//  Inputs:
//    layer_mat...........An array of integer codes.
//
//
//  Outputs:
//    layer_mat_string....An array of road layer strings. (e.g. asphalt, crushed rock, or cement)
//
//  Returns:  0.......................If no fatal errors were found.
//            1.......................If a fatal processing condition was encountered.    
//

int convert_layer_mat(int layer_mat[], string layer_mat_string[], int num_layers)
{
  int i; 

  for (i = 0; i < num_layers; i++)
    {
      if (layer_mat[i] == 91)
	{
	  layer_mat_string[i] = "cement";
	}
      else if (layer_mat[i] == 92)
	{
	  layer_mat_string[i] = "crushed rock";
	}
      else if ((layer_mat[i] == 93) || (layer_mat[i] == 94))
	{
	  layer_mat_string[i] = "asphalt";
	}
      else
	{
	  proc_log->write_time("Error: Invalid material type (%i) in site config file\n", layer_mat[i]);
	  return -1;
	}
    }
  return 0;
}

//
//  Routine: convert_road_cond_metro_value_to_ssi_code()
//
//  Description:
//    This module converts METRo's integer code for road condition into 
//    an SSI code for input to METRo.
//
//  Inputs:
//    road_cond_value...........The METRo value (integer) for road condition.
//
//  Returns:
//    int.......................The SSI code for road condition

int convert_road_cond_metro_value_to_ssi_code(int road_cond_value)
{
  int wet_road = 34;
  int ice_snow_road = 35;
  int dry_road = 33;
  int frost = 40;

  if (road_cond_value == 2)
    return wet_road;
  // 4 is mix/water snow on the road but has no ssi code, so make it ice_snow_road
  else if (road_cond_value == 3 || road_cond_value == 4) 
    return ice_snow_road;
  else if (road_cond_value == 5) // dew - no ssi code, so make wet
    return wet_road;
  else if (road_cond_value == 6) // melting snow - no ssi code, so make wet
    return wet_road;
  else if (road_cond_value == 7)
    return frost;
  else if (road_cond_value == 8) // icing rain - no ssi code, so make wet
    return wet_road;
  else
    return dry_road;
}




int write_METRo_forecast_input_csv_file(char *forecast_file_name, double forc_time, int chem_effective_time,
					 int current_hour, int num_fcst, int nt_start, int site_id,  
					 WxFcstTS *wx_fcst_ts, int treatment_option)
{
  int f_hr; 
  double ftime = forc_time + current_hour * SEC_PER_HOUR;
  float total_rain = 0;
  float total_snow = 0;
  int precip_type;
  tm *t;
  time_t clock;
  FILE *fp;
  UTIMstruct date;
  int start_hr;
  int chem_failure_hr;
  char time_str[32];

  if (NULL == (fp = fopen(forecast_file_name, "w")))
    {
      return(1);
    }

  time(&clock);
  t = gmtime(&clock);

  if (0 >= sprintf(time_str, "%d-%02d-%02dT%02d:%02dZ",
		   t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min))
    {
      proc_log->write_time("Error: Unable to write time string for %s.\n", site_id);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Hardwire version number,
  if (0 >= fprintf(fp, "1.1, %s, %d\n", time_str, site_id))
    {
      proc_log->write_time("Error: Unable to write line 1 to station file for %s.\n", site_id);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }


  start_hr = nt_start + current_hour;
  chem_failure_hr = nt_start + chem_effective_time;

  // Print the forecast for each hour
  for (f_hr = 0; f_hr < num_fcst - current_hour; f_hr++)
    {
      // Forecast Date and Time
      UTIMunix_to_date((time_t)ftime, &date);

      //METRo requires a zero in front of every month, day, hour, min less than ten
      //Example: "01" is required instead of "1" for January
      if (0 > fprintf(fp, "%d-%02d-%02dT%02d:%02dZ, ", 
		      date.year, date.month, date.day, date.hour, date.min))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

 
      // Wind Speed
      float wind_speed = wx_fcst_ts->fcst[start_hr + f_hr].wind_speed;
      if (wind_speed != FCST_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%.2f, ", check_value(wind_speed)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
 
      // Surface Pressure
      if (0 > fprintf(fp, "%.2f, ", check_value(wx_fcst_ts->fcst[start_hr + f_hr].P_sfc)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

     // Temperature
      if (0 > fprintf(fp, "%.2f, ", check_value(wx_fcst_ts->fcst[start_hr + f_hr].T)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Dew point
      // Make sure dewpoint depression is not too large if precip is occurring
      // As per Kevin Petty's request
      float dewpt = wx_fcst_ts->fcst[start_hr + f_hr].dewpt;
      if (dewpt != FCST_MISSING_VALUE && dewpt != NC_FILL_FLOAT &&
	  wx_fcst_ts->fcst[start_hr + f_hr].Ptype != C_PTYPE_NONE &&
	  wx_fcst_ts->fcst[start_hr + f_hr].T - dewpt > 1.0)
	dewpt = wx_fcst_ts->fcst[start_hr + f_hr].T - 1.0;

      if (0 > fprintf(fp, "%.2f, ", check_value(dewpt)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}


      // Cloud Cover
      float cloud_cov = wx_fcst_ts->fcst[start_hr + f_hr].cloud_cov;
      if (cloud_cov != FCST_MISSING_VALUE && cloud_cov != NC_FILL_FLOAT)
	cloud_cov = cloud_cov * OCTAL + 0.5;

      if (0 > fprintf(fp, "%d, ", check_value((int)cloud_cov)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Precip since start of forecast
      precip_type = wx_fcst_ts->fcst[start_hr + f_hr].Ptype;
      if (wx_fcst_ts->fcst[start_hr + f_hr].qpf01 == FCST_MISSING_VALUE ||
	  wx_fcst_ts->fcst[start_hr + f_hr].qpf01 == NC_FILL_FLOAT)
	{
	  total_rain = FCST_MISSING_VALUE;
	  total_snow = FCST_MISSING_VALUE;
	}
      else if (treatment_option == SUFFICIENT_TREATMENT)
	{
	  total_rain = total_rain + wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  total_snow = 0.;
	}
      else if (start_hr + f_hr < chem_failure_hr && precip_type == C_PTYPE_SNOW)
	{
	  // Need to change rain to snow
	  total_rain = total_rain + wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  // total_snow unchanged
        }
      else if (wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_RAIN ||
	       wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_FRZRAIN)
	{
	  // Raining
	  total_rain += wx_fcst_ts->fcst[start_hr + f_hr].qpf01;
	  // total_snow unchanged
	}
      else if (wx_fcst_ts->fcst[start_hr + f_hr].Ptype == C_PTYPE_SNOW)
	{
	  // Snowing
	  // total rain unchanged
	  total_snow += wx_fcst_ts->fcst[start_hr + f_hr].snow_accum * MM_TO_CM;
	}
      else
	{
	  // No precip
	  // total rain and snow unchanged
	}

      if (0 > fprintf(fp, "%.2f, ", check_value(total_snow)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%.2f", check_value(total_rain)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

#ifdef NOT_NEEDED     
      // Wind Direction
      if (0 > fprintf(fp, "%.2f", check_value(wx_fcst_ts->fcst[start_hr + f_hr].wind_dir)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
#endif
  
      if (0 > fprintf(fp, "\n"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", forecast_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      ftime = ftime + SEC_PER_HOUR;
    
    }

  fclose(fp);

  return(0);
}


int write_METRo_obs_input_csv_file(char * obs_file_name, double forc_time, double obs_start_time,
				   int site_id, int bridge, ObsTS *obs_ts, WxFcstTS *wx_fcst_ts,
				   RoadConditions *roadcond, int nt_start, int current_hour)
{
  int i;
  double otime;
  FILE *fp;
  UTIMstruct date;
  int start_hr;

  //Set up first observation time
  otime = (((forc_time - obs_start_time) / SEC_PER_HOUR) - NUM_OBS_HRS + 1) * SEC_PER_HOUR + 
    obs_start_time + current_hour * SEC_PER_HOUR;

  if (NULL == (fp = fopen(obs_file_name, "w")))
    {
      proc_log->write_time("Error: Could not open %s\n", obs_file_name);
      return(1);
    }

  if (0 >= fprintf(fp, "1.0, %d, rwis-observation\n", site_id))
    {
      proc_log->write_time("Error: Unable to write line 1 to station file for %s.\n", site_id);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Print the observations for each hour (before current time)
  for (i=0; i<NUM_OBS_HRS - current_hour; i++)
    {
      // Obs Date and Time
      UTIMunix_to_date((time_t)otime, &date);

      if (0 > fprintf(fp, "%d-%02d-%02dT%02d:%02dZ, ", 
		      date.year, date.month, date.day, date.hour, date.min))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Temperature
      if (0 > fprintf(fp, "%.2f, ", check_value(obs_ts->obs[current_hour+i].T)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Dew point
      if (0 > fprintf(fp, "%.2f, ", check_value(obs_ts->obs[current_hour+i].dewpt)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Precip
      int precip = obs_ts->obs[current_hour+i].Precip;
      if (precip == PRECIP_ROAD_COND_MISSING_VALUE)
	precip = OBS_MISSING_VALUE;
	  
      if (0 > fprintf(fp, "%d, ", check_value(precip)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Wind Speed
      float wind_speed = obs_ts->obs[current_hour+i].wspd;
      if (wind_speed != OBS_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	  wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%.2f, ", check_value(wind_speed)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Condition
      int road_cond = 0;
      int rc_tmp = obs_ts->obs[current_hour+i].road_condition;
      if (rc_tmp == PRECIP_ROAD_COND_MISSING_VALUE || rc_tmp == OBS_MISSING_VALUE)
	road_cond = OBS_MISSING_VALUE;
      else 
	road_cond = convert_road_cond_metro_value_to_ssi_code(rc_tmp);

      if (0 > fprintf(fp, "%d, ", check_value(road_cond)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Temperature
      // If the site is a bridge site, read in the bridge temp,
      // otherwise read in the road temp.
      if (bridge)
	{
	  if (0 > fprintf(fp, "%.2f, ", check_value(obs_ts->obs[current_hour+i].bridge_T)))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      else
	{
	  if (0 > fprintf(fp, "%.2f, ", check_value(obs_ts->obs[current_hour+i].road_T)))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }	  
	}
      
      // Subsurface Temperature
      // If the site is a bridge site, there is no subsurface temp,
      // so read in the air temperature, otherwise read in the subsurface temp.
      if (bridge)
	{
	  if (0 > fprintf(fp, "%.2f", check_value(obs_ts->obs[current_hour+i].T)))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      else
	{
	  if (0 > fprintf(fp, "%.2f", check_value(obs_ts->obs[current_hour+i].subsurface_T)))
	    {
	      proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	}
      

      // End obs hour
      if (0 > fprintf(fp, "\n"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      otime = otime + SEC_PER_HOUR;

    }

  // Print the pseudo-observations for each hour ahead of current time (using forecasts)
  if (current_hour > NUM_OBS_HRS)
    start_hr = current_hour - NUM_OBS_HRS + 1;
  else
    start_hr = 1;

  // Print the observations for each hour (before current time)
  for (i=start_hr; i<=current_hour; i++)
    {
      // Obs Date and Time
      UTIMunix_to_date((time_t)otime, &date);

      if (0 > fprintf(fp, "%d-%02d-%02dT%02d:%02dZ, ", 
		      date.year, date.month, date.day, date.hour, date.min))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      // Temperature
      if (0 > fprintf(fp, "%.2f, ", check_value(wx_fcst_ts->fcst[nt_start+i].T)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Dew point
      if (0 > fprintf(fp, "%.2f, ", check_value(wx_fcst_ts->fcst[nt_start+i].dewpt)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
	  
      // Precip
      float precip = wx_fcst_ts->fcst[nt_start+i].qpf01;
      if (precip != FCST_MISSING_VALUE && precip != NC_FILL_FLOAT && precip > 0.)
	precip = 1.;

      if (0 > fprintf(fp, "%d, ", check_value((int)precip)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Wind Speed
      float wind_speed = wx_fcst_ts->fcst[nt_start+i].wind_speed;
      if (wind_speed != FCST_MISSING_VALUE && wind_speed != NC_FILL_FLOAT)
	  wind_speed = wind_speed * MPS_TO_KMPH;

      if (0 > fprintf(fp, "%.2f, ", check_value(wind_speed)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Condition
      int road_cond;
      if (wx_fcst_ts->fcst[nt_start+i].qpf01 == FCST_MISSING_VALUE ||
	  wx_fcst_ts->fcst[nt_start+i].qpf01 == NC_FILL_FLOAT)
	road_cond = FCST_MISSING_VALUE;
      else if (wx_fcst_ts->fcst[nt_start+i].qpf01 > 0)
	road_cond = convert_road_cond_metro_value_to_ssi_code(2);
      else 
	road_cond = convert_road_cond_metro_value_to_ssi_code(1);

      if (0 > fprintf(fp, "%d, ", check_value(road_cond)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Road Temperature
      // If the site is a bridge site, read in the bridge temp,
      // otherwise read in the road temp.
      if (0 > fprintf(fp, "%.2f, ", check_value(roadcond[i].road_T)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // Subsurface Temperature
      if (0 > fprintf(fp, "%.2f", check_value(roadcond[i].subsurface_T)))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // End obs hour
      if (0 > fprintf(fp, "\n"))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", obs_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      otime = otime + SEC_PER_HOUR;

    }


  fclose(fp);
  return(0);
}

int write_METRo_station_input_csv_file(char *station_file_name, float lat, float lon,  int site_id,
				       LayerInfo *site_layer, int bridge, string layer_mat_string[])
{
  FILE *fp;
  struct tm *t;
  time_t clock;
  char time_str[32];
  string station_type = "";
  int exit_status = 0;
  int position = 0;
  int i;

  if (NULL == (fp = fopen(station_file_name, "w")))
    {
      proc_log->write_time("Error: Could not open %s\n", station_file_name);
      return(1);
    }

  time(&clock);
  t = gmtime(&clock);

  if (0 >= sprintf(time_str, "%d-%02d-%02dT%02d:%02dZ",
		   t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min))
    {
      proc_log->write_time("Error: Unable to write time string for %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit_status = 1;
      exit(exit_status);
    }

  if (bridge)
    station_type = "bridge";
  else
    station_type = "road";

  // Hardwire version number,
  if (0 >= fprintf(fp, "1.0, %d, %s, %s, %.2f, %.2f, %s\n", site_id, site_layer->tz_code.c_str(),
		   time_str, lat, lon, station_type.c_str()))
    {
      proc_log->write_time("Error: Unable to write line 1 to station file for %s.\n", station_file_name);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit_status = 1;
      exit(exit_status);
    }

  if (0 != convert_layer_mat(site_layer->layer_mat, layer_mat_string, site_layer->num_layer))
    {
      exit_status = 1;
      return(exit_status);
    }

  // Print road layer attributes
  for (i = MAX_LAYER-1; i >= 0 ; i--)
    {

      if (layer_mat_string[i] == "")
	continue;
      
      position++;

      if (0 >= fprintf(fp, "%d, ", position))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (0 > fprintf(fp, "%s, ", layer_mat_string[i].c_str()))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      
      // METRo workaround:
      // Cement conductivity is higher than for asphalt and needs a higher 
      // space resolution. For a thin layer of cement, METRo does not have
      // enough resolution.  To have a numerically stable solution, we 
      // need to have a thickness of at least MIN_CEMENT_THICKNESS  for cement.
      float thickness = site_layer->layer_thickness[i];
      //if ((layer_mat_string[i] == "cement") && (thickness < MIN_CEMENT_THICKNESS))
      //thickness = MIN_CEMENT_THICKNESS;

      if (0 > fprintf(fp, "%.4f\n", thickness))
	{
	  proc_log->write_time("Error: Unable to write to output file %s.\n", station_file_name);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
    }


  fclose(fp);
  return(0);
}

void parse_METRo_csv_output(char *roadcast_output_filename, int current_hour, double forc_time, 
			    int num_fcst, RoadConditions *roadcond)
{
  int i;
  FILE *fp;
  char *tok;
  char line[160];
  char version[32], run_time[32], station_name[32], roadway_type[32];
  float lat, lon;
  struct tm first_fcst_time, fcst_time;
  int yr, month, mday, hr, min;
  float road_T, road_condition, subsurface_T;
  float lead_time;
  time_t utime, start_time;
  int hours_offset_from_forecast;

  // Initialize the values for this site in the road condition structure
  if (current_hour == 0)
    init_road_cond(num_fcst, roadcond);

  if (NULL == (fp = fopen(roadcast_output_filename, "r")))
    {
      proc_log->write_time("Error: Could not open %s\n", roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Parse header line
  if (NULL != fgets(line, 160, fp))
    {
      if (NULL == (tok = strtok(line, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}


      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      //printf("roadway_type %s\n", tok);
      sprintf(roadway_type, "%s", tok);

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (5 != sscanf(tok, "%4d-%02d-%02dT%02d:%02dZ", &first_fcst_time.tm_year, &month, 
		      &first_fcst_time.tm_mday, &first_fcst_time.tm_hour, &first_fcst_time.tm_min))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      first_fcst_time.tm_year -= 1900;
      first_fcst_time.tm_mon = month - 1;
      first_fcst_time.tm_sec = 0;
      start_time = mktime(&first_fcst_time);
      start_time = (int)(start_time / 3600) * 3600;

      hours_offset_from_forecast = (int)((start_time - forc_time) / SEC_PER_HOUR + 0.5);

      /*
	printf("%4d%02d%02d %02d%02d00   start_time %d\n", first_fcst_time.tm_year, first_fcst_time.tm_mon,
	first_fcst_time.tm_mday, first_fcst_time.tm_hour, first_fcst_time.tm_min, start_time);
      */
    }
  else
    {
      proc_log->write_time("Error: Could not read first line from %s\n", roadcast_output_filename);
      proc_log->write_time("Ending: exit status = 1.\n");
      exit(1);
    }

  // Now parse road forecast lines
  int count = 0;
  while (NULL != fgets(line, 160, fp))
    {
      if (NULL == (tok = strtok(line, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      if (5 != sscanf(tok, "%4d-%02d-%02dT%02d:%02dZ", &fcst_time.tm_year, &month, 
		      &fcst_time.tm_mday, &fcst_time.tm_hour, &fcst_time.tm_min))
	{
	  proc_log->write_time("Error: Could not parse first line from %s\n", roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}

      fcst_time.tm_year -= 1900;
      fcst_time.tm_mon = month - 1;
      fcst_time.tm_sec = 0;
      utime = mktime(&fcst_time);

      /*
	printf("%4d%02d%02d %02d%02d00   utime %d\n", fcst_time.tm_year, fcst_time.tm_mon,
	fcst_time.tm_mday, fcst_time.tm_hour, fcst_time.tm_min, utime);
      */

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      lead_time = atof(tok);
      //printf("lead time %f %f\n", lead_time, lead_time - (int)lead_time);
      
      if (utime - start_time == 20*60)
	{
	  utime -= 20*60;
	  lead_time = (int)lead_time;
	}

      //printf("lead_time %f  timediff %d\n", lead_time, ((int)(utime-start_time))%3600);

      if (lead_time != (int)lead_time || ((int)(utime-start_time))%3600 != 0)
	{
	  count++;
	  continue;
	}

      int index = hours_offset_from_forecast + (int)((utime-start_time)/3600);
      //printf("index %d\n", index);
      if (index >= num_fcst)
	{
	  proc_log->write_time("Error: The number of forecast hours has exceeded %d in %s.\n", 
			       num_fcst, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}


      // Skip N values to get to road cond values
      for (i=0; i<3; i++)
	{
	  if (NULL == (tok = strtok(NULL, ",")))
	    {
	      proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	  //printf("%d %s\n", i, tok);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      roadcond[index].PsnowOnRoad = atof(tok);
      roadcond[index].PsnowOnRoad *= CM_TO_MM;
      //printf("snow on road %s\n", tok);

      for (i=0; i<12; i++)
	{
	  if (NULL == (tok = strtok(NULL, ",")))
	    {
	      proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	  //printf("%d %s\n", i, tok);
	}

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      roadcond[index].road_condition = atoi(tok);
      //printf("road state %s\n", tok);

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      roadcond[index].road_T = atof(tok);
      //printf("road temp %s\n", tok);

      if (NULL == (tok = strtok(NULL, ",")))
	{
	  proc_log->write_time("Error: Could not parse line %d from %s\n", count, roadcast_output_filename);
	  proc_log->write_time("Ending: exit status = 1.\n");
	  exit(1);
	}
      roadcond[index].subsurface_T = atof(tok);
      //printf("road sst %s\n", tok);

      //road_condition
      //subsurface_T
      count++;
    }


  fclose(fp);
  return;
}
