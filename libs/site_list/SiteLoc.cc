// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:34:34 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/////////////////////////////////////////////////////////////////
// SiteLoc:  Container class for information associated with
// a given site 
//
// $Id: SiteLoc.cc,v 1.16 2014/03/06 04:40:05 jaimi Exp $
//
////////////////////////////////////////////////////////////////
#include "SiteLoc.hh"

//
// Constants
//
const float SiteLoc::MISSING_LAT_LON = -9999.0;

SiteLoc::SiteLoc( int siteIdent, float latitude, float longitude, 
                  float altitude, char* longNm, int siteIndex,
                  char* icaoIdent, int wmoIdent, int wmoReg, 
                  char* state, char* country ) 
{
   index       = siteIndex;
   siteId      = siteIdent;
   wmoId       = wmoIdent;
   lat         = latitude;
   lon         = longitude;
   alt         = altitude;
   wmoRegion   = wmoReg;

   if( longNm )
      longName = longNm;
   else
      longName = "";
   
   if( icaoIdent )
      icaoId = icaoIdent;
   else
      icaoId = "";
   
   if( state ) 
      stateName = state;
   else
      stateName = "";
   
   if( country ) 
      countryName = country;
   else
      countryName = "";
   
}

SiteLoc::~SiteLoc() 
{
   neighbors.erase( neighbors.begin(), neighbors.end() );
   neighborIdLookup.erase( neighborIdLookup.begin(), neighborIdLookup.end() );
}

int
SiteLoc::addNeighbor( int neighborId, int neighborIndex ) 
{
   if( neighborId != siteId ) {
      
      map< int, int, less<int> >::iterator it = 
	neighborIdLookup.find( neighborId );
      
      if( it == neighborIdLookup.end() ) {

	 neighbors.push_back( neighborId );
	 neighborIdLookup[neighborId] = neighborIndex;
         return( 0 );
      }
   }

   return( 1 );
}

int
SiteLoc::getIndeces( int* indeces, int arraySize ) 
{
   int numIndeces = neighbors.size() + 1;

   indeces[0] = index;
   
   int i = 1;
   vector< int >::iterator it;
   for( it = neighbors.begin();
        it != neighbors.end() && i < arraySize; it++ ) {
      int id = (*it);
      indeces[i] = neighborIdLookup[id];
      i++;
   }
   
   for( int i = numIndeces; i < arraySize; i++ ) {
      indeces[i] = -1;
   }
   
   return( min( numIndeces, arraySize ) );
}

int
SiteLoc::getIds( int* ids, int arraySize ) 
{
   int numIds = neighbors.size() + 1;

   ids[0] = siteId;
   
   int i = 1;
   vector< int >::iterator it;
   for( it = neighbors.begin();
        it != neighbors.end() && i < arraySize; it++ ) {
      ids[i] = (*it);
      i++;
   }
   
   for( int i = numIds; i < arraySize; i++ ) {
      ids[i] = -1;
   }
   
   return( min( numIds, arraySize ) );
}

int
SiteLoc::getNbrIds( int* ids, int arraySize ) 
{
   int numIds = neighbors.size();
   
   vector< int >::iterator it;
   
   int i = 0;
   for( it = neighbors.begin(); 
        i < arraySize && it != neighbors.end(); it++, i++ ) {
      ids[i] = (*it);
   }
   
   for( int i = numIds; i < arraySize; i++ ) {
      ids[i] = -1;
   }
   
   return( min( numIds, arraySize ) );
}


