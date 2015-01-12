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
 *   Module: SiteLayerTraffic.hh
 *
 *   Author: Sue Dettling
 *
 *   Date:   12/4/02
 *
 *   Description: This is the header file for the SiteLayerTraffic class.
 *                A SiteLayerTraffic object holds information about the 
 *                surface layer properties of a given site including
 *                the number of layers, the layer material, the number of
 *                nodes per layer and the thickness of each node. It also
 *                holds information including traffic volume, route length, 
 *                number of traffic lanes, interval between road treatments
 *                and default chemical types.
 *
 *   Changes: Jim Cowie 8/8/05 To add new variables related to default
 *            chemical types and application rates.
 *     
 */

#ifndef SITELAYERTRAFFIC_HH
#define SITELAYERTRAFFIC_HH

#include <string>
#include <stdlib.h>

using namespace std;  // For string

class SiteLayerTraffic {

public:
  
  //
  // Constructor
  //
  SiteLayerTraffic();
  
  // 
  // Destructor
  //
  ~SiteLayerTraffic();

  //
  // Copy
  //
  SiteLayerTraffic(const SiteLayerTraffic &);

  //
  // Assignment
  //
  SiteLayerTraffic & operator=(const SiteLayerTraffic &);

  //
  // "Set" methods
  // 
  void setSiteId( long id ) { siteId = id; }

  void setTzCode(char *tz_code) { tzCode.assign(tz_code); }
  
  int  setNumLayers( int num_layers);
  
  int  setLayerMat(int index, int material);
  
  int setLayerThickness(int layer, float thickness);

  int  setNumTrafficTimes( int num_times);
 
  int  setTrafficVol( int index, int vol);
  
  void setRouteLen( float length) { routeLen = length; }

  void setRouteLanes( int lane_num) { routeLanes = lane_num; }

  void setRouteTreatmentTime( int treat_time) { routeTreatTime = treat_time; }

  void setTreatmentStrategy( int strategy) { treatmentStrategy = strategy; }

  void setSnowPlowThreshold( float threshold) { snowPlowThreshold = threshold; }

  void setChemType( int type) { chemType = type; }

  void setChemForm( int form) { chemForm = form; }

  void setChemMinRate( float rate) { chemMinRate = rate; }

  void setChemMaxRate( float rate) { chemMaxRate = rate; }

  void setChemRateInc( float increment) { chemRateInc = increment; }

  void setPretreatType( int type) { pretreatType = type; }

  void setPretreatForm( int form) { pretreatForm = form; }

  void setPretreatMinRate( float rate) { pretreatMinRate = rate; }

  void setPretreatMaxRate( float rate) { pretreatMaxRate = rate; }

  void setPretreatRateInc( float increment) { pretreatRateInc = increment; }

  //
  // "Get" methods
  //
  long  getSiteId()  { return siteId; }

  string getTzCode() { return tzCode; }

  int   getNumLayers() { return numLayers; }
 
  int   getLayerMat(int index); 
  
  int   getNumNodes(int layer);
  
  float getLayerThickness(int layer);

  int   getNumTrafficTimes()  { return numTrafficTimes; }
  
  int   getTrafficVol(int index);
  
  float getRouteLen()  { return routeLen; }
  
  int   getRouteLanes() { return routeLanes; }

  int   getRouteTreatmentTime() { return routeTreatTime; }

  int   getTreatmentStrategy() { return treatmentStrategy; }

  float getSnowPlowThreshold() { return snowPlowThreshold; }

  int   getChemType() { return chemType; }

  int   getChemForm() { return chemForm; }

  float getChemMinRate() { return chemMinRate; }

  float getChemMaxRate() { return chemMaxRate; }

  float getChemRateInc() { return chemRateInc; }

  int   getPretreatType() { return pretreatType; }

  int   getPretreatForm() { return pretreatForm; }

  float getPretreatMinRate() { return pretreatMinRate; }

  float getPretreatMaxRate() { return pretreatMaxRate; }

  float getPretreatRateInc() { return pretreatRateInc; }

  //
  //  Check that all layer, traffic, info has been set
  //
  bool isOK();

private:

  long siteId;

  //
  // Time zone code
  //
  string tzCode;

  //
  // Number of surface layers
  //
  int numLayers;
  
  //
  // Material codes for each layer
  //
  int *layerMat;
  
  //
  // The thickness of each layer
  //
  float *layerThickness;
  
  //
  // Number of times in 24 hours that traffic volume is recorded
  //
  int numTrafficTimes;
  
  //
  // Traffic volume indicators for each time
  //
  int *trafficVol;
  
  //
  // Length of route
  //
  float routeLen;
  
  //
  // Number of lanes in route
  // 
  int routeLanes;

  //
  // Treat time for route
  // 
  int routeTreatTime;

  //
  // Treatment strategy
  //
  int treatmentStrategy;

  //
  // Snow amount threshold before recommending plowing
  //
  float snowPlowThreshold;

  //
  // Default recommended chemical type
  //
  int chemType;

  //
  // Default recommended chemical form (solid, prewet, liquid)
  //
  int chemForm;

  //
  // Minimum application rate for default chem type
  //
  float chemMinRate;

  //
  // Maximum application rate for default chem type
  //
  float chemMaxRate;

  //
  // Rate increment for default chem type
  //
  float chemRateInc;

  //
  // Default pretreat chemical type
  //
  int pretreatType;

  //
  // Default pretreat chemical form (solid, prewest, liquid)
  //
  int pretreatForm;

  //
  // Minimum application rate for pretreat chem type
  //
  float pretreatMinRate;

  //
  // Maximum application rate for pretreat chem type
  //
  float pretreatMaxRate;

  //
  // Rate increment for pretreat chem type
  //
  float pretreatRateInc;


};

#endif /* SITELAYERTRAFFIC_HH */

