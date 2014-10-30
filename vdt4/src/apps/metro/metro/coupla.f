* METRo : Model of the Environment and Temperature of Roads
* METRo is Free and is proudly provided by the Government of Canada
* Copyright (C) Her Majesty The Queen in Right of Canada, Environment Canada, 2006

*  Questions or bugs report: metro@ec.gc.ca
*  METRo repository: https://gna.org/projects/metro/
*  Documentation: http://documentation.wikia.com/wiki/METRo
*
*
* Code contributed by:
*  Louis-Philippe Crevier - Canadian meteorological center
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
*


***
*     Subroutine COUPLA: Do the coupling between the atmospheric forecast
*                        and the observations of the RWIS station.
*     Sous-routine COUPLA: effectue le couplage des previsions  avec 
*                          les observations des stations meteo-routieres
*
*     Auteur /  Author: Louis-Philippe Crevier
*     Date: Aout 1999 / August 1999
***
      SUBROUTINE COUPLA ( FS, FI, P0, TA , QA , VA , TYP, FT, PR,
     *                    PVC , iref, NTP, NTP2, NTFM, CNT_IN, ITP, TSO,
     *                    FLAT,
     *                    FCOR, WW , WA, ALN, ALR, FP,
     *                    FSCORR   , FICORR  , ER1, ER2, 
     *                    FAIL, EPSILON, Z0, Z0T, ZU, ZT, ECHEC,
     *                    dpRA, dpSN, npRC, dpRT, dpIR, dpSF, dpFV,
     *                    dpFC, dpFA, dpG, dpBB, dpFP, TMTTYP)

      IMPLICIT NONE
      INTEGER i, j
      INTEGER Nl, n
      REAL DT
      INTEGER DTMAX
      REAL PADDING
      COMMON /BUFFER_SIZE/ DTMAX, Nl, DT, PADDING, n

***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Input
*     -------
*     FS: Incident solar flux (W)
*     FI: Incident infra-red flux (W)
*     P0: Surface pressure (Pa)
*     TA: Air temperature (C) at level ZT
*     QA: Specific humidity (g/kg) at level ZT
*     VA: Wind (m/s) at level ZU
*     TYP: Probable type of precipitation ( 1 -> rain, 2 -> snow )
*     FT: "Forecast time" (GMT)
*     PR: Precipitation rate (m/s)
*     PVC: Road condition (0 [dry] ou 1 [wet])
*     iref: Number of levels in the grid
*     NTP: Index for the start of coupling
*     NTP2: Index for the end of coupling
*     NTFM: Index for the end of forecast (Added by Cowie for FS check)
*     CNT: Grid constants
*     Z0: Roughness length (m)
*     Z0T: Roughness length (m)
*     ZU: Height of level of the wind forecast (m)
*     ZT:  Height of level of the atmospheric forecast (m)
*     EPSILON: Road emissivity
*     TS0: Target temperature for the end of coupling (C)
*     FCOR: Coriolis factor
*     WW: Minimum winds for the day and the night (m/s)
*     WA: Anthropogenic flux
*     ALN: Snow Albedo 
*     ALR: Road Albedo 
*     FP: Frozing point (C)
*     FLAT: Road or bridge
***
      LOGICAL FLAT
      INTEGER iref, NTP, NTP2, NTFM
      DOUBLE PRECISION  FS(DTMAX), FI(DTMAX), TA(DTMAX)
      DOUBLE PRECISION QA(DTMAX), PR(DTMAX)
      DOUBLE PRECISION VA(DTMAX), P0(DTMAX), FT(DTMAX)
      DOUBLE PRECISION TYP(DTMAX), PVC(DTMAX)
      DOUBLE PRECISION CNT(n,2), CNT_IN(2*n)
      DOUBLE PRECISION FCOR, WW(2), WA
      DOUBLE PRECISION ALN, ALR, TSO, FP
      DOUBLE PRECISION EPSILON, ZU, ZT, Z0, Z0T
      DOUBLE PRECISION dpSN(DTMAX), dpRA(DTMAX)
      DOUBLE PRECISION dpIR(DTMAX), dpSF(DTMAX)
      DOUBLE PRECISION dpFC(DTMAX), dpFA(DTMAX)
      DOUBLE PRECISION dpG(DTMAX), dpBB(DTMAX)
      DOUBLE PRECISION dpRT(DTMAX), dpFV(DTMAX)
      DOUBLE PRECISION dpFP(DTMAX)
      INTEGER TMTTYP
      INTEGER npRC(DTMAX)
      INTEGER nCheckBefore, nCheckAfter, nTimeStepsIn3Hours
***
*     I/O
*     ---------------
*     ITP: Road temperature profile  (C, iref levels != 0)
***
      LOGICAL ECHEC
      DOUBLE PRECISION ITP(n)
***
*     Output
*     -------
*     FAIL: Tells if the coupling have failed or not
*     TS: Time series for the surface temperature
*     FSCORR: Coupling Coefficient for the solar flux
*     FICORR: Coupling Coefficient for the infra-red flux
*     ER1: Water quantity of the road (mm)
*     ER2: Water Equivalent for ice/snow on the road (mm)
***
      LOGICAL FAIL
      DOUBLE PRECISION  FSCORR, FICORR, ER1, ER2
***
*     Internes
*     --------
***
      INTEGER ier, CHKDIV
      EXTERNAL CHKDIV
      CHARACTER*250 outfmt
      INTEGER next, now, iter
      LOGICAL DOWN
      REAL T(n,2), G(0:n)
      REAL RA, QG, RHO, TSK, AL, COUDUR, M
      REAL CL, PR1, PR2, DX, PRG, FZ
      REAL coeff1, coeff2, cotemp, COFS, COFI
***
*     Variables of FLXSURFZ
*     ---------------------
***
      REAL CMU, CTU, RIB,  ILMO, FTEMP, FVAP
      REAL H, UE, LZZ0, LZZ0T, FM, FH
***
*     Initialisation of common CLELOG
*     -------------------------------
***

      REAL  TCDK
      REAL CPD, CPV, RGASD, RGASV, TRPL, RAUW, EPS1
      REAL DELTA, CAPPA, TGL, CONSOL, GRAV, RAYT, STEFAN, PI
      REAL OMEGA, EPS2
      REAL KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD

      COMMON/CTSPHY/  CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW,
     $                EPS1, EPS2, DELTA, CAPPA, TGL, CONSOL,
     $                GRAV, RAYT, STEFAN, PI, OMEGA,
     $                KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD


      REAL TTT,PRS,QQQ,EEE,TVI,QST
      REAL FOEW,FODLE,FOQST,FODQS,FOEFQ,FOQFE,FOTVT,FOTTV,FOHR
      REAL FOEWA,FODLA,FOQSA,FODQA,FOHRA

      LOGICAL bSilent
      COMMON /SILENT/ bSilent

      INTRINSIC SIGN

*   DEFINITION DES FONCTIONS THERMODYNAMIQUES DE BASE
*   POUR LES CONSTANTES, UTILISER LE COMMON /CTESDYN/
*     NOTE: TOUTES LES FONCTIONS TRAVAILLENT AVEC LES UNITES S.I.
*           I.E. TTT EN DEG K, PRS EN PA, QQQ EN KG/KG
*          *** N. BRUNET - MAI 90 ***
*          * REVISION 01 - MAI 94 - N. BRUNET
*                          NOUVELLE VERSION POUR FAIBLES PRESSIONS
*
*     FONCTION DE TENSION DE VAPEUR SATURANTE (TETENS) - EW OU EI SELON TT
      FOEW(TTT) = 610.78*EXP( AMIN1(SIGN(17.269,TTT-TRPL),SIGN
     W (21.875,TTT-TRPL))*ABS(TTT-TRPL)/(TTT-35.86+AMAX1(0.,SIGN
     W (28.2,TRPL-TTT))))
*
*     FONCTION CALCULANT LA DERIVEE SELON T DE  LN EW (OU LN EI)
      FODLE(TTT)=(4097.93+AMAX1(0.,SIGN(1709.88,TRPL-TTT)))
     W /((TTT-35.86+AMAX1(0.,SIGN(28.2,TRPL-TTT)))*(TTT-35.86+AMAX1(0.
     W ,SIGN(28.2,TRPL-TTT))))
*
*     FONCTION CALCULANT L'HUMIDITE SPECIFIQUE SATURANTE (QSAT)
      FOQST(TTT,PRS)=EPS1/(AMAX1(1.,PRS/FOEW(TTT))-EPS2)
*
*     FONCTION CALCULANT LA DERIVEE DE QSAT SELON T
      FODQS(QST,TTT)=QST*(1.+DELTA*QST)*FODLE(TTT)
*     QST EST LA SORTIE DE FOQST
*
*     FONCTION CALCULANT TENSION VAP (EEE) FN DE HUM SP (QQQ) ET PRS
      FOEFQ(QQQ,PRS) = AMIN1(PRS,(QQQ*PRS) / (EPS1 + EPS2*QQQ))
*
*      FONCTION CALCULANT HUM SP (QQQ) DE TENS. VAP (EEE) ET PRES (PRS)
      FOQFE(EEE,PRS) = AMIN1(1.,EPS1*EEE / (PRS-EPS2*EEE))
*
*      FONCTION CALCULANT TEMP VIRT. (TVI) DE TEMP (TTT) ET HUM SP (QQQ)
      FOTVT(TTT,QQQ) = TTT * (1.0 + DELTA*QQQ)
*
*      FONCTION CALCULANT TTT DE TEMP VIRT. (TVI) ET HUM SP (QQQ)
      FOTTV(TVI,QQQ) = TVI / (1.0 + DELTA*QQQ)
*
*      FONCTION CALCULANT HUM REL DE HUM SP (QQQ), TEMP (TTT) ET PRES (PRS)
*      HR = E/ESAT
       FOHR(QQQ,TTT,PRS) = AMIN1(PRS,FOEFQ(QQQ,PRS)) / FOEW(TTT)
*
*     LES 5 FONCTIONS SUIVANTES SONT VALIDES DANS LE CONTEXTE OU ON
*     NE DESIRE PAS TENIR COMPTE DE LA PHASE GLACE DANS LES CALCULS
*     DE SATURATION.
*   FONCTION DE VAPEUR SATURANTE (TETENS)
      FOEWA(TTT)=610.78*EXP(17.269*(TTT-TRPL)/(TTT-35.86))
*   FONCTION CALCULANT LA DERIVEE SELON T DE LN EW
      FODLA(TTT)=17.269*(TRPL-35.86)/(TTT-35.86)**2
*   FONCTION CALCULANT L'HUMIDITE SPECIFIQUE SATURANTE
      FOQSA(TTT,PRS)=EPS1/(AMAX1(1.,PRS/FOEWA(TTT))-EPS2)
*   FONCTION CALCULANT LA DERIVEE DE QSAT SELON T
      FODQA(QST,TTT)=QST*(1.+DELTA*QST)*FODLA(TTT)
*   FONCTION CALCULANT L'HUMIDITE RELATIVE
      FOHRA(QQQ,TTT,PRS)=AMIN1(PRS,FOEFQ(QQQ,PRS))/FOEWA(TTT)
*
***
*
*     Procedure
*     =========
      if( .not. bSilent) then
         WRITE(*,*) "DEBUT COUPLA"
      end if
*****
**     Conversion of array in matrix
      CALL ARRAY2MATRIXDOUBLEPRECISION(CNT_IN, CNT , n, 2)
**     Initialisation of parametres
**     ++++++++++++++++++++++++++++
      FAIL = .FALSE.
      DOWN = .FALSE.
      FZ = 0.1
      iter = 0
      coeff1 = 0.
      coeff2 = 1.
      ILMO = 1.
*     Compute the length of coupling
      COUDUR = real(NTP2-NTP+1)*DT/3.6e3
*     Initialization of albedo variation due to snow presence
*     ---------------------------------------------------------------
*     |                                                             |
*     |  See balanc.F for the description of the function used      |
*     |                                                             |
*     ---------------------------------------------------------------
      M = (ALN - ALR) / 5.
 11   next = 1
      now = 2
      H = 300.0
      ER1 = 0.0
      ER2 = 0.0
      iter = iter + 1
      G(0) = 0.0
      do j=1,iref
         T(j,now)=REAL(ITP(j))
         T(j,next)=REAL(ITP(j))
         G(j) = 0.
      end do

************************************************
*    Criterium: Solar flux must be non zero
*     during the 3 hours BEFORE the roadcast and 
*     1 hour AFTER the begining of the roadcast.
      nTimeStepsIn3Hours = 3*3600./DT
      if (NTP2 - nTimeStepsIn3Hours > NTP) then
         nCheckBefore = NTP2-nTimeStepsIn3Hours 
      else
         nCheckBefore = NTP
      end if
      if (NTP2 + nTimeStepsIn3Hours/3 < DTMAX) then
         nCheckAfter = NTP2+nTimeStepsIn3Hours/3
      else
         nCheckAfter = DTMAX
      end if

*     avoid going off array. Also, added +1 to NPT2 index below
*     because it can be 0. (Cowie)
      nCheckBefore = max(1, nCheckBefore)
      nCheckAfter = min(DTMAX, nCheckAfter)

      if(FS(nCheckBefore) > 0.0 .and.
     *   FS(NTP2+1) > 100.0 .and. 
     *   FS(nCheckAfter) > 0.0) then
         COFI = 1.0
         COFS = coeff2
      else
*        Nuit
         COFI = coeff2
         COFS = 1.0
      end if

*     +++++++++++++++++++++++++++++++++
*     Entry of the coupling loop
*     +++++++++++++++++++++++++++++++++
      do i = NTP+1,NTP2
*        Adjustement of the road conditions
*        +++++++++++++++++++++++++++++++++++++
         ER1 = PVC(i)*ER1
         ER2 = PVC(i)*ER2
*        Preparation of differents terms of balance
*        ++++++++++++++++++++++++++++++++++++++++++
*        road temp in Kelvins         
         TSK = T(1,now) + TCDK
*        Black body radiation 
         RA = EPSILON*STEFAN*TSK**4
*        Humidity at the surface
         call SRFHUM  ( QG, CL, ER1, ER2, TSK, P0(i), QA(i), FP )
*        Air density at ground
         ier = CHKDIV ( RGASD * FOTVT ( TSK , QG ), "coupla.ftn", 209 )
         if ( ier .eq. 1 ) then
            ECHEC = .true.
            return
         end if
         RHO = P0(i) / ( RGASD * FOTVT ( TSK , QG ) )
*        Wind modification 
         call VENMIN  ( WW, FT(i), VA(i) )
*        Energy used/free by the melting snow/freezing rain
         call VERGLAS ( TYP(i), T(1,now), FP, FZ, PR(i),
     *                  PR1, PR2, PRG, FAIL )
*        Heat flux coefficients
         call FLXSURFZ( CMU  , CTU , RIB  , FTEMP, FVAP ,
     *                  ILMO, UE   , FCOR , TA(i)+TCDK,
     *                  QA(i), ZU, ZT  , VA(i), TSK  , QG   ,
     *                  H    , Z0  , Z0T  , LZZ0 , LZZ0T,
     *                  FM   , FH  ,  1,  1     )
*        Effective albedo 
         AL = 1.0 - max( ALR,min( ALN,M*ER2+ALR-M ) )
*        Energetic balance computation
*        +++++++++++++++++++++++++++
         G(0) = max(0.0,REAL(COFS*AL*FS(i))) - RA 
     *        + COFI*EPSILON*FI(i)
     *        + RHO*CTU*( CPD*(TA(i)-T(1,now))
     *        + CL*(QA(i)-QG)) + PRG + WA
*        Output information         
         dpIR(i) = COFI*EPSILON*FI(i)
         dpSF(i) = COFS*AL*FS(i)
         dpFV(i) = RHO*CTU*(CL*(QA(i)-QG))
         dpBB(i) = - RA
         dpFC(i) =  RHO*CTU*( CPD*(TA(i)-T(1,now)))
         dpFA(i) = WA
         dpFP(i) = PRG
         dpG(i) = G(0)

         G(1) = REAL(CNT(1,1) * ( T(2,now) - T(1,now) ))
         T(1,next) = REAL(T(1,now)+DT*(CNT(1,2)*( G(1) - G(0)) ))

         DX = 0.0
*        Compute the evolution of temperature in the ground
*        +++++++++++++++++++++++++++++++++++++++++++++++++
         call TSEVOL ( T, iref, now, CNT, G, FLAT, TA(i) )
*        Balance of accumulation at the ground
*        ++++++++++++++++++++++++++++++
         call RODCON ( ER1, ER2, RHO  , CTU, CL , FP , FZ , 
     *                 T(1,now), QA(i), QG , PR1, PR2, PRG, DX , 
     *                 npRC(i), TMTTYP)

         dpRT(i) = T(1,next)
         dpRA(i) = ER1
         dpSN(i) = ER2

*        Inversion of indices
*        +++++++++++++++++++++
         next = 3 - next
         now = 3 - now
      end do
*      if ( VERBOSE ) then
         FSCORR = COFS - 1.0
         FICORR = COFI - 1.0
         write(outfmt,223) iter, COUDUR,
     *        min(99.99,max(-99.99,T(1,now))),
     *        min(99.99,max(-99.99,REAL(TSO))),COFI,COFS
*      end if
*     Check if other iterations are necessary
*     +++++++++++++++++++++++++++++++++++++++++++
      if ( iter .eq. 25 ) then
         FICORR = 0.0
         FSCORR = 0.0
         FAIL = .true.
         WRITE(*,*) "More than 25 iterations in the coupling phase "
      else if ( abs(min(99.99,max(-99.99,T(1,now)))) .eq. 99.99 .or.
     *        FAIL ) then
         FICORR = 0.0
         FSCORR = 0.0
         write(outfmt,223) iter, COUDUR,
     *        min(99.99,max(-99.99,T(1,now))),
     *        min(99.99,max(-99.99,REAL(TSO))),COFI,COFS
         FAIL = .true.
         WRITE(*,*) " Temperature over 99 degrees, aborting in coupling"
      else if ( mod(iter,15) .eq. 0 .and.
     *        min(REAL(max(T(1,now),REAL(TSO-0.1))),REAL(TSO+0.1)) 
     *    .ne. T(1,now) ) then
         coeff1 = 0.0
         coeff2 = 1.5*coeff2
         go to 11
      else if ( T(1,now) - TSO .gt. 0.1 ) then
         coeff2 = coeff2 - 0.5*(coeff2-coeff1)
         DOWN = .true.
         go to 11
      else if ( TSO - T(1,now) .gt. 0.1 ) then
        if ( DOWN ) then
           cotemp = coeff1
           coeff1 = coeff2
           coeff2 = coeff2 + 0.5*(coeff2-cotemp)
         else
            cotemp = coeff1
            coeff1 = coeff2
            coeff2 = 2.0*coeff2 - cotemp
         end if
         go to 11
      else
         FSCORR = COFS - 1.0
         FICORR = COFI - 1.0
         FAIL = .false.
         do j=1,iref
            ITP(j) = T(j,now)
         end do
      end if
 223  format(1x,i3,3x,f5.2,3x,f6.2,3x,f6.2,f9.5,f9.5)

**     Conversion of matrix in array
      CALL MATRIX2ARRAYDOUBLEPRECISION(CNT, CNT_IN, n, 2)

      if( .not. bSilent) then
         WRITE(*,*) "FIN COUPLA"
      end if

      RETURN
      END

