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
 * Module: metro_direct.cc
 *
 * Author: Jim Cowie
 *
 * Date:   12/20/2012
 *
 * Description:
 *
 *   This file contains functions used to call the METRo heat balance model
 *   directly. The main function is metro_direct().
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
#include <gtime/calc_sun.hh>
#include <metro/macadam.h>
#include "Obs.hh"
#include "layer_info.hh"
#include "RulesOfPractice.hh"
#include "RoadCond.hh"
#include "Utilities.hh"

extern Log *proc_log;
extern int debug_level;;

// METRo constants from metro_constant.py (may not all be used)

double fConsol = 0.1367e4;
double fTimeStep = 30;
double nSnowWaterRatio = 10;
double lCloudsDay[] = {1.0, 0.97, 0.94, 0.89, 0.85, 0.80, 0.71, 0.65, 0.33};
double lCloudsNightCoeff1[] = {3.79, 4.13, 4.13, 4.26, 4.38, 4.19, 4.395,
			       4.34, 4.51};
double lCloudsNightCoeff2[] = {214.7, 226.2, 234.8, 243.4, 250.7,
			       259.2, 270.9, 280.9, 298.4};
double nRoadTemperatureMin = -50;  // originally -40 in METRo
double nRoadTemperatureHigh = 80;
double nSubSurRoadTmpHigh = 80;
double nSubSurRoadTmpMin = -40;
double nAirTempHigh = 50;
double nAirTempMin = -60;
double nMaxWindSpeed = 90;
double nHourForExpirationOfObservation = 48;
double nGapMinuteObservation = 240;
double nThreeHours = 3;
double nLowerPressure = 600; // originally 900 in METRo
double fNormalPressure = 1013.25; // 1 atmosphere
double nUpperPressure = 1100;

double fConst = 1. / ( 4.0 * 3.6E3 );

double Eps1 = 0.62194800221014;
double Eps2 = 0.3780199778986;
double fTrpl = 273.16;
double fTcdk = 273.15;

// end METRo constants

double noPrecip = 0; // DO NOT USE
double rain = 1;
double snow = 2;


// Function used in AH computation
inline float sign(float v1, float v2) {return ((v2 >= 0) ? fabs(v1) : -fabs(v1));};

//-----------------------------------------------------------------------------
//  
//  Routine: absoluteHumidity()
//  
//  Description:
//    Compute absolute humidity. The algorithm comes from the METRo code in
//    metro_physics.py. Note that the METRO calculation forces the sfcPres
//    between 900 and 1100 mb, and if outside this range, sets it to the
//    standard SLP, 1013mb. We are not doing that here.
//
//  Inputs:
//    dewPoint ....... dew point temperature, deg C
//    sfcPres ........ surface pressure, Pa
//
//  Returns:
//    absolute humidity value
//
//-----------------------------------------------------------------------------

double absoluteHumidity(double dewPoint, double sfcPres)
{
  double absHum;
  double tdk = dewPoint + fTcdk;
  double dDiff = tdk - fTrpl;
  double vap_pr = 610.78 *
    exp(fmin(sign(17.269,dDiff),sign(21.875,dDiff)) *
	fabs(dDiff)/(tdk-35.86+fmax(0.0,sign(28.2,-dDiff))));
  
  absHum = Eps1/(fmax(1.0, sfcPres/vap_pr) - Eps2);
  return(absHum);
}

//-----------------------------------------------------------------------------
//  
//  Routine: metroPrecipType()
//  
//  Description:
//    Determines precip type code for METRo.
//
//  Inputs:
//    precipType ....... RWFS precip type code
//    precipAmt ........ RWFS precip amount, meters
//    treatmentOption .. treatment type number
//    fcstHour .. ...... hour of forecast
//    chemFailHour ..... hour chems fail
//    airTemp .......... air temperature, deg C
//
//  Returns:
//    0 - no precip (NOT USED, caused segfault)
//    1 - rain
//    2 - snow
//
//-----------------------------------------------------------------------------

double metroPrecipType(int precipType, double precipAmt, int treatmentOption,
		       int fcstHour, int chemFailHour, double airTemp)
{
  double metroType;
  
  if (precipAmt > 0)  // there is precip
    {
      //
      // Previously, we forced snow to rain under certain conditions, but now
      // just leave it as-is.
      //
      //if (treatmentOption == SUFFICIENT_TREATMENT)
      //{
      //  metroType = rain;  // force snow to rain
      //}
      //else if (fcstHour < chemFailHour && precipType == C_PTYPE_SNOW)
      //{
      //  metroType = rain;  // turn snow to rain because melted by chems (??)
      //}
      //else if (precipType == C_PTYPE_RAIN || precipType == C_PTYPE_FRZRAIN)


      // Convert freezing rain to rain
      if (precipType == C_PTYPE_RAIN || precipType == C_PTYPE_FRZRAIN)
	{
	  metroType = rain;  // this is rain
	}
      else if (precipType == C_PTYPE_SNOW)
	{
	  metroType = snow;  // this is snow
	}
      else
	{
	  metroType = rain;  // must be rain 
	}
    }
  else      // No precip, metroType is based on temperature
    {	  
      if (airTemp > 0.)
	metroType = rain;
      else
	metroType = snow;
    }

  return(metroType);
}

//-----------------------------------------------------------------------------
//  
//  Routine: infraredFlux()
//  
//  Description:
//    Compute the IR flux. Algorithm from metro_physics.py.
//
//  Inputs:
//    cloudOctal ....... octal cloud cover (0-8)
//    airTemp .......... air temp, deg C
//
//  Returns:
//    IR flux, W/m^2
//
//-----------------------------------------------------------------------------

double infraredFlux(int cloudOctal, double airTemp)
{
  double coeff1, coeff2;

  coeff1 = lCloudsNightCoeff1[cloudOctal];
  coeff2 = lCloudsNightCoeff2[cloudOctal];

  return(coeff1 * airTemp + coeff2);
}

//-----------------------------------------------------------------------------
//  
//  Routine: solarFlux()
//  
//  Description:
//    Compute the solar flux. Algorithm adapted from metro_physics.py.
//
//  Inputs:
//    cloudOctal ....... octal cloud cover (0-8)
//    fcstStartTime .... start time of forecast, UNIX time
//    fcstHour ......... hour of forecast
//    lat .............. latitude
//    lon .............. longitude
//
//  Returns:
//    solar flux, W/m^2
//
//-----------------------------------------------------------------------------

double solarFlux(int cloudOctal, double fcstStartTime, double fcstHour,
		 double lat, double lon)
{
  //
  // Get sunrise, sunset Unix times. This function is comparable to the METRo 
  // third-party Sun.py version, within a minute.
  //
  double sunRise, sunSet;
  calc_sun2(lat, lon, fcstStartTime, sunRise, sunSet);

  //
  // For a discernable sunrise/sunset (sunries != sunset), check for night
  // time, in which case return with 0 flux. If sunrise = sunset, there is
  // no rise or set (polar regions) so we allow the code below to figure
  // out the solar flux (if any).
  //
  int nCurrentHour = (int)fcstHour%24;
  if (sunRise != sunSet)
    {
      double sunRiseHour, sunSetHour;
      sunRiseHour = int(sunRise/3600)%24 + (int(sunRise/60)%60)/60. +
	(int(sunRise)%60)/3600.;
      sunSetHour = int(sunSet/3600)%24 + (int(sunSet/60)%60)/60. +
	(int(sunSet)%60)/3600.;
      
      if ((sunSetHour > sunRiseHour &&
	   (nCurrentHour < sunRiseHour || nCurrentHour > sunSetHour)) ||
	  (sunRiseHour > sunSetHour &&
	   (nCurrentHour > sunSetHour && nCurrentHour < sunRiseHour)))
	return (0);
    }

  //
  // Convert time to tm struct, compute fractional day of year
  //
  time_t uTime = (time_t)fcstStartTime;
  tm *tms = gmtime(&uTime);
  double fJulianDate = tms->tm_yday+1 + tms->tm_hour/24.0;
  double fDivide;
  if (tms->tm_year%4 == 0)
    fDivide = 366.0;
  else
    fDivide = 365.0;

  double fA = fJulianDate/fDivide*2*M_PI;

  //
  // Compute maximum theoretical solar flux for this day of year
  // regardless of latitude (accounts for orbital eccentricity)
  //
  double fR0r = 1.0/pow(1.0-9.464e-4*sin(fA)-0.01671*cos(fA)- 
			+ 1.489e-4*cos(2.0*fA)-2.917e-5*sin(3.0*fA)-
			+ 3.438e-4*cos(4.0*fA),2) * fConsol;
  //
  // Compute earth-sun angles
  //
  double fRdecl = 0.412*cos((fJulianDate+10.0)*2.0*M_PI/fDivide-M_PI);
  double fDeclsc1 = sin(lat*M_PI/180.0)*sin(fRdecl);
  double fDeclsc2 = cos(lat*M_PI/180.0)*cos(fRdecl);
  
  //
  // Compute time adjustment (not clear if this is needed or overkill)
  //
  double fEot = 0.002733 -7.343*sin(fA)+ .5519*cos(fA) -9.47*sin(2.0*fA)
    -3.02*cos(2.0*fA) -0.3289*sin(3.*fA) -0.07581*cos(3.0*fA)
    -0.1935*sin(4.0*fA) -0.1245*cos(4.0*fA);
  
  fEot = fEot/60.0;           // minutes to fraction of hour
  fEot = fEot*15*M_PI/180.0;  // to radians

  //
  // Compute max flux for this time of day
  //
  double fDh =  M_PI*(nCurrentHour/12.0 + lon/180 - 1) + fEot;
  double fCosz = fDeclsc1 + fDeclsc2*cos(fDh);
  double npSft = fmax(0.0, fCosz)*fR0r;
  
  //
  // This factor appears to reduce the flux, perhaps due to absorption
  // and reflectance.
  //
  double npCoeff = -1.56e-12*pow(npSft,4) + 5.972e-9*pow(npSft,3) -
    8.364e-6*pow(npSft,2)  + 5.183e-3*npSft - 0.435;

  //
  // Compute and return final solar flux
  //
  return(fmax(0.0, npSft * npCoeff * lCloudsDay[cloudOctal]));

}

//-----------------------------------------------------------------------------
//  
//  Routine: interp()
//  
//  Description:
//    Interpolates the data values in 'a' of size 'size' to a finer resolution
//    based on 'increment' and returns the interpolated array. If 'nearest' is
//    set, nearest neighbor interpolation is performed, otherwise linear.
//
//  Inputs:
//    a ................ array containing original data points
//    size ............. size of a
//    increment ........ number subdivisions to make between each a value
//    nearest .......... nearest neighbor interp flag
//
//  Returns:
//    double array of interpolated values
//
//  Note:
//    memory is allocated for new array here. 
//  
//-----------------------------------------------------------------------------

double *interp(double *a, int size, int increment, int nearest)
{
  int outSize = (size-1)*increment;
  double *a2 = new double[outSize];
  double step;
  int index=0;
  int ind1, ind2;

  // Initialize output array
  for (int i=0; i<outSize; i++)
    a2[i] = METRO_MISSING_VALUE;

  // start outer loop
  ind1 = 0;
  ind2 = 0;
  while (ind1 < size-1)
    {
      // keep going if value is missing
      if (a[ind1] == METRO_MISSING_VALUE)
	{
	  // Incremennt index until we have first non-missing value
	  if (ind2 == 0)
	    index += increment;

	  ind1++;
	  continue;
	}

      // find next non-missing value
      ind2 = ind1 + 1;
      while (ind2 < size && a[ind2] == METRO_MISSING_VALUE)
	ind2++;

      // make sure indexes are valid
      if (ind1 < size-1 && ind2 < size)
	{
	  for (int j=0; j<increment*(ind2-ind1); j++)
	    {
	      if (nearest)
		if (j<increment*(ind2-ind1)/2)
		  step = 0;
		else
		  step = a[ind2] - a[ind1];
	      else
		step = (j) * (a[ind2] - a[ind1]) / (increment-1) / (ind2-ind1);
	      
	      a2[index] = a[ind1] + step;
	      index++;
	    }
	  ind1 = ind2;
	}
      else
	{
	  ind1++;
	}
    }
  return (a2);
}

//-----------------------------------------------------------------------------
//  
//  Routine: print_array()
//  
//  Description:
//    Utility to print an array of data mainly for debugging
//
//-----------------------------------------------------------------------------

void print_array(char *name, double *array, long size)
{
  for (int i=0; i<size; i++)
    printf("%s[%d]: %f\n", name, i, array[i]);
}


//-----------------------------------------------------------------------------
//  
//  Routine: metro_direct()
//  
//  Description:
//    Gathers variables for direct call to METRO heat balance model, macadam.
//  
//  Inputs:
//    bridge.............Flag indicating if site is a bridge (1) or a road (0)
//    lat................The latitude of the forecast site
//    lon................The longitude of the forecast site
//    site_layer.........Structure containing subsurface characteristics
//    current_hr.........Hours ahead of nt_start being run
//    obs_start_time.....Unix time of first observation
//    obs_ts.............Obs struct array
//    forc_time..........The Unix time of the start of the forecast
//    nt_start...........The hour offset of the first forecast
//    max_fcst...........The last time step (hr) to be forecast
//    wx_fcst_ts.........Wx fcst struct array
//    chem_failure_time..Hour when chems lose effectiveness
//    treatment_option...The type of treatment being performed
//    useSolarFluxFcst...Use the solar flux from the wx forecast
//    useIrFluxFcst......Use the IR flux from the wx forecast
//
//  Input/Outputs:
//   road_cond ......... Road conditions array struct
//  
//  Returns:
//    0 - success
//    1 - failure
//  
//-----------------------------------------------------------------------------

int metro_direct(int bridge, float lat, float lon, LayerInfo *site_layer,
		 int current_hr, double obs_start_time, ObsTS *obs_ts,
		 double forc_time, int nt_start, int max_fcst,
		 WxFcstTS *wx_fcst_ts, int chem_failure_time,
		 int treatment_option, int useSolarFluxFcst,
		 int useIrFluxFcst,
		 RoadConditions *road_cond)
{
  //
  // Time values defining temporal resolution of data sent to macadam.
  // 
  int metroTimesPerHour = SEC_PER_HOUR/fTimeStep;

  int status = 0;
  int i, j;

  //
  // Set up site location and bridge flag variables
  //
  BOOL bFlat = bridge;
  double dMLat = lat;
  double dMLon = lon;

  //
  // Set up subsurface materials and depths (need to reverse order). Also
  // convert material types to METRo values. I think the types date back
  // to SNTHERM.
  //
  long nNbrOfZone = site_layer->num_layer;
  int extra_layer = 1; // Do_Metro() adds a sand layer at the bottom for roads
  if (bFlat)
    extra_layer = 0;
  double *dpZones = new double[nNbrOfZone+extra_layer];
  long *npMateriau = new long[nNbrOfZone+extra_layer];  
  int ii = nNbrOfZone-1;
  for (i=0; i<nNbrOfZone; i++)
    {
      dpZones[ii] = site_layer->layer_thickness[i];
      switch (site_layer->layer_mat[i])
	{
	case 93:
	case 94:
	  npMateriau[ii] = 1;
	  break;
	case 92:
	  npMateriau[ii] = 2;
	  break;
	case 91:
	  npMateriau[ii] = 3;
	  //if (dpZones[ii] < .2750) // Maintain minimum concrete thickness
	      //dpZones[ii] = .2750;
	  break;
	case 3:
	  npMateriau[ii] = 4;
	  break;
	default:
	  proc_log->write_time("Error: Unsupported material: %d\n",
			       site_layer->layer_mat[i]);
	  return(1);
	}
      ii--;
    }

  //
  //------------------- Create observation variables ------------------------
  //
  // Collect valid obs records for air, road surface and subsurface temp from
  // the input obs and/or forecast data. A data record is valid if it has a
  // good road surface temp and the time between obs is within limits, based
  // on the METRo internal criteria. The other obs can be bad or missing and
  // they will be flagged if so later.
  //
  // Forecasts are used to fill in the obs history if current_hr > 0, which
  // means we are doing a treatement in the future.
  //
  //-------------------------------------------------------------------------

  //
  // Allocate space for hourly obs
  //
  double *obsRoadTemp = new double[NUM_OBS_HRS];
  double *obsSubSurfTemp = new double[NUM_OBS_HRS];
  double *obsAirTemp = new double[NUM_OBS_HRS];
  double *obsDewPoint = new double[NUM_OBS_HRS];
  double *obsWindSpeed = new double[NUM_OBS_HRS];
  double *obsTime = new double[NUM_OBS_HRS];

  // Flags for required obs conditions
  BOOL simultaneousRoadAndSubSurfTemp = 0;
  int numRoadTemps = 0;

  //
  // Loop over obs times to find valid obs records
  //
  int num_obs = 0;
  for (i=0; i<NUM_OBS_HRS - current_hr; i++)
    {
      float sfcTemp;
      if (bFlat)
	sfcTemp = obs_ts->obs[current_hr+i].bridge_T;
      else
	sfcTemp = obs_ts->obs[current_hr+i].road_T;
      //
      // Road temp must be non-missing for the first record
      //
      if (num_obs == 0 &&
	  (sfcTemp < nRoadTemperatureMin || sfcTemp > nRoadTemperatureHigh))
	continue;

      double hr = nt_start + current_hr - NUM_OBS_HRS + 1 + i;

      // start obs over if there is a large time gap
      if ((num_obs > 0) &&
	  (fabs(hr-obsTime[num_obs-1]) > nGapMinuteObservation/60))
	{
	  num_obs = 0;
	  numRoadTemps = 0;
	}    
      obsTime[num_obs] = hr;
      obsRoadTemp[num_obs] = sfcTemp;
      obsSubSurfTemp[num_obs] = obs_ts->obs[current_hr+i].subsurface_T;
      obsAirTemp[num_obs] = obs_ts->obs[current_hr+i].T;
      obsDewPoint[num_obs] = obs_ts->obs[current_hr+i].dewpt;
      obsWindSpeed[num_obs] = obs_ts->obs[current_hr+i].wspd;

      // QC. Set to a missing value so it is not used in interpolation.
      // Also set obs flags.
      if (sfcTemp < nRoadTemperatureMin || sfcTemp > nRoadTemperatureHigh)
	obsRoadTemp[num_obs] = METRO_MISSING_VALUE;
      else
	numRoadTemps++;
      
      if (obsSubSurfTemp[num_obs] < nSubSurRoadTmpMin || 
	  obsSubSurfTemp[num_obs] > nSubSurRoadTmpHigh)
	obsSubSurfTemp[num_obs] = METRO_MISSING_VALUE;
      
      if (obsAirTemp[num_obs] < nAirTempMin || 
	  obsAirTemp[num_obs] > nAirTempHigh)
	obsAirTemp[num_obs] = METRO_MISSING_VALUE;
      
      if (obsDewPoint[num_obs] < nAirTempMin || 
	  obsDewPoint[num_obs] > nAirTempHigh)
	obsDewPoint[num_obs] = METRO_MISSING_VALUE;
      
      if (obsWindSpeed[num_obs] < 0 || 
	  obsWindSpeed[num_obs] > nMaxWindSpeed)
	obsWindSpeed[num_obs] = METRO_MISSING_VALUE;

      num_obs++;
    }

  //
  // Fill in obs with forecasts if current_hr > 0 (future). Should
  // not need to do QC here since bad or missing values would have
  // been caught in previous run (current_hr == 0).
  //
  int fcst_start_hr = 1;
  if (current_hr > NUM_OBS_HRS)
    fcst_start_hr = current_hr - NUM_OBS_HRS + 1;

  for (i=fcst_start_hr; i<=current_hr; i++)
    {
      // Road and bridge are the same in the forecasts
      if ((road_cond[i].road_T > nRoadTemperatureMin) &&
	  (road_cond[i].road_T < nRoadTemperatureHigh))
	{
	  obsTime[num_obs] = nt_start + i;
	  obsRoadTemp[num_obs] = road_cond[i].road_T;
	  obsSubSurfTemp[num_obs] = road_cond[i].subsurface_T;
	  obsAirTemp[num_obs] = wx_fcst_ts->fcst[nt_start+i].T;
	  obsDewPoint[num_obs] = wx_fcst_ts->fcst[nt_start+i].dewpt;
	  obsWindSpeed[num_obs] = wx_fcst_ts->fcst[nt_start+i].wind_speed;
	  numRoadTemps++;
	  num_obs++;
	}
    }

  //
  // METRo fills in any missing subsurface temps if possible. Check if first
  // value is missing to find the next valid one to use for filling. Also
  // set simultaneousRoadAndSubSurfTemp flag here.
  //
  double subSurfaceTemp;
  if (num_obs > 0 && obsSubSurfTemp[0] == METRO_MISSING_VALUE)
    {
      int ii = 1;
      while (ii < num_obs && obsSubSurfTemp[ii] == METRO_MISSING_VALUE)
	ii++;
      subSurfaceTemp = obsSubSurfTemp[ii];
    }
  for (i=0; i<num_obs; i++)
    {
      if (obsSubSurfTemp[i] != METRO_MISSING_VALUE)
	subSurfaceTemp = obsSubSurfTemp[i];
      else
	obsSubSurfTemp[i] = subSurfaceTemp;

      if (obsRoadTemp[i] != METRO_MISSING_VALUE &&
	  obsSubSurfTemp[i] != METRO_MISSING_VALUE)
	simultaneousRoadAndSubSurfTemp = 1;
    }

  //
  // Set the obs flags for limited obs cases. Don't bother setting bpNoObs[2]
  // since the call to Do_Metro would just return with an error anyway.
  // bpNoObs[0] and bpNoObs[1] are set below once we know the first forecast
  // hour.
  //
  BOOL *bpNoObs = new BOOL[4];
  bpNoObs[0] = 0;
  bpNoObs[1] = 0;
  bpNoObs[2] = 0;
  bpNoObs[3] = 0;

  if (num_obs == 0) // no obs
    {
      proc_log->write_time("Warning: No obs for site.\n");
      status = 1;
    }

  if (num_obs == 1) // (one valid obs)
    bpNoObs[3] = 1;

  //
  // **** Check that obs meet METRo criteria ****
 //
  // Last road temp can't be missing (ob/forecast overlap required)
  //
  if (num_obs > 0 && obsRoadTemp[num_obs-1] == METRO_MISSING_VALUE)
    {
      proc_log->write_time("Warning: Last road surface temp is missing.\n");
      status = 1;
    }
  else if (simultaneousRoadAndSubSurfTemp == 0)
    {
      proc_log->write_time("Warning: No valid obs with both road temp and subsurface temp.\n");
      status = 1;
    }
  else if (numRoadTemps < 2)
    {
      proc_log->write_time("Warning: Not enough valid road temp obs.\n");
      status = 1;
    }

  //
  // Clean up and return of there were problems with obs
  //
  if (status)
    {
      delete [] obsRoadTemp;
      delete [] obsSubSurfTemp;
      delete [] obsAirTemp;
      delete [] obsDewPoint;
      delete [] obsWindSpeed;
      delete [] obsTime;
      delete [] bpNoObs;
      delete [] dpZones;
      delete [] npMateriau;
      proc_log->write_time("Warning: Did not run METRo model due to obs issues.\n");
      return(status);
    }

  //
  // Interpolate the data to the finer time resolution.
  //
  long nLenObservation = (num_obs-1) * metroTimesPerHour; 
  double *dpTimeO = interp(obsTime, num_obs, metroTimesPerHour, 0);
  double *dpRTO = interp(obsRoadTemp, num_obs, metroTimesPerHour, 0);
  double *dpDTO = interp(obsSubSurfTemp, num_obs, metroTimesPerHour, 0);
  double *dpTAO = interp(obsAirTemp, num_obs, metroTimesPerHour, 0);
  double *dpTDO = interp(obsDewPoint, num_obs, metroTimesPerHour, 0);
  double *dpFFO = interp(obsWindSpeed, num_obs, metroTimesPerHour, 0);

  //
  // Normalize obs times from 0 to < 24 hours
  //
  for (i=0; i<nLenObservation; i++)
    {
      if (dpTimeO[i] < 0.0)
	dpTimeO[i] = dpTimeO[i] + 24.0;
      if (dpTimeO[i] >= 24.0)
	dpTimeO[i] = dpTimeO[i] - (int)(dpTimeO[i]/24)*24.0 ;
    }

  //
  // Set the Obs QC flags. This is done (in order) for subsurface temp, air
  // temp, dew point and wind speed. Dew point and wind speed obs are not sent
  // to the model, but they are used to 'relax' the forecasts later on.
  // The array size is made to match what is used in the metro model, since
  // it assumes a max size.
  //
  //long *npSwo = new long[nLenObservation*4];
  long *npSwo = new long[11520*4];
  for (i=0; i<nLenObservation; i++)
    {
      // Initialize to pass.
      npSwo[4*i] = 1;
      npSwo[4*i+1] = 1;
      npSwo[4*i+2] = 1;
      npSwo[4*i+3] = 1;

      if (dpDTO[i] < nSubSurRoadTmpMin || dpDTO[i] > nSubSurRoadTmpHigh)
	npSwo[4*i] = 0;
      
      if (dpTAO[i] < nAirTempMin || dpTAO[i] > nAirTempHigh)
	npSwo[4*i+1] = 0;

      if (dpTDO[i] < nAirTempMin || dpTDO[i] > nAirTempHigh)
	npSwo[4*i+2] = 0;

      if (dpFFO[i] < 0 || dpFFO[i] > nMaxWindSpeed)
	npSwo[4*i+3] = 0;
    }

  //
  // Clean up hourly obs data
  //
  delete [] obsRoadTemp;
  delete [] obsSubSurfTemp;
  delete [] obsAirTemp;
  delete [] obsDewPoint;
  delete [] obsWindSpeed;

  //
  //------------------- Create forecast variables --------------------------
  //
  // Set up arrays for forecasts at hourly intervals. Most variables are
  // straight-forward and are extracted from the wx_fcst_ts, but a few are
  // more complicated and explained below.
  //
  //------------------------------------------------------------------------
  //
  int num_fcst = max_fcst-current_hr;
  long nNbrTimeSteps = (num_fcst-1) * metroTimesPerHour;
  int start_hr = nt_start + current_hr;
  int chem_failure_hr = nt_start + chem_failure_time;

  //
  // Allocate space for hourly forecasts
  // 
  double *fTime = new double[num_fcst];
  double *airTemp = new double[num_fcst];
  double *windSpeed = new double[num_fcst];
  double *sfcPres = new double[num_fcst];
  double *dewPoint = new double[num_fcst];
  double *swFlux = new double[num_fcst];
  double *lwFlux = new double[num_fcst];
  double *precipAmt = new double[num_fcst];
  double *precipType = new double[num_fcst];
  double *roadCond =  new double[num_fcst];

  for (i=0; i<num_fcst; i++)
    {
      fTime[i] = start_hr%24 + i;
      airTemp[i] = wx_fcst_ts->fcst[start_hr + i].T;
      if (airTemp[i] < nAirTempMin || airTemp[i] > nAirTempHigh)
	{
	  proc_log->write_time("Warning: Bad air temp at forecast hour %d: %f\n", i, airTemp[i]);
	  status = 1;
	}
      dewPoint[i] = wx_fcst_ts->fcst[start_hr + i].dewpt;
      if (dewPoint[i] < nAirTempMin || dewPoint[i] > nAirTempHigh)
	{
	  proc_log->write_time("Warning: Bad dewpt temp at forecast hour %d: %f\n", i, dewPoint[i]);
	  status = 1;
	}
      windSpeed[i] = wx_fcst_ts->fcst[start_hr + i].wind_speed;
      if (windSpeed[i] < 0 || windSpeed[i] > nMaxWindSpeed)
	{
	  proc_log->write_time("Warning: Bad wind speed at forecast hour %d: %f\n", i, windSpeed[i]);
	  status = 1;
	}      
      sfcPres[i] = wx_fcst_ts->fcst[start_hr + i].P_sfc;
      if (sfcPres[i] < nLowerPressure || sfcPres[i] > nUpperPressure)
	{
	  proc_log->write_time("Warning: Bad stn pressure at forecast hour %d: %f\n", i, sfcPres[i]);
	  status = 1;
	}      
      sfcPres[i] *= 100; // to HPa
      
      int cloudOctal = int(wx_fcst_ts->fcst[start_hr + i].cloud_cov * 8 + 0.5);
      if (cloudOctal < 0 || cloudOctal > 8)
	{
	  proc_log->write_time("Warning: Bad cloud cover at forecast hour %d: %d\n", i, cloudOctal);
	  status = 1;
	}

      if (useSolarFluxFcst)
	{
	  swFlux[i] = wx_fcst_ts->fcst[start_hr + i].dswrf_sfc;
	  if (swFlux[i] == NC_FILL_FLOAT)
	    {
	      proc_log->write_time("Warning: Solar flux missing at forecast hour %d\n", i);
	      status = 1;
	    }
	}
      else
	swFlux[i] = solarFlux(cloudOctal, forc_time, fTime[i],
			      lat, lon);
      
      if (useIrFluxFcst)
	{
	  lwFlux[i] = wx_fcst_ts->fcst[start_hr + i].dlwrf_sfc;
	  if (lwFlux[i] == NC_FILL_FLOAT)
	    {
	      proc_log->write_time("Warning: IR flux missing at forecast hour %d\n", i);
	      status = 1;
	    }
	}    
      else
	lwFlux[i] = infraredFlux(cloudOctal, airTemp[i]);
      
      roadCond[i] = 1;  // always 1 (wet) for some reason

      // For precip amount, the METRo preprocessing code adds snow and rain
      // together which, in metro_file_io.cc, were split apart, so we just use
      // the qpf01 as-is. Note that the metro_file_io.cc code used snow_accum
      // in some cases which was computed using a dynamic snow/liquid ratio,
      // but then it was converted for METRo input using a fixed 10/1 ratio,
      // a bug. The METRo code also muliplied precip (in mm) by 10e-4 to get
      // meters which seems off by 10.
      //
      if (wx_fcst_ts->fcst[start_hr + i].qpf01 == NC_FILL_FLOAT ||
	  wx_fcst_ts->fcst[start_hr + i].Ptype == NC_FILL_FLOAT)
	{
	  proc_log->write_time("Warning: Precip amount or type forecast missing at hour %d\n", i);
	  status = 1;
	}
      else
	{
	  precipAmt[i] = wx_fcst_ts->fcst[start_hr + i].qpf01 / 3600 * 10e-4;  // 10e-3 ??
	  precipType[i] = metroPrecipType(wx_fcst_ts->fcst[start_hr + i].Ptype,
					  precipAmt[i],
					  treatment_option,
					  start_hr+i, chem_failure_hr,
					  airTemp[i]);
	}

    } // end of hourly forecast

  //
  // See if we can continue with forecasts
  //
  if (status == 0)
    {
      //
      // Interpolate to finer time resolution
      //
      double *dpTA = interp(airTemp, num_fcst, metroTimesPerHour, 0);
      double *dpTD = interp(dewPoint, num_fcst, metroTimesPerHour, 0);
      double *dpPS = interp(sfcPres, num_fcst, metroTimesPerHour, 0);
      double *dpFF = interp(windSpeed, num_fcst, metroTimesPerHour, 0);
      double *dpFS = interp(swFlux, num_fcst, metroTimesPerHour, 0);
      double *dpFI = interp(lwFlux, num_fcst, metroTimesPerHour, 0);
      double *dpQP = interp(precipAmt, num_fcst, metroTimesPerHour, 0);
      double *dpTYP = interp(precipType, num_fcst, metroTimesPerHour, 1);
      //double *dpTYP = interp(precipType, num_fcst, metroTimesPerHour, 0);
      double *dpRC = interp(roadCond, num_fcst, metroTimesPerHour, 1);
      double *dpFT = interp(fTime, num_fcst, metroTimesPerHour, 0);

      //
      // Adjust forecasts to the obs. The METRo preprocessor uses exponential
      // decay of the fcst-ob error, all the way out to 44 hours, which seems
      // like over-kill. (This may also be a bug, since 44 is pretty arbitrary
      // and the comments seem to indicate the forecasts will be adjusted out
      // to only 4 or 6 hours.) So, we will just do a simple linear fec over 6
      // hours. This is only done for air temp, dew point and wind speed.
      //
      int fecExtent = 6; // hrs
      double airTempCorr = dpTA[0] - dpTAO[nLenObservation-1];
      double dewPointCorr = dpTD[0] - dpTDO[nLenObservation-1];
      double windSpeedCorr = dpFF[0] - dpFFO[nLenObservation-1];
      int nTimes = fmin(metroTimesPerHour*fecExtent, nNbrTimeSteps);
      for (i=0; i<nTimes; i++)
	{
	  // Check the QC flag, then adjust
	  if (npSwo[4*(nLenObservation-1)+1] == 1)
	    dpTA[i] = dpTA[i] - airTempCorr*(nTimes-i)/nTimes;
	  if (npSwo[4*(nLenObservation-1)+2] == 1)
	    dpTD[i] = dpTD[i] - dewPointCorr*(nTimes-i)/nTimes;
	  if (npSwo[4*(nLenObservation-1)+3] == 1)
	    dpFF[i] = dpFF[i] - windSpeedCorr*(nTimes-i)/nTimes;
	}

      //
      // QC dewpoint then compute absolute humidity
      //
      double *dpAH = new double[nNbrTimeSteps];
      for (i=0; i<nNbrTimeSteps; i++)
	{
	  if (dpTD[i] > dpTA[i] )
	    dpTD[i] = dpTA[i];
	  
	  dpAH[i] = absoluteHumidity(dpTD[i], dpPS[i]);
	}
      
      //
      // force dpTYP to snow (2) where needed (Commented out since METRo
      // changes the type internally anyway)
      //
      //for (i=0; i<nNbrTimeSteps; i++)
      //{
      //  if (dpTYP[i] > 1.0)
      //    dpTYP[i] = 2;
      //  else
      //    dpTYP[i] = 1;
      //}


      //
      // Set dDeltaT, which is the hour difference between the first ob and
      // first forecast. Now we can set bpNoObs[0] (ob is at or after the first
      // forecast time) and bpNoObs[1] (< 3 hrs of obs-fcst overlap).
      // Use start_hr since it is not modulo 24.
      //
      double dDeltaT = start_hr - obsTime[0];
      if (dDeltaT <= 0)  // Ob overlaps forecast time
	bpNoObs[0] = 1;

      // (< 3 hrs obs-fcst overlap)
      if ((nLenObservation - dDeltaT*3600/fTimeStep) < nThreeHours*3600/fTimeStep)
	bpNoObs[1] = 1;

      //
      // Debug mode for the model
      //
      BOOL bSilent = 1;
      if (debug_level > 4)
	bSilent = 0;

      long tmtType = treatment_option;      

      // Return code for metro run
      BOOL bRet = 0;

      // Allocate space for output variables
      // These will be compared to indirectly generated output vars
      long *outRC = new long[nNbrTimeSteps];
      double *outRT = new double[nNbrTimeSteps];
      double *outSST = new double[nNbrTimeSteps];
      double *outSN = new double[nNbrTimeSteps];
      double *outRA = new double[nNbrTimeSteps];

      //
      // Call the METRo heat-balance model
      //
      // For clarity, we've used the same variable names used in the macadam
      // module Do_Metro function definition, like 'em or not. Not sure how FF
      // translates to wind speed in french.
      //
      Do_Metro(bFlat, dMLat, dMLon, dpZones, nNbrOfZone, npMateriau, dpTA, dpQP,
	       dpFF, dpPS, dpFS, dpFI, dpFT, dpTYP, dpRC, dpTAO, dpRTO, dpDTO,
	       dpAH, dpTimeO, npSwo, bpNoObs, dDeltaT, nLenObservation,
	       nNbrTimeSteps, bSilent, outRT, outSST, outRC, outSN, outRA, &bRet,
	       tmtType);
      
      //
      // Check for success, process data into the roadcond struct
      //
      if (bRet == 0)
	{
	  
	  //
	  // Pull out forecasts on the hour. The first roadcast starts at
	  // index 1.
	  //
	  int offset=0;
	  for(int hr=0; hr<num_fcst; hr++)
	    {
	      if (hr != 0)
		offset = (hr * metroTimesPerHour) - 1;
	      
	      road_cond[current_hr + hr].road_T = outRT[offset];
	      road_cond[current_hr + hr].subsurface_T = outSST[offset];
	      road_cond[current_hr + hr].road_condition = outRC[offset];
	      road_cond[current_hr + hr].PsnowOnRoad = outSN[offset];
	      road_cond[current_hr + hr].PsnowOnRoad *= CM_TO_MM; // should this be liquid to snow ratio??
	                                                          // This is not clear in METRRo code.

	      //printf("start_hr %d, current_hr %d, hr %d, QP %.10f, TYP %.0f, RA %.10f, SN %.2f, RC %d, RT %.2f, SS %.2f\n", start_hr, current_hr, hr, precipAmt[hr], dpTYP[offset], outRA[offset], outSN[offset]*CM_TO_MM, road_cond[current_hr + hr].road_condition,  road_cond[current_hr + hr].road_T, road_cond[current_hr + hr].subsurface_T);
	    }
	}
      else
	{
	  proc_log->write_time("Error: METRo failed. (Run by hand to see error messages from model.)\n");
	  status = 1;
	}
      //
      // Clean up memory allocated in this block
      //
      delete [] outRC;
      delete [] outRT;
      delete [] outSST;
      delete [] outSN;
      delete [] outRA;

      delete [] dpTA;
      delete [] dpTD;
      delete [] dpQP;
      delete [] dpFF;
      delete [] dpPS;
      delete [] dpFT;
      delete [] dpFS;
      delete [] dpFI;
      delete [] dpTYP;
      delete [] dpRC;
      delete [] dpAH;
    }
  else
    {
      proc_log->write_time("Warning: Did not run METRo model due to forecast issues.\n");
    }

  //
  // clean up remaining memory
  //
  delete [] dpZones;
  delete [] npMateriau;

  delete [] obsTime;
  delete [] dpTimeO;
  delete [] dpRTO;
  delete [] dpDTO;
  delete [] dpTAO;
  delete [] dpTDO;
  delete [] dpFFO;
  delete [] npSwo;
  delete [] bpNoObs;

  delete [] fTime;
  delete [] airTemp;
  delete [] windSpeed;
  delete [] sfcPres;
  delete [] dewPoint;
  delete [] swFlux;
  delete [] lwFlux;
  delete [] precipAmt;
  delete [] precipType;
  delete [] roadCond;

  //
  // macadam memory
  //
  //free_structure();

  return(status);
}
