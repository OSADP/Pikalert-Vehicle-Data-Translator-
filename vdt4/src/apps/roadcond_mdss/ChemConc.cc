//=======================================================================
//
//   (c) Copyright, 2003-05 Massachusetts Institute of Technology.
//
//   Permission is herby granted, without written agreement or royalty fee, to use, 
//   copy, modify, and distribute this software and its documentation for any purpose,
//   provided that the above copyright notice and the following three paragraphs
//   appear in all copies of this software.
//
//   IN NO EVENT SHALL M.I.T. BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
//   INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
//   AND ITS DOCUMENTATION, EVEN IF M.I.T. HAS BEEN ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//
//   M.I.T. SPECIFICALLY DISCLAIMS ANY WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
//   NON-INFRINGEMENT.
//
//   THE SOFTWARE IS PROVIDED ON AN "AS-IS" BASIS AND M.I.T. HAS NO OBLIGATION TO
//   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//   < END MIT Copyright notice >
//
//=======================================================================
//
//       SCCS ID is " %W% %G% %I% "
//
//       Date           Author                     Action
//
//       11/07/01       Robert Hallowell           Created
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//       08/24/02       Robert Hallowell           Added read params call and additional
//                                                 comments.
//       08/18/03       Robert Hallowell           Adjusted comments
//
//=======================================================================
//
// File: ChemConc.C
//
// Description:
//
//   This file contains routines for calculating the chemical concentration and dilution
// of chemicals applied to road surfaces. The algorithm concepts and descriptions were
// developed by the Cold Regions Research and Engineering Laboratory (CRREL). The software
// was developed by MIT Lincoln Laboratory (with some modifications to the original CRREL
// descriptions). The main routine (AdjustChemicalConcentration) uses the forecasted weather
// and current road conditions and applies any specified treatment (chemical) resulting in a
// profile of the chemcal concentration on the road over the forecast period. The following
// additional routines are included in this file:
//
//    CalcEvaporationRate...........Calculate the evaporation rate given forecasted weather.
//    AdjustChemicalConcentration...Main routine described above.
//    CalcTrafficFactor.............Calculate the impact (in % loss) to available chemicals
//                                  based on the road traffic volume.
//    CalcCriticalChemSolutionPoint....Calculate the point at which the chemical concentration
//                                     will become too weak to prevent freezing.
//    CalcCriticalChemSaturationPoint..Calculate the point at which the solution is saturated with
//                                     chemicals (adding more won't increase effectiveness).
//
//    ChemConc_InitAndGetparams.....A routine to extract user defined settings for the Chemical
//                                  Concentration code.
//    GetChemEutecticTemp...........Returns the eutectic temperature of the specified chemical.
//    GetChemEutecticConc...........Returns the eutectic concentration of the specified chemical.
//
//=======================================================================
//
#include <string.h>

#include "RulesOfPractice.hh"
#include <log/log.hh>

extern Log *proc_log;

//
// Define parameters globally
//
ChemicalType ChemConc_Params;
ChemicalType Pretreat_ChemConc_Params;

int P_ROADT_TOOCOLD;

//
// Module: CalcEvaporationRate()
//
// Description:
//   This routine estimates the evaporation rate of water from the road surface. There is
//   very little specific research on the phenomenon. Most studies have been done on lake or
//   agricultural related evaporation rates and they are generally concerned with time scales
//   of weeks and months rather than minutes and hours. There are four generally accepted 
//   factors to be considered in estimating evaporation rates. They are: 1) relative humidity
//   2) solar insolation 3) wind speed and 4) traffic flow. For the initial algorithm, only 
//   relative humidity and wind speed are used. 
//
//   Inputs:
//     Wfcst.......The current weather forecast structure.
//     Rfcst.......The current road conditions forecast.
//
//   Outputs:      None
//
//   Returns:
//     evap_rate...The fractional hourly rate of evaporation (0.0-1.0) where 0.0 is no
//                 evaporation and 1.0 is immediate evaporation (each hour).
//
//
float CalcEvaporationRate( WxForecast Wfcst, RoadConditions Rfcst )
{
  float evap_rate = 0.0;  // No evaporation occurs (0.0) immediate evaporation (1.0)

  if ( Wfcst.LiquidPrecipRate > 0.0 ) {
    evap_rate = 0.0;
  }
  else {
  
    //
    // Relative humidity is the simplest measure we have for estimating the possiblity
    // of evaporation. Low RH should allow lots of evaporation, while rain or high humidity
    // will result in very little evaporation.
    //
    float rh_factor = Wfcst.rh;
    
    if ( Wfcst.rh > 0.95 )
      rh_factor = 0.0;
    else if ( Wfcst.rh < 0.5 )
      rh_factor = 0.75;
    else 
      rh_factor = 0.75 * ( 1 - (Wfcst.rh - 0.5)/0.5 );
    //
    // Increasing winds allow more water to be evaporated from the road surface.
    // We've coded a linear relationship it is likely more exponential -- but further
    // research is needed to refine the relationship.
    //
    float w_factor = 1.0;
    
    if ( Wfcst.wind_speed < 1.0 )
      w_factor = 1.0;
    else if ( Wfcst.wind_speed > 10.0 )
      w_factor = 1.1;
    else
      w_factor = 1.0 + 0.1 * (Wfcst.wind_speed - 1.0)/9.0; 
    
    evap_rate = rh_factor * w_factor;
  }
	//  cout << Wfcst.LiquidPrecipRate << "Prate " << evap_rate << " evap_rate " << Wfcst.wind_speed << " wspd " << Wfcst.rh << " rh\n";

  return(evap_rate);
}
//
//  Routine: CalcTrafficFactor()
//
//  Description:
//      Calculates the fraction (0.0-1.0) of chemicals that will be dispersed from the roadway
//  by passing traffic. The dilution affect of traffic is presumed to be exponential in nature.
//  Heavy traffic results in a dilution that is 9 (3^2) times greater than low levels of traffic.
//  The relative factor is them multiplied by a nominal dilution factor for traffic in general 
//  (nominally 0.01 or a 1% reduction in the overall chemical on the road). This is a simple 
//  assumption for now.
//
//  Inputs:
//     intensity....An integer value (0 = very low 1=low 2=moderate 3=heavy) 
//                  indicating the traffic intensity.
//                  Current definitions for categories:
//                  Very low(0):    < 50  vehicles/hour/lane
//                  Low(1)     :  50-150  vehicles/hour/lane   
//                  Moderate(2):  150-250 vehicles/hour/lane   
//                  High(3)    :  >250    vehicles/hour/lane   
//
//  Outputs:
//     none.
//
//  Returns:
//     traffic factor...The fraction (0.0-1.0) of chemicals that will be dispersed from the roadway
//                      by passing traffic.
//
float CalcTrafficFactor( int intensity )
{
  return( ChemConc_Params.NominalTrafficDilution * intensity * intensity );
}

//
//  Routine: ChemConc_InitAndGetParams()
//
//  Descriptiony:
//      Routine to read user defined chemical concentration parameters from a text file.
//
//  Inputs:
//      ctype.............Value of chemical to be used
//      cform.............Phase of chemical (dry, prewet, wet)
//      site_config.......Structure containing defaults for the current site
//      pretreat_flag.....Flag indicating chem is for pretreatment (0 = no, 1 = yes)
//      app_rate_units....External units for application rates
//
//  Outputs:
//      c.................An updated structure containing all the data needed for the site
//
//  Returns:
//      status............A status flag (0 = OK, -1 Failure)
//
int ChemConc_InitAndGetParams( int ctype, int cform, SiteLayerTraffic *site_config, int ptflag, int app_rate_units, ChemicalType *c )
{
  if ( c == NULL ) {
    proc_log->write_time("Warning : InitAndGetParams: ChemicalType structure must be allocated before initialization\n");
    return( C_STATUS_BAD );
  }
  // Initialize parameters to default values
  c->NominalTrafficDilution = P_NominalTrafficDilution;
  c->UserChemType = ctype;
  c->UserChemForm = cform;

  // Parameters related to chemical application rate 
  if ( ptflag == 0 && site_config != NULL ) {
    c->MinApplicationRate = site_config->getChemMinRate();
    c->MaxApplicationRate = site_config->getChemMaxRate();
    c->RoundTreatment     = site_config->getChemRateInc();
  }
  else if ( ptflag == 1 && site_config != NULL ) {
    c->MinApplicationRate = site_config->getPretreatMinRate();
    c->MaxApplicationRate = site_config->getPretreatMaxRate();
    c->RoundTreatment     = site_config->getPretreatRateInc();
  }
  else { // Should not happen!
    proc_log->write_time("Error: No site_config specified. Should not happen!!\n");
    return(C_STATUS_BAD);
  }

  //
  // Rates are assumed to be in 'app_rate_units'. Need to convert
  // them to internal units (lb/ln-mile) if they are not. If the form
  // is liquid, and app_rate_units are not lb/lane-mile, assume they
  // are in liters/lane-km, since that is currently the only other
  // liquid (volume) units supported.
  //
  if (app_rate_units != C_CHEMUNITS_LBS_LANEMILE)
    {
      if ( c->UserChemForm == C_CHEMFORM_LIQUID )
	{
	  c->Units = C_CHEMUNITS_LITERS_LANEKM;
	  c->MinApplicationRate = ConvertChemUnits(c->MinApplicationRate, c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
	  c->MaxApplicationRate = ConvertChemUnits(c->MaxApplicationRate, c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
	  c->RoundTreatment     = ConvertChemUnits(c->RoundTreatment,     c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
	}
      else
	{
	  c->MinApplicationRate = ConvertChemUnits(c->MinApplicationRate, c->UserChemType, app_rate_units, C_CHEMUNITS_LBS_LANEMILE);
	  c->MaxApplicationRate = ConvertChemUnits(c->MaxApplicationRate, c->UserChemType, app_rate_units, C_CHEMUNITS_LBS_LANEMILE);
	  c->RoundTreatment     = ConvertChemUnits(c->RoundTreatment,     c->UserChemType, app_rate_units, C_CHEMUNITS_LBS_LANEMILE);
	}
    }
  else
    {
      if ( c->UserChemForm == C_CHEMFORM_LIQUID ) {
	c->Units = C_CHEMUNITS_GALS_LANEMILE;
	c->MinApplicationRate = ConvertChemUnits(c->MinApplicationRate, c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
	c->MaxApplicationRate = ConvertChemUnits(c->MaxApplicationRate, c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
	c->RoundTreatment     = ConvertChemUnits(c->RoundTreatment,     c->UserChemType, c->Units, C_CHEMUNITS_LBS_LANEMILE);
      }
    }

  // Set to internal unit
  c->Units = C_CHEMUNITS_LBS_LANEMILE;


  // Min temp is used to clip the effective range of the chemical
  switch( c->UserChemType ) 
    {
    case C_CHEMTYPE_NACL:
      c->MinTemp = C_NACL_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_MGCL2:
      c->MinTemp = C_MGCL2_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_CACL2:
      c->MinTemp = C_CACL2_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_CMA:
      c->MinTemp = C_CMA_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_KAC:
      c->MinTemp = C_KAC_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_CALIBER:
      c->MinTemp = C_CALIBER_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_ICESLICER:
      c->MinTemp = C_ICESLICER_HALFPOINT_TEMP;
      break;
    case C_CHEMTYPE_ICEBAN:
      c->MinTemp = C_ICEBAN_HALFPOINT_TEMP;
      break;
    default: 
      // default unknown chemical (assume nacl like) 
      proc_log->write_time("Warning : ChemConc_InitAndGetParams: Bad chemical type specified - assuming NACL\n");
      c->MinTemp = C_NACL_HALFPOINT_TEMP;
      break;
    }
  c->MaxTemp = P_MAX_TEMP;
  
  // Set value at which roads are considered too cold for chemicals based on MinTemp
  P_ROADT_TOOCOLD = (int)(c->MinTemp - 5.0);

  // runoff rates are estimates -- further research/surveys are needed to refine these numbers
  if ( c->UserChemForm == C_CHEMFORM_LIQUID )
    c->RoadChemAppInitialLossRate    = P_InitialChemAppLossRateLiquid; 
  else if ( c->UserChemForm == C_CHEMFORM_DRY )
    c->RoadChemAppInitialLossRate    = P_InitialChemAppLossRateDry; 
  else {
		//    proc_log->write_time("Warning : ChemConc_InitAndGetParams: Bad chemical form specified - assuming PREWET\n");
    c->RoadChemAppInitialLossRate    = P_InitialChemAppLossRatePrewet; 
  }
  //
  // Runoff percentages are adapted from the U.S. Soil Conservation Service.  Technical Release 55:
  // Urban Hydrology for Small Watersheds.  USDA (U.S.Department of Agriculture).  June 1986.  
  // Available from NTIS (National Technical Information Service), NTIS #PB87101580.
  c->RoadWaterImmediateRunoff      = P_RoadwayImmediateRunoff; 
  c->RoadChemSolutionRunoffPerHour = P_ChemSolutionRunoff; 

  // 
  // A parameter to control how close to the solution curve in this chemical's
  // phase diagram is the algorithm allowed to get. With a value of 0 degC 
  // concentrations may drop to exactly on the curve.
  // 
  c->ChemSafeTempDelta = P_CHEM_SAFE_DELTA; 
  //

  return(C_STATUS_OK);
}

//
//  Routine: AdjustChemicalConcentration()
//
//  Description:
//      This is the main chemical concentration routine, it calculates the affect of applying
//  chemicals to the road surface. Chemicals are reduced by traffic on the roadway and by
//  dilution of the chemicals due to precipiation. The caller controls the timestep to start
//  and end processing and the amount (if any) of chemicals applied to the road (treat).
//  An array of structures containing the weather variables (liquid and frozen precipitation
//  rates) for the forecast period are passed in (used for dilution). The road weather
//  variables are also passed in (Tpavement, PsnowOnRoad, TrafficIntensity) but a variety of
//  road weather variables are also set in this routine (AvailH20, AvaliableChem, FinalChemConc).
//  In addition, the return value of this routine indicates the timestep where any chemicals
//  applied drop below their acceptable concentrations to maintain ice-free conditions.
//
//  Inputs:
//    treat.......A treatment structure containing information on the next treatment to be applied.
//    FcstStartTime..The first timestep to process.
//    FcstEndTime....The final timestep to process.
//    wxfcst.........An array containing the pertinent weather variables for predicting road
//                   conditions after treatment.
//    roadfcst.......An array containing the pertinent road condition variables for predicting
//                   road condtions after treatment (Tpavement, PsnowOnRoad, TrafficIntensity).
//
//  Outputs:
//    roadfcst.......An array containing the resultant road condition variables pertinent to
//                   the chemcicals applied to the road surface(AvailH20, AvaliableChem, FinalChemConc).
//
//  Returns:
//    low_chem.....The timestep just prior to the point where predicted chemical concentrations
//                 will drop below the acceptable value (presumably resulting in ice/snow buildup).
//                 Returns -999 when chemical stays within acceptable ranges through forecast
//                 period.
//
int AdjustChemicalConcentration(int FcstStartTime, int FcstEndTime, Treatment *treat,
                                WxForecast *wxfcst, RoadConditions *roadfcst )
{
  //
  // Declare and initialize variables
  //

  bool Applied = FALSE;
  int i, low_chem = -999;
  int StartTime, EndTime;
  float ChemRate=0.0, ResidualChem=0.0, ChemInSolution=0.0;
  float SatPoint=0.0, CorrectedChemConc=0.0;
  float tfactor;

  StartTime = FcstStartTime; // initialize to fcst start time

  int chem_type = roadfcst[StartTime].ChemSolutionType;
  if ( chem_type == C_CHEMTYPE_UNKNOWN )
    chem_type = P_user_chemtype;
	//    chem_type = P_USER_CHEMTYPE;

  // If applying a new treatment then start at treatment hour otherwise start at the beginning
  if ( treat != NULL ) {
    if ( treat->StartTime == 0 ) {
      treat->StartTime = 1;
      //cout << "StartTime advanced to time step 1 to avoid array bounds error\n";
    }
    StartTime = treat->StartTime; // Can't start at time 0 due to indexes of i-1 in dilution loop
    chem_type = treat->Chemicals.UserChemType;
  }
  else if ( FcstStartTime == 0 ) {
    StartTime = FcstStartTime+1; // Can't start at time 0 due to indexes of i-1 in dilution loop
    //cout << "StartTime advanced to time step 1 to avoid array bounds error\n";
  }

  EndTime = FcstEndTime;

  // cout << "TIME\tTpvmnt\tPRate\tFinalChemConc\tSolnCurve\tSatCurve\tAvailH20\tAvailChem\n";

  for ( i=StartTime; i<EndTime; i++ ) {
    //
    // Determine the amount of water on the road surface
    // This calculation assumes that some % of the water will runoff immediately
    // and that the previous hours water or chemical solution runs off at a slightly
    // different rate. This calculation does not take into account evaporative effects
    // from the sun or extremely dry air. Therefore, it will overestimate the amount
    // of water remaining on the road surface in post storm environments. An assumption
    // is also made that if the previous water is a solution that the water and chemical
    // evaporate evenly (keeping the concentration % equal until all the solution is gone).
    // This is clearly a simplified equation.
    // 
    // Reset the available water and snow depth to "zero" when the water is nearly gone
    // This may lead to problems because the avail water and snow depth are calculated
    // independently right now -- eventually they will be dependent on each other.
    //
    // If it is precipitating, however lightly, put some water on the road (10% over minimum)
    //

    roadfcst[i].AvailH2O = wxfcst[i].LiquidPrecipRate * C_mmHr_to_LbSf *
      (1 - ChemConc_Params.RoadWaterImmediateRunoff) +
      roadfcst[i-1].AvailH2O * (1 - ChemConc_Params.RoadChemSolutionRunoffPerHour);

    if ( roadfcst[i].AvailH2O < C_AVAILH2O_THRESH ) {
      if ( wxfcst[i].LiquidPrecipRate > 0.0 )
	roadfcst[i].AvailH2O    = 1.1*C_AVAILH2O_THRESH;
      else
	roadfcst[i].AvailH2O    = 0.0;
      //      roadfcst[i].PsnowOnRoad = 0.0;
    }


    // New application of chemicals
    if ( treat != NULL ) 
    {
      if ( treat->ApplyChemicals == TRUE && treat->StartTime == i ) 
      {
        //
        proc_log->write_time(1,"Info: Apply chemicals at hour %d at a rate of %6.2f (lbs/lane-mile)\n",
                             i,treat->ApplicationRate);
        //
        Applied = TRUE;

        // convert application rate from user-specified units to lbs/sf of road
        //
        ChemRate = treat->ApplicationRate / (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);

        //
        // Trucks dropping solid material (and to some extent liquid) will lose some portion of
        // the application as solid material bounces off the road or as liquid is blown off the 
        // road. The user may select a defined loss rate to reduce the overall available chemicals 
        // 
        ChemRate = ChemRate * ( 1 - treat->Chemicals.RoadChemAppInitialLossRate );
        proc_log->write_time(2,"Info: Reduce available chemical due to %4.2f %% road splatter as chemical is delivered\n",100*treat->Chemicals.RoadChemAppInitialLossRate);
        chem_type = treat->Chemicals.UserChemType;
      }
      else {
        // No chemicals, just plowing or some other non-chemical application
        ChemRate = 0.0;
        chem_type = roadfcst[i].ChemSolutionType;
      }
    }
    else {
      // No new chemical, but if a new treatment was used previously then reset the expected
      // chemical in solution.
      ChemRate = 0.0;
      if ( chem_type != roadfcst[i].ChemSolutionType ) {
        roadfcst[i].ChemSolutionType = chem_type;
      }
    }


    //
    // Calculate the residual chemical (if nominal conc indicates all was not used in solution)
    //
    SatPoint = CalcCriticalChemSaturationPoint( chem_type, roadfcst[i-1].road_T );
    if ( roadfcst[i-1].NominalChemConc > SatPoint && SatPoint >= 0.0 &&
	 roadfcst[i-1].AvailH2O >= C_AVAILH2O_THRESH) {
      ResidualChem = roadfcst[i-1].AvailableChem -
        ((CorrectedChemConc/100.0) * roadfcst[i-1].AvailH2O) / (1.0 - CorrectedChemConc/100.0);
    }
    else if ( roadfcst[i-1].AvailH2O < C_AVAILH2O_THRESH )
      ResidualChem = roadfcst[i-1].AvailableChem;
    else
      ResidualChem = 0.0;

    //
    // Calculate the amount of chemical in solution
    //
    ChemInSolution = (1-ChemConc_Params.RoadChemSolutionRunoffPerHour) * 
      ( roadfcst[i-1].AvailH2O * roadfcst[i-1].FinalChemConc / 100.0 ) /
      ( 1.0 - roadfcst[i-1].FinalChemConc / 100.0 );

    // Calculate the amount of chemicals available for melting at time i
    //
    // NOTE: Added a reduction in chemicals due to traffic intensity, this was not in the
    // CRREL tables, but it seems logical that the dry chemicals would be dispersed by traffic. In
    // the CRREL tables only the solution is affected by traffic. Consequently, if I drop chemicals
    // 12 hours before the storm or 1 hour before the storm the same amount of chemicals are on
    // the road at the start of the storm. Adding the traffic dependency reduces the chemicals
    // slightly with each passing hour.
    //
    // Calculate the traffic dispersion factor
    //
    tfactor = CalcTrafficFactor( roadfcst[i].TrafficIntensity );

    //
    // If all of the chemical is residual chemical then increase the rate of chemical 
    // dissipation -- don't want this to happen if user selects pre-treat.
    //
    // Note: P_DryChem_LossRate seems the logical factor to use here, as it was defined
    // in RulesOfPractive.hh but not used. Previously, the code had the 'else if' clause
    // commented out, hence dry_factor was always 1 in non-treatment scenarios, and the
    // only dissipation of chem on the dry road was by traffic, at a 1-4% rate, way too
    // slow. Cowie 2/26/13
    //

    float dry_factor;

    // In wet conditions, use the runoff for solution, otherwise use dry loss rate
    if ( roadfcst[i].AvailH2O >= C_AVAILH2O_THRESH )
      dry_factor = 1.0 - P_ChemSolutionRunoff;
    else 
      dry_factor = 1.0 - P_DryChem_LossRate;


    roadfcst[i].AvailableChem = dry_factor * (1.0 - tfactor) * ResidualChem +
      (1.0 - tfactor) * (ChemRate + ChemInSolution);


    proc_log->write_time(3,"Info: time %d, AvailableChem %f, dfactor %f, tfactor %f, ResidualChem %f, ChemRate %f, ChemInSolution %f AvailH2O %f\n", i, roadfcst[i].AvailableChem, dry_factor, tfactor, ResidualChem, ChemRate, ChemInSolution, roadfcst[i].AvailH2O);


    // Traffic dilution has two affects: 1) It removes "dry" (undisolved chemical) and
    // (2) it reduces the amount of chemical in solution. This same factor is not used to 
    // reduce the available water meaning that traffic transports chemicals from the solution
    // but not the water. This assumption will require further research.
    //

    // Calculate the theoretical chemical concentration (ignoring solution/saturation curves)
    if ( roadfcst[i].AvailableChem > C_NOCHEM_THRESH)
      roadfcst[i].NominalChemConc = (roadfcst[i].AvailableChem / (roadfcst[i].AvailableChem +
                                                                  roadfcst[i].AvailH2O)) * 100.0;
    else
      roadfcst[i].NominalChemConc = 0.0;

    // Check to see if estimated concentration exceeds saturation curve,
    // if so set concentration to saturation
    SatPoint = CalcCriticalChemSaturationPoint( chem_type, roadfcst[i].road_T );

    if ( roadfcst[i].NominalChemConc <= SatPoint )
      CorrectedChemConc = roadfcst[i].NominalChemConc;
    else
      CorrectedChemConc = SatPoint;

    // 
    // Since the available water already includes any newly fallen precipitation, we can
    // calculate the final diluted chemical concentration by simply using the corrected
    // chemical concentration and adjusting for additional traffic dilution.
    //

    if ( roadfcst[i].AvailH2O > 0.0 ) {
      roadfcst[i].FinalChemConc = CorrectedChemConc;
    }
    else
      roadfcst[i].FinalChemConc = 0.0;

    //
    // Calculate the time step where chemicals will again become ineffective
    //
    float solution_point = CalcCriticalChemSolutionPoint(chem_type,roadfcst[i].road_T -
                                                         ChemConc_Params.ChemSafeTempDelta);
    if ( solution_point > 0.0 && roadfcst[i].FinalChemConc < solution_point  && 
         roadfcst[i].AvailH2O > 0.0 && low_chem < -900 ) {
      proc_log->write_time(1, "Info: Chemicals ineffective after hr %d (ChemConc%) %4.1f  (min%) %4.1f AvailH20(lbs/sqft) %f Prate(mm/hr) %4.3f\n", 
			   i-1, roadfcst[i].FinalChemConc, solution_point, roadfcst[i].AvailH2O, wxfcst[i].LiquidPrecipRate); 
      
      low_chem = i-1;
    }
    else if (low_chem < -900) { // chemicals are effective -- factor in evaporation on available water
      float EvaporationRate = CalcEvaporationRate( wxfcst[i], roadfcst[i] );
      roadfcst[i].AvailH2O = roadfcst[i].AvailH2O * (1 - EvaporationRate);
    }
    // Otherwise chemicals are ineffective and we assume that no evaporation is occurring becuase the precip is
    // frozen. RGH
  
    //
    // Set the type of chemical that should be expected for this hour. Some DOTs may use
    // different chemicals within a storm.
    //
    roadfcst[i].ChemSolutionType = chem_type;
  }

  return(low_chem);
}

//
//
// ChemType: An indexed value indicating the type of chemical being used
// road_T: The temperature of the road surface (deg C)
//   original equation (NaCl)degF: conc = 21.26 - 0.38 * road_T - 0.009 * road_T * road_T;
//
// From FHWA Anti-icing Guidelines
//
// Chemical                         Eutectic Temp        Eutectic Concentration
//                                  degC (degF)          %
// ----------------                 ------------------   ----------------------
// Sodium Chloride (NaCl)           -21.6 (-5.8)         23.3
// Calcium Chloride (CaCl2)         -51 (-60)            29.8
// Magnesium Chloride (MgCl2)       -33 (-28)            21.6
// Calcium Magnesium Acetate (CMA)  -27.5 (-17.5)        32.5 *
// Potassium acetate (KAc)          -60 (-76)            49.0 *
//
//    * Not yet fully supported
// 
// CalcCriticalChemSolutionPoint:
//
// Description: 
//  Calculates the lowest concentration at which a chemical can effectively keep a solution 
//  from freezing give the current temperature of the surface the solution is on.
//
// Imports:
//   ChemType....An index indicating the type of chemical being used.
//   road_T...The temperature of the surface that the chemical is protecting.
//
// Returns:
//   conc: The calculated solution point of the chemical (fractional percentage).
//
// Notes: Unclear if the slope and intercept in the saturation equation are fixed
//        or could be modified -- may need these as parameters.
//
float CalcCriticalChemSolutionPoint( int ChemType, float road_T )
{
  float conc;

  // 
  // Clip the concentration to a small value when pavement temperatures are above 0.0 deg C
  //  if ( road_T > P_RAIN_FRZRAIN_TROAD ) {
  //    conc = C_BAD_CONC;
  //  }
  //  else if ( road_T > 0.0 ) {
  //    conc = C_MIN_ACCEPTABLE_CONC;
  //  }
  //  if ( road_T > 10000.0 ) {
  //    conc = C_MIN_ACCEPTABLE_CONC;
  //  }
  //
  // If the pavement temperature is below the eutectic temperature of the applied chemical
  // then the concentration is a fixed value (and the chemical is ineffective)
  //
  if ( road_T <= GetChemEutecticTemp( ChemType ) ) {
    conc = GetChemEutecticConc( ChemType );
  }
  //
  // Otherwise explicitly calculate the concentration for the specified chemical type
  //
  // These concentrations are calculated from the Solution Concentration curves found on
  // page 53 of the Manual of Practice for an Effective Anti-icing Program. 
  // Publication No. FHWA-RD-95-202  -- June 1996
  //
  else {
    switch (ChemType) {
      case C_CHEMTYPE_NACL:
        conc = -0.02916 * road_T * road_T - 1.7208 * road_T - 0.116;
        break;
        
      case C_CHEMTYPE_MGCL2:
        conc = -0.02 * road_T * road_T - 1.25 * road_T + 0.8214;
        break;
        
      case C_CHEMTYPE_CACL2:
        conc = -0.0116 * road_T * road_T - 1.1058 * road_T + 2.4441;
        break;
        
        // Data for Caliber M1000 from Envirotech Physical Properties document
      case C_CHEMTYPE_CALIBER:
        conc = -0.0051 * road_T * road_T - 0.764 * road_T + 5.2629;
        break;
        
		case C_CHEMTYPE_ICESLICER:  // Copied from MgCl -- no field data
        conc = -0.02 * road_T * road_T - 1.25 * road_T + 0.8214;
        break;

      default:
        conc = -0.02916 * road_T * road_T - 1.7208 * road_T - 0.116;
        // cout << "No such chemical type\n";
        break;
    }
  }
  // Ensure that concentration is above absoulute miniumums
  if ( conc < 0.0 )
    conc = C_MIN_ACCEPTABLE_CONC;

  return conc;
}
//
// CalcCriticalChemSaturationPoint:
//
// Description: 
//  Calculates the highest concentration at which a chemical can effectively keep in solution.
//  Above this level, some chemical remains in raw form.
// 
// Imports:
//   ChemType....An index indicating the type of chemical being used.
//   road_T...The temperature of the surface that the chemical is protecting.
//
// Returns:
//   conc: The calculated saturation point of the chemical.
//
float CalcCriticalChemSaturationPoint( int ChemType, float road_T )
{
  float conc;
  //
  // If the pavement temperature is below the eutectic temperature of the applied chemical
  // then the concentration is a fixed value (and the chemical is ineffective)
  //
  if ( road_T < GetChemEutecticTemp( ChemType ) ) {
    conc = GetChemEutecticConc( ChemType );
  }
  else { // otherwise calculate the conc based on chemical concentration curves
    //
    // These concentrations are calculated from the Solution Concentration curves found on
    // page 53 of the Manual of Practice for an Effective Anti-icing Program. 
    // Publication No. FHWA-RD-95-202  -- June 1996
    //
    switch (ChemType) 
      {

      case C_CHEMTYPE_NACL:
	conc = 26.2 + 0.134 * road_T;
	break;
	
      case C_CHEMTYPE_MGCL2:
	// sat point is 21.6
	conc = 0.0183 * road_T * road_T + 1.4223 * road_T + 48.2365;
	break;
	
      case C_CHEMTYPE_CACL2:
	// sat point is 29.8
	conc = 0.0019 * road_T * road_T + 0.228 * road_T + 36.6794;
	break;
      case C_CHEMTYPE_CALIBER:
	conc = 100.0;
	break;	

      case C_CHEMTYPE_ICESLICER:  
	// sat point is 21.6
	conc = 0.0183 * road_T * road_T + 1.4223 * road_T + 48.2365;
	break;
	
      default:
	conc = 26.2 + 0.134 * road_T;
	//cout << "No such chemical type\n";
	break;
      }
  }

  return conc;
}

//
//  Routine: GetChemEutecticTemp
//
//  Description:
//    Utility routine to return the Eutectic temperature of the indexed chemical type.
//
//  Inputs:
//    ChemType....The type of chemical for which the Eutectic Temp is desired.
//
//  Outputs:
//    none
//
//  Returns:
//    <Eutectic Temperature of the specified chemical>
//
float GetChemEutecticTemp( int ChemType )
{
  switch (ChemType) {
    case C_CHEMTYPE_NACL:
      return( C_NACL_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_MGCL2:
      return( C_MGCL2_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_CACL2:
      return( C_CACL2_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_CMA:
      return( C_CMA_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_KAC:
      return( C_KAC_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_CALIBER:
      return( C_CALIBER_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_ICESLICER:
      return( C_ICESLICER_EUTECTIC_TEMP );
      break;
    case C_CHEMTYPE_ICEBAN:
      return( C_ICEBAN_EUTECTIC_TEMP );
      break;
    default:
      proc_log->write_time("Warning : GetChemEutecticTemp: Bad chemical type specified (%d)\n", ChemType);
      return( C_BAD_TEMP );
      break;
  }
}

//
//  Routine: GetChemEutecticConc
//
//  Description:
//    Utility routine to return the Eutectic concentration of the indexed chemical type.
//
//  Inputs:
//    ChemType....The type of chemical for which the Eutectic Temp is desired.
//
//  Outputs:
//    none
//
//  Returns:
//    <Eutectic concentration of the specified chemical>
//
float GetChemEutecticConc( int ChemType )
{
  switch (ChemType) {
    case C_CHEMTYPE_NACL:
      return( C_NACL_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_MGCL2:
      return( C_MGCL2_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_CACL2:
      return( C_CACL2_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_CMA:
      return( C_CMA_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_KAC:
      return( C_KAC_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_CALIBER:
      return( C_CALIBER_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_ICESLICER:
      return( C_ICESLICER_EUTECTIC_CONC );
      break;
    case C_CHEMTYPE_ICEBAN:
      return( C_ICEBAN_EUTECTIC_CONC );
      break;
    default:
      proc_log->write_time("Warning : GetChemEutecticConc: Bad chemical type specified\n");
      return( C_BAD_CONC );
      break;
  }
}

//
//
//  Routine: ConvertChemUnits()
//
//  Description:
//      Converts the various units of chemical rates to lbs/sq-ft for use internally
//  within the algorithm.
//
//  Inputs:
//     rate........The number of units of chemical to convert.
//     in_units....The type of units that rate is in.
//     out_units...The type of units to convert to.
//
//
float ConvertChemUnits( float rate, int in_chem, int in_units, int out_units )
{
  float lbs_sqft_rate, outrate;
  float Cpcnt, Cwt; 

  //
  // For each chemical we need to know the weight of the liquid solution for
  // wet chemical conversions. This will not be exact -- it assumes 0 degC 
  // solution for instance
  //
  switch ( in_chem )
    {
    case C_CHEMTYPE_NACL:
      Cpcnt = C_NACL_EUTECTIC_CONC/100.0;
      Cwt   = 10.45; //lbs per gallon of solution (ref found)
      break;
    case C_CHEMTYPE_CACL2:
      Cpcnt = C_CACL2_EUTECTIC_CONC/100.0;
      Cwt   = 11.0; //lbs per gallon of solution (ref found)
      break;
    case C_CHEMTYPE_MGCL2:
      Cpcnt = C_MGCL2_EUTECTIC_CONC/100.0;
      Cwt   = 11.0; //lbs per gallon of solution (guesstimate)
      break;
    case C_CHEMTYPE_CMA:
      Cpcnt = C_CMA_EUTECTIC_CONC/100.0;
      Cwt   = 11.0; //lbs per gallon of solution (guesstimate)
      break;
    case C_CHEMTYPE_KAC:
      Cpcnt = C_KAC_EUTECTIC_CONC/100.0;
      Cwt   = 11.0; //lbs per gallon of solution (guesstimate)
      break;
    case C_CHEMTYPE_CALIBER:
      Cpcnt = C_CALIBER_EUTECTIC_CONC/100.0;
      Cwt   = 11.0; //lbs per gallon of solution (guesstimate)
      break;
    case C_CHEMTYPE_ICESLICER:
      Cpcnt = C_ICESLICER_EUTECTIC_CONC/100.0;
      Cwt   = 10.45; //lbs per gallon of solution (guesstimate)
      break;
    case C_CHEMTYPE_ICEBAN:
      Cpcnt = C_ICEBAN_EUTECTIC_CONC/100.0;
      Cwt   = 10.45; //lbs per gallon of solution (guesstimate)
      break;
    default:
      Cpcnt = C_NACL_EUTECTIC_CONC/100.0;
      Cwt   = 10.45; //lbs per gallon of solution (NaCl)
      break;
    }

  // Convert whatever the in_units are to LBS_SQFT
  switch ( in_units )
    {
    case C_CHEMUNITS_LBS_SQFT:
      lbs_sqft_rate = rate;
      break;
    case C_CHEMUNITS_GMS_SQMETER:
      lbs_sqft_rate = rate / (C_LBS_TO_GMS) * C_FT_TO_METERS * C_FT_TO_METERS;
      break;
    case C_CHEMUNITS_KGS_SQMETER:
      lbs_sqft_rate = rate / (C_LBS_TO_KGS) * C_FT_TO_METERS * C_FT_TO_METERS;
      break;
    case C_CHEMUNITS_LBS_LANEMILE:
      lbs_sqft_rate = rate / (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);
      break;
    case C_CHEMUNITS_KGS_LANEKM:
      lbs_sqft_rate = rate / (C_LBS_TO_KGS * C_NOMINAL_LANEWIDTH) * C_FT_TO_METERS / C_KILO;
      break;
    case C_CHEMUNITS_GALS_LANEMILE:
      lbs_sqft_rate = (rate*Cwt*Cpcnt) / (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);
      break;
    case C_CHEMUNITS_LITERS_LANEKM:
      lbs_sqft_rate = (rate*Cwt*Cpcnt) / (C_LBS_TO_KGS * C_NOMINAL_LANEWIDTH) * C_FT_TO_METERS / C_KILO;
      break;
    }

  // Convert the converted lbs/sqft to the expected output units
  switch ( out_units )
    {
    case C_CHEMUNITS_LBS_SQFT:
      outrate = lbs_sqft_rate;
      break;
    case C_CHEMUNITS_GMS_SQMETER:
      outrate = lbs_sqft_rate * C_LBS_TO_GMS / (C_FT_TO_METERS * C_FT_TO_METERS );
      break;
    case C_CHEMUNITS_KGS_SQMETER:
      outrate = lbs_sqft_rate * C_LBS_TO_KGS / (C_FT_TO_METERS * C_FT_TO_METERS );
      break;
    case C_CHEMUNITS_LBS_LANEMILE:
      outrate = lbs_sqft_rate * (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);
      break;
    case C_CHEMUNITS_KGS_LANEKM:
      outrate = lbs_sqft_rate * C_LBS_TO_KGS * C_NOMINAL_LANEWIDTH / C_FT_TO_METERS * C_KILO;
      break;
    case C_CHEMUNITS_GALS_LANEMILE:
      outrate = (lbs_sqft_rate/(Cwt*Cpcnt))* (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);
      break;
    case C_CHEMUNITS_LITERS_LANEKM:
      outrate = (lbs_sqft_rate/(Cwt*Cpcnt)) * C_LBS_TO_KGS * C_NOMINAL_LANEWIDTH / C_FT_TO_METERS * C_KILO;
      break;
    }

  return( outrate );

}


//
//
//  Routine: GetChemUnits()
//
//  Description:
//      Converts the input units string to a known enumeration.
//
//  Inputs:
//     unitsString.......A char string denoting units (e.g., 'lb/lane-mile')
//
//  Returns:
//     The enumerated units value or -1 if the input rate is unknown.
//

int GetChemUnits( char *unitsString )
{

  int units = C_CHEMUNITS_LBS_LANEMILE;  // default


  if (strcmp(unitsString, "lb/ft2") == 0)
    {
      units = C_CHEMUNITS_LBS_SQFT;
    }
  else if (strcmp(unitsString, "g/m2") == 0)
    {
      units = C_CHEMUNITS_GMS_SQMETER;
    }
  else if (strcmp(unitsString, "kg/m2") == 0)
    {
      units = C_CHEMUNITS_KGS_SQMETER;
    }
  else if (strcmp(unitsString, "kg/lane-km") == 0)
    {
      units = C_CHEMUNITS_KGS_LANEKM;
    }
  else if (strcmp(unitsString, "lb/lane-mile") == 0)
    {
      units = C_CHEMUNITS_LBS_LANEMILE;
    }
  else
    {
      proc_log->write_time("Warning : Unknown application_rate units ('%s'), assuming lb/lane-mile.\n", unitsString);
    }

  proc_log->write_time(1, "Info: application rate units: %d\n", units );
  return (units);

}
