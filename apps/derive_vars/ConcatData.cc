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
// $Id: ConcatData.cc,v 1.32 2009/07/08 17:58:19 dicast Exp $
//
///////////////////////////////////////////////////////
#include <string.h>

#include "log/log.hh"
#include "nxncf/nxncf.hh"

#include "ConcatData.hh"
#include "ObsFile.hh"

extern Log *proc_log;

//
// Constants
//
const int    ConcatData::FILE_NAME_LEN    = 120;
const string ConcatData::WSPD_NAME        = "wind_speed";
const string ConcatData::T_NAME           = "T";
const string ConcatData::POP_NAME         = "prob_precip01";
const string ConcatData::QPF_NAME         = "qpf01";
const string ConcatData::CPOS_NAME        = "cprob_snow";
const string ConcatData::CPOR_NAME        = "cprob_rain";
const string ConcatData::CPOI_NAME        = "cprob_ice";
const string ConcatData::SNOW_NAME        = "snow";
const string ConcatData::RAIN_NAME        = "rain";
const string ConcatData::ICE_NAME         = "ice";
const float  ConcatData::MISSING_PRECIP   = -2.0;

ConcatData::ConcatData( long maxHist, double forecastTime, int dl ) 
{  
   debugLevel         = dl;
   numDays            = 0;
   numFcstDays        = 0;
   maxHistory         = (int) maxHist;
   numOutputSites     = 0;
   fcstTime           = forecastTime;
   maxTimeDiff        = 0;
   numDataValsPerSite = 0;
   wspd               = NULL;
   T                  = NULL;
   pop                = NULL;
   qpf                = NULL;
   snow               = NULL;
   rain               = NULL;
   ice                = NULL;
   cpos               = NULL;
   cpor               = NULL;
   cpoi               = NULL;
   timeArray          = NULL;
}

ConcatData::~ConcatData() 
{
   delete[] wspd;
   delete[] T;
   delete[] pop;
   delete[] qpf;
   delete[] snow;
   delete[] rain;
   delete[] ice;
   delete[] cpos;
   delete[] cpor;
   delete[] cpoi;
   delete[] timeArray;
   
   vector< ObsFile* >::iterator obsIt;
   for( obsIt = obsFiles.begin(); obsIt != obsFiles.end(); obsIt++ ) {
      delete (*obsIt);
   }
}

int
ConcatData::init( char* obsConfigFileName, char* forecastFileName,
                  int* outputSiteIdArray, int nOutputSites,
                  int nFcstDays ) 
{
   //
   // Check that we have a config file for the obs
   //
   if( !obsConfigFileName ) {
      proc_log->write_time_error( "No obs config file\n" );
      return( 1 );
   }

   //
   // Set variables
   //
   numOutputSites = nOutputSites;
   numFcstDays    = nFcstDays;
   
   //
   // Set up the forecast file object
   //
   if( fcstFile.init( forecastFileName, outputSiteIdArray, 
                      numOutputSites, fcstTime, numFcstDays ) != 0 ) {
      return( 1 );
   }
   
   //
   // Set up the obs file objects using the config file
   //
   if( readConfigFile( obsConfigFileName, outputSiteIdArray ) != 0 ) {
      return( 1 );
   }

   numDays = numFcstDays + (int) obsFiles.size();

   //
   // Set up data arrays
   //  Note that we do not have a seperate value for numDataValsPerSite
   //  for each variable - these are already garanteed to be the same,
   //  but getVar is the best place to compute this...
   //
   wspd = getVar( WSPD_NAME, numDataValsPerSite );
   T    = getVar( T_NAME, numDataValsPerSite );
   pop  = getVar( POP_NAME, numDataValsPerSite );
   cpos = getVar( CPOS_NAME, numDataValsPerSite );
   cpor = getVar( CPOR_NAME, numDataValsPerSite );
   cpoi = getVar( CPOI_NAME, numDataValsPerSite );
   snow = getVar( SNOW_NAME, numDataValsPerSite );
   rain = getVar( RAIN_NAME, numDataValsPerSite );
   ice  = getVar( ICE_NAME, numDataValsPerSite );
   qpf  = getVar( QPF_NAME, numDataValsPerSite );

   //
   // Set time array
   //
   int numTimes = setTime();

   if( numTimes != numDataValsPerSite ) {
      proc_log->write_time_error( "Number of times not what is expected\n" );
      return( 1 );
   }

   return( 0 );
}

int
ConcatData::readConfigFile( char* obsConfigFile, int* siteIdArray ) 
{
   
   //
   // Open the config file in read mode
   //
   FILE *fp = fopen( obsConfigFile, "r" );

   if ( fp == NULL ) {
      proc_log->write_time_error( "Could not open %s\n", obsConfigFile );
      return( 1 );
   }

   //
   // Read the first maxHistory lines from the config file
   // into the inputFile array.
   //   Note:  If we find a line that is less than 2 characters
   //   long, we assume we are done reading the file.
   //
   int i = 0;
   char tmpStr[FILE_NAME_LEN];
   while ( i < maxHistory &&
           NULL != fgets( tmpStr, FILE_NAME_LEN, fp ) && 
           strlen( tmpStr ) > 2 ) {

      //
      // Take care of null termination
      //
      if( tmpStr[strlen( tmpStr ) - 1] == '\n' )
         tmpStr[ strlen( tmpStr ) - 1] = '\0';
      
      //
      // Get the file name
      //   Note that the memory associated with this string
      //   will be owned by the ObsFile object that it gets
      //   passed into
      //
      string *newString = new string( tmpStr );

      //
      // Set up the obs file object
      //
      ObsFile *newObsFile = new ObsFile( fcstTime );
      
      if( newObsFile->init( newString, siteIdArray, numOutputSites ) != 0 ) {
         proc_log->write_time( 1, "Unable to use obs file %s\n", 
                               newString->c_str() );
      }
      else {
      
         //
         // Put it in the list
         //
         obsFiles.push_back( newObsFile );

	 //
	 // Only count this file toward the max history if
	 // we can use it
	 //
         i++;
      }
   }

   //
   // Close the file
   //
   fclose( fp );

   return( 0 );
}

float*
ConcatData::getVar( const string& varName, int& numVals ) 
{
   //
   // Initialize numVals so that if we return with an
   // error, it makes sense
   //
   numVals = 0;
   
   //
   // Make sure we have obs data
   //
   if( obsFiles.size() < 1 ) {
     proc_log->write_time_error( "No obs files\n" );
     return( NULL );
   }

   //
   // Get forecast data
   //
   long   fcstValsPerSite = fcstFile.getNumValsPerSite();
   long   fcstTimesPerDay = fcstFile.getNumValsPerDay();
   float *fcstData        = fcstFile.getData( varName );

   if( !fcstData ) {
      return( NULL );
   }

   //
   // Get obs data and sizes
   //   Put the obs data in these structures in reverse order
   //   so that we have the oldest file first.  Note that we
   //   assume that this value will be the same over all obs
   //   files
   //
   int obsTimesPerDay = obsFiles[0]->getNumTimesPerDay();

   vector< long >   obsSize;
   vector< float* > obsData;
   
   int numObs = 0;
   
   for( int no = (int) obsFiles.size() - 1; no >= 0; no-- ) {
      long   currentObsSize = obsFiles[no]->getNumTimesUsed();
      float *currentObsData = obsFiles[no]->getData( varName );

      if( !currentObsData ) {
         return( NULL );
      }
      
      obsSize.push_back( currentObsSize );
      obsData.push_back( currentObsData );

      numObs += currentObsSize;
   }

   //
   // Make sure the number of observations per day match
   // the number of forecasts per day
   //
   if( fcstTimesPerDay != obsTimesPerDay ) {
      proc_log->write_time_error( "Number of times per day do not "
                                  "match between obs data and forecast "
                                  "data\n" );
      return( NULL );
   }
   
   //
   // Set up array
   //
   int numValsPerSite = numObs + fcstValsPerSite;
   
   float *dataArray = new float[numValsPerSite*numOutputSites];

   for( int i = 0; i < numValsPerSite*numOutputSites; i++ ) {
      dataArray[i] = NC_FILL_FLOAT;
   }

   //
   // Copy obs data in 
   //
   int obsIndex = 0;
   
   for( int no = 0; no < (int) obsData.size(); no++ ) {

      float *obsArray = obsData[no];

      //
      // For each site, calculate the indeces and copy
      // the data for the current file in
      //
      for( int ns = 0; ns < numOutputSites; ns++ ) {
	 int outputIndex = ns * numValsPerSite + obsIndex;
         int inputIndex  = ns * obsSize[no];
      
         for( int nt = 0; nt < obsSize[no]; nt++ ) {
	    dataArray[outputIndex] = obsArray[inputIndex];
	    outputIndex++;
	    inputIndex++;
	 }
      }

      obsIndex += obsSize[no];
   }
   

   //
   // Copy forecast data in
   //
   for( int ns = 0; ns < numOutputSites; ns++ ) {
      int outputIndex = ns * numValsPerSite + numObs;
      int inputIndex  = ns * fcstValsPerSite;
      
      for( int nv = 0; nv < fcstValsPerSite; nv++ ) {
	dataArray[outputIndex] = fcstData[inputIndex];
	outputIndex++;
	inputIndex++;
      }
   }

   //
   // Clean up
   //
   obsData.erase( obsData.begin(), obsData.end() );
   obsSize.erase( obsSize.begin(), obsSize.end() );

   numVals = numValsPerSite;

   return( dataArray );
}


int
ConcatData::setTime() 
{
   //
   // Get forecast times
   //
   int     numFcstTimes = fcstFile.getNumValsPerSite();
   double *fcstTimes    = fcstFile.getTime();

   //
   // Get obs data and sizes
   //   Put the data in these structures in reverse order
   //   so that we have the oldest data first
   //
   vector< int >     obsSize;
   vector< double* > obsTime;
   
   int numObs = 0;
   
   for( int no = (int) obsFiles.size() - 1; no >= 0; no-- ) {
      int     currentObsSize = obsFiles[no]->getNumTimesUsed();
      double *currentObsTime = obsFiles[no]->getTime();

      obsSize.push_back( currentObsSize );
      obsTime.push_back( currentObsTime );

      numObs += currentObsSize;
   }

   //
   // Set up array
   //
   int numVals = numObs + numFcstTimes;

   timeArray = new double[numVals];

   //
   // Copy obs data in 
   //
   int    outputIndex = 0;
   int    diffCount   = 0;
   double diffSum     = 0.0;
   
   for( int no = 0; no < (int) obsTime.size(); no++ ) {

      double *obsArray = obsTime[no];

      for( int i = 0; i < obsSize[no]; i++ ) {
         timeArray[outputIndex] = obsArray[i];
         
         if( outputIndex > 0 ) {
            diffSum += ( timeArray[outputIndex] - timeArray[outputIndex-1] );
            diffCount++;
         }
         
         outputIndex++;
      }
   }

   //
   // Calculate a maximum time difference based on the "normal"
   // time gap
   //
   maxTimeDiff = 2.0 * ( diffSum / diffCount );

   //
   // Make sure we don't have a big time gap between the last obs
   // file and the forecast file
   //
   if( timeArray[outputIndex-1] == NC_FILL_DOUBLE ||
       fcstTimes[0] - timeArray[outputIndex-1] > maxTimeDiff ) {
      proc_log->write_time_error( "Large time gap in data\n" );
      return( 0 );
   }

   for( int nt = 0; nt < numFcstTimes; nt++ ) {
      timeArray[outputIndex] = fcstTimes[nt];
      outputIndex++;
   }

   //
   // Clean up
   //
   obsTime.erase( obsTime.begin(), obsTime.end() );
   obsSize.erase( obsSize.begin(), obsSize.end() );

   return( numVals );
}

void
ConcatData::printData( float* dataArray, int start, int dataLen, 
                       const string& dataName ) 
{
   for( int i = start; i < start+dataLen; i++ ) {
     if( dataArray[i] != NC_FILL_FLOAT )
       fprintf( stderr, "%s[%d] = %f\n", dataName.c_str(), i, dataArray[i] );
     else
       fprintf( stderr, "%s[%d] = missing\n", dataName.c_str(), i );
   }
}

void
ConcatData::printPrecipData( int start, int dataLen ) 
{
   for( int i = start; i < start+dataLen; i++ ) {
     if( pop[i] != NC_FILL_FLOAT )
       fprintf( stderr, "pop[%d] = %f, ", i, pop[i] );
     else
       fprintf( stderr, "pop[%d] = missing, ", i );

     if( T[i] != NC_FILL_FLOAT )
       fprintf( stderr, "T[%d] = %f, ", i, T[i] );
     else
       fprintf( stderr, "T[%d] = missing, ", i );

     if( cpos[i] != NC_FILL_FLOAT )
       fprintf( stderr, "cpos[%d] = %f, ", i, cpos[i] );
     else
       fprintf( stderr, "cpos[%d] = missing, ", i );

     if( snow[i] != MISSING_PRECIP )
        fprintf( stderr, "snow[%d] = %f, ", i, snow[i] );
     else
        fprintf( stderr, "snow[%d] = missing, ", i );
     
     if( rain[i] != MISSING_PRECIP )
        fprintf( stderr, "rain[%d] = %f, ", i, rain[i] );
     else
        fprintf( stderr, "rain[%d] = missing, ", i );
     
     if( ice[i] != MISSING_PRECIP ) 
        fprintf( stderr, "ice[%d] = %f\n", i, ice[i] );
     else
        fprintf( stderr, "ice[%d] = missing\n", i );
   }
}

void
ConcatData::printTimes() 
{
   for( int i = 0; i < numDataValsPerSite; i++ ) {

     time_t     currentTime = (time_t) timeArray[i];
     struct tm *tmStruct    = gmtime( &currentTime );

     fprintf( stderr, "day = %.2d, hour = %.2d\n", 
                     tmStruct->tm_mday, tmStruct->tm_hour );
   }
}




