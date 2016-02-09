// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: met_utils.cc
 *
 * Author: Jim Cowie
 *
 * Date:   1/15/15
 *
 * Description: Utilities for computing various meteorological values. 
 *              This code was copied out of the nextcast model_reg library
 *              in order to make it easier for the derive_vars application
 *              to stand on its own.
 *     
 */

// Include files 
#include <math.h>
#include "met_params.hh"

#define REG_CHECK_VAL  -999.0
#define REG_BAD_VAL   -9999.0

using namespace std;


// Constant, macro and type definitions 

// Global variables 

// Functions and objects


/*------------------------------------------------------------------

Function Name: 	calc_pr_vapr
Description:  Calculates vapor pressure, given temp (Celsius)
Returns:   Vapor pressure (mb)

--------------------------------------------------------------------*/

float calc_pr_vapr(float temp)
{
  float pr_vap;
  pr_vap = REG_BAD_VAL;
  
  if (temp > REG_CHECK_VAL) 
    pr_vap = 6.112 * exp( (17.67*temp) / (temp+243.5));

  return (pr_vap);
}


/*------------------------------------------------------------------

Function Name:  calc_rh
Description: Calculates rh, given temp and dewpoint (in Celsius)
Returns: Rh (range in [0,1])

--------------------------------------------------------------------*/

float calc_rh(float temp, float dewpt)
{
  float ratio;

  if ((temp<REG_CHECK_VAL) || (dewpt<REG_CHECK_VAL))
    return(REG_BAD_VAL);
  ratio = calc_pr_vapr(dewpt)/calc_pr_vapr(temp);;
  return(ratio);
}


/*-------------------------------------------------------------------

Function Name: 	calc_mixr
Description: Calculates mixing ratio
    Inputs: temp (C), pressure (mb)
Returns:   Mixing ratio

---------------------------------------------------------------------*/

float calc_mixr(float temp, float press)
{
  float corr;
  float e;
  float mixr;
  float pr_vapr;

  if ((press <= 0) || (temp <= REG_CHECK_VAL))
    return (REG_BAD_VAL);

  corr = 1.001 + ( (press-100)*0.0034/900 );
  
  pr_vapr = calc_pr_vapr(temp);

  e = pr_vapr*corr;
  mixr = 0.62197 * (e/(press-e)) * 1000;
  return (mixr);
}


/*---------------------------------------------------------------------

Function Name: 	calc_wet_bulb_temp
Description:  Calculates wet bulb temperature.
   Inputs: pressure (mb), temperature (C), dewpoint temp (C)
Returns:   Wet bulb temp (C)

-----------------------------------------------------------------------*/

float calc_wet_bulb_temp(float press, float temp, float dewpt)
{
  float vl = 2.5e6;
  float cp = 1004.0;
  int iters = 0;
  float dt = 9.9e9;

  float top;
  float bottom;
  float twet;
  float rmixr;
  float smixr;
  float twet_new;
  float wb_temp = REG_BAD_VAL;

  if ( (temp < REG_CHECK_VAL) || (dewpt < REG_CHECK_VAL) || (press < 0) )
    return (wb_temp);

  top = temp;
  bottom = dewpt;
  while ( (iters < 100) && (dt > 0.1) ) {
    iters++;
    twet = (top+bottom) / 2;
    rmixr =(calc_mixr(dewpt, press)/1000);
    smixr = (calc_mixr(twet, press)/1000);
    twet_new = temp - (vl/cp) * (smixr-rmixr);
    dt = fabs(twet-twet_new);
    if (twet_new < twet)
      top = twet;
    else
      bottom = twet;
  };
  if ( (iters < 100) || (dt < 1.0) )
    wb_temp = twet;

  return (wb_temp);
}


/*------------------------------------------------------------------

Function Name:  calc_dirspd
Description: Calculates wind direction, speed given u and v components
Returns: wind direction, speed in arg list

--------------------------------------------------------------------*/

void calc_dirspd(float U, float V, float *dir, float *spd)
{
  *spd = sqrt(U*U + V*V);
  if (*spd > 0)
     *dir = 270.-(180./M_PI)*atan2(V,U);
  else
     *dir = 0.;
  if (*dir > 360.) *dir -= 360.;
}



// Derive air density from moist air calculation
float calc_air_density(float T, float rh, float P_sfc)
{
  // Two constants
  float R_d = 287.058;
  float R_v = 461.495;
  
  // First estimate saturation vapor pressure of water
  float tempK = T + 273.15;
  float exponent = (7.5 * tempK - 2048.625) / (tempK - 35.85);
  float P_sat = 6.1078 * pow(10, exponent);
  
  // Then calculate vapor pressure of water and convert from mB to Pa
  float P_v = rh * P_sat * 100.;
  
  // Now Calculate Partial pressure of dry air
  float P_d = P_sfc * 100. - P_v;
  
  // Now calculate density
  float air_density = (P_d / R_d + P_v / R_v) / tempK;
  //printf("T = %.2f, rh =  %.2f, air_density = %.4f\n", T, rh, air_density);
  
  return air_density;
}


float calc_pop_rap1(float conv_precip, float tot_precip, float mean_rh, int algo_hr)
{
  float pop_rh1 = RP_POP1_RH1;
  float pop_rh2 = RP_POP1_RH2;
  float pop_nc1 = RP_POP1_NC1;
  float pop_nc2 = RP_POP1_NC2;
  float pop_c1 = RP_POP1_C1;
  float tp_thresh = RP_POP1_TP_THR;
  float rh_min = RP_POP1_RH_MIN;
  float rh_max = RP_POP1_RH_MAX;
  float qpf_nc1 = RP_POP1_QPF_NC1;
  float qpf_c1 = RP_POP1_QPF_C1;
  float c_thresh = RP_POP1_C_THR;
  float mm2in = RP_POP1_MM2IN;
  float pop_c2 = RP_POP1_C2;
  float qpf_c2 = RP_POP1_QPF_C2_12;
  float qpf_nc2 = RP_POP1_QPF_NC2_12;
  float thresh_a = RP_POP1_THR_A_12;

  float min1, ret1, ret_a, ret_b;
  float pop_rap1;

  float qpfnc = tot_precip-conv_precip;

  if (algo_hr == RP_06HR) {
    qpf_c2 = RP_POP1_QPF_C2_06;
    qpf_nc2 = RP_POP1_QPF_NC2_06;
    thresh_a = RP_POP1_THR_A_06;
  }
  else if (algo_hr == RP_03HR) {
    qpf_c2 = RP_POP1_QPF_C2_03;
    qpf_nc2 = RP_POP1_QPF_NC2_03;
    thresh_a = RP_POP1_THR_A_03;
  }
  else if (algo_hr == RP_01HR) {
    qpf_c2 = RP_POP1_QPF_C2_01;
    qpf_nc2 = RP_POP1_QPF_NC2_01;
    thresh_a = RP_POP1_THR_A_01;
  }


  if (tot_precip < tp_thresh) {
    ret1 = pop_rh1 +  (pop_rh2-pop_rh1)*(mean_rh-rh_min)/(rh_max-rh_min);
    min1 = min(ret1, 1.0);
    pop_rap1 = max(min1, 0.0);
  }
  else if (qpfnc < c_thresh) {
    ret1 = pop_c1 + (pop_c2-pop_c1)*(conv_precip-qpf_c1)/(qpf_c2-qpf_c1);
    min1 = min(ret1, 1.0);
    pop_rap1 = max(min1, 0.0);
  }
  else if (conv_precip < c_thresh) {
    ret1 = pop_nc1 + (pop_nc2-pop_nc1)*(qpfnc-qpf_nc1)/(qpf_nc2-qpf_nc1);
    min1 = min(ret1, 1.0);
    pop_rap1 = max(min1, 0.0);
  }
  else {
    ret1 = pop_c1 + (pop_c2-pop_c1)*(conv_precip-qpf_c1)/(qpf_c2-qpf_c1);
    min1 = min(ret1, 1.0);
    ret_a = max(min1, 0.0);

    ret1 = pop_nc1 + (pop_nc2-pop_nc1)*(qpfnc-qpf_nc1)/(qpf_nc2-qpf_nc1);
    min1 = min(ret1, 1.0);
    ret_b = max(min1, 0.0);
    pop_rap1 = 1.0 - (1. - ret_a) * (1. - ret_b);
  }
  return (pop_rap1);
}



float calc_pof_rap1_2(float rh_perc)
{
  float pof_rap1, pof_rap2;
  float a;
  float min1;
  float powrh;

  a = (rh_perc-RP_POF1_RH_MIN) / (RP_POF1_RH_MAX-RP_POF1_RH_MIN);
  min1 = min(a, 1.0);
  pof_rap1 = RP_POF1_MAX * max(min1, 0.0);
  pof_rap1 = int(pof_rap1*100)/100.;

  rh_perc/=100;
  powrh = pow(max(rh_perc, RP_POF2_RH_LB), RP_POF2_B);
  pof_rap2 = exp(RP_POF2_A*powrh);
  pof_rap2 = int(pof_rap2*100)/100.;

  return((pof_rap1+pof_rap2)/2.);
}
