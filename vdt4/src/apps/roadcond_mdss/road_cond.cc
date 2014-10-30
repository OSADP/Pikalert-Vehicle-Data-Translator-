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
 * Module: road_cond.cc
 *
 * Author: Bill Myers
 *
 * Date:   10/12/01
 *
 * Description: This file contains one routine which acts as the main
 *              driver for the road conditions calculations. All input 
 *              files are passed to the appropriate modules to initialize
 *              internal data structures. Then a set of nested loops are 
 *              entered which generate the road conditions forecasts and
 *              treatment plans. These plans are stored in output arrays
 *              after each site has been processed and finally written to
 *              the output file.
 *              
 *       road_cond()...Main routine described above.
 *
 */

/* System include files / Local include files */
#include <iostream>
#include <math.h>
#include <new>
#include <vector>

#include "nxncf/nxncf.hh"
#include "site_list/site_list.hh"
#include "log/log.hh"
#include "layer.hh"
#include "parse_traffic_layer_config.hh"
#include "RulesOfPractice.hh"
#include "layer_info.hh"
#include "Obs.hh"

const static int HOUR_PER_DAY = 24;

// These should never be used - they should be over-ridden later by the
// the chem type/form in the user-defined treatment or from the site
// config file.
int P_user_chemtype = C_CHEMTYPE_CALIBER; // default
int P_user_chemform = C_CHEMFORM_PREWET; // default
int P_user_chemtype_pretreat = C_CHEMTYPE_CALIBER; // default
int P_user_chemform_pretreat = C_CHEMFORM_LIQUID; // default

/* External global variables / Non-static global variables / Static globals */
extern struct road_temp_vars metdata_;
extern ROPParamsStruct ROP_Params;
extern ChemicalType ChemConc_Params;



/* External functions / Internal global functions / Internal static functions */
extern int init_output_data(int max_site_num, int num_output_fc_days, int num_output_fc_per_day);
extern int write_output(NXNCF_fcst_output *nc_mod_fcst);

extern int init_bridge_sites(char *bridge_site_file_name, int *output_site_array,
                             long num_output_sites, int *bridge);
extern int read_wx_data(char *wx_file_name, int *output_site_array, int num_sites,
			double forc_time, int *num_days, int *num_times, double *fcst_start_time,
			WxFcstTS *wx_fcst_ts);

extern int read_obs_data(char *obs_file_name,
                         int *output_site_array, int num_sites,
                         double *obs_start_time, ObsTS *obs_ts);

extern  int read_layer_info(list <SiteLayerTraffic*> &layer_traffic_list,
			    int *output_site_array,int num_output_sites, LayerInfo *site_layer);

extern int read_last_rc_file(char *last_rc_file, double fcst_time, int num_fcst,
                             int *output_site_array, long num_output_sites,
                             LayerInfo *site_layer, float **last_chem_conc,
                             float **last_nom_chem_conc, float **last_availH2O,
                             float **last_avail_chem, float **last_snow_depth,
                             float **last_chem_type);


extern int read_traffic(list <SiteLayerTraffic*> &layer_traffic_list, 
                        int *output_site_array, long num_output_sites,
                        int **traffic_level, float **route_length, 
                        int **route_lanes, int **route_treatment_time);

extern int parse_treatment_list(char *treatment_def_file_name, int site_id, double forc_time,
                                int num_fcst, int app_rate_units, Treatment *TreatmentList);
extern void init_road_cond(int num_fcst, RoadConditions *roadcond);
extern float calc_local_time_diff(float lon);
extern int calc_road_temp(int treatment_num, int ns, float lat, float lon, int site_id,
			  double forc_time, int curr_hr, int chem_failure_time, 
			  int treatment_option, int metro_run_type, char *use_flux, int num_fcst,
			  int nt_start, int bridge, double obs_start_time,
			  WxFcstTS *wx_fcst_ts, ObsTS *obs_ts,
			  LayerInfo *site_layer, RoadConditions *roadcond);

extern int calc_mobility(int current_hour, int nt_start, int num_fcst, WxFcstTS *wx_fcst_ts,
                         RoadConditions *roadcond, int chem_failure_time);

extern int store_rc_fcst(int ns, int num_output_fc_days, int num_output_fc_per_day, 
                         int nt_start, int num_fcst, int bridge,
                         RoadConditions *roadcond, 
                         float last_chem_type, Treatment *TreatmentPlan, 
                         int treatment_num, int app_rate_units,
			 float rt_len, int rt_lanes);

extern void free_output_data();

int InitSiteObs(ObsTS *curr_obs, ObsTS *obs_ts, double obs_start_time, int ns,
		double forc_time);

int InitSiteWxData( WxForecast *wxfcst, int currHr, 
		    WxFcstTS *wxfcst_list, int ts_offset, int isite, 
		    int Nsteps );

int InitSiteRoadCondData( SiteLayerTraffic *site_config, RoadConditions *roadcond,
			  int *Tlevels, float *latlon_array,
			  int num_fc_per_day, int Toption,
			  float *last_chem_conc, 
			  float *last_nom_chem_conc, float *last_availH2O, 
			  float *last_avail_chem, float *last_snow_depth,
			  float *last_chem_type, int *route_treatment_time,
			  int app_rate_units,
			  WxForecast *wxfcst, WxFcstTS *wxfcst_list, 
			  int currHr, int isite, int Nsteps, int *Cfailure );

void init_wx(WxForecast *wx_fcst, int nh);

extern Log *proc_log;

void DumpSnow( RoadConditions *roadcond);
void DumpPt( RoadConditions *roadcond);
void DumpBS( WxForecast *wxfcst);
//  
//  Routine: road_cond()
//  
//  Description:
//      This module handles the initialization of all internal data structures. It
//  controls the main processing flow for each site depending on the treatment option 
//  it receives. After calculating the road temperatures and snow depths for a site,
//  the treatment option determines whether any treatments are to be considered. 
//
//  Four treatment options are supported and each leads to a variation on the processing. 
//  1. & 2. If the no treatment or the sufficient treatment option has been chosen, 
//  processing is complete for that site. The difference here is that the sufficient 
//  treatment option assumes that all snow is melted by an adequate plan.
//
//  3. If the suggested treatment option has been chosen, the forecast road conditions 
//  are examined to see if any treatment should be recommended. 
//    If a recommended treatment is found, the road conditions forecast is invalid beyond 
//  the treatment time as the snow on the road will either be plowed or melted. Therefore, 
//  the road conditions must be re-calculated from that treatment time on. The latter part 
//  of the time series is then examined to determine whether any further treatments are 
//  recommended. If so, the process is repeated until the end of the forecast period. 
//
//  4. If a user-defined treatment is selected, a configuration file describing the treatment
//  plan is required. After the initial pass of calculating road conditions, the treatment
//  plan is consulted to determine when/if another treatment is to take place. While there
//  are more treatments in the plan, each treatment is applied and the road conditions are 
//  re-calculated from the treatment time forward.
//  
//  Inputs: 
//    forc_time................The unix time of the first road conditions forecast. This time
//                             should be at the top of the hour.
//    site_file_name...........The name of the file containing the list of site where the road 
//                             condition forecasts should be calculated.
//    bridge_site_file_name....File containng list of sites whihc are bridges (could be NULL)
//    treatment_option.........The treatment option in the calculation of the road condition 
//                             forecasts. 0 = no treatment. 1 = suggested treatment. 
//                             2 = user-defined treatment.
//    treatment_def_file_name..The name of the file containing the user-defined treatment. This
//                             is ignored if the treatment option is not user-defined.
//    last_rc_file.............Most recent road condition file for initializing pavement
//    obs_file_name............File containing obs history
//    wx_file_name.............The name of the file containing the weather forecast data.
//    cdl_name.................The name of the cdl file describing the format of the output file.
//    metro_run_type...........Integer indicating how to interface to METRo
//    use_flux.................Option to indicate weather to use forecasted flux values
//    mat_default..............Default material name if site is not explicitly listed in site
//                             config file
//    layer_mat_file...........The name of the file containing all the known road roadway material
//                             types and their attributes.
//    site_config_name.........The name of the file containing site/route specific information
//                             for all the road conditions forecast sites. The file also contains
//                             the subsurface characteristics specific to each site. This includes
//                             the material type and thickness of each layer as well as the node
//                             configuration to be used in the road temperature model.
//    output_name..............The name of the output file to be produced.
//  
//  Outputs:
//    None.
//  
//  Returns:
//    0..................If no fatal errors were found.
//    1..................If a fatal processing condition was encountered.
//

int road_cond(double forc_time, char *site_file_name, 
              char *bridge_site_file_name, int treatment_option, 
              char *treatment_def_file_name, char *last_rc_file, char *obs_file_name,
              char *wx_file_name, char *cdl_name, int metro_run_type, 
	      char *use_flux, char *mat_default,
	      char *site_config_name, char *output_name)
{
  int *output_site_array = 0; 
  float *output_latlon_array = 0; 
  long num_output_sites = 0;
	
  int *bridge = 0;
  int num_bridge_sites = 0;
	
  const char *err_str; 
	
  int i;
  int ns, nf, nt;
  int nt_start = -1;
  int num_fcst = 0;
  int num_output_fc_days = 0;
  int num_output_fc_per_day = 0;
  int num_fc_days = 0;
  int num_fc_per_day = 0;
  int current_hour;
  int roadT_current_hour;
  int treatment_hour;
  int treatment_num;
  int num_user_def_treat;
  int chem_failure_time;
	
  double fcst_start_time, obs_start_time;
	
  float time_offset;
	
  int ret = 0;
  int exit_status = 0;
	
  WxFcstTS *wx_fcst_ts = NULL;
  WxForecast *curr_wxfcst = NULL;

  ObsTS *obs_ts = NULL;
  ObsTS curr_obs;

  RoadConditions *roadcond = NULL;
	
  int *traffic_level = NULL;
  float *route_length = NULL;
  int *route_lanes = NULL;
  int *route_treatment_time = NULL;
	
  LayerInfo *site_layer;
	
  //
  // Define a road condition structure to hold the last set of road conditions from the
  // previous run.
  //
  float *last_chem_conc = NULL;
  float *last_nom_chem_conc = NULL;
  float *last_availH2O = NULL;
  float *last_avail_chem = NULL;
  float *last_snow_depth = NULL;
  float *last_chem_type = NULL;
	
  float initial_chem_type;
	
  // data space specific to a single site
  Treatment *TreatmentList = NULL;
	
  int iplow, ichem;
		
  // Create Site_list obj to hold all site information (location, soild profiles, etc) 
  // for route point observations (bridges are handled seperately below)
  //
  Site_list output_sites(site_file_name);
  if (output_sites.error()) {
    proc_log->write_time("Error: %s", output_sites.error());
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }
	
  num_output_sites = output_sites.make_id_array(&output_site_array);
  if (num_output_sites < 0) {
    proc_log->write_time("Error: failure in call to make_id_array.\n");
    proc_log->write_time("Error: %s", output_sites.error());
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }
	
  if (num_output_sites != output_sites.make_latlon_array(&output_latlon_array))
    {
      proc_log->write_time("Error: could not make_latlon_array\n");
      proc_log->write_time(output_sites.error());
      proc_log->write_time("Ending: exit status = 1.\n");
      return(1);
    }  
	
  // Create bridge list to hold location information for bridge locations
  //
  bridge = new int[num_output_sites];
  num_bridge_sites = init_bridge_sites(bridge_site_file_name, output_site_array,
                                       num_output_sites, bridge);
	
  // Ensure forc_time provided is on the hour
  forc_time = (int)(forc_time / SEC_PER_HOUR) * SEC_PER_HOUR;
	
  // Create Weather Forecast Time Series Object
  // Important NOTE : This data starts at the first time in the forecast file
  // That may not be the same as the start of the 48 hour forecast period.
  wx_fcst_ts = new WxFcstTS[num_output_sites];

  // Create Weather Obs Time Series Object
  // Important NOTE : This data starts at 00Z the day before the forecast.
  obs_ts = new ObsTS[num_output_sites];
	
  NcError* ncErr = new NcError(NcError::silent_nonfatal);
  
  //
  // Parse traffic/layer config file. Store information about
  // each site in a SiteLayerTraffic object.
  //
  list <SiteLayerTraffic*> layer_traffic_list;
	
  if (0 != parse_traffic_layer_config( site_config_name, output_site_array,
				       num_output_sites, mat_default,
				       layer_traffic_list))
    {
      list< SiteLayerTraffic* > :: const_iterator slt;
      for (slt=layer_traffic_list.begin(); slt != layer_traffic_list.end(); slt++)
	delete *slt;

      exit_status = 1;
      return(exit_status);
    }
  
  //
  // Read in the traffic levels for each site and time of day, and 
  // the route specific info from the configuration file
  //
  if (0 != read_traffic( layer_traffic_list , output_site_array, num_output_sites,
                         &traffic_level, &route_length, &route_lanes,
                         &route_treatment_time))
    {
      exit_status = 1;
      return(exit_status);
    }

  //
  // Create and populate the layer info at each site 
  // (with each layer's material properties)
  //
  site_layer = new LayerInfo[num_output_sites];
  
  ret = read_layer_info(layer_traffic_list, output_site_array,
                        num_output_sites, site_layer);
  if (ret != 0)
    {
      exit_status = 1;
      return(exit_status);
    }

  //
  // Now read in weather data from all sites and time periods into a weather
  // forecast time series (wx_fcst_ts)
  //
  ret = read_wx_data(wx_file_name, output_site_array, num_output_sites,
                     forc_time, &num_fc_days, &num_fc_per_day, 
                     &fcst_start_time, wx_fcst_ts);
  if (ret != 0) {
    proc_log->write_time("Error: Unable to read file %s.\n", wx_file_name);
    exit_status = 1;
    return(exit_status);
  }

  //
  // Now read in observational data for all sites to get a history of weather obs
  // for each site.
  //
  ret = read_obs_data(obs_file_name, output_site_array, num_output_sites,
                      &obs_start_time, obs_ts);
  if (ret != 0) {
    proc_log->write_time("Error: Unable to read file %s.\n", obs_file_name);
    exit_status = 1;
    return(exit_status);
  }
  // -----------------------------------------------------------------------
  //
  // Create output file to store all modified data
  //
  NXNCF_fcst_output nc_mod_fcst(cdl_name, output_site_array, num_output_sites,
                                forc_time, output_name);
  if ((err_str = nc_mod_fcst.error()) != 0)	{
    proc_log->write_time("Error: %s.\n", err_str);
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }
	
  /* Get number of output sites dimensioned */
  int max_site_num = nc_mod_fcst.get_dim_size("max_site_num");
  if (max_site_num < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", "max_site_num");
    exit_status = 1;
    return(exit_status);
  }
  
  /* Get number of forecast output times (hours) to compute */
  // This is "num_times" from the cdl and is now used instead
  // of C_NOMINAL_FCST previously defined in RulesOfPractice.hh
  num_fcst = nc_mod_fcst.get_dim_size("num_times");
  //cout << "num_fcst = " << num_fcst << endl;
  if (num_fcst < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", "num_times");
    exit_status = 1;
    return(exit_status);
  }
  
  /* Get number of forecast output days */
  num_output_fc_days = nc_mod_fcst.get_dim_size(DAYS_DIM_NAME);
  if (num_output_fc_days < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", DAYS_DIM_NAME);
    exit_status = 1;
    return(exit_status);
  }
	
  /* Get number of output forecast per day */
  num_output_fc_per_day = nc_mod_fcst.get_dim_size(FC_TIMES_PER_DAY_NAME);
  if (num_output_fc_per_day < 0) {
    proc_log->write_time("Error: Unable to get dimension %s.\n", FC_TIMES_PER_DAY_NAME);
    exit_status = 1;
    return(exit_status);
  }

  // Get the units of the application_rate from the output file. (We can't
  // use NXNCF class for this, as it does not have a method for getting
  // attributes.)

  //char *units_str = new char[80];
  char *units_str;
  NcFile *nc_out = new NcFile(output_name, NcFile::ReadOnly );
  if (NCF_get_var_attr(nc_out, (char *)"application_rate", (char *)"units", &units_str) != 0) {
    proc_log->write_time("Error: Unable to get application_rate units from output file.\n");
    exit_status = 1;
    return(exit_status);
  } 

  // Convert units string to a known enumerated value
  int app_rate_units = GetChemUnits(units_str);

  //----------------------------------------------------------------------

  // Read in the last road data to initialize the chemical/snow data.
  // This routine returns a array of the last values for each site.
  //
  ret = read_last_rc_file(last_rc_file, forc_time, num_fcst,
                          output_site_array, num_output_sites, site_layer,
                          &last_chem_conc, &last_nom_chem_conc, &last_availH2O,
                          &last_avail_chem, &last_snow_depth, &last_chem_type);
  if (ret != 0) {
    proc_log->write_time("Error: Unable to read file %s.\n", last_rc_file);
    exit_status = 1;
    return(exit_status);
  }


  //----------------------------------------------------------------------

  // Initialize output file's data arrays and then write out the current forcast time
  // to start the output data.
  //
  ret = init_output_data(max_site_num, num_output_fc_days, num_output_fc_per_day);
	
  ret = nc_mod_fcst.set_var("forc_time");
  ret = nc_mod_fcst.write_var(&fcst_start_time);
  if (ret < 0) {
    proc_log->write_time("Error: Unable to put data for var %s\n",
			 "forc_time");
    proc_log->write_time("Ending: exit status = 1.\n");
    return(1);
  }
  else
    proc_log->write_time(1,"Info: Forecast time is %.0f\n", (float)forc_time);   
	
  //-------------------------------------------------------------------------
  //
  // Find our forecast time period ("nt_start") in the weather data
  // Match up starting output time to an index in wx data
  //
  for (nf = 0; nf < num_fc_days; nf++)
    for (nt = 0; nt < num_fc_per_day; nt++)
      if (forc_time == 
          fcst_start_time + nf * SEC_PER_DAY + nt * SEC_PER_HOUR)
	{
	  nt_start = nf * num_fc_per_day + nt;
	}
	
  if (nt_start < 0)  // Not found
    {
      proc_log->write_time("Error: Unable to line up input time with wx data.\n");
      exit_status = 1;
      return(exit_status);
    }
	
  // Allocate arrays used in calculations
  // NOTE : These arrays start at first forecast time (hour 0)
	
  // Create storage space for calculation of a single road condition forecast
  roadcond = new RoadConditions[num_fcst+1];
	
  // Create storage space for all potential treatments (at most one per timestep)
  // (In the case of pre-treatments, the code continues to increment the treatment
  // number, so you can have more than num_fcst, hence, add that to the size.)
  TreatmentList = new Treatment[num_fcst+P_PRETREAT_OFFSET];
	
  //
  // Create a weather forecast structure that will be used to hold the local forecast
  // for each site analyzed.
  //
  curr_wxfcst = new WxForecast[num_fcst+1];
  if ( curr_wxfcst == NULL ) {
    proc_log->write_time("Error: Memory allocation of curr_wxfcst failed\n");
    proc_log->write_time("Ending: exit status = 1.\n");
    exit(1);
  }
	
  init_wx(curr_wxfcst, num_fcst+1);

  /******************************************************************/
  //
  // 
  // Loop over all the output sites, updating the road condition data based on the
  // weather forecast and road temperature algorithm. In addition, examine the data to 
  // determine if, when and how much treatment needs to be performed.
  //
  for (ns = 0; ns < num_output_sites; ns++)
    {
      proc_log->write_time("Info: Start site %d %d\n", ns, output_site_array[ns]);

      // Get site config list pointer
      list< SiteLayerTraffic* > :: const_iterator site_config;
      site_config = layer_traffic_list.begin();

      while ( site_config != layer_traffic_list.end() &&
	      (*site_config)->getSiteId() != output_site_array[ns] )
	{
	  site_config++;
	}

      //
      // Initialize weather obs history for this site
      //
      ret = InitSiteObs(&curr_obs, obs_ts, obs_start_time, ns, forc_time);
      if (ret != 0) {
        proc_log->write_time("Error: InitSiteObs() unable to initialize site data.\n");
        return(1);
      }

      // Initialize general local variables
      current_hour = 0;
      roadT_current_hour = 0;
      treatment_hour = num_fcst;
      treatment_num = 0;
      chem_failure_time = C_BAD_VAL;  // the breakdown point is unknown at first
			
      //
      // Initialize weather data for this site
      //
      ret = InitSiteWxData(curr_wxfcst, current_hour, 
			   wx_fcst_ts, nt_start, ns, num_fcst);
      if (ret != 0) {
	proc_log->write_time("Error: InitSiteWxData() unable to initialize site data.\n");
      }

      //
      // Initialize treatment info for this site
      //
      for (i=0; i<num_fcst+P_PRETREAT_OFFSET; i++)
	InitNewTreatment( &TreatmentList[i] );
	

      // Initialize site-specific rules
      ROP_GetParams( *site_config, &ROP_Params );

      //
      // Initialize road condition data for this site, including traffic and subsoil 
      // temperature profile
      //
      ret = InitSiteRoadCondData(*site_config, roadcond, traffic_level,
				 output_latlon_array, num_fc_per_day,
				 treatment_option,
				 last_chem_conc, last_nom_chem_conc, last_availH2O,
				 last_avail_chem, last_snow_depth, last_chem_type,
				 route_treatment_time, app_rate_units,
				 curr_wxfcst, wx_fcst_ts, nt_start, ns, num_fcst,
				 &chem_failure_time);
      if (ret != 0) {
	proc_log->write_time("Error: InitRoadCondData() unable to initialize site data.\n");
      }
			
      //
      // Initialize Chem Conc stuff for this site's default chems
      //
      P_user_chemtype = (*site_config)->getChemType();
      P_user_chemform = (*site_config)->getChemForm();
      P_user_chemtype_pretreat = (*site_config)->getPretreatType();
      P_user_chemform_pretreat = (*site_config)->getPretreatForm();

      ChemConc_InitAndGetParams( P_user_chemtype, P_user_chemform, *site_config, 0, app_rate_units,
				 &ChemConc_Params ); 
      ChemConc_InitAndGetParams( P_user_chemtype_pretreat, P_user_chemform_pretreat, *site_config,
				 1, app_rate_units, &Pretreat_ChemConc_Params );
								

      if (treatment_option == USER_DEFINED_TREATMENT)
	{
	  // Parse treatments here
	  num_user_def_treat = parse_treatment_list(treatment_def_file_name, 
						    output_site_array[ns], forc_time, 
						    num_fcst, app_rate_units, TreatmentList);
	}
			
			
      //
      // Calculate road temperatures for the first time before entering
      // treatment loop.
      //
      //
      // Calcluate road temperatures for user and suggested treatments assuming that 
      // the road surface will
      // generally be cleared by maintenance operations (SUFFICIENT_TREATMENT).
      // 
      switch ( treatment_option )
	{
	case NO_TREATMENT:
	case SUFFICIENT_TREATMENT:
	case USER_DEFINED_TREATMENT:
	  ret = calc_road_temp(99, ns, output_latlon_array[ns*2], output_latlon_array[ns*2+1],
			       output_site_array[ns], forc_time, roadT_current_hour, chem_failure_time,
			       treatment_option, metro_run_type, use_flux, num_fcst, nt_start,
			       bridge[ns], obs_start_time, 
			       &wx_fcst_ts[ns], &curr_obs, &site_layer[ns], roadcond);
	  if (ret > 1)
	    {
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	  else if (ret > 0)
	    {
	      continue;
	    }

	  break;
	case SUGGESTED_TREATMENT:
	  ret = calc_road_temp(99, ns, output_latlon_array[ns*2], output_latlon_array[ns*2+1],
			       output_site_array[ns], forc_time, roadT_current_hour, chem_failure_time,
			       SUFFICIENT_TREATMENT, metro_run_type, use_flux, num_fcst, nt_start,
			       bridge[ns], obs_start_time,
			       &wx_fcst_ts[ns], &curr_obs, &site_layer[ns], roadcond);
	  if (ret > 1)
	    {
	      proc_log->write_time("Ending: exit status = 1.\n");
	      exit(1);
	    }
	  else if (ret > 0)
	    {
	      continue;
	    }

	  break;
	}
      //
      // If no last road cond file was available then we weren't able to initialize
      // the chem concentration. Do this now because we have run the road temp algorithm
      //
      chem_failure_time = AdjustChemicalConcentration(current_hour, num_fcst, 
						      NULL, curr_wxfcst, 
						      roadcond );
      if (chem_failure_time == -999)  // Flag indicating no breakdown
	chem_failure_time = num_fcst;

      int jplow = FindPlowStartTime( current_hour, roadcond, num_fcst );
      if (jplow != C_BAD_VAL && chem_failure_time > jplow ) { // Do we need to plow before chemicals?
	current_hour = jplow;
      }
      else {
	current_hour = chem_failure_time;
      }

      // 
      // Characterize storm situation for this site for use in determining treatments.
      //
      StormType StormSummary;
      int status;
      status = CharacterizeStorm( roadcond, curr_wxfcst, &StormSummary, num_fcst );
      proc_log->write_time(1, "Info: %s\n", StormSummary.StormTypeText.c_str());
      PrintStormType( &StormSummary );
			
      if ( treatment_option != USER_DEFINED_TREATMENT ) {
	if ( StormSummary.BeginLookout != C_BAD_VAL ) {
	  current_hour = current_hour;
	}
	else {
	  current_hour = num_fcst+1;
	}
      }
      else { // USER_DEFINED_TREATMENT 
	if ( num_user_def_treat == 0 )
	  current_hour = num_fcst;
	else
	  current_hour = 0;

      }

      // Calculate Mobility based on predicted road conditions
      calc_mobility(roadT_current_hour, nt_start, num_fcst, &wx_fcst_ts[ns], 
		    roadcond, chem_failure_time);
			
      // MAIN ROAD CONDITION CALCULATION LOOP
      // Calculate road variables time series (and treatments).
      // If treatments occur, time series must be recalculated from that point on
      while (current_hour < num_fcst-1)
	{
	  int treatment_needed = 0;
					
	  //
	  // Determine if any treatments need to be done.
	  //
	  switch (treatment_option)
	    {
	    case NO_TREATMENT:
	    case SUFFICIENT_TREATMENT:
	      {
		// We're done. Set required end flags,etc.
								
		treatment_hour = num_fcst;
		treatment_needed = C_NOTREAT_NONEED;
		break;
	      }
	    case SUGGESTED_TREATMENT:
	      {

		// Initialize next treatment
		InitNewTreatment( &TreatmentList[treatment_num] );
								
		Treatment *LastTreatment = NULL;
								
		// Initialize last treatment
		for (int ii=treatment_num-1; ii>=0 && LastTreatment == NULL; ii--) {
		  if ( TreatmentList[ii].TreatType > C_TREAT_THRESH ) {
		    LastTreatment = &TreatmentList[ii];
		    //	cout << "Check last treat " << TreatmentList[ii].TreatType << endl;
		  }
		}
								
		// Determine if another treatment is necessary.
								
		proc_log->write_time(1, "Info: Check for additional treatment at hour: %d\n", current_hour);
								
		//
		// Determine what, if any, treatments are needed (including pre-treating,
		// chemicals and plow-only
		//
		// cout << current_hour << " " << treatment_num << " " << "call determine treatment\n";
		treatment_needed = StormSumDetermineTreatment(current_hour, num_fcst, 
							      curr_wxfcst, roadcond,
							      &StormSummary, 
							      &TreatmentList[treatment_num], 
							      LastTreatment, num_fcst);

		proc_log->write_time(1, "Info: Treatment type needed: %d\n", treatment_needed);

		if (treatment_needed < C_TREAT_THRESH) {
		  // No triggers were found in the remaining forecast hours.
		  //treatment_hour = num_fcst; // No more treatment needed
		  treatment_hour = TreatmentList[treatment_num].EffectiveTime;
		}
		else {
		  // There is some kind of treatment -- could be plow-only 
		  // or chemical treatment
									
		  // Ensure treatment timing is set relative to forecast start,
		  // NOT time since last treatment
		  treatment_hour = TreatmentList[treatment_num].StartTime;

									
		  //
		  // Calculate the average amount of snow that will be removed over the plow
		  // period. The first hour will be removed down to minimums, but future
		  // hours will show snow on the road
		  //
		  float avgSnow = 	roadcond[treatment_hour].PsnowOnRoad + 
		    (roadcond[TreatmentList[treatment_num].EffectiveTime].PsnowOnRoad - 
		     roadcond[treatment_hour].PsnowOnRoad) / 2.0;
		  if ( avgSnow < roadcond[treatment_hour].PsnowOnRoad )
		    avgSnow = roadcond[treatment_hour].PsnowOnRoad;
		  //
		  // PsnowOnRoad is a cumulative snow, but calc_road_temp handles accumulating
		  // snow after plow period 
		  //
		  for ( iplow=treatment_hour; iplow<=TreatmentList[treatment_num].EndTime; iplow++) {
		    float tmpSnow = roadcond[iplow].PsnowOnRoad - avgSnow;
		    if (tmpSnow < SNOW_DEPTH_AFTER_PLOW) {
		      roadcond[iplow].PsnowOnRoad = tmpSnow;
		    }
		    else {
		      roadcond[iplow].PsnowOnRoad = SNOW_DEPTH_AFTER_PLOW;
		    }
		  }
		}
		treatment_num++;
		//
		break;
	      }
	    case USER_DEFINED_TREATMENT:
	      {
		if ( treatment_num < num_user_def_treat ) 
		  {
		    // Use the next treatment (if any) for configuration file
		    P_user_chemtype = TreatmentList[treatment_num].Chemicals.UserChemType;
		    P_user_chemform = TreatmentList[treatment_num].Chemicals.UserChemForm;

		    if ( TreatmentList[treatment_num].ApplyChemicals == 0 )
		      treatment_needed = C_TREAT_PLOWONLY;
		    else 
		      treatment_needed = C_TREAT;

		    // if no chemical specified them use a default just for the purposes of
		    // initialization. This likely means a plow only operation
		    if ( P_user_chemtype == 0 )
		      P_user_chemtype = P_USER_CHEMTYPE;
										
		    // These need to be re-called in case chemicals change
		    ROP_GetParams( *site_config, &ROP_Params );
		    ChemConc_InitAndGetParams( P_user_chemtype, P_user_chemform, *site_config, 0, app_rate_units, &ChemConc_Params ); 

		    // I don't think we need this? JRC 
		    // ChemConc_InitAndGetParams( P_user_chemtype_pretreat, P_user_chemform_pretreat, *site_config, 1, app_rate_units, &Pretreat_ChemConc_Params ); 
		    TreatmentList[treatment_num].Chemicals = ChemConc_Params;
										
		    // Determine the end time of the expected treatment
		    TreatmentList[treatment_num].EndTime = TreatmentList[treatment_num].StartTime + 
		      (int)(0.5+(ROP_Params.RouteRoundtripTiming+ROP_Params.TruckTurnAroundTime)/60.0);

		    if ( TreatmentList[treatment_num].EndTime > num_fcst )
		      TreatmentList[treatment_num].EndTime = num_fcst;

		    if ( treatment_num+1 >= num_user_def_treat )
		      TreatmentList[treatment_num].EffectiveTime = num_fcst; 
		    else if ( TreatmentList[treatment_num+1].StartTime < TreatmentList[treatment_num].EndTime+1 )
		      TreatmentList[treatment_num].EffectiveTime = TreatmentList[treatment_num+1].StartTime; 
		    else
		      TreatmentList[treatment_num].EffectiveTime = TreatmentList[treatment_num+1].EndTime+1; 
										
		    //chem_used = P_user_chemtype;
										
		    treatment_hour = TreatmentList[treatment_num].StartTime;
		    //treatment_needed = C_TREAT;
											
		    //
		    // Calculate the average amount of snow that will be removed over the plow
		    // period. The first hour will be removed down to minimums, but future
		    // hours will show snow on the road
		    //
		    float avgSnow = 	roadcond[treatment_hour].PsnowOnRoad + 
		      (roadcond[TreatmentList[treatment_num].EffectiveTime].PsnowOnRoad - 
		       roadcond[treatment_hour].PsnowOnRoad) / 2.0;
		    if ( avgSnow < roadcond[treatment_hour].PsnowOnRoad )									
		      avgSnow = roadcond[treatment_hour].PsnowOnRoad;
										
		    //
		    // PsnowOnRoad is a cumulative snow, but calc_road_temp handles accumulating
		    // snow after plow period 
		    //
		    for ( iplow=treatment_hour; iplow<=TreatmentList[treatment_num].EndTime; iplow++) {
		      float tmpSnow = roadcond[iplow].PsnowOnRoad - avgSnow;
		      if (tmpSnow < SNOW_DEPTH_AFTER_PLOW) {
			roadcond[iplow].PsnowOnRoad = tmpSnow;
		      }
		      else {
			roadcond[iplow].PsnowOnRoad = SNOW_DEPTH_AFTER_PLOW;
		      }
		    }
								
		    treatment_num++;
		  }
		else
		  {
		    current_hour = num_fcst;
		    treatment_hour = num_fcst;
		    treatment_needed = C_NOTREAT_NONEED;
		  }

		break;
	      }
	    default:
	      {
		proc_log->write_time("Error: Undefined Treatment Option %d specified\n", 
				     treatment_option);
		proc_log->write_time("Ending: exit status = 1.\n");
		exit(1);
	      }
	    }
					
	  // Calculate Chemical Concentration (based on chemical application) and
	  // determine when the chemicals will fail (to melt the falling snow)
	  //
	  // We may want to call this whether a treatment is recommeded or not, chem dilution may 
	  // change due to previous temperature changes. For now only call when a treatment is 
	  // recommended.
	  if ( treatment_needed > C_TREAT_THRESH ) {
	    // Some type of treatment need to adjust chemicals and/or road conditions
	    // Calculate updated chemical state time series and chemical failure time
	    chem_failure_time = AdjustChemicalConcentration(treatment_hour, num_fcst, 
							    &TreatmentList[treatment_num-1], 
							    curr_wxfcst, roadcond );
	    // Need to determine the "effective" time of the applied treatment based on the treatment type
	    // For chemical applications, use chem_failure_time, for plow only use treatment start time of 
	    // treatment (chemicals weren't effective)
	    if ( TreatmentList[treatment_num-1].TreatType == C_TREAT_PLOWONLY )
	      chem_failure_time = TreatmentList[treatment_num-1].EndTime;  // 
						
	    if (chem_failure_time == -999) // Flag indicating no breakdown
	      chem_failure_time = num_fcst;
						
	    if (TreatmentList[treatment_num-1].ApplyChemicals == TRUE) {
	      // Assume chemicals on road melt remaining snow
	      for ( ichem=treatment_hour; ichem <= chem_failure_time; ichem++) {
		roadcond[ichem].PsnowOnRoad = 0.;
	      }
	    }
	    //        
	    if ( treatment_option == USER_DEFINED_TREATMENT )
	      current_hour = TreatmentList[treatment_num-1].StartTime+1;
	    else {
	      if ( ROP_Params.TreatmentStrategy == C_CONTINUOUS ) {
		if ( TreatmentList[treatment_num-1].EndTime+1 > current_hour )
		  current_hour = TreatmentList[treatment_num-1].EndTime+1;
		else
		  current_hour++;
	      }
	      else {								
		if ( chem_failure_time == num_fcst ) {
		  // Chemical breakdown not expected but need to check if plowing will be needed
		  int jplow = FindPlowStartTime( current_hour, roadcond, num_fcst );
		  if ( jplow != C_BAD_VAL ) {
		    current_hour = jplow;
		  }
		}
		current_hour = chem_failure_time;

	      }
	    }
						
	    roadT_current_hour = TreatmentList[treatment_num-1].StartTime;
						
	    proc_log->write_time(1, "Info: Start road temp model for site %d, treatment_num %d, current hr %d, chem eff_hr %d\n", 
				 ns, treatment_num, roadT_current_hour, chem_failure_time);
	    // Calculate Road Temperature Time Series
	    if (current_hour < num_fcst)
	      {
		ret = calc_road_temp(treatment_num, ns, output_latlon_array[ns*2],
				     output_latlon_array[ns*2+1], output_site_array[ns], forc_time,
				     roadT_current_hour, chem_failure_time, treatment_option,
				     metro_run_type, use_flux, num_fcst, nt_start, bridge[ns], 
				     obs_start_time, &wx_fcst_ts[ns], &curr_obs, &site_layer[ns],
				     roadcond);
		if (ret > 1)
		  {
		    proc_log->write_time("Ending: exit status = 1.\n");
		    exit(1);
		  }
		else if (ret > 0)
		  {
		    break;
		  }
	      }

	    // DumpPt(roadcond);
						
	    // Calculate Mobility based on predicted road conditions with any expected treatment
	    calc_mobility(roadT_current_hour, nt_start, num_fcst, &wx_fcst_ts[ns], roadcond, chem_failure_time);
	  }
	  else if (treatment_option == SUGGESTED_TREATMENT) {
	    current_hour       = TreatmentList[treatment_num-1].EffectiveTime;
	    roadT_current_hour = TreatmentList[treatment_num-1].EffectiveTime;
	  }
	  else {
	    current_hour       = num_fcst;
	    roadT_current_hour = num_fcst;
	  }
					
	}

      // Check if time loop exited prematurely
      if (current_hour < num_fcst-1)
	continue;


      StormRoadWaterPhase( nt_start, roadcond, curr_wxfcst, num_fcst );
			
      PrintTreatmentPlan( TreatmentList, treatment_num, 1.0, (char *)"lbs/lane-mile" );

      // Store this site's data in output arrays
      store_rc_fcst(ns, num_output_fc_days, num_output_fc_per_day, nt_start,
		    num_fcst, bridge[ns], roadcond,
		    initial_chem_type, TreatmentList, treatment_num,
		    app_rate_units,
		    route_length[ns],
		    route_lanes[ns]);

    }

  proc_log->write_time(1, "Info: Writing output.\n");

  //
  // Write everything to the output file
  //
  write_output(&nc_mod_fcst);

  //
  // Clean up
  //
  delete [] units_str;
  delete [] output_site_array;
  delete [] output_latlon_array;
  delete [] bridge;
  delete [] obs_ts;
  delete [] site_layer;
  delete [] roadcond;
  delete [] TreatmentList;
  delete [] curr_wxfcst;
  delete [] traffic_level;
  delete [] route_length;
  delete [] route_lanes;
  delete [] route_treatment_time;
  delete [] last_chem_conc;
  delete [] last_nom_chem_conc;
  delete [] last_availH2O;
  delete [] last_avail_chem;
  delete [] last_snow_depth;
  delete [] last_chem_type;

  for (ns=0; ns<num_output_sites; ns++)
    delete [] wx_fcst_ts[ns].fcst;
  delete [] wx_fcst_ts;

  list< SiteLayerTraffic* > :: const_iterator slt;
  for (slt=layer_traffic_list.begin(); slt != layer_traffic_list.end(); slt++)
    delete *slt;

  free_output_data();

  delete nc_out;
  delete ncErr;

  return(exit_status);
}


int InitSiteRoadCondData( SiteLayerTraffic *site_config, RoadConditions *roadcond,
			  int *Tlevels, float *latlon_array,
			  int num_fc_per_day, int Toption, 
			  float *last_chem_conc, 
			  float *last_nom_chem_conc, float *last_availH2O, 
			  float *last_avail_chem, float *last_snow_depth,
			  float *last_chem_type, int *route_treatment_time,
			  int app_rate_units,
			  WxForecast *wxfcst, WxFcstTS *wxfcst_list, 
			  int currHr, int isite, int Nsteps, int *Cfailure )
{
  int   i;
  float time_offset, initial_chem_type;

  // Reset roadway characteristics for this site
  init_road_cond(Nsteps, roadcond);
  
  // Set up traffic data for this site
  // Find local time
  time_offset = calc_local_time_diff(latlon_array[isite*2+1]);
  time_offset /= SEC_PER_HOUR;
  
  // Get traffic data for forecast period
  for (i=0; i<Nsteps; i++) {
    int tr_offset = isite*HOUR_PER_DAY;
    tr_offset += ((currHr+i+(int)time_offset)%num_fc_per_day);
    if (tr_offset < 0)
      tr_offset += num_fc_per_day;
    roadcond[i].TrafficIntensity = Tlevels[tr_offset];
  }
  
  // Initialize from previous files (where possible)
  if (last_snow_depth == NULL || Toption == SUFFICIENT_TREATMENT)  {
    // No previous file. Assume no chemicals or snow on road.
    roadcond[0].PsnowOnRoad = 0.;
    roadcond[0].ChemSolutionType = P_USER_CHEMTYPE;
    initial_chem_type = P_USER_CHEMTYPE;
    P_user_chemtype   = P_USER_CHEMTYPE;
  }
  else {
    if (last_snow_depth[isite] != NC_FILL_FLOAT)
      roadcond[0].PsnowOnRoad = last_snow_depth[isite];
    if ( roadcond[0].PsnowOnRoad > 0.0 )
      roadcond[0].roadWphase = C_RTYPE_SNOW;
    initial_chem_type = last_chem_type[isite];
    if ( initial_chem_type == C_CHEMTYPE_UNKNOWN ) {
      roadcond[0].ChemSolutionType = P_USER_CHEMTYPE;  // for now
    }
    else {
      roadcond[0].ChemSolutionType = (int)initial_chem_type;
    }

    // 
    // If there is no snow on the road and chemicals have already been used then 
    // check to see if chemicals are effective.
    //
    if (roadcond[0].PsnowOnRoad == 0. ) 
      {
	// No snow left. If chemicals are left, set initial value, determine 
	// chem concentration time series, and effective time
	if (last_chem_conc[isite] != NC_FILL_FLOAT)
	  roadcond[0].FinalChemConc = last_chem_conc[isite];
	if (last_nom_chem_conc[isite] != NC_FILL_FLOAT)
	  roadcond[0].NominalChemConc = last_nom_chem_conc[isite];
	if (last_avail_chem[isite] != NC_FILL_FLOAT)
	  roadcond[0].AvailableChem = last_avail_chem[isite];
	if (last_availH2O[isite] != NC_FILL_FLOAT) 
	  roadcond[0].AvailH2O = last_availH2O[isite];

	if ( last_chem_type[isite] != 0 )
	  P_user_chemtype = (int)last_chem_type[isite];

	ChemConc_InitAndGetParams( P_user_chemtype, P_user_chemform, site_config, 0, app_rate_units, &ChemConc_Params ); 

      }
    else
      {
	//
	// There is snow on the road with/without chemicals OR there is snow and no chemicals
	// 
	// These chem settings were omitted for some reason, hence there was no
	// knowledge of previous treatments when there is still snow on the road.
	// Seems to positively impact the timing of the first treatment or two.
	// (Cowie 1/14/13)
	//
	if (last_chem_conc[isite] != NC_FILL_FLOAT)
	  roadcond[0].FinalChemConc = last_chem_conc[isite];
	if (last_nom_chem_conc[isite] != NC_FILL_FLOAT)
	  roadcond[0].NominalChemConc = last_nom_chem_conc[isite];
	if (last_avail_chem[isite] != NC_FILL_FLOAT)
	  roadcond[0].AvailableChem = last_avail_chem[isite];
	//
	// Need to set available water to last setting, if there is still water there..
	//
	if (last_availH2O[isite] != NC_FILL_FLOAT && last_availH2O[isite] > 0.0)
	  roadcond[0].AvailH2O = last_availH2O[isite];
	else { // Here we have snow on the road, but no available water. Since snow depth and
	  // availabale water are decoupled (the snow rate is used to calculate available water)
	  // if no treatment is done, the water is still assumed to go away at some point.
	  // Originally recalculated the initial availH20 like all the snow just fell in one hour.
	  // But, this leads to unrealistic water levels when snow piles up. Instead put just enough water 
	  // on the road to trigger a treatment if necessay.
	  roadcond[0].AvailH2O    = 1.1*C_AVAILH2O_THRESH;

	  if ( last_chem_type[isite] != 0 )
	    P_user_chemtype = (int)last_chem_type[isite];

	  ChemConc_InitAndGetParams( P_user_chemtype, P_user_chemform, site_config, 0, app_rate_units, &ChemConc_Params ); 

	}
      }
    *Cfailure = AdjustChemicalConcentration(1, Nsteps, NULL, wxfcst, roadcond);
  }
  return(0);  
}


int InitSiteWxData( WxForecast *wxfcst, int currHr, WxFcstTS *wxfcst_list, 
		    int ts_offset, int isite, int Nsteps )
{
  int i;
  // Initialize weather data for this site
  //
  // Local version of weather conditions 
  //
  // Fill 0th index with previous hour's weather where possible,
  // if not available, then fill it with the first forecast hour's weather
  //
  //if ( ts_offset > 0 )
  //{
  //  wxfcst[0] = wxfcst_list[isite].fcst[ts_offset+currHr-1];
  //  if (!verify_wxfcst( &wxfcst[0]))
  //	wxfcst[0] = wxfcst_list[isite].fcst[ts_offset+currHr];
  //}
  //else
  //{
  //  wxfcst[0] = wxfcst_list[isite].fcst[ts_offset+currHr];
  //}

  //
  // Fill the rest of the local weather forecast for this site and start time

  for (i=0; i<Nsteps; i++) {

    // There does not seem to be any reason why we would fill the 0th element
    // with past weather. That causes the weather and roadcond array to be
    // mis-alligned. So just fill it normally. (Cowie 2/13)

    //wxfcst[i+1] = wxfcst_list[isite].fcst[ts_offset+currHr+i];
    wxfcst[i] = wxfcst_list[isite].fcst[ts_offset+currHr+i];
  }


  
  return(0);
} 

void DumpPt(RoadConditions *roadcond)
{
  int j, i;

  cout << "Dump Pavement Temperature Values\n";
  for(j=0;j<4;j++) {
    int istart = j*12;
    for(i=istart;i<istart+12;i++) {
      printf("%5.2f ",roadcond[i].road_T);
    }
    printf("\n");
  }
}
void DumpSnow(RoadConditions *roadcond)
{
  int j, i;

  cout << "Dump Snow Depth Values\n";
  for(j=0;j<4;j++) {
    int istart = j*12;
    for(i=istart;i<istart+12;i++) {
      printf("%5.2f ",roadcond[i].PsnowOnRoad);
    }
    printf("\n");
  }
}
void DumpBS(WxForecast *wxfcst)
{
  int j, i;

  cout << "Dump Blowing Snow Index Values\n";
  for(j=0;j<6;j++) {
    int istart = j*8;
    for(i=istart;i<istart+8;i++) {
      printf("%5.2f(%d) ",wxfcst[i].blowing_snow_pot_vals, wxfcst[i].blowing_snow_potential);
    }
    printf("\n");
  }
}

int InitSiteObs(ObsTS *curr_obs, ObsTS *obs_ts, double obs_start_time, int ns, 
		  double forc_time)
{
  int i;
  int offset;

  offset = (int)(forc_time - obs_start_time)/SEC_PER_HOUR;
  //printf("offset %d\n", offset);

  if (offset < NUM_OBS_HRS)
    return(1);

  // Copy obs starting at NUM_OBS_HRS from fcst_time into struct
  for (i=0; i<NUM_OBS_HRS; i++)
    {
      int index = offset+i-NUM_OBS_HRS+1;
      curr_obs->obs[i].T = obs_ts[ns].obs[index].T;
      curr_obs->obs[i].wspd = obs_ts[ns].obs[index].wspd;
      curr_obs->obs[i].dewpt = obs_ts[ns].obs[index].dewpt;
      curr_obs->obs[i].Precip = obs_ts[ns].obs[index].Precip;
      curr_obs->obs[i].road_T = obs_ts[ns].obs[index].road_T;
      curr_obs->obs[i].bridge_T = obs_ts[ns].obs[index].bridge_T;
      curr_obs->obs[i].road_condition = obs_ts[ns].obs[index].road_condition;
      curr_obs->obs[i].subsurface_T = obs_ts[ns].obs[index].subsurface_T;
    }

  return(0);
}
