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
************************************************************************


***
*     Sous-routine GRILLE: Creation of network for the thermic conduction
*                           model of the road.
*                          Sert a creer le maillage pour le modele de 
*                           conduction thermique du sol.
*
*     Auteur: Louis-Philippe Crevier
*     Date: Juin 1999
*     Adaptation to C and Fortran77: Miguel Tremblay
*     Date: 26 avril 2004
***
      SUBROUTINE GRILLE ( GRI_IN, CNT_IN, iref, ir40, 
     *     FLAT, NZONE, ZONES, MAT, DIFF, dpDepth, ECHEC )
      IMPLICIT NONE
      INTEGER i, j, k
      INTEGER Nl, n
      REAL DT
      INTEGER DTMAX
      REAL PADDING
      COMMON /BUFFER_SIZE/ DTMAX, Nl, DT, PADDING, n

***               ***
*     DEFINITIONS   *
***               ***
***
*     Input
*     -------
*     FLAT: switch bridge / road
*     MAT: Materiel type of each zone
*     NZONE: number of zones
*     ZONES: limites in differents zones
***
      LOGICAL FLAT, ECHEC
      INTEGER NZONE, MAT(20)
      DOUBLE PRECISION ZONES(20), CS(20), KS(20)
***
*     Output
*     -------
*     iref:  number of grid levels 
*     ir40: level at 40 cm depth
*     GRI: depth of the levels
*     CNT: constants of conduction
*     DIFF: Vector used to create the initial profile of temperature
*     dpDepth: Depth of grid levels
***
      INTEGER iref, ir40
      DOUBLE PRECISION GRI(n,2), CNT(n,2), DIFF
      DOUBLE PRECISION GRI_IN(n*2)
      DOUBLE PRECISION CNT_IN(n*2)
      DOUBLE PRECISION dpDepth(n)
***
*     Local
*     --------
*     CS: ground capacity, by zone
*     KS: ground conductivity, by zone
*     CC: grid parameter ( FLAT = .f. )
*     INTER: indice of present level
*     YPG, YPT: 1st and 2nd derivatives of GRI
*     DY: thickness of levels  ( after transformation )
*     C: CS transpose on GRI
*     Ko: KS transpose on GRI
*     dd: parameter of GRI ( FLAT = .f. )
*     ratio: transition ratio of transfert CS -> C and KS -> Ko
***
      INTEGER INTER, NMAX
      REAL YPG(n), YPT(n), C(n), Ko(n)
      REAL dd, ratio, CC, DY

      LOGICAL bSilent
      COMMON /SILENT/ bSilent

***
*
*     Procedure
*     =========

      if( .not. bSilent) then
         WRITE(*,*) "GRILLE ROUTINE START"
      end if

      DO k=1,2
         DO j=1,n
            CNT(j,k) = 0.
         END DO
      END DO
*     Association of conductivity/capicity on different layers of 
*     materiels.
*     Associer les conductivites/capacites aux differentes 
*     couches de materiaux
*     ----------------------------------------------------
      NMAX = NZONE
      DO k=1,NZONE
         IF ( ZONES(NZONE + 1 - k) .ge. 0.40 ) NMAX = NZONE + 1 - k
         IF ( MAT(k) .eq. 1 ) THEN
*           asphalt(e)
*           ----------
            CS(k) = 2.10e6
            KS(k) = 0.8
         ELSE IF ( MAT(k) .eq. 2 ) THEN
*           gravier / crushed rock
*           ----------------------
            CS(k) = 2.10e6
            KS(k) = 0.95
         ELSE IF ( MAT(k) .eq. 3 ) THEN
*           beton / concrete
*           ----------------
            CS(k) = 2.10e6
            KS(k) = 2.2
         ELSE IF ( MAT(k) .eq. 4 ) THEN
*           sous-sol (sable) / deep soil (sable)
*           ------------------------------------
            CS(k) = 2.0e6
            KS(k) = 1.0
         ELSE
            WRITE(*,*) "Bad material type: ", MAT(k)," for zone", k
            ECHEC = .true.
            return
         END IF
      END DO
      DIFF = (MIN(0.4, REAL(ZONES(1)))/0.4)*KS(1)/CS(1)

      DO i=2,NMAX
         DIFF = ((MIN(0.4,REAL(ZONES(i)))-ZONES(i-1))/0.4)* 
     *        KS(i)/CS(i) + DIFF
      END DO
*     Creation of the grid itself and his derivatives
*     Creer la grille elle-meme et ses derivees
*     -----------------------------------------
      IF ( FLAT ) THEN
*        Cas FLAT = .true. => PONT
*        -------------------------
*        ZONES(NZONE) is the bottom of the 
         DY = max( 0.01 , REAL(ZONES(NZONE)) / real(n) )
         iref = int( ZONES(NZONE) / DY )
         DO j=1,iref
*           Grid of the flux layers
            GRI(j,1) = j * DY
*           Grid of temperature layers
            GRI(j,2) = ( real(j)-0.5 ) * DY
            dpDepth(j) = GRI(j,2)
*           Derivate on the flux layers
            YPG(j) = 1.0
*           Derivate on the temperature layer
            YPT(j) = 1.0
            IF ( GRI(j,2) .le. 0.4 ) ir40 = j
         END DO
      ELSE
*    Case FLAT = .false. => ROAD
*    ---------------------------
         CC = 3.6
         dd = 20.0
         ir40=int(0.5+((1-exp(-(CC*0.4)))/(1-exp(-(CC*0.01)))))
         DY = dd * (1-exp(-(CC*0.4)))/(real(ir40)-0.5)
         j=1
 12      IF ( real(j)*DY/dd .ge. 1.0 ) THEN
            iref = j - 1
         ELSE
            j = j + 1
            go to 12
         END IF
         GRI(1,1) = - (log(( 1 - ( DY / dd ) )) / CC)
         GRI(1,2) = 0.5 * GRI(1,1)
         j=1
         dpDepth(j) = GRI(j,2)
         DO j=2,iref
            GRI(j,1) = - (log((1. -(real(j) * DY / dd))) / CC)
            GRI(j,2) = 0.5 * ( GRI(j,1) + GRI(j-1,1) )
            dpDepth(j) = GRI(j,2)
         END DO
         YPG(1) = dd * CC * exp( - (CC * 0.5 * GRI(1,1) ))
         YPT(1) = dd * CC * exp( -(CC * 0.5 * GRI(1,2) ))
         DO j=2,iref
            YPG(j) = dd * CC * exp( - (CC * GRI(j,1) ))
            YPT(j) = dd * CC * exp( - (CC * GRI(j,2) ))
         END DO
      END IF

*     Association of conductivity/capacity on the grid levels
*     -------------------------------------------------------------
      INTER = 1
      DO j=1,iref
         IF ( GRI(j,1) .ge. ZONES(INTER) ) THEN
            ratio = ( ZONES(INTER) - GRI(j-1,1) ) / 
     *           ( GRI(j,1) - GRI(j-1,1) )
            C(j) = CS(INTER)*ratio + CS(INTER+1)*(1-ratio)
            INTER = INTER + 1
         ELSE
            C(j) = CS(INTER)
         END IF
      END DO
      INTER = 1
      DO j=1,iref
         IF ( GRI(j,2) .ge. ZONES(INTER) ) THEN
            ratio = ( ZONES(INTER) - GRI(j-1,2) ) / 
     *                 ( GRI(j,2) - GRI(j-1,2) )
            Ko(j) = KS(INTER)*ratio + KS(INTER+1)*(1-ratio)
            INTER = INTER + 1
         ELSE
            Ko(j) = KS(INTER)
         END IF
*     Stability check for the CFL condition
         IF ( Ko(j) .lt. 0 ) THEN
            WRITE(*,*) "Numerical stability test failed"
            WRITE(*,*) " for grid level", j
            WRITE(*,*) "Please examine the thicknesses of your "
            WRITE(*,*) " layers, especially the cement if present"
            ECHEC = .true.
            return
         END IF
      END DO

*     Creation of the CNT that contains the contribution of the capacity
*     and conductivity in addition of the used metric factors
*     Creation de CNT qui contient les contributions des capacites et
*     conductivites en plus des facteurs de la metrique utilisee
*     ---------------------------------------------------------------
      CNT(1,1) = - (Ko(1) * YPG(1) / DY)
      CNT(1,2) = - (YPT(1) / ( DY*C(1) ))
      DO j=2,iref
         CNT(j,1)= - (Ko(j) * YPG(j) / DY)
         CNT(j,2)= - (YPT(j) / ( DY*C(j) ))
      END DO
      ECHEC = .false.


      CALL MATRIX2ARRAYDOUBLEPRECISION(GRI,GRI_IN, n, 2)
      CALL MATRIX2ARRAYDOUBLEPRECISION(CNT, CNT_IN, n, 2)

      if( .not. bSilent) then
         WRITE(*,*) "GRILLE ROUTINE ENDED"      
      end if
      

      RETURN
      END SUBROUTINE GRILLE
