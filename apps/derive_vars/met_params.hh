
#ifndef REGRESSOR_PARAMS_HH
#define REGRESSOR_PARAMS_HH


inline float max(float val1, float val2) {return ( (val1 > val2) ? val1 : val2);};
inline float min(float val1, float val2) {return ( (val1 > val2) ? val2 : val1);};


enum {RP_12HR = 0, RP_06HR, RP_03HR, RP_01HR};
enum {SNOW=0, ICE_PELLET, FRZRN, RAIN, SN_RN, ICEP_RN};

/* lower bound for rh in the case that rh is 0 in the model */
const float RP_RELH_LB = 0.1;

/* conditional probability rap1 parameters, all in degrees Celsius */
const float RP_CP1_TW_MAX_THR1 = 0.0;
const float RP_CP1_TW_STN_THR1 = 3.0;
const float RP_CP1_TW_STN_THR2 = 0.0;
const float RP_CP1_CTT_THRESH = -15.0;
const float RP_CP1_A1 = -1.0;
const float RP_CP1_A2 = -15.0;
const float RP_CP1_A3 = -10.0;
const float RP_CP1_A4 = -15.0;
const float RP_CP1_A5 = -3.0;
const float RP_CP1_B1 = 3.0;  
const float RP_CP1_B2 = -10.0;
const float RP_CP1_B3 = -5.0; 
const float RP_CP1_B4 = -10.0;

/* cpos_rap2,3 and cpor_rap2,3, all in degrees F */

enum {RP_CALC_CPOR2 = 0, RP_CALC_CPOS2, RP_CALC_CPOR3, RP_CALC_CPOS3};

/* cpos_rap2 parameters */
const float RP_CPOS2_A1 = -17.805;
const float RP_CPOS2_B1 = 0.543;  

/* cpos_rap3 parameters */
const float RP_CPOS3_A1 = -16.331;
const float RP_CPOS3_B1 = 0.481;  

/* cpor_rap2 parameters */
const float RP_CPOR2_A1 = 28.335; 
const float RP_CPOR2_B1 = -0.850;

/* cpor_rap3 parameters */
const float RP_CPOR3_A1 = 24.632;
const float RP_CPOR3_B1 = -0.714;

enum {RP_CALC_CPOI2 = 0, RP_CALC_CPOI3};

/* cpoi_rap2 parameters */
const float RP_CPOI2_TW_THR = 30.0;
const float RP_CPOI2_PW_MAX = 0.18;
const float RP_CPOI2_A1 = -2.0;
const float RP_CPOI2_B1 = 0.1;
const float RP_CPOI2_A2 = 11.0;
const float RP_CPOI2_B2 = -0.333;

/* cpoi_rap3 parameters */
const float RP_CPOI3_T_THR = 31.0;
const float RP_CPOI3_P_MAX = 0.18;
const float RP_CPOI3_A1 = -1.818;
const float RP_CPOI3_B1 = 0.0909;
const float RP_CPOI3_A2 = 16.5;
const float RP_CPOI3_B2 = -0.5;

/* in-cloud parameters */
const float RP_IN_CLOUD_THR = 70.0;
const float RP_OUT_CLOUD_THR = 50.0;

/* tweg0 and teg0 parameters */
const float RP_TEG0_THR1 = 0.0;
const float RP_TEG0_THR2 = 100.0;

/* pot_rap1 parameters */
const float RP_LI_THR = 2.0;                // C
const float RP_CIN_THR = -100.0;            // J/kg
const float RP_POT_MIN = 0.1;
const float RP_POT_MAX = 0.5;
const float RP_QPF_MIN = 0.1;               // mm
const float RP_QPF_MAX12 = 13.0;            // mm
const float RP_QPF_MAX06 = RP_QPF_MAX12/2;  // mm
const float RP_QPF_MAX03 = RP_QPF_MAX06/2;  // mm

enum {RP_CALC_POF1 = 0, RP_CALC_POF2};

/* pof_rap1 parameters */
const float RP_POF1_MAX = 0.9;
const float RP_POF1_RH_MIN = 85.0;
const float RP_POF1_RH_MAX = 100.0;

/* pof_rap2 parameters */
const float RP_POF2_A = -0.3567;
const float RP_POF2_B = -14.3;
const float RP_POF2_RH_LB = RP_RELH_LB;

/* pop_rap1 parameters */

const float RP_POP1_RH1 = 0.1;
const float RP_POP1_RH2 = 0.5;  // 0.3;
const float RP_POP1_NC1 = 0.0;
const float RP_POP1_NC2 = 1.0;
//const float RP_POP1_C1 =  0.25; // 0.2;
const float RP_POP1_C1 =  0.10; 
const float RP_POP1_C2 =  0.7;  // 0.4;
const float RP_POP1_TP_THR = 0.1;
const float RP_POP1_RH_MIN = 0.80; // from 0.65
const float RP_POP1_RH_MAX = 1.0;
const float RP_POP1_QPF_NC1 = 0.1;
const float RP_POP1_QPF_NC2_12 = 7.5;
const float RP_POP1_QPF_NC2_06 = RP_POP1_QPF_NC2_12/2;
const float RP_POP1_QPF_NC2_03 = RP_POP1_QPF_NC2_06/2;
const float RP_POP1_QPF_NC2_01 = RP_POP1_QPF_NC2_03/3;
const float RP_POP1_QPF_C1 = 0.1;
const float RP_POP1_QPF_C2_12 = 12;  // 10;
const float RP_POP1_QPF_C2_06 = RP_POP1_QPF_C2_12/2;
const float RP_POP1_QPF_C2_03 = RP_POP1_QPF_C2_06/2;
const float RP_POP1_QPF_C2_01 = RP_POP1_QPF_C2_03/3;
const float RP_POP1_C_THR = 0.1;
const float RP_POP1_THR_A_12 = 7.5;
const float RP_POP1_THR_A_06 = RP_POP1_THR_A_12/2;
const float RP_POP1_THR_A_03 = RP_POP1_THR_A_06/2;
const float RP_POP1_THR_A_01 = RP_POP1_THR_A_03/3;
const float RP_POP1_MM2IN = 25.4;

/* t_stn parameters */
const float RP_T_STN_THR1 = 7.5;
const float RP_T_STN_THR2 = 3.5;
const float RP_T_STN_GAM1 = 9.8;
const float RP_T_STN_GAM2 = 6.5;
const float RP_T_STN_GAM3 = 0.0;

/* f_weasd parameters */
const float RP_W_MAX = 75.0;

/* vis_rap parameters */
const float RP_VIS_COEFF[5][4] = 
{
  // Cubic polynominal coefficients 
  {1.3057E2, -4.8182E0, 7.3930E-2, -3.8410E-4}, // All cases
  {1.4141E2, -5.4736E0, 8.5578E-2, -4.4539E-4}, // Non-prcp
  {1.6412E2, -5.5812E0, 7.1080E-2, -3.1337E-4}, // All-prcp
  {3.1119E1, -6.9131E-1, 8.5802E-3, -4.5351E-5}, // Prcp T<0
  {1.6625E2, -5.7022E0, 7.4852E-2, -3.4093E-4}  // Prcp T>0
};
const float RP_VIS_PMAX_12 = 10;
const float RP_VIS_PMAX_06 = RP_VIS_PMAX_12/2;
const float RP_VIS_PMAX_03 = RP_VIS_PMAX_06/2;
const float RP_VIS_VMAX = 75;

const float UNLIMITED_CEILING = 12000 * 0.3048; // feet * meter/foot


#endif /* REGRESSOR_PARAMS_HH */
