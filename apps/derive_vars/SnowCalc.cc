// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
////////////////////////////////////////////////////////////
// SnowCalc - does the blowing snow calculation
//
// $Id: SnowCalc.cc,v 1.41 2007/10/01 20:33:37 cowie Exp $
//
///////////////////////////////////////////////////////////
#include <string>
#include <math.h>

#include "netcdf.hh"
#include "ncf/ncf.hh"
#include "log/log.hh"
#include "SnowCalc.hh"
#include "ConcatData.hh"

extern Log *proc_log;

//
// Constants
//
const double SnowCalc::MIN_TEMP            = -273.15;
const double SnowCalc::MT_LOOK_BACK_SECS   = 3 * 3600.0;
const double SnowCalc::SNOW_LOOK_BACK_SECS = 48 * 3600.0;
const double SnowCalc::SECS_PER_HOUR       = 3600.0;
const string SnowCalc::MISSING_STR         = "missing";

SnowCalc::SnowCalc( int numOutDays, int numOutTimes, int maxSites ) 
{
   obsPrecipAvailable  = false;
   numOutputDays       = numOutDays;
   numOutputTimes      = numOutTimes;
   numMaxSites         = maxSites;
   timeInterval        = 0.0;

   snage            = new double[numMaxSites*numOutputDays*numOutputTimes];
   snageMap         = new float[numMaxSites*numOutputDays*numOutputTimes];
   wspdMap          = new float[numMaxSites*numOutputDays*numOutputTimes];
   liqprcpMap       = new float[numMaxSites*numOutputDays*numOutputTimes];
   maxT             = new float[numMaxSites*numOutputDays*numOutputTimes];
   maxTMap          = new float[numMaxSites*numOutputDays*numOutputTimes];
   blowingSnowVals  = new float[numMaxSites*numOutputDays*numOutputTimes];
   blowingSnowIndex = new float[numMaxSites*numOutputDays*numOutputTimes];

   for( int i = 0; i < numMaxSites*numOutputDays*numOutputTimes; i++ ) {
      snage[i]            = -1.0;
      snageMap[i]         = -1.0;
      wspdMap[i]          = -1.0;
      liqprcpMap[i]       = -1.0;
      maxT[i]             = NC_FILL_FLOAT;
      maxTMap[i]          = -1.0;
      blowingSnowVals[i]  = NC_FILL_FLOAT;
      blowingSnowIndex[i] = NC_FILL_FLOAT;
   }
}

SnowCalc::~SnowCalc() 
{
   delete[] snage;
   delete[] wspdMap;
   delete[] liqprcpMap;
   delete[] maxT;
   delete[] maxTMap;
   delete[] blowingSnowVals;
   delete[] blowingSnowIndex;
}

int
SnowCalc::calc( float* wspd, float* T, float* rain, float* snow, float* ice, 
                double* dataTime, int numSites, int numInValsPerSite, 
                int dataOffset ) 
{
   
   //
   // Set time info
   //
   int numObs = numInValsPerSite - 
      (numOutputDays * numOutputTimes - dataOffset);
   
   timeInterval = (double) SEC_PER_DAY / (double) numOutputTimes;

   //
   // For each site
   //
   for( int ns = 0; ns < numSites; ns++ ) {

      //
      // Initialize indeces
      //
      snowStartIndex = -1;
      snowEndIndex   = -1;

      //
      // Find the first snow event looking back from the
      // first forecast time
      //
      findSnowBounds( snow, ns, numObs, numInValsPerSite );

      //
      // For each forecast day and time
      //
      for( int nd = 0; nd < numOutputDays; nd++ ) {
         for( int nt = 0; nt < numOutputTimes; nt++ ) {

            //
            // Set indeces
            //   timeIndex refers to index in input arrays for a
            //   given site - that is index from the current site
            //   in the input array
            //
            //   currentIndex refers to the 'absolute index' -
            //   that is the index into the input array irrespective
            //   of the site
            //
            //   forecast index refers to the 'absolute index' into 
            //   the output arrays
            //
            //   Note that the dataTime array does not have seperate
            //   values for different sites, so timeIndex is the
            //   'absolute index' for this array
            //
            int timeIndex     = numObs + nd * numOutputTimes + nt;

	    int currentIndex  = ns * numInValsPerSite + timeIndex; 

            int forecastIndex = ns * numOutputDays * numOutputTimes +
               nd * numOutputTimes + nt + dataOffset;

	    //
	    // If we are past the end of the site, go on to the
	    // next site.
	    //   Note that this will break us out of the inner
	    //   nt loop, but usually we will be on the final
	    //   nd loop for this site in this case.  If we
	    //   are not, it will continually break out of the
	    //   nt loop until we get to the next site anyway.
	    //
	    if( forecastIndex >= (ns+1) * numOutputDays * numOutputTimes ) {
	      break;
	    }

	    //
	    // Update the snow bounds
	    //    Note that this could leave the end index
	    //    such that it is more than 48 hours ago, but
	    //    in this case, the snagemap will return
	    //    0, so this will do what we want below.
	    //
	    updateSnowBounds( snow, ns, timeIndex, 
                              numInValsPerSite );
	       
            //
            // If there is no snow we're done with this time
            //
            if( snowEndIndex == -1 ) {

               //
               // Set values accordingly if there was precip,
               // but just no snow - default values indicate
               // precip was missing in obs
               //
               if( obsPrecipAvailable ) {
                  snage[forecastIndex]            = -2.0;
                  blowingSnowIndex[forecastIndex] = 0.0;
                  blowingSnowVals[forecastIndex]  = 0.0;
               }
               continue;
            }

            //
            // Calculate the snow age map, if it produces zero
            // interest, don't bother with the rest of the
            // calculations.  If there is an error, don't bother
            // with any other calculations either.
            //   Note that if snagemap is zero it means that if 
            //   we have missing values or any other errors for 
            //   the other maps, it doesn't matter, we will still 
            //   return a zero interest value for blowing snow index
            //
            SNAGEmap( snow[currentIndex], dataTime[snowEndIndex], 
                      dataTime[timeIndex], forecastIndex );

            if( snageMap[forecastIndex] == -1.0 ) {
               continue;
            }
            
            if( snageMap[forecastIndex] == 0.0 ) {
               blowingSnowVals[forecastIndex]  = 0.0;
               blowingSnowIndex[forecastIndex] = 0.0;
               continue;
            }
               
            //
            // Calculate the other maps
            //
            WSPDmap( wspd[currentIndex], forecastIndex );
            LIQOCCmap( rain, ice, ns, timeIndex, numInValsPerSite, 
                       forecastIndex );
            SNEMAXTmap( T, ns, timeIndex, numInValsPerSite, forecastIndex );

            //
            // If we have error values for any of the maps,
            // use the default missing values and go on
            //   Note that checking for a negative value for blowingSnow
            //   is not enough, since if an even number of the maps
            //   are set to the missing value, the final value will be
            //   positive
            //
            if( wspdMap[forecastIndex] == -1 || 
                liqprcpMap[forecastIndex] == -1 || 
                maxTMap[forecastIndex] == -1 ) {
               continue;
            }

            //
            // Otherwise, calculate blowing snow and the blowing
            // snow index
            //
            else {
               blowingSnowVals[forecastIndex] = wspdMap[forecastIndex] * 
                  snageMap[forecastIndex] * liqprcpMap[forecastIndex] * 
                  maxTMap[forecastIndex];
               
               blowingSnowIndex[forecastIndex] = 
                  calcIndex( blowingSnowVals[forecastIndex] );
            }
            
         }
      }
   }

   return( 0 );
   
}
   

float
SnowCalc::calcIndex( float val ) 
{
   if( val >= 0.0 && val <= 0.05 ) 
      return( 0.0 );
   else if( val > 0.05 && val <= 0.333 )
      return( 1.0 );
   else if( val > 0.333 && val <= 0.667 ) 
      return( 2.0 );
   else if( val > 0.667 && val <= 1.0 )
      return( 3.0 );
   
   //
   // Error
   //
   return( NC_FILL_FLOAT );
}

void
SnowCalc::findSnowBounds( float* snow, int currentSiteIndex, 
                          int numObs, int numTimes ) 
{
   //
   // Set an index to the start of the values at this site
   //
   int currentIndex = currentSiteIndex * numTimes;

   //
   // Set the end index
   //
   int lookBack = (int) floor( SNOW_LOOK_BACK_SECS / timeInterval );
   int endIndex = max( numObs - lookBack, 0 );
   
   //
   // Look back from the current time to find the first
   // occurrance of snow.  Don't go back into the previous
   // site's data.
   //
   bool snowEndFound = false;
   for( int i = numObs; i >= endIndex; i-- ) {
      int idex = currentIndex + i;
     
      if( snow[idex] != ConcatData::MISSING_PRECIP ) {

	 if( i < numObs )
	   obsPrecipAvailable = true;

         if( snow[idex] > 0 ) {
            snowEndIndex = i;
	    snowEndFound = true;
	    break;
         }
         
      }
   }

   //
   // If we found a snow event, look back from there
   // to find the start of it
   //
   if( snowEndFound ) {

      bool snowStartFound = false;
      int  lastSnowIndex  = snowEndIndex;
      
      for( int i = snowEndIndex; i >= 0; i-- ) {
	 int idex = currentIndex + i;

         //
         // If we have a missing value for snow
         // set the snow start index to the last 
         // known time of snow, but keep looking
         //   Note that this takes the approach that 
         //   missing values do NOT indicate the start
         //   or end of a storm
         //
         if( snow[idex] == ConcatData::MISSING_PRECIP ) {
            snowStartIndex = lastSnowIndex;
            snowStartFound = true;
         }

         // 
         // Set the index for the last known snow
         // event
         //
         else if( snow[idex] > 0 ) {
            lastSnowIndex = i;
         }
	
	 //
	 // It is not snowing. This is the first that this 
	 // has occured since we've been looking back through 
	 // a snow event, so that means the previous time was 
	 // the beginning of the snow event.
	 // 
	 else {
	    snowStartIndex = lastSnowIndex;
	    snowStartFound = true;
	    break;
	 }
      }
      
      //
      // If there was no beginning at this site, we were
      // in the middle of the storm when we started gathering
      // data.  Use the first time as the start in this case.
      //
      if( !snowStartFound ) {
         snowStartIndex = 0;
      }
   }
}


void
SnowCalc::updateSnowBounds( float* snow, int currentSiteIndex, 
                            int currentTimeIndex, int numTimes ) 
{

   //
   // Find the index into the arrays for this site and this time
   //
   int currentIndex = currentSiteIndex * numTimes + currentTimeIndex;

   //
   // If we have a missing value for snow
   // do nothing, i.e. don't update the indeces
   // 
   if( snow[currentIndex] == ConcatData::MISSING_PRECIP ) {
      return;
   }
      
   //
   // If it is snowing now, update the ending index
   //
   if( snow[currentIndex] > 0 ) {
      int newEndIndex = currentTimeIndex;

      //
      // Check if we need to update the starting index
      //   This will only happen if the snow stopped and
      //   then started again or if it was never snowing 
      //   until now.  We know this has happened
      //   if it was not snowing at the previous time. 
      //   Otherwise, it would have been caught at the 
      //   last pass.
      //
      if( currentTimeIndex > 0 ) {
	int prevIndex = currentSiteIndex * numTimes + currentTimeIndex - 1;

        //
        // Skip over missing values in snow
        //   Note that this takes the approach that missing values
        //   do NOT indicate the start or end of a storm
        //
        while( snow[prevIndex] == ConcatData::MISSING_PRECIP &&
               prevIndex > snowEndIndex ) {
	  prevIndex--;
	}
     
	//
        // If it is not snowing...
        // 
        if( snow[prevIndex] == 0 ) {
	  snowStartIndex = currentTimeIndex;
	}

	snowEndIndex = newEndIndex;
      }
   }
   
}

void
SnowCalc::WSPDmap( float wspd, int wspdIndex ) 
{
   if( wspd == NC_FILL_FLOAT ) {
      wspdMap[wspdIndex] = -1.0;
      return;
   }
   
   float wmap = 1.0;

   if( wspd < 3.75 )
      wmap = 0.0;
   else if( wspd >= 3.75 && wspd < 8.75 )
      wmap = ( wspd - 3.75 ) / 5.0;

   wspdMap[wspdIndex] = wmap;
}

void
SnowCalc::SNAGEmap( float snowVal, double snowTime, double currentTime, 
                    int snageIndex ) 
{
   if( snowVal == ConcatData::MISSING_PRECIP || 
       snowTime == NC_FILL_DOUBLE || 
       currentTime == NC_FILL_DOUBLE ) {
      snage[snageIndex]    = -1.0;
      snageMap[snageIndex] = -1.0;
      return;
   }
   
   double snowAge    = (currentTime - snowTime) / SECS_PER_HOUR;
   float  snowAgeMap = 0.0;

   if( snowAge < 12.0 )
     snowAgeMap = 1.0;
   else if( snowAge >= 12 && snowAge < 72 )
     snowAgeMap = (72.0 - snowAge) / 60.0;
   
   snage[snageIndex]    = snowAge;
   snageMap[snageIndex] = snowAgeMap;

}

void
SnowCalc::LIQOCCmap( float* rain, float* ice, int currentSiteIndex, 
                     int currentTimeIndex, int numTimes, int liqIndex ) 
{

   //
   // Set the indeces
   //   currentIndex is the beginning of the values for this site
   //   endIndex is the farthest back we are going to look for
   //     the values at this site
   //
   int currentIndex = currentSiteIndex * numTimes;

   //
   // Look through the values at this site
   //
   bool nonMissingPrecip = false;
   
   for( int i = currentTimeIndex; i >= snowEndIndex; i-- ) {
      int idex = currentIndex + i;

      //
      // Is there a non-missing value for precip vars?
      // 
      if( rain[idex] != ConcatData::MISSING_PRECIP ||
          ice[idex] != ConcatData::MISSING_PRECIP ) {
         nonMissingPrecip = true;
         
     
         //
         // If it is precipitating and it is some type
         // of liquid precip, we are done
         // 
         if( (rain[idex] != ConcatData::MISSING_PRECIP && rain[idex] > 0) || 
             (ice[idex] != ConcatData::MISSING_PRECIP && ice[idex] > 0) ) {
            liqprcpMap[liqIndex] = 0.0;
            return;
         }
      }
   }

   //
   // If we had no precip information, return the error value
   //
   if( !nonMissingPrecip ) {
      liqprcpMap[liqIndex] = -1.0;
      return;
   }
   
   //
   // If we've reached this point, there was no liquid precip
   //
   liqprcpMap[liqIndex] = 1.0;
}

void
SnowCalc::SNEMAXTmap( float* T, int currentSiteIndex,
                      int currentTimeIndex, int numTimes, int maxTIndex ) 
{
   //
   // Find the indeces to look through to find the max temp
   //
   int lookBack     = (int) floor( MT_LOOK_BACK_SECS / timeInterval );
   int startIndex   = max( snowEndIndex - lookBack, snowStartIndex );
   int currentIndex = currentSiteIndex * numTimes;
   
   //
   // Initialize
   //
   float snemaxt    = MIN_TEMP;
   float snemaxtmap = 0.0;
   
   //
   // Find the max
   //
   for( int i = startIndex; i <= currentTimeIndex; i++ ) {

      int idex = currentIndex + i;

      if( T[idex] != NC_FILL_FLOAT && T[idex] > snemaxt ) {
         snemaxt = T[idex];
      }
   }

   //
   // If there were no non-missing values over this
   // range, return the "no interest" value
   //
   if( snemaxt == MIN_TEMP ) {
      maxT[maxTIndex] = NC_FILL_FLOAT;
      maxTMap[maxTIndex] = -1.0;
      return;
   }
   
   //
   // Translate the max temp into an "interest" value
   //
   if( snemaxt <= -4.0 )
      snemaxtmap = 1.0;
   else if( snemaxt > -4.0 && snemaxt <= 1.0 )
      snemaxtmap = pow( (1.0-snemaxt)/5.0, 0.75 );

   maxT[maxTIndex] = snemaxt;
   maxTMap[maxTIndex] = snemaxtmap;
   
}

void
SnowCalc::printData( int* siteIds, double* dataTime, float* T, float* pop, 
                     float* qpf, float* cpos, float* cpor, float* cpoi, 
                     float* snow, float* rain, float* ice, float* wspd, 
                     int numSites, int numInValsPerSite, int dataOffset ) 
{
   int numObs = numInValsPerSite - 
      (numOutputDays * numOutputTimes - dataOffset);

   for( int ns = 0; ns < numSites; ns++ ) {
      
      proc_log->write( 8, "\n\nsite %d = %d\n"
                       "\tday  hour     "
                       "temp      "
                       "pop      "
                       "qpf     "
                       "cpos     "
                       "cpor     "
                       "cpoi     "
                       "rain     "
                       "snow      "
                       "ice  "
                       "snow age  "
                       "snow age map     "
                       "wspd  "
                       "wspd map  "
                       "liq prcp map    "
                       "max T  "
                       "max T map  "
                       "blowing snow  "
                       "blowing snow index\n", ns, siteIds[ns] );

      //
      // For each forecast day and time
      //
      for( int nd = 0; nd < numOutputDays; nd++ ) {
         for( int nt = 0; nt < numOutputTimes; nt++ ) {

            //
            // Set indeces
            //   timeIndex refers to index in input arrays for a
            //   given site - that is index from the current site
            //   in the input array
            //
            //   currentIndex refers to the 'absolute index' -
            //   that is the index into the input array irrespective
            //   of the site
            //
            //   forecast index refers to the 'absolute index' into 
            //   the output arrays
            //
            //   Note that the dataTime array does not have seperate
            //   values for different sites, so timeIndex is the
            //   'absolute index' for this array
            //
            int timeIndex     = numObs + nd * numOutputTimes + nt;

	    int currentIndex  = ns * numInValsPerSite + timeIndex; 

            int forecastIndex = ns * numOutputDays * numOutputTimes +
               nd * numOutputTimes + nt + dataOffset;


            //
	    // If we are past the end of the site, go on to the
	    // next site.
	    //   Note that this will break us out of the inner
	    //   nt loop, but usually we will be on the final
	    //   nd loop for this site in this case.  If we
	    //   are not, it will continually break out of the
	    //   nt loop until we get to the next site anyway.
	    //
	    if( forecastIndex >= (ns+1) * numOutputDays * numOutputTimes ) {
	      break;
	    }

            //
            // Time -- day and hour
            //
            time_t     currentTime = (time_t) dataTime[timeIndex];
            struct tm *tmStruct    = gmtime( &currentTime );

            proc_log->write( 8, "\t %.2d    %.2d  ", 
                             tmStruct->tm_mday, tmStruct->tm_hour );

            //
            // Temperature
            //
            if( T[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", T[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // POP
            //
            if( pop[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", pop[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // QPF
            //
            if( qpf[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", qpf[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // CPOS
            //
            if( cpos[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", cpos[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // CPOR
            //
            if( cpor[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", cpor[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // CPOI
            //
            if( cpoi[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", cpoi[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }

            //
            // Rain
            //
            if( rain[currentIndex] != ConcatData::MISSING_PRECIP ) {
               proc_log->write( 8, "%7.0f  ", rain[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }
            
            //
            // Snow
            //
            if( snow[currentIndex] != ConcatData::MISSING_PRECIP ) {
               proc_log->write( 8, "%7.0f  ", snow[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }
            
            //
            // Ice
            //
            if( ice[currentIndex] != ConcatData::MISSING_PRECIP ) {
               proc_log->write( 8, "%7.0f  ", ice[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }
               
            //
            // Snow age
            //
            if( snage[forecastIndex] == -2.0 ) {
               proc_log->write( 8, " no snow  " );
            }
            else if( snage[forecastIndex] == -1.0 ) {
               proc_log->write( 8, " %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, " %7.0f  ", snage[forecastIndex] );
            }
            
            //
            // Snow age map
            //
            if( snageMap[forecastIndex] == -1.0 ) {
               proc_log->write( 8, "     %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, "     %7.2f  ", snageMap[forecastIndex] );
            }
            
            //
            // Wind speed
            //
            if( wspd[currentIndex] != NC_FILL_FLOAT ) {
               proc_log->write( 8, "%7.2f  ", wspd[currentIndex] );
            }
            else {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }
            
            //
            // Wind speed map
            //
            if( wspdMap[forecastIndex] == -1.0 ) {
               proc_log->write( 8, " %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, " %7.2f  ", wspdMap[forecastIndex] );
            }
            
            //
            // Liquid precip map
            //
            if( liqprcpMap[forecastIndex] == -1.0 ) {
               proc_log->write( 8, "     %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, "     %7.2f  ", liqprcpMap[forecastIndex] );
            }
            
            //
            // Max temp over storm
            if( maxT[forecastIndex] == NC_FILL_FLOAT ) {
               proc_log->write( 8, "%s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, "%7.2f  ", maxT[forecastIndex] );
            }
            
            //
            // Max temp map
            //
            if( maxTMap[forecastIndex] == -1.0 ) {
               proc_log->write( 8, " %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, " %7.2f  ", maxTMap[forecastIndex] );
            }
            
            //
            // Blowing snow values
            //
            if( blowingSnowVals[forecastIndex] == NC_FILL_FLOAT ) {
               proc_log->write( 8, "     %s  ", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, "     %7.2f  ", 
                                blowingSnowVals[forecastIndex] );
            }
            
            //
            // Blowing snow index
            //
            if( blowingSnowIndex[forecastIndex] == NC_FILL_FLOAT ) {
               proc_log->write( 8, "           %s\n", MISSING_STR.c_str() );
            }
            else {
               proc_log->write( 8, "           %7.2f\n", 
                                blowingSnowIndex[forecastIndex] );
            }
         }
         
      }
   }
}

            

