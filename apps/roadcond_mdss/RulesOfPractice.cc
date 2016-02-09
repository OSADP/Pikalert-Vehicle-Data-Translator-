//=======================================================================
//
//   (c) Copyright, 2003 Massachusetts Institute of Technology.
//
//   Permission is hereby granted, without written agreement or royalty fee, to use,
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
//       1/14/01        Robert Hallowell           Created
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//       7/13/05        Robert Hallowell           Added explanantion strings,
//                                                 modified pretreat and treatment strategies.
//
//=======================================================================
//
//
// File: RulesOfPractice.cc
//
// Description:
//
//     This file contains routines for determining the recommended treatment
// of a road surface for anti-icing and de-icing winter storm situations. Many of
// the concepots are based on the FHWA Manual of Practice for an Effective
// Anti-icing Program (FHWA-RD-95-202). The software was deeloped by MIT
// Lincoln Laboratory.
//
// The following routines are included in this file:
//
// StormSumDetermineTreatment..Main routine, given forecasted road and weather conditions 
//                             recommends a treatment plan based, in general, on FHWA guidelines.
// InitNewTreatment............Initializes the values in a Treatment structure.
// PrintTreatmentPlan..........Routine that prints to stdout, the prescribed treatement plan.
// ROP_GetParams...............A routine to extract user defined parameters for the Rules of Practice
//                             algorithm.
// DetermineRoadWaterPhase.....Determine the phase (ice, water, chem-wet, etc) of liquid on road surface.
// FindPlowStartTime...........Find the time when snow on the road exceeds plowing thresholds.
// FindDryRoadTime.............Find the hour at which the roads will be considered "dry".
//
//=======================================================================
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <log/log.hh>

#include "RulesOfPractice.hh"

extern Log *proc_log;

//
// Define the parameters globally
//
ROPParamsStruct ROP_Params;

//
//  Routine: ROP_GetParams()
//
//  Descriptiony:
//      Routine to read user defined rules of practice parameters from a another structure
//
//  Inputs:
//      site_config.......A memory allocated structure containing site-specific ROP defaults
//
//  Outputs:
//      p.................An updated structure containing all the data successfully obtained
//                        from the site config struct
//
//  Returns:
//      status............A status flag (0 = OK, -1 Failure)
//
int ROP_GetParams( SiteLayerTraffic *site_config, ROPParamsStruct *p )
{
  // Initialize parameters to default values
  p->Bare_Pavement_Thresh = P_BARE_PAVEMENT_THRESH;
  p->Snow_Plow_Thresh     = site_config->getSnowPlowThreshold();
  p->RouteRoundtripTiming = site_config->getRouteTreatmentTime();
  p->TruckTurnAroundTime = P_TURNAROUND_TIME; // Time it takes to re-load truck
  p->ChemRateSensitivity = P_CHEMRATE_SENSITIVITY_PCT;
  p->PreRainDelta        = P_PRERAIN_DELTA;
  p->PretreatOffset      = P_PRETREAT_OFFSET * 60.0;
  p->TreatmentStrategy   = site_config->getTreatmentStrategy();


  return(C_STATUS_OK);
}


//
// Module: StormSumDetermineTreatment()
//
//   Description:
//     This is the main routine for determining the next recommended treatement given
//     current and forecasted values for weather and road conditions.
//     This is the next generation of treatement generation it uses data gathered by CharacterizeStorm()
//     to determine what type of treatment should be attempted. Plow only and pre-treatment 
//     recommendations are handled first and then straight chemical applications.
//     Chemical rates are determined by a direct estimate based on hourly precip rates, runoffs, and
//     the specific chemical being applied (based on eutectic curves).
//
//   Inputs:
//     f_start......The timestep at which to start processing.
//     f_end........The timestep at which to end processing.
//     wxfcst.......The current weather forecast structure.
//     roadfcst.....The current road conditions forecast.
//     PrevTreatment..The last treatment that was recommended (will be NULL if no treatments
//                    have been recommended).
//   Outputs:
//     TreatmentPlan..The next recommended treatment (plow, drop chemicals, apply sand)
//   Returns:
//     Treat_Type.....The treatment code type being recommended (eg C_TREAT_TOOCOLD)
//
//
int StormSumDetermineTreatment( int f_start, int f_end, WxForecast *wxfcst,
				RoadConditions *roadfcst,
				StormType *stormsum,
				Treatment *TreatmentPlan, Treatment *PrevTreatment,  
				int num_rc_times )
{
  // Declare and initialize variables
  int   i, chem_status = C_CHEMRATE_OK;
  float max_chemicals=0.0;
  int   treatment;
  int   prev_treatment;
  float total_storm_chemrate = 0.0, new_chemrate = 0.0, treatment_rate = 0.0;
  
  int TripDelta = (int)(0.5+ROP_Params.RouteRoundtripTiming/60.0);
  
  int RouteDelta = (int)(0.5+(ROP_Params.RouteRoundtripTiming+
                              ROP_Params.TruckTurnAroundTime)/60.0);
  // 
  // Determine last hour where either precipitation is falling and/or
  // water is available on the roadway and must be protected from
  // re-freezing.
  //
  int i_first_wetroad = num_rc_times;

  int i_dryroad = FindDryRoadTime( f_start, roadfcst, &i_first_wetroad, num_rc_times );
  
  // First, examine the whole storm to determine if treatment is needed at all, 
  // and if so, what kind of treatment - plowing or chemical application
  //
  chem_status = CalcChemicalRate(i_first_wetroad, i_dryroad, f_end, wxfcst, roadfcst, &total_storm_chemrate);

  // By default assume chemicals will be used

  treatment = C_TREAT;
  TreatmentPlan->ExplanationIndx = 0;
  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];

  TreatmentPlan->Chemicals = ChemConc_Params;

  // Get previous treatment, if any

  if ( PrevTreatment != NULL )
    prev_treatment = PrevTreatment->TreatType;
  else
    prev_treatment = C_NOTREAT_UNDEF;
  
  //
  // Examine forecasted road temperatures during the storm -- if they are too cold then 
  // recommend plow only options.
  //
  switch( stormsum->RoadTrend_StormType ) {
    // If temp are always cold then use a plow only treatment strategy because 
    // chemicals will be ineffective
    case C_ROADT_C:
    case C_ROADT_IC:
      treatment = C_TREAT_PLOWONLY;
      TreatmentPlan->ExplanationIndx = 7;
      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
      proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
      break;
      //
      // For inrange and inrange/cold check for blowing snow conditions. If blowing
      // snow is present recommend plow only option to avoid hazardous re-freezing.
      //
    case C_ROADT_I:
    case C_ROADT_CI:
      if ( stormsum->ColdBS_score > P_ColdBS_final_thresh ) {
        treatment = C_TREAT_PLOWONLY;
        TreatmentPlan->ExplanationIndx = 5;
	TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
      }
      break;
      // If there are warm or inrange/warm temperatures during the storm continue to consider
      // chemical treatments
    case C_ROADT_W:
    case C_ROADT_WI:
    case C_ROADT_IW:
    default:
      // 
      // Check the post storm road temperature trends -- if they are too cold 
      // then only do plowing operations otherwise re-freeze may be a problem.
      //
      switch( stormsum->RoadTrend_PoststormType ) {
        case C_ROADT_C:
          treatment = C_TREAT_PLOWONLY;
          TreatmentPlan->ExplanationIndx = 7;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
          break;
          // Otherwise - all other conditions would indicate chemical treatments are ok.
        case C_ROADT_W:
        case C_ROADT_I:
        case C_ROADT_WI:
        case C_ROADT_IW:
        case C_ROADT_CI:
        default:
          treatment = C_TREAT;
          TreatmentPlan->ExplanationIndx = 0;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
          break;
      }
  }

  //
  // If chemicals are recommended (as opposed to plowonly) then check to see
  // if we need to pretreat.
  //

  if ((stormsum->StartAnyPrecip > 1) && (f_start <= stormsum->StartAnyPrecip) &&
      (total_storm_chemrate > C_CHEM_CHK) && (f_start > P_PRETREAT_OFFSET) &&
      (treatment == C_TREAT) && (prev_treatment != C_PRETREAT))
    {

      treatment     = C_PRETREAT;

      //
      // Examine the storm type (based on precip type) first.
      //
      switch( stormsum->PrecipType_StormType ) {
        // 
        // Any of the following types would warrant pre-treating because the first
        // precipitation is freezing rain. 
        //
        // Warm roads are a suppressor for pretreatment, but, we don't need to check here 
        // because if the precip type is freezing rain then we already know the roads are 
        // not "warm"
        //
        case C_STORM_Z: 
        case C_STORM_ZR: 
        case C_STORM_ZS: 
        case C_STORM_ZZR: 
        case C_STORM_ZZS: 
        case C_STORM_ZRR:
        case C_STORM_ZRZ:
        case C_STORM_ZSS:
        case C_STORM_ZSZ:
          treatment = C_PRETREAT;
          TreatmentPlan->ExplanationIndx = 1;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
          break;
          // 
          // For storms that are all snow or start as snow we need to make sure pre-treating 
          // is warranted. Cold roads combined with strong winds will make applying a 
          // liquid pre-treatment OR starting a regular treatment early potentially disastorous
          // due to blowing snow depositing on the road surface.
          //
        case C_STORM_S: 
        case C_STORM_SZ: 
        case C_STORM_SR: 
        case C_STORM_SSZ:
        case C_STORM_SZZ:
        case C_STORM_SZS:
        case C_STORM_SZR:
	  // Looks like a bug - shouldn't we be looking at the pre-storm road trend, not
	  // in-storm
          //switch( stormsum->RoadTrend_StormType ) {
          switch( stormsum->RoadTrend_PrestormType ) {
            //
            // If the start of the storm had road temperatures that are warm then suppress
            // pre-treatment because it isn't needed.
            //
            case C_ROADT_W:
            case C_ROADT_WI:
              treatment = C_TREAT;
              TreatmentPlan->ExplanationIndx = 14;
	      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	      proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
              break;
              //
              // If the road temps star as inrange, then continue considering pre-treatment.
              //
            case C_ROADT_I:
            case C_ROADT_IW:
              treatment = C_PRETREAT;
              TreatmentPlan->ExplanationIndx = 2;
	      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	      proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
              break;
              // In the case of pre-storm pavement temperatures that lean toward or are
              // too cold for chemicals we need to check wind speeds to make sure 
              // pre-treatment would still be acceptable.
            case C_ROADT_C:
            case C_ROADT_IC:
            case C_ROADT_CI:
              treatment = C_PRETREAT;
              //
              // Check for conditions that would indicate suppression of any pretreatment 
              // application. In this case check for blowing snow across a cold road surface. 
              // Placing chemicals on the road would potentially cause refreezing to occur as 
              // the blowing snow dilutes the low-levels of pretreatment chemicals.
              //
              if ( stormsum->ColdBS_score > P_ColdBS_final_thresh ) {
                treatment = C_TREAT;
                TreatmentPlan->ExplanationIndx = 15;
		TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
		proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
              }
              break;
              // 
              // This shouldn't happen.
            default:
              treatment = C_PRETREAT;
              break;
          }
          break;
          // 
          // All other storm precip categories have leading rain (so don't pre-treat)
        case C_STORM_R:
        case C_STORM_RS:
        case C_STORM_RZ:
        case C_STORM_RRZ:
        case C_STORM_RZZ:
        case C_STORM_RZR:
        case C_STORM_RZS:
        default:
          treatment = C_TREAT;
          TreatmentPlan->ExplanationIndx = 16;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
          break;
      }
    }
  
  //
  // If we are recommending treatments during the storm, first figure out how many 
  // treatments we will need -- then recommend the next treatment.
  //
  if ( treatment == C_TREAT ) {
    // how many more hours are there in this storm?
    //    int time_left = stormsum->EndAnyPrecip - f_start;
    //
    // If the storm starts as rain, hold off treating until the rain is almost 
    // over. This way chemical dilution from rainfall is reduced.
    //
    //    if ( (f_start - RouteDelta) < stormsum->StartFrozenPrecip ) 
    //      f_start = stormsum->StartFrozenPrecip + RouteDelta/2;
    int time_left = i_dryroad - f_start;
    // how many treatments (maximum) will I need?
    float ftreat = (float)time_left / RouteDelta;
    
    new_chemrate = C_BAD_VAL;

    //
    // If the storm is almost over try to put down more chemical to finish up the storm
    // 
    if ( ftreat < 1.5 ) {
      chem_status = CalcChemicalRate(f_start, i_dryroad, f_end, wxfcst, roadfcst, &new_chemrate);
      // we exceeded the maximum chemical allowed so increasing the rate won't work
      if ( new_chemrate > TreatmentPlan->Chemicals.MaxApplicationRate )
        new_chemrate = C_BAD_VAL;
    } 
    
    // 
    // Last, measure how much chemical needed to just make it to the next time
    // the truck can turn around.
    //
    if ( new_chemrate == C_BAD_VAL ) {
      chem_status = CalcChemicalRate(f_start, f_start+RouteDelta, f_end, wxfcst, roadfcst, &new_chemrate);
    }

    if ( chem_status == C_CHEMRATE_THIGH ) {
      treatment = C_NOTREAT_NONEED;
      TreatmentPlan->ExplanationIndx = 11;
      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
      proc_log->write_time(1,"Info: %s at hour %d\n", TreatmentPlan->Explanation.c_str(), f_start);
    }
    else if ( chem_status == C_CHEMRATE_TLOW ) {
      treatment = C_NOTREAT_NONEED;
      TreatmentPlan->ExplanationIndx = 12;
      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
      proc_log->write_time(1,"Info: %s at hour %d\n ", TreatmentPlan->Explanation.c_str(), f_start);
    }

    // Throw out extremely small treatments

    //else if ( new_chemrate < TreatmentPlan->Chemicals.MinApplicationRate/20 ) {
    else if ( new_chemrate < C_CHEM_CHK ) {
      treatment = C_NOTREAT_NONEED;
      TreatmentPlan->ExplanationIndx = 13;
      TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
      proc_log->write_time(1,"Info: %s (%f < %f) at hour %d\n ", TreatmentPlan->Explanation.c_str(), new_chemrate, TreatmentPlan->Chemicals.MinApplicationRate, f_start);
    }

    else {
      proc_log->write_time(1,"Info: Calculated raw chemicals needed for round-trip %6.2f lbs/lane-mile\n",
                           new_chemrate);
    }

    //    if ( new_chemrate > TreatmentPlan->Chemicals.MaxApplicationRate )
    //      TreatmentPlan->AddTandemPlowing = TRUE;

  }

  int plowNeeded;
  plowNeeded = FindPlowStartTime( f_start, roadfcst, num_rc_times );
  if ( plowNeeded == f_start && (treatment < C_TREAT_THRESH || treatment == C_PRETREAT) ) {
    treatment = C_TREAT_PLOWONLY;
    TreatmentPlan->ExplanationIndx = 4;
    TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
    proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
  }

  //
  //  Based on the treatment strategy selected define the next treatment
  //
  //TreatmentPlan->Chemicals = ChemConc_Params;
  int p_start;
  switch ( treatment ) {
    case C_PRETREAT:
      TreatmentPlan->Chemicals = Pretreat_ChemConc_Params;
      TreatmentPlan->TreatType = C_PRETREAT;
      TreatmentPlan->ApplyChemicals = TRUE;
      TreatmentPlan->DoPlowing = TRUE;
      TreatmentPlan->ApplicationRate = Pretreat_ChemConc_Params.MinApplicationRate;

      // With new CharacterizeStorm settings, pretreat runs should start at the
      // pretreatment hour (f_start) so no need to offset in here.
      p_start = f_start - (int)(ROP_Params.PretreatOffset/60.0);
      if ( p_start < 1 )
	p_start = 1;

      TreatmentPlan->StartTime = p_start;
      TreatmentPlan->EndTime = p_start + TripDelta;
			//      TreatmentPlan->EffectiveTime = p_start + RouteDelta;
      TreatmentPlan->EffectiveTime = f_start;

      proc_log->write_time(1,"Info: Trigger pre-treatment from %d to %d\n",
                           TreatmentPlan->StartTime,TreatmentPlan->EndTime);
      break;
    case C_TREAT_PLOWONLY:
      TreatmentPlan->TreatType = C_TREAT_PLOWONLY;
      TreatmentPlan->ApplyChemicals = FALSE;

      //
      // Find the first hour where the snow on the road reaches plowable levels
      //
			//      i = FindPlowStartTime( f_start, roadfcst );
      i = plowNeeded;
      //
      if ( i >= f_start ) {
      //      if ( i >= f_start && i <= f_start+RouteDelta) {
        TreatmentPlan->DoPlowing = TRUE;
        // skip ahead to the point where plowing is needed
        TreatmentPlan->StartTime = i-P_PLOW_OFFSET;
        TreatmentPlan->EndTime = i-P_PLOW_OFFSET+RouteDelta;
        TreatmentPlan->EffectiveTime = i-P_PLOW_OFFSET+RouteDelta;
        if ( TreatmentPlan->StartTime <= 1 && f_end == num_rc_times ) {
          TreatmentPlan->StartTime = 1;
          TreatmentPlan->EndTime = 1 + RouteDelta;
          TreatmentPlan->EffectiveTime = 1 + RouteDelta;
        }
        TreatmentPlan->ExplanationIndx = 3;
	TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
        proc_log->write_time(1, "Info: %s at hour %d\n", TreatmentPlan->Explanation.c_str(), TreatmentPlan->StartTime); 
      }
      //
      // Otherwise - the roads would normally need treatment if we were using chemicals
      // because the roads are refreezing after the storm. However, we have only plowed
      // so the roads should still be dry. (in theory)
      //
      else {
        treatment = C_NOTREAT_NONEED;
        TreatmentPlan->ExplanationIndx = 10;
	TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	proc_log->write_time(1,"Info: %s Snow depth did not reach plow threshold.\n", TreatmentPlan->Explanation.c_str());
	TreatmentPlan->EffectiveTime = f_start+RouteDelta;
      }
      break;
    case C_TREAT:
      max_chemicals = new_chemrate;
      TreatmentPlan->DoPlowing = TRUE;
      TreatmentPlan->ApplyChemicals = TRUE;
      //proc_log->write_time(1,"Info: Drop chemicals at hour %d at %6.2f (%s)\n",
      //                   f_start, max_chemicals, "lbs/lane-mile");

      // hack just to get this to work RGH

      // I am removing the 'hack' since it looks like this condition is caught
      // above, and in testing, it did not change the results. JRC

      //if ( max_chemicals < TreatmentPlan->Chemicals.MinApplicationRate/20) {
      //treatment_rate = 0.0;
      //TreatmentPlan->DoPlowing = FALSE;
      //TreatmentPlan->ApplyChemicals = FALSE;
      //TreatmentPlan->ExplanationIndx = 13;
      //TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
      //proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
      //}
      //else

      { // Need this since we declare variables in this case switch

        // If we recommend treating, make sure the minimum treatment threshold is met
	treatment_rate = max_chemicals;
        
	// Always treat at the minimum if we are going to treat
	if ( treatment_rate < TreatmentPlan->Chemicals.MinApplicationRate )
	  treatment_rate = TreatmentPlan->Chemicals.MinApplicationRate;
	
	float p_round = TreatmentPlan->Chemicals.RoundTreatment;
	
	if ( max_chemicals < TreatmentPlan->Chemicals.MinApplicationRate ) {
	  treatment_rate = TreatmentPlan->Chemicals.MinApplicationRate;
	  TreatmentPlan->ExplanationIndx = 8;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
	}
	else if ( max_chemicals > TreatmentPlan->Chemicals.MaxApplicationRate ) {
	  treatment_rate = TreatmentPlan->Chemicals.MaxApplicationRate;
	  TreatmentPlan->ExplanationIndx = 9;
	  TreatmentPlan->Explanation = treatmentExplanations[TreatmentPlan->ExplanationIndx];
	  proc_log->write_time(1,"Info: %s\n", TreatmentPlan->Explanation.c_str());
	}
	else
	  treatment_rate = p_round * ((int)(max_chemicals/p_round + 1.0));
	
	//
	// If the storm starts as rain, hold off treating until the rain is almost 
	// over. This way chemical dilution is reduced
	//
	int rain_delta = 0;
	if ( (f_start - RouteDelta) < stormsum->StartFrozenPrecip ) 
	  rain_delta = RouteDelta - (int)(ROP_Params.PreRainDelta/60.0);
	
	//	TreatmentPlan->StartTime       = f_start - RouteDelta + rain_delta;
	TreatmentPlan->StartTime       = f_start + rain_delta;
	//TreatmentPlan->EndTime         = f_start + rain_delta;
	TreatmentPlan->EndTime         = f_start + RouteDelta + rain_delta;
	TreatmentPlan->EffectiveTime   = f_start + RouteDelta + 1 + rain_delta;
	TreatmentPlan->TreatType       = C_TREAT;
	TreatmentPlan->ApplicationRate = treatment_rate;
	//
	// If we are applying chemicals then we will also recommend plowing as needed
	//
	TreatmentPlan->DoPlowing = TRUE;
	TreatmentPlan->ApplyChemicals = TRUE;
	
	//
	// Want to make sure that the next treatment doesn't overlap the last one so we
	// keep track of the last treatment's effective time.
	//
	//RGH        int prev_treatment_time = 2;
	int prev_treatment_time = 0;
	if ( PrevTreatment != NULL )
	  prev_treatment_time = PrevTreatment->EffectiveTime;
	
	if ( TreatmentPlan->StartTime <= prev_treatment_time ) {
	  TreatmentPlan->StartTime = prev_treatment_time;
	  TreatmentPlan->EndTime = prev_treatment_time + RouteDelta;
	  TreatmentPlan->EffectiveTime = prev_treatment_time + RouteDelta;
	}
      }
      break;
  default: // C_NOTREAT_NONEED
      TreatmentPlan->DoPlowing = FALSE;
      TreatmentPlan->ApplyChemicals = FALSE;
      TreatmentPlan->StartTime     = f_start;
      // Used to be 'effective' to f_start+RouteDelta, but since there is no
      // treatment, the truck is not out, so there is no RouteDelta to worry about.
      // So just make the 'treatment' effective to the next hour. Seems like this
      // was a bug that might have delayed the first necessary treatment. (Cowie)
      TreatmentPlan->EndTime       = f_start + 1;
      TreatmentPlan->EffectiveTime = f_start + 1;
      break;
  }

  if ( TreatmentPlan->EffectiveTime > num_rc_times ) {
    TreatmentPlan->EffectiveTime = num_rc_times;
  }
  if ( TreatmentPlan->EndTime > num_rc_times ) {
    TreatmentPlan->EndTime = num_rc_times;
  }

  //
  // Return the last value of a treatment
  //
  return(treatment);
  
}


//
// Routine: InitNewTreatment()
//
// Desc:
//       Initialize a treatment structure to nominal (user-defined)
//   and default values. Returns C_STATUS_OK upon successful completion.
//   C_STATUS_BAD if an error occurs.
//
// Inputs:
//   treat.....A treatment structure to initialize.
//
// Outputs: None.
// Returns: None.
//
int InitNewTreatment( Treatment *treat )
{
  treat->StartTime = 0;
  treat->EndTime = 0;
  treat->EffectiveTime = 0;
  treat->DoPlowing = FALSE;
  treat->ApplyChemicals = FALSE;
  treat->ApplicationRate = 0.0;
  treat->Chemicals = ChemConc_Params;
  treat->TreatType = C_NOT_SET;

  return(C_STATUS_OK);
}


//
//  Routine: PrintTreatmentPlan()
//
//  Description:
//     Print a summary of the recommended treatment plan.
//  Inputs:
//    list.....A list of the treatment to be applied.
//    convert..A conversion constant to get the treatement rate (lbs/sf) into the user
//             specified units.
//    units....A test string describing the user defined units.
//
//  Outputs:
//

#ifdef NOTNOW
void PrintTreatmentPlan( Treatment **list, float convert, char *units )
{
  int i;

  if ( list[0] == NULL ) {
    cout << "No treatment recommended\n";
    return;
  }

  cout << "Treatment plan for test data using " << list[0]->Chemicals.ChemName << " :\n";

  // Set the rounding value for recommended treatments from the chemical characteristics
  float p_round = list[0]->Chemicals.RoundTreatment;

  for ( i=0; i<100 && list[i] != NULL; i++ ) {

    if ( list[i]->ApplyChemicals == TRUE ) {
      cout << "Hour " << list[i]->StartTime << " to "
           << list[i]->EndTime
           << " Drop " << convert*p_round*(int)(list[i]->ApplicationRate/p_round)
           << " (" << units << ") and Plow as neccesary\n";
    }
    else if ( list[i]->DoPlowing == TRUE ) {
      cout << "Hour " << list[i]->StartTime << " to "
           << list[i]->EndTime
           << " Plow route\n";
    }
  }
}
#endif

void PrintTreatmentPlan( Treatment *list, int num_treatment, float convert, char *units )
{
  int i;

  proc_log->write_time(2,"Info: Treatment plan:\n");

	int total_treat = 0;
  for ( i=0; i<num_treatment; i++ ) {
    if ( list[i].ApplyChemicals == TRUE ) {
      total_treat++;
      if ( list[i].Chemicals.UserChemForm == C_CHEMFORM_LIQUID ) 
	proc_log->write_time(2,"Info: Hour %d to %d Drop %.1f (%s) and Plow as neccesary\n",
			     list[i].StartTime, list[i].EndTime, list[i].ApplicationRate, "gals/lane-mile");
      else 
	proc_log->write_time(2,"Info: Hour %d to %d Drop %.1f (%s) and Plow as neccesary\n",
			     list[i].StartTime, list[i].EndTime, list[i].ApplicationRate, units);
    }
    else if ( list[i].DoPlowing == TRUE ) {
      total_treat++;
      proc_log->write_time(2,"Info: Hour %d to %d Plow route\n", list[i].StartTime, 
                           list[i].EndTime);
    }
  }
  if (total_treat == 0)
    {
      proc_log->write_time(2,"Info: No Treatments Required\n");
    }

  return;
}


//
//=======================================================================
//
//
// File: DetermineRoadWaterPhase
//
// Description:
//
// Road water phase is designed to show the state of the water/ice/snow on the road surface.
// This factors in current precip (Ptype), road temperature (roadPtype) and the chemical
// freeze point of any solution on the road. 
//
// The various types of phase type are:
//
// C_RTYPE_DRY:  No precipitation is falling and the amount of available water on the road is
//               below the C_AVAILH2O_THRESH
// C_RTYPE_WET:  Either the type of precipitation is rain and temps are above freeze points, or
//               water is sitting on the road but above the P_RAIN_FRZRAIN_TROAD threshold.
// C_RTYPE_SNOW: Snow is falling or snow has fallen and no chemicals have been applied.
// C_RTYPE_ICE:  Freezing rain is falling and no treatment has been applied or water is sitting
//               on a road where the pavement temp is below the chemical freezepoint.
// C_RTYPE_CHEMWET: 
//
// Inputs:
//
//     current_hour...The current hour to be examined for road water phase
//     wxfcst.........The current weather forecast structure.
//     roadfcst.......The current road conditions forecast.
//
// Outputs:
//
//     roadfcst.......The current road conditions forecast with roadWphase modified.
//
// Returns:
//
//     roadWphase.....The current phase of any road water at this hour.
//
//
int DetermineRoadWaterPhase( int current_hour, WxForecast *wxfcst, RoadConditions *roadfcst )
{
    int wxfcst_ptype     = wxfcst[current_hour].Ptype;
    int roadPtype        = roadfcst[current_hour].roadPtype;
    float roadfcst_ptemp = roadfcst[current_hour].road_T;
    int roadWphase       = C_RTYPE_DRY;
    float availH2O       = roadfcst[current_hour].AvailH2O;
    float chemConc       = roadfcst[current_hour].FinalChemConc;

    // Road water phase is designed to show the state of the water/ice/snow on the road surface.
    // This factors in current precip (Ptype), road temperature (roadPtype) and the chemical
    // freeze point of any solution on the road. 
    //
    // The various types of phase type are:
    //
    // C_RTYPE_DRY:  No precipitation is falling and the amount of available water on the road is
    //               below the C_AVAILH2O_THRESH
    // C_RTYPE_WET:  Either the type of precipitation is rain and temps are above freeze points, or
    //               water is sitting on the road but above the P_RAIN_FRZRAIN_TROAD threshold.
    // C_RTYPE_SNOW: Snow is falling or snow has fallen and no chemicals have been applied.
    // C_RTPYE_ICE:  Freezing rain is falling and no treatment has been applied or water is sitting
    //               on a road where the pavement temp is below the chemical freezepoint.
    // C_RTYPE_CHEMWET: 


    int last_phase = C_RTYPE_DRY;
    if ( current_hour > 0 )
      last_phase = roadfcst[current_hour-1].roadWphase;

    // No precip and no water on road
    if ( roadPtype == C_PTYPE_NONE && availH2O < C_AVAILH2O_THRESH ) {
      roadWphase = C_RTYPE_DRY;
    }
    // If there is some chemical on the road then it will affect phase on the road 
    else if ( chemConc > 0.0 ) {
      float solution_point = CalcCriticalChemSolutionPoint(ChemConc_Params.UserChemType,
                                                           roadfcst[current_hour].road_T -
							   ChemConc_Params.ChemSafeTempDelta);
      // Assume chemically wet
      roadWphase = C_RTYPE_CHEMWET;

      // Check for failed chemicals
      if ( solution_point > 0.0 && chemConc < solution_point ) {
	roadWphase = C_RTYPE_CHEMICE;
      }
    }

    // There are no chemicals on the road, when precip is occuring phase will be
    // whatever the precip occurring is except when the road temperature tells us it
    // should be different (Modified to include pavement temp 5/16/07 RGH)
    else if ( roadPtype == C_PTYPE_SNOW || roadPtype == C_PTYPE_MIXED) {
      if ( roadfcst[current_hour].road_T > P_FRZPOINT_WATER )
	roadWphase = C_RTYPE_WET;
      else
	roadWphase = C_RTYPE_SNOW;
    }
    else if ( roadPtype == C_PTYPE_FRZRAIN || roadPtype == C_PTYPE_SLEET) {
      if ( roadfcst[current_hour].road_T > P_FRZPOINT_WATER )
	roadWphase = C_RTYPE_WET;
      else
	roadWphase = C_RTYPE_ICE;
    }
    else if ( roadPtype == C_PTYPE_RAIN ) {
      if ( roadfcst[current_hour].road_T <= P_FRZPOINT_WATER )
	roadWphase = C_RTYPE_ICE;
      else
	roadWphase = C_RTYPE_WET;
    }

    // No precip is falling and we know it isn't dry (otherwise the initial if would have 
    // triggered) there are also no chemicals so conditions are only impacted by road temperature
    else if ( roadPtype == C_PTYPE_NONE ) {
      roadWphase = last_phase;
      // uncommented RGH
      if ( roadfcst[current_hour].road_T > P_FRZPOINT_WATER )
        roadWphase = C_RTYPE_WET;
      else if ( last_phase == C_PTYPE_SNOW )
        roadWphase = C_RTYPE_SNOW;
      else
        roadWphase = C_RTYPE_ICE;
    }
    else {
      // unclear that we should get here -- print an error
      proc_log->write_time("Error: Undefined roadWphase in DetermineWaterPhase(). Declaring road dry\n");
      roadWphase = C_RTYPE_DRY;
    }

    roadfcst[current_hour].roadWphase = roadWphase;

    return(roadWphase);
}

int DeterminePrecipType( int current_hour, WxForecast *wxfcst, RoadConditions *roadfcst )
{
    // Freezing rain precip type:
    // 1) If the precip type is "freezing rain" from the models then trigger if the road is at
    //    P_FRZRAIN_FRZRAIN_TROAD or less.
    // 2) If the model precip type is "rain" then trigger if temp is at or below 
    //    P_RAIN_FRZRAIN_TROAD
    //
    // Set current Ptype based on above rules.
    //
    int Ptype            = wxfcst[current_hour].Ptype;
    int wxfcst_ptype     = wxfcst[current_hour].Ptype;
    float roadfcst_ptemp = roadfcst[current_hour].road_T;

    if ( wxfcst_ptype == C_PTYPE_FRZRAIN && roadfcst_ptemp < P_FRZRAIN_FRZRAIN_TROAD) {
      Ptype = C_PTYPE_FRZRAIN;
    }
    else if ( wxfcst_ptype == C_PTYPE_RAIN && roadfcst_ptemp <= P_RAIN_FRZRAIN_TROAD ) {
      Ptype = C_PTYPE_FRZRAIN;
    }
    else if ( wxfcst_ptype == C_PTYPE_FRZRAIN ) {
      Ptype = C_PTYPE_RAIN;
    }

    // Look over the past hour to see if freezing rain or freezing roads have been reported
    //    if ( Ptype == C_PTYPE_RAIN || Ptype == C_PTYPE_NONE ) {
    if ( Ptype == C_PTYPE_RAIN ) {
      // Note current_hour will be > 0
      wxfcst_ptype   = wxfcst[current_hour-1].Ptype;
      roadfcst_ptemp = roadfcst[current_hour-1].road_T;

      if ( wxfcst_ptype == C_PTYPE_FRZRAIN && roadfcst_ptemp < P_FRZRAIN_FRZRAIN_TROAD) {
        Ptype = C_PTYPE_FRZRAIN;
      }
      else if ( wxfcst_ptype == C_PTYPE_RAIN && roadfcst_ptemp <= P_RAIN_FRZRAIN_TROAD ) {
        Ptype = C_PTYPE_FRZRAIN;
      }
      else if ( wxfcst_ptype == C_PTYPE_FRZRAIN ) {
        Ptype = C_PTYPE_RAIN;
      }
    }

    // 
    // Set new precip type in road condition forecast structure
    //
    roadfcst[current_hour].roadPtype = Ptype;
    
    return ( Ptype );
}


//
// Routine: FindPlowStartTime()
//
// Description:
//   Find the first hour (after the start time) where snow on the road 
//   reaches levels that should trigger plowing.
//
// Inputs:
//
//   start........The hour to start the search.
//   rfcst........The full array of road condition forecast information.
// 
// Outputs: NONE
//
// Returns:
//  
//   plow_trigger...The hour at which snow is considered plowable.
//
//
//////////////////////////////////////////////////////////////////////////   
//
int FindPlowStartTime( int start, RoadConditions *rfcst, int num_rc_times )
{
  int i, plow_trigger = C_BAD_VAL;
  int i_max_snow = C_BAD_VAL;
  float max_snow = 0.0;

  for (i=start; i<num_rc_times && plow_trigger == C_BAD_VAL; i++) {
   if ( rfcst[i].PsnowOnRoad > ROP_Params.Snow_Plow_Thresh ) {
      plow_trigger = i;
    }
    if ( rfcst[i].PsnowOnRoad > max_snow ) {
      i_max_snow = i;
      max_snow = rfcst[i].PsnowOnRoad;
    }
  }

  //  if ( plow_trigger == C_BAD_VAL )
  //  plow_trigger = num_rc_times;

  return(plow_trigger);
}
//
// Routine: FindDryRoadTime()
//
// Description:
//   Find the hour at which the roads are dry and the road is dry (as measured
//   by AvailH2O.
//
// Inputs:
//
//   start........The hour to start the search, typically set to the end of 
//                precipitation in the storm.
//   rfcst........The full array of road condition forecast information.
// 
// Outputs: NONE
//
// Returns:
//  
//   dryroad......The hour at which the road is consired dry.
//
//
//////////////////////////////////////////////////////////////////////////   
//
int FindDryRoadTime( int start, RoadConditions *rfcst, int *first, int num_rc_times )
{
  int i, dryroad = num_rc_times;
  
  *first = num_rc_times;

  for (i=start; i<num_rc_times; i++) {
    if ( rfcst[i].AvailH2O > C_AVAILH2O_THRESH && *first == num_rc_times ) {
      *first = i;
    }
    else if ( rfcst[i].AvailH2O < C_AVAILH2O_THRESH && dryroad == num_rc_times && i > start) {
      dryroad = i;
    }

  }
  
  return(dryroad);
}
