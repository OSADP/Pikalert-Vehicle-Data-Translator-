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
// ObsFile - container class for information relating
//           to an obs file
//
// $Id: ObsFile.hh,v 1.10 2004/09/08 02:52:13 jaimi Exp $
//
///////////////////////////////////////////////////////
#ifndef _OBS_FILE_INC_
#define _OBS_FILE_INC_

#include <string>

//
// Forward class declarations
//
class NcFile;

class ObsFile 
{
public:

   //
   // Constructor
   //
   ObsFile( double forcTime );

   //
   // Destructor
   //
   ~ObsFile();
   
   //
   // Init
   //   fileName     = name of obs file
   //   siteIdArray  = list of output site ids
   //   nOutputSites = number of output sites
   //
   //   This method initializes the object to prepare for
   //   further operations.  It gets all the general info
   //   that we will need from the file.  Sets up the
   //   data arrays and fills them, deriving the additional
   //   variables that we will need.
   //
   int    init( string* fileName, int* siteIdArray, int nOutputSites );

   //
   // Gets the number of values per day
   //   Note that this is not done per variable.  This is
   //   the number of values per day for all the variables
   //   that we will be using from this file.  So there
   //   is no point in getting this value with each variable
   //   that we need.
   //
   int    getNumTimesPerDay(){ return numTimesPerDay; }

   //
   // Gets the number of times used for each site
   //   Note that each obs file consists of one day's worth
   //   of data.  For some days, however, we will not use
   //   all the available times.  We do not want to grab
   //   any data past the forecast time.  Even if this
   //   data is missing in the file, we do not want to
   //   overwrite forecast data with missing data, so 
   //   just do not grab it in the first place.  If we
   //   are operating in archive mode, we will still have
   //   the problem of overwriting the forecast data with
   //   obs data that isn't really valid for the given run.
   //
   int    getNumTimesUsed(){ return numTimesUsed; }

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

private:

   //
   // File name
   //
   string *fname;

   //
   // File dimensions
   //   maxRecNum      = maximum number of records in file
   //   numObsSites    = number of obs sites in file
   //   numOutputSites = number of output sites
   //   numTimesPerDay = number of times per day for the
   //                    variables that we will be using
   //   numTimesUsed   = number of times that will be used
   //                    from this file 
   //                    (see note on getNumTimesUsed method above)
   //   forecastTime   = first forecast time; we do not want
   //                    to use any obs data after this time
   //
   int     maxRecNum;  
   int     numObsSites;
   int     numOutputSites;
   int     numTimesPerDay;
   int     numTimesUsed;
   double  forecastTime;

   //
   // NcFile object for given file
   //
   NcFile *ncFile;

   //
   // Array that contains the indeces of the output sites
   // in the same order as the siteIdArray above
   //
   long   *indexArray;

   //
   // Time data
   //
   double *timeArray;

   //
   // Variables retreived from file
   //   T    = temperature
   //   wspd = wind speed
   //   pop  = probability of precip; in obs this is just
   //          1 or 0 indicating whether precip was detected
   //          or not
   //   cpos = conditional probability of snow; in obs this is
   //          just 1 or 0 indicating whether the precip
   //          detected was snow or not
   //   cpor = conditional probability of rain; in obs this is
   //          just 1 or 0 indicating whether the precip 
   //          detected was rain or not
   //   cpoi = conditional probability of ice; in obs this is
   //          just 1 or 0 indicating whether the precip
   //          detected was ice or not
   //   qpf  = qpf
   //
   float  *T;
   float  *wspd;
   float  *pop;
   float  *cpos;
   float  *cpor;
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
   //   arrays of floating point values.
   //  
   //   Note also that for obs, these arrays are very similar
   //   to cpos, cpor and cpoi respectively.  However, there
   //   will be some quality control and additional missing
   //   values may be inserted.
   //   
   float  *snow;
   float  *rain;
   float  *ice;

   //
   // readObsInfo - read obs info from file
   //
   int     readObsInfo();

   //
   // getVar - get a variable from the file
   //   varName        = variable name
   //   numVarTimes    = number of times used for this variable;
   //                    output parameter
   //   varTimesPerDay = number of times per day for this
   //                    variable; output parameter
   //
   float*  getVar( const string& varName, long& numVarTimes, 
                   long& varTimesPerDay );

   //
   // setData - set the data associated with this file; i.e.
   //           get all the variables we will need from the file
   //           and derive the additional variables we will need
   //
   int     setData();
   
};

#endif




