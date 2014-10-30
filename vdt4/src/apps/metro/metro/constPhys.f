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
************************************************************************

************************************************************************
***   
*     Sous-routine SETCONSTPHYS: Initialise les constantes physiques 
*     qui seront utilisee partout dans le code fortran.  Ces constantes
*     et ces formules sont recuperees de METRo 1.o
*     
*     Auteur/Author: Miguel Tremblay
*     Date: 10 Juin 2004
************************************************************************

      SUBROUTINE SETCONSTPHYS (silent)
      IMPLICIT NONE

      
      REAL CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW, EPS1
      REAL DELTA, CAPPA, TGL, CONSOL, GRAV, RAYT, STEFAN, PI
      REAL OMEGA, EPS2
      REAL KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD
      
      REAL AS,ASX,CI,BS,BETA,FACTN,HMIN
      LOGICAL bSilent, silent
      INTEGER Nl, n
      REAL DT
      INTEGER DTMAX
      COMMON/CTSPHY/  CPD, CPV, RGASD, RGASV, TRPL, TCDK, RAUW,
     *                EPS1, EPS2, DELTA, CAPPA, TGL, CONSOL,
     *                GRAV, RAYT, STEFAN, PI, OMEGA,
     *                KNAMS, STLO, KARMAN, RIC, CHLC, CHLF, DG2RAD

      COMMON /SURFCON/AS,ASX,CI,BS,BETA,FACTN,HMIN
      COMMON /SILENT/ bSilent
      

*     DTMAX -> # de pas de temps maximal
*     Nl -> nombre de pas de temps maximal de la periode contenant 
*     les observations (48h + un coussin = 60h)
*     DT -> pas de temps du modele de bilan energetique
*     n -> nombre maximal de niveaux des grilles dans le sol
      REAL PADDING
      COMMON /BUFFER_SIZE/ DTMAX, Nl, DT, PADDING, n

      REAL TTT,PRS,QQQ,EEE,TVI,QST
      REAL FOEW,FODLE,FOQST,FODQS,FOEFQ,FOQFE,FOTVT,FOTTV,FOHR
      REAL FOEWA,FODLA,FOQSA,FODQA,FOHRA

      INTRINSIC SIGN

***   
*
*   DEFINITION DES FONCTIONS THERMODYNAMIQUES DE BASE
*   POUR LES CONSTANTES, UTILISER LE COMMON /CTESDYN/
*     NOTE: TOUTES LES FONCTIONS TRAVAILLENT AVEC LES UNITES S.I.
*           I.E. TTT EN DEG K, PRS EN PA, QQQ EN KG/KG
*    *** N. BRUNET - MAI 90 ***
*    * REVISION 01 - MAI 94 - N. BRUNET
*                          NOUVELLE VERSION POUR FAIBLES PRESSIONS
*
*     FONCTION DE TENSION DE VAPEUR SATURANTE (TETENS) - EW OU EI SELON TT
      FOEW(TTT) = 610.78*EXP( AMIN1(SIGN(17.269,TTT-TRPL),SIGN 
     W (21.875,TTT-TRPL))*ABS(TTT-TRPL)/(TTT-35.86+AMAX1(0.,SIGN 
     W (28.2,TRPL-TTT))))
*
*     FONCTION CALCULANT LA DERIVEE SELON T DE  LN EW (OU LN EI)
      FODLE(TTT)=(4097.93+AMAX1(0.,SIGN(1709.88,TRPL-TTT)))
     W  /((TTT-35.86+AMAX1(0.,SIGN(28.2,TRPL-TTT)))*(TTT-35.86+AMAX1(0.
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

      if(silent) then
         bSilent = .true.
      else
         bSilent = .false.
      end if

      if( .not. bSilent) then
         WRITE(*,*) "DEBUT SETCONSTPHYS"
      end if
*     initialiser les constantes physiques
      PI = 3.141592653590e0
      STEFAN = 5.6698e-8
      OMEGA = 0.7292e-4
      CPD = 0.10054600e4
      CHLC = 0.250100e7
      RGASD = .28705E3
      RGASV = .46151E3
      TRPL = .27316e3
      TCDK = .27315e3
      RAUW = .1e4
      EPS1 = .62194800221014
      EPS2 = .3780199778986
      DELTA = .6077686814144
      CAPPA = .28549121795
      TGL = .27316e3
      CONSOL = .1367e4
      GRAV = .980616e1
      RAYT = .637122e7
      KNAMS = .514791
      RIC = .2
      CHLF = .334e6
      KARMAN = .40
      DG2RAD = 0.17453293e-1
      AS = 12.
      ASX= 5.0
      CI = 40.
      BS = 1.0
*     Facteur multiplicatif du flux de chaleur
      BETA = 1.0
      FACTN = 1.2
      HMIN = 35.
*     Dimension des arrays
*      DTMAX = 12000  (original value, 100 hours for 30s timestep)
*      Nl = 11520     (original value 96 hrs 30s timestep)
      DT = 30.0
      n = 200

*     New computation utilizing number of days and timestep
      DTMAX = 8*24*3600/DT
      Nl = 4*24*3600/DT

      if( .not. bSilent) then
         WRITE(*,*) "FIN SETCONSTPHYS"
      end if

      END SUBROUTINE SETCONSTPHYS
