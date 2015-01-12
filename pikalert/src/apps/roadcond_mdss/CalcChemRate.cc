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
//       11/07/04       Robert Hallowell           Created
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//
//=======================================================================
//
//    CalcBlowingSnowFactor.........Calculate 
//
//

#include "RulesOfPractice.hh"
//
float CalcBlowingSnowFactor( int index )
{
  float factor = 1.0 + (index*index)*P_BlowingSnowChemAdjustment;  
  return(factor);
}
//
// File: CalcChemRate.cc
//
// Description:
//
//
//=======================================================================
//

//
//  Routine: CalcChemRate()
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
//    FcstStartTime..The first timestep to process.
//    FcstEndTime....The final timestep to process.
//    wxfcst.........An array containing the pertinent weather variables for predicting road
//                   conditions after treatment.
//    roadfcst.......An array containing the pertinent road condition variables for predicting
//                   road condtions after treatment (Tpavement, PsnowOnRoad, TrafficIntensity).
//    chem_id........The type of de-icing chemical to apply/dilute.
//
//  Outputs:
//    NONE
//
//  Returns:
//    chem_rate.....The amount of chemical neccessary to keep road from freezing.
//
int CalcChemicalRate(int FcstStartTime, int FcstEndTime, int fcst_end,
                       WxForecast *wxfcst, RoadConditions *roadfcst, float *chemrate )
{
  //
  // Declare and initialize variables
  //
  int i;
  int StartTime, EndTime;
  float LastChemRate=0.0, ChemRate=0.0, ChemInSolution=0.0;
  float SatPoint=0.0, SolutionPoint=0.0, ChemPoint=0.0;
  float AvailH2O = 0.0, AvailChem = 0.0;
  float tfactor;
  float CumulativeRunoff = 1.0, CumulativeTraffic = 1.0;
  int status_flag = 0;


  StartTime = FcstStartTime; // initialize to fcst start time
  EndTime = FcstEndTime;

  for ( i=StartTime; i<EndTime && i<fcst_end; i++ ) {

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
    AvailH2O = roadfcst[i].AvailH2O;

    //
    // What to do if no amount of chemical will keep ice from forming?
    // RGH - this needs to be handled differently
    if ( roadfcst[i].road_T <= ChemConc_Params.MinTemp ) {
			status_flag = C_CHEMRATE_TLOW;
    }
    else if ( roadfcst[i].road_T > ChemConc_Params.MaxTemp ) {
      status_flag = C_CHEMRATE_THIGH;
    }
    else {
      //
      // Calculate the amount of chemical needed for melting this hour
      //
      SatPoint = CalcCriticalChemSaturationPoint( ChemConc_Params.UserChemType, 
						  roadfcst[i].road_T - 
						  ChemConc_Params.ChemSafeTempDelta);
      
      SolutionPoint = CalcCriticalChemSolutionPoint( ChemConc_Params.UserChemType, 
						     roadfcst[i].road_T -
						     ChemConc_Params.ChemSafeTempDelta );
      
      //
      // Calculate the amount of chemical that we would need to have in solution in order to
      // keep the solution from freezing or saturating. Split the difference between sat and solution
      // point to keep solution mid-range.
      // The amount of chemical that will runoff in solution is also factored in, increasing the 
      // amount of chemical needed this hour.
      //
      // Factor for adjusting the sensitivity of the system, a 0.0 would weight the value towards
      // the solution turning to ice a 1.0 weights the value towards too much chemical.
      //
      
      float Sensitivity = ROP_Params.ChemRateSensitivity;
      //
      // If the precipitation type is freezing rain then increase the amount of chemical that
      // will be required on the road surface
      //
      if ( roadfcst[i].roadPtype == C_PTYPE_FRZRAIN )
	Sensitivity *= P_FRZRAIN_FACTOR;
      
      ChemPoint = SolutionPoint + Sensitivity;
      if ( ChemPoint > SatPoint )
	ChemPoint = SatPoint;
      
      // Determine the amount (lbs/sf) of chemical needed based on eutectic curve
      //    ChemInSolution = ( AvailH2O * ChemPoint / 100.0 ) / ( 1.0 - ChemPoint / 100.0 ) /
      ChemInSolution = ( AvailH2O * ChemPoint / 100.0 ) / 
	(1-ChemConc_Params.RoadChemSolutionRunoffPerHour);

      // 
      // So, given the current water on the road and temperature of the road we need
      // ChemInSolution chemical available on the road surface to keep ice from forming.
      
      // Adjust chemical rate to account for current chemicals on the road surface
      // at the start of the event. We may already have chemical on the road surface
      // so we subtract that amount from the overall chemical needed.
      //
      if (roadfcst[i].AvailableChem >= 0.0 && roadfcst[i].AvailableChem < 999 ) 
	ChemRate = ChemInSolution - roadfcst[i].AvailableChem;
      else
	ChemRate = ChemInSolution;
      
      //			cout << ChemRate << " " << roadfcst[i].AvailableChem << "first chemrate\n";
      // Adjust chemical rate to account for chemicals applied based on this runs 
      // recommendations. For example if the user puts down 500 lbs/lane-mile at
      // StartTime that chemical is still available in the next hour, and so forth.
      // RGH Take this out? AvailChem was simply set to zero anyway.
      //
      //     ChemRate = ChemRate - AvailChem*(1-ChemConc_Params.RoadChemSolutionRunoffPerHour);
      
      //
      // Make an adjustment based on the blowing snow potential.
      //
      float BSfactor = CalcBlowingSnowFactor( wxfcst[i].blowing_snow_potential );
      
      ChemRate = ChemRate * BSfactor;
      
      //
      // If this hour's needed ChemRate is lower than the LastChemRate then do nothing
      // we still have enough chemical on the road.
      if ( ChemRate <= LastChemRate ) {
	LastChemRate = LastChemRate;
      }
      // Otherwise, need to adjust the original ChemRate higher to handle forecasted conditions
      // and runoff and traffic from the original treatment point
      else {
	LastChemRate += ChemRate / (CumulativeRunoff*CumulativeTraffic);
      }
      //
      // Set the amount of available chemical for next hour (adjusted by potential losses)
      //
      AvailChem = LastChemRate*(1-ChemConc_Params.RoadChemSolutionRunoffPerHour);
      
      CumulativeRunoff  *= (1-ChemConc_Params.RoadChemSolutionRunoffPerHour);
      CumulativeTraffic *= (1-CalcTrafficFactor( roadfcst[i].TrafficIntensity ));
    }
  }

  *chemrate = LastChemRate;

  if (LastChemRate == 0.0 && status_flag < 0 ) {
    return(status_flag);
  }
  //
  // Increase the amount of chemical required to account for the initial loss as the 
  // truck drops the chemical.
  //
  LastChemRate /= (1.0 - ChemConc_Params.RoadChemAppInitialLossRate);
  
  //
  // Convert from lbs/sf to lbs/lane-mile
  //
  LastChemRate *= (C_NOMINAL_LANEWIDTH * C_MILES_TO_FEET);
  
  *chemrate = LastChemRate;
  
  return(0);
}










