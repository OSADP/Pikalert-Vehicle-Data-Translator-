// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:20:23 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
//////////////////////////////////////////////////////
// ObsFile - container class for information relating
//           to an obs file
//
// $Id: ObsFile.cc,v 1.19 2010/06/18 17:14:06 dicast Exp $
//
///////////////////////////////////////////////////////
#include <assert.h>
#include <string.h>
#include "ncf/ncf.hh"
#include "netcdf.hh"
#include "log/log.hh"
#include "nxncf/nxncf.hh"
#include "ObsFile.hh"
#include "ConcatData.hh"

extern Log *proc_log;

ObsFile::ObsFile( double forcTime ) 
{
   fname          = NULL;
   maxRecNum      = 0;
   numObsSites    = 0;
   numOutputSites = 0;
   numTimesPerDay = 0;
   numTimesUsed   = 0;
   forecastTime   = forcTime;
   ncFile         = NULL;
   indexArray     = NULL;
   timeArray      = NULL;
   T              = NULL;
   wspd           = NULL;
   pop            = NULL;
   cpos           = NULL;
   cpor           = NULL;
   cpoi           = NULL;
   snow           = NULL;
   rain           = NULL;
   ice            = NULL;
   qpf            = NULL;
}

ObsFile::~ObsFile() 
{
   delete fname;
   delete ncFile;
   delete [] indexArray;
   delete [] timeArray;
   delete [] T;
   delete [] wspd;
   delete [] cpos;
   delete [] cpor;
   delete [] cpoi;
   delete [] snow;
   delete [] rain;
   delete [] ice;
   delete [] qpf;
}

int
ObsFile::init( string* fileName, int* siteIdArray, int nOutputSites ) 
{
   assert( fileName );

   //
   // Save the file name
   //
   fname = fileName;

   //
   // Save the number of output sites
   //
   numOutputSites = nOutputSites;
   
   //
   // Open the file
   //
   ncFile = new NcFile( fname->c_str(), NcFile::ReadOnly );
   if ( !ncFile->is_valid() ) {
      proc_log->write_time_error( "Unable to open obs input file "
                                  "%s\n", fname->c_str() );
      return( 1 );
   }

   //
   // Get the basic info from the file
   //   
   if( readObsInfo() != 0 ) {
      return( 1 );
   }
   
   //
   // Set up the site index array
   //
   int nSitesProcessed = NXNCF_create_index_array( siteIdArray, 
                                                   numOutputSites, 
				                   ncFile, &indexArray );

   if( nSitesProcessed < 1 ) {
      proc_log->write_time_error( "Could not set up index array for sites\n" );
      return( 1 );
   }

   //
   // Get the time data
   //
   double *inputTimeArray = new double[numTimesPerDay];
   
   NcVar *varPtr = ncFile->get_var( "time_nominal" );
   if( !varPtr ) {
      proc_log->write_time_error( "Unable to get time data from %s\n",
                                  fname->c_str() );
      return( 1 );
   }

   if( varPtr->get( inputTimeArray, numTimesPerDay ) == 0 ) {
      proc_log->write_time_error( "Unable to get time data from %s\n",
                                  fname->c_str() );
      return( 1 );
   }

   //
   // Check the times
   //
   //for( int nt = numTimesPerDay - 1; nt >= 0; nt-- ) {
   //  if( inputTimeArray[nt] < forecastTime ) {
   //    numTimesUsed = nt + 1;
   //    break;
   //  }
   //}
   //
   // Modified to use the latest ob from the current hour (i.e. the forecastTime)
   //
   for( int nt = numTimesPerDay - 1; nt >= 0; nt-- ) {
     if( inputTimeArray[nt] <= forecastTime ) {
       numTimesUsed = nt + 1;
       break;
     }
   }
   
   if( numTimesUsed < 1 ) {
      proc_log->write_time( 1, "File %s is not in our time range\n",
                            fname->c_str() );
      delete[] inputTimeArray;
      return( 1 );
   }

   timeArray = new double[numTimesUsed];
   memcpy( (void*) timeArray, (void*) inputTimeArray, 
           sizeof( double ) * numTimesUsed );

   delete[] inputTimeArray;

   //
   // Set the data arrays
   //
   if( setData() != 0 )
      return( 1 );

   return( 0 );
   
}


float*
ObsFile::getData( const string& varName ) 
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


int
ObsFile::setData() 
{
   
   //
   // Initialize
   //
   long numTVals    = 0;
   long numPopVals  = 0;
   long numCporVals = 0;
   long numCposVals = 0;
   long numCpoiVals = 0;
   long numWspdVals = 0;
   long numQpfVals  = 0;

   long numTPerDay    = 0;
   long numPopPerDay  = 0;
   long numCporPerDay = 0;
   long numCposPerDay = 0;
   long numCpoiPerDay = 0;
   long numWspdPerDay = 0;
   long numQpfPerDay  = 0;

   //
   // Get all variables needed
   //   Note that since these are obs, pop really refers
   //   to a variable that tells us if precipitation was
   //   reported and the conditional probabilities really
   //   tell us what type of precip was reported, i.e.
   //   cpos tells us if snow was reported - we can still
   //   have more than one type...
   //
   T    = getVar( ConcatData::T_NAME, numTVals, numTPerDay );
   pop  = getVar( ConcatData::POP_NAME, numPopVals, numPopPerDay );
   cpor = getVar( ConcatData::CPOR_NAME, numCporVals, numCporPerDay );
   cpos = getVar( ConcatData::CPOS_NAME, numCposVals, numCposPerDay );
   cpoi = getVar( ConcatData::CPOI_NAME, numCpoiVals, numCpoiPerDay );
   wspd = getVar( ConcatData::WSPD_NAME, numWspdVals, numWspdPerDay );
   qpf  = getVar( ConcatData::QPF_NAME, numQpfVals, numQpfPerDay );
   
   //
   // Make sure we got all the times we need
   //
   //cout << "numTVals = " << numTVals << " numTimesUsed = " << numTimesUsed << " numTPerDay = " << numTPerDay << " numTimesPerDay = " << numTimesPerDay << endl;
   if( numPopVals != numTimesUsed || numPopPerDay != numTimesPerDay ||
       numCporVals != numTimesUsed || numCporPerDay != numTimesPerDay ||
       numCposVals != numTimesUsed || numCposPerDay != numTimesPerDay ||
       numCpoiVals != numTimesUsed || numCpoiPerDay != numTimesPerDay ||
       numWspdVals != numTimesUsed || numWspdPerDay != numTimesPerDay ||
       numQpfVals != numTimesUsed || numQpfPerDay != numTimesPerDay ||
       numTVals != numTimesUsed || numTPerDay != numTimesPerDay ) {
      proc_log->write_time_error( "Wrong number ov values for one or more "
                                  "of the obs variables\n" );
      return( 1 );
   }
   
   //
   // Set up arrays
   //
   int numVals = numTimesUsed * numOutputSites;
   
   snow = new float[numVals];
   rain = new float[numVals];
   ice  = new float[numVals];
   
   //
   // Do the precip calculation for each value in the
   // array
   //
   for( int i = 0; i < numVals; i++ ) {
      if( pop[i] == NC_FILL_FLOAT || 
          (pop[i] > 0 && cpos[i] == NC_FILL_FLOAT &&
           cpor[i] == NC_FILL_FLOAT && cpoi[i] == NC_FILL_FLOAT) ) {
         snow[i] = ConcatData::MISSING_PRECIP;
         rain[i] = ConcatData::MISSING_PRECIP;
         ice[i]  = ConcatData::MISSING_PRECIP;
         continue;
      }
      
      snow[i] = 0.0;
      rain[i] = 0.0;
      ice[i]  = 0.0;
      
      if( pop[i] > 0 ) {
         if( cpos[i] != NC_FILL_FLOAT && cpos[i] > 0 )
            snow[i] = 1.0;
         
         if( cpor[i] != NC_FILL_FLOAT && cpor[i] > 0 ) 
            rain[i] = 1.0;
         
         if( cpoi[i] != NC_FILL_FLOAT && cpoi[i] > 0 )
            ice[i] = 1.0;
      }
      
   }

   return( 0 );

}


float*
ObsFile::getVar( const string& varName, long& numVarTimes, 
                 long& varTimesPerDay ) 
{
   //
   // Set numVarTimes so that if we have to return
   // NULL on error, it is set to something
   //
   numVarTimes = 0;

   //
   // Get the variable from the file
   //
   NcVar *inputVarPtr = ncFile->get_var( varName.c_str() );
   if ( !inputVarPtr ) {
      proc_log->write_time_error( "Unable to get ptr for %s\n", 
                                  varName.c_str() );
      return( NULL );
   }

   //
   // Get the dimensions on that variable
   //
   long *dimArray = inputVarPtr->edges();

   //
   // Set the times per day
   //
   varTimesPerDay = dimArray[1];

   //
   // Get the number of dimensions
   //
   long numDims = inputVarPtr->num_dims();

   //
   // Determine raw data size
   //
   int rawDataSize = 1;
   for ( int i = 0; i < numDims; i++ )
      rawDataSize *= dimArray[i];

   //
   // Allocate space for the data
   //
   float *rawData = new float[rawDataSize];

   //
   // Get the raw data
   //
   if ( inputVarPtr->get( rawData, dimArray ) == 0 ) {
      proc_log->write_time_error( "Unable to get data for var %s from %s\n",
                                  inputVarPtr->name(), fname->c_str() );
      return( NULL );
   }

   proc_log->write_time_info( 1,"Successfully read %s\n", 
                              inputVarPtr->name() );

   //
   // Find out how many of these data points we can use -
   //  How many are in the time range we need
   //
   double varTimeInterval = (double) SEC_PER_DAY / (double) varTimesPerDay;

   int varTimesUsed = 0;
   //while( timeArray[0] + varTimesUsed * varTimeInterval < forecastTime &&
   //       varTimesUsed < varTimesPerDay ) {
   //   varTimesUsed++;
   //}
   //
   // Modified to use the latest ob from the current hour (i.e. the forecastTime)
   //
   while( timeArray[0] + varTimesUsed * varTimeInterval <= forecastTime &&
          varTimesUsed < varTimesPerDay ) {
      varTimesUsed++;
   }
   
   //
   // Set up the output array
   //
   int    dataSize = numOutputSites * varTimesUsed;
   float *data     = new float[dataSize];

   //
   // Save the data for the sites in which we are interested
   //
   for( int i = 0; i < numOutputSites; i++ ) {

      long currentIndex = indexArray[i];
      
      if( currentIndex >= 0 ) {
         for( int j = 0; j < numTimesUsed; j++ ) {
            data[i*varTimesUsed + j] = 
               rawData[currentIndex*varTimesPerDay + j];
         }
      }
      else {
         for( int j = 0; j < numTimesUsed; j++ ) {
            data[i*varTimesUsed + j] = NC_FILL_FLOAT;
         }
      }
   }

   numVarTimes = varTimesUsed;

   delete [] rawData;
   delete [] dimArray;

   return( data );
   
}

int 
ObsFile::readObsInfo()
{
  //
  // Get the actual number of sites
  //
  NcVar *inputVarPtr = ncFile->get_var( NUM_SITES_NAME );
  if ( !inputVarPtr ) {
     proc_log->write_time_error( "Unable to get ptr for %s\n",
                                 NUM_SITES_NAME );
     return( 1 );
  }

  if( inputVarPtr->get( &numObsSites, 1 ) == 0 ) {
     proc_log->write_time_error( "Unable to get data for var %s\n",
                                 inputVarPtr->name() );
     return( 1 );
  }

  //
  // Get the dimensions
  //
  //    Times per day
  //
  NcDim *numTimesDim = ncFile->get_dim( "times_per_day" );

  numTimesPerDay = numTimesDim->size();

  //
  //    Maximum number of records
  //
  NcDim *recNumDim = ncFile->get_dim( "recNum" );

  maxRecNum = recNumDim->size();

  return( 0 );
}













