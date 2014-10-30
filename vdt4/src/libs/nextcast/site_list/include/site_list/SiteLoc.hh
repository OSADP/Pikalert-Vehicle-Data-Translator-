// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/////////////////////////////////////////////////////////////////
// SiteLoc:  Container class for information associated with
// a given site 
//
// $Id: SiteLoc.hh,v 1.14 2014/03/06 04:38:33 jaimi Exp $
//
////////////////////////////////////////////////////////////////
#ifndef SITE_LOC_HH
#define SITE_LOC_HH

#include <string>
#include <vector>
#include <map>
using namespace std;

class SiteLoc 
{
 public:


   //
   // Constructor
   //  siteIdent = site id number
   //  latitude  = latitude of site
   //  longitude = longitude of site
   //  altitude  = altitude of site
   //  longNm    = long name
   //  siteIndex = index of site in file
   //  icaoIdent = icao id 
   //  wmoIdent  = wmo id
   //  wmoReg    = wmo region
   //  state     = state name
   //  country   = country name
   //
   SiteLoc( int siteIdent, float latitude, float longitude, 
            float altitude, char* longNm, int siteIndex = -1, 
            char* icaoIdent = NULL, int wmoIdent = 0, int wmoReg = 0, 
            char* state = NULL, char* country = NULL );
   
   ~SiteLoc();
   
   //
   // Add a neighbor to the current site
   //   neighborId   = site id of neighbor
   //   neighborIdex = site index of neighbor
   //
   //   returns 0 on success, 1 on failure
   //
   int     addNeighbor( int neighborId, int neighborIdex = -1 );

   int     getSiteId() const { return siteId; }
   int     getWmoId() const { return wmoId; }
   string& getIcaoId() { return icaoId; }
   float   getLat() const { return lat; }
   float   getLon() const { return lon; }
   float   getAlt() const { return alt; }
   int     getWmoRegion() const { return wmoRegion; }
   string& getLongName() { return longName; }
   string& getStateName() { return stateName; }
   string& getCountryName() { return countryName; }

   //
   // Get the indeces of the neighbors of this site including
   // the current site
   //   indeces   = array of neighbor indeces.  Allocated and
   //               freed by caller, filled in by this routine.
   //   arraySize = size of indeces array allocated by caller.
   //
   //   returns 0 on success, 1 on failure
   //
   int     getIndeces( int* indeces, int arraySize );

   //
   // Get the id numbers of the neighbors of this site including
   // the current site
   //   indeces   = array of neighbor ids.  Allocated and
   //               freed by caller, filled in by this routine.
   //   arraySize = size of id array allocated by caller.
   //
   //   returns 0 on success, 1 on failure
   //
   int     getIds( int* ids, int arraySize );

   //
   // Get the id numbers of the neighbors of this site NOT including
   // the current site
   //   indeces   = array of neighbor ids.  Allocated and
   //               freed by caller, filled in by this routine.
   //   arraySize = size of id array allocated by caller.
   //
   //   returns 0 on success, 1 on failure
   //
   int     getNbrIds( int* ids, int arraySize );

   //
   // Constants
   //
   static const float  MISSING_LAT_LON;
    
 private:

   int    index;
   int    siteId;
   int    wmoId;
   string icaoId;

   float  lat;
   float  lon;
   float  alt;

   int    wmoRegion;
   string longName;
   string stateName;
   string countryName;
   
   vector< int >              neighbors;
   map< int, int, less<int> > neighborIdLookup;
};

#endif







