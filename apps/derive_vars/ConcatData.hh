// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
///////////////////////////////////////////////////////
// ConcatData - class that concatinates desired data
//              from a history of obs and data from
//              the forecast
//
// $Id: ConcatData.hh,v 1.21 2004/09/08 03:26:39 jaimi Exp $
//
///////////////////////////////////////////////////////

#ifndef _CONCAT_DATA_INC_
#define _CONCAT_DATA_INC_

#include <map>
#include <vector>
#include "FcstFile.hh"

//
// Forward Class Declarations
//
class ObsFile;

class ConcatData 
{
public:

   //
   // Constructor
   //   maxHist      = maximum amount of history for obs in days
   //   forecastTime = time of first forecast
   //   dl           = debug level
   //
   ConcatData( long maxHist, double forecastTime, int dl );

   //
   // Destructor
   //
   ~ConcatData();
   
   //
   // Initialize
   //   obsConfigFileName = obs configuration file name
   //   forecastFileName  = forecast file name
   //   outputSiteIdArray = output site id array
   //   nOutputSite       = number of output sites
   //   nFcstDays         = number of forecast days
   //
   //   Initializes forecast file.  Reads obs config file and
   //   initializes the files found in there.  Sets up data
   //   arrays and vector of obs files
   //
   int init( char* obsConfigFileName, char* forecastFileName,
             int* outputSiteIdArray, int nOutputSite, int nFcstDays );

   //
   // Get the total number of days in the concatinated arrays
   //
   int getNumDays(){ return numDays; }

   //
   // Get the number of history (or obs) days
   //
   int getNumHistoryDays(){ return obsFiles.size(); }

   //
   // Get the number of values for each site.  This will
   // be the number of obs and the number of forecast times
   // for each site.
   //
   int getNumValsPerSite(){ return numDataValsPerSite; }
  
   //
   // Get data arrays
   //   Note that this class owns this data so it is
   //   responsible for allocating and freeing the
   //   memory associated with this data
   //
   double* getDataTime(){ return timeArray; }
   float*  getWspd(){ return wspd; }
   float*  getT(){ return T; }
   float*  getPop(){ return pop; }
   float*  getQpf(){ return qpf; }
   float*  getCpos(){ return cpos; }
   float*  getCpor(){ return cpor; }
   float*  getCpoi(){ return cpoi; }
   float*  getSnow(){ return snow; }
   float*  getRain(){ return rain; }
   float*  getIce(){ return ice; }

   //
   // Constants
   //  FILE_NAME_LEN  = length of file names
   //  WSPD_NAME      = name to be used when printing wind speed values
   //  T_NAME         = name to be used when printing temperature values
   //  POP_NAME       = name to be used when printing pop values
   //  QPF_NAME       = name to be used when printing qpf values 
   //  CPOS_NAME      = name to be used when printing cpos values
   //  CPOR_NAME      = name to be used when printing cpor values 
   //  CPOI_NAME      = name to be used when printing cpoi values
   //  SNOW_NAME      = name to be used when printing snow values
   //  ICE_NAME       = name to be used when printing ice values
   //  RAIN_NAME      = name to be used when printing rain values
   //  MISSING_PRECIP = missing precip value for snow, ice and rain
   //                   arrays (1=yes, 0=no, MISSING_PRECIP=missing)
   //
   static const int    FILE_NAME_LEN;
   static const string WSPD_NAME;
   static const string T_NAME;
   static const string POP_NAME;
   static const string QPF_NAME;
   static const string CPOS_NAME;
   static const string CPOR_NAME;
   static const string CPOI_NAME;
   static const string SNOW_NAME;
   static const string ICE_NAME;
   static const string RAIN_NAME;
   static const float  MISSING_PRECIP;

private:

   //
   // debug level
   //
   int                debugLevel;

   //
   // Dimensions
   //   numDays            = total number of days for each site
   //   numFcstDays        = number of forecast days for each site
   //   maxHistory         = maximum number of days of history for each site
   //   numOutputSites     = number of output sites
   //   numDataValsPerSite = number of data values per site (obs+fcst)
   //   fcstTime           = first forecast time
   //   maxTimeDiff        = maximum allowable time difference between obs
   //                        files
   //
   int                numDays;
   int                numFcstDays;
   int                maxHistory;
   int                numOutputSites;
   int                numDataValsPerSite;
   double             fcstTime;
   double             maxTimeDiff;

   //
   // Data arrays
   //   wspd      = wind speed
   //   T         = temperature
   //   pop       = probability of precip
   //   qpf       = qpf
   //   snow      = is it snowing (1=yes, 0=no)
   //   rain      = is it raining (1=yes, 0=no)
   //   ice       = is there freezing rain? (1=yes, 0=no)
   //   cpos      = conditional probability of snow
   //   cpor      = conditional probability of rain
   //   cpoi      = conditional probability of ice
   //   timeArray = time data
   //
   //   Note that the probabilities have different meanings
   //   in the obs part versus the forecast part
   //
   float             *wspd;
   float             *T;
   float             *pop;
   float             *qpf;
   float             *snow;
   float             *rain;
   float             *ice;
   float             *cpos;
   float             *cpor;
   float             *cpoi;
   double            *timeArray;
   
   //
   // List of obs file objects
   //
   vector< ObsFile* > obsFiles;

   //
   // Forecast file object
   //
   FcstFile           fcstFile;

   //
   // Read the obs config file
   //   obsConfigFile = obs configuration file name
   //   siteIdArray   = output site id array
   //
   //   Reads the config file and gets max history file names.
   //   Creates an ObsFile object for each of these file names
   //   and initializes it. Appends that object onto the list 
   //   of ObsFile objects.
   //
   //   Note that if the initialization fails, it will not add
   //   it to the list and will continue to the next file in the
   //   configuration file.  In this way, it will try to get
   //   max history files.  There may be large time gaps between
   //   them.  There is a check for this in the concatination
   //   steps.
   //
   int    readConfigFile( char* obsConfigFile, int* siteIdArray );

   //
   // Sets up the time array - 'concatinates' the time data
   //
   int    setTime();

   //
   // Get a variable
   //   varName = variable name
   //   numVals = number of values per site for this variable;
   //             output parameter
   //
   //   Performs the concatination for a given variable
   //
   float* getVar( const string& varName, int& numVals );

   //
   // Print routine for debugging
   //   dataArray = data to be printed
   //   start     = start index
   //   dataLen   = number of points to print from start index
   //   dataName  = name of data to be used in print statements
   //
   void printData( float* dataArray, int start, int dataLen, 
                   const string& dataName );

   //
   // Print precip data
   //   start = start index
   //   dataLen = number of points to print from start index
   //  
   //   Prints pop, T, cpos, snow, rain and ice for debugging
   //   purposes
   //
   void printPrecipData( int start, int dataLen );

   //
   // Print all the times in a nice format for debugging
   // purposes.
   //
   void printTimes();

};

#endif


   





