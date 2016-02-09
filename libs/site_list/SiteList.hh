// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:34:34 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
///////////////////////////////////////////////////
// SiteList:  Container class for a site list 
// and associated information
//
// $Id: SiteList.hh,v 1.19 2014/03/06 04:38:33 jaimi Exp $
////////////////////////////////////////////////////
#ifndef SITE_LIST_INC
#define SITE_LIST_INC

#include <string>
#include <map>
using namespace std;

//
// Forward class declarations
//
class SiteLoc;

class SiteList {

 public:

   SiteList();   
  ~SiteList();

   //
   // Set up the site list object
   //   siteListFile = name of text based site list file
   //   nbhdFile     = name of netcdf file with a list of neighbors
   //                  and distances for each site
   //   maxNbrDist   = maximum distance from a given site for
   //                  another site to be considered a neighbor
   //   maxNumNbr    = maximum number of neighbors for a given
   //                  site - takes the first maxNumNbr neighbors
   //                  within maxNbrDist
   //
   //   Note that it is not necessary to have neighbor information
   //   to use this class
   //  
   //   Returns 0 on success, 1 on error
   //
   int    init( const char* siteListFile, const char* nbhdFile = NULL,
                float maxNbrDist = 0.0, int maxNumNbr = 0 );

   //
   // Return the number if sites in the list
   //
   long   getNumSites() const { return numSites; }

   //
   // Return the list of site ids
   //
   int*   getIdArray() { return idArray; }

   //
   // Returns the error string
   //
   const char *error() { return errString.c_str(); }

   //
   // Get the indeces of the neighbors for a given site index.
   // Includes the index of the given site in the list.
   //    siteIndex = index of given site
   //    indeces   = list of indeces for the neighbors of the
   //                given site.  The memory for this array
   //                should be allocated and freed by caller.
   //                The values are filled in by this routine
   //    arraySize = size of the indeces array allocated by
   //                caller
   //  
   //    returns 0 on success, 1 on failure
   //
   int    getIndeces( int siteIndex, int* indeces, int arraySize );

   //
   // Get the ids of the neighbors for a given site id.
   // Includes the id of the given site in the list.
   //    siteId    = id of given site
   //    indeces   = list of ids for the neighbors of the
   //                given site.  The memory for this array
   //                should be allocated and freed by the caller.
   //                The values are filled in by this routine.
   //    arraySize = size of the id array allocated by caller
   //
   //    returns 0 on success, 1 on failure
   //
   int    getIds( int siteId, int* ids, int arraySize );

   //
   // Get the ids of the neighbors for a given site id.
   // Does NOT include the id of the given site in the list.
   //    siteId    = given site id
   //    ids       = list of ids for the neighbors of the
   //                given site.  The memory for this array
   //                should be allocated and freed by the caller.
   //                The values are filled in by this routine.
   //    arraySize = size of the id array allocated by caller.
   //
   //    return 0 on success, 1 on failure
   //
   int    getNbrIds( int siteId, int* ids, int arraySize );
   
   //
   // Return the latitude of the site identified by the id
   //   returns SiteLoc::MISSING_LAT_LON on failure
   //
   float  getLat( int siteId );

   //
   // Return the longitude of the site identified by the id
   //   returns SiteLoc::MISSING_LAT_LON on failure
   //
   float  getLon( int siteId );

   //
   // Return the icao id of the site identified by the id
   //   returns NULL on failure
   //
   string* getIcao( int siteId );

   //
   // Get a map of site ids to indeces from a file.
   // User passes a list of site ids from a file.  This routine
   // will find the ids in the SiteList object in the user
   // supplied list of sites.  It will then create a map of the
   // id's to the indeces in the user supplied list.
   //
   //   siteIndeces  = map to be filled out
   //   fileSiteIds  = user supplied list of sites in a given
   //                 netcdf file
   //   fileNumSites = number of sites in the user supplied list
   //
   //   returns 0 on success, 1 on failure
   //
   int    getIdMap( map< int, int, less<int> >& siteIndeces, 
                    int* fileSiteIds, int fileNumSites );
   
   //
   // Constants
   //
   static const int    MAX_LINE;
   static const int    ICAO_LEN;
   static const int    SHORT_LINE;
   static const int    NBR_DIM_IDEX;
   static const int    SITE_DIM_IDEX;
   static const int    PARAM_DIM_IDEX;
   static const string NEIGHBOR_LIST_NAME;

 private:

   long    numSites;  
   string  errString;
   int    *idArray;
   float   maxDist;
   int     maxNbr;
   
   map< int, SiteLoc*, less<int> > siteLocs; 
   map< int, int, less<int> >      siteIndeces;

   int  processSiteListFile( const char* inputFile );
   int  processNbhdFile( const char* inputFile );
   
};

#endif

