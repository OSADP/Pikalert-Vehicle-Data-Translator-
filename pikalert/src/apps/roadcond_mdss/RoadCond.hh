//=======================================================================
//
//   (c) Copyright, 2003-2005 Massachusetts Institute of Technology and
//   National Center for Atmospheric Research
//
//   Permission is herby granted, without written agreement or royalty fee, to use, 
//   copy, modify, and distribute this software and its documentation for any purpose,
//   provided that the above copyright notice and the following three paragraphs
//   appear in all copies of this software.
//
//   IN NO EVENT SHALL M.I.T. or N.C.A.R BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
//   INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
//   AND ITS DOCUMENTATION, EVEN IF M.I.T. or NCAR HAS BEEN ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//
//   M.I.T. and NCAR SPECIFICALLY DISCLAIM ANY WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
//   NON-INFRINGEMENT.
//
//   THE SOFTWARE IS PROVIDED ON AN "AS-IS" BASIS AND M.I.T. and NCAR HAVE NO OBLIGATION TO
//   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//   < END Copyright notice >
//
//=======================================================================
//
//       CVS id "%W% %G% %I% "
//
//       Date           Author                   Action
//       1/1/01         Bill Myers, NCAR         Created
//       7/13/05        Robert Hallowell         Added roadWphase and ChemSolutionType members.
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//
//=======================================================================
//
//
// File: RoadCond.hh
//
// Description:
//
//    Defines the road condition structure used to hold roadway weather and
// contaminant layer values for each time step of the roadway forecast.
//
//=======================================================================
//

#ifndef RoadCond_hh
#define RoadCond_hh

const float CM_TO_IN = 0.393700787;       // cm to inches

// Structure defining the conditions of the road
// Some of these are not used as yet but may be later.
struct roadconditions {
  float road_T;           // Pavement temperature (deg C)
  float subsurface_T;     // Subsurface temperature (deg C)
  int   road_condition;   // METRo value for road_cond
                          //   Dry road = 1
                          //   Wet road = 2
                          //   Ice/Snow on the road = 3
                          //   Mix water/snow on the road = 4
                          //   Dew = 5
                          //   Melting Snow = 6
                          //   Frost = 7
                          //   Icing Rain = 8
  float Tfreezepoint;  // Freeze point of the road surface (taking into account applied chemcicals)
  float PliquidOnRoad; // <reserved> not currently used
  float PsnowOnRoad;   // Snow depth on the road (mm)
  int   roadPtype;     // The type of precipiation hitting the road. Modified from wxfcst to 
  //                      factor in the temperature of the road surface (rain-frz rain)
  int   roadWphase;     // The expected phase of the water on the road given current conditions

  // Variables used in chemical dilution algorithm
  float AvailH2O;        // Water available for chemicals to dissolve into (lbs/sf) 
  float NominalChemConc; // The theoretical chemical concentration (ignoring solution/saturation curve)
  float AvailableChem;   // The amount of pure de-icing chemical available on the road surface. 
  float FinalChemConc;   // The final chemical concentration (factoring in dilution and
                         // and solution/saturation curves
  int   ChemSolutionType; // The type of chemical on the road surface, this may change for each hour depending on
                          // the mix of chemicals being used by the DOT. For example, at hour 6 they might put down
                          // salt and at hour 12 MgCl.

  float Friction;        // <reserved> not currently used
  float NetMobility;     // <reserved> not currently used

  int TrafficIntensity;  // Amount of traffic on roadway (light=1 moderate=2 heavy=3)
};
typedef struct roadconditions RoadConditions;


#endif
