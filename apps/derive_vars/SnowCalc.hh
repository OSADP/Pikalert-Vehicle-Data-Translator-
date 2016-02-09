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
// $Id: SnowCalc.hh,v 1.24 2004/09/08 03:26:39 jaimi Exp $
//
///////////////////////////////////////////////////////////

#ifndef _SNOW_CALC_INC_
#define _SNOW_CALC_INC_

#include <string>

class SnowCalc 
{
public:

   //
   // Constructor
   //   numOutDays  = number of output days; that is the number
   //                 of days we will be predicting blowing
   //                 snow potential
   //   numOutTimes = number of output times per day; that is 
   //                 the number of times per day that we will
   //                 be predicting blowing snow potential
   //   maxSites    = the maximum number of output sites
   //
   //   The constructor will set up and initialize all the
   //   intermediate and output arrays.
   //
   SnowCalc( int numOutDays, int numOutTimes, int maxSites );

   //
   // Destructor
   //
   ~SnowCalc();
   
   //
   // Calculate the blowing snow potential
   //   wspd             = wind speed
   //   T                = temperature
   //   rain             = is it raining?
   //   snow             = is it snowing?
   //   ice              = is there freezing rain?
   //   dataTime         = time data
   //   numSites         = number of sites we are processing
   //   numInValsPerSite = total number of input values for each
   //                      site for each variable.  This will be
   //                      the total of the number of obs and the
   //                      number of forecast values for each
   //                      variable and each site
   //   dataOffset       = the offset into the forecast data at
   //                      each site where the forecast actually
   //                      begins; e.g. the user will supply a
   //                      forecast time on the command line of
   //                      say 18:00.  The forecast time in the
   //                      file will be 00:00 of the same day.
   //                      So for each site we need to skip
   //                      over 18 hours of data to get to the forecast.
   //                      In the forecast file, these will be 'hindcasts'.
   //                      In our data, this will be obs.  If the
   //                      number of forecast times per day is say
   //                      8, then the delta time is 3 hours and
   //                      we would need to skip over 6 data points
   //                      at each site
   //
   //   This method calculates the blowing snow potential and blowing
   //   snow index for each forecast time.  The blowing
   //   snow index is one of 4 values (0,1,2,3) which gives us
   //   a warning level of how likely it is that blowing snow will
   //   occur, 0 being not very likely and 3 being very likely.
   //
   int calc( float* wspd, float* T, float* rain, float* snow, float* ice, 
             double* dataTime, int numSites, int numInValsPerSite, 
             int dataOffset );

   //
   // Get the blowing snow potential values
   //   The blowing snow potential is a number between 0 and 1 
   //   which gives an indication of how likely it is that blowing 
   //   snow will occur.  
   //
   //   Note that this class allocates and deletes the memory
   //   associated with this array
   //
   float* getBlowingSnowVals(){ return blowingSnowVals; }

   //
   // Get the blowing snow potential index values
   //   The blowing snow index is one of 4 values (0,1,2,3) which 
   //   gives us a warning level of how likely it is that blowing 
   //   snow will occur, 0 being not very likely and 3 being very 
   //   likely.
   //
   //   Note that this class allocates and deletes the memory
   //   associated with this array
   //
   float* getBlowingSnowIndex(){ return blowingSnowIndex; }

   //
   // Print function for debugging
   //   siteIds          = list of site ids
   //   dataTime         = time data
   //   T                = temperature
   //   pop              = probability of precip
   //   qpf              = qpf
   //   cpos             = conditional probability of snow
   //   cpor             = conditional probability of rain
   //   cpoi             = conditional probability of ice
   //   snow             = is it snowing? (1=yes, 0=no)
   //   rain             = is it raining? (1=yes, 0=no)
   //   ice              = is there freezing rain? (1=yes, 0=no)
   //   wspd             = wind speed
   //   numSites         = number of sites
   //   numInValsPerSite = total number of values per site (obs+fcsts)
   //   dataOffset       = offset into data where forecast begins
   //
   //   Note that this will not print anything if the debug
   //   level is set less than 8.  In addition, this will print
   //   to the specified log file.
   //
   void printData( int* siteIds, double* dataTime, float* T, float* pop, 
                   float* qpf, float* cpos, float* cpor, float* cpoi, 
                   float* snow, float* rain, float* ice, float* wspd, 
                   int numSites, int numInValsPerSite, int dataOffset );
   
   //
   // Constants
   //  MIN_TEMP            = minimum temperature possible
   //  MT_LOOK_BACK_SECS   = look back in seconds for max temperuature map
   //  SNOW_LOOK_BACK_SECS = look back in seconds to find snow
   //  SECS_PER_HOUR       = seconds per hour
   //  MISSING_STR         = string used to indicate missing data
   //                        in print statements
   //
   static const double MIN_TEMP;
   static const double MT_LOOK_BACK_SECS;
   static const double SNOW_LOOK_BACK_SECS;
   static const double SECS_PER_HOUR;
   static const string MISSING_STR;

private:

   //
   // Is there precip data available in the obs at the given
   // site?
   //
   bool    obsPrecipAvailable;
   
   //
   // Dimensions
   //  numOutputDays  = number of output days
   //  numOutputTimes = number of output times per day
   //  numMaxSites    = maximum number of sites in output
   //  timeInterval   = time interval between forecasts
   //
   int     numOutputDays;
   int     numOutputTimes;
   int     numMaxSites;
   double  timeInterval;
   
   //
   // Intermediate data 
   //   snage = snow age
   //   maxT  = maximum temperature
   //
   double *snage;
   float  *maxT;

   //
   // Interest maps
   //   snageMap   = snow age map
   //   wspdMap    = wind speed map
   //   liqprcpMap = liquid precip map
   //   maxTMap    = maximum temperature map
   //
   float  *snageMap;
   float  *wspdMap;
   float  *liqprcpMap;
   float  *maxTMap;

   //
   // Blowing snow values
   //   blowingSnowVals = blowing snow potential values
   //   blowingSnowIndex = blowing snow potential index values
   //
   float  *blowingSnowVals;
   float  *blowingSnowIndex;

   //
   // Indeces for snow start and end
   //   snowStartIndex = index into data array for snow start
   //   snowEndIndex   = index into data array for snow end
   //
   int     snowStartIndex;
   int     snowEndIndex;

   //
   // Calculate index
   //   val = blowing snow potential value
   //
   //   Calculates the blowing snow potential index value given
   //   a blowing snow potential value
   //
   float   calcIndex( float val );

   //
   // Find the snow bounds
   //   snow             = is it snowing?
   //   currentSiteIndex = index of current site in the site list
   //   numObs           = number of obs
   //   numTimes         = number of times per site
   //
   //   Finds the snow bounds looking back from the first forecast
   //   time.
   //
   void    findSnowBounds( float* snow, int currentSiteIndex, 
                           int numObs, int numTimes );

   //
   // Update the snow bounds
   //   snow             = is it snowing?
   //   currentSiteIndex = index of current site in site list
   //   currentTimeIndex = index from the current site in the
   //                      data array for the given time
   //   numTimes         = number of times per site
   //
   //   Decides if it has snowed or stopped snowing since the last 
   //   time we detected snow and updates as necessary
   //
   void    updateSnowBounds( float* snow, int currentSiteIndex, 
                             int currentTimeIndex, int numTimes );

   //
   // Wind speed interest map
   //   wspd      = wind speed 
   //   wspdIndex = index of where we are in the wind speed map array.
   //               That is, which value in the wind speed map array
   //               are we currently calculating?
   //
   //   Calculates the wind speed interest map value for the given
   //   wind speed and places the value in the wspdIndex position
   //   of the wspdMap array.
   //
   void    WSPDmap( float wspd, int wspdIndex );

   //
   // Snow age interest map
   //   snowVal     = is it snowing; used only to decide if we have
   //                 valid data at this time so that we can know if
   //                 it is snowing or not
   //   snowTime    = time of most recent snow
   //   currentTime = time associated with the value for which
   //                 we are calculating the snow age map
   //   snageIndex  = index of where we are in the snow age map
   //                 array. That is, which value in the snow age
   //                 map array are we currently calculating?
   //
   //   Calculates the snow age and the snow age interest value
   //   for the given snow time and 'current' time and places the
   //   value in the snageIndex position of the snageMap array.
   //   It also places the snow age in the snageIndex position of
   //   the snage array.
   //
   void    SNAGEmap( float snowVal, double snowTime, double currentTime, 
                     int snageIndex );

   //
   // Liquid precip interest map
   //   rain             = is it raining?
   //   ice              = is there freezing rain?
   //   currentSiteIndex = index of current site in the site list
   //   currentTimeIndex = index in data arrays from current site
   //   numTimes         = number of times for this site
   //   liqIndex         = index of where we are in the liquid 
   //                      precip map array. That is, which value
   //                      in the liquid precip map array are we
   //                      currently calculating?
   //  
   //   Calculates the liquid precip interest value and places
   //   the value in the liqIndex position of the liqoccMap.
   //
   void    LIQOCCmap( float* rain, float* ice, int currentSiteIndex, 
                      int currentTimeIndex, int numTimes, int liqIndex );

   //
   // Maximum temperature during snow event interest map
   //   T                = temperature
   //   currentSiteIndex = index of current site in the site list
   //   currentTimeIndex = index in data arrays from current site
   //   numTimes         = number of times for this site
   //   maxTIndex        = index of where we are in the maxTMap
   //
   //   Calculates the maximum temperature during the last 
   //   MT_LOOK_BACK_SECS of the most recent snow event.  It
   //   then uses this value to calculate the associated interest
   //   map value.  Each value is put into the maxTIndex position
   //   of their respective arrays.
   //
   //   Note that this method will not get called if there was
   //   not a snow event in the recent enough past to matter.
   //
   void    SNEMAXTmap( float* T, int currentSiteIndex,
                       int currentTimeIndex, int numTimes, int maxTIndex );
};

#endif



