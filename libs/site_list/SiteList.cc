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
// $Id: SiteList.cc,v 1.25 2014/03/06 04:40:05 jaimi Exp $
////////////////////////////////////////////////////
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <nxncf/nxncf.hh>
#include "SiteList.hh"
#include "SiteLoc.hh"

//
// Constants
//
const int    SiteList::MAX_LINE           = 256;
const int    SiteList::ICAO_LEN           = 10;
const int    SiteList::SHORT_LINE         = 10;
const int    SiteList::SITE_DIM_IDEX      = 0;
const int    SiteList::NBR_DIM_IDEX       = 1;
const int    SiteList::PARAM_DIM_IDEX     = 2;
const string SiteList::NEIGHBOR_LIST_NAME = "nbr_list";


SiteList::SiteList()
{ 
  numSites  = 0;
  errString = "";
  idArray   = NULL;
  maxDist   = 0.0;
  maxNbr    = 0;
}

SiteList::~SiteList()
{
   delete[] idArray;
   
   map< int, SiteLoc*, less<int> >::iterator siteIt;
   for( siteIt = siteLocs.begin(); siteIt != siteLocs.end(); siteIt++) {
      delete( (*siteIt).second );
   }
   siteLocs.erase( siteLocs.begin(), siteLocs.end() );
}

int
SiteList::init( const char* siteListFile, const char* nbhdFile, 
                float maxNbrDist, int maxNumNbr ) 
{
   //
   // Process the site list file
   //
   if( processSiteListFile( siteListFile ) != 0 ) {
      return( 1 );
   }

   //
   // Make id array
   //
   idArray = new int[numSites];

   map< int, int, less<int> >::iterator it = siteIndeces.begin();
   for( int i = 0; i < numSites; i++, it++ ) {
      idArray[i] = (*it).first;
   }

   //
   // Set the maximum distance for the neighbors
   //
   maxDist = maxNbrDist;

   //
   // Set the maximum number of neighbors
   //
   maxNbr = maxNumNbr;
   
   //
   // Process the neighborhood file
   //
   if( processNbhdFile( nbhdFile ) != 0 ) {
      return( 1 );
   }
   

   return( 0 );
}

int
SiteList::processSiteListFile( const char* inputFile ) 
{
   int      count = 0;
   char     inLine[MAX_LINE];
   char     temp[ICAO_LEN];

   int      siteId;
   int      wmoId;
   int      wmoRegion;
   float    lat;
   float    lon;
   float    alt;
   char     icaoId[ICAO_LEN];
   char     longName[MAX_LINE];
   char     stateName[MAX_LINE];
   char     countryName[MAX_LINE];

   int      siteIndex = 0;

   SiteLoc *loc = NULL;

   //
   // Open the site list file
   //
   FILE *siteFile = fopen( inputFile, "r" );
   if( !siteFile ) {
      errString = "Unable to open site list file.\n";
      return( 1 );
   }
   
   //
   // Loop through the lines in the file
   //
   while( fgets( inLine, MAX_LINE, siteFile ) != NULL ) {

      //
      // Count how many lines we have read
      //
      count++;

      //
      // Skip this line if it is too short or is a comment
      //
      if( strlen(inLine) <= (unsigned) SHORT_LINE || inLine[0] == '#') {
         continue;
      }

      //
      // Interpret the line from the file.  Assume the icao standard
      // first.
      //
      int ret = sscanf( inLine,"%d;%d;%10[^;];%f;%f;%f;%d;%[^;];%[^;];%[^\n]",
     		        &(siteId), &(wmoId), temp, &(lat),
		        &(lon), &(alt), &(wmoRegion), 
                        longName, stateName, countryName );

      if( ret == 10 ) {

         temp[ICAO_LEN-1] = '\0';
     
         //
         // Get rid of white space at start of id
         //
         int idex = strspn( temp, " " );
         if( idex >= ICAO_LEN ) {
            errString  = "Problem reading icao id from line ";
            errString += count;
            errString += ".\n";
	    fclose(siteFile);
            return( 1 );
         }

         //
         // Copy the icao string in
         //
         strcpy( icaoId, &temp[idex] );

         //
         // Create a new site location object
         //
         loc = new SiteLoc( siteId, lat, lon, alt, longName, siteIndex,
                            icaoId, wmoId, wmoRegion, stateName, 
                            countryName );

      }
         

      //
      // If that didn't work, try the old style format
      //
      else {
         
         ret = sscanf( inLine, "%d%f%f%f%*[\t]%[^\n]", &(siteId),
                       &(lat), &(lon), &(alt), longName );
        
         if( ret != 5 ) {
            errString  = "Problem reading line ";
            errString += count;
            errString += ".\n";
	    fclose(siteFile);
            return( 1 );
         }

         loc = new SiteLoc( siteId, lat, lon, alt, longName, siteIndex );
      }     

      //
      // Add this location to the lists
      //
      siteLocs[siteId]    = loc;
      siteIndeces[siteId] = siteIndex;
      siteIndex++;
      
   }
   
   //
   // Set the number of sites
   //
   numSites = siteLocs.size();
  
   //
   // Return with failure if there are no sites
   //
   if( numSites == 0 ) {
      errString = "No sites in file.\n";
      fclose(siteFile);
      return( 1 );
   }

   fclose(siteFile);
   return( 0 );
}

int
SiteList::processNbhdFile( const char* inputFile ) 
{
   //
   // If input file name is null, we're done
   //
   if( inputFile == NULL ) {
      return( 0 );
   }
   
   //
   // Open the netCDF file
   //
   NcFile *ncInput = new NcFile( inputFile, NcFile::ReadOnly );
   
   if( !ncInput->is_valid() ) {
      errString  = "Open for ";
      errString += inputFile;
      errString += ".\n";
      return( 1 );
   }

   //
   // Get the site list from the file
   //
   NcVar *siteListVar = ncInput->get_var( SITE_LIST_NAME );
   
   if( !siteListVar->is_valid() ) {
      errString  = SITE_LIST_NAME;
      errString += " is not a valid variable in the neighborhood file.\n";
      return( 1 );
   }

   long *siteEdges   = siteListVar->edges();
   int   numSiteVals = siteListVar->num_vals();
   int  *siteList    = new int[numSiteVals];
   
   if( !siteListVar->get( siteList, siteEdges ) ) {
      errString  = "Couldn't get ";
      errString += SITE_LIST_NAME;
      errString += " from neighborhood file.\n";
      return( 1 );
   }

   //
   // Get the neighbor list
   //
   NcVar *nbrListVar = ncInput->get_var( NEIGHBOR_LIST_NAME.c_str() );
   
   if( !nbrListVar->is_valid() ) {
      errString  = NEIGHBOR_LIST_NAME;
      errString += " is not a valid variable in the neighborhood file.\n";
      return( 1 );
   }
   
   long   *nbrEdges   = nbrListVar->edges();
   int     numNbrVals = nbrListVar->num_vals();
   double *nbrList    = new double[numNbrVals];

   if( !nbrListVar->get( nbrList, nbrEdges ) ) {
      errString  = "Couldn't get ";
      errString += NEIGHBOR_LIST_NAME;
      errString += " from neighborhood file.\n";
      return( 1 );
   }
   
   //
   // Get dimensions of neighbor list
   //
   int nbrDim   = nbrEdges[NBR_DIM_IDEX];
   int paramDim = nbrEdges[PARAM_DIM_IDEX];
   
   //
   // Add the neighbors to the given site location
   //
   for( int i = 0; i < numSiteVals; i++ ) {
      
      //
      // Try to find this site in our list
      //
      map< int, SiteLoc*, less<int> >::iterator it = 
         siteLocs.find( siteList[i] );
      
      //
      // Process this site if it is in our list.  
      // Otherwise do nothing.
      //
      if( it != siteLocs.end() ) {
         
         //
         // Get a pointer to the corresponding site location
         //
         SiteLoc *currentSite = (*it).second;

         //
         // Initialize the number of neighbors for this site
         //
         int numAdded = 0;
         
         //
         // Add each neighbor to our list of neighbors for this
         // site
         //
         for( int j = 0; j < nbrDim; j++ ) {

            //
            // Calculate the index into the neighbor array from 
            // the netCDF file.  Get the distance for this neighbor.
            //   Note that it is assumed that the distance will
            //   follow the neighbor id in the neighbor list.
            //
            int   nbrIdex = i*paramDim*nbrDim + j*paramDim;
            int   nbrId   = (int) nbrList[nbrIdex];
            float nbrDist = nbrList[nbrIdex+1];

            if( nbrId == NC_FILL_DOUBLE ) {
               continue;
            }

            //
            // Skip this neighbor if it is too far away
            //
            if( nbrDist > maxDist ) {
               continue;
            }

            //
            // Make sure we know this site
            //
            map< int, int, less<int> >::iterator it = 
               siteIndeces.find( nbrId );
            if( it == siteIndeces.end() ) {
               continue;
            }
            
            //
            // Add the neighbor
            //
            if( currentSite->addNeighbor( nbrId, 
                                          siteIndeces[nbrId] ) == 0 )

               numAdded++;

            //
            // If we've reached our max, quit
            //
            if( numAdded >= maxNbr ) {
               break;
            }
            
         }
      }
   }
   
   return( 0 );
}

int
SiteList::getIndeces( int siteIndex, int* indeces, int arraySize ) 
{
   if( siteIndex < 0 || siteIndex > numSites ) {
      errString  = "Site index ";
      errString += siteIndex;
      errString += " out of range";
      return( 0 );
   }
   
   int siteId = idArray[siteIndex];
   
   return( siteLocs[siteId]->getIndeces( indeces, arraySize ) );
}

int
SiteList::getIds( int siteId, int* ids, int arraySize ) 
{
   if( siteId < 0 ) {
      errString  = "Invalid site id";
      return( 0 );
   }

   map< int, SiteLoc*, less<int> >::iterator it =
      siteLocs.find( siteId );

   if( it != siteLocs.end() )
      return( siteLocs[siteId]->getIds( ids, arraySize ) );

   return( 0 );
   
}

int
SiteList::getNbrIds( int siteId, int* ids, int arraySize ) 
{
   if( siteId < 0 ) {
      errString = "Invalid site id";
      return( 0 );
   }
   
   map< int, SiteLoc*, less<int> >::iterator it = 
      siteLocs.find( siteId );
   
   if( it != siteLocs.end() ) {
      return( (*it).second->getNbrIds( ids, arraySize ) );
   }
   
   return( 0 );
}

float
SiteList::getLat( int siteId ) 
{
   if( siteId < 0 ) {
      errString = "Invalid site id";
      return( SiteLoc::MISSING_LAT_LON );
   }
   
   map< int, SiteLoc*, less<int> >::iterator it =
      siteLocs.find( siteId );
   
   if( it != siteLocs.end() ) {
      return( (*it).second->getLat() );
   }
   
   return( SiteLoc::MISSING_LAT_LON );
}

float
SiteList::getLon( int siteId ) 
{
  if( siteId < 0 ) {
      errString = "Invalid site id";
      return( SiteLoc::MISSING_LAT_LON );
   }
   
   map< int, SiteLoc*, less<int> >::iterator it =
      siteLocs.find( siteId );
   
   if( it != siteLocs.end() ) {
      return( (*it).second->getLon() );
   }
   
   return( SiteLoc::MISSING_LAT_LON ); 
}

string*
SiteList::getIcao( int siteId ) 
{  
   if( siteId < 0 ) 
   {
      errString = "Invalid site id";
      return( NULL );
   }
   
   map< int, SiteLoc*, less<int> >::iterator it = 
      siteLocs.find( siteId );
   
   if( it != siteLocs.end() ) 
   {
      return( &((*it).second->getIcaoId()) );
   }
   
   return( NULL );
}


int
SiteList::getIdMap( map< int, int, less<int> >& siteIndeces, 
                    int* fileSiteIds, int fileNumSites ) 
{
   
   //
   // Set up the map of site ids to indeces
   //   Note that this algorithm assumes that the id's are coming
   //   in order, so there are checks below to make sure this is so
   //
   int j = 0;

   for( int i = 0; i < numSites; i++ ) {

      //
      // Make sure the id's are in order
      //
      if( i < numSites - 1 && idArray[i] >= idArray[i+1] ) {
         return( 1 );
      }

      int currentSiteId = idArray[i];

      siteIndeces[currentSiteId] = -1;
      
      while( fileSiteIds[j] < currentSiteId && j < fileNumSites ) {

         //
         // Make sure the ids from the file are in order
         //
         if( j < fileNumSites - 1 && fileSiteIds[j] >= fileSiteIds[j+1] ) {
            return( 1 );
         }
         j++;
      }

      if( j < fileNumSites && fileSiteIds[j] == currentSiteId ) {
         siteIndeces[currentSiteId] = j;
      }
   }

   return( 0 );
}





