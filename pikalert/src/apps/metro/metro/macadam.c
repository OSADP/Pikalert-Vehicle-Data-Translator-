/*************************************************************************
* METRo : Model of the Environment and Temperature of Roads
* METRo is Free and is proudly provided by the Government of Canada
* Copyright (C) Her Majesty The Queen in Right of Canada, Environment Canada, 2006

*  Questions or bugs report: metro@ec.gc.ca 
*  METRo repository: https://gna.org/projects/metro/ 
*  Documentation: http://documentation.wikia.com/wiki/METRo 
* 
* 
* Code contributed by:
*  Miguel Tremblay - Canadian meteorological center 
*
*  $LastChangedDate$ 
*  $LastChangedRevision$ 
*
************************************************************************ 
*  This program is free software; you can redistribute it and/or modify 
*  it under the terms of the GNU General Public License as published by 
*  the Free Software Foundation; either version 2 of the License, or 
*  (at your option) any later version. 
*
*  This program is distributed in the hope that it will be useful, 
*  but WITHOUT ANY WARRANTY; without even the implied warranty of 
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
*  GNU General Public License for more details. 
*
*  You should have received a copy of the GNU General Public License 
*  along with this program; if not, write to the Free Software 
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
* 
* 
*****************************************************************************/ 

/***************************************************************************
**
** Nom:         macadam.c
**
** Auteur:      Miguel Tremblay
**
** Date:        April 16, 2004
**
** Description: File that handled the METRo model. 
**  All the fortran routines must be called in this file. The modele
**  sequence is here.
**
** 
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "macadam.h"
#define f77name(x) x##_


/* Constants for the string width for the compatibility with the fortran code. */

#define nNBROFSTRING 50
#define nNAMELENGTH 100
#define nNAMELENGTHLONG 150
#define nNBRARGS 27

 
/****************************************************************************
 Name: Do_Metro 
 
 Parameters:  
[I BOOL bFlat : road (FALSE) or bridge (TRUE)] 
[I double dMLat : Latitude of the RWIS station]
[I double dMLong : Longitude of the RWIS station]
[I double* dpZones : Depth in meter of each layer of the road]
[I long nNbrOfZone : Number of layers in the road]
[I long* npMateriau : code indicating the composition of the road:
   see http://documentation.wikia.com/wiki/Layer_type_%28METRo%29]
[I double* dpTA : interpolated air temperature]
[I double* dpQP : interpolated quantity of precipitation]
[I double* dpFF : interpolated wind velocity] 
[I double* dpPS : interpolated surface pressure]
[I double* dpFS : interpolated solar flux]
[I double* dpFI : interpolated visible flux]
[I double* TYP : Type of precipitation: 0 = nada, 1= liquid, 2=solid]
[I double* dpRC : interpolated road condition. 0 = dry, 1=wet] 
[I double dpTAO : interpolated observed air temperature] 
[I double* dpRTO : interpolated observed road temperature]
[I double* dpDTO : interpolated observed deep road temperature]
[I double* dpTimeO : steps of 30 seconds for the observation] 
[I long* npSWO1 : Boolean field to check if the deep road temperature  
    passed the QA/QC]
[I long* npSWO2 : Boolean field to check if the air temperature  
    passed the QA/QC]
[I long* npSWO3 : Boolean field to check if the dew point 
    passed the QA/QC]
[I long* npSWO4 : Boolean field to check if the wind speed
    passed the QA/QC]
[I BOOL* bpNoObs : boolean field to tell the number of observation used]
[I double dDeltaT : Time diffence, in hours, between the first observation 
  and the start of METRo.]
[I long nLenObservation : Number of valid observations.  30 seconds steps.]
[I long nNbrTimeSteps : number of 30 seconds steps in the forecast] 

[O double* outRT : output Road Temperature]
[O double* outSST : output Subsurface Temperature]
[O long*   outRC : output Road Condition]
[O double* outSN : output Snow/Ice Accumulation]
[O double* outRT : output Liquid Accumulation]

Returns: None 
 
Functions Called: 
Those are fortran functions called with f77name.  Only the function
balanc and grille should remains here at the release of macadam.
grille : Grid creation for the model. 
makitp : Creation of an analytic temperature for the temperature of the road.
initial : Initilization of the initial temperature profile 
coupla : Coupling
balanc : Forecast

<function description>
Description:
This is part of the module "metro_model.py".  This C function make the forecast
 for the METRo software

Notes:

Revision History:

Author		Date		Reason
Miguel Tremblay  May 2004     
 
***************************************************************************/

void Do_Metro( BOOL bFlat, double dMLat, double dMLon, double* dpZones, long nNbrOfZone,  long* npMateriau, double* dpTA, double* dpQP, double* dpFF,  double* dpPS, double* dpFS, double* dpFI, double* dpFT, double* dpTYP, double* dpRC, double* dpTAO,  double* dpRTO, double* dpDTO, double* dpAH, double* dpTimeO, long* npSwo, BOOL* bpNoObs, double dDeltaT, long nLenObservation, long nNbrTimeSteps, BOOL bSilent, double* outRT, double* outSST, long*   outRC, double* outSN, double* outRA, BOOL *bRet, long TMTTYP)
{

  /* Argument de la ligne de commande. Donne par python  */

 /**     A "O" at the end of a variable name 
  **     indicates it comes from the local observation
  **
  **     Ex: TA, TAO (observations)
  **
  **     TA : air temperature
  **     TD : dew point temperature
  **     VA : wind speed
  **     DD : wind direction
  **     FS : solar flux 
  **     FI : infra-red flux 
  **     AC : accumulations
  **     TYP: precipitation type
  **     P0 : surface pressure
  **     GMT: Time GMT
  **     DT : Subsurface temperature
  ******/
  BOOL bFail = FALSE;
  BOOL bSucces = TRUE;
  long nNtp;
  long nNtp2;
  long nNRec;
  long nNtdcl;
  double* dpItp;
  double dDiff;
  double* dpWw;
  double dWa = 10.0;
  double dAln = 0.5;
  double dAlr = 0.1;
  double dEpsilon = 0.92;
  double dZ0 = 0.001;
  double dZ0t = 0.0005;
  double dZt = 1.5;
  double dZu = 10;
  double dFCorr; 
  double dFsCorr=0;
  double dFiCorr=0;
  double dEr1=0;
  double dEr2=0;
  double dFp=0.0;
  /* Grid values */
/*   long nIRef=0;   */
  long nIR40;
  double* dpCnt;
  double* dpGri;
  long i;
  long nDeltaTIndice=0;
  //long TMTTYP = 3;


/* Those variables are declared here because we have to get them in python */
/* This cannot be perform unless you retrieve one pointer as a return value */
struct doubleStruct stRA; /* Liquid accumlation */
struct doubleStruct stSN; /* Snow/ice accumulation */
struct longStruct   stRC; /* Road condition */
struct doubleStruct stRT; /* Road temperature */
struct doubleStruct stIR; /* Infra-red flux */
struct doubleStruct stSF; /* Solar flux */
struct doubleStruct stFV; /* Vapor flux */
struct doubleStruct stFC; /* Sensible heat */
struct doubleStruct stFA; /* Anthropogenic flux */
struct doubleStruct stG;  /* Ground flux */
struct doubleStruct stBB; /* Black body radiation */ 
struct doubleStruct stFP; /* Phase change energy */
struct longStruct   stEc; /* Boolean to know if the execution was a success */
struct doubleStruct stSST; /* Subsurface temperature */
struct doubleStruct stDepth;  /* Depth of grid levels */
struct doubleStruct stLT; /* Level temperature */

  /* print the input arguments for debugging */
  if (!bSilent)
    print_input(bFlat, dMLat, dMLon, dpZones, nNbrOfZone, npMateriau, dpTA,
		dpQP, dpFF, dpPS, dpFS, dpFI, dpFT, dpTYP, dpRC, dpTAO,	dpRTO,
		dpDTO ,dpAH, dpTimeO, npSwo, bpNoObs, dDeltaT, nLenObservation,
		nNbrTimeSteps);

  /* Allocate memory for all structures */
  init_structure(nNbrTimeSteps,nNGRILLEMAX, &stRC, &stRA, &stRT, &stIR, &stSF, &stSN, &stFV, &stFC, &stFA, &stG, &stBB, &stFP, &stEc, &stSST, &stDepth, &stLT);
  
  if(!bSilent)
    printf("Back from init\n"); fflush(0);

  for (i=0; i<nNbrTimeSteps; i++){
    stIR.pdArray[i] = dpFI[i];
  }

  /* double */ 
  dpItp = (double*)malloc((nNGRILLEMAX)*sizeof(double));
  dpWw = (double*)calloc((2),sizeof(double));
  dpCnt = (double*)calloc((2*nNGRILLEMAX),sizeof(double));
  dpGri = (double*)malloc((2*nNGRILLEMAX)*sizeof(double));
  
  /* Initilization of physical constants in the fortran code */
  f77name(setconstphys)(&bSilent);

  /******************************* Station ********************************/

  *stEc.plArray = FALSE;
  dFCorr = 2.0*dOMEGA*sin(dPI*dMLat/180.0); 

  if(!bFlat){
    /* Note: In the case of a 'road', a 20 sand meters layer is added at the bottom.*/
    dpZones[nNbrOfZone] = 20.0;
    npMateriau[nNbrOfZone]= 4;
    nNbrOfZone = nNbrOfZone +1;
  }

  /* Grid creation */
  f77name(grille)(dpGri, dpCnt, &(stDepth.nSize), &nIR40, &bFlat, &nNbrOfZone,\
		  dpZones, npMateriau, &dDiff, stDepth.pdArray, stEc.plArray); 
  if(*(stEc.plArray)){
    bSucces = FALSE;
    goto liberation;
  }

  /* Extraction of observations */
  /*  Those -1 is because it is use in fortran */
  nDeltaTIndice = (dDeltaT)*3600/dDT-1;
  nLenObservation = nLenObservation -1;

   
  /***********************************************************************/
  /*           Couplage de la prevision et des observations.
  /*           Differentes possibilites selon la quantite d'observations
  /*           presentes.
  /**********************************************************************/
  bFail = FALSE;
  if(bpNoObs[2]){
    goto liberation;
  }
  else if(bpNoObs[3]){
    BOOL bFalse = FALSE;
    if(!bSilent)
      printf(" Only one valid observation: No initialization nor coupling.\n");
    f77name(makitp)(dpItp, &stDepth.nSize, &nIR40, &bFlat, &(dpTimeO[0]), &(dpRTO[0]), &(dpDTO[0]), &(dpTAO[0]), &dDiff, &dMLon, dpGri, npSwo);
    nNtp2 = nLenObservation - nDeltaTIndice;
   }
  else if(bpNoObs[1]){
    /* less than 3 hours of observation in the coupling */
    BOOL bFalse = FALSE;
    long nOne =1;
    if(!bSilent)
      printf(" Not enough data for coupling.\n");
    f77name(makitp)(dpItp, &stDepth.nSize, &nIR40, &bFlat, &(dpTimeO[0]),\
		    &(dpRTO[0]), &(dpDTO[0]), &(dpTAO[0]), &dDiff, \
		    &dMLon, dpGri, npSwo); 
    f77name(initial)(dpItp , (dpRTO+1), (dpDTO+1), (dpTAO+1), &nOne,	\
		     &nLenObservation, dpCnt, &stDepth.nSize, &nIR40, &bFlat, npSwo); 
    nNtp2 = nLenObservation - nDeltaTIndice;
  }
  else if(bpNoObs[0]){
    BOOL bTmp = FALSE;    
    if(!bSilent)
      printf(" Not enough data for initialization.\n");
    nNtdcl  = nLenObservation - ((nLenObservation < 28800.0/dDT)\
				 ? nLenObservation : 28800.0/dDT);
    /* Patch because nNtdcl does not take the value 0 in fortran!*/
    if(nNtdcl == 0) 
      nNtdcl =1;
    f77name(makitp)(dpItp, &stDepth.nSize, &nIR40, &bFlat, &(dpTimeO[nNtdcl]),\
		    &(dpRTO[nNtdcl]), &(dpDTO[nNtdcl]), &(dpTAO[nNtdcl]),\
		    &dDiff, &dMLon, dpGri, npSwo); 
    nNtp = - nDeltaTIndice + nNtdcl;
    nNtp2 = nLenObservation - nDeltaTIndice;
    f77name(coupla)(dpFS, dpFI, dpPS, dpTA, dpAH, dpFF, dpTYP, dpFT, dpQP, dpRC,\
		    &stDepth.nSize, &nNtp, &nNtp2, &nNbrTimeSteps,
		    dpCnt, dpItp,					\
		    &(dpRTO[nLenObservation]), &bFlat, &dFCorr, dpWw, &dWa, \
		    &dAln, &dAlr, &dFp, &dFsCorr, &dFiCorr, &dEr1, &dEr2, \
		    &bFail, &dEpsilon, &dZ0, &dZ0t, &dZu, &dZt, stEc.plArray, \
		    stRA.pdArray, stSN.pdArray, stRC.plArray, stRT.pdArray,\
		    stIR.pdArray, stSF.pdArray, stFV.pdArray, stFC.pdArray, \
		    stFA.pdArray, stG.pdArray, stBB.pdArray, stFP.pdArray,
		    &TMTTYP);  
    if(!bSilent)
      printf("coupla 1 \n");
    if(*(stEc.plArray)){
      bSucces = FALSE;       
      goto liberation;
    }
    if(bFail){
      long nOne = 1;
      if(!bSilent)
	printf("fail\n");
      f77name(initial)(dpItp, (dpRTO+1), (dpDTO+1), (dpTAO+1), &nOne,\
		       &nLenObservation, dpCnt, &stDepth.nSize, &nIR40, &bFlat, npSwo); 
     }
  }
  else{/* observation complete */
    long nOne =1;
    if(!bSilent)
      printf("Complete observations\n");

    f77name(makitp)(dpItp, &stDepth.nSize, &nIR40, &bFlat, &(dpTimeO[nDeltaTIndice]),\
		    &(dpRTO[nDeltaTIndice]), &(dpDTO[nDeltaTIndice]), \
		    &(dpTAO[nDeltaTIndice]), &dDiff, &dMLon, dpGri, npSwo); 
    nNtdcl  = nLenObservation - nDeltaTIndice -\
      ((nLenObservation-nDeltaTIndice < 28800.0/dDT)	\
       ? nLenObservation-nDeltaTIndice : 28800.0/dDT);
    f77name(initial)(dpItp , (dpRTO+1), (dpDTO+1), (dpTAO+1), &nOne,	\
		     &nLenObservation, dpCnt, &stDepth.nSize, &nIR40, &bFlat, npSwo); 
    nNtp = 0 + nNtdcl;
    nNtp2 = nLenObservation - nDeltaTIndice;
    f77name(coupla)(dpFS, dpFI, dpPS, dpTA, dpAH, dpFF, dpTYP, dpFT, dpQP, \
		    dpRC, &stDepth.nSize, &nNtp, &nNtp2, &nNbrTimeSteps,
		    dpCnt, dpItp,					\
		    &(dpRTO[nLenObservation]), &bFlat, &dFCorr, dpWw, &dWa,\
		    &dAln, &dAlr, &dFp, &dFsCorr, &dFiCorr, &dEr1, &dEr2,\
		    &bFail, &dEpsilon, &dZ0, &dZ0t, &dZu, &dZt, stEc.plArray,\
		    stRA.pdArray, stSN.pdArray, stRC.plArray, stRT.pdArray,\
		    stIR.pdArray, stSF.pdArray, stFV.pdArray, stFC.pdArray,\
		    stFA.pdArray, stG.pdArray, stBB.pdArray, stFP.pdArray,
		    &TMTTYP);
    if(!bSilent)
      printf("coupla 2\n");
    if(*(stEc.plArray)){
       bSucces = FALSE;       
       goto liberation;
     }
     if(bFail){
       long nOne = 1;
       if(!bSilent)
	 printf("fail\n");
       f77name(initial)(dpItp, (dpRTO+1), (dpDTO+1), (dpTAO+1), &nOne, &nLenObservation, dpCnt, &stDepth.nSize, &nIR40, &bFlat, npSwo);
     }
  }/* End else observation complete */

  /************ roadcast **************************************************/
  f77name(balanc)(dpFS, dpFI, dpPS, dpTA, dpAH, dpFF, dpTYP, dpFT, dpQP,\
		  &stDepth.nSize,					\
		  &nIR40, &nNtp2, &nNbrTimeSteps, dpCnt, dpItp, &bFlat, &dFCorr,\
		  dpWw,&dWa, &dAln, &dAlr, &dFp, &dFsCorr, &dFiCorr, &dEr1,\
		  &dEr2, &dEpsilon, &dZ0, &dZ0t, &dZu, &dZt, stEc.plArray,\
		  stRT.pdArray, stRA.pdArray ,stSN.pdArray, stRC.plArray,\
		  stIR.pdArray, stSF.pdArray, stFV.pdArray, stFC.pdArray,\
		  stFA.pdArray, stG.pdArray, stBB.pdArray, stFP.pdArray,\
		  stSST.pdArray, stLT.pdArray, &TMTTYP); 

  if(!bSilent)
    printf("Back from balanc\n");

  *bRet = *(stEc.plArray);
  if(*(stEc.plArray)){
    if(!bSilent)
      printf("Failed in balanc\n");
    bSucces = FALSE;       
    goto liberation;
  }

  // Copy output from doubleStructs to output arrays
  memcpy(outRT, stRT.pdArray, nNbrTimeSteps * sizeof(double));
  memcpy(outSST, stSST.pdArray, nNbrTimeSteps * sizeof(double));
  memcpy(outSN, stSN.pdArray, nNbrTimeSteps * sizeof(double));
  memcpy(outRA, stRA.pdArray, nNbrTimeSteps * sizeof(double));
  memcpy(outRC, stRC.plArray, nNbrTimeSteps * sizeof(long));

  /* Preparation of output file */
  if(!bSilent)
    printf("Free memory\n");

 liberation:
/* Free everybody */
/* String */
  /* double */
  free(dpItp);
  dpItp = NULL;
  free(dpWw);
  dpWw = NULL;
  free(dpCnt);
  dpCnt = NULL;
  free(dpGri);
  dpGri = NULL; 

  free(stRC.plArray);
  free(stRA.pdArray);
  free(stIR.pdArray);
  free(stSF.pdArray);
  free(stRT.pdArray);
  free(stSN.pdArray);
  free(stFV.pdArray);
  free(stFC.pdArray);
  free(stFA.pdArray);
  free(stG.pdArray);
  free(stBB.pdArray);
  free(stFP.pdArray);
  free(stEc.plArray);
  free(stSST.pdArray);
  free(stDepth.pdArray);
  free(stLT.pdArray);

}/* End Do_Metro */


/*
 * this conflicts with the roadcond_mdss main() and does nothing anyway (Cowie)
 *

int main(argc, argv) 
     int argc; 
     char *argv[];      
{   
 return 0; 
} 

*/

void init_structure(long nTimeStepMax, long nGrilleLevelMax, struct longStruct *stRC, struct doubleStruct *stRA, struct doubleStruct *stRT, struct doubleStruct *stIR, 
		    struct doubleStruct *stSF, struct doubleStruct *stSN, struct doubleStruct *stFV, struct doubleStruct *stFC, struct doubleStruct *stFA, 
		    struct doubleStruct *stG, struct doubleStruct *stBB, struct doubleStruct *stFP, struct longStruct *stEc, struct doubleStruct *stSST, 
		    struct doubleStruct *stDepth, struct doubleStruct *stLT)
{
  /* Memory allocation */

  /* Size */

  stRC->nSize = nTimeStepMax;
  stRA->nSize = nTimeStepMax;
  stRT->nSize = nTimeStepMax;
  stIR->nSize = nTimeStepMax;
  stSF->nSize = nTimeStepMax;
  stSN->nSize = nTimeStepMax;
  stFV->nSize = nTimeStepMax;
  stFC->nSize = nTimeStepMax;
  stFA->nSize = nTimeStepMax;
  stG->nSize = nTimeStepMax;
  stBB->nSize = nTimeStepMax;
  stFP->nSize = nTimeStepMax;
  stEc->nSize = 1;
  stSST->nSize = nTimeStepMax;
  stDepth->nSize = 0; /* Will be computed later */
  stLT->nSize = nTimeStepMax*nGrilleLevelMax;
  /* Memory alloc */
  stRC->plArray = (long*)calloc((nTimeStepMax),sizeof(long));
  stRA->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stIR->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stSF->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stRT->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stSN->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stFV->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stFC->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stFA->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stG->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stBB->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stFP->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stEc->plArray = (long*)calloc((1),sizeof(long));
  stSST->pdArray = (double*)calloc((nTimeStepMax),sizeof(double));
  stDepth->pdArray =  (double*)calloc((nTimeStepMax),sizeof(double));
  stLT->pdArray = (double*)calloc((nTimeStepMax*nGrilleLevelMax),sizeof(double));
}

#ifdef NOTNOW
void free_structure()
{
  free(stRC.plArray);
  free(stRA.pdArray);
  free(stIR.pdArray);
  free(stSF.pdArray);
  free(stRT.pdArray);
  free(stSN.pdArray);
  free(stFV.pdArray);
  free(stFC.pdArray);
  free(stFA.pdArray);
  free(stG.pdArray);
  free(stBB.pdArray);
  free(stFP.pdArray);
  free(stEc.plArray);
  free(stSST.pdArray);
  free(stDepth.pdArray);
  free(stLT.pdArray);
}

struct doubleStruct get_ra(void){

  return stRA;

}

struct doubleStruct get_sn(void){

  return stSN;

}

struct longStruct get_rc(void){

  return stRC;
}

struct doubleStruct get_rt(void){

  return stRT;
}

struct doubleStruct get_ir(void){

  return stIR;
}

struct doubleStruct get_sf(void){

  return stSF;
}

struct doubleStruct get_fv(void){

  return stFV;
}

struct doubleStruct get_fc(void){

  return stFC;
}

struct doubleStruct get_fa(void){

  return stFA;
}

struct doubleStruct get_g(void){

  return stG;
}

struct doubleStruct get_bb(void){

  return stBB;
}

struct doubleStruct get_fp(void){

  return stFP;
}

struct longStruct get_echec(void){

  return stEc;
}

struct doubleStruct get_sst(void){

  return stSST;
}

struct doubleStruct get_depth(void){

  return stDepth;
}

long get_nbr_levels(void){

  return stDepth.nSize;
}

struct doubleStruct get_lt(void){

  return stLT;
}

#endif

void print_input(BOOL bFlat, double dMLat, double dMLon, double* dpZones, long nNbrOfZone,  long* npMateriau, double* dpTA, double* dpQP, double* dpFF,  double* dpPS, double* dpFS, double* dpFI, double* dpFT, double* dpTYP, double* dpRC, double* dpTAO,  double* dpRTO, double* dpDTO, double* dpAH, double* dpTimeO, long* npSwo, BOOL* bpNoObs, double dDeltaT, long nLenObservation, long nNbrTimeSteps)
{
  /* print out args for testing */

  int ii;
  printf("Do_Metro arguments:\n");
  printf("bFlat (bridge?) %d, dMLat %f, dMLon %f\n", bFlat, dMLat, dMLon);
  printf("nNbrOfZone (layers) %d\n", nNbrOfZone);

  for (ii=0; ii<nNbrOfZone; ii++)
    printf("  ii %d: dpZones %f, npMateriau %d\n", ii, dpZones[ii], npMateriau[ii]);

  printf("nNbrTimeSteps (forecasts) %d\n", nNbrTimeSteps);
  printf("nLenObservation (obs) %d\n", nLenObservation);
  printf("bpNoObs[0] (no data) %d\n", bpNoObs[0]);
  printf("bpNoObs[1] (< 3 hrs obs) %d\n", bpNoObs[1]);
  printf("bpNoObs[2] (no valid obs) %d\n", bpNoObs[2]);
  printf("bpNoObs[3] (one valid obs) %d\n", bpNoObs[3]);

  printf("dDeltaT (hrs from obs to fcst) %.0f\n", dDeltaT);

  printf("Observations:\n");
  for (ii=0; ii<nLenObservation; ii++)
    printf(" ii %d, dpTimeO %.2f, dpTAO %.2f, dpRTO %.2f, dpDTO %.2f, npSwo[0-3] SST %d, AT %d, TD %d, WS %d\n", ii, dpTimeO[ii], dpTAO[ii], dpRTO[ii], dpDTO[ii], npSwo[4*ii], npSwo[4*ii+1], npSwo[4*ii+2], npSwo[4*ii+3]);

  printf("Forecasts:\n");
  for (ii=0; ii<nNbrTimeSteps; ii++)
    printf(" ii %d, dpFT %.2f, dpTA %.2f, dpQP %f, dpFF %.1f, dpPS %.0f, dpAH %f, dpFS %0.f dpFI %.0f, dpTYP %.0f, dpRC %.0f\n", ii, dpFT[ii], dpTA[ii], dpQP[ii]*10e4, dpFF[ii], dpPS[ii], dpAH[ii], dpFS[ii],  dpFI[ii], dpTYP[ii], dpRC[ii]);

}
