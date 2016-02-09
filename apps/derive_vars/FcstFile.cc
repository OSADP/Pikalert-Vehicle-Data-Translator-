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
// FcstFile - container class for information relating
//            to a forecast file
//
// $Id: FcstFile.cc,v 1.14 2007/10/01 20:33:37 cowie Exp $
//
///////////////////////////////////////////////////////
#include <string>
#include <math.h>

#include "netcdf.hh"
#include "log/log.hh"
#include "nxncf/nxncf.hh"
#include "ncf/ncf.hh"
#include "FcstFile.hh"
#include "ConcatData.hh"

extern Log *proc_log;

//
// Constants
//
const float  FcstFile::PROB_THRESH      = 0.25;
const float  FcstFile::QPF_THRESH       = 0.10;
const float  FcstFile::ICE_THRESH       = 0.25;
const float  FcstFile::MIN_CONFIDENCE   = -0.00001;

FcstFile::FcstFile() 
{
   fname          = "";
   numTimes       = 0;
   fcstIndex      = 0;
   numInputSites  = 0;
   numOutputSites = 0;
   numOutputDays  = 0;
   numTimesPerDay = 0;
   numInputDays   = 0;
   maxSiteNum     = 0;
   forecastTime   = 0.0;
   rawDataAlloced = 0;
   rawData        = NULL;
   ncFile         = NULL;
   timeArray      = NULL;
   T              = NULL;
   wspd           = NULL;
   pop            = NULL;
   cpor           = NULL;
   cpos           = NULL;
   cpoi           = NULL;
   qpf            = NULL;
   snow           = NULL;
   rain           = NULL;
   ice            = NULL;
}

FcstFile::~FcstFile() 
{
   delete [] rawData;
   delete ncFile;
   delete [] timeArray;
   delete [] T;
   delete [] wspd;
   delete [] pop;
   delete [] cpor;
   delete [] cpos;
   delete [] cpoi;
   delete [] qpf;
   delete [] snow;
   delete [] rain;
   delete [] ice;
}

int
FcstFile::init( char* fileName, int* siteIdArray, int nOutputSites,
                double fcstTime, int nOutputDays ) 
{
   //
   // Save some variables
   //
   fname          = fileName;
   forecastTime   = fcstTime;
   numOutputDays  = nOutputDays;
   numOutputSites = nOutputSites;
   
   //
   // Open the file
   //
   ncFile = new NcFile( fname.c_str(), NcFile::ReadOnly );
   if ( !ncFile->is_valid() ) {
      proc_log->write_time_error( "Unable to open obs input file "
                                  "%s\n", fname.c_str() );
      return( 1 );
   }

   //
   // Get the basic info from the file
   //   
   double fileFcstTime = 0.0;
   if( readFcstInfo( fileFcstTime ) != 0 ) {
      return( 1 );
   }

   //
   // Check variable values
   //
   if( numOutputDays > numInputDays ) {
     numOutputDays = numInputDays;
   }

   if( numOutputSites > maxSiteNum ) {
     proc_log->write_time_error( "Too many output sites expected\n" );
     return( 1 );
   }

   //
   // Set up index to start from for the desired forecast time
   //
   double timeInterval = (double) SEC_PER_DAY / (double) numTimesPerDay;
   
   fcstIndex = (int) floor( ( forecastTime - fileFcstTime ) / timeInterval );
   
   //
   // Set up time array
   //
   numTimes = numOutputDays * numTimesPerDay - fcstIndex;
   timeArray = new double[numTimes];
   
   for( int i = 0; i < numTimes; i++ ) {
      timeArray[i] = forecastTime + timeInterval * i;
   }

   //
   // Set the data arrays
   //
   if( setData() != 0 ) 
      return( 1 );

   return( 0 );
   
}

int 
FcstFile::readFcstInfo( double& forcTime )
{

  NcVar *inputVarPtr = ncFile->get_var( NUM_SITES_NAME );
  if ( !inputVarPtr ) {
      proc_log->write_time_error( "Unable to get ptr for %s\n", 
                                  NUM_SITES_NAME );
      return( 1 );
  }

  if( inputVarPtr->get( &numInputSites, 1 ) == 0 ) {
      proc_log->write_time_error( "Unable to get data for var %s\n",
		                  inputVarPtr->name() );
      return( 1 );
  }

  inputVarPtr = ncFile->get_var( FORC_TIME_NAME );
  if ( !inputVarPtr ) {
      proc_log->write_time_error( "Unable to get ptr for %s\n", 
                                  FORC_TIME_NAME );
      return( 1 );
  }
    
  forcTime = inputVarPtr->as_double(0);

  NcDim *numInputTimesDim = ncFile->get_dim( "fc_times_per_day" );
  numTimesPerDay = numInputTimesDim->size();

  NcDim *numInputDaysDim = ncFile->get_dim( "days" );
  numInputDays = numInputDaysDim->size();

  NcDim *maxSiteDim = ncFile->get_dim( "max_site_num" );
  maxSiteNum = maxSiteDim->size();

  return( 0 );
}

int
FcstFile::setData()
{
   //
   // Initialize
   //
   long numTVals    = 0;
   long numQpfVals  = 0;
   long numPopVals  = 0;
   long numCporVals = 0;
   long numCposVals = 0;
   long numCpoiVals = 0;
   long numWspdVals = 0;

   long numTPerDay    = 0;
   long numQpfPerDay  = 0;
   long numPopPerDay  = 0;
   long numCporPerDay = 0;
   long numCposPerDay = 0;
   long numCpoiPerDay = 0;
   long numWspdPerDay = 0;

   //
   // Get all variables needed for precip calculations
   //
   T    = getVar( ConcatData::T_NAME, numTVals, numTPerDay );
   qpf  = getVar( ConcatData::QPF_NAME, numQpfVals, numQpfPerDay );
   pop  = getVar( ConcatData::POP_NAME, numPopVals, numPopPerDay );
   cpor = getVar( ConcatData::CPOR_NAME, numCporVals, numCporPerDay );
   cpos = getVar( ConcatData::CPOS_NAME, numCposVals, numCposPerDay );
   cpoi = getVar( ConcatData::CPOI_NAME, numCpoiVals, numCpoiPerDay );
   wspd = getVar( ConcatData::WSPD_NAME, numWspdVals, numWspdPerDay );
   
   //
   // Make sure we got all the times we need
   //
   if( numQpfVals != numTimes || numQpfPerDay != numTimesPerDay ||
       numPopVals != numTimes || numPopPerDay != numTimesPerDay ||
       numCporVals != numTimes || numCporPerDay != numTimesPerDay ||
       numCposVals != numTimes || numCposPerDay != numTimesPerDay ||
       numCpoiVals != numTimes || numCpoiPerDay != numTimesPerDay ||
       numTVals != numTimes || numTPerDay != numTimesPerDay ) {
      proc_log->write_time_error( "Wrong number of values for one or more "
                                  "of the forecast variables\n" );
      return( 1 );
   }
   
   //
   // Set up arrays
   //
   int numVals = numTimes * numOutputSites;
   
   snow = new float[numVals];
   rain = new float[numVals];
   ice  = new float[numVals];

   //
   // Do the precip calculations for each value in the
   // array
   //
   for( int i = 0; i < numVals; i++ ) {

      //
      // Initalize
      //
      snow[i] = 0.0;
      rain[i] = 0.0;
      ice[i]  = 0.0;

      //
      // Missing data
      //
      if( pop[i] == NC_FILL_FLOAT || qpf[i] == NC_FILL_FLOAT ) {
         snow[i] = ConcatData::MISSING_PRECIP;
         rain[i] = ConcatData::MISSING_PRECIP;
         ice[i]  = ConcatData::MISSING_PRECIP;
         continue;
      }

      //
      // No precip
      //
      if( pop[i] < PROB_THRESH || qpf[i] < QPF_THRESH ) {
         continue;
      }

      //
      // If we have the conditional probabilities, use them
      // to calculate the precip type
      //
      if( cpor[i] != NC_FILL_FLOAT && cpos[i] != NC_FILL_FLOAT && 
          cpoi[i] != NC_FILL_FLOAT ) {

         if( cpoi[i] > ICE_THRESH ) {
            ice[i] = 1.0;
         }

         else if( cpos[i] > cpor[i] ) {
            snow[i] = 1.0;
         }
         
         else {
            rain[i] = 1.0;
         }
         
      }
   
      //
      // If we don't have the conditional probabilities, use
      // temperature to calculate the precip type
      //
      if( T[i] != NC_FILL_FLOAT && T[i] > 0.0 ) {
         rain[i] = 1.0;
      }
      
      else if( T[i] != NC_FILL_FLOAT && T[i] <= 0.0 ) {
         snow[i] = 1.0;
      }
   }

   return( 0 );
   
}

float*
FcstFile::getData( const string& varName ) 
{

   if( varName == ConcatData::T_NAME ) 
      return( T );
   
   if( varName == ConcatData::WSPD_NAME )
      return( wspd );
   
   if( varName == ConcatData::POP_NAME )
      return( pop );
   
   if( varName == ConcatData::CPOR_NAME ) 
      return( cpor );
      
   if( varName == ConcatData::CPOS_NAME )
      return( cpos );
   
   if( varName == ConcatData::CPOI_NAME )
      return( cpoi );
   
   if( varName == ConcatData::SNOW_NAME )
      return( snow );
   
   if( varName == ConcatData::RAIN_NAME )
      return( rain );
   
   if( varName == ConcatData::ICE_NAME )
      return( ice );

   if( varName == ConcatData::QPF_NAME )
     return( qpf );
   
   return( NULL );
}

float*
FcstFile::getVar( const string& varName, long& numValsPerSite, 
                  long& varTimesPerDay ) 
{
   //
   // Initialize
   //
   numValsPerSite = 0;
   varTimesPerDay = 0;

   //
   // Get the variable
   //
   NcVar *inputVarPtr = ncFile->get_var( varName.c_str() );
   if ( !inputVarPtr ) {
     proc_log->write_time_error( "Unable to get ptr for %s\n",
                                 varName.c_str() );
     return( NULL );
   }

   //
   // Determine raw data size
   //
   long *inDimArray = inputVarPtr->edges();
   long  numDims    = inputVarPtr->num_dims();

   int rawDataSize = 1;
   for( int i = 0; i < numDims; i++ )
     rawDataSize *= inDimArray[i];

   //
   // Allocate space for raw data if necessary
   //
   if( rawDataAlloced != rawDataSize ) {
     delete [] rawData;

     rawData        = new float[rawDataSize];
     rawDataAlloced = rawDataSize;
   }

   //
   // Get the raw data
   //
   if( inputVarPtr->get( rawData, inputVarPtr->edges() ) == 0 ) {
     proc_log->write_time_error( "Unable to get raw data for var %s\n",
		                 inputVarPtr->name() );
     return( NULL );
   }
   else {
     proc_log->write_time_info( 1,"Successfully read %s\n",
		                inputVarPtr->name() );
   }

   //
   // Set the number of values per site and the number of times
   // per day for this variable
   //
   varTimesPerDay = inDimArray[2];
   numValsPerSite = numOutputDays * varTimesPerDay - fcstIndex;

   //
   // Allocate space for data - 
   //   Note that the caller will be responsible for freeing this
   //   memory
   //
   int    dataSize = numOutputSites * numValsPerSite;
   float *data     = new float[dataSize];

   //
   // If input file is in post_proc format, fill in the data array
   //
   if( numDims < IND_FM_NUM_VAR_DIM ) {
     for( int i = 0; i < numOutputSites; i++ ) {
       for( int j = 0; j < numValsPerSite; j++ ) {
	 data[i*numValsPerSite + j] =
	   rawData[i*numInputDays*varTimesPerDay + j + fcstIndex];
       }
     }

     return( data );
   }

   //
   // Input file is in FM format
   //
   int numParams      = inDimArray[IND_FM_PVALS_DIM];
   int numSites       = inDimArray[IND_FM_SITE_DIM];
   int numDays        = inDimArray[IND_FM_DAYS_DIM];
   int localNumTimes  = inDimArray[IND_FM_TIME_DIM];

   long inputNumValsPerSite = numDays * localNumTimes;

   for( int ns = 0; ns < min( numSites, numOutputSites ); ns++ ) {
     for( int nv = 0; nv < min( inputNumValsPerSite, numValsPerSite ); nv++ ) {

	   int rawIndex = ns * inputNumValsPerSite * numParams +
	     (nv + fcstIndex) * numParams + IND_FM_MEAN;
	   int rawIndexConf = rawIndex - IND_FM_MEAN + IND_FM_MEAN_CONF;

	   int index = ns * numValsPerSite + nv;

	   if( rawData[rawIndexConf] > MIN_CONFIDENCE )
	     data[index] = rawData[rawIndex];
     }
   }

   return( data );

}

