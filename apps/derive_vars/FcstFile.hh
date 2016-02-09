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
//           to a forecast file
//
// $Id: FcstFile.hh,v 1.9 2004/09/08 02:52:13 jaimi Exp $
//
///////////////////////////////////////////////////////
#ifndef _FCST_FILE_INC_
#define _FCST_FILE_INC_

#include <string>

//
// Forward class declarations
//
class NcFile;

class FcstFile 
{
public:

   //
   // Constructor
   //
   FcstFile();

   //
   // Destructor
   //
   ~FcstFile();
   
   //
   // Init
   //   fileName     = name of forecast file
   //   siteIdArray  = list of output site ids
   //   nOutputSites = number of output sites
   //   fcstTime     = first forecast time
   //   nOutputDays  = number of output forecast days
   //
   //   This method initializes the object to prepare for
   //   further operations.  It gets all the general info
   //   that we will need from the file.  Sets up the
   //   data arrays and fills them, deriving the additional
   //   variables that we will need.
   //
   int    init( char* fileName, int* siteIdArray, int nOutputSites,
                double fcstTime, int nOutputDays );

   //
   // Gets the number of values per day
   //   Note that this is not done per variable.  This is
   //   the number of values per day for all the variables
   //   that we will be using from this file.  So there
   //   is no point in getting this value with each variable
   //   that we need.
   //
   int    getNumValsPerDay(){ return numTimesPerDay; }

   //
   // Gets the number of values per site
   //   Note that this is not done per variable.  This is
   //   the number of values per site for all the variables
   //   that we will be using from this file.  So there
   //   is no point in getting this value with each variable
   //   that we need.
   //
   int    getNumValsPerSite(){ return numTimes; }

   //
   // Gets the data for the given variable
   //   varName = variable name
   //
   // Note:  FcstFile object will allocate and free memory associated
   //        with these arrays
   //
   float* getData( const string& varName );

   //
   // Get the time data
   //
   // Note:  FcstFile object will allocate and free memory associated with
   //        time array
   //
   double* getTime(){ return timeArray; }

   //
   // Constants
   //  PROB_THRESH    = threshold for probability of precip;
   //                   values below this threshold indicate no
   //                   precip
   //  QPF_THRESH     = threshold for qpf;
   //                   values below this threshold indicate no 
   //                   precip
   //  ICE_THRESH     = threshold for conditional probability of ice;
   //                   values above this threshold indicate ice
   //  MIN_CONFIDENCE = minimum confidence;
   //                   only data values that have associated
   //                   confidences above this threshold will
   //                   be used
   //
   static const float  PROB_THRESH;
   static const float  QPF_THRESH;
   static const float  ICE_THRESH;
   static const float  MIN_CONFIDENCE;

private:

   //
   // File name
   //
   string  fname;

   //
   // Dimensions
   //   numTimes       = number of times per site that will be used;
   //                    note that we will sometimes skip over the 
   //                    'hindcasts' so these times will not be included
   //                    in the numTimes
   //   fcstIndex      = index of the first forecast that we will use
   //   numInputSites  = number of sites in file
   //   numOutputSites = number of sites for which we will provide
   //                    the blowing snow potential
   //   numOutputDays  = number of forecast days for which we will
   //                    provide the blowing snow potential
   //   numTimesPerDay = the number of times per day;
   //                    this should be the same for all the variables
   //                    we are interested in, but we will check to
   //                    make sure this is true
   //   numInputDays   = the number of forecast days in the file
   //   maxSiteNum     = the maximum number of sites in the file
   //   forecastTime   = the first forecast time we will use
   //   
   int     numTimes;
   int     fcstIndex;
   int     numInputSites;
   int     numOutputSites;
   int     numOutputDays;
   long    numTimesPerDay;
   long    numInputDays;
   long    maxSiteNum;
   double  forecastTime;

   //
   // The data for each variable that we read will be put into
   // this array to be processed.  We only reallocate memory for
   // this as necessary.
   //   rawDataAlloced = the size of memory allocated to the rawData 
   //                    array
   //   rawData        = the raw data array
   //
   int     rawDataAlloced;
   float  *rawData;

   //
   // NcFile object for the current file
   //
   NcFile *ncFile;

   //
   // Time data
   //
   double *timeArray;

   //
   // Variables that we will read in from file
   //   T = temperature
   //   wspd = wind speed
   //   pop  = probability of precip
   //   cpor = conditional probability of rain
   //   cpos = conditional probability of snow
   //   cpoi = conditional probability of ice
   //   qpf  = qpf
   //
   float  *T;
   float  *wspd;
   float  *pop;
   float  *cpor;
   float  *cpos;
   float  *cpoi;
   float  *qpf;

   //
   // Variables that we will derive
   //   snow = is it snowing?
   //   rain = is it raining?
   //   ice  = is there freezing rain?
   //
   //   For each of these variables 1.0 indicates yes and 0.0
   //   indicates no.  
   //
   //   Note that these should really be boolean variables, but
   //   in order to be able to grab these data and treat them the
   //   same way as the other variables, we had to make them
   //   arrays of floating point values
   //
   float  *snow;
   float  *rain;
   float  *ice;

   //
   // readFcstInfo - read forecast info from file
   //   forcTime = first forecast time we will use
   //
   //   This method reads in all the general information
   //   from the current file
   //
   int readFcstInfo( double& forcTime );

   //
   // getVar - get a variable from the file
   //   varName = variable name
   //   numValsPerSite = number of values per site; output parameter -
   //                    this function sets this value for future use
   //   varTimesPerDay = number of times per day for this variable;
   //                    output parameter - this function sets this 
   //                    value for future use
   //
   float* getVar( const string& varName, long& numValsPerSite, 
                  long& varTimesPerDay );

   //
   // setData - set the data associated with this file; i.e.
   //           get all the variables we will need from the file
   //           and derive the additional variables we will need
   //
   int setData();
   
};

#endif
