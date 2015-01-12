// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2014 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2014/10/20 13:00:00 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/*
 * Module: SiteLayerTraffic.cc
 *
 * Author: Sue Dettling
 *
 * Date:   12/4/02
 *
 * Description:
 *     
 */

// Include files 

#include "SiteLayerTraffic.hh"

//////////////////////////////////////////////////////////////
//
// Method: SiteLayerTraffic()
//
// Description: Constructor sets default values of data members
//
// Inputs: None
// 
// Outputs: None
//
// Returns: None
//
SiteLayerTraffic::SiteLayerTraffic()
{
  siteId = -1;

  tzCode.clear();
  
  numLayers = -1;
  
  layerMat = NULL;

  layerThickness = NULL;
  
  numTrafficTimes = -1;
  
  trafficVol = NULL;
  
  routeLen = -1;
  
  routeLanes = -1;

  routeTreatTime = -1;

  treatmentStrategy = -1;

  snowPlowThreshold = -1;

  chemType = -1;

  chemForm = -1;

  chemMinRate = -1;

  chemMaxRate = -1;

  chemRateInc = -1;

  pretreatType = -1;

  pretreatForm = -1;

  pretreatMinRate = -1;

  pretreatMaxRate = -1;

  pretreatRateInc = -1;

}

//
// Copy constructor
//
SiteLayerTraffic::SiteLayerTraffic(const SiteLayerTraffic &site)
{
  //
  // Copy single-value variables
  //
  siteId = site.siteId;
  tzCode = site.tzCode;
  numLayers = site.numLayers;
  numTrafficTimes = site.numTrafficTimes;
  routeLen = site.routeLen;
  routeLanes = site.routeLanes;
  routeTreatTime  = site.routeTreatTime;
  treatmentStrategy = site.treatmentStrategy;
  snowPlowThreshold = site.snowPlowThreshold;
  chemType = site.chemType;
  chemForm = site.chemForm;
  chemMinRate = site.chemMinRate;
  chemMaxRate = site.chemMaxRate;
  chemRateInc = site.chemRateInc;
  pretreatType = site.pretreatType;
  pretreatForm = site.pretreatForm;
  pretreatMinRate = site.pretreatMinRate;
  pretreatMaxRate = site.pretreatMaxRate;
  pretreatRateInc = site.pretreatRateInc;

  //
  // Copy arrays
  //
  layerMat =  new int[numLayers];
  layerThickness = new float[numLayers];
  for (int i=0; i<numLayers; i++)
    {
      layerMat[i] = site.layerMat[i];
      layerThickness[i] = site.layerThickness[i];
    }

  trafficVol = new int[numTrafficTimes];
  for (int i=0; i<numTrafficTimes; i++)
    trafficVol[i] = site.trafficVol[i];

}

//
// Assignment operator
//
SiteLayerTraffic& SiteLayerTraffic::operator=(const SiteLayerTraffic &site)
{
  if (this != &site)
    {
      //
      // Copy scalar variables
      //
      siteId = site.siteId;
      tzCode = site.tzCode;
      numLayers = site.numLayers;
      numTrafficTimes = site.numTrafficTimes;
      routeLen = site.routeLen;
      routeLanes = site.routeLanes;
      routeTreatTime  = site.routeTreatTime;
      treatmentStrategy = site.treatmentStrategy;
      snowPlowThreshold = site.snowPlowThreshold;
      chemType = site.chemType;
      chemForm = site.chemForm;
      chemMinRate = site.chemMinRate;
      chemMaxRate = site.chemMaxRate;
      chemRateInc = site.chemRateInc;
      pretreatType = site.pretreatType;
      pretreatForm = site.pretreatForm;
      pretreatMinRate = site.pretreatMinRate;
      pretreatMaxRate = site.pretreatMaxRate;
      pretreatRateInc = site.pretreatRateInc;
      //
      // Copy arrays
      //
      layerMat =  new int[numLayers];
      layerThickness = new float[numLayers];
      for (int i=0; i<numLayers; i++)
	{
	  layerMat[i] = site.layerMat[i];
	  layerThickness[i] = site.layerThickness[i];
	}
      
      trafficVol = new int[numTrafficTimes];
      for (int i=0; i<numTrafficTimes; i++)
	trafficVol[i] = site.trafficVol[i];
      
      return *this;
    }
}

////////////////////////////////////////////////////////////
// 
// Method: ~SiteLayerTraffic()
// 
// Description: Destructor handles memory cleanup.
//
// Input: none
// 
// Output: none
//
// Returns: none
//
SiteLayerTraffic::~SiteLayerTraffic()
{

  //
  // Free memory
  //
  if ( layerMat != NULL)
    delete[] layerMat;

  if ( layerThickness != NULL)
    delete[] layerThickness;
  
  if ( trafficVol != NULL)
    delete[] trafficVol;

}  

/////////////////////////////////////////////////////////////////////////////////////////
// 
// Method: setNumLayers()
//
// Description: Set private member numLayers. Allocate space to hold
//   layer material codes, number of nodes per layer, pointers to arrays node thickenesses
//
// Inputs: int num_layers 
// 
// Outputs: none
//
// Returns: 0..................If no fatal errors were found.
//          -1.................If a fatal processing condition was encountered.
//
// 
int SiteLayerTraffic::setNumLayers( int num_layers) 
{ 
  numLayers = num_layers; 

  //
  // Allocate space to hold layer material codes
  //
  layerMat = new int[numLayers];

  if (layerMat == NULL)
    return(-1);

  //
  // Initialize layerMat
  //
  for (int i = 0; i < numLayers; i++)
    layerMat[i] = -1;

  //
  // Allocate memory for layerThickness
  //
  layerThickness = new float[numLayers]; 

  //
  // Initialize layerThickness 
  //
  for (int i = 0; i < numLayers; i++)
    layerThickness[i] = 0;
  
  return(0);
}

////////////////////////////////////////////////////////////////////////////
// 
// Method: setLayerMat()
//
// Description: Sets the material code for a given layer. 
//
// Inputs: int index.............index of layer.
//         int material .........code for material of layer
//
// Outputs: none
//
// Returns:  0.................If no fatal errors were found.
//          -1.................If a fatal processing condition was encountered.
// 
int SiteLayerTraffic::setLayerMat(int index, int material)
{
   
  //
  // Check that index is within bounds of layerMat array
  //
  if ( numLayers <= index || layerMat == NULL)
    return(-1);
  
  layerMat[index] = material;

  //
  // Allocate space to hold layer thickenss
  //
  if (layerThickness == NULL)
    return (-1);
  else
    layerThickness[index] = 0;

  //
  // Initialize Layer Thickness
  //
  for (int i = 0; i < numLayers; i++)
    layerThickness[index] = 0;

  return(0);
  
} 


/////////////////////////////////////////////////////////////////////////
// 
// Method:   setLayerThickness()
//
// Description: Sets the thickness of a layer 
//
// Inputs: int layer..........index of layer
//         float thickness....thickness of layer
// 
// Outputs: none
//
// Returns:  0.................If no fatal errors were found.
//          -1.................If a fatal processing condition was encountered.
// 
int SiteLayerTraffic::setLayerThickness(int layer, float thickness)
{   
  //
  // Check that layer is less than numLayers  
  //
  if ( numLayers <= layer || layerThickness == NULL )
    return(-1);
 
  layerThickness[layer] = thickness;

  return(0); 
} 

//////////////////////////////////////////////////////////////////////
// 
// Method: setNumTrafficTimes()
//
// Description: Set the private member numTrafficTimes. Allocate space 
//              to hold traffic volume indicators for each time
//
// Inputs: int num_times ......... number of times in 24 hours that traffic
//                                 volume is recorded
// Outputs: none
//
// Returns:  0.................If no fatal errors were found.
//          -1.................If a fatal processing condition was encountered.
// 
int SiteLayerTraffic::setNumTrafficTimes( int num_times) 
{ 
  numTrafficTimes = num_times; 
  
  //
  // Allocate space to hold traffic volume indicators 
  // for specified number of time per day. 
  //
  trafficVol = new int[numTrafficTimes];
  
  if ( trafficVol == NULL )
    return(-1);
  
  //
  // Initialize trafficVol
  //
  for (int i = 0; i < num_times; i++)
    trafficVol[i] = -1;
  

  return(0);
}

//////////////////////////////////////////////////////////////////////
// 
// Method: setTrafficVol()
//
// Description: Set the traffic volume indicator for a given time index 
//
// Inputs: int index.........time indicator
//         int vol ..........volume indicator 
//
// Outputs: none
//
// Returns: none
// 
int SiteLayerTraffic::setTrafficVol(int index, int vol)
{
  
  //
  // Check that index is within bounds of trafficVol array
  //
  if ( numTrafficTimes <= index ||  trafficVol == NULL)
    return(-1);
  
  trafficVol[index] = vol;

  return(0);
  
} 

//////////////////////////////////////////////////////////////////////
// 
// Method: getLayerMat()
//
// Description:  Get layer material indicator for a layer
//
// Inputs: int index.............layer indicator
// 
// Outputs: none
//
// Returns: int .................material code
// 
int SiteLayerTraffic::getLayerMat(int index)
{
  //
  // Check that index is within bounds of layerMat array
  //
  if ( numLayers <= index || layerMat == NULL)
    return(-1);
  else
    return (layerMat[index]);

}


//////////////////////////////////////////////////////////////////////
// 
// Method: getLayerThickness
//
// Description: Given the layer, return the thickness
//
// Inputs: int layer.................layer indicator
//
// Outputs: none
//
// Returns: float .................thickness of layer
// 
float SiteLayerTraffic::getLayerThickness(int layer)
{
  //
  // Check that layer is within proper bounds 
  //
  if ( layerThickness == NULL || numLayers <= layer) 
    return(-1);
  else
    return (layerThickness[layer]);
}

//////////////////////////////////////////////////////////////////////
// 
// Method: getTrafficVol()
//
// Description: Get traffic volume for given time indicator
//
// Inputs: int index..........time indicator
// 
// Outputs: none
//
// Returns: int ..............code for traffic volume.
// 
int SiteLayerTraffic::getTrafficVol(int index)
{
  //
  // Check that index is within bounds of trafficVol array
  //
  if ( numTrafficTimes <= index || trafficVol == NULL)
    return(-1);
  else
    return (trafficVol[index]);
}

//////////////////////////////////////////////////////////////////////
// 
// Method: isOK()
//
// Description: Checks to see if SiteLayerTraffic object has 
//              been completely set
//
// Inputs: none
// 
// Outputs: none
//
// Returns: true .............if object is completely set.
//          false ............if object is not completely set
// 
bool SiteLayerTraffic::isOK()
{
  
  if (siteId == -1)
    return false;

  if (tzCode.empty())
    return false;
  
  if (numLayers == -1)
    return false;
  
  if (layerMat == NULL)
    return false;
  
  if (layerThickness == NULL)
    return false;
  
  if (numTrafficTimes == -1)
    return false;
  
  if (trafficVol == NULL)
    return false;
  
  if (routeLen == -1)
    return false;
  
  if (routeLanes == -1)
    return false;

  if (routeTreatTime == -1)
    return false;

  if (treatmentStrategy == -1)
    return false;

  if (snowPlowThreshold == -1)
    return false;

  if (chemType == -1)
    return false;

  if (chemForm == -1)
    return false;

  if (chemMinRate == -1)
    return false;

  if (chemMaxRate == -1)
    return false;

  if (chemRateInc == -1)
    return false;

  if (pretreatType == -1)
    return false;

  if (pretreatForm == -1)
    return false;

  if (pretreatMinRate == -1)
    return false;

  if (pretreatMaxRate == -1)
    return false;

  if (pretreatRateInc == -1)
    return false;

  return true;

}

