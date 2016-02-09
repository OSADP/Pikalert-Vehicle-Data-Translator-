// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1992 - 2015 
// ** University Corporation for Atmospheric Research(UCAR) 
// ** National Center for Atmospheric Research(NCAR) 
// ** Research Applications Laboratory(RAL) 
// ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA 
// ** See LICENCE.TXT if applicable for licence details 
// ** 2015/02/02 20:23:05 
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
const int MAX_OBS_HISTORY = 48;
const int NUM_OBS_HRS = 12;
const int OBS_MISSING_VALUE = -9999;
const int PRECIP_ROAD_COND_MISSING_VALUE = -333;
const int METRO_MISSING_VALUE = 9999;
const float METRO_MAX_SURFACE_TEMP =  80.0;
const float METRO_MIN_SURFACE_TEMP = -40.0;

struct obs {
  float T;               // air Temp (degC)
  float wspd;            // wind speed (m/s)
  float dewpt;           // dew point (degC)
  int Precip;            // Precip (0 or 1)
  int road_condition;    // Road condition (codes need documention)
  float road_T;           // Pavement Temperature
  float bridge_T;        // Pavement Temperature for bridges 
  float subsurface_T;     // Subsurface (40cm) temperature
};
typedef struct obs Obs;

struct obs_time_series {
  Obs obs[MAX_OBS_HISTORY];
};
typedef struct obs_time_series ObsTS;

enum METRo_road_cond_values {DRY = 1, WET, ICE_SNOW, WATER_SNOW, DEW, MELTING_SNOW, FROST, ICING_RAIN, MISSING=METRO_MISSING_VALUE};
