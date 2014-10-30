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
 * Module: calc_mobility.cc
 *
 * Author: Bill Myers
 *
 * Date:   02/22/02
 *
 * Description: 
 *              This file contains a routine that computes the net mobility index.
 *              Note that the mobility function computed here is very primitive. It 
 *              needs to be refined. This has been left here as a place holder.
 *     
 *    calc_mobility()...Main routine described above.
 *
 */



/* System include files / Local include files */
#include <iostream>
#include <new>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <log/log.hh>
#include "RulesOfPractice.hh"

extern Log *proc_log;
inline float celsius2fahrenheit(float temp_cel) {return (1.8*temp_cel + 32.);};

/* External functions / Internal global functions / Internal static functions */

//  
//  Routine: calc_mobility()
//  
//  Description:
//      Calculates the net mobility index at each lead time. The crude algorithm implemented
//  can be summarized as follows:
//
//    Mobility     Road State
//    --------     ----------
//     1.0          Dry
//     0.7          Wet
//     0.65         Snow < 2"  (For Fomento display)
//     0.6          Snow 2"-4"
//     0.4          Snow 4"-6"
//     0.3          Snow > 6"
//     0.2          Ice     
//
//  Since determination of some of these states cannot be directly inferred from the data
//  available, crude techniques are applied to try to determine the road state.
//  
//  Inputs: 
//    current_hour.......The first timestep to process.
//    nt_start...........The index of the first weather time step.
//    num_fcst...........The final timestep to process.
//    wx_fcst_ts.........An array containing the pertinent weather forecast variables.
//    roadcond...........A time series of the road condition forecasts (esp.snow depth).
//    chem_failure_time..Number of hours until the chemicals will no longer melt forecast snow.
//  
//  Outputs:
//    roadcond...........A time series of the road condition forecasts. In particular, the
//                       mobility values at each lead time are set.
//  
//  Returns:
//    Hard-wired to return zero (for some reason).
//  


int calc_mobility(int current_hour, int nt_start, int num_fcst, WxFcstTS *wx_fcst_ts,
		  RoadConditions *roadcond, int chem_failure_time)
{
  int i;

  // Calculate net Mobility
  for (i=current_hour; i<num_fcst; i++)
    {
      // Determine road state

      // Is there snow on the road? The 0.65 mobility was added for the
      // Fomanto display in order to alert at a lower snow amount.
      if (roadcond[i].PsnowOnRoad > 0. && 
	  roadcond[i].PsnowOnRoad < 2. * MM_PER_INCH) 
	roadcond[i].NetMobility = 0.65;
      else if (roadcond[i].PsnowOnRoad >= 2. * MM_PER_INCH && 
	       roadcond[i].PsnowOnRoad < 4. * MM_PER_INCH) 
	roadcond[i].NetMobility = 0.6;
      else if (roadcond[i].PsnowOnRoad >= 4. * MM_PER_INCH && 
	       roadcond[i].PsnowOnRoad < 6. * MM_PER_INCH) 
	roadcond[i].NetMobility = 0.4;
      else if (roadcond[i].PsnowOnRoad >= 6. * MM_PER_INCH)
	roadcond[i].NetMobility = 0.3;


      // Is there ice on the road? (Check last couple hours for potential freezing rain
      // conditions)
      else if (wx_fcst_ts->fcst[nt_start+i].qpf01 > 0. && 
	       wx_fcst_ts->fcst[nt_start+i].Ptype == C_PTYPE_RAIN &&
	       celsius2fahrenheit(roadcond[i].road_T) < P_RAIN_FRZRAIN_TROAD &&
	       i > chem_failure_time)
	roadcond[i].NetMobility = 0.2;
      else if (wx_fcst_ts->fcst[nt_start+i].qpf01 > 0. && 
	       wx_fcst_ts->fcst[nt_start+i].Ptype == C_PTYPE_FRZRAIN &&
	       celsius2fahrenheit(roadcond[i].road_T) < P_FRZRAIN_FRZRAIN_TROAD &&
	       i > chem_failure_time)
	roadcond[i].NetMobility = 0.2;
      else if (i-1 >= 0 && wx_fcst_ts->fcst[nt_start+i-1].qpf01 > 0. && 
	       wx_fcst_ts->fcst[nt_start+i-1].Ptype == C_PTYPE_FRZRAIN &&
	       celsius2fahrenheit(roadcond[i-1].road_T) < P_FRZRAIN_FRZRAIN_TROAD &&
	       i > chem_failure_time)
	roadcond[i].NetMobility = 0.2;
      else if (i-1 >= 0 && wx_fcst_ts->fcst[nt_start+i-1].qpf01 > 0. && 
	       wx_fcst_ts->fcst[nt_start+i-1].Ptype == C_PTYPE_RAIN &&
	       celsius2fahrenheit(roadcond[i-1].road_T) < P_RAIN_FRZRAIN_TROAD &&
	       i > chem_failure_time)
	roadcond[i].NetMobility = 0.2;


      // Is the road is wet? (Check to see if there's been precip in the last couple hours)
      else if (wx_fcst_ts->fcst[nt_start+i].qpf01 > 0.)
	roadcond[i].NetMobility = 0.7;
      else if (i-1 >= 0 &&wx_fcst_ts->fcst[nt_start+i-1].qpf01 > 0.)
	roadcond[i].NetMobility = 0.7;

      // Dry Road Assumed
      else
	roadcond[i].NetMobility = 1.0; 

      /*
      printf("i %d  cft %d Snow %f   Ptype,qpf01  %d %f   %d %f    TP %f %f Mob %.2f\n", 
	     i, chem_failure_time, roadcond[i].PsnowOnRoad,
	     wx_fcst_ts->fcst[nt_start+i].Ptype,wx_fcst_ts->fcst[nt_start+i].qpf01, 
	      wx_fcst_ts->fcst[nt_start+i-1].Ptype, wx_fcst_ts->fcst[nt_start+i-1].qpf01,
	     roadcond[i].road_T, P_FRZRAIN_FRZRAIN_TROAD, roadcond[i].NetMobility );
      */
    }


  return(0);
}

