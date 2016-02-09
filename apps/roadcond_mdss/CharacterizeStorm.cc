//=======================================================================
//
//   (c) Copyright, 2003 Massachusetts Institute of Technology.
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
//       11/08/03       Robert Hallowell           Created
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//       3/5/04         Robert Hallowell           Bug fix in calculating blowing snow -- added
//                                                 check to ensure that frozen precip was occurring
//                                                 This calculation will be improved when we start 
//                                                 using blowing snow algorithm output. RGH
//       4/1/05         Robert Hallowell           Utilize blowing snow potential (NCAR alg) for 
//                                                 cold road/blowing snow potential
//       7/13/05        Robert Hallowell           Added interpolation of preciptypes in times
//                                                 of no precip (helps adjust storm type)
//
//=======================================================================
//
//
// File: CharacterizeStorm.cc
//
// Description:
//
//     This file contains routines for determining the general storm charactersistics of
// a winter event. Each forecast perido is scanned to find the beginning and ending of an
// event, it's phase changes (rain-to-snow, frzrain-rain, etc), intensity changes, and road 
// temperature characteristics (before, during and after the precipitation).
// These charactersistics are then used to help define the type and level of treatment
// that should be recommended for this storm.
//
// The following routines are included in this file:
//
// CharacterizeStorm.....Main routine, given forecasted road and weather conditions determine
//                       the relevant storm characteristics.
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
StormTypeKernel StormKernel[C_N_STORM_TYPES];
StormTypeKernel RoadKernel[C_N_ROADT_TYPES];

float StormKernelScore[C_N_STORM_TYPES];
float PreRoadKernelScore[C_N_ROADT_TYPES];
float RoadKernelScore[C_N_ROADT_TYPES];
float PostRoadKernelScore[C_N_ROADT_TYPES];


//
//  Routine: InitStormTypeKernel()
//
//  Description:
//      Routine to initialize fuzzy kernels for storm precip type.
//      Each kernel characterizes the possible changes of phase from snow to freezing
//      rain to rain in various combinations (up to 3 phase changes).
//
//  Inputs:
//      kernel........The array of kernel types to fill.
//
//  Outputs:
//      kernel........The initialized kernel types.
//
//  Returns:
//      status............A status flag (0 = OK, -1 Failure)
//

int InitStormTypeKernel()
{
  StormKernel[C_STORM_NA].score = 0.0;  
  StormKernel[C_STORM_NA].num_periods = 0;  
  StormKernel[C_STORM_NA].period[0] = C_PTYPE_NONE;
  StormKernel[C_STORM_NA].name = "Cannot determine storm type.";

  // Start with all rain, all frz rain, all snow

  StormKernel[C_STORM_R].score = 0.0;  
  StormKernel[C_STORM_R].num_periods = 1;  
  StormKernel[C_STORM_R].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_R].name = "All rain event.";

  StormKernel[C_STORM_Z].score = 0.0;  
  StormKernel[C_STORM_Z].num_periods = 1;  
  StormKernel[C_STORM_Z].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_Z].name = "All freezing rain event.";

  StormKernel[C_STORM_S].score = 0.0;  
  StormKernel[C_STORM_S].num_periods = 1;  
  StormKernel[C_STORM_S].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_S].name = "All snow event.";

  // Next look at single phase change storms

  StormKernel[C_STORM_RZ].score = 0.0;  
  StormKernel[C_STORM_RZ].num_periods = 2;  
  StormKernel[C_STORM_RZ].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RZ].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RZ].name = "Rain changing to freezing rain.";

  StormKernel[C_STORM_RS].score = 0.0;  
  StormKernel[C_STORM_RS].num_periods = 2;  
  StormKernel[C_STORM_RS].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RS].period[1] = C_PTYPE_SNOW;
  StormKernel[C_STORM_RS].name = "Rain changing to snow.";

  StormKernel[C_STORM_ZR].score = 0.0;  
  StormKernel[C_STORM_ZR].num_periods = 2;  
  StormKernel[C_STORM_ZR].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZR].period[1] = C_PTYPE_RAIN;
  StormKernel[C_STORM_ZR].name = "Freezing rain to rain.";

  StormKernel[C_STORM_ZS].score = 0.0;  
  StormKernel[C_STORM_ZS].num_periods = 2;  
  StormKernel[C_STORM_ZS].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZS].period[1] = C_PTYPE_SNOW;
  StormKernel[C_STORM_ZS].name = "Freezing rain to snow.";

  StormKernel[C_STORM_SR].score = 0.0;  
  StormKernel[C_STORM_SR].num_periods = 2;  
  StormKernel[C_STORM_SR].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SR].period[1] = C_PTYPE_RAIN;
  StormKernel[C_STORM_SR].name = "Snow changing to rain.";

  StormKernel[C_STORM_SZ].score = 0.0;  
  StormKernel[C_STORM_SZ].num_periods = 2;  
  StormKernel[C_STORM_SZ].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SZ].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SZ].name = "Snow changing to freezing rain.";

  // Finally look at triple phase storms

  StormKernel[C_STORM_RRZ].score = 0.0;  
  StormKernel[C_STORM_RRZ].num_periods = 3;  
  StormKernel[C_STORM_RRZ].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RRZ].period[1] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RRZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RRZ].name = "Mostly rain ending in freezing rain.";

  StormKernel[C_STORM_RZZ].score = 0.0;  
  StormKernel[C_STORM_RZZ].num_periods = 3;  
  StormKernel[C_STORM_RZZ].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RZZ].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RZZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RZZ].name = "Starting as rain then mostly freezing rain.";

  StormKernel[C_STORM_RZR].score = 0.0;  
  StormKernel[C_STORM_RZR].num_periods = 3;  
  StormKernel[C_STORM_RZR].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RZR].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RZR].period[2] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RZR].name = "Starting and ending as rain with intermittent freezing rain.";

  StormKernel[C_STORM_RZS].score = 0.0;  
  StormKernel[C_STORM_RZS].num_periods = 3;  
  StormKernel[C_STORM_RZS].period[0] = C_PTYPE_RAIN;
  StormKernel[C_STORM_RZS].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_RZS].period[2] = C_PTYPE_SNOW;
  StormKernel[C_STORM_RZS].name = "Rain to freezing rain to snow.";

  StormKernel[C_STORM_ZZR].score = 0.0;  
  StormKernel[C_STORM_ZZR].num_periods = 3;  
  StormKernel[C_STORM_ZZR].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZZR].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZZR].period[2] = C_PTYPE_RAIN;
  StormKernel[C_STORM_ZZR].name = "Freezing rain ending as rain.";

  StormKernel[C_STORM_ZZS].score = 0.0;  
  StormKernel[C_STORM_ZZS].num_periods = 3;  
  StormKernel[C_STORM_ZZS].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZZS].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZZS].period[2] = C_PTYPE_SNOW;
  StormKernel[C_STORM_ZZS].name = "Freezing rain ending as snow.";

  StormKernel[C_STORM_ZRR].score = 0.0;  
  StormKernel[C_STORM_ZRR].num_periods = 3;  
  StormKernel[C_STORM_ZRR].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZRR].period[1] = C_PTYPE_RAIN;
  StormKernel[C_STORM_ZRR].period[2] = C_PTYPE_RAIN;
  StormKernel[C_STORM_ZRR].name = "Starting as freezing rain and then turning mostly to rain event";

  StormKernel[C_STORM_ZRZ].score = 0.0;  
  StormKernel[C_STORM_ZRZ].num_periods = 3;  
  StormKernel[C_STORM_ZRZ].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZRZ].period[1] = C_PTYPE_RAIN;
  StormKernel[C_STORM_ZRZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZRZ].name = "Mostly freezing rain with intermittent rain.";

  StormKernel[C_STORM_ZSS].score = 0.0;  
  StormKernel[C_STORM_ZSS].num_periods = 3;  
  StormKernel[C_STORM_ZSS].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZSS].period[1] = C_PTYPE_SNOW;
  StormKernel[C_STORM_ZSS].period[2] = C_PTYPE_SNOW;
  StormKernel[C_STORM_ZSS].name = "Starting as freezing rain and then turning mostly to snow.";

  StormKernel[C_STORM_ZSZ].score = 0.0;  
  StormKernel[C_STORM_ZSZ].num_periods = 3;  
  StormKernel[C_STORM_ZSZ].period[0] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZSZ].period[1] = C_PTYPE_SNOW;
  StormKernel[C_STORM_ZSZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_ZSZ].name = "Beginning and ending as freezing rain with intermittent snow.";

  StormKernel[C_STORM_SSZ].score = 0.0;  
  StormKernel[C_STORM_SSZ].num_periods = 3;  
  StormKernel[C_STORM_SSZ].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SSZ].period[1] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SSZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SSZ].name = "Mostly snow ending as freezing rain.";

  StormKernel[C_STORM_SZZ].score = 0.0;  
  StormKernel[C_STORM_SZZ].num_periods = 3;  
  StormKernel[C_STORM_SZZ].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SZZ].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SZZ].period[2] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SZZ].name = "Starting as snow and then turning mostly to freezing rain.";

  StormKernel[C_STORM_SZS].score = 0.0;  
  StormKernel[C_STORM_SZS].num_periods = 3;  
  StormKernel[C_STORM_SZS].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SZS].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SZS].period[2] = C_PTYPE_RAIN;
  StormKernel[C_STORM_SZS].name = "Beginning and ending as snow with intermittent freezing rain.";

  StormKernel[C_STORM_SZR].score = 0.0;  
  StormKernel[C_STORM_SZR].num_periods = 3;  
  StormKernel[C_STORM_SZR].period[0] = C_PTYPE_SNOW;
  StormKernel[C_STORM_SZR].period[1] = C_PTYPE_FRZRAIN;
  StormKernel[C_STORM_SZR].period[2] = C_PTYPE_RAIN;
  StormKernel[C_STORM_SZR].name = "Snow to freezing rain to rain.";

  return(C_STATUS_OK);
}

//
//  Routine: InitRoadTypeKernel()
//
//  Description:
//      Routine to initialize fuzzy kernels for road temperature trending summary.
//      Each kernel characterizes the possible changes of temperature band from warm
//      to in-chemical-range to too cold for chemicals.
//
//  Inputs:
//      kernel........The array of kernel types to fill.
//
//  Outputs:
//      kernel........The initialized kernel types.
//
//  Returns:
//      status............A status flag (0 = OK, -1 Failure)
//

int InitRoadTypeKernel( StormTypeKernel kernel[] )
{

  // Start with all warm roads, all in-range roads and all too cold roads
  kernel[C_ROADT_NA].score = 0.0;  
  kernel[C_ROADT_NA].num_periods = 0;  
  kernel[C_ROADT_NA].period[0] = C_ROADT_W_INDEX;
  kernel[C_ROADT_NA].name = "Could not determine road trends.";
  kernel[C_ROADT_NA].kerneltype = C_SCALED_KERNEL;

  // Start with all warm roads, all in-range roads and all too cold roads
  kernel[C_ROADT_W].score = 0.0;  
  kernel[C_ROADT_W].num_periods = 1;  
  kernel[C_ROADT_W].period[0] = C_ROADT_W_INDEX;
  kernel[C_ROADT_W].name = "Roads always/mostly warm.";
  kernel[C_ROADT_W].kerneltype = C_SCALED_KERNEL;

  kernel[C_ROADT_I].score = 0.0;  
  kernel[C_ROADT_I].num_periods = 1;  
  kernel[C_ROADT_I].period[0] = C_ROADT_I_INDEX;
  kernel[C_ROADT_I].name = "Roads always/mostly in chemical range.";
  kernel[C_ROADT_I].kerneltype = C_SCALED_KERNEL;

  kernel[C_ROADT_C].score = 0.0;  
  kernel[C_ROADT_C].num_periods = 1;  
  kernel[C_ROADT_C].period[0] = C_ROADT_C_INDEX;
  kernel[C_ROADT_C].name = "Roads always/mostly too cold.";
  kernel[C_ROADT_C].kerneltype = C_SCALED_KERNEL;

  // Next look at dual phase storms

  kernel[C_ROADT_WI].score = 0.0;  
  kernel[C_ROADT_WI].num_periods = 2;  
  kernel[C_ROADT_WI].period[0] = C_ROADT_W_INDEX;
  kernel[C_ROADT_WI].period[1] = C_ROADT_I_INDEX;
  kernel[C_ROADT_WI].name = "Roads start warm and end in chemical range.";
  kernel[C_ROADT_WI].kerneltype = C_SCALED_KERNEL;

  kernel[C_ROADT_IC].score = 0.0;  
  kernel[C_ROADT_IC].num_periods = 2;  
  kernel[C_ROADT_IC].period[0] = C_ROADT_I_INDEX;
  kernel[C_ROADT_IC].period[1] = C_ROADT_C_INDEX;
  kernel[C_ROADT_IC].name = "Roads start in range and end too cold.";
  kernel[C_ROADT_IC].kerneltype = C_SCALED_KERNEL;

  kernel[C_ROADT_IW].score = 0.0;  
  kernel[C_ROADT_IW].num_periods = 2;  
  kernel[C_ROADT_IW].period[0] = C_ROADT_I_INDEX;
  kernel[C_ROADT_IW].period[1] = C_ROADT_W_INDEX;
  kernel[C_ROADT_IW].name = "Roads start in-range and end warm.";
  kernel[C_ROADT_IW].kerneltype = C_SCALED_KERNEL;

  kernel[C_ROADT_CI].score = 0.0;  
  kernel[C_ROADT_CI].num_periods = 2;  
  kernel[C_ROADT_CI].period[0] = C_ROADT_C_INDEX;
  kernel[C_ROADT_CI].period[1] = C_ROADT_I_INDEX;
  kernel[C_ROADT_CI].name = "Roads start too cold and end in-range.";
  kernel[C_ROADT_CI].kerneltype = C_SCALED_KERNEL;

  // Finally look at triple phase storms

  // Nothing here for now.
  return(C_STATUS_OK);

}
//
// CharacterizeStorm()
//
// Description:
//   This routine is used to summarize the type of winter storm that is forecasted.
//   The characterization includes the start/stop and total storm length. Road temperature
//   and precipitation type regimes. This structure is then used to determine the appropriate
//   overall treatment strategy.
//
// Inputs:
//   roadfcst......The forecasted road pavement conditions.
//   wxfcst........The forecasted ambient weather conditions.
//
// Outputs: <NONE>
//   stormType.....A structure containing storm summary information.
//
// Returns:
//   status........A flag indicating the routine was error free (1) or not (0)
//

int CharacterizeStorm( RoadConditions *roadfcst, WxForecast *wxfcst, StormType *stormType, int num_rc_times )
{
  int i = 0, j = 0, stop_count = 0, storm_length = 0, period, last = 0;
  int frz_stop_count = 0, frz_last = 0;
  int current_Ptype, ptypes[num_rc_times+1], FinalType = C_BAD_VAL;
	int last_Ptype;
  float tvals[num_rc_times+1];

  InitStormType( stormType );

  //
  // Load roadPtype with road specific precip type
  //
  for (i=1; i<num_rc_times; i++)
    current_Ptype = DeterminePrecipType( i, wxfcst, roadfcst );    

  //
  // Loop over the entire storm to determine characteristics of any oncoming storm.
  // Start by determining the start/stop of the storm (based on precip type) and loading 
  // arrays with various storm parameters.
  // Specifically the type of precipitation event and the road temperature trends.
  //

  for (i=1; i<num_rc_times && stormType->EndAnyPrecip == C_BAD_VAL; i++) {

    // Determine the start point of the storm (for now just look at precip type)

    current_Ptype = roadfcst[i].roadPtype;

    if ( current_Ptype != C_PTYPE_NONE && stormType->StartAnyPrecip == C_BAD_VAL ) {
      stormType->TotalLiquidPrecip += wxfcst[i].LiquidPrecipRate;
      stormType->StartAnyPrecip = i;
      ptypes[j] = current_Ptype;
      tvals[j] = roadfcst[i].road_T;
      last_Ptype = current_Ptype;
      j++;
    }
    else if (stormType->StartAnyPrecip != C_BAD_VAL) {
      stormType->TotalLiquidPrecip += wxfcst[i].LiquidPrecipRate;
      ptypes[j] = current_Ptype;      
      tvals[j] = roadfcst[i].road_T;
      last_Ptype = current_Ptype;
      j++;
    }

    if ( current_Ptype >= C_PTYPE_SNOW && stormType->StartFrozenPrecip == C_BAD_VAL ) {
      stormType->StartFrozenPrecip = i;
      if ( current_Ptype == C_PTYPE_FRZRAIN )
        stormType->TotalFrozenPrecip += wxfcst[i].LiquidPrecipRate;
      else
        stormType->TotalFrozenPrecip += wxfcst[i].FrozenPrecipRate;
    }

    //
    // If a start point exists then attempt to determine the end point
    //
    if ( stormType->StartAnyPrecip > C_BAD_VAL && current_Ptype == C_PTYPE_NONE ) {
      stop_count++;
      last = i;
    }
    else {
      //
      // Interpolate between temporary start/stop for ptype
      for ( int kk=stop_count; kk>0; kk-- )
	ptypes[j-kk] = last_Ptype;

      stop_count = 0;
    }

    //
    // If a start point of frozen precip exists then attempt to determine the end point
    //
    if ( stormType->StartFrozenPrecip > C_BAD_VAL && 
         (current_Ptype == C_PTYPE_NONE || current_Ptype == C_PTYPE_RAIN) ) {
      frz_stop_count++;
      frz_last = i;
    }
    else {
      frz_stop_count = 0;
    }
    if ( stormType->EndFrozenPrecip == C_BAD_VAL && 
         stormType->StartFrozenPrecip > C_BAD_VAL && current_Ptype != C_PTYPE_RAIN ) {
        stormType->TotalFrozenPrecip += wxfcst[i].FrozenPrecipRate;
    }

    // 
    // If we have had enough hours of no weather -- declare the event finished
    //
    if ( stop_count > P_MAX_STOP_COUNT ) {
      stormType->EndAnyPrecip    = i-P_MAX_STOP_COUNT;
      stormType->EndFrozenPrecip = i-frz_stop_count;
    }

    // Examine water on road surface, as well
    if ( stormType->StartWetRoad == C_BAD_VAL && roadfcst[i].AvailH2O > C_AVAILH2O_THRESH )
      stormType->StartWetRoad = i;
		/*
    else if ( stormType->EndWetRoad == C_BAD_VAL && 
		stormType->StartWetRoad != C_BAD_VAL &&
		roadfcst[i].AvailH2O <= C_AVAILH2O_THRESH ) {
		stormType->EndWetRoad = i;
		}
		*/     
  }

  //
  // If a storm is not found then return without further processing.
  //
  if ( stormType->StartAnyPrecip == C_BAD_VAL ) {
    stormType->StormTypeText = "No storm expected.";
    return(C_STATUS_OK);
  }

  // End of storm limit never triggered, but the forecast time was exceeded so
  // set stop point to last precip hour.
  //
  if ( stormType->EndAnyPrecip == C_BAD_VAL ) {
    stormType->EndAnyPrecip = num_rc_times - stop_count;
    if ( stormType->EndFrozenPrecip == C_BAD_VAL )
      stormType->EndFrozenPrecip = stormType->EndAnyPrecip;
  }
  if ( stormType->EndFrozenPrecip == C_BAD_VAL )
    stormType->EndFrozenPrecip = num_rc_times - frz_stop_count;

  //
  // Need to determine when the road dries out after the storm
  if ( stormType->StartWetRoad != C_BAD_VAL ) {
    stormType->EndWetRoad = stormType->EndAnyPrecip;
    for (i=1; i<num_rc_times; i++) {
      if ( roadfcst[i].AvailH2O > C_AVAILH2O_THRESH ) {
	stormType->EndWetRoad = i;
      }
    }
  }

  storm_length = stormType->EndAnyPrecip - stormType->StartAnyPrecip;

  // 
  // Examine the precip type to determine the phase trends of the storm. Snow-rain, frz rain to
  // snow, etc.
  //

  // Now build appropriate size kernels for determining storm type
  InitStormTypeKernel();

  float hit_score = 1.0/storm_length;
  float maxScore = 0.0;
  FinalType = C_STORM_NA;
  for (i=0; i<C_N_STORM_TYPES-1; i++) {
    for ( j=0; j<storm_length; j++ ) {
      if (j <= storm_length/StormKernel[i].num_periods) 
        period = 0;
      else if (j > 2*storm_length/StormKernel[i].num_periods)
        period = 2;
      else
        period = 1;

      if ( ptypes[j] == StormKernel[i].period[period] ) 
        StormKernel[i].score += hit_score;
    }
    if (StormKernel[i].score > maxScore) {
      FinalType = i;
      maxScore = StormKernel[i].score;
    }

  }

  proc_log->write_time( 4, "Final storm type: %d (weight = %4.2f)\n",FinalType, maxScore);
  stormType->PrecipType_StormType = FinalType; 
  stormType->StormTypeText += StormKernel[FinalType].name;
 
  // 
  // Next determine the precipitation rate trending for the storm environment (this gets 
  // superimposed on the type trending to create a consistent storm picture).
  //

  //
  // Determine the road temperature trending. This extends the analysis to look at pre- and 
  // post-storm road temperatures  
  //

  // Now build appropriate size kernels for determining road temperature trends
  InitRoadTypeKernel( RoadKernel );

  int road_storm_length = storm_length;
  int start_road = stormType->StartAnyPrecip - P_ROADT_NUM_PRE;
  if ( start_road > stormType->StartWetRoad && stormType->StartWetRoad != C_BAD_VAL )
    start_road = stormType->StartWetRoad;

  int end_road = stormType->EndAnyPrecip + P_ROADT_NUM_POST;
  if ( end_road < stormType->EndWetRoad && stormType->EndWetRoad != C_BAD_VAL )
    end_road = stormType->EndWetRoad;

  if ( start_road < 1 )
    start_road = 1;
  if ( end_road >= num_rc_times )
    end_road = num_rc_times - 1;

  stormType->BeginLookout = start_road;
  stormType->EndLookout   = end_road;

  // 
  // Check for conditions that may require a suppression of treatment.
  // Cold roads with blowing snowblowing snow threat during storm (crude).
  //
  int k = 0;
  int s = 0;
  stormType->ColdBS_score = C_BAD_VAL;
  if ( stormType->StartFrozenPrecip != C_BAD_VAL ) {
    for ( j=stormType->StartFrozenPrecip; j<stormType->EndFrozenPrecip; j++ ) {
      s += FuzzyScoreLookup( wxfcst[j].blowing_snow_potential, 1, 0, 2, 25, 3, 100 );
      s += FuzzyScoreLookup( (int)(10*roadfcst[j].road_T), 
                             (int)(10*P_ColdBS_lowT_thresh), 100, 
                             (int)(10*P_ColdBS_T_thresh), 25, 0, -100); 
      s += FuzzyScoreLookup( road_storm_length, 1, 100, 4, 50, 8, -50 );
      
      if ( roadfcst[j].roadPtype == C_PTYPE_SNOW ) 
        s += 100;
      else if ( roadfcst[j].roadPtype == C_PTYPE_NONE )
        s += 50;
      else
        s += 0;
      
    }
    // Calculate 0.0 to 1.0 value indicating likeliehood of need for treatment
    // suppression
    int k = stormType->EndFrozenPrecip-stormType->StartFrozenPrecip;
    stormType->ColdBS_score = s / (k*4*100.0);
  }
  proc_log->write_time(4, "Final blowing snow on cold roads score: %4.2f\n",stormType->ColdBS_score);

  // 
  // Check road temperature trends in pre-storm environment.
  //
  // Initialize the kernal
  //
  for (int k=0; k<C_N_ROADT_TYPES; k++)
    PreRoadKernelScore[k] = 0.0;

  for ( j=start_road; j<stormType->StartAnyPrecip; j++ ) {
    PreRoadKernelScore[C_ROADT_W] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                        C_ROADT_W_INDEX );
    PreRoadKernelScore[C_ROADT_I] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                        C_ROADT_I_INDEX );
    PreRoadKernelScore[C_ROADT_C] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                        C_ROADT_C_INDEX );
  }
  PreRoadKernelScore[C_ROADT_W] /= ((stormType->StartAnyPrecip-start_road)*100.0);
  PreRoadKernelScore[C_ROADT_I] /= ((stormType->StartAnyPrecip-start_road)*100.0);
  PreRoadKernelScore[C_ROADT_C] /= ((stormType->StartAnyPrecip-start_road)*100.0);

  // This looks like a bug - if you start with warm road score but an undefined type,
  // then you can never get a warm road type. Changed to match what is done for the
  // post storm case, ie set to 0.0.

  //float PremaxScore = PreRoadKernelScore[C_ROADT_W];
  float PremaxScore = 0.0;
  int PreFinalType = C_ROADT_NA;
  if (PreRoadKernelScore[C_ROADT_W] > PremaxScore) {
    PreFinalType = C_ROADT_W;
    PremaxScore = PreRoadKernelScore[C_ROADT_W];
  }
  if (PreRoadKernelScore[C_ROADT_I] > PremaxScore) {
    PreFinalType = C_ROADT_I;
    PremaxScore = PreRoadKernelScore[C_ROADT_I];
  }
  if (PreRoadKernelScore[C_ROADT_C] > PremaxScore) {
    PreFinalType = C_ROADT_C;
    PremaxScore = PreRoadKernelScore[C_ROADT_C];
  }
  proc_log->write_time( 4, "Final pre-storm road trend type: %d (weight = %4.2f)\n",PreFinalType, PremaxScore);
  stormType->RoadTrend_PrestormType = PreFinalType;
  stormType->StormTypeText += " Pre-storm: " + RoadKernel[PreFinalType].name;
  if (start_road == stormType->StartAnyPrecip)
    {
      stormType->StormTypeText += " (Storm started immediately.) ";
    }


  //
  // Check road temperature trends during storm.
  //
  // Initialize the kernal
  //
  for (int k=0; k<C_N_ROADT_TYPES; k++)
    RoadKernelScore[k] = 0.0;

  maxScore = 0.0;
  FinalType = C_ROADT_NA;

  for (i=0; i<C_N_ROADT_TYPES-1; i++) {
    for ( j=0; j<road_storm_length; j++ ) {
      if (j <= road_storm_length/RoadKernel[i].num_periods) 
        period = 0;
      else if (j > 2*road_storm_length/RoadKernel[i].num_periods)
        period = 2;
      else
        period = 1;
      RoadKernelScore[i] += SelectFuzzyScore( (int)rint(tvals[j]), RoadKernel[i].period[period] );
    }

    RoadKernelScore[i] /= (road_storm_length*100.0);
    if (RoadKernelScore[i] > maxScore) {
      FinalType = i;
      maxScore = RoadKernelScore[i];
    }
    
  }

  proc_log->write_time( 4, "Final in-storm road trend type: %d (weight = %4.2f)\n",FinalType, maxScore);
  stormType->RoadTrend_StormType = FinalType;
  stormType->StormTypeText += " In-storm: " + RoadKernel[FinalType].name;

  // 
  // Check road temperature trends in post-storm environment.
  //
  // Initialize the kernal
  //
  for (int k=0; k<C_N_ROADT_TYPES; k++)
    PostRoadKernelScore[k] = 0.0;

  for ( j=stormType->EndAnyPrecip; j<end_road; j++ ) {
    PostRoadKernelScore[C_ROADT_W] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                         C_ROADT_W_INDEX );
    PostRoadKernelScore[C_ROADT_I] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                         C_ROADT_I_INDEX );
    PostRoadKernelScore[C_ROADT_C] += SelectFuzzyScore( (int)rint(roadfcst[j].road_T), 
                                                           C_ROADT_C_INDEX );
  }
  PostRoadKernelScore[C_ROADT_W] /= ((end_road - stormType->EndAnyPrecip)*100.0);
  PostRoadKernelScore[C_ROADT_I] /= ((end_road - stormType->EndAnyPrecip)*100.0);
  PostRoadKernelScore[C_ROADT_C] /= ((end_road - stormType->EndAnyPrecip)*100.0);
  float PostmaxScore = 0.0;
  int PostFinalType = C_ROADT_NA;
  if (PostRoadKernelScore[C_ROADT_W] > PostmaxScore) {
    PostFinalType = C_ROADT_W;
    PostmaxScore = PostRoadKernelScore[C_ROADT_W];
  }
  if (PostRoadKernelScore[C_ROADT_I] > PostmaxScore) {
    PostFinalType = C_ROADT_I;
    PostmaxScore = PostRoadKernelScore[C_ROADT_I];
  }
  if (PostRoadKernelScore[C_ROADT_C] > PostmaxScore) {
    PostFinalType = C_ROADT_C;
    PostmaxScore = PostRoadKernelScore[C_ROADT_C];
  }
  proc_log->write_time( 4, "Final post-storm road trend type: %d (weight = %4.2f)\n",PostFinalType, PostmaxScore);
  stormType->RoadTrend_PoststormType = PostFinalType;
  stormType->StormTypeText += " Post-storm: " + RoadKernel[PostFinalType].name;

  return(C_STATUS_OK);
}

int StormRoadWaterPhase( int nt_start, RoadConditions *roadfcst, WxForecast *wxfcst, int num_rc_times )
{
  int i = 0;
  int current_phase;

  for (i=0; i<num_rc_times; i++) {
    current_phase = DetermineRoadWaterPhase( i, wxfcst, roadfcst );    
    proc_log->write_time( 4, "Hour %d Ptype %d rate %4.3f roadPtype %d waterPhase %d  roadTemp %5.2f  H2O %7.5f  Chem %6.3f AvailChem %6.5f\n",i,wxfcst[i].Ptype,wxfcst[i].LiquidPrecipRate, roadfcst[i].roadPtype,roadfcst[i].roadWphase,roadfcst[i].road_T,roadfcst[i].AvailH2O,roadfcst[i].FinalChemConc,roadfcst[i].AvailableChem);
  }

  return(C_STATUS_OK);
}

//${
//=======================================================================
//
//    FUNCTION: FillFuzzyLookup
//
//    DESCRIPTION:
//
//
//    INPUTS:
//
//
//    OUTPUTS:
//
//
//    RETURNS:
//
//
//=======================================================================
//
//           


int FuzzyScoreLookup( int testval, int anchor1, int score1, int anchor2, int score2, int anchor3, int score3 )
{
  int Fscore = 0;

  if ( testval <= anchor1 )
    Fscore = score1;
  else if ( testval > anchor1 & testval <= anchor2 ) 
    Fscore = score1 + (int)((float)((float)(testval - anchor1) / (anchor1-anchor2)) * (score1-score2));
  else if ( testval > anchor2 & testval <= anchor3 ) 
    Fscore = score2 + (int)((float)((float)(testval - anchor2) / (anchor2-anchor3)) * (score2-score3));
  else if ( testval >= anchor3 )
    Fscore = score3;
  else
    Fscore = 0;

  return(Fscore);
}

int SelectFuzzyScore( int testval, int FuzzyIndex )
{
  int score = 0;

  if ( FuzzyIndex == C_ROADT_W_INDEX ) 
    score = FuzzyScoreLookup( testval, P_ROADT_TOOCOLD, 0, P_ROADT_INRANGE, 0, P_ROADT_WARM, 100 );
  else if ( FuzzyIndex == C_ROADT_I_INDEX ) 
    score = FuzzyScoreLookup( testval, P_ROADT_TOOCOLD, 0, P_ROADT_INRANGE, 100, P_ROADT_WARM, 0 );
  else if ( FuzzyIndex == C_ROADT_C_INDEX ) 
    score = FuzzyScoreLookup( testval, P_ROADT_TOOCOLD, 100, P_ROADT_INRANGE, 0, P_ROADT_WARM, 0 );
  else
    score = 0;

  return(score);
}


int InitStormType( StormType *storm )
{
  storm->BeginLookout             = C_BAD_VAL;
  storm->EndLookout               = C_BAD_VAL;
  storm->StartWetRoad             = C_BAD_VAL;
  storm->EndWetRoad               = C_BAD_VAL;
  storm->StartAnyPrecip           = C_BAD_VAL;
  storm->EndAnyPrecip             = C_BAD_VAL;
  storm->TotalLiquidPrecip        = 0.0;
  storm->StartFrozenPrecip        = C_BAD_VAL;
  storm->EndFrozenPrecip          = C_BAD_VAL;
  storm->TotalFrozenPrecip        = 0.0;
  storm->RoadTrend_PrestormType   = C_BAD_VAL;
  storm->RoadTrend_PoststormType  = C_BAD_VAL;
  storm->RoadTrend_StormType      = C_BAD_VAL;
  storm->PrecipType_StormType     = C_BAD_VAL;
  storm->PrecipRate_StormType     = C_BAD_VAL;

  return(C_STATUS_OK);
}


int PrintStormType( StormType *storm )
{
  proc_log->write_time( 8, "Storm type; Lookout: Begin %d End \n", storm->BeginLookout, storm->EndLookout);
  proc_log->write_time( 8, "Storm type; WetRoad: Start %d End \n", storm->StartWetRoad, storm->EndWetRoad);
  proc_log->write_time( 8, "Storm type; AnyPrecip: Start %d End \n", storm->StartAnyPrecip, storm->EndAnyPrecip);
  return(C_STATUS_OK);
}
