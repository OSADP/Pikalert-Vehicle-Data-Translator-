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
//       CVS ID is " %W% %G% %I% "
//
//
//       Date           Author                   Action
//
//       1/14/01        Robert Hallowell         Created
//                      MIT Lincoln Laboratory
//                      (781)981-3645
//                      bobh@ll.mit.edu
//       8/24/02        Robert Hallowell         Final update for release.
//       8/18/03        Robert Hallowell         Updated comments.
//
//=======================================================================
//
//
// File: RulesOfPractice.h
//
// Description:
//
//    Contains the constants (C_) and parameters (P_) needed for the rules
// of practice treatment recommendations and the chemical concentration/dilution
// algorithms. In addition, the file contains structure definitions and module
// argument declarations.
//
//=======================================================================
//

#ifndef RulesOfPractice_hh
#define RulesOfPractice_hh

#include <stdio.h>
#include <iostream>
#include <alloca.h>
#include <fstream>
#include <sstream>
#include <string>

#include "WxFcst.hh"
#include "RoadCond.hh"
#include "SiteLayerTraffic.hh"

using namespace std;

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE  (1)
#endif

#define PARSE_BOTH 0
#define PARSE_SNOW_ONLY 1
#define PARSE_TEMP_ONLY 2


#define NO_TREATMENT 0
#define SUGGESTED_TREATMENT 1
#define USER_DEFINED_TREATMENT 2
#define SUFFICIENT_TREATMENT 3

#define P_SNOW_SUPPRESSION 0.05  // mm of snow

#define C_ONTRIGGER  0
#define C_CONTINUOUS 1
//
// Notation: #define Variables prefixed by C_ are constants, P_ indicates a route adaptable
//           parameter.
//
// Units: By default parameters are in MKS units unless otherwise specified.
//
//
// Subroutine return status variables:
//
#define C_STATUS_OK   0
#define C_STATUS_BAD -1
//
#define C_CHEMRATE_OK 0
#define C_CHEMRATE_THIGH -1
#define C_CHEMRATE_TLOW -2
// Data bad data check value
//
#define C_BAD_VAL -999
#define C_BAD_TEMP -999
#define C_BAD_CONC -999

// Storm Types

#define C_N_STORM_TYPES 24

#define C_STORM_R    0
#define C_STORM_Z    1
#define C_STORM_S    2
#define C_STORM_RS   3
#define C_STORM_RZ   4
#define C_STORM_SZ   5
#define C_STORM_SR   6
#define C_STORM_ZR   7
#define C_STORM_ZS   8

#define C_STORM_RRZ  9
#define C_STORM_RZZ  10
#define C_STORM_RZR  11
#define C_STORM_RZS  12
#define C_STORM_ZZR  13
#define C_STORM_ZZS  14
#define C_STORM_ZRR  15
#define C_STORM_ZRZ  16
#define C_STORM_ZSS  17
#define C_STORM_ZSZ  18
#define C_STORM_SSZ  19
#define C_STORM_SZZ  20
#define C_STORM_SZS  21
#define C_STORM_SZR  22
#define C_STORM_NA   23

// Road temperature kernel types

#define C_N_ROADT_TYPES 8
#define C_N_ROADT_TYPES_SINGLE 3

#define C_ROADT_W    0
#define C_ROADT_I    1
#define C_ROADT_C    2
#define C_ROADT_WI   3
#define C_ROADT_IC   4
#define C_ROADT_IW   5
#define C_ROADT_CI   6
#define C_ROADT_NA   7 // Not available, could not calculate the value

#define C_ROADT_W_INDEX 0
#define C_ROADT_I_INDEX 1
#define C_ROADT_C_INDEX 2

#define C_BS_INDEX 0

#define C_EXACT_KERNEL  0
#define C_SCALED_KERNEL 1

#define P_ROADT_NUM_PRE  6
#define P_ROADT_NUM_POST 6

// 10DegC might be too warm
//#define P_ROADT_WARM 10
#define P_ROADT_WARM 5

#define P_ROADT_INRANGE -5

// (see below -- made global based on chemical type)
//#define P_ROADT_TOOCOLD -15

// Precipitation types
// Determined by using % probabilities of each type, for now only three of these types
// are used activelty: rain, snow, and freezing rain. It is unclear if a "mixed" or "sleet" 
// identifiers would be useful for this application (perhaps useful for chemical conc or 
// snow depth calculations or for a more complicated treatment module that uses chemical dilution).
//
#define C_PTYPE_NONE 0
#define C_PTYPE_RAIN 1
#define C_PTYPE_SNOW 2
#define C_PTYPE_MIXED 3 // snow/rain
#define C_PTYPE_SLEET 4 // snow/rain
#define C_PTYPE_FRZRAIN 5
#define C_PTYPE_WETROAD 6  // Used to indicate that the road is wet -- not a valid type for precipitation type
//                            only roadPtype
#define C_N_PTYPES 6

//
// Define a new set of codes to determine the phase of the water sitting on the road
//
#define C_RTYPE_DRY     0
#define C_RTYPE_WET     1
#define C_RTYPE_CHEMWET 2
#define C_RTYPE_CHEMICE 3
#define C_RTYPE_SLUSH   4
#define C_RTYPE_SNOW    5
#define C_RTYPE_ICE     6

#define C_N_RTYPES 6

#define C_MAX_CHEMNAME  30  // Maximum number of characters for the chemical name (salt)
#define C_MAX_TREATNAME 30  // Maximum number of characters for the chemical rate name (lbs/lanemile)

//
// The maximum number of forecast time periods, and the nominal number of forecasts
//
//#define C_MAXFCST_TIME 72 // This not used anywhere
//#define C_NOMINAL_FCST 72 // This is now set dynamically in road_cond.cc

// Threshold limits for the available chemical and water on the road surface. When respective 
// array values reach these levels they are reset to "0".
//
#define C_NOCHEM_THRESH 0.0001  // in lbs/sf
//#define C_AVAILH2O_THRESH 0.00005 // in lbs/sf
//#define C_AVAILH2O_THRESH 0.0005 // in lbs/sf
#define C_AVAILH2O_THRESH 0.0005 // in lbs/sf

// A default value returned by CalcCriticalChemSolutionPoint() as a minimum solution concentration
//#define C_MIN_ACCEPTABLE_CONC 0.05 // in fractional percentage  (concentration)
#define C_MIN_ACCEPTABLE_CONC -0.0001 // in fractional percentage  (concentration)

// Used ir rules of practice as nominal "zero threshold" for deployed chemicals
#define C_CHEM_CHK 0.01 // in P_TREAT_UNITS

//// Parameters

// Storm type thresholds and settings

#define P_MAX_STOP_COUNT 12
#define P_MAX_PERIODS    3


// Rules of practice thresholds and settings. These are old defaults. Values are now
// read in via the site config file.

//#define P_TREAT_UNITS "lbs/lane-mile"
//#define P_ROUND_TREAT 20       // round treatment to nearest interval (P_TREAT_UNITS)
//#define P_MIN_TREAT 60        // the lowest treatment rate to be recommended (P_TREAT_UNITS)
//#define P_MAX_TREAT 200        // the highest treatment rate to be recommended (P_TREAT_UNITS)
//#define P_TREAT_UNITS_LIQ "gals/lane-mile"
//#define P_ROUND_TREAT_LIQ 10       // round treatment to nearest interval (P_TREAT_UNITS_LIQ)
//#define P_MIN_TREAT_LIQ   40       // the lowest treatment rate to be recommended (P_TREAT_UNITS_LIQ)
//#define P_MAX_TREAT_LIQ  100       // the highest treatment rate to be recommended (P_TREAT_UNITS_LIQ)
//#define P_ROUTE_TIMING 180     // the average time it takes to plow a route (minutes)


#define P_TURNAROUND_TIME 30   // the time it takes to reload a truck (minutes)
#define P_PRERAIN_DELTA 120    // time offset for treat start when rain precedes event
//#define P_CHEMRATE_SENSITIVITY 1.0 // number between 0 and 1, 1 yields higher chem rates based on saturation curve, 
                                     // 0 lower based on solution curve
#define P_CHEMRATE_SENSITIVITY_PCT 2.0 // A sensitivity value based on a percentage point offset from the solution curve. 
                                       // This replaces the straight CHEMRATE_SENSITIVITY parameter.
#define P_FRZRAIN_FACTOR 1.25

// Following are thresholds for suppressing pre-treatment applications when cold road and blowing
// snow are expected
#define P_ColdBS_T_thresh -6.7     // degC equivalent of 20 degF threshold for 20/15 rule.
#define P_ColdBS_lowT_thresh -17.8 // degC equivalent of 0 degF threshold for 20/15 rule low temp.
#define P_ColdBS_final_thresh 0.6
// 
// Current system only allows 1 type of chemical application (dry salt lbs/lane-mile), to 
// represent a pre-treatment with liquid brine we must recommend the equivalent dry salt treatment.
// We also need a pretreatment offset calculation so that the treatment is recommended well in 
// advance of the storm system.
//
#define P_LIQUID_BRINE_EQUIV 110.0  // The equivalent dry salt app (lbs/lane-mile) of a 50 gal/lane-mile treatment.
#define P_LIQUID_BRINE_OFFSET 6 // The minimum number of hours of data needed to declare a liquid
                                // brine pretreat event. (apparently not used)
//#define P_PRETREAT_OFFSET 6 // The minimum number of hours of data needed to declare a liquid
#define P_PRETREAT_OFFSET 4 // The minimum number of hours of data needed to declare a liquid
                                // brine pretreat event.

// Parameter to control any potential offset from the plowing trigger, currently plow strating at
// the trigger time. If a user wanted to trigger on 3" of snow, but begin plowing the hour the
// previous hour this variable would be set to 1.
//
#define P_PLOW_OFFSET 0 // Offset in hours from plowing trigger to begin plowing. (0 for now)

//
// Base factor for adjusting treatment recommendations to account for blowing snow conditions.
// A number greater than 1.0 yields higher rates, less than 1.0 lower.
//
#define P_BlowingSnowChemAdjustment 0.05; // A base 5% increase

// Precipitation definition rates inches/hr of liquid equivalent precipitation.
// Used to modify the recommended spread rate based on precip intensity
// Typically moderate would yield no change (1.0), light precip would reduce recommendation,
// heavy precip would increase recommendation
// Freezing rain rates are handled differently because even small amounts of freezing rain 
// may cause slippery conditions.
//
#define P_LGT_PRECIP_RATE 0.10  // Lowest threshold for precip to be considered
#define P_MOD_PRECIP_RATE 0.25  // Threshold for moderate precipitation
#define P_HVY_PRECIP_RATE 0.75  // Threshold for heavy precipitation
#define P_RDELTA (P_HVY_PRECIP_RATE - P_LGT_PRECIP_RATE) // Pre calculate range to simplify
//                                                          ratio calcualtion
#define P_LGT_FRZRAIN_RATE 0.001  // Lowest threshold for precip to be considered
#define P_MOD_FRZRAIN_RATE 0.03  // Threshold for moderate precipitation
#define P_HVY_FRZRAIN_RATE 0.75  // Threshold for heavy precipitation
#define P_FRZ_DELTA (P_HVY_FRZRAIN_RATE - P_LGT_FRZRAIN_RATE) // Pre calculate range to simplify
//
// Use these multipliers to adjust for light to heavy precip snow rates
//
#define P_LGT_PRECIP_FACTOR 0.65  // Reduce recommendation for light precip
#define P_HVY_PRECIP_FACTOR 1.35  // Increase recommendation for heavy precip
#define P_FDELTA (P_HVY_PRECIP_FACTOR - P_LGT_PRECIP_FACTOR) // Pre-calculate range to simplify
//                                                              ratio calculation
//
// Potential return treatment status
//
#define C_TREAT_NOPRECIP   5
#define C_PRETREAT         4 // Apply chemicals
#define C_NOPRETREAT_ColdBS_RULE 3 // winds > 15 mph temps < 15 degF pretreating results in wet/refrozen roads
#define C_TREAT            2 // Apply chemicals
#define C_TREAT_PLOWONLY   1 // Plow only
#define C_TREAT_THRESH     0 // Check value to determine if a treatment has been recommended
#define C_NOTREAT_NONEED  -1 // No chemical treatment needed because there is no snow on the road
#define C_NOTREAT_TOOCOLD -2 // No chemical treatment needed because the temperatures are too cold
#define C_NOTREAT_TOOWARM -3 // No chemical treatment needed because the temperatures are too warm
#define C_NOTREAT_UNDEF     -9 // Unknown (this should not happen)
#define C_NOT_SET         -10  // Initialized value for TreatType


// Possible chemical rate units
#define C_CHEMUNITS_LBS_SQFT       0   // pounds per square foot - native units for internal calculations
#define C_CHEMUNITS_GMS_SQMETER    1   // grams per square meter 
#define C_CHEMUNITS_KGS_SQMETER    2   // kilograms per square meter 
#define C_CHEMUNITS_LBS_LANEMILE   3   // pounds per lane-mile
#define C_CHEMUNITS_KGS_LANEKM     4   // kilograms per lane-kilometer
#define C_CHEMUNITS_GALS_LANEMILE  5   // gallons per lane-mile
#define C_CHEMUNITS_LITERS_LANEKM  6   // liters per lane-kilometer

// Conversion constants
#define C_KILO 1000                     // A kilo of something
#define C_InHr_to_LbSf (double)5.2      // 62.4/12.0  Convert Inches/Hour to lbs/square-foot
#define C_mmHr_to_LbSf (double)0.2047   // Convert Inches/Hour to lbs/square-foot
#define C_NOMINAL_LANEWIDTH 12.0          // nominal width of a lane in feet
#define C_MILES_TO_FEET 5280.0            // number of feet in a mile
#define C_LBS_TO_KGS 0.45                 // Number of kilograms in a pound
#define C_LBS_TO_GMS (C_LBS_TO_KGS * C_KILO)  // Number of grams in a pound
#define C_FT_TO_METERS 0.30                 // Number of meters in a foot

// Default parameters for liquid water runoff calculations
//
// Runoff percentages are adapted from the U.S. Soil Conservation Service.  Technical Release 55: 
// Urban Hydrology for Small Watersheds.  USDA (U.S.Department of Agriculture).  June 1986.  
// Available from NTIS (National Technical Information Service), NTIS #PB87101580.
// Document states that 98% of the water will runoff the pavement immediately. 
// Remaining solution/water drains more slowly (15% runoff)..
// 
//
#define P_RoadwayImmediateRunoff   0.98 // See note above
#define P_ChemSolutionRunoff       0.15 // See note above
#define P_DryChem_LossRate         0.25 // If chemicals have dried on the road, change the
//                                         rate at which they dissipate. (Dont know if this should
//                                         be higher or lower actually.)
//
#define P_EvaporationPerHour       0.15
//
// Following are placeholders and are not currently used in the algorithm
#define P_RoadwayMinDepthforRunoff 0.05 // Minimum amount of water on road for runoff to occur
#define P_RoadwayRunoffPerHour     0.90 // Not used
//
// Chemical loss as application is dropped from the truck and bounces or sprays off the road
//
// Currently this is a fixed parameter for the type of chemical being used, if multiple application
// types were allowed then loss rates would be assigned for each type and form of chemical and
// perhaps even be dependent on the type of spreader being used.
//
#define P_InitialChemAppLossRateDry      0.20 // This is an estimate for pre-wet chemical applications.
#define P_InitialChemAppLossRatePrewet   0.10 // This is an estimate for pre-wet chemical applications.
#define P_InitialChemAppLossRateLiquid   0.05 // This is an estimate for liquid chemical applications.
//
// Traffic dilution defaults, nominal percentage of chemical bounced off the road due to a
// low amount of traffic. Higer levels of traffic, lose higher percnetages of chemical as
// specified in the CalcTrafficFactor() routine.
//
#define P_NominalTrafficDilution   0.01 // Nominally 1% reduction from traffic effects
//
// Default treatment and plowing thresholds (inches of snow).
//
#define P_BARE_PAVEMENT_THRESH 0.005 // Min snow on road to trigger treatment estimates (mm of snow)
#define P_SNOW_PLOW_THRESH     25.4  // Min snow on road to trigger plowing (mm of snow)

//#define SNOW_DEPTH_AFTER_PLOW 6.0   // (mm) about 1/4"
#define SNOW_DEPTH_AFTER_PLOW 1.0   // (mm) matches METRo's removal depth

// 25.4 mm ~ 1 inch of snow

//
// Thresholds to trigger treatment investigation for freezing rain 
// First is for cases where the models indicate freezing rain precip types, the 
// second is for cases that the model inidcates rain (but the road temp is very cold).
// These were originally set at different values with the rain_frzrain case requiring a colder 
// temperature. However, Iowa DOT opted for more conservative numbers to protect against
// possible freezing rain and the negative impacts it would have on the road surface.
// Iowa dot 35 deg F
// Changed based on Iowa DOT demonstration to 33.5 degF
#define P_FRZRAIN_FRZRAIN_TROAD 0.833
#define P_RAIN_FRZRAIN_TROAD    0.833
// Standing water on the road will freeze at P_FRZPOINT_WATER pavement temperature.
#define P_FRZPOINT_WATER        0.833

// Temperature treatment ranges used to set the ChemicalType structure in the routine
// ChemConc_InitAndGetParams().
//
#define P_MAX_TEMP 2.0    // For anti-cing the upper range of temperatures to treat for (degC)
//                           This value is the same for all chemical types.
#define P_MIN_TEMP -10.0  // For anti-icing the lowest temperature where chemical will be
//                           applied (degC). This value is no longer used in the system, instead 
//                           the ChemicalType structure is dynamically modified based on the
//                           type of chemical being applied.

//
// Types of chemicals currently supported in the system. But only 1 chemical can be chosen at a 
// time. The parameters P_MAX_TEMP and P_MIN_TEMP need to be modified for each chemical.
//
#define C_NUM_CHEMTYPE        8

#define C_CHEMTYPE_UNKNOWN    0   // Error value - unset chemical type 
#define C_CHEMTYPE_NACL       1   // Sodium chloride
#define C_CHEMTYPE_CACL2      2   // Calcium chloride(CaCl2)
#define C_CHEMTYPE_MGCL2      3   // Magnesium chloride (MgCl2)
#define C_CHEMTYPE_CMA        4   // Calcium Magnesium Acetate
#define C_CHEMTYPE_KAC        5   // Potassium acetate
#define C_CHEMTYPE_CALIBER    6   // MgCl base with additives
#define C_CHEMTYPE_ICESLICER  7   // NaCl base with additives
#define C_CHEMTYPE_ICEBAN     8   // Organic compounds

#define C_CHEMFORM_DRY        0
#define C_CHEMFORM_PREWET     1
#define C_CHEMFORM_LIQUID     2

// The default chemical type
#define P_USER_CHEMTYPE       C_CHEMTYPE_NACL
#define P_USER_CHEMFORM       C_CHEMFORM_PREWET

//
// Define a sensitivity threshold to adjust solution point. Used in combination with the 
// Eutectic curve of the various chemicals -- basically defines how close the solution is 
// allowed to get to the ice/solution point of the eutectic curve. The higher the number
// the more chemical treatments or volume will be needed. Units: degrees Celcius.
// 
#define P_CHEM_SAFE_DELTA 1.5 


//
// Eutectic values of various chemical de-icers
//    NACL  -- Salt
//    CACL2 -- Calcium Chloride
//    MGCL2 -- Magnesium Chloride
//    CMA   -- 
//    KAC   -- Potassium 
//
// Eutectic temperature (degress Celsius)
#define C_NACL_EUTECTIC_TEMP      -21.6
#define C_CACL2_EUTECTIC_TEMP     -51.0
#define C_MGCL2_EUTECTIC_TEMP     -33.0
#define C_CMA_EUTECTIC_TEMP       -27.5
#define C_KAC_EUTECTIC_TEMP       -60.0
#define C_CALIBER_EUTECTIC_TEMP   -60.0
#define C_ICESLICER_EUTECTIC_TEMP -60.0
#define C_ICEBAN_EUTECTIC_TEMP    -60.0  // guess

// Eutectic concentration (%)
#define C_NACL_EUTECTIC_CONC  23.3  
#define C_CACL2_EUTECTIC_CONC 29.8
#define C_MGCL2_EUTECTIC_CONC 21.6
#define C_CMA_EUTECTIC_CONC   32.5
#define C_KAC_EUTECTIC_CONC   49.0
#define C_CALIBER_EUTECTIC_CONC   32.0
#define C_ICESLICER_EUTECTIC_CONC   21.6 // need to fix
#define C_ICEBAN_EUTECTIC_CONC   32.0 // guess

// Effective temperatures of various chemicals (based on 50% of eutectic concentration (deg C)
// The following are the 1/2 point temperatures for various chemicals.
#define C_NACL_HALFPOINT_TEMP  -10.0
#define C_CACL2_HALFPOINT_TEMP -12
#define C_MGCL2_HALFPOINT_TEMP -9.5
#define C_CMA_HALFPOINT_TEMP   -7.5
#define C_KAC_HALFPOINT_TEMP   -16
#define C_CALIBER_HALFPOINT_TEMP -30.0 // Not sure how to handle an unlimited halfpoint
#define C_ICESLICER_HALFPOINT_TEMP -15.0  // Simlar to MgCl but CDOT believe effective to lower temps
#define C_ICEBAN_HALFPOINT_TEMP -17.0  // guess

//
// Treatment explanation strings
//
const string treatmentExplanations[] =
  {
    "Normal anti-icing operations. ",                            // 0
    "Storm starts as freezing rain. Pre-treat. ",                // 1
    "Storm starts as snow, road Ts in-range. Pre-treat. ",       // 2
    "Plow only operations. ",                                    // 3
    "No need for chemicals. Plow only. ",                        // 4
    "Blowing snow and cold roads expected. Plow only. ",         // 5
    "Snow still on roadway. Plow only. ",                        // 6 (no longer used)
    "Roads too cold for chemicals. Plow only. ",                 // 7
    "Chem treatment below minimum, treat at minumum. ",          // 8
    "Treatment exceeds maximum. May need additional plowing. ",  // 9
    "No plowing needed yet. ",                                   // 10
    "Chemicals not needed with warm road temps. ",               // 11
    "Road temps too low for this chemical. ",                    // 12
    "Estimated chemical rate too low. ",                         // 13
    "Warm roads at onset. Don't pre-treat. ",                    // 14
    "Blowing snow and cold roads expected. Don't pre-treat. ",   // 15
    "Storm starts as rain. Don't pre-treat. ",                   // 16
    "User defined treatment."                                    // 17
  };

//
// Declare structure for Chemical Concentration characteristics and user parameters
//
struct chemtype {
  int   UserChemType;             // Type of de-icing chemical
  int   UserChemForm;             // Form of de-icing chemical (solid/liquid)
  char  ChemName[C_MAX_CHEMNAME]; // Name of chemical
	// Should this be a string??

  // Parameters related to chemical application 
  float MinApplicationRate;       // Minimum application rate (P_TREAT_UNITS)
  float MaxApplicationRate;       // Maximum application rate (P_TREAT_UNITS)
  float RoundTreatment;           // Round applilcation rate to nearest (P_TREAT_UNITS)
  int   Units;                    // What are the units gal/lane-mile, lbs/sq-meter, etc
  float MinTemp;       // Minimum temperature chemical is effective for de- or anti-icing (degC)
  float MaxTemp;       // Maximum temperature chemical is effective for de- or anti-icing (degC)

  // Parameters related to dilution
  float NominalTrafficDilution;     // Traffic dilution rate (frac %) at low traffic volumes
  float RoadChemAppInitialLossRate; // Chemical loss rate due to splatter/spray off truck (frac %) 
  float RoadWaterImmediateRunoff;   // Initial loss of precip off road in first hour (frac %)
  float RoadChemSolutionRunoffPerHour; // Subsequent loss of solution (frac % per hour)
  float ChemSafeTempDelta;          // Sensitivity adjustment (deg C) for eutectic concentration
};
typedef struct chemtype ChemicalType;
//
// Structure defining any treatment to be performed
//
struct treatment {
  int           StartTime;       // The expected start of the treatment route (in hours)
  int           EndTime;         // The expected end of the treatment route (in hours)
  int           EffectiveTime;   // The last time period covered by this treatment (should
                                 // be the same as EndTime for plowing and EndTime+RouteDelta for 
                                 // chemical treatments
  int           DoPlowing;       // Should plowing be done at the same time? (true/false)
  int           ApplyChemicals;  // Apply chemicals? (true/false)
  ChemicalType  Chemicals;       // The characteristics of the de-icing chemical to be applied.
  float         ApplicationRate; // The estimated rate at which the chemical should be applied.
  int           TreatType;       // What kind of treatment based on treatment flags
  string        Explanation;     // String explaining the treatment strategy
  int           ExplanationIndx; // Index of explanation string
};
typedef struct treatment Treatment;


//
// Declare structure for Rules of Practice user parameters
//
struct ropparams {
  // Parameters to determine if treatment is needed
  float Bare_Pavement_Thresh;  // The minimum amount of snow on the road (mm) that would
                               // trigger a check to see if chemical treatment is needed
  float Snow_Plow_Thresh;      // The minimum amount of snow on the road (mm) that would
                               // trigger snow plowing operations
  float RouteRoundtripTiming;  // The time (in minutes) that it takes to complete the route
  float TruckTurnAroundTime;   // The time (in minutes) that it takes to re-load a truck
	float PreRainDelta;          // The time delta (in minutes) to offset chemical treatment
	                             // when rain precedes the trigger event.
	float PretreatOffset;        // The time (in minutes) to offset pretreatment operations from trigger
  float ChemRateSensitivity;   // Changed from ratio to fixed % point offset
	int   TreatmentStrategy;     // C_CONTINUOUS or C_ONTRIGGER 
};
typedef struct ropparams ROPParamsStruct;

//
// Declare structure for Storm Type characterization
//
struct stormtype {
  int   BeginLookout;
  int   EndLookout;
  int   StartWetRoad;
  int   EndWetRoad;
  int   StartAnyPrecip;            // First hour of precipitation
  int   EndAnyPrecip;              // Last hour of precipitation
  float TotalLiquidPrecip;
  float TotalFrozenPrecip;
  int   StartFrozenPrecip;
  int   EndFrozenPrecip;
  int   RoadTrend_PrestormType;
  int   RoadTrend_PoststormType;
  int   RoadTrend_StormType;
  int   PrecipType_StormType;
  int   PrecipRate_StormType;
  float ColdBS_score;
  string StormTypeText;
};
typedef struct stormtype StormType;


//
// Declare structure for Storm Type characterization
//
struct stormtypekernel {
  float   score;                  // holds the overall score for the kernel
  int     num_periods;
  int     kerneltype;             // exact (0) or scaled (1). Exact kernels require that the
  //                                 value be exactly equal to the maxval to be scored a 1.0
  //                                 any other value scores a 0. Scaled kernels give a maximum score
  //                                 of 1 for values at or above the maxval, a score of 0.0 for
  //                                 values below minval, and alinearly scaled score for values
  //                                 in between minval and maxval.
  int     period[P_MAX_PERIODS];  // holds the expected precipitaion type for this kernel
  string  name;
};
typedef struct stormtypekernel StormTypeKernel;

//
//-----------------------------------------------------------------------------------------------
//
//
// Declare subroutines (chemical concentration)
//
int   AdjustChemicalConcentration(int f_start, int f_end, Treatment *treat, WxForecast *fcst,
                                  RoadConditions *roadfcst );

float CalcCriticalChemSolutionPoint( int ChemType, float Tpavement );
float CalcCriticalChemSaturationPoint( int ChemType, float Tpavement );
float GetChemEutecticTemp( int ChemType );
float GetChemEutecticConc( int ChemType );
float CalcTrafficFactor( int intensity );
float CalcBlowingSnowFactor( int index );
int   CalcChemicalRate(int FcstStartTime, int FcstEndTime, int fcst_end, WxForecast *fcst,
                       RoadConditions *roadfcst, float *chemrate);
int   ChemConc_InitAndGetParams( int ctype, int cform, SiteLayerTraffic *site_config,
				 int pretreat_flag, int app_rate_units, ChemicalType *ChemType );
float ConvertChemUnits( float rate, int in_chem, int in_units, int out_units );
int   GetChemUnits( char *unitsString );

//
// Declare subroutines (rules of practice)
//
int CharacterizeStorm ( RoadConditions *roadfcst, WxForecast *curr_wxfcst,
                        StormType *stormType, int num_rc_times ); 
int InitStormType( StormType *storm );
int PrintStormType( StormType *storm );

int InitNewTreatment ( Treatment *treat );
int DeterminePrecipType ( int current_hour, WxForecast *wxfcst, RoadConditions *roadfcst );
int StormRoadWaterPhase ( int nt_start, RoadConditions *roadfcst, WxForecast *wxfcst, int num_rc_times );
int DetermineRoadWaterPhase ( int current_hour, WxForecast *wxfcst, RoadConditions *roadfcst );
int StormSumDetermineTreatment( int f_start, int f_end, WxForecast *wxfcst,
                                RoadConditions *roadfcst,
				StormType *stormsum,
                                Treatment *TreatmentPlan, Treatment *PrevTreatment, 
				int num_rc_times );
int FindPlowStartTime( int start, RoadConditions *rfcst, int num_rc_times );
int FindDryRoadTime( int start, RoadConditions *rfcst, int *first, int num_rc_times );
int ROP_GetParams( SiteLayerTraffic *site, ROPParamsStruct *ROP_Params);
void PrintTreatmentPlan( Treatment **list, float convert, char *units );
void PrintTreatmentPlan( Treatment *list, int num, float convert, char *units );

int SelectFuzzyScore( int testval, int FuzzyIndex );
int FuzzyScoreLookup( int testval, int anchor1, int score1, int anchor2, int score2, int anchor3, int score3 );

int verify_wxfcst( WxForecast *wxfcst );
//
// External global variables
//
#ifndef P_ROADT_TOOCOLD
  extern int P_ROADT_TOOCOLD;
#endif

// User chemical type 
#ifndef P_user_chemtype
  extern int P_user_chemtype;
  extern int P_user_chemform;
  extern int P_user_chemtype_pretreat;
  extern int P_user_chemform_pretreat;
#endif

// Parameters controlling chemical characteristics for dilution
#ifndef ChemConc_Params
  extern ChemicalType ChemConc_Params;
  extern ChemicalType Pretreat_ChemConc_Params;
#endif

// Parameters defining rules of practice triggers and offsets
#ifndef ROP_Params
  extern ROPParamsStruct      ROP_Params;
#endif


#endif














