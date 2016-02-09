*
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
*************************************************************************


***
*     Sous-routine VERGLAS: Sous-routine calculant le gel-degel de la
*                           precipitation au contact de la route
*     Auteur / Author: Louis-Philippe Crevier
*     Date: Decembre 1999 / December 1999
***
      SUBROUTINE VERGLAS ( TYP, TS, FP, FZ, PR, PR1, PR2, PRG, FAIL )
      IMPLICIT NONE
***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Entrees
*     -------
      REAL TS, FZ
      DOUBLE PRECISION TYP, FP, PR
***
*     Sorties
*     -------
***
      REAL PR1, PR2, PRG
***
*     Entrees/Sorties
*     ---------------
***
      LOGICAL FAIL
***
*     Internes
*     --------
***
      REAL RATIO
***
*     Declarations des constantes physique 
*     et des fonctions thermodynamiques
*     ------------------------------------
***
*
      REAL CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW, EPS1
      REAL DELTA, CAPPA, TGL, CONSOL, GRAV, RAYT, STEFAN, PI
      REAL OMEGA, EPS2
      REAL KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
      COMMON/CTSPHY/  CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW,
     $                EPS1, EPS2, DELTA, CAPPA, TGL, CONSOL,
     $                GRAV, RAYT, STEFAN, PI, OMEGA,
     $                KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
***
*
      FAIL = .false.

*     Procedure
*     =========
      RATIO = 0.0
      if ( TYP.eq.2. .and. TS.le.FP ) then
*     neige sur route a T < FP
         PR1 = 0.0
         PR2 = 1e3 * PR
         PRG = 0.0
      else if ( TYP.eq.1. .and. TS.gt.FP ) then
*     pluie sur route a T > FP
         PR1 = 1e3 * PR
         PR2 = 0.0
         PRG = 0.0
      else if ( TYP.eq.2..and. TS.gt.FP+FZ ) then
*     neige sur route a T > FZ; neige fond au contact de la route
         PR1 = 1e3 * PR
         PR2 = 0.0
         PRG = - (PR * CHLF * RAUW)
      else if ( TYP.eq.1. .and. TS.le.FP-FZ ) then
*     pluie sur route a T < FZ; pluie gele au contact de la route
         PR1 = 0.0
         PR2 = 1e3 * PR
         PRG = PR * CHLF * RAUW
      else if ( TYP.eq.2. .and.
     *        max(min(TS,REAL(FP+FZ)),REAL(FP)).eq.TS ) then
*     neige sur route a FZ > T > 0; une partie de la neige fond au 
*     contact de la route
         RATIO = 0.25*sin(PI*(TS-FP)/FZ)+0.75
         PR1 = RATIO * 1e3 * PR
         PR2 = (1.0-RATIO) * 1e3 * PR
         PRG = - (RATIO * PR * CHLF * RAUW)
      else if ( TYP.eq.1. .and.
     *        min(max(TS,REAL(FP-FZ)),REAL(FP)).eq.TS ) then
*     pluie sur route a -FZ < T < 0; une partie de la pluie gele 
*     au contact de la route
         RATIO = 0.25*sin(PI*(TS-FP)/FZ)+0.75
         PR1 = (1.0-RATIO) * 1e3 * PR
         PR2 = RATIO * 1e3 * PR
         PRG = RATIO * PR * CHLF * RAUW
      else
         FAIL = .true.
      end if
      return
      end


************************************************************************
***
*     Subroutine   VENMIN: Impose the minimum winds specified by the WW
*                           variable. WW(1) is minimum wind for the day
*                           WW(2) is minimum wind for the night.
*                           FT is Forecast Time 
*                           VA is wind speed (Vitesse Air)
*                           Default is currently 0.
*
*     Auteur / Author: Louis-Philippe Crevier
*     Date: Decembre 1999 / December 1999
***
      SUBROUTINE VENMIN ( WW, FT, VA )
      IMPLICIT NONE
***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Input
*     -------
*     WW : Minimum winds for the day and the night (m/s)
*     FT : Forecast time
*     VA : Wind speed (m/s)
***
      DOUBLE PRECISION WW(2), FT
***
*     Input/Output
*     ---------------
***
      DOUBLE PRECISION VA
***
*
*     Procedure
*     =========
*     Specification of a minimum wind based on the time of the day
*     WARNING: UTC time. Based on East timezone
*     -----------------------------------------------------
      if ( FT .gt. 12.5 .and. FT .lt. 25.5 .or.
     *     FT .gt. 36.5 .and. FT .lt. 50.0 ) then
         VA = max( WW(1), VA )
      else if ( FT .ge. 11.5 .and. FT .le. 12.5 ) then
         VA = max( WW(2)+(FT-11.5)*(WW(1)-WW(2)), VA )
      else if ( FT .ge. 25.5 .and. FT .le. 26.5 ) then
         VA = max( WW(1)+(FT-25.5)*(WW(2)-WW(1)), VA )
      else if ( FT .ge. 35.5 .and. FT .le. 36.5 ) then
         VA = max( WW(2)+(FT-35.5)*(WW(1)-WW(2)), VA )
      else
         VA = max( WW(2), VA )
      end if

      return
      end


************************************************************************
***
*     Sous-routine TSEVOL: Calcul l'evolution du profil de temperature
*                          a partir du bilan de surface
*
*     Auteur / Author: Louis-Philippe Crevier
*     Date: Decembre 1999 / December 1999
***
      SUBROUTINE TSEVOL ( T, iref, now, CNT, G, FLAT, TA )
      IMPLICIT NONE

      INTEGER Nl, n
      REAL DT
      INTEGER DTMAX
      REAL PADDING
      COMMON /BUFFER_SIZE/ DTMAX, Nl, DT, PADDING, n
***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Entrees
*     -------
***
      INTEGER now, iref
      REAL G(0:n)
      DOUBLE PRECISION CNT(n,2), TA
      LOGICAL FLAT
***
*     Entrees/Sorties
*     ---------------
***
      REAL T(n,2)
***
*     Internes
*     --------
***
      INTEGER j, next

***
*
*     Procedure
*     =========
      next = 3 - now
      do j=2,iref-1
         G(j) = CNT(j,1) * ( T(j+1,now) - T(j,now) )
      end do
      do j=2,iref-1
         T(j,next) = T(j,now)+DT*(CNT(j,2)*( G(j) - G(j-1 )))
      end do
      if ( FLAT ) then
*     BC: underside temp. is air temp
         T(iref,next) = TA
      else
*     BC: no flux ( G(iref) = 0.0 )
         T(iref,next) = T(j,now) - DT*CNT(j,2)*G(iref-1)
      end if
      return
      end


************************************************************************
***
*     Sub-routine RODCON: Compute the quantity of the reservoir 
*                          ER1 (water) and ER2 (ice/snow)
*
*     Auteur / Author: Louis-Philippe Crevier
*     Date: Decembre 1999 / December 1999
***
      SUBROUTINE RODCON ( ER1, ER2, RHO, CTU, CL , FP, FZ,
     *                    TS , QA , QG , PR1, PR2, PRG, DX, ETAT,
     *                    TMTTYP)
      IMPLICIT NONE

      INTEGER Nl, n
      REAL DT
      INTEGER DTMAX
      REAL PADDING
      COMMON /BUFFER_SIZE/ DTMAX, Nl, DT, PADDING, n
*     MEA -> Water quantity before runoff. MEA(1) : liquid 
*      MEA(2): solid
      REAL MEA(2) 
      DATA MEA /0.2, 1.0/ 

      REAL REMVSN

***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Input
*     -------
***
*     QA Specific humidity (g/kg) at ZT level
*     QG Specific humidity (g/kg) at the road surface


      DOUBLE PRECISION FP, QA
      INTEGER ETAT
      INTEGER TMTTYP
      REAL RHO, CTU, CL, FZ
      REAL TS, QG, PR1, PR2, PRG, DX

***
*     Input/Output
*     ---------------
***
      DOUBLE PRECISION ER1, ER2
***
*     Internals
*     --------
***
      REAL FV1, FV2, CUTOFF
***
*     Physical constants and thermondynamical functions declarations
*     ------------------------------------
***
*
      REAL CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW, EPS1
      REAL DELTA, CAPPA, TGL, CONSOL, GRAV, RAYT, STEFAN, PI
      REAL OMEGA, EPS2
      REAL KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
      COMMON/CTSPHY/  CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW,
     $                EPS1, EPS2, DELTA, CAPPA, TGL, CONSOL,
     $                GRAV, RAYT, STEFAN, PI, OMEGA,
     $                KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
***
*
*     Procedure
*     =========
      QA = REAL(QA)
      if (TS.gt.FP+FZ ) then
         if ( ER2.gt.0.0 .and. (QA - QG).lt.0.0 ) then
            FV1 = 0.0
            FV2 = RHO*CTU*CL*( QA - QG )/CHLF
         else
            FV1 = RHO*CTU*CL*( QA - QG )/CHLF
            FV2 = 0.0
         end if
      else if (TS.lt.FP-FZ) then
         if ( ER1.gt.0.0 .and. (QA-QG).lt.0.0 ) then
            FV1 = RHO*CTU*CL*( QA - QG )/CHLF
            FV2 = 0.0
         else
            FV1 = 0.0
            FV2 = RHO*CTU*CL*( QA - QG )/CHLF
         end if
      else
         if ( ER1.gt.0.0 ) then
            FV1 = RHO*CTU*CL*( QA - QG )/CHLF
            FV2 = 0.0
         else
            FV1 = 0.0
            FV2 = RHO*CTU*CL*( QA - QG )/CHLF
         end if
      endif

*    ER1: Water reservoir de (mm)
*    ER2: Snow/Ice reservoir (mm)
*    The following variables are rate per second
*    PR1: Precipitation rate, rain (mm/sec)
*    PR2: Precipitation rate, snow (mm/sec)
*    PRG: Transfert rate from one reservoir to the other
*    FV1: Liquid condensation (negative of evaporation)
*    FV2: Solid condensation
*    DX : Melted water
*    MEA: Maximum water quantity => after there is runoff
*    3e-3*( max(REAL(ER2),REAL(MEA(2)))-MEA(2)) ): Runoff, 
*     proportionnal to reservoir hight.

*    Only remove snow for sufficient treatment type - 3 (Cowie)

      if ( TMTTYP .eq. 3 ) then
         REMVSN = 3e-4
      else
         REMVSN = 0.0
      end if

      ER1 = max(0.0,REAL(ER1 + DT*(
     *     PR1 + FV1 + DX - 
     *     3e-3*( max(REAL(ER1),MEA(1))-MEA(1)) ) ))
      ER2 = max(0.0,REAL(ER2 + DT*(
     *     PR2 + FV2 - DX 
     *     - REMVSN * ( max(REAL(ER2),MEA(2))-MEA(2)) ) ))
*     *     - 3e-4*( max(REAL(ER2),MEA(2))-MEA(2)) ) ))

*     Road condition computation
*
*         Code: 1. -> Dry road
*               2. -> Wet road
*               3. -> Ice/snow on the road
*               4. -> Mix water/snow on the road
*               5. -> Dew
*               6. -> Melting snow
*               7. -> Frost
*               8. -> Icing rain 
*
*     --------------------------------------------
      CUTOFF = 0.2
      if ( PRG .gt. 0.0 ) then
*     -----------------------------------------------------------
*        8. -> Icing rain 
*     -----------------------------------------------------------
         ETAT = 8
      else if ( PRG .lt. 0.0 ) then
*     -----------------------------------------------------------
*        6. -> Melting snow
*     -----------------------------------------------------------
         ETAT = 6
      else if ( ER1.lt.CUTOFF .and. ER2.lt.CUTOFF ) then
         if ( FV1.gt.0.0 .and. PR1 .eq. 0.0 ) then

*     -----------------------------------------------------------
*           5. -> Dew
*     -----------------------------------------------------------
            ETAT = 5
         else if ( ( FV2 .gt. 0.0 .and. PR2 .eq. 0.0 ) .or.
     *               DX .lt. 0.0 ) then
*     -----------------------------------------------------------
*           7. -> *** Black ice warning ***
*                 Frost or icing of water on the ground
*     -----------------------------------------------------------
            ETAT = 7
         else
*     -----------------------------------------------------------
*           1. -> Dry road
*     -----------------------------------------------------------
            ETAT = 1
         end if
      else if ( ER1 .ge. CUTOFF .and. ER2 .ge. CUTOFF ) then
*     -----------------------------------------------------------
*        4. -> Phase transition : Mix water/snow on the road
*     -----------------------------------------------------------
         ETAT = 4
      else if ( ER1 .ge. CUTOFF ) then
*     -----------------------------------------------------------
*        2. -> Wet road
*     -----------------------------------------------------------
         ETAT = 2
      else if ( ER2 .ge. CUTOFF ) then
*     -----------------------------------------------------------
*        3. -> Ice/snow on the road
*     -----------------------------------------------------------
         ETAT = 3
      end if

*      WRITE(*,*) ETAT
*      WRITE(*,*) "PRG", PRG
*      WRITE(*,*) "ER1", ER1
*      WRITE(*,*) "FV1", FV1
*      WRITE(*,*) "PR1", PR1
*      WRITE(*,*) "ER2", ER2
*      WRITE(*,*) "FV2", FV2
*      WRITE(*,*) "PR2", PR2
      return
      end


************************************************************************
***
*     Sous-routine SRFHUM: Calcul l'humidite specifique a la surface de
*                          la route
*
*     Auteur / Author: Louis-Philippe Crevier
*     Date: Decembre 1999 / December 1999
***
      SUBROUTINE SRFHUM ( QG, CL, ER1, ER2, TSK, P0, QA, FP )
      IMPLICIT NONE
***                 ***
*     DEFINITIONS     *
***                 ***
***
*     Entrees
*     -------
***
      DOUBLE PRECISION ER1, ER2, P0, QA, FP
      REAL TSK
***
*     Sorties
*     -------
***
      REAL QG, CL
***
*     Internes
*     --------
***
      REAL QGsat
***
*     Declarations des constantes physique 
*     et des fonctions thermodynamiques
*     ------------------------------------
***
*
      REAL CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW, EPS1
      REAL DELTA, CAPPA, TGL, CONSOL, GRAV, RAYT, STEFAN, PI
      REAL OMEGA, EPS2
      REAL KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
      COMMON/CTSPHY/  CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW,
     $                EPS1, EPS2, DELTA, CAPPA, TGL, CONSOL,
     $                GRAV, RAYT, STEFAN, PI, OMEGA,
     $                KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
*
*
      REAL TTT,PRS,QQQ,EEE,TVI,QST
      REAL FOEW,FODLE,FOQST,FODQS,FOEFQ,FOQFE,FOTVT,FOTTV,FOHR
      REAL FOEWA,FODLA,FOQSA,FODQA,FOHRA
*
      INTRINSIC SIGN

*
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
*     Humidite selon Sass (1992) + 
*     recommandation de Davies pour min(QGsat,QA)
      QGsat = FOQST ( REAL(TSK) , REAL(P0) )
      QG = min(1.0,REAL((ER1+ER2)/0.5))*QGsat +
     *     max(0.0,1.0-REAL((ER1+ER2)/0.5))
     *     *min(REAL(QGsat),REAL(QA))


      if ( ER1.gt.0.0 ) then
         CL = CHLC
      else if ( ER2.gt.0.0 ) then
         CL = CHLC + CHLF
      else
         if ( TSK - TCDK .gt. FP ) then
*     condensation sur route seche
            CL = CHLC
         else
*     frimas sur route seche
            CL = CHLF + CHLC
         end if
      end if
      return
      end
